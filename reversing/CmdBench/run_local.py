"""Corrida local del banco por comando (CmdBench): lanza el juego de pruebas con la sonda, lee cmdbench-runtime.jsonl y resume.

Uso:  python -B run_local.py [--cmds ids.txt | --risk alto,medio | --ids 2536,2537] [--cases N] [--skip skip.txt]
      python -B run_local.py --analyze <comparisons.jsonl>      (solo analiza un registro)
Requisitos: sonda compilada (build_probe.py), candado del juego D:/Original/gta_lock (mkdir atomico), sin gta_sa.exe ajeno.
Si el juego cae dentro de un comando, el comando en curso se anade a la lista de omitidos (cmdbench-skip.txt) y se relanza el resto (--retry).
"""
import argparse
import csv
import json
import os
import subprocess
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from runtime_runner import digest, terminal_event

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
LOCK = Path('D:/Original/gta_lock')


def last_command_started(path):
    cur = None
    if not path.exists():
        return None
    with path.open('r', encoding='utf-8', errors='replace') as f:
        for line in f:
            try:
                r = json.loads(line)
            except ValueError:
                continue
            if r.get('event') == 'command_start':
                cur = r['command']
            elif r.get('event') in ('command_summary', 'command_skipped'):
                cur = None
    return cur


def analyze(path):
    """Devuelve el resumen cuando el registro termino (evento complete/error/assertion); None mientras corre."""
    path = Path(path)
    if not terminal_event_tolerant(path):
        return None
    summaries, comps, skipped, begin, complete, errors = {}, [], [], None, None, []
    with path.open('r', encoding='utf-8', errors='replace') as f:
        for line in f:
            if not line.endswith('\n'):
                continue
            try:
                r = json.loads(line)
            except ValueError:
                continue
            ev = r.get('event')
            if ev == 'begin':
                begin = r
            elif ev == 'command_summary':
                summaries[r['command']] = r
            elif ev == 'comparison':
                comps.append(r)
            elif ev == 'command_skipped':
                skipped.append(r)
            elif ev == 'complete':
                complete = r
            elif ev in ('error', 'assertion'):
                errors.append(r)
    diff_cmds = sorted({c['command'] for c in comps})
    return dict(passed=bool(complete) and not errors and not comps, commands_run=len(summaries), commands_skipped=len(skipped),
                commands_with_diffs=diff_cmds, diff_cases=len(comps), complete=complete, errors=errors[:3],
                cases=sum(s['cases'] for s in summaries.values()), log_sha256=digest(path))


def terminal_event_tolerant(path):
    if terminal_event(path):
        return True
    if not path.exists():
        return False
    with path.open('rb') as stream:
        stream.seek(max(0, path.stat().st_size - 20000))
        lines = stream.read().splitlines()
    for raw in reversed(lines):
        if b'first_chance_av' in raw[:600]:
            continue
        try:
            return json.loads(raw).get('event') in ('complete', 'error', 'assertion')
        except ValueError:
            return False
    return False


def command_ids(args):
    if args.ids:
        return sorted({int(x) for x in args.ids.split(',')})
    ids = []
    for r in csv.DictReader(open(ROOT / 'docs/ComandosScript.csv', encoding='utf-8')):
        if r['estado'] != 'reescrito':
            continue
        if args.risk and r.get('riesgo') not in args.risk.split(','):
            continue
        if args.category and Path(r['fuente'].split(':')[0]).name.replace('Commands.cpp', '') not in args.category.split(','):
            continue
        ids.append(int(r['id']))
    return ids


def take_lock():
    while True:
        try:
            LOCK.mkdir()
            (LOCK / 'owner_cmdbench').write_text(time.strftime('%H:%M:%S'))
            return
        except FileExistsError:
            time.sleep(10)


def wait_no_game(minutes=60):
    end = time.time() + minutes * 60
    while time.time() < end:
        out = subprocess.run(['tasklist', '/FI', 'IMAGENAME eq gta_sa.exe', '/NH'], capture_output=True, text=True).stdout
        if 'gta_sa.exe' not in out.lower() and not (runtime_runner.GAME / 'reversed-tests.lock').exists():
            return True
        time.sleep(10)
    return False


def release_lock():
    try:
        (LOCK / 'owner_cmdbench').unlink(missing_ok=True)
        LOCK.rmdir()
    except OSError:
        pass


GUARDED = ['scripts/gta_reversed.asi', 'scripts/gta_reversed.pdb', 'gta-reversed.ini', 'scripts/gta-reversed.ini']


