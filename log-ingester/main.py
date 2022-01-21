# poetry install
# poetry run uvicorn main:app --reload --port 5555

from fastapi import FastAPI, Request
import json

LOG_FILE = "func-calls.log"

app = FastAPI(
    docs_url=None, redoc_url=None, openapi_url=None, swagger_ui_oauth2_redirect_url=None
)  # Disable docs endpoints


@app.get("/")
def index():
    return {"api-status": "up"}


@app.post("/")
async def log_json(request: Request):
    log_line = await request.json()
    # print(log_line)
    func = log_line.get('name', 'fail-name')
    # print("func", func)
    args = log_line.get('parameters', 'fail-parameters')
    args = ", ".join([str(arg) for arg in args])
    # print("args", args)
    log_clean = f"{func}({args})"
    print(log_clean)
    with open(LOG_FILE, "a+") as f:
        f.write(log_clean + "\n")
    # import ipdb;ipdb.set_trace()
    return {"status": "ok"}
