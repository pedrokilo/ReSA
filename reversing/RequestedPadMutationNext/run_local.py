"""Envoltorio (REPARAR-REQUESTED): `bench_here.sh` invoca run_local.py; este banco usa el nombre historico run_local_six.py."""
import runpy
from pathlib import Path
runpy.run_path(str(Path(__file__).resolve().with_name('run_local_six.py')), run_name='__main__')