def guard_snapshot():
    """Copia de seguridad de los archivos que run_case sustituye (ASI/PDB normales e INI). Se toma con el candado en mano y sin juego."""
    import shutil
    bk = HERE / 'artifacts' / 'normal_game_files'
    bk.mkdir(parents=True, exist_ok=True)
    out = {}
    for rel in GUARDED:
        src = runtime_runner.GAME / rel
        dst = bk / rel.replace('/', '__')
        h = digest(src)
        probes = {json.loads((HERE / 'artifacts/build-manifest.json').read_text())['binaries'].get('asi')}
        if rel.endswith('.asi') and h in probes:
            raise SystemExit('el ASI instalado es una sonda de CmdBench (restauracion previa fallida): restaurar a mano antes de seguir')
        shutil.copy2(src, dst)
        out[rel] = (dst, h)
    return out


def guard_restore(snap):
    import shutil
    for rel, (dst, h) in snap.items():
        tgt = runtime_runner.GAME / rel
        for _ in range(30):
            try:
                if digest(tgt) != h:
                    shutil.copy2(dst, tgt)
                if digest(tgt) == h:
                    break
            except OSError:
                pass
            time.sleep(1)
        else:
            print('NO SE PUDO RESTAURAR', rel)


STOP_FILE = HERE / 'cmdbench.stop'


def watchdog():
    """Si aparece cmdbench.stop se cierra SOLO el gta_sa.exe lanzado por este proceso (hijo directo) y el bucle de reintentos termina."""
    me = os.getpid()
    while True:
        time.sleep(3)
        if not STOP_FILE.exists():
            continue
        out = subprocess.run(['powershell', '-NoProfile', '-Command', f"Get-CimInstance Win32_Process -Filter \"ParentProcessId={me} AND Name='gta_sa.exe'\" | ForEach-Object {{ $_.ProcessId }}"],
                             capture_output=True, text=True).stdout.split()
        for pid in out:
            subprocess.run(['taskkill', '/PID', pid, '/F'], capture_output=True)


def main():
    import threading
    threading.Thread(target=watchdog, daemon=True).start()
    STOP_FILE.unlink(missing_ok=True)
    ap = argparse.ArgumentParser()
    ap.add_argument('--analyze')
    ap.add_argument('--ids')
    ap.add_argument('--risk')
    ap.add_argument('--category')
    ap.add_argument('--cases', type=int, default=64)
    ap.add_argument('--skip', default='')
    ap.add_argument('--retry', type=int, default=6)
    ap.add_argument('--timeout', type=int, default=3600)
    ap.add_argument('--tag', default='run')
    args = ap.parse_args()
    if args.analyze:
        print(json.dumps(analyze(args.analyze), indent=2))
        return 0
    manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
    ids = command_ids(args)
    skip = set()
    if args.skip and Path(args.skip).exists():
        skip |= {int(x) for x in Path(args.skip).read_text().split() if x.strip().isdigit()}
    skip_file = HERE / f'skip-{args.tag}.txt'
    take_lock()
    if not wait_no_game():
        release_lock()
        print('hay un gta_sa.exe ajeno que no se cierra: no se lanza nada')
        return 2
    rc = 1
    snap = guard_snapshot()
    try:
        for attempt in range(args.retry + 1):
            if STOP_FILE.exists():
                print('parada pedida (cmdbench.stop)')
                break
            if attempt:
                wait_no_game(5)
                guard_restore(snap)
                time.sleep(8)
            todo = [i for i in ids if i not in skip]
            if not todo:
                break
            cmds_file = HERE / f'cmds-{args.tag}.txt'
            cmds_file.write_text('\n'.join(map(str, todo)))
            skip_file.write_text('\n'.join(map(str, sorted(skip))))
            os.environ['CMDBENCH_CMDS'] = str(cmds_file)
            os.environ['CMDBENCH_SKIP'] = str(skip_file)
            os.environ['CMDBENCH_CASES'] = str(args.cases)
            log = runtime_runner.GAME / 'cmdbench-runtime.jsonl'
            (runtime_runner.GAME / 'cmdbench-runtime.start').unlink(missing_ok=True)   # resto de una corrida propia interrumpida (este script posee el candado)
            t_attempt = time.time()
            rc = runtime_runner.run_case('cmdbench', manifest['binaries']['asi'], analyze, 'CmdBench ' + args.tag, timeout=args.timeout)
            fresh = log.exists() and log.stat().st_mtime >= t_attempt
            culprit = last_command_started(log) if (rc and fresh) else None
            # el registro se archiva dentro de run_case; si fallo y quedo un comando sin resumen, es el culpable
            if rc and culprit is not None:
                print(f'comando {culprit} no termino: se omite y se reintenta ({attempt + 1}/{args.retry})')
                skip.add(culprit)
                continue
            break
    finally:
        wait_no_game(5)
        guard_restore(snap)
        release_lock()
    return rc


if __name__ == '__main__':
    sys.exit(main())
