"""Ejecuta el analizador de run_local.py sobre un registro generado SIN el juego por livetest.exe (mismo nucleo Core.hpp, original = bytes del EXE reubicados).
Uso: python -I run_local_check.py live_test.jsonl   (las direcciones de colaboradores del EXE de la prueba son ficticias: se adaptan en el analizador)"""
import json
import sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
import run_local
path = Path(sys.argv[1])
begin = [json.loads(l) for l in path.read_text(encoding='utf-8').splitlines() if '"event":"begin"' in l][0]
run_local.EXE_COLLABORATORS = begin['collaborators']['exe']
print(json.dumps(run_local.analyze(path), indent=1))
