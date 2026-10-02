"""Prueba de humo del ASI normal con Audio ACTIVO (sin --unhook=Audio).

Instala bin/Debug/gta_reversed.{asi,pdb} en la copia de pruebas (respaldando el
instalado en _baseline/before-weaponaudio-<fecha>), activa temporalmente la carga
rápida, arranca GTA, espera a la partida (gGameState 9) y lo mantiene vivo
--seconds segundos vigilando el proceso y los títulos de sus ventanas (un diálogo
de assert de la CRT se detecta por su título). Después cierra el proceso propio,
restaura los INI, comprueba partidas y analiza runtime-hooks.csv.

El ASI nuevo queda instalado (es la entrega normal); el anterior queda respaldado.
"""
import argparse
import csv
import ctypes
import ctypes.wintypes as wt
import json
import shutil
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import GAME, EXE_HASH, digest, saves, probe_configuration

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent

user = ctypes.WinDLL('user32', use_last_error=True)
kernel = ctypes.WinDLL('kernel32', use_last_error=True)
kernel.OpenProcess.argtypes = [ctypes.c_ulong, ctypes.c_int, ctypes.c_ulong]; kernel.OpenProcess.restype = ctypes.c_void_p
kernel.ReadProcessMemory.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
kernel.CloseHandle.argtypes = [ctypes.c_void_p]
EnumProc = ctypes.WINFUNCTYPE(wt.BOOL, wt.HWND, wt.LPARAM)


def window_titles(pid):
    titles = []
    def cb(hwnd, _):
        owner = wt.DWORD()
        user.GetWindowThreadProcessId(hwnd, ctypes.byref(owner))
        if owner.value == pid and user.IsWindowVisible(hwnd):
            n = user.GetWindowTextLengthW(hwnd)
            buf = ctypes.create_unicode_buffer(n + 1)
            user.GetWindowTextW(hwnd, buf, n + 1)
            if buf.value: titles.append(buf.value)
        return True
    user.EnumWindows(EnumProc(cb), 0)
    return titles


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--seconds', type=int, default=35)
    ap.add_argument('--label', default='weaponaudio')
    args = ap.parse_args()

    rows = csv.reader(subprocess.check_output(['tasklist', '/FO', 'CSV', '/NH'], text=True, errors='replace').splitlines())
    if any(r and r[0].lower() in ('gta_sa.exe', 'gta-sa.exe') for r in rows): raise RuntimeError('Ya hay GTA abierto; no se cierra una sesión ajena.')
    if digest(GAME/'gta_sa.exe') != EXE_HASH: raise RuntimeError('Ejecutable distinto del verificado.')
    lock = GAME/'reversed-tests.lock'
    with lock.open('x') as f: f.write('smoke weaponaudio')
    stamp = datetime.now().strftime('%Y%m%d-%H%M%S')
    folder = HERE/'artifacts'/f'smoke-{stamp}'; folder.mkdir(parents=True)
    result = dict(passed=False, seconds_required=args.seconds, unhook=None, saves_before=saves())
    backups = []
    process = handle = None
    try:
        # Respaldo del ASI/PDB instalado y promoción del normal recompilado.
        baseline = GAME/'_baseline'/f'before-{args.label}-{datetime.now():%Y%m%d}'
        baseline.mkdir(parents=True, exist_ok=True)
        for ext in ('asi', 'pdb'):
            inst = GAME/f'scripts/gta_reversed.{ext}'; new = ROOT/f'bin/Debug/gta_reversed.{ext}'
            target = baseline/f'gta_reversed.{ext}'
            if not target.exists(): shutil.copy2(inst, target)
            result[f'backup_{ext}'] = dict(path=str(target), sha256=digest(target))
            if digest(inst) != digest(new): shutil.copy2(new, inst)
            result[f'installed_{ext}'] = digest(inst)
            if result[f'installed_{ext}'] != digest(new): raise RuntimeError('Instalación fallida: ' + ext)
        for label, relative in (('root', 'gta-reversed.ini'), ('scripts', 'scripts/gta-reversed.ini')):
            target = GAME/relative; backup = folder/f'normal-{label}.ini'
            shutil.copy2(target, backup); backups.append((target, backup, digest(backup)))
            target.write_bytes(probe_configuration(backup.read_bytes()))
        hooks_csv = GAME/'scripts'/'runtime-hooks.csv'
        if hooks_csv.exists(): hooks_csv.unlink()
        process = subprocess.Popen([str(GAME/'gta_sa.exe'), '--dump-hooks-to=runtime-hooks.csv'], cwd=GAME)
        handle = kernel.OpenProcess(0x410, False, process.pid)
        start = time.monotonic(); playing_since = None; states = []; titles_seen = set()
        while True:
            elapsed = time.monotonic() - start
            if process.poll() is not None: raise RuntimeError(f'GTA terminó solo (código {process.returncode}).')
            titles = window_titles(process.pid); titles_seen.update(titles)
            if any('Visual C++' in t or 'Assert' in t or 'assert' in t or 'Error' in t for t in titles):
                raise RuntimeError('Diálogo de assert/error: ' + repr(titles))
            state = ctypes.c_int32(); read = ctypes.c_size_t()
            if kernel.ReadProcessMemory(handle, 0xC8D4C0, ctypes.byref(state), 4, ctypes.byref(read)) and read.value == 4:
                if not states or states[-1]['state'] != state.value: states.append(dict(state=state.value, seconds=round(elapsed, 2)))
                if state.value == 9 and playing_since is None: playing_since = time.monotonic()
            if playing_since is not None and time.monotonic() - playing_since >= args.seconds: break
            if elapsed > 240: raise RuntimeError('No se alcanzó/mantuvo la partida en 240 s.')
            time.sleep(1)
        result.update(passed=True, states=states, titles=sorted(titles_seen), alive_seconds_in_game=args.seconds,
                      total_seconds=round(time.monotonic() - start, 1))
    except Exception as error:
        result['error'] = str(error)
        if process is not None: result['titles_at_error'] = window_titles(process.pid)
    finally:
        if handle: kernel.CloseHandle(handle)
        if process is not None and process.poll() is None:
            process.terminate(); process.wait(timeout=15)
        for target, backup, expected in backups:
            shutil.copy2(backup, target)
            if digest(target) != expected: result.update(passed=False, error='INI no restaurado')
            backup.unlink()
        result['saves_unchanged'] = saves() == result['saves_before']
        if not result['saves_unchanged']: result['passed'] = False
        hooks_csv = GAME/'scripts'/'runtime-hooks.csv'
        if hooks_csv.exists():
            shutil.copy2(hooks_csv, folder/'runtime-hooks.csv')
            rows = [r for r in csv.DictReader(hooks_csv.open(encoding='utf-8')) if r['class'] == 'CAEWeaponAudioEntity']
            result['runtime_hooks'] = {r['fn_name']: dict(address=r['address'], reversed=r['reversed'], locked=r['locked']) for r in rows}
            result['runtime_hook_count'] = len(rows)
        lock.unlink(missing_ok=True)
        (folder/'summary.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
        print(json.dumps({k: v for k, v in result.items() if k not in ('saves_before',)}, indent=2))
    return 0 if result['passed'] else 1


if __name__ == '__main__':
    sys.exit(main())
