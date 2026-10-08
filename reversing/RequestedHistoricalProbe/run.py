import json
import runpy
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / 'reversing'))
import runtime_runner as runner

bank = Path(sys.argv[1]).resolve()
mode = sys.argv[2]
selection = runner.GAME / 'historical-hooks.txt'
selection_hash = runner.digest(selection)
original_popen = runner.subprocess.Popen

def launch(args, *a, **kw):
    if isinstance(args, list) and str(args[0]).lower().endswith('gta_sa.exe'):
        args = [*args, '--historical-hooks=' + str(selection)]
        (HERE / 'last-launch.json').write_text(json.dumps(dict(
            args=args, selection_sha256=selection_hash,
            wrapper_sha256=runner.digest(Path(__file__))), indent=2))
    return original_popen(args, *a, **kw)

runner.subprocess.Popen = launch
sys.argv = [str(bank), mode]
runpy.run_path(str(bank), run_name='__main__')
