"""Observa 30 segundos de motor activo con el ASI instalado, sin exclusiones."""
import csv
import ctypes
import json
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import GAME, EXE_HASH, digest, saves, probe_configuration


def main():
    rows = csv.reader(subprocess.check_output(['tasklist', '/FO', 'CSV', '/NH'], text=True).splitlines())
    if any(r and r[0].lower() in ('gta_sa.exe', 'gta-sa.exe') for r in rows):
        raise RuntimeError('Ya hay GTA abierto; no se cierra una sesión ajena.')
    assert digest(GAME / 'gta_sa.exe') == EXE_HASH
    folder = Path(__file__).parent / 'artifacts' / datetime.now().strftime('current-%Y%m%d-%H%M%S')
    folder.mkdir(parents=True)
    lock = GAME / 'reversed-tests.lock'
    with lock.open('x') as stream:
        stream.write('ASI normal: integración breve')
    paths = [GAME / 'gta-reversed.ini', GAME / 'scripts/gta-reversed.ini']
    originals = {}
    process = None
    handle = None
    result = dict(passed=False, states=[], samples=[], saves_before=saves(),
                  asi_sha256=digest(GAME / 'scripts/gta_reversed.asi'), unhook='',
                  scope='30 segundos en estado IDLE; sin conducción, combate ni guardado')
    kernel = ctypes.WinDLL('kernel32', use_last_error=True)
    kernel.OpenProcess.argtypes = [ctypes.c_ulong, ctypes.c_int, ctypes.c_ulong]
    kernel.OpenProcess.restype = ctypes.c_void_p
    kernel.ReadProcessMemory.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
    kernel.CloseHandle.argtypes = [ctypes.c_void_p]
    log = GAME / 'scripts/logs/log.log'
    previous_log = log.read_bytes() if log.exists() else b''

    def read_int(address):
        value = ctypes.c_uint32()
        read = ctypes.c_size_t()
        if not kernel.ReadProcessMemory(handle, address, ctypes.byref(value), 4, ctypes.byref(read)) or read.value != 4:
            raise ctypes.WinError(ctypes.get_last_error())
        return value.value

    try:
        for index, path in enumerate(paths):
            originals[path] = path.read_bytes()
            (folder / f'normal-{index}.ini').write_bytes(originals[path])
            path.write_bytes(probe_configuration(originals[path]))
        process = subprocess.Popen([str(GAME / 'gta_sa.exe')], cwd=GAME)
        handle = kernel.OpenProcess(0x410, False, process.pid)
        if not handle:
            raise ctypes.WinError(ctypes.get_last_error())
        started = time.monotonic()
        active_since = None
        first_frame = None
        while time.monotonic() - started < 180:
            if process.poll() is not None:
                raise RuntimeError(f'GTA terminó antes de completar: {process.returncode}')
            state = read_int(0xC8D4C0)
            elapsed = round(time.monotonic() - started, 2)
            if not result['states'] or result['states'][-1]['state'] != state:
                result['states'].append(dict(state=state, seconds=elapsed))
            if state == 9:
                frame = read_int(0xB7CB4C)
                result['samples'].append(dict(seconds=elapsed, frame=frame))
                if active_since is None:
                    active_since, first_frame = time.monotonic(), frame
                if time.monotonic() - active_since >= 30:
                    progress = (frame - first_frame) & 0xFFFFFFFF
                    if progress < 100:
                        raise RuntimeError('El contador no avanzó al menos 100 frames.')
                    result.update(passed=True, idle_seconds=round(time.monotonic() - active_since, 2), frames_advanced=progress)
                    break
            else:
                active_since = None
            time.sleep(.5)
        else:
            raise RuntimeError('Tiempo agotado; el motor no completó 30 segundos activos.')
    except Exception as error:
        result['error'] = str(error)
    finally:
        if handle:
            kernel.CloseHandle(handle)
        if process and process.poll() is None:
            process.terminate()
            process.wait(timeout=15)
        for path, data in originals.items():
            path.write_bytes(data)
        result['configs_restored'] = len(originals) == 2 and all(path.read_bytes() == data for path, data in originals.items())
        result['saves_unchanged'] = saves() == result['saves_before']
        result['asi_unchanged'] = digest(GAME / 'scripts/gta_reversed.asi') == result['asi_sha256']
        result['passed'] &= result['configs_restored'] and result['saves_unchanged'] and result['asi_unchanged']
        if log.exists():
            current = log.read_bytes()
            (folder / 'game.log').write_bytes(current[len(previous_log):] if current.startswith(previous_log) else current)
        lock.unlink()
        (folder / 'summary.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
        print(('APROBADO' if result['passed'] else 'FALLO') + ': ASI normal sin exclusiones')
        print('Informe:', folder)
    return 0 if result['passed'] else 1


if __name__ == '__main__':
    sys.exit(main())
