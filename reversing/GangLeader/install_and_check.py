"""Install only the isolated normal build; roll back on failed menu integration."""
import csv, ctypes, importlib.util, json, shutil, subprocess, sys, time
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE.parent))
from runtime_runner import GAME, digest, saves
from run_local import menu_configuration

def observe(handle, kernel, result):
    def read(address, size):
        buf=(ctypes.c_ubyte*size)();count=ctypes.c_size_t()
        assert kernel.ReadProcessMemory(handle,address,buf,size,ctypes.byref(count)) and count.value==size
        return bytes(buf)
    entry=read(0x65DF20,6)
    vtable=int.from_bytes(read(0x86F8FC+4*4,4),'little')
    destination=(0x65DF20+5+int.from_bytes(entry[1:5],'little',signed=True)) & 0xffffffff
    assert entry[0]==0xE9 and vtable!=0x65DF20 and destination==vtable, 'Hook normal no activo'
    result['get_task_type_hook']={'entry_bytes':entry.hex(),'vtable_target':hex(vtable),'jump_target':hex(destination),'enabled':True}

def main():
    rows=csv.reader(subprocess.check_output(['tasklist','/FO','CSV','/NH'],text=True,errors='replace').splitlines())
    if any(r and r[0].lower() in ('gta_sa.exe','gta-sa.exe') for r in rows): raise RuntimeError('GTA abierto')
    summary=json.loads((HERE/'validation-summary.json').read_text())
    assert summary['passed'] and summary['pairs']==24
    manifest=json.loads((HERE/'artifacts/normal/manifest.json').read_text())
    folder=GAME/'_baseline'/time.strftime('before-gang-leader-%Y%m%d-%H%M%S')
    folder.mkdir()
    backups=[]
    before=saves()
    configs={str(p):digest(p) for p in (GAME/'gta-reversed.ini',GAME/'scripts/gta-reversed.ini')}
    config_bytes={Path(p):Path(p).read_bytes() for p in configs}
    result={'passed':False,'manifest':manifest,'backup_directory':str(folder),'saves_before':before,'configs_before':configs}
    try:
        for ext in ('asi','pdb'):
            source=HERE/f'artifacts/normal/gta_reversed-gang-leader.{ext}'
            assert digest(source)==manifest[ext]
            for label,target in [('installed',GAME/f'scripts/gta_reversed.{ext}'),('workspace',ROOT/f'bin/Debug/gta_reversed.{ext}')]:
                backup=folder/f'{label}.{ext}'
                shutil.copy2(target,backup); backups.append((target,backup,digest(backup)))
                shutil.copy2(source,target)
        spec=importlib.util.spec_from_file_location('gang_startup',HERE.parent/'InicioNormal/run_local.py')
        startup=importlib.util.module_from_spec(spec);spec.loader.exec_module(startup)
        for p,data in config_bytes.items(): p.write_bytes(menu_configuration(data))
        result['temporary_configuration']='FastLoader=true, SaveGameToLoad=-2; skip videos, remain at menu'
        assert startup.main(observer=observe)==0, 'Integracion normal fallida'
        latest=max((HERE.parent/'InicioNormal/artifacts').glob('startup-*/summary.json'),key=lambda p:p.stat().st_mtime_ns)
        integration=json.loads(latest.read_text())
        assert integration['passed'] and integration['asi_sha256']==manifest['asi']
        result.update(passed=True,integration=integration,integration_path=str(latest),integration_sha256=digest(latest))
    finally:
        for p,data in config_bytes.items(): p.write_bytes(data)
        result['saves_unchanged']=saves()==before
        result['configs_unchanged']=all(digest(Path(p))==h for p,h in configs.items())
        result['passed'] &= result['saves_unchanged'] and result['configs_unchanged']
        if not result['passed']:
            for target,backup,h in reversed(backups):
                shutil.copy2(backup,target); assert digest(target)==h
        result['installed_hashes']={ext:digest(GAME/f'scripts/gta_reversed.{ext}') for ext in ('asi','pdb')}
        (HERE/'installation-summary.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
        print(json.dumps(result,indent=2))
    return 0 if result['passed'] else 1
if __name__=='__main__':sys.exit(main())
