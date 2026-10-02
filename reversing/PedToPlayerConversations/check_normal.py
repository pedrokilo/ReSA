"""Verify three normal starts/closes and optionally deliver the exact tested ASI/PDB."""
import ctypes as c
from ctypes import wintypes as w
import csv, json, shutil, subprocess, sys, time
from datetime import datetime
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE.parent))
from runtime_runner import GAME, EXE_HASH, digest, saves, probe_configuration

def main():
    baseline='--baseline' in sys.argv
    install='--install' in sys.argv
    seconds=3 if '--startup-only' in sys.argv else 10
    assert not (baseline and install)
    build=Path(json.loads((HERE/'latest-normal.json').read_text())['folder'])
    manifest=json.loads((build/'manifest.json').read_text())
    validation=json.loads((HERE/'validation-summary.json').read_text())
    assert not manifest['probe'] and validation['passed'] and validation['pairs']==6266
    assert digest(GAME/'gta_sa.exe')==EXE_HASH
    if not baseline:
        for p,h in manifest['compiled_sources'].items():
            if p not in manifest['isolated_files']:assert digest(ROOT/p)==h,p
        for ext in ('asi','pdb'):assert digest(build/f'gta_reversed.{ext}')==manifest[ext]
    else:
        build=ROOT/'bin/Debug'
        manifest={'asi':digest(build/'gta_reversed.asi'),'pdb':digest(build/'gta_reversed.pdb')}
        assert manifest['asi']=='b132855e4c96c1a02824b91569de158848a723718059962d5b83f91e02c5f980'
    processes=csv.reader(subprocess.check_output(['tasklist','/FO','CSV','/NH'],text=True,errors='replace').splitlines())
    assert not any(r and r[0].lower() in ('gta_sa.exe','gta-sa.exe') for r in processes),'GTA ya está abierto'
    folder=HERE/'artifacts'/datetime.now().strftime('normal-%Y%m%d-%H%M%S');folder.mkdir(parents=True)
    lock=GAME/'reversed-tests.lock'
    with lock.open('x') as stream:stream.write('ped-to-player-normal')
    before=saves();backups=[];process=None;promoted=False;result={'passed':False,'baseline':baseline,'runs':[],'build_manifest':None if baseline else str(build/'manifest.json'),'validation_sha256':digest(HERE/'validation-summary.json'),'saves_before':before,'tested_asi':manifest['asi'],'tested_pdb':manifest['pdb'],'scope':f'New-game startup, {seconds} seconds in state 9, hook mapping and normal close. Not a complete introduction or gameplay stability test.','known_issue':'Intermittent PROLOG1 audio preload timeout also reproduced with the previous ShutdownLists binary. See integration-observations.json.'}
    k=c.WinDLL('kernel32',use_last_error=True);u=c.WinDLL('user32',use_last_error=True);ps=c.WinDLL('psapi',use_last_error=True)
    k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE
    k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.c_void_p]
    k.CloseHandle.argtypes=[w.HANDLE]
    ps.GetMappedFileNameW.argtypes=[w.HANDLE,c.c_void_p,w.LPWSTR,w.DWORD]
    u.GetWindowThreadProcessId.argtypes=[w.HWND,c.POINTER(w.DWORD)]
    u.GetClassNameW.argtypes=[w.HWND,w.LPWSTR,c.c_int]
    u.PostMessageW.argtypes=[w.HWND,w.UINT,w.WPARAM,w.LPARAM]
    callback=c.WINFUNCTYPE(w.BOOL,w.HWND,w.LPARAM)
    try:
        for label,target in [('game-asi',GAME/'scripts/gta_reversed.asi'),('game-pdb',GAME/'scripts/gta_reversed.pdb'),('build-asi',ROOT/'bin/Debug/gta_reversed.asi'),('build-pdb',ROOT/'bin/Debug/gta_reversed.pdb'),('root-ini',GAME/'gta-reversed.ini'),('scripts-ini',GAME/'scripts/gta-reversed.ini')]:
            backup=folder/label;shutil.copy2(target,backup);backups.append((target,backup,digest(backup)))
            if target.suffix=='.ini':target.write_bytes(probe_configuration(backup.read_bytes()).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = 0\n'))
            elif label.startswith('game-'):shutil.copy2(build/target.name,target)
        for i in range(3):
            process=subprocess.Popen([str(GAME/'gta_sa.exe')],cwd=GAME)
            print(f'Arranque normal {i+1}/3, PID {process.pid}',flush=True)
            handle=k.OpenProcess(0x410,False,process.pid);assert handle
            try:
                loaded=None;deadline=time.monotonic()+180
                while time.monotonic()<deadline:
                    assert process.poll() is None,f'Salida prematura: {process.returncode}'
                    state=w.DWORD();assert k.ReadProcessMemory(handle,0xC8D4C0,c.byref(state),4,None)
                    if state.value==9:
                        if loaded is None:loaded=time.monotonic()
                        if time.monotonic()-loaded>=seconds:break
                    time.sleep(.25)
                else:raise RuntimeError('No se alcanzó el estado de juego')
                entry=(c.c_ubyte*5)();assert k.ReadProcessMemory(handle,0x43B0F0,entry,5,None)
                target=0x43B0F0;name=c.create_unicode_buffer(1024)
                if baseline:
                    assert entry[0]==0x64,'El control debe ejecutar Update original'
                else:
                    assert entry[0]==0xE9,'Update no está conectado'
                    target=0x43B0F5+int.from_bytes(bytes(entry)[1:],'little',signed=True)
                    assert ps.GetMappedFileNameW(handle,target,name,1024)
                    assert name.value.lower().endswith('\\scripts\\gta_reversed.asi'),name.value
                windows=[]
                @callback
                def visit(hwnd,_):
                    owner=w.DWORD();u.GetWindowThreadProcessId(hwnd,c.byref(owner));title=c.create_unicode_buffer(128);u.GetClassNameW(hwnd,title,128)
                    if owner.value==process.pid and title.value=='Grand theft auto San Andreas':windows.append(hwnd)
                    return True
                u.EnumWindows(visit,0);assert len(windows)==1
                u.ShowWindow(windows[0],9);u.SetForegroundWindow(windows[0]);time.sleep(.5)
                assert u.PostMessageW(windows[0],0x10,0,0)
                try:
                    code=process.wait(timeout=30)
                except subprocess.TimeoutExpired:
                    with (folder/'timeout-stack.txt').open('w') as output:
                        subprocess.run([sys.executable,str(HERE.parent/'capture_live_stack.py'),str(process.pid),str(folder/'timeout')],stdout=output,stderr=subprocess.STDOUT,timeout=45)
                    raise
                assert code==0,code
                result['runs'].append({'exit_code':code,'gameplay_seconds':seconds,'update_hook_target':hex(target),'module':name.value})
            finally:k.CloseHandle(handle)
        assert saves()==before
        if install:
            for ext in ('asi','pdb'):
                shutil.copy2(build/f'gta_reversed.{ext}',ROOT/f'bin/Debug/gta_reversed.{ext}')
                assert digest(ROOT/f'bin/Debug/gta_reversed.{ext}')==digest(GAME/f'scripts/gta_reversed.{ext}')==manifest[ext]
            promoted=True
        result.update(passed=True,installed=promoted,asi=manifest['asi'],pdb=manifest['pdb'])
    except Exception as exc:
        result['error']=str(exc)
    finally:
        if process is not None and process.poll() is None:process.terminate();process.wait(timeout=15)
        for target,backup,h in reversed(backups):
            if not promoted or target.suffix=='.ini':shutil.copy2(backup,target);assert digest(target)==h
        result['saves_unchanged']=saves()==before
        result['configs_restored']=all(digest(p)==h for p,b,h in backups if p.suffix=='.ini')
        result['backups']={str(p):{'path':str(b),'sha256':h} for p,b,h in backups}
        result['passed']=result['passed'] and result['saves_unchanged'] and result['configs_restored']
        lock.unlink();(folder/'summary.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
        if result['passed'] and promoted:
            (HERE/'installation-summary.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
        print(json.dumps({k:v for k,v in result.items() if k in ('passed','installed','error','asi','pdb','configs_restored','saves_unchanged')},indent=2))
    return 0 if result['passed'] else 1

if __name__=='__main__':sys.exit(main())
