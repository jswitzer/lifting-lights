#!/bin/bash

cd "$(dirname ${BASH_SOURCE[0]})"/..

. .venv/bin/activate
python server.py server
