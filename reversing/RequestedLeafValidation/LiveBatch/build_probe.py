import hashlib,json,os,shutil,subprocess,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[2];ART=HERE/'artifacts';ART.mkdir(exist_ok=True)
sys.path.insert(0,str(ROOT/'reversing'));from runtime_runner import digest,EXE_HASH,GAME
paths=['source/app/app_game.cpp','source/dllmain.cpp'];saved={p:(ROOT/p).read_bytes() for p in paths};bins={ext:ROOT/f'bin/Debug/gta_reversed.{ext}' for ext in ('asi','pdb')}
for ext,p in bins.items():shutil.copy2(p,ART/f'before.{ext}')
manifest={'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),'tested_sources':{p:digest(ROOT/p) for p in ['source/game_sa/DecisionMakers/Decision.cpp','source/game_sa/DecisionMakers/Decision.h','source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.h','source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp']},'bank_sources':{p.name:digest(p) for p in HERE.glob('*.py')}|{'LiveComparison.hpp':digest(HERE/'LiveComparison.hpp')},'exe_sha256':digest(GAME/'gta_sa.exe'),'fixture_scope':'CDecision: disjoint valid objects and self assignment; no partial object overlap. GetTaskType: direct entry, GTA vtable, compiler virtual dispatch; 256 valid field combinations. No collaborator behavior accreditation.'}
assert manifest['exe_sha256']==EXE_HASH
try:
 p=ROOT/paths[0];s=saved[paths[0]].decode('utf-8');anchor='void FrontendIdle() {';assert s.count(anchor)==1;s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/'LiveComparison.hpp').as_posix()+'"',1).replace(anchor,anchor+'\n    RequestedLeafLive::Update();');p.write_text(s,encoding='utf-8')
 p=ROOT/paths[1];s=saved[paths[1]].decode('utf-8').replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(ROOT/'reversing/ProbeRuntime.hpp').as_posix()+'"',1).replace('        LoadConfigurations();','        ReversingProbe::InstallFailureCapture("requested-leaf");\n        LoadConfigurations();');p.write_text(s,encoding='utf-8')
 manifest['compiled_instruments']={p:digest(ROOT/p) for p in paths}
 task_env=os.environ.copy();task_env['_CL_']='/MP6';cmake=next((ROOT/'reversing/CVehicle/conan-cache/p').glob('cmake*/p/bin/cmake.exe'))
 with (ART/'build.log').open('w',encoding='utf-8') as log:subprocess.run([str(cmake),'--build',str(ROOT/'build'),'--config','Debug','--parallel','1'],stdout=log,stderr=subprocess.STDOUT,env=task_env,check=True)
 for p,h in manifest['tested_sources'].items():assert digest(ROOT/p)==h
 for p,h in manifest['bank_sources'].items():assert digest(HERE/p)==h
 for ext,p in bins.items():target=ART/f'gta_reversed-requested-leaf.{ext}';shutil.copy2(p,target);manifest[ext]=digest(target)
 (ART/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8');print('Build and source freeze verified')
finally:
 for p,data in saved.items():(ROOT/p).write_bytes(data)
 for ext,p in bins.items():shutil.copy2(ART/f'before.{ext}',p)
 assert all((ROOT/p).read_bytes()==data for p,data in saved.items())
