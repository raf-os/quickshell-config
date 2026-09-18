#!/bin/bash

echo 'Running generator...'

cd ..
uv run -m plugin.src.NightShell.configs.generator.generator
cd scripts
