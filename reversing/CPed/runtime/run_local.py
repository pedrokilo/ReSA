"""Local deterministic GTA comparison runner. No network or AI services."""
import csv
import hashlib
import json
import shutil
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path

GAME = Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test')
EXE_HASH = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
PROBE_HASH = 'fd66e7dbfc8d44d739141d553ec807778881e7e7816dad33eb27e68ac97575c6'

def digest(path):
    return hashlib.file_digest(path.open('rb'), 'sha256').hexdigest()

def analyze(path):
    rows=[]
    if path.exists():
        for line in path.read_text(encoding='utf-8').splitlines(keepends=True):
            if line.endswith('\n'):rows.append(json.loads(line))
    if any(r.get('event') in ('error','assertion','stopped') for r in rows):raise RuntimeError('Error registrado dentro del juego.')
    pairs=[r for r in rows if r.get('event')=='comparison']
    for r in pairs:
        if not r.get('equal') or r.get('max_error',1)>0.00001 or r.get('candidate_calls')!=1 or r.get('original_candidate_calls')!=0:
            raise RuntimeError('Diferencia de estado o seleccion del hook.')
    complete=[r for r in rows if r.get('event')=='complete']
    if not complete:return None
    if len(complete)!=1 or complete[0]['pairs']!=361 or not complete[0]['state_restored'] or len(pairs)!=361 or {r['pair'] for r in pairs}!=set(range(361)) or complete[0]['ladder_cases']==0:
        raise RuntimeError('Cobertura incompleta o estado no restaurado.')
    ready=[r for r in rows if r.get('event')=='player_ready']
    if len(ready)!=1:raise RuntimeError('No se verifico entrada al juego.')
    return dict(pairs=361,all_equal=True,tolerance=0.00001,max_error=max(r['max_error'] for r in pairs),ladder_cases=complete[0]['ladder_cases'],state_restored=True,candidate_calls=361,log_sha256=digest(path))

def saves():
    roots=[GAME/'userfiles',Path(r'C:\Users\pedro\OneDrive\Documents\GTA San Andreas User Files')]
    return {str(p):digest(p) for root in roots for p in root.glob('*.b')}

def run():
    exe = GAME / 'gta_sa.exe'
    probe = GAME / '_baseline/gta_reversed-attached-probe.asi'
    if digest(exe) != EXE_HASH or digest(probe) != PROBE_HASH:
        raise RuntimeError('El ejecutable o el ASI de pruebas no coincide con la version verificada.')
    tasks = subprocess.check_output(['tasklist', '/FO', 'CSV', '/NH'], text=True, errors='replace')
    if any(r and r[0].lower() in ('gta_sa.exe', 'gta-sa.exe') for r in csv.reader(tasks.splitlines())):
        raise RuntimeError('Cierra GTA antes de iniciar esta prueba.')
    # Exclusive creation also prevents two runners from modifying the same installation.
    lock = GAME / 'attached-runner.lock'
    with lock.open('x') as f:
        f.write(str(datetime.now()))
    folder = GAME / 'resultados-attached' / datetime.now().strftime('%Y%m%d-%H%M%S-%f')
    process = None
    owns_marker = False
    backups = []
    marker = GAME / 'attached-runtime.start'
    log = GAME / 'attached-runtime.jsonl'
    original_saves = saves()
    result = dict(passed=False, saves_before=original_saves)
    try:
        folder.mkdir(parents=True)
        if marker.exists():
            raise RuntimeError('Existe un marcador de otra prueba; no se ha iniciado el juego.')
        if log.exists():
            shutil.move(log, folder / 'previous.jsonl')
        for ext in ('asi', 'pdb'):
            target = GAME / f'scripts/gta_reversed.{ext}'
            backup = folder / f'normal.{ext}'
            shutil.copy2(target, backup)
            backups.append((target, backup, digest(backup)))
            shutil.copy2(GAME / f'_baseline/gta_reversed-attached-probe.{ext}', target)
        marker.write_text('Local runner\n', encoding='utf-8')
        owns_marker = True
        process = subprocess.Popen([str(exe), '--unhook=Audio,Global/SurfaceInfos_c',
                                    '--dump-hooks-to=runtime-hooks.csv'], cwd=GAME)
        deadline = time.monotonic() + 120
        while time.monotonic() < deadline:
            evidence = analyze(log)
            if evidence is not None:
                result.update(evidence, passed=True)
                break
            if process.poll() is not None:
                raise RuntimeError(f'El juego termino antes de completar las pruebas ({process.returncode}).')
            time.sleep(1)
        else:
            raise RuntimeError('Tiempo agotado (120 segundos).')
    except Exception as error:
        result.update(passed=False, error=str(error))
    finally:
        if process is not None:
            try:
                if process.poll() is None:
                    process.terminate()
                process.wait(timeout=15)
            except Exception as error:
                result.update(passed=False, error='No se pudo cerrar el juego: ' + str(error))
        for target, backup, expected in reversed(backups):
            try:
                shutil.copy2(backup, target)
                if digest(target) != expected:
                    raise RuntimeError('Hash diferente al restaurar.')
                backup.unlink()
            except Exception as error:
                result.update(passed=False, restore_error=str(error))
        if owns_marker:
            marker.unlink(missing_ok=True)
            if log.exists():
                shutil.copy2(log, folder / 'comparisons.jsonl')
        lock.unlink(missing_ok=True)
        folder.mkdir(parents=True, exist_ok=True)
        result['saves_unchanged'] = saves() == original_saves
        if not result['saves_unchanged']:
            result.update(passed=False, error='Las partidas cambiaron durante la prueba.')
        (folder / 'summary.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
        status = ('APROBADO: 361 comparaciones de PositionAttachedPed dentro de GTA coinciden.'
                  if result['passed'] else 'FALLO: ' + result.get('restore_error', result.get('error', 'desconocido')))
        message = status + '\nSe probo PositionAttachedPed; no todas las funciones de peatones.\nInforme: ' + str(folder)
        (folder / 'RESULTADO.txt').write_text(message, encoding='utf-8')
        print(message)
    return 0 if result['passed'] else 1

if __name__ == '__main__':
    try:
        if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
            evidence = analyze(Path(sys.argv[2]))
            if evidence is None:
                raise RuntimeError('Registro incompleto.')
            print(json.dumps(evidence, indent=2))
            sys.exit(0)
        sys.exit(run())
    except Exception as error:
        print('No se pudo ejecutar la prueba:', error)
        sys.exit(1)
