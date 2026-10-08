from pathlib import Path
p=Path('reversing/ScriptShutdown355');p.mkdir(exist_ok=True)
for name in ['build_probe.py','run_local.py']:
 s=Path('reversing/ProjectileCore355',name).read_text(encoding='utf-8-sig').replace('ProjectileCore355','ScriptShutdown355').replace('projectile-core355','script-shutdown355')
 if name=='build_probe.py':s=s.replace("'source/game_sa/ProjectileInfo.cpp', 'source/game_sa/ProjectileInfo.h', 'source/game_sa/Entity/Object/Projectile.h'","'source/game_sa/Scripts/RunningScript.cpp', 'source/game_sa/Scripts/RunningScript.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h'")
 (p/name).write_text(s,encoding='utf8')
s=Path('reversing/DecisionPending/LiveComparison.hpp').read_text().replace('DecisionPendingLive','ScriptShutdown355Live').replace('decision-pending','script-shutdown355').replace('#include "DecisionMakers/Decision.h"','#include "Scripts/RunningScript.h"\n#include "StreamedScripts.h"\n#include "Tasks/ScriptedBrainTaskStore.h"\n#include "Pools/PedPool.h"\n#include "Pools/ObjectPool.h"')
start=s.index('inline const std::array<Fn,2>');end=s.index('inline ReversibleHooks',start)
s=s[:start]+'inline const std::array<Fn,1> fns{{{"CRunningScript","ShutdownThisScript",0x465AA0,0,-1,1024}}};\n'+s[end:]
s=s.replace('inline bool done{};', '''inline json trace;inline uint32 scenario{},allocations{};alignas(16) inline std::array<uint8,8> task{};
inline uint32 __fastcall Index(CStreamedScripts*,void*,uint8* ip){trace.push_back({"index",uintptr_t(ip)});return scenario%4;}
inline void* __cdecl NewTask(size_t size){++allocations;return scenario%3==0?nullptr:task.data();}
inline CTask* __cdecl SetTask(CPed* p,CTask* t){trace.push_back({"set_task",uintptr_t(p),t!=nullptr,t?*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(t)+4):0});return nullptr;}
inline bool done{};''').replace('inline bool done{}; inline std::ofstream log; inline json trace;','inline bool done{}; inline std::ofstream log;')
s=s.replace(' bool passed=true;uint32 pairCount=0;',''' bool passed=true;uint32 pairCount=0;
 auto& pedPool=*reinterpret_cast<CPedPool**>(0xB74490);auto* savedPedPool=pedPool;auto& objectPool=*reinterpret_cast<CObjectPool**>(0xB7449C);auto* savedObjectPool=objectPool;
 std::array<uint8,128> savedStream{};memcpy(savedStream.data(),reinterpret_cast<void*>(0xA47B60),128);const auto savedLocal=*reinterpret_cast<int32*>(0xA48960);
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x470910,Index),ad(bit_cast<uintptr_t>(&CStreamedScripts::GetStreamedScriptWithThisStartAddress),Index);
 EntryPatch b(0x635720,SetTask),bd(bit_cast<uintptr_t>(&CScriptedBrainTaskStore::SetTask),SetTask);
 EntryPatch c(0x61A5A0,NewTask),cd(bit_cast<uintptr_t>(&CTask::operator new),NewTask);''')
start=s.index('    alignas(CDecision)');end=s.index('\n   };',start)
s=s[:start]+'''    alignas(CRunningScript) std::array<uint8,sizeof(CRunningScript)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CRunningScript*>(raw.data());
    alignas(16) std::array<uint8,0x7C4*4> peds{};alignas(16) std::array<uint8,0x19C*4> objects{};
    std::array<uint8,4> pedStates{},objectStates{};alignas(16) std::array<uint8,sizeof(CPedPool)> fakePedPool{};alignas(16) std::array<uint8,sizeof(CObjectPool)> fakeObjectPool{};
    At<void*>(fakePedPool.data(),0)=peds.data();At<void*>(fakePedPool.data(),4)=pedStates.data();At<int32>(fakePedPool.data(),8)=4;At<void*>(fakeObjectPool.data(),0)=objects.data();At<void*>(fakeObjectPool.data(),4)=objectStates.data();At<int32>(fakeObjectPool.data(),8)=4;
    for(size_t k=0;k<4;++k){pedStates[k]=uint8(i+k);objectStates[k]=uint8(i+k);At<uint32>(peds.data()+0x7C4*k,0x474)=0xFFFFFFFF;At<uint32>(objects.data()+0x19C*k,0x140)=0xFFFFFFFF;}
    pedPool=reinterpret_cast<CPedPool*>(fakePedPool.data());objectPool=reinterpret_cast<CObjectPool*>(fakeObjectPool.data());
    self->m_IsActive=true;self->m_IsExternal=(i/4)%2!=0;self->m_ExternalType=int8(i%8);self->m_ThisMustBeTheOnlyMissionRunning=(i/8)%2!=0;self->m_BaseIP=reinterpret_cast<uint8*>(0x123450);
    const auto index=(i/16)%4;const int32 reference=index*256+int32(uint8(i+index+(i%3==0?1:0)));self->m_LocalVars[0].iParam=reference;*reinterpret_cast<int32*>(0xA48960)=reference;
    memset(reinterpret_cast<void*>(0xA47B60),uint8(i),128);scenario=uint32(i);trace=json::array();allocations=0;task.fill(0xCD);
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CRunningScript*)>(fn.address)(self);
    return json{{"bytes",ReversingProbe::Bytes(raw.data(),raw.size())},{"peds",ReversingProbe::Bytes(peds.data(),peds.size())},{"objects",ReversingProbe::Bytes(objects.data(),objects.size())},{"stream",ReversingProbe::Bytes(reinterpret_cast<void*>(0xA47B60),128)},{"allocations",allocations},{"task_parent",ReversingProbe::Bytes(task.data()+4,4)},{"trace",trace},{"switches",switched}};''' +s[end:]
s=s.replace(' Write({{"event",passed?', ' pedPool=savedPedPool;objectPool=savedObjectPool;memcpy(reinterpret_cast<void*>(0xA47B60),savedStream.data(),128);*reinterpret_cast<int32*>(0xA48960)=savedLocal;\n Write({{"event",passed?')
(p/'LiveComparison.hpp').write_text(s,encoding='utf8')
