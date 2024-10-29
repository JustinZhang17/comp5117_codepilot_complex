import os
import uvicorn
import textwrap
import json
from hashlib import sha512
from dotenv import load_dotenv
from fastapi import FastAPI, status, HTTPException, Request, Depends
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from fastapi.security import APIKeyHeader
from tempfile import NamedTemporaryFile

# LLMs
from langchain_openai import OpenAI
from langchain_core.prompts import PromptTemplate

load_dotenv()

models = ["gpt-3.5-turbo-instruct"]

header_scheme = APIKeyHeader(name="x-key")
os.environ["OPENAI_API_KEY"] = os.getenv("OPENAI_API_KEY", "")
API_KEY_HASH = os.getenv("API_KEY_HASH")

app = FastAPI()

# p: dict[str, str | list[dict[str, str]]] = {
#     "phase": "one",
#     "model": "gpt",
#     "examples": [
#         {
#             "code": """
#                 num1 = 5
#                 num2 = 7
#                 result = num1 * num2
#                 print("The product of {} and {} is {}".format(num1, num2, result))
#                 """,
#             "plan": """
#                 Step 1 : Variable Initialization: Set num1 to 5. Set num2 to 7. "num1 = 5" and "num2 = 7" will be executed.
#                 Step 2: Operation: Multiply num1 and num2 and Store the result in the variable result. "result = num1 * num2 " will be executed.
#                 Step 3: Display Result: Use the print function to display a formatted string. "print("The product of {} and {} is {}".format(num1, num2, result))" will be executed
#                 """,
#             "coverage": """
#                     > num1 = 5
#                     > num2 = 7
#                     > result = num1 * num2
#                     > print("The product of {} and {} is {}".format(num1, num2, result))
#                     """,
#         }
#     ],
#     "code": """
#         number = 15
#         if number % 2 == 0:
#             print("{} is an even number.".format(number))
#         else:
#             print("{} is an odd number.".format(number))
#         """,
# }

app.mount("/public", StaticFiles(directory="src/public"), name="public")


@app.get("/", status_code=status.HTTP_200_OK, response_class=FileResponse)
async def Home():
    return FileResponse("src/views/index.html")


@app.post("/onephase", status_code=status.HTTP_200_OK)
async def Onephase(p: Request) -> FileResponse:
    # Parse JSON from the request body.
    content = await p.body()
    try:
        # Preprocess the JSON string
        preprocessed_json = json.dumps(json.loads(content.decode()))
        # Parse the preprocessed JSON
        p = json.loads(preprocessed_json)
    except json.JSONDecodeError:
        raise HTTPException(status_code=400, detail="Unable to parse body")

    if (
        sha512(p["api-key"].encode("utf-8")).hexdigest() != API_KEY_HASH
        or p["api-key"] == ""
    ):
        raise HTTPException(status_code=401, detail="Unauthorized")

    # Parse Template
    path: str = os.path.join(os.getcwd(), "src", "templates", "onephase.txt")
    template: str = open(path, "r").read()
    prompt_template = PromptTemplate.from_template(template)
    text = prompt_template.format(
        examples=build_examples(p["examples"]),
        code=textwrap.dedent(p["code"]),
    )

    if not (p["model"] in models):
        raise HTTPException(status_code=400, detail="Unknown LLM")

    # Selecting LLM (Temperature 0.1 was used since code coverage is a factual task that requires less creativity)
    llm = OpenAI(model=p["model"], temperature=0.1, max_tokens=-1)

    response = llm.invoke(text)

    # Create a temporary file
    with NamedTemporaryFile(mode="w+", delete=False) as temp_file:
        # Write the output to the temporary file
        temp_file.write("Input:\n\n" + text + "\n\n")
        temp_file.write("Output:\n\n" + response)

        # Get the path of the temporary file
        file_path = temp_file.name

    # Return the temporary file as a FileResponse
    return FileResponse(path=file_path, media_type="text/plain")


@app.post("/twophase", status_code=status.HTTP_200_OK)
async def Twophase(p: Request) -> FileResponse:
    # Parse JSON from the request body.
    content = await p.body()
    try:
        # Preprocess the JSON string
        preprocessed_json = json.dumps(json.loads(content.decode()))
        # Parse the preprocessed JSON
        p = json.loads(preprocessed_json)
    except json.JSONDecodeError:
        raise HTTPException(status_code=400, detail="Unable to parse body")

    if (
        sha512(p["api-key"].encode("utf-8")).hexdigest() != API_KEY_HASH
        or p["api-key"] == ""
    ):
        raise HTTPException(status_code=401, detail="Unauthorized")

    # Parse Template
    path: str = os.path.join(os.getcwd(), "src", "templates", "twophase_step1.txt")
    template: str = open(path, "r").read()
    prompt_template: PromptTemplate = PromptTemplate.from_template(template)
    text: str = prompt_template.format(
        examples=build_examples(p["examples"]),
        code=textwrap.dedent(p["code"]),
    )

    if not (p["model"] in models):
        raise HTTPException(status_code=400, detail="Unknown LLM")

    # Selecting LLM (Temperature 0.1 was used since code coverage is a factual task that requires less creativity)
    llm = OpenAI(model=p["model"], temperature=0.1, max_tokens=-1)

    plan = llm.invoke(text)

    # Step 2
    path: str = os.path.join(os.getcwd(), "src", "templates", "twophase_step2.txt")
    template: str = open(path, "r").read()
    prompt_template: PromptTemplate = PromptTemplate.from_template(template)
    text_coverage: str = prompt_template.format(
        examples=build_examples(p["examples"]),
        code=textwrap.dedent(p["code"]),
        plan=plan,
    )

    response = llm.invoke(text_coverage)

    # Create a temporary file
    with NamedTemporaryFile(mode="w+", delete=False) as temp_file:
        # Write the output to the temporary file
        temp_file.write("Input 1:\n\n" + text + "\n\n")
        temp_file.write("Output 1:\n\n" + plan + "\n\n")
        temp_file.write("Input 2:\n\n" + text_coverage + "\n\n")
        temp_file.write("Output 2:\n\n" + response)

        # Get the path of the temporary file
        file_path = temp_file.name

    # Return the temporary file as a FileResponse
    return FileResponse(path=file_path, media_type="text/plain")


def build_examples(examples: list) -> str:
    out: str = ""

    if len(examples) == 1:
        out += textwrap.dedent(
            """
            DISCLAIMER: Lines that are not executed are to be denoted with a SINGLE '!' whereas lines that are executed are to be denoted with a single '>'
            Below is an example of the process you need to follow to predict the code coverage of a given code snippet and its plan.
            """
        )

    if len(examples) > 1:
        out += textwrap.dedent(
            """
            DISCLAIMER: Lines that are not executed are to be denoted with a SINGLE '!' whereas lines that are executed are to be denoted with a single '>'
            Below are a couple examples of the process you need to follow to predict the code coverage of a given code snippet and its plan.
            """
        )

    for i, ex in enumerate(examples):
        out += textwrap.dedent(
            """
            Example {num}

            Given CODE SNIPPET: 
            {code}

            Given PLAN: 
            {plan}

            So the CODE COVERAGE for the given code snippet will be: 
            {coverage}
            """
        ).format(
            num=i + 1,
            code=textwrap.dedent(ex["code"]),
            plan=textwrap.dedent(ex["plan"]),
            coverage=textwrap.dedent(ex["coverage"]),
        )

    return out


def start():
    """Launched with `poetry run start` at root level"""
    uvicorn.run("src.main:app", host="0.0.0.0", port=8000, reload=True)
