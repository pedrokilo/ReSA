"""Verify both hooks during three short normal starts, then optionally install the tested pair."""
import argparse
import csv
import ctypes as c
from ctypes import wintypes as w
import json
import shutil
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE.parent))
from runtime_runner import GAME, EXE_HASH, digest, saves, probe_configuration
from run_local import FUNCTIONS, TARGET_SOURCES, require, verify_manifest

HOOKS = {'Update': 0x43C190, 'IsPlayerInPositionForConversation': 0x43AC40}
GAMEPLAY_SECONDS = 3


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--install', action='store_true')
    parser.add_argument('--startup-only', action='store_true', help='Compatibilidad: los tres arranques siempre duran tres segundos.')
    args = parser.parse_args()
    build = Path(json.loads((HERE / 'latest-normal.json').read_text(encoding='utf-8'))['folder'])
    manifest = verify_manifest(build, probe=False)
    validation = json.loads((HERE / 'validation-summary.json').read_text(encoding='utf-8'))
    counts = validation.get('function_pairs', {})
    require(validation.get('passed') is True and validation.get('all_equal') is True
            and validation.get('state_restored') is True and validation.get('natural_exit') is True
            and validation.get('exit_code') == 0, 'Falta una comparación diferencial aprobada con cierre natural.')
    require(set(counts) == FUNCTIONS and all(type(n) is int and n > 0 for n in counts.values())
            and sum(counts.values()) == validation.get('pairs'), 'La evidencia no cubre las dos funciones.')
    require(validation.get('source_sha256') == {p: manifest['workspace_sources'][p] for p in TARGET_SOURCES},
            'Las fuentes normales no son las que aprobó el banco diferencial.')
    require(digest(GAME / 'gta_sa.exe') == EXE_HASH, 'El ejecutable del juego no coincide.')
    processes = csv.reader(subprocess.check_output(['tasklist', '/FO', 'CSV', '/NH'], text=True, errors='replace').splitlines())
    require(not any(row and row[0].lower() in ('gta_sa.exe', 'gta-sa.exe') for row in processes), 'GTA ya está abierto.')

    kernel = c.WinDLL('kernel32', use_last_error=True)
    user = c.WinDLL('user32', use_last_error=True)
    psapi = c.WinDLL('psapi', use_last_error=True)
    kernel.OpenProcess.argtypes = [w.DWORD, w.BOOL, w.DWORD]
    kernel.OpenProcess.restype = w.HANDLE
    kernel.ReadProcessMemory.argtypes = [w.HANDLE, c.c_void_p, c.c_void_p, c.c_size_t, c.c_void_p]
    kernel.CloseHandle.argtypes = [w.HANDLE]
    psapi.GetMappedFileNameW.argtypes = [w.HANDLE, c.c_void_p, w.LPWSTR, w.DWORD]
    user.GetWindowThreadProcessId.argtypes = [w.HWND, c.POINTER(w.DWORD)]
    user.GetClassNameW.argtypes = [w.HWND, w.LPWSTR, c.c_int]
    user.PostMessageW.argtypes = [w.HWND, w.UINT, w.WPARAM, w.LPARAM]
    user.ShowWindow.argtypes = [w.HWND, c.c_int]
    user.SetForegroundWindow.argtypes = [w.HWND]
    callback_type = c.WINFUNCTYPE(w.BOOL, w.HWND, w.LPARAM)
    user.EnumWindows.argtypes = [callback_type, w.LPARAM]

    folder = HERE / 'artifacts' / datetime.now().strftime('normal-%Y%m%d-%H%M%S-%f')
    folder.mkdir(parents=True)
    before = saves()
    lock = GAME / 'reversed-tests.lock'
    with lock.open('x') as stream:
        stream.write('conversation-for-ped-normal')
    backups = []
    process = None
    promoted = False
    result = {
        'passed': False, 'runs': [], 'build_manifest': str(build / 'manifest.json'),
        'validation_sha256': digest(HERE / 'validation-summary.json'), 'saves_before': before,
        'tested_asi': manifest['asi'], 'tested_pdb': manifest['pdb'],
        'scope': 'Three new-game starts, three seconds in state 9 each, both hook targets mapped to the tested module, '
                 'and natural WM_CLOSE exits. This is not a complete introduction or prolonged gameplay test.',
    }
    try:
        targets = (
            ('game-asi', GAME / 'scripts/gta_reversed.asi'),
            ('game-pdb', GAME / 'scripts/gta_reversed.pdb'),
            ('build-asi', ROOT / 'bin/Debug/gta_reversed.asi'),
            ('build-pdb', ROOT / 'bin/Debug/gta_reversed.pdb'),
            ('root-ini', GAME / 'gta-reversed.ini'),
            ('scripts-ini', GAME / 'scripts/gta-reversed.ini'),
        )
        for label, target in targets:
            backup = folder / label
            shutil.copy2(target, backup)
            backups.append((target, backup, digest(backup)))
            if target.suffix == '.ini':
                config = probe_configuration(backup.read_bytes())
                require(b'SaveGameToLoad = 2\n' in config, 'No se encontró la opción temporal de partida.')
                target.write_bytes(config.replace(b'SaveGameToLoad = 2\n', b'SaveGameToLoad = 0\n'))
            elif label.startswith('game-'):
                shutil.copy2(build / target.name, target)

        for index in range(3):
            process = subprocess.Popen([str(GAME / 'gta_sa.exe')], cwd=GAME)
            print(f'Arranque normal {index + 1}/3, PID {process.pid}', flush=True)
            handle = kernel.OpenProcess(0x410, False, process.pid)
            require(bool(handle), 'No se pudo abrir el proceso del juego.')
            try:
                loaded = None
                deadline = time.monotonic() + 180
                while time.monotonic() < deadline:
                    require(process.poll() is None, f'Salida prematura: {process.returncode}')
                    state = w.DWORD()
                    require(kernel.ReadProcessMemory(handle, 0xC8D4C0, c.byref(state), 4, None), 'No se pudo leer el estado del juego.')
                    if state.value == 9:
                        if loaded is None:
                            loaded = time.monotonic()
                        if time.monotonic() - loaded >= GAMEPLAY_SECONDS:
                            break
                    else:
                        loaded = None
                    time.sleep(.25)
                else:
                    raise RuntimeError('No se alcanzó el estado de juego durante tres segundos.')
                hook_results = {}
                for name, address in HOOKS.items():
                    entry = (c.c_ubyte * 5)()
                    require(kernel.ReadProcessMemory(handle, address, entry, 5, None), f'No se pudo leer {name}.')
                    require(entry[0] == 0xE9, f'{name} no está conectado.')
                    destination = (address + 5 + int.from_bytes(bytes(entry)[1:], 'little', signed=True)) & 0xFFFFFFFF
                    module = c.create_unicode_buffer(1024)
                    require(psapi.GetMappedFileNameW(handle, destination, module, 1024), f'No se pudo resolver el destino de {name}.')
                    require(module.value.lower().endswith('\\scripts\\gta_reversed.asi'), f'{name}: módulo inesperado {module.value}')
                    hook_results[name] = {'entry': hex(address), 'target': hex(destination), 'module': module.value}
                windows = []

                @callback_type
                def visit(hwnd, _):
                    owner = w.DWORD()
                    user.GetWindowThreadProcessId(hwnd, c.byref(owner))
                    title = c.create_unicode_buffer(128)
                    user.GetClassNameW(hwnd, title, 128)
                    if owner.value == process.pid and title.value == 'Grand theft auto San Andreas':
                        windows.append(hwnd)
                    return True

                user.EnumWindows(visit, 0)
                require(len(windows) == 1, 'No se encontró una única ventana del juego.')
                user.ShowWindow(windows[0], 9)
                user.SetForegroundWindow(windows[0])
                time.sleep(.5)
                require(user.PostMessageW(windows[0], 0x10, 0, 0), 'No se pudo solicitar el cierre normal.')
                try:
                    code = process.wait(timeout=30)
                except subprocess.TimeoutExpired:
                    with (folder / f'timeout-stack-{index + 1}.txt').open('w', encoding='utf-8') as output:
                        subprocess.run([
                            sys.executable, str(HERE.parent / 'capture_live_stack.py'), str(process.pid),
                            str(folder / f'timeout-{index + 1}'),
                        ], stdout=output, stderr=subprocess.STDOUT, timeout=45)
                    raise
                require(code == 0, f'Cierre natural fallido: {code}')
                result['runs'].append({
                    'exit_code': code, 'natural_exit': True, 'gameplay_seconds': GAMEPLAY_SECONDS,
                    'hooks': hook_results,
                })
            finally:
                kernel.CloseHandle(handle)
        require(saves() == before, 'Partidas modificadas durante la comprobación.')
        if args.install:
            for ext in ('asi', 'pdb'):
                shutil.copy2(build / f'gta_reversed.{ext}', ROOT / f'bin/Debug/gta_reversed.{ext}')
                require(digest(ROOT / f'bin/Debug/gta_reversed.{ext}') == digest(GAME / f'scripts/gta_reversed.{ext}') == manifest[ext],
                        f'No se instaló exactamente el artefacto .{ext} comprobado.')
            promoted = True
        result.update(passed=True, installed=promoted, asi=manifest['asi'], pdb=manifest['pdb'])
    except Exception as error:
        result['error'] = str(error)
    finally:
        restore_errors = []
        if process is not None and process.poll() is None:
            try:
                process.terminate()
                process.wait(timeout=15)
            except Exception as error:
                restore_errors.append(f'No se pudo cerrar el proceso: {error}')
        for target, backup, sha256 in reversed(backups):
            if not promoted or target.suffix == '.ini':
                try:
                    deadline = time.monotonic() + 10
                    while True:
                        try:
                            shutil.copy2(backup, target)
                            break
                        except PermissionError:
                            if time.monotonic() >= deadline:
                                raise
                            time.sleep(.2)
                    require(digest(target) == sha256, f'Hash incorrecto al restaurar {target}.')
                except Exception as error:
                    restore_errors.append(str(error))
        result['saves_unchanged'] = saves() == before
        result['configs_restored'] = all(path.is_file() and digest(path) == sha256 for path, _, sha256 in backups if path.suffix == '.ini')
        result['backups'] = {str(path): {'path': str(backup), 'sha256': sha256} for path, backup, sha256 in backups}
        result['passed'] = result['passed'] and result['saves_unchanged'] and result['configs_restored'] and not restore_errors
        if restore_errors:
            result['restore_errors'] = restore_errors
        lock.unlink(missing_ok=True)
        (folder / 'summary.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
        if result['passed'] and promoted:
            (HERE / 'installation-summary.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
        print(json.dumps({key: value for key, value in result.items() if key in (
            'passed', 'installed', 'error', 'restore_errors', 'asi', 'pdb', 'configs_restored', 'saves_unchanged',
        )}, indent=2), flush=True)
    return 0 if result['passed'] else 1


if __name__ == '__main__':
    sys.exit(main())
