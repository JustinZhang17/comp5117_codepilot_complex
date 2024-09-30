import uvicorn
from fastapi import FastAPI

app = FastAPI()


@app.get("/hello")
async def Hello():
    return "World"


def start():
    """Launched with `poetry run start` at root level"""
    uvicorn.run("src.main:app", host="0.0.0.0", port=8000, reload=True)
