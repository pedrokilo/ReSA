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
PROBE_HASH = '2941ea33e49a6b319368c81f775f08dbc3688b2306591508f1278ab3ba45c845'

def digest(path):
    return hashlib.file_digest(path.open('rb'), 'sha256').hexdigest()

def analyze(path):
    rows = []
    if path.exists():
        for line in path.read_text(encoding='utf-8').splitlines(keepends=True):
            if line.endswith('\n'):
                rows.append(json.loads(line))
    if any(r.get('event') in ('assertion', 'error', 'stopped') for r in rows):
        raise RuntimeError('El motor informo un error; revisar el registro.')
    comparisons = [r for r in rows if r.get('event') == 'comparison']
    edges = [r for r in rows if r.get('event') == 'edge_comparison']
    for r in comparisons + edges:
        if r.get('equal') is not True or r['original'] != r['candidate']:
            raise RuntimeError('La funcion original y la reconstruida difieren.')
        for side in ('original', 'candidate'):
            if 'error' in r[side] or any(r[side].get('reference_delta', [])):
                raise RuntimeError('Error o referencias sin liberar.')
    if not any(r.get('event') == 'complete' for r in rows):
        return None
    if len(comparisons) != 60 or {r['pair'] for r in comparisons} != set(range(60)) or len(edges) != 3:
        raise RuntimeError('Registro incompleto o duplicado.')
    coverage = {}
    for r in comparisons + edges:
        for key, value in r['coverage'].items():
            coverage[key] = coverage.get(key, 0) + value
    if any(coverage.get(k, 0) <= 0 for k in ('calls', 'removed', 'destroyed_frame', 'kept_upgrade', 'non_atomic', 'model_ref')):
        raise RuntimeError('Faltan ramas de cobertura.')
    return dict(vehicle_pairs=60, edge_pairs=3, all_equal=True, coverage=coverage,
                log_sha256=digest(path), visual_review=False, general_game_stability_validated=False)

def run():
    exe = GAME / 'gta_sa.exe'
    probe = GAME / '_baseline/gta_reversed-runtime-probe.asi'
    if digest(exe) != EXE_HASH or digest(probe) != PROBE_HASH:
        raise RuntimeError('El ejecutable o el ASI de pruebas no coincide con la version verificada.')
    tasks = subprocess.check_output(['tasklist', '/FO', 'CSV', '/NH'], text=True, errors='replace')
    if any(r and r[0].lower() in ('gta_sa.exe', 'gta-sa.exe') for r in csv.reader(tasks.splitlines())):
        raise RuntimeError('Cierra GTA antes de iniciar esta prueba.')
    # Exclusive creation also prevents two runners from modifying the same installation.
    lock = GAME / 'cvehicle-runner.lock'
    with lock.open('x') as f:
        f.write(str(datetime.now()))
    folder = GAME / 'resultados-cvehicle' / datetime.now().strftime('%Y%m%d-%H%M%S-%f')
    process = None
    owns_marker = False
    backups = []
    marker = GAME / 'cvehicle-runtime.start'
    log = GAME / 'cvehicle-runtime.jsonl'
    result = dict(passed=False)
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
            shutil.copy2(GAME / f'_baseline/gta_reversed-runtime-probe.{ext}', target)
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
        (folder / 'summary.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
        status = ('APROBADO: 60 comparaciones de vehiculos y 3 casos adicionales coinciden.'
                  if result['passed'] else 'FALLO: ' + result.get('restore_error', result.get('error', 'desconocido')))
        message = status + '\nNo certifica estabilidad general ni aspecto visual.\nInforme: ' + str(folder)
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
