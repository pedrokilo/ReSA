from pathlib import Path
p=Path('reversing/CameraDuck355');p.mkdir(exist_ok=True)
for name in ['build_probe.py','run_local.py']:
 s=Path('reversing/CameraLinear355',name).read_text(encoding='utf-8-sig').replace('CameraLinear355','CameraDuck355').replace('camera-linear355','camera-duck355');(p/name).write_text(s,encoding='utf8')
s=Path('reversing/DecisionPending/LiveComparison.hpp').read_text().replace('DecisionPendingLive','CameraDuck355Live').replace('decision-pending','camera-duck355').replace('#include "DecisionMakers/Decision.h"','#include "Camera.h"\n#include "PedIntelligence.h"\n#include "Tasks/TaskTypes/TaskSimpleDuck.h"')
start=s.index('inline const std::array<Fn,2>');end=s.index('inline ReversibleHooks',start)
s=s[:start]+'''inline const std::array<Fn,2> fns{{
{"CCamera","HandleCameraMotionForDucking",0x50CFA0,0,-1,1024},
{"CCamera","HandleCameraMotionForDuckingDuringAim",0x50D090,0,-1,1024}
}};
'''+s[end:]
s=s.replace('inline bool done{};', '''inline json trace;inline uint32 scenario{};alignas(CTaskSimpleDuck) inline std::array<uint8,sizeof(CTaskSimpleDuck)> task{};
inline CTaskSimpleDuck* __fastcall Duck(CPedIntelligence*,void*,bool simple){trace.push_back({"duck",simple});return scenario%3==0?nullptr:reinterpret_cast<CTaskSimpleDuck*>(task.data());}
inline bool done{};''').replace('inline bool done{}; inline std::ofstream log; inline json trace;','inline bool done{}; inline std::ofstream log;')
s=s.replace(' bool passed=true;uint32 pairCount=0;',''' bool passed=true;uint32 pairCount=0;
 const auto savedStep=CTimer::GetTimeStep(),savedStill=*reinterpret_cast<float*>(0x8CCB94),savedMoving=*reinterpret_cast<float*>(0x8CCB98);
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x6010A0,Duck),ad(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskDuck),Duck);''')
start=s.index('    alignas(CDecision)');end=s.index('\n   };',start)
s=s[:start]+'''    alignas(CCamera) std::array<uint8,sizeof(CCamera)> camera{};camera.fill(uint8(i*37));auto* self=reinterpret_cast<CCamera*>(camera.data());
    alignas(CPed) std::array<uint8,sizeof(CPed)> raw{};auto* ped=reinterpret_cast<CPed*>(raw.data());*reinterpret_cast<void**>(raw.data()+0x47C)=reinterpret_cast<void*>(0x123400);ped->bIsDucking=(i/3)%2!=0;
    task.fill(0);task[0x19]=uint8((i/6)%2);scenario=uint32(i);trace=json::array();
    constexpr std::array<float,6> speeds{0.f,.0001f,.001f,.0010001f,.125f,.5f};ped->m_vecMoveSpeed=CVector{speeds[(i/12)%6],speeds[(i/24)%6],speeds[(i/48)%6]};
    *reinterpret_cast<float*>(0xB7CB5C)=float(i%9)*.125f;*reinterpret_cast<float*>(0x8CCB94)=float(i%7)*.125f;*reinterpret_cast<float*>(0x8CCB98)=float(i%11)*.0625f;
    self->m_fDuckCamMotionFactor=float(i%13-6)*.0625f;self->m_fDuckAimCamMotionFactor=float(i%17-8)*.0625f;
    CVector source{1.f,2.f,float(i)*.125f},target{-1.f,-2.f,-float(i)*.0625f};
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__thiscall*)(CCamera*,CPed*,CVector*,CVector*,bool)>(fn.address)(self,ped,i%4==0?nullptr:&source,i%5==0?nullptr:&target,(i/96)%2!=0);
    return json{{"camera",ReversingProbe::Bytes(camera.data()+0xC54,8)},{"source",ReversingProbe::Bytes(&source,sizeof(source))},{"target",ReversingProbe::Bytes(&target,sizeof(target))},{"ped",ReversingProbe::Bytes(raw.data(),raw.size())},{"trace",trace},{"switches",switched}};''' +s[end:]
s=s.replace(' Write({{"event",passed?', ' *reinterpret_cast<float*>(0xB7CB5C)=savedStep;*reinterpret_cast<float*>(0x8CCB94)=savedStill;*reinterpret_cast<float*>(0x8CCB98)=savedMoving;\n Write({{"event",passed?')
(p/'LiveComparison.hpp').write_text(s,encoding='utf8')
