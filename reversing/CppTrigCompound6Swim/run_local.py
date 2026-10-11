import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from runtime_runner import run_case
from audit_nested import analyze
if __name__=='__main__':
    here=Path(__file__).parent
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])),indent=2));sys.exit(0)
    manifest=json.loads((here/'artifacts/build-manifest.json').read_text())
    sys.exit(run_case('cpptrigcompound6swim',manifest['binaries']['asi'],analyze,'CppTrigCompound6Swim',timeout=1500))
