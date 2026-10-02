"""Shared local runner: own process, guarded binary swap, restore and save hashes."""
import csv
import hashlib
import json
import re
import shutil
import subprocess
import time
from datetime import datetime
from pathlib import Path
GAME=Path(r'D:\JUEGOS\GTA San Andreas - CVehicle Test')
EXE_HASH='72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
def digest(path):
    with path.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def saves():
    roots=[GAME/'userfiles',Path(r'C:\Users\pedro\OneDrive\Documents\GTA San Andreas User Files')]
    return {str(p):digest(p) for root in roots for p in root.glob('*.b')}
def terminal_event(path):
    """Cheap completion check; the full analyzer remains responsible for acceptance."""
    if not path.exists():return False
    with path.open('rb') as stream:
        stream.seek(max(0,path.stat().st_size-4096))
        tail=stream.read()
    if not tail.endswith(b'\n'):return False
    try:return json.loads(tail.splitlines()[-1]).get('event') in ('complete','error','assertion')
    except (ValueError,IndexError):return False

def probe_configuration(data):
    """Activar carga rápida solo durante el banco, conservando otras secciones."""
    text = data.decode('utf-8-sig')
    values = dict(Enable='true', SaveGameToLoad='2', NoEAX='true', NoNVidia='true',
                  NoLogo='true', NoTitleOrIntro='true', NoCopyright='true',
                  NoFading='true', NoLoadingTune='true', RenderAtAllTimes='true')
    section = re.search(r'(?ms)^\[FastLoader\][^\r\n]*\r?\n.*?(?=^\[|\Z)', text)
    replacement = '[FastLoader]\n' + ''.join(f'{key} = {value}\n' for key, value in values.items())
    if section:
        text = text[:section.start()] + replacement + text[section.end():]
    else:
        text = replacement + text
    return text.encode('utf-8')

def game_dialog(pid):
    """Texto de un cuadro de dialogo (#32770) visible del proceso, o None. Asserts, errores de RW, seleccion de dispositivo..."""
    import ctypes
    from ctypes import wintypes as w
    user=ctypes.windll.user32;found=[]
    proc=ctypes.WINFUNCTYPE(w.BOOL,w.HWND,w.LPARAM)
    def text(h):
        buf=ctypes.create_unicode_buffer(1024);user.GetWindowTextW(h,buf,1024);return buf.value.strip()
    def child(h,_):
        t=text(h)
        if t:found[-1].append(t)
        return True
    def top(h,_):
        owner=w.DWORD();user.GetWindowThreadProcessId(h,ctypes.byref(owner))
        cls=ctypes.create_unicode_buffer(64);user.GetClassNameW(h,cls,64)
        if owner.value==pid and cls.value=='#32770' and user.IsWindowVisible(h):
            found.append([text(h)]);user.EnumChildWindows(h,proc(child),0)
        return True
    user.EnumWindows(proc(top),0)
    return ' | '.join(' / '.join(x for x in d if x) for d in found) or None

