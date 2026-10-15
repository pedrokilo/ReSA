"""Build temporary instrumentation and restore sources even when compilation fails."""
import hashlib
import json
import shutil
import subprocess
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import digest
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CMAKE = ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'

def main():
    paths = [ROOT/'source/game_sa/Entity/Ped/Ped.cpp', ROOT/'source/game_sa/Game.cpp']
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        for p, data in saved.items():
            (artifacts/(p.name+'.before')).write_bytes(data)
            content = data.decode('utf-8').replace('\r\n', '\n')
            header = 'CompletionProbe.hpp' if p.name == 'Ped.cpp' else 'CompletionLive.hpp'
            anchor = 'void CPed::CalculateNewVelocity() {' if p.name == 'Ped.cpp' else 'void CGame::Process() {'
            injected = '\n    ++PedCompletionProbe::calls[0];' if p.name == 'Ped.cpp' else '\n    PedCompletion::Update();'
            if content.count(anchor) != 1 or content.count('#include "StdInc.h"') != 1:
                raise RuntimeError('Instrumentation anchor missing or duplicated: '+str(p))
            content = content.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../reversing/CPed/'+header+'"').replace(anchor, anchor+injected)
            if p.name == 'Ped.cpp':
                content = content.replace('../../reversing/CPed/CompletionProbe.hpp', '../../../../reversing/CPed/CompletionProbe.hpp')
            if p.name == 'Ped.cpp':
                signature='void CPed::DoFootLanded(bool leftFoot, uint8 arg1) {'
                assert content.count(signature)==1
                content=content.replace(signature,signature+'\n    ++PedCompletionProbe::calls[1];')
                begin=content.index(signature); end=content.index('void CPed::PlayFootSteps()',begin)
                fragment=content[begin:end]
                substitutions={
                    'GetBonePosition(&position, leftFoot ? BONE_L_FOOT : BONE_R_FOOT, false)': 'plugin::CallMethod<0x5E4280>(this, &position, leftFoot ? BONE_L_FOOT : BONE_R_FOOT, false)',
                    'EmitFootRainParticles(this, &position);': 'if (PedCompletionProbe::footEmit[0]) PedCompletionProbe::footEmit[0](this, &position); else EmitFootRainParticles(this, &position);',
                    'EmitFootDustParticles(this, &position);': 'if (PedCompletionProbe::footEmit[1]) PedCompletionProbe::footEmit[1](this, &position); else EmitFootDustParticles(this, &position);',
                    'CLocalisation::Blood()': 'plugin::CallAndReturn<bool, 0x56D230>()',
                    'g_surfaceInfos.LeavesFootsteps(m_nContactSurface)': 'plugin::CallMethodAndReturn<bool, 0x55E890>(&g_surfaceInfos, uint32(m_nContactSurface))',
                    'g_surfaceInfos.IsShallowWater(m_nContactSurface)': 'plugin::CallMethodAndReturn<bool, 0x55E730>(&g_surfaceInfos, uint32(m_nContactSurface))',
                    'CCullZones::CamNoRain()': 'plugin::CallAndReturn<bool, 0x72DDB0>()',
                    'CCullZones::PlayerNoRain()': 'plugin::CallAndReturn<bool, 0x72DDC0>()',
                    'CWaterLevel::GetWaterLevel(pos.x, pos.y, pos.z + 1.5f, level, true, nullptr)': 'plugin::CallAndReturn<bool, 0x6EB690, float, float, float, float&, uint8, CVector*>(pos.x, pos.y, pos.z + 1.5f, level, true, nullptr)',
                    'g_fx.TriggerFootSplash(splash)': 'plugin::CallMethod<0x4A1150, Fx_c*, const CVector&>(&g_fx, splash)',
                    'm_pedAudio.AddAudioEvent(AE_PED_FOOTSTEP_LEFT, 0.0f, 1.0f, nullptr, eSurfaceType(39), 0, 0)': 'plugin::CallMethod<0x4E2BB0>(&m_pedAudio, AE_PED_FOOTSTEP_LEFT, 0.0f, 1.0f, nullptr, eSurfaceType(39), 0, 0)',
                }
                for old,new in substitutions.items():
                    assert fragment.count(old)==1,old
                    fragment=fragment.replace(old,new)
                content=content[:begin]+fragment+content[end:]

            if p.name == 'Ped.cpp':
                begin=content.index('void CPed::PlayFootSteps() {'); end=content.index('void CPed::AddWeaponModel',begin)
                fragment=content[begin:end].replace('void CPed::PlayFootSteps() {','void CPed::PlayFootSteps() {\n    ++PedCompletionProbe::calls[2];')
                substitutions={
                    'RpAnimBlendClumpGetFirstAssociation(GetRpClump())':'plugin::CallAndReturn<CAnimBlendAssociation*, 0x4D15E0>(GetRpClump())',
                    'RpAnimBlendGetNextAssociation(anim)':'plugin::CallAndReturn<CAnimBlendAssociation*, 0x4D6AB0>(anim)',
                    'land->RightFootLanded()':'plugin::CallMethodAndReturn<bool, 0x678FE0>(land)',
                    'land->LeftFootLanded()':'plugin::CallMethodAndReturn<bool, 0x679010>(land)',
                    'DoFootLanded(':'plugin::CallMethod<0x5E5380>(this, ',
                    'g_surfaceInfos.GetAdhesionGroup(m_nContactSurface)':'plugin::CallMethodAndReturn<uint32, 0x55E5C0>(&g_surfaceInfos, uint32(m_nContactSurface))',
                    'CGeneral::GetRandomNumber()':'plugin::CallAndReturn<int, 0x821B1E>()',
                    'data->m_pPedClothesDesc->GetIsWearingBalaclava()':'plugin::CallMethodAndReturn<bool, 0x5A7950>(data->m_pPedClothesDesc)',
                    'GetEventGlobalGroup()->Add(&event)':'plugin::CallMethodAndReturn<CEvent*, 0x4AB420>(GetEventGlobalGroup(), &event, false)',
                    'm_pedAudio.AddAudioEvent(AE_PED_SKATE_LEFT, AudioLog10Times20Cpp(adhesion), speed)':'plugin::CallMethod<0x4E2BB0>(&m_pedAudio, AE_PED_SKATE_LEFT, AudioLog10Times20Cpp(adhesion), speed, nullptr, SURFACE_DEFAULT, 0, 0)',
                    'm_pedAudio.AddAudioEvent(AE_PED_SKATE_RIGHT, AudioLog10Times20Cpp(adhesion), speed)':'plugin::CallMethod<0x4E2BB0>(&m_pedAudio, AE_PED_SKATE_RIGHT, AudioLog10Times20Cpp(adhesion), speed, nullptr, SURFACE_DEFAULT, 0, 0)',
                    'm_pedAudio.AddAudioEvent(left ? AE_PED_FOOTSTEP_LEFT : AE_PED_FOOTSTEP_RIGHT, volume, speed)':'plugin::CallMethod<0x4E2BB0>(&m_pedAudio, left ? AE_PED_FOOTSTEP_LEFT : AE_PED_FOOTSTEP_RIGHT, volume, speed, nullptr, SURFACE_DEFAULT, 0, 0)',
                }
                for old,new in substitutions.items():
                    assert old in fragment,old
                    fragment=fragment.replace(old,new)
                content=content[:begin]+fragment+content[end:]
            if p.name == 'Ped.cpp':
                begin=content.index('void CPed::MakeTyresMuddySectorList(PtrListType& ptrList)'); end=content.index('void CPed::DeadPedMakesTyresBloody()',begin)
                fragment=content[begin:end].replace('    CAutomobile* automobile = nullptr;','    ++PedCompletionProbe::calls[3];\n    CAutomobile* automobile = nullptr;')
                substitutions={
                    'CLocalisation::Blood()':'plugin::CallAndReturn<bool, 0x56D230>()',
                    'selected->m_vehicleAudio.AddAudioEvent(AE_PED_DRIVE_OVER, 0.0f)':'plugin::CallMethod<0x4F6420>(&selected->m_vehicleAudio, AE_PED_DRIVE_OVER, 0.0f)',
                    'FindPlayerVehicle(-1, false)':'plugin::CallAndReturn<CVehicle*, 0x56E0D0>(-1, false)',
                    'CPad::GetPad(0)->StartShake(300, 70, 0)':'plugin::CallMethod<0x53F920>(CPad::GetPad(0), int16(300), uint8(70), uint32(0))',
                }
                for old,new in substitutions.items():
                    assert old in fragment,old
                    fragment=fragment.replace(old,new)
                content=content[:begin]+fragment+content[end:]
            if p.name == 'Ped.cpp':
                begin=content.index('bool CPed::PositionPedOutOfCollision('); end=content.index('bool CPed::PositionAnyPedOutOfCollision()',begin)
                fragment=content[begin:end].replace('bool findClosestNode) {','bool findClosestNode) {\n    ++PedCompletionProbe::calls[4];')
                substitutions={
                    'vehicle->GetColModel()':'plugin::CallMethodAndReturn<CColModel*, 0x535300>(vehicle)',
                    'vehicle->IsOnItsSide()':'plugin::CallMethodAndReturn<bool, 0x6D1DD0>(vehicle)',
                    'CheckCollision()':'plugin::CallMethodAndReturn<bool, 0x54D920>(this)',
                    'CWorld::GetIsLineOfSightClear(origin, position, true, false, false, true, false, false, false)':'plugin::CallAndReturn<bool, 0x56A490, const CVector&, const CVector&, bool, bool, bool, bool, bool, bool, bool>(origin, position, true, false, false, true, false, false, false)',
                    'ThePaths.FindNodeClosestToCoors(origin, PATH_TYPE_PED, 999999.875f, 0, 0, 0, 0, 0)':'PedCompletionProbe::Closest(origin, PATH_TYPE_PED)',
                    'ThePaths.FindNodeClosestToCoors(origin, PATH_TYPE_VEH, 999999.875f, 0, 0, 0, 0, 0)':'PedCompletionProbe::Closest(origin, PATH_TYPE_VEH)',
                    'position = CPedPlacement::FindZCoorForPed(position).first':'plugin::Call<0x616920>(&position)',
                }
                for old,new in substitutions.items():
                    assert old in fragment,old
                    fragment=fragment.replace(old,new)
                content=content[:begin]+fragment+content[end:]
            if p.name == 'Ped.cpp':
                begin=content.index('void CPed::KillPedWithCar('); end=content.index('template<typename PtrListType>',begin)
                fragment=content[begin:end].replace('bool bPlayDeadAnimation) {','bool bPlayDeadAnimation) {\n    ++PedCompletionProbe::calls[5];')
                substitutions={
                    'tasks.GetSimplestActiveTask()':'plugin::CallMethodAndReturn<CTask*, 0x6819D0>(&tasks)',
                    'tasks.FindActiveTaskByType(TASK_COMPLEX_DESTROY_CAR_MELEE)':'plugin::CallMethodAndReturn<CTask*, 0x681740>(&tasks, TASK_COMPLEX_DESTROY_CAR_MELEE)',
                    'car->GetColModel()':'plugin::CallMethodAndReturn<CColModel*, 0x535300>(car)',
                    'CGeneral::GetRandomNumber()':'plugin::CallAndReturn<int, 0x821B1E>()',
                    'FindPlayerVehicle(-1, false)':'plugin::CallAndReturn<CVehicle*, 0x56E0D0>(-1, false)',
                    'CPad::GetPad(0)->StartShake(int16(40000 / frequency), frequency, 0)':'plugin::CallMethod<0x53F920>(CPad::GetPad(0), int16(40000 / frequency), frequency, uint32(0))',
                    'car->AsAutomobile()->RemoveBonnetInPedCollision()':'plugin::CallMethodAndReturn<CObject*, 0x6AA200>(car)',
                    'CLocalisation::KnockDownPeds()':'plugin::CallAndReturn<bool, 0x56D290>()',
                    'car->m_vehicleAudio.AddAudioEvent(bounce ? AE_PED_BOUNCE : AE_PED_CRUNCH, 0.0f)':'plugin::CallMethod<0x4F6420>(&car->m_vehicleAudio, bounce ? AE_PED_BOUNCE : AE_PED_CRUNCH, 0.0f)',
                    'car->m_vehicleAudio.AddAudioEvent(AE_PED_KNOCK_DOWN, 0.0f)':'plugin::CallMethod<0x4F6420>(&car->m_vehicleAudio, AE_PED_KNOCK_DOWN, 0.0f)',
                    'CCrime::ReportCrime(m_nPedType == PED_TYPE_COP ? CRIME_KILL_COP_PED_WITH_CAR : CRIME_KILL_PED_WITH_CAR, this, car->m_pDriver)':'plugin::Call<0x532010>(m_nPedType == PED_TYPE_COP ? CRIME_KILL_COP_PED_WITH_CAR : CRIME_KILL_PED_WITH_CAR, this, car->m_pDriver)',
                    'event.AffectsPed(this)':'plugin::CallMethodAndReturn<bool, 0x4B35A0>(&event, this)',
                    'calculator.ComputeDamageResponse(this, event.m_damageResponse, true)':'plugin::CallMethod<0x4B5AC0>(&calculator, this, &event.m_damageResponse, true)',
                    'CAnimManager::BlendAnimation(GetRpClump(), ANIM_GROUP_DEFAULT, anim, blend)':'plugin::CallAndReturn<CAnimBlendAssociation*, 0x4D4610>(GetRpClump(), ANIM_GROUP_DEFAULT, anim, blend)',
                    'CAnimManager::BlendAnimation(GetRpClump(), ANIM_GROUP_DEFAULT, event.m_nAnimID, 8.0f)':'plugin::CallAndReturn<CAnimBlendAssociation*, 0x4D4610>(GetRpClump(), ANIM_GROUP_DEFAULT, event.m_nAnimID, 8.0f)',
                    'GetEventGroup().Add(&event)':'plugin::CallMethodAndReturn<CEvent*, 0x4AB420>(&GetEventGroup(), &event, false)',
                    'Say(CTX_GLOBAL_PAIN_DEATH_HIGH)':'plugin::CallMethod<0x5EFFE0>(this, CTX_GLOBAL_PAIN_DEATH_HIGH, uint32(0), 1.0f, false, false, false)',
                    'Say(CTX_GLOBAL_PAIN_LOW)':'plugin::CallMethod<0x5EFFE0>(this, CTX_GLOBAL_PAIN_LOW, uint32(0), 1.0f, false, false, false)',
                    'car->ApplyForce(direction * force, point, true)':'plugin::CallMethod<0x542B50>(car, direction * force, point, true)',
                }
                for old,new in substitutions.items():
                    assert old in fragment,old
                    fragment=fragment.replace(old,new)
                content=content[:begin]+fragment+content[end:]
            p.write_text(content, encoding='utf-8', newline='\n')
        with (artifacts/'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        hashes = {}
        for ext in ('asi','pdb'):
            dest = artifacts/f'gta_reversed-cped-completion-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest); hashes[ext] = digest(dest)
        manifest = dict(binaries=hashes, sources={str(p.relative_to(ROOT)):hashlib.sha256(data).hexdigest() for p,data in saved.items()}, instrumentation={p.name:digest(p) for p in (*HERE.glob('*.hpp'),HERE/'build_completion_probe.py',HERE.parent/'ProbeSupport.hpp')})
        (artifacts/'build-manifest.json').write_text(json.dumps(manifest,indent=2))
        print(json.dumps(hashes))
    finally:
        for p,data in saved.items(): p.write_bytes(data)

if __name__ == '__main__': main()

