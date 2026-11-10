"""Envoltorio (REPARAR-REQUESTED): `bench_here.sh` invoca build_probe.py; este banco usa el nombre historico build_probe_six.py."""
import runpy
from pathlib import Path
runpy.run_path(str(Path(__file__).resolve().with_name('build_probe_six.py')), run_name='__main__')
