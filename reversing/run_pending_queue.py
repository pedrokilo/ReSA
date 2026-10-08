"""Cola serial de sondas: nunca compila ni ejecuta dos bancos a la vez."""
import json
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
env = dict(os.environ, CL='/MP6')
results = []
build_failed = False
for name in sys.argv[1:]:
    here = ROOT / 'reversing' / name
    print(f'BUILD {name}', flush=True)
    built = subprocess.run([sys.executable, str(here / 'build_probe.py')], cwd=ROOT, env=env)
    if built.returncode:
        print(f'BUILD_FAILED {name}', flush=True)
        build_failed = True
        break
    print(f'RUN {name}', flush=True)
    ran = subprocess.run([sys.executable, str(here / 'run_local.py')], cwd=ROOT, env=env)
    results.append(dict(bank=name, passed=ran.returncode == 0))
    print('RESULT ' + json.dumps(results[-1]), flush=True)
print('QUEUE_RESULT ' + json.dumps(results), flush=True)
sys.exit(1 if build_failed else 0)
