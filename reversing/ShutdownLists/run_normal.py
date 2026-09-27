"""Exercise normal ASI shutdown using WM_CLOSE, with guarded installation."""
import ctypes as c
from ctypes import wintypes as w
import csv,json,shutil,subprocess,sys,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE.parent))
from runtime_runner import GAME,EXE_HASH,digest,saves,probe_configuration

def main():
    probe='--probe' in sys.argv
    assert digest(GAME/'gta_sa.exe')==EXE_HASH
    rows=csv.reader(subprocess.check_output(['tasklist','/FO','CSV','/NH'],text=True,errors='replace').splitlines())
    if any(r and r[0].lower() in ('gta_sa.exe','gta-sa.exe') for r in rows):raise RuntimeError('GTA abierto')
    folder=HERE/'artifacts'/time.strftime('normal-%Y%m%d-%H%M%S');folder.mkdir(parents=True)
    lock=GAME/'reversed-tests.lock'
    with lock.open('x') as f:f.write('shutdown-normal')
    backups=[];process=None;debugger=None;debuglog=None;before=saves();result={'passed':False,'runs':[]}
    k=c.WinDLL('kernel32',use_last_error=True);u=c.WinDLL('user32',use_last_error=True)
    k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE
    k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.c_void_p]
    k.CloseHandle.argtypes=[w.HANDLE]
    u.GetWindowThreadProcessId.argtypes=[w.HWND,c.POINTER(w.DWORD)]
    u.IsWindowVisible.argtypes=[w.HWND]
    u.PostMessageW.argtypes=[w.HWND,w.UINT,w.WPARAM,w.LPARAM]
    u.GetClassNameW.argtypes=[w.HWND,w.LPWSTR,c.c_int]
    u.SetForegroundWindow.argtypes=[w.HWND];u.ShowWindow.argtypes=[w.HWND,c.c_int]
    callback=c.WINFUNCTYPE(w.BOOL,w.HWND,w.LPARAM)
    try:
        for rel in ['scripts/gta_reversed.asi','scripts/gta_reversed.pdb','gta-reversed.ini','scripts/gta-reversed.ini']:
            target=GAME/rel;backup=folder/rel.replace('/','-');shutil.copy2(target,backup);backups.append((target,backup,digest(backup)))
            if target.suffix=='.ini':target.write_bytes(probe_configuration(backup.read_bytes()).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = 0\n'))
            else:shutil.copy2(HERE/'artifacts'/('isolated-probe' if probe else 'isolated-normal')/target.name,target)
        if probe:
            (GAME/'shutdown-lists-runtime.jsonl').unlink(missing_ok=True)
            for suffix in ('start','manual'):(GAME/f'shutdown-lists-runtime.{suffix}').write_text('manual close test')
        result['asi']=digest(GAME/'scripts/gta_reversed.asi')
        for attempt in range(1 if probe or '--once' in sys.argv else 3):
            process=subprocess.Popen([str(GAME/'gta_sa.exe')],cwd=GAME)
            (HERE/'artifacts/active-run.json').write_text(json.dumps({'pid':process.pid,'folder':str(folder),'attempt':attempt}))
            if '--debug' in sys.argv:
                debuglog=(folder/'debugger.log').open('w')
                debugger=subprocess.Popen([sys.executable,str(HERE/'debug_abort.py'),str(process.pid),str(folder/'debug')],stdout=debuglog,stderr=subprocess.STDOUT)
            print(f'GTA propio: {process.pid}. FastLoader: partida nueva automatica, sin cargar guardados.',flush=True)
            handle=k.OpenProcess(0x410,False,process.pid);loaded=None;deadline=time.monotonic()+180
            try:
                while time.monotonic()<deadline:
                    if process.poll() is not None:raise RuntimeError(f'Premature exit: {process.returncode}')
                    state=w.DWORD()
                    assert k.ReadProcessMemory(handle,0xC8D4C0,c.byref(state),4,None)
                    if state.value==9:
                        if loaded is None:loaded=time.monotonic()
                        if time.monotonic()-loaded>=10:break
                    time.sleep(.25)
                else:raise RuntimeError('No se alcanzo gameplay')
                windows=[]
                @callback
                def visit(hwnd,_):
                    owner=w.DWORD();u.GetWindowThreadProcessId(hwnd,c.byref(owner))
                    name=c.create_unicode_buffer(128);u.GetClassNameW(hwnd,name,128)
                    if owner.value==process.pid and u.IsWindowVisible(hwnd) and name.value=='Grand theft auto San Andreas':windows.append(hwnd)
                    return True
                u.EnumWindows(visit,0)
                if not windows:raise RuntimeError('Sin ventana de juego')
                u.ShowWindow(windows[0],9);u.SetForegroundWindow(windows[0]);time.sleep(.5)
                if not u.PostMessageW(windows[0],0x10,0,0):raise RuntimeError('WM_CLOSE fallo')
                close_deadline=time.monotonic()+(120 if '--debug' in sys.argv else 30)
                while process.poll() is None and time.monotonic()<close_deadline:
                    dialogs=[]
                    @callback
                    def inspect(hwnd,_):
                        owner=w.DWORD();u.GetWindowThreadProcessId(hwnd,c.byref(owner));name=c.create_unicode_buffer(128)
                        u.GetClassNameW(hwnd,name,128)
                        if owner.value==process.pid and name.value=='#32770' and u.IsWindowVisible(hwnd):dialogs.append(hwnd)
                        return True
                    u.EnumWindows(inspect,0)
                    if dialogs:
                        with (folder/'assert-stack.txt').open('w') as output:
                            subprocess.run([sys.executable,str(HERE.parent/'capture_live_stack.py'),str(process.pid),str(folder/'assertion')],stdout=output,stderr=subprocess.STDOUT)
                        raise RuntimeError('Dialogo de asercion: pila capturada')
                    time.sleep(.1)
                if process.poll() is None:
                    with (folder/'timeout-stack.txt').open('w') as output:
                        subprocess.run([sys.executable,str(HERE.parent/'capture_live_stack.py'),str(process.pid),str(folder/'timeout')],stdout=output,stderr=subprocess.STDOUT,timeout=45)
                code=process.wait(timeout=1)
                result['runs'].append({'attempt':attempt,'exit_code':code,'natural_exit':True,'startup':'FastLoader new game; SaveGameToLoad=0'})
                if code!=0:raise RuntimeError(f'Cierre anormal: {code}')
                if probe:
                    from run_local import analyze
                    probe_log=folder/'shutdown-lists-runtime.jsonl'
                    shutil.copy2(GAME/probe_log.name,probe_log)
                    result['cleanup']=analyze(probe_log)
                    if not result['cleanup']:raise RuntimeError('Banco sin completar')
            finally:k.CloseHandle(handle)
        result['passed']=True
    except Exception as e:
        import traceback
        result['error']=str(e);result['traceback']=traceback.format_exc()
    finally:
        if process and process.poll() is None:process.terminate();process.wait(timeout=15)
        if debugger:
            try:debugger.wait(timeout=15)
            except subprocess.TimeoutExpired:debugger.terminate();debugger.wait(timeout=5)
        if debuglog:debuglog.close()
        if probe:
            for suffix in ('start','manual'):(GAME/f'shutdown-lists-runtime.{suffix}').unlink(missing_ok=True)
        for target,backup,h in reversed(backups):
            shutil.copy2(backup,target)
            if digest(target)!=h:result.update(passed=False,error='Restauracion incorrecta')
        result['saves_unchanged']=saves()==before
        result['restored']=all(digest(t)==h for t,b,h in backups)
        game_log=GAME/'scripts/logs/log.log'
        if game_log.exists():shutil.copy2(game_log,folder/'game.log')
        result['passed']=result['passed'] and result['saves_unchanged'] and result['restored']
        lock.unlink()
        (folder/'summary.json').write_text(json.dumps(result,indent=2));print(json.dumps(result));print(folder)
    return 0 if result['passed'] else 1
if __name__=='__main__':sys.exit(main())
