"""Comprueba el inicio normal leyendo estados del motor, sin pulsar teclas."""
import csv,ctypes,json,subprocess,sys,time
from datetime import datetime
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import GAME,EXE_HASH,digest,saves
def main():
    rows=csv.reader(subprocess.check_output(['tasklist','/FO','CSV','/NH'],text=True,errors='replace').splitlines())
    if any(r and r[0].lower() in ('gta_sa.exe','gta-sa.exe') for r in rows):raise RuntimeError('Ya hay GTA abierto; no se cierra una sesion ajena.')
    assert digest(GAME/'gta_sa.exe')==EXE_HASH
    lock=GAME/'reversed-tests.lock'
    with lock.open('x') as f:f.write('inicio normal')
    folder=Path(__file__).parent/'artifacts'/datetime.now().strftime('startup-%Y%m%d-%H%M%S')
    folder.mkdir(parents=True)
    before=saves();configs={str(p):digest(p) for p in (GAME/'gta-reversed.ini',GAME/'scripts/gta-reversed.ini')}
    result=dict(passed=False,states=[],foreground=[],saves_before=before,configs_before=configs,asi_sha256=digest(GAME/'scripts/gta_reversed.asi'))
    log=GAME/'scripts/logs/log.log'
    log_before=log.read_bytes() if log.exists() else b''
    process=None;handle=None
    kernel=ctypes.WinDLL('kernel32',use_last_error=True)
    kernel.OpenProcess.argtypes=[ctypes.c_ulong,ctypes.c_int,ctypes.c_ulong];kernel.OpenProcess.restype=ctypes.c_void_p
    kernel.ReadProcessMemory.argtypes=[ctypes.c_void_p,ctypes.c_void_p,ctypes.c_void_p,ctypes.c_size_t,ctypes.POINTER(ctypes.c_size_t)]
    kernel.CloseHandle.argtypes=[ctypes.c_void_p]
    user=ctypes.WinDLL('user32',use_last_error=True)
    user.GetForegroundWindow.restype=ctypes.c_void_p
    user.GetWindowThreadProcessId.argtypes=[ctypes.c_void_p,ctypes.POINTER(ctypes.c_ulong)]
    try:
        process=subprocess.Popen([str(GAME/'gta_sa.exe')],cwd=GAME)
        handle=kernel.OpenProcess(0x410,False,process.pid)
        if not handle:raise ctypes.WinError(ctypes.get_last_error())
        started=time.monotonic();menu_since=None
        while time.monotonic()-started<180:
            if process.poll() is not None:raise RuntimeError(f'GTA termino: {process.returncode}')
            state=ctypes.c_int32();read=ctypes.c_size_t()
            if kernel.ReadProcessMemory(handle,0xC8D4C0,ctypes.byref(state),4,ctypes.byref(read)) and read.value==4:
                value=state.value
                elapsed=time.monotonic()-started
                foreground=ctypes.c_ubyte();foreground_read=ctypes.c_size_t()
                if kernel.ReadProcessMemory(handle,0x8D621C,ctypes.byref(foreground),1,ctypes.byref(foreground_read)) and foreground_read.value==1:
                    owner=ctypes.c_ulong()
                    user.GetWindowThreadProcessId(user.GetForegroundWindow(),ctypes.byref(owner))
                    active=bool(foreground.value)
                    if not result['foreground'] or result['foreground'][-1]['active']!=active or result['foreground'][-1]['foreground_pid']!=owner.value:
                        result['foreground'].append(dict(active=active,seconds=round(elapsed,2),foreground_pid=owner.value))
                if not result['states'] or result['states'][-1]['state']!=value:result['states'].append(dict(state=value,seconds=round(time.monotonic()-started,2)))
                if value in (8,9):raise RuntimeError('Se entro al juego sin seleccion del usuario')
                if value==7:
                    if menu_since is None:menu_since=time.monotonic()
                    if time.monotonic()-menu_since>=10:
                        result.update(passed=True,menu_stable_seconds=10);break
                else:menu_since=None
            time.sleep(.25)
        else:raise RuntimeError('No se alcanzo el menu en 180 segundos')
    except Exception as error:result['error']=str(error)
    finally:
        if handle:kernel.CloseHandle(handle)
        if process and process.poll() is None:process.terminate();process.wait(timeout=15)
        result['saves_unchanged']=saves()==before
        result['configs_unchanged']=all(Path(p).is_file() and digest(Path(p))==h for p,h in configs.items())
        if not result['saves_unchanged'] or not result['configs_unchanged']:result['passed']=False
        if log.exists():
            contents=log.read_bytes()
            fresh=contents[len(log_before):] if contents.startswith(log_before) else contents
            (folder/'game.log').write_bytes(fresh)
            result['automatic_save_load_logged']='Loading game from slot' in fresh.decode(errors='replace')
            if result['automatic_save_load_logged']:result['passed']=False
        lock.unlink()
        (folder/'summary.json').write_text(json.dumps(result,indent=2))
        print(json.dumps(result,indent=2));print('Informe:',folder)
    return 0 if result['passed'] else 1
if __name__=='__main__':sys.exit(main())