def run_case(prefix,probe_hash,analyze,title,timeout=180,stop_before_analysis=False,unhook='Audio,Global/SurfaceInfos_c',wait_for_exit=False):
    exe=GAME/'gta_sa.exe';probe=GAME/f'_baseline/gta_reversed-{prefix}-probe.asi'
    if digest(exe)!=EXE_HASH or digest(probe)!=probe_hash:raise RuntimeError('Ejecutable o sonda no coincide con el hash aprobado.')
    rows=csv.reader(subprocess.check_output(['tasklist','/FO','CSV','/NH'],text=True,errors='replace').splitlines())
    if any(r and r[0].lower() in ('gta_sa.exe','gta-sa.exe') for r in rows):raise RuntimeError('Cerrar GTA antes de la prueba.')
    lock=GAME/'reversed-tests.lock'
    with lock.open('x') as f:f.write(prefix)
    folder=GAME/f'resultados-{prefix}'/datetime.now().strftime('%Y%m%d-%H%M%S-%f')
    marker=GAME/f'{prefix}-runtime.start';log=GAME/f'{prefix}-runtime.jsonl'
    process=None;backups=[];owns_marker=False;before=saves();result={'passed':False,'saves_before':before}
    try:
        folder.mkdir(parents=True)
        if marker.exists():raise RuntimeError('Existe un marcador previo de prueba.')
        if log.exists():shutil.move(log,folder/'previous.jsonl')
        result['config_hashes_before'] = {}
        for label, relative in (('root', 'gta-reversed.ini'), ('scripts', 'scripts/gta-reversed.ini')):
            target=GAME/relative;backup=folder/f'normal-{label}.ini'
            shutil.copy2(target,backup);expected=digest(backup)
            backups.append((target,backup,expected));result['config_hashes_before'][relative]=expected
            target.write_bytes(probe_configuration(backup.read_bytes()))
        for ext in ('asi','pdb'):
            target=GAME/f'scripts/gta_reversed.{ext}';backup=folder/f'normal.{ext}'
            shutil.copy2(target,backup);backups.append((target,backup,digest(backup)))
            shutil.copy2(GAME/f'_baseline/gta_reversed-{prefix}-probe.{ext}',target)
        marker.write_text('local deterministic comparison\n');owns_marker=True
        args=[str(exe),'--dump-hooks-to=runtime-hooks.csv']
        if unhook:args.append('--unhook='+unhook)
        process=subprocess.Popen(args,cwd=GAME)
        deadline=time.monotonic()+timeout
        while time.monotonic()<deadline:
            if stop_before_analysis and process.poll() is None and terminal_event(log):
                process.terminate()
                process.wait(timeout=15)
                result['game_stopped_before_analysis']=True
            evidence=analyze(log)
            if evidence is not None:
                if wait_for_exit:
                    code=process.wait(timeout=30)
                    if code!=0:raise RuntimeError(f'Cierre normal fallido: {code}')
                    result.update(natural_exit=True,exit_code=code)
                result.update(evidence,passed=True);break
            if process.poll() is not None:raise RuntimeError(f'GTA termino antes de completar ({process.returncode}).')
            # Cualquier cuadro de dialogo bloquea la prueba: se aborta en el acto y se guarda su texto.
            dialog=game_dialog(process.pid)
            if dialog:
                result['dialog']=dialog
                raise RuntimeError('Dialogo en GTA (abortado): '+dialog)
            time.sleep(.5)
        else:raise RuntimeError('Tiempo agotado.')
    except Exception as error:result.update(passed=False,error=str(error))
    finally:
        if process is not None:
            try:
                if process.poll() is None:process.terminate()
                process.wait(timeout=15)
            except Exception as error:result.update(passed=False,error='No se pudo cerrar el proceso: '+str(error))
        for target,backup,expected in reversed(backups):
            try:
                # Windows may release a terminated process's mapped ASI slightly
                # after wait() returns. Retry only that transient sharing error.
                restore_deadline=time.monotonic()+10
                while True:
                    try:
                        shutil.copy2(backup,target)
                        break
                    except PermissionError:
                        if time.monotonic()>=restore_deadline:raise
                        time.sleep(.2)
                if digest(target)!=expected:raise RuntimeError('Hash al restaurar no coincide.')
                backup.unlink()
            except Exception as error:result.update(passed=False,restore_error=str(error))
        if owns_marker:
            marker.unlink(missing_ok=True)
            if log.exists():shutil.copy2(log,folder/'comparisons.jsonl')
        lock.unlink(missing_ok=True);folder.mkdir(parents=True,exist_ok=True)
        result['saves_unchanged']=saves()==before
        result['configs_restored']=all((GAME/p).is_file() and digest(GAME/p)==h for p,h in result.get('config_hashes_before',{}).items())
        if not result['configs_restored']:result.update(passed=False,error='No se restauraron los INI.')
        if not result['saves_unchanged']:result.update(passed=False,error='Partidas modificadas durante la prueba.')
        (folder/'summary.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
        message=('APROBADO' if result['passed'] else 'FALLO')+': '+title+'\n'+result.get('restore_error',result.get('error','Comparacion automatizada completada.'))+'\nInforme: '+str(folder)
        (folder/'RESULTADO.txt').write_text(message,encoding='utf-8');print(message)
    return 0 if result['passed'] else 1
