#!/bin/bash

cd "$(dirname ${BASH_SOURCE[0]})"/..

if [[ -d .venv ]]; then
  exit 0
fi

pip install virtualenv
rm -rf .venv
python -m virtualenv .venv
. .venv/bin/activate
pip install -r requirements.txt
