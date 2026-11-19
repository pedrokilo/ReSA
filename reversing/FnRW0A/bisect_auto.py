"""Biseccion binaria automatica: python bisect_auto.py <archivo_candidatos> <prefijo>. Senal de fallo: NaN en la posicion del jugador, crash o caidas. Un culpable unico supuesto."""
import json, subprocess, sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
C = Path(sys.argv[1]).read_text().strip().split(',')
pre = sys.argv[2]
base = Path(sys.argv[3]).read_text().strip().split(',') if len(sys.argv) > 3 else []
log = HERE / f'{pre}.log'
step = 0
while len(C) > 1:
    step += 1
    half = C[:len(C) // 2]
    out = subprocess.run([sys.executable, str(HERE / 'bisect_run.py'), f'{pre}{step}', ','.join(base + half)], capture_output=True, text=True).stdout.strip().splitlines()[-1]
    sig = json.loads(out)
    bad = bool(sig.get('nan_en_estado') or sig.get('crash'))
    with log.open('a', encoding='utf-8') as f:
        f.write(json.dumps(dict(paso=step, apagados=len(half), candidatos=len(C), senal=sig, fallo_con_mitad_apagada=bad), ensure_ascii=False) + '\n')
    C = C[len(C) // 2:] if bad else half
with log.open('a', encoding='utf-8') as f:
    f.write('CULPABLE: ' + ','.join(C) + '\n')
