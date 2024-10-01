# import os
from dotenv import load_dotenv
from fastapi import FastAPI
import uvicorn

load_dotenv()

app = FastAPI()


@app.get("/hello")
async def Hello():
    return "World"


def start():
    """Launched with `poetry run start` at root level"""
    uvicorn.run("src.main:app", host="0.0.0.0", port=8000, reload=True)


# API_KEY = os.getenv("LANGCHAIN_API_KEY")
# TRACING_V2 = os.getenv("LANGCHAIN_TRACING_V2")
#
# print(f"API Key: {API_KEY}")
# print(f"Tracing V2: {TRACING_V2}")
