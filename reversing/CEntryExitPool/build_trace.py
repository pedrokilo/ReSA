import sys,shutil,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'reversing'))
from runtime_runner import digest
paths=[ROOT/'source/game_sa/EntryExitManager.cpp',ROOT/'source/game_sa/Game.cpp',ROOT/'source/dllmain.cpp']
saved={p:p.read_bytes() for p in paths};art=HERE/'artifacts/trace';art.mkdir(parents=True,exist_ok=True)
try:
 for p,data in saved.items():
  (art/(p.name+'.before')).write_bytes(data)
  s=data.decode('utf-8').replace('\r\n','\n')
  if p.name=='EntryExitManager.cpp':s=s.replace('new CEntryExitsPool(512u,', 'new CEntryExitsPool(400u,')
  header=ROOT/'reversing/ProbeRuntime.hpp' if p.name=='dllmain.cpp' else HERE/'Trace.hpp'
  s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+header.as_posix()+'"')
  if p.name=='dllmain.cpp':s=s.replace('        LoadConfigurations();','        ReversingProbe::InstallFailureCapture("entryexit-trace");\n        LoadConfigurations();')
  elif p.name=='Game.cpp':s=s.replace('void CGame::Process() {','void CGame::Process() {\n    EETrace::Update();')
  else:
   args='centerX, centerY, centerZ, entranceAngle, entranceRangeX, entranceRangeY, entranceRangeZ, exitX, exitY, exitZ, exitAngle, area, flags, skyColor, timeOn, timeOff, numberOfPeds, name'
   s=s.replace('    const auto ptr = mp_poolEntryExits->New();','    const auto before=mp_poolEntryExits->GetNoOfUsedSpaces();\n    if(EETrace::Native()){EETrace::Original guard("AddOne");auto r=reinterpret_cast<decltype(&AddOne)>(0x43FA00)('+args+');EETrace::Added(before,centerX,centerY,flags,r);return r;}\n    const auto ptr = mp_poolEntryExits->New();\n    if(!ptr)EETrace::Added(before,centerX,centerY,flags,EETrace::FailureReturn());')
   s=s.replace('    return mp_poolEntryExits->GetIndex(enex);','    EETrace::Added(before,centerX,centerY,flags,mp_poolEntryExits->GetIndex(enex));\n    return mp_poolEntryExits->GetIndex(enex);',1)
   s=s.replace('void CEntryExitManager::DeleteOne(int32 index) {','void CEntryExitManager::DeleteOne(int32 index) {\n    EETrace::Delete(index);\n    if(EETrace::Native()){EETrace::Original guard("DeleteOne");reinterpret_cast<decltype(&DeleteOne)>(0x43FD50)(index);return;}')
   s=s.replace('int32 CEntryExitManager::FindNearestEntryExit(const CVector2D& position, float range, int32 ignoreArea) {','int32 CEntryExitManager::FindNearestEntryExit(const CVector2D& position, float range, int32 ignoreArea) {\n    if(EETrace::Native()){EETrace::Original guard("FindNearestEntryExit");auto r=reinterpret_cast<decltype(&FindNearestEntryExit)>(0x43F4B0)(position,range,ignoreArea);EETrace::Found(position,r);return r;}')
   s=s.replace('    return closest\n        ? mp_poolEntryExits->GetIndex(closest)','    EETrace::Found(position,closest?mp_poolEntryExits->GetIndex(closest):-1);\n    return closest\n        ? mp_poolEntryExits->GetIndex(closest)')
  p.write_text(s,encoding='utf-8',newline='\n')
 with (art/'trace-build.log').open('w') as log:subprocess.run([str(ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
 hashes={}
 for ext in ('asi','pdb'):
  dest=art/f'gta_reversed-entryexit-trace-probe.{ext}';shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',dest);hashes[ext]=digest(dest)
 (art/'trace-manifest.json').write_text(json.dumps(hashes,indent=2));print(hashes)
finally:
 for p,data in saved.items():p.write_bytes(data)
