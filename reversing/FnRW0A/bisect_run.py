"""Una corrida de biseccion: python bisect_run.py <etiqueta> <ruta1,ruta2,...|@archivo> [minutos]. Devuelve la senal de fallo por stdout (JSON)."""
import json, re, subprocess, sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
label, lst = sys.argv[1], sys.argv[2]
minutes = sys.argv[3] if len(sys.argv) > 3 else '2.5'
if lst.startswith('@'):
    lst = Path(lst[1:]).read_text().strip()
p = subprocess.run([sys.executable, str(HERE / 'run_phase.py'), 'F2', '--label', label, '--unhook', lst, '--minutes', minutes], capture_output=True, text=True)
folder = None
for l in p.stdout.splitlines():
    if l.startswith('Carpeta:'):
        folder = Path(l.split(':', 1)[1].strip())
sig = dict(label=label, folder=str(folder))
if folder:
    res = json.loads((folder / 'phase_result.json').read_text(encoding='utf-8'))
    sig['verdict'] = res.get('verdict')
    sig['exit_code'] = res.get('exit_code')
    falls = 0
    sm = folder / 'game_log_summary.txt'
    if sm.exists():
        for l in sm.read_text(encoding='utf-8').splitlines():
            m = re.match(r'\s*(\d+)\s+.*caído a través', l)
            if m:
                falls += int(m.group(1))
        sig['version_aviso'] = 'otra versión' in sm.read_text(encoding='utf-8')
    sig['caidas_en_plantillas'] = falls
    sig['nan_en_estado'] = sum(1 for l in (folder / 'state.jsonl').read_text().splitlines() if ':nan' in l)
    sig['crash'] = (folder / 'crash.log').exists() and res.get('verdict', {}).get('kind') == 'exit'
    sig['fallo'] = bool(falls or sig['nan_en_estado'] or sig['crash'] or res.get('verdict', {}).get('kind') not in ('completada',))
print(json.dumps(sig, ensure_ascii=False))
