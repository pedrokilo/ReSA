import re, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import main, analyzer
src = (Path(__file__).parent/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED = {f'{m[0]}::{m[1]}': int(m[2]) for m in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}', src)}
_common = analyzer(EXPECTED)
def analyze(path):
    path = Path(path)
    if not path.exists(): return None
    summary = _common(path)
    if summary is None: return None
    rows = [__import__('json').loads(s) for s in path.read_text().splitlines()]
    ph = [r for r in rows if r['event'] == 'partner_hook']
    # La base 0x681F20 (hook Virtual) debe haberse resuelto (cuerpo en la DLL) y restaurado al final.
    if len(ph) != 1 or not ph[0].get('body') or ph[0]['body'] < 0x1000000: raise RuntimeError('Hook de la base 0x681F20 sin resolver')
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0].get('partner_restored') is not True: raise RuntimeError('Hook/bytes de la base 0x681F20 no restaurados')
    summary['partner_hook'] = ph[0]
    return summary
if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(__import__('json').dumps(analyze(Path(sys.argv[2])), indent=2)); sys.exit(0)
    probe_hash = __import__('json').loads((Path(__file__).parent/'artifacts/build-manifest.json').read_text())['binaries']['asi']
    sys.exit(runtime_runner.run_case('ctasks7b', probe_hash, analyze, 'Tareas complejas B (lista de 7)', timeout=1500))
