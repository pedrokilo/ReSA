import re, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import main, analyzer as inherited_analyzer
import probe_case
import json
def strict_analyzer(expected):
    base=inherited_analyzer(expected)
    def analyze(path):
        result=base(path)
        if result is None: return None
        for line in path.read_text().splitlines():
            row=json.loads(line)
            if row.get("event")!="comparison": continue
            for side in ("original","candidate"):
                value=row.get(side)
                if not isinstance(value,dict) or value.get("switches") is not True or "trace" not in value or "ret" not in value:
                    raise RuntimeError("Registro lateral incompleto: "+str(row.get("pair")))
        return result
    return analyze
probe_case.analyzer=strict_analyzer
src = (Path(__file__).parent/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED = {f'{m[0]}::{m[1]}': int(m[2]) for m in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}', src) if m[0]=='CModelInfo'}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'cppmodelinforegression', 'Regresión CModelInfo tras /Zi con PCH', EXPECTED, timeout=1500))
