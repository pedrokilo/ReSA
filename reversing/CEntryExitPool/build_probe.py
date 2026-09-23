import sys,shutil,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'reversing'))
from runtime_runner import digest
paths=[ROOT/'source/game_sa/EntryExitManager.cpp',ROOT/'source/game_sa/Game.cpp',ROOT/'source/dllmain.cpp']
saved={p:p.read_bytes() for p in paths};art=HERE/'artifacts';art.mkdir(exist_ok=True)
try:
 for p,data in saved.items():
  (art/(p.name+'.before')).write_bytes(data)
  s=data.decode('utf-8').replace('\r\n','\n')
  header=ROOT/'reversing/ProbeRuntime.hpp' if p.name=='dllmain.cpp' else HERE/('LiveComparison.hpp' if p.name=='Game.cpp' else 'Probe.hpp')
  s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+header.as_posix()+'"')
  if p.name=='dllmain.cpp':s=s.replace('        LoadConfigurations();','        ReversingProbe::InstallFailureCapture("entryexit-pool");\n        LoadConfigurations();')
  elif p.name=='Game.cpp':s=s.replace('void CGame::Process() {','void CGame::Process() {\n    EELive::Update();')
  else:
   s=s.replace('    const auto ptr = mp_poolEntryExits->New();','    ++EEProbe::adds;\n    const auto ptr = mp_poolEntryExits->New();')
   s=s.replace('void CEntryExitManager::DeleteOne(int32 index) {','void CEntryExitManager::DeleteOne(int32 index) {\n    ++EEProbe::deletes;')
  p.write_text(s,encoding='utf-8',newline='\n')
 with (art/'probe-build.log').open('w') as log:subprocess.run([str(ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
 hashes={}
 for ext in ('asi','pdb'):
  dest=art/f'gta_reversed-entryexit-pool-probe.{ext}';shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',dest);hashes[ext]=digest(dest)
 (art/'build-manifest.json').write_text(json.dumps(dict(binaries=hashes,sources={str(p.relative_to(ROOT)):__import__('hashlib').sha256(data).hexdigest() for p,data in saved.items()}),indent=2));print(hashes)
finally:
 for p,data in saved.items():p.write_bytes(data)
