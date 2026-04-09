#!/bin/bash
#./scripts/start.sh

python -m venv .venv 
source .venv/bin/activate 

pip install -r requirements.txt 

python plot_tiempos.py 
python plot_speedup.py