"""Build shutdown work without installing deferred CustomBuilding or local Climb edits."""
import hashlib,json,shutil,subprocess,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE.parent))
from runtime_runner import digest
ISOLATE={
 'source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp':'11fd6912f759d3bc93208a9def001b6dd751dbdf',
 'source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp':'11fd6912f759d3bc93208a9def001b6dd751dbdf',
 'source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp':'aef6bcbf051d0370cc6ed68e8d9809e0b7bb8d51',
}
def main():
    probe='--probe' in sys.argv
    folder=HERE/'artifacts'/('isolated-probe' if probe else 'isolated-normal');folder.mkdir(parents=True,exist_ok=True)
    saved={p:(ROOT/p).read_bytes() for p in ISOLATE}
    for p,data in saved.items():(folder/(Path(p).name+'.before')).write_bytes(data)
    for ext in ('asi','pdb'):shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',folder/f'before.{ext}')
    try:
        for p,ref in ISOLATE.items():(ROOT/p).write_bytes(subprocess.check_output(['git','show',f'{ref}:{p}'],cwd=ROOT))
        source_files=subprocess.check_output(['git','ls-files','source','cmake','conan','CMakeLists.txt','CMakePresets.json','conanfile.py'],cwd=ROOT,text=True).splitlines()
        manifest={'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),'isolated_files':ISOLATE,'workspace_sources':{p:hashlib.sha256(b).hexdigest() for p,b in saved.items()},'compiled_sources':{p:digest(ROOT/p) for p in source_files if (ROOT/p).is_file()},'probe':probe}
        with (folder/'build.log').open('w') as log:
            cmd=[sys.executable,str(HERE/'build_probe.py')] if probe else [str(ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'),'--build',str(ROOT/'build'),'--config','Debug']
            subprocess.run(cmd,cwd=ROOT,stdout=log,stderr=subprocess.STDOUT,check=True)
        for ext in ('asi','pdb'):
            dest=folder/f'gta_reversed.{ext}';shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',dest);manifest[ext]=digest(dest)
        (folder/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
        print(json.dumps({k:manifest[k] for k in ('head','probe','asi','pdb')}))
    finally:
        for p,b in saved.items():(ROOT/p).write_bytes(b)
        for ext in ('asi','pdb'):shutil.copy2(folder/f'before.{ext}',ROOT/f'bin/Debug/gta_reversed.{ext}')
        assert all((ROOT/p).read_bytes()==b for p,b in saved.items())
if __name__=='__main__':main()
