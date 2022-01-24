"""
# Setup & run
poetry install

# Test & Usage
poetry run uvicorn main:app --reload --port 5555
./sapi/cli/php sample.php

# Clean before push
poetry run black .
"""

import logging
from logging.config import dictConfig
from fastapi import FastAPI, Request
import json


app = FastAPI(
    docs_url=None,
    redoc_url=None,
    openapi_url=None,
    swagger_ui_oauth2_redirect_url=None,
    debug=True,
)  # Disable docs endpoints

common_log_level = logging.INFO

# Logs setup
logging_config = dict(
    version=1,
    formatters={
        "f": {
            "format": "%(asctime)s.%(msecs)03d %(name)-4s %(levelname)-8s %(message)s"
        }
    },
    handlers={
        "h1": {
            "class": "logging.StreamHandler",
            "formatter": "f",
            "level": common_log_level,
            "stream": "ext://sys.stdout",
        },
        "h2": {
            "class": "logging.FileHandler",
            "formatter": "f",
            "level": common_log_level,
            "filename": "internalog.log",
        },
    },
    root={
        "handlers": ["h1", "h2"],
        "level": common_log_level,
    },
)
dictConfig(logging_config)
logger = logging.getLogger("internalog")
logger.info("Log ingester is UP")


@app.get("/")
def index():
    return {"api-status": "up"}


@app.post("/")
async def log_json(request: Request):
    log_line = await request.json()
    logger.debug(f"log_line: {log_line}")
    func = log_line.get("name", "fail-name")
    logger.debug(f"func: {func}")
    args = log_line.get("parameters", "fail-parameters")
    args = json.dumps(args).strip("[]")
    logger.debug(f"args: {args}")
    # import ipdb;ipdb.set_trace()
    log_clean = f"{func}({args})"
    logger.info(log_clean)
    return {"status": "ok"}
