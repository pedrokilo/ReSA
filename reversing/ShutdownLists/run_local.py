import json,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
from runtime_runner import run_case,terminal_event,digest
def analyze(path):
    if not terminal_event(path):return None
    rows=[json.loads(s) for s in path.read_text().splitlines()]
    if any(r['event'] in ('error','assertion') for r in rows):raise RuntimeError('Fallo durante la limpieza o el cierre')
    cases=[r for r in rows if r['event']=='cleanup'];cleanup=[r for r in rows if r['event']=='shutdown_cleanup']
    entries=[r for r in rows if r['event']=='shutdown_entry']
    if len(entries)!=1 or entries[0]['seeded_buildings']!=2:raise RuntimeError('Lista de cierre sin preparar')
    if len(cases)!=16 or {(r['mode'],r['size']) for r in cases}!={(m,n) for m in range(4) for n in (0,1,8,32)} or not all(r['equal'] for r in cases):raise RuntimeError('Regresion incompleta')
    if len(cleanup)!=1 or not cleanup[0]['empty'] or not rows[-1].get('shutdown_returned'):raise RuntimeError('Cierre sin verificar')
    return dict(cleanup_cases=16,shutdown_returned=True,log_sha256=digest(path))
if __name__=='__main__':
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    sys.exit(run_case('shutdown-lists',manifest['asi'],analyze,'Cierre y referencias de cobertura',unhook='',wait_for_exit=True))
