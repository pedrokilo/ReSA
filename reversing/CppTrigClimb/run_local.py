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
        rows=[json.loads(line) for line in path.read_text().splitlines()]
        starts=[row for row in rows if row.get("event")=="begin"]
        if len(starts)!=1 or starts[0].get("world_isolation")!={"regular_sectors":14400,"repeat_sectors":256,"fixture_sectors":9,"heads_only":True}:
            raise RuntimeError("Aislamiento completo del mundo no demostrado")
        for line in path.read_text().splitlines():
            row=json.loads(line)
            if row.get("event")!="comparison": continue
            for side in ("original","candidate"):
                value=row.get(side)
                if not isinstance(value,dict) or value.get("switches") is not True or "trace" not in value or "ret" not in value or value.get("clean") is not True:
                    raise RuntimeError("Registro lateral incompleto: "+str(row.get("pair")))
                for call in value["trace"]:
                    if call[0]=="get_bound_centre" and (len(call)!=2 or call[1] not in {"ent"+str(i)+"+0" for i in range(6)}):
                        raise RuntimeError("Entidad externa alcanzada: "+str(row.get("pair")))
        return result
    return analyze
probe_case.analyzer=strict_analyzer
src = (Path(__file__).parent/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED = {f'{m[0]}::{m[1]}': int(m[2]) for m in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}', src) if m[0]+"::"+m[1] in ['CTaskSimpleClimb::ProcessPed', 'CTaskSimpleClimb::TestForClimb', 'CTaskSimpleClimb::TestForStandUp', 'CTaskSimpleClimb::TestForVault', 'CTaskSimpleClimb::GetCameraStickModifier', 'CTaskSimpleClimb::GetCameraTargetPos']}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'cpptrigclimb', 'CppTrigClimb', EXPECTED, timeout=1500))
