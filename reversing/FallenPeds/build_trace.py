"""Build a diagnostic probe; restore source files and normal build artifacts."""
import json
import hashlib
import re
import shutil
import subprocess
import sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
from runtime_runner import digest

def main():
    art = HERE / 'artifacts/trace'
    art.mkdir(parents=True, exist_ok=True)
    paths = [ROOT / ('source/' + p) for p in ('game_sa/World.cpp', 'game_sa/Entity/Placeable.cpp', 'game_sa/Entity/Ped/Ped.cpp', 'game_sa/PedIntelligence.cpp', 'game_sa/Entity/Physical.cpp', 'game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp', 'game_sa/Game.cpp', 'dllmain.cpp')]
    saved = {p: p.read_bytes() for p in paths}
    for ext in ('asi', 'pdb'):
        shutil.copy2(ROOT / f'bin/Debug/gta_reversed.{ext}', art / f'normal.{ext}')
    try:
        for p, data in saved.items():
            (art / (p.name + '.before')).write_bytes(data)
            s = data.decode('utf-8').replace('\r\n', '\n')
            header = HERE / 'Trace.hpp' if p.name != 'dllmain.cpp' else HERE.parent / 'ProbeRuntime.hpp'
            s = s.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "' + header.as_posix() + '"', 1)
            if p.name == 'dllmain.cpp':
                s = s.replace('        LoadConfigurations();', '        ReversingProbe::InstallFailureCapture("fallen-peds");\n        LoadConfigurations();', 1)
            elif p.name == 'Game.cpp':
                s = s.replace('void CGame::Process() {', 'void CGame::Process() {\n    FallenTrace::Update();', 1)
            elif p.name == 'Placeable.cpp':
                s = s.replace('void CPlaceable::SetPosn(float x, float y, float z) {', 'void CPlaceable::SetPosn(float x, float y, float z) {\n    FallenTrace::Input(this,CVector{x,y,z});', 1)
                s = s.replace('void CPlaceable::SetPosn(const CVector& posn) {', 'void CPlaceable::SetPosn(const CVector& posn) {\n    FallenTrace::Input(this,posn);', 1)
                s = s.replace('void CPlaceable::SetMatrix(CMatrix& matrix) {', 'void CPlaceable::SetMatrix(CMatrix& matrix) {\n    FallenTrace::Input(this,matrix.GetPosition());', 1)
            elif p.name == 'TaskSimpleRunNamedAnim.cpp':
                s = s.replace('void CTaskSimpleRunNamedAnim::OffsetPedPosition(CPed* ped) {', 'void CTaskSimpleRunNamedAnim::OffsetPedPosition(CPed* ped) {\n    FallenTrace::Write({{"event","named_offset"},{"name",m_animName},{"group",m_animGroupName},{"offset",FallenTrace::Vec(m_vecOffsetAtEnd)},{"flags",m_nFlags},{"ptr",reinterpret_cast<uintptr_t>(ped)}});\n    FallenTrace::Watch watch(ped,"CTaskSimpleRunNamedAnim::OffsetPedPosition");', 1)
            elif p.name in ('Ped.cpp', 'Physical.cpp', 'PedIntelligence.cpp'):
                cls, target, methods = {
                    'Ped.cpp': ('CPed', 'this', ('ProcessBuoyancy', 'PlayFootSteps', 'CalculateNewVelocity', 'UpdatePosition', 'SetMoveAnim', 'CalculateNewOrientation')),
                    'Physical.cpp': ('CPhysical', 'this', ('ProcessControl',)),
                    'PedIntelligence.cpp': ('CPedIntelligence', 'm_pPed', ('ProcessFirst', 'Process')),
                }[p.name]
                for method in methods:
                    pattern = r'(void ' + cls + '::' + method + r'\(\)\s*\{)'
                    s, count = re.subn(pattern, r'\1\n    FallenTrace::Watch traceWatch(' + target + ',"' + cls + '::' + method + '");', s, count=1)
                    assert count == 1, (p, method)
                if p.name == 'PedIntelligence.cpp':
                    for call in ('m_eventHandler.HandleEvents();', 'm_TaskMgr.ManageTasks();'):
                        s = s.replace(call, '{ FallenTrace::Watch watch(m_pPed,"' + call + '"); ' + call + ' }')
            else:
                for method in ('UpdateAnim', 'ProcessControl', 'ProcessCollision', 'ProcessShift'):
                    call = f'entity->{method}();'
                    s = s.replace(call, '{ FallenTrace::Watch watch(entity,"' + method + '"); ' + call + ' }')
                s = s.replace('ped->GetIntelligence()->ProcessAfterProcCol();', '{ FallenTrace::Watch watch(ped,"ProcessAfterProcCol"); ped->GetIntelligence()->ProcessAfterProcCol(); }')
                s = s.replace('        NOTSA_LOG_WARN("&&&&&&Another ped', '        FallenTrace::Fallen(ped);\n        NOTSA_LOG_WARN("&&&&&&Another ped', 1)
            p.write_text(s, encoding='utf-8', newline='\n')
        with (art / 'build.log').open('w') as log:
            subprocess.run([str(ROOT / 'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'), '--build', str(ROOT / 'build'), '--config', 'Debug'], stdout=log, stderr=subprocess.STDOUT, check=True)
        manifest = {'sources': {str(p.relative_to(ROOT)): digest(p) for p in paths},
                    'base_sources': {str(p.relative_to(ROOT)): hashlib.sha256(data).hexdigest() for p,data in saved.items()},
                    'trace_header': digest(HERE / 'Trace.hpp')}
        for ext in ('asi', 'pdb'):
            dest = art / f'gta_reversed-fallen-peds-probe.{ext}'
            shutil.copy2(ROOT / f'bin/Debug/gta_reversed.{ext}', dest)
            manifest[ext] = digest(dest)
        (art / 'manifest.json').write_text(json.dumps(manifest, indent=2))
        print(json.dumps(manifest))
    finally:
        for p, data in saved.items():
            p.write_bytes(data)
        for ext in ('asi', 'pdb'):
            shutil.copy2(art / f'normal.{ext}', ROOT / f'bin/Debug/gta_reversed.{ext}')

if __name__ == '__main__':
    main()
