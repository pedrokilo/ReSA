import hashlib,json,shutil,subprocess,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import digest
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
CMAKE=ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'
TARGETS={
'source/game_sa/Pad.cpp':[(0,'void CPad::UpdateMouse() {'),(1,'void CPad::ProcessPad(ePadID padID) {')],
'source/game_sa/Frontend/MenuManager_Input.cpp':[(2,'void CMenuManager::CheckForMenuClosing() {')],
'source/app/platform/win/WinPs.cpp':[(3,'RwBool psInitialize() {'),(4,'void psTerminate() {')],
'source/app/platform/win/WinMain.cpp':[(5,'INT WINAPI NOTSA_WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) {'),(6,'void MessageLoop() {')],
'source/app/platform/SDLWrapper.cpp':[],
'source/game_sa/Game.cpp':[],
'source/dllmain.cpp':[]}
def main():
 saved={ROOT/f:(ROOT/f).read_bytes() for f in TARGETS};artifacts=HERE/'artifacts'
 try:
  for p,data in saved.items():
   (artifacts/(p.name+'.before')).write_bytes(data);s=data.decode().replace('\r\n','\n');header=HERE/('LiveComparison.hpp' if p.name=='Game.cpp' else 'Probe.hpp');s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+header.as_posix()+'"')
   for index,needle in TARGETS[p.relative_to(ROOT).as_posix()]:
    assert s.count(needle)==1,needle;s=s.replace(needle,needle+'\n ++PlatformProbe::calls['+str(index)+'];')
   if p.name=='Game.cpp':s=s.replace('void CGame::Process() {','void CGame::Process() {\n PlatformLive::Update();')
   if p.name=='SDLWrapper.cpp':s=s.replace('    pumping = false;','    pumping = false;\n    PlatformProbe::Tick();').replace('            ImGui_ImplSDL3_ProcessEvent(&event);','            ++PlatformProbe::uiEvents;\n            ImGui_ImplSDL3_ProcessEvent(&event);')
   if p.name=='dllmain.cpp':
    s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(ROOT/'reversing/ProbeRuntime.hpp').as_posix()+'"');s=s.replace('        LoadConfigurations();','        ReversingProbe::InstallFailureCapture("unlock-platform");\n        LoadConfigurations();');s=s.replace('        ApplyCommandLineHookSettings();','        PlatformProbe::Configure();\n        ApplyCommandLineHookSettings();')
   p.write_text(s,newline='\n')
  with (artifacts/'probe-build.log').open('w') as log:subprocess.run([str(CMAKE),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
  hashes={}
  for ext in ('asi','pdb'):
   dest=artifacts/f'gta_reversed-unlock-platform-probe.{ext}';shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',dest);hashes[ext]=digest(dest)
  dependencies=['source/game_sa/MenuManager.cpp','source/game_sa/RenderWare/rw/skeleton.h','source/app/platform/SDLWrapper.hpp','source/app/platform/win/WinInput.cpp','source/app/platform/win/WinInput.h','source/toolsmenu/UIRenderer.cpp','source/game_sa/ControllerConfigManager.cpp','source/app/platform/win/WindowedMode.cpp','source/app/app_debug.cpp','source/game_sa/MouseControllerState.h']
  m=dict(binaries=hashes,sources={str(p.relative_to(ROOT)):hashlib.sha256(data).hexdigest() for p,data in saved.items()},dependencies={f:digest(ROOT/f) for f in dependencies},instrumentation={p.name:digest(p) for p in (HERE/'Probe.hpp',HERE/'LiveComparison.hpp',HERE/'build_probe.py',ROOT/'reversing/ProbeRuntime.hpp')})
  (artifacts/'build-manifest.json').write_text(json.dumps(m,indent=2));print(json.dumps(hashes))
 finally:
  for p,data in saved.items():p.write_bytes(data)
if __name__=='__main__':main()
