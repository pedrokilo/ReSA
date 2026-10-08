from pathlib import Path
p=Path('reversing/HandSignal355');p.mkdir(exist_ok=True)
s=Path('reversing/GangJoin355/LiveComparison.hpp').read_text()
s=s.replace('GangJoin355Live','HandSignal355Live').replace('gang-join355','hand-signal355').replace('TaskComplexGangJoinRespond','TaskComplexPlayHandSignalAnim').replace('#include "TaskComplexGangLeader.h"','#include "TaskComplexSequence.h"')
a=s.index('inline Fn fns[]');b=s.index('alignas(16)',a)
s=s[:a]+'''inline Fn fns[]{
{"CTaskComplexPlayHandSignalAnim","GetAnimIdForPed",0x61B460,0,-1,256},
{"CTaskComplexPlayHandSignalAnim","CreateSubTask",0x61B2F0,0,-1,256},
{"CTaskComplexPlayHandSignalAnim","Clone",0x61BA00,0x86D5DC,1,256},
{"CTaskComplexPlayHandSignalAnim","CreateNextSubTask",0x61B570,0x86D5DC,8,256},
{"CTaskComplexPlayHandSignalAnim","CreateFirstSubTask",0x61B4F0,0x86D5DC,9,512},
{"CTaskComplexPlayHandSignalAnim","ControlSubTask",0x61B580,0x86D5DC,10,1024}};
'''+s[b:]
s=s.replace('std::array<uint8,128> arena,subtask','std::array<uint8,512> arena,subtask')
s=s.replace('inline size_t allocated{};', 'inline size_t allocated{};inline std::array<size_t,8> sizes{};inline int numAlloc{};')
s=s.replace('inline void* __cdecl New(size_t n){allocated=n;trace.push_back({"new",n});return arena.data();}', 'inline void* __cdecl New(size_t n){auto*p=arena.data()+numAlloc*64;allocated=n;sizes[numAlloc++]=n;trace.push_back({"new",n,Ptr(uintptr_t(p))});return p;}')
a=s.index('inline bool __cdecl Load()');b=s.index('inline ReversibleHooks::',a)
s=s[:a]+'''inline bool __stdcall Arm(eIKArm arm,CPed*){bool b=(scenario&1)!=0;trace.push_back({"arm",int(arm),b});return b;}
inline int __cdecl Block(const char* name){trace.push_back({"block",name});return 0;}
inline void __cdecl Add(int index){trace.push_back({"add_ref",index});}
inline void __cdecl Request(int index,int flags){trace.push_back({"request",index,flags});}
inline void __fastcall ModelRef(CBaseModelInfo*,void*){trace.push_back({"model_ref"});}
inline int __cdecl Random(){trace.push_back({"rand"});return scenario*701%32768;}
inline CTask* __fastcall Sub(CTaskComplexPlayHandSignalAnim*,void*,int type){trace.push_back({"sub",type});return reinterpret_cast<CTask*>(arena.data());}
'''+s[b:]
s=s.replace('const auto savedLoaded=block.IsLoaded;', 'const auto savedLoaded=block.IsLoaded;const auto savedBlock=CTaskComplexPlayHandSignalAnim::ms_animBlock;const int models[]{MODEL_SHANDL,MODEL_SHANDR,MODEL_FHANDL,MODEL_FHANDR};std::array<eStreamingLoadState,4> savedStates;for(int k=0;k<4;++k)savedStates[k]=CStreaming::GetInfo(models[k]).m_LoadState;')
a=s.index('EntryPatch pe(');b=s.index('for(int i=0;',a)
s=s[:a]+'''EntryPatch ike(0x6182B0,Arm),ikd(bit_cast<uintptr_t>(&IKChainManager_c::IsArmPointing),Arm);
EntryPatch be(0x4D3990,Block),bd(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),Block);
EntryPatch ae(0x4D3FB0,Add),ad(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),Add);
EntryPatch qe(0x4087E0,Request),qd(bit_cast<uintptr_t>(&CStreaming::RequestModel),Request);
EntryPatch me(0x4C4BA0,ModelRef),md(bit_cast<uintptr_t>(&CBaseModelInfo::AddRef),ModelRef);
EntryPatch rne(0x821B1E,Random),rnd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
std::optional<EntryPatch> sube,subd;if(fn.slot==9||fn.slot==10){sube.emplace(0x61B2F0,Sub);subd.emplace(bit_cast<uintptr_t>(&CTaskComplexPlayHandSignalAnim::CreateSubTask),Sub);}
'''+s[b:]
s=s.replace('scenario=i%32;subType=i%3==0?0x190:i%3==1?TASK_COMPLEX_TURN_TO_FACE_ENTITY:TASK_SIMPLE_STAND_STILL;', 'scenario=i%128;subType=i%2?0xCB:0x1A9;')
s=s.replace('allocated=0;trace=', 'allocated=0;numAlloc=0;trace=')
s=s.replace('auto*q=reinterpret_cast<CPed*>(player.data());','')
a=s.index('t->m_response=');b=s.index('\nuintptr_t ret=',a)
s=s[:a]+'''t->m_animationId=static_cast<AnimationId>(i%3?-1:0x140);t->m_AnimBlenDelta=float(i%9)/4.f;t->m_bLeftHandLoaded=(scenario&2)!=0;t->m_bRightHandLoaded=(scenario&4)!=0;t->m_bAnimationLoaded=(scenario&8)!=0;t->m_DoUseFatHands=(scenario&16)!=0;p->m_nPedType=static_cast<ePedType>(i%20);Put<int16>(ped.data(),0x22,int16(i%5==0?103:i%5==1?105:i%5==2?107:104));block.IsLoaded=uint8((scenario&32)!=0);CTaskComplexPlayHandSignalAnim::ms_animBlock=(scenario&64)?uint32(-1):0;for(int k=0;k<4;++k)CStreaming::GetInfo(models[k]).m_LoadState=(i&(128<<k))?eStreamingLoadState::LOADSTATE_LOADED:eStreamingLoadState::LOADSTATE_NOT_LOADED;
'''+s[b:]
a=s.index('uintptr_t ret=');b=s.index('\nif(candidate)',a)
s=s[:a]+'''uintptr_t ret=fn.slot==1?uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(address)(t)):fn.slot<0?(std::string(fn.name)=="GetAnimIdForPed"?uintptr_t(reinterpret_cast<int(__stdcall*)(CPed*)>(address)(p)):uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int)>(address)(t,i%3==0?0xCB:i%3==1?0x1A9:12345))):uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,p));
'''+s[b:]
s=s.replace('auto address=*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=(address<0x400000||address>=0x1000000)==(candidate!=0);', 'auto address=fn.slot<0?fn.address:*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=fn.slot<0?ReversingProbe::JumpsToDll(fn.address)==(candidate!=0):(address<0x400000||address>=0x1000000)==(candidate!=0);')
s=s.replace('result[candidate]={{"return",fn.slot==6?json(ret):Ptr(ret)},', 'json allocs=json::array();for(int k=0;k<numAlloc;++k)allocs.push_back(Words(arena.data()+64*k,sizes[k]));result[candidate]={{"return",std::string(fn.name)=="GetAnimIdForPed"?json(int32(ret)):Ptr(ret)},')
s=s.replace('allocated?Words(arena.data(),allocated):json(nullptr)', 'allocs')
s=s.replace('}block.IsLoaded=savedLoaded;', '}block.IsLoaded=savedLoaded;CTaskComplexPlayHandSignalAnim::ms_animBlock=savedBlock;for(int k=0;k<4;++k)CStreaming::GetInfo(models[k]).m_LoadState=savedStates[k];')
a=s.index('if(fn.slot==8){require(');b=s.index('Write({{"event","function_summary"}',a)
s=s[:a]+'''if(fn.slot==9){require("arm");require("sub");require("null");}if(fn.slot==10){require("request");require("add_ref");require("model_ref");require("sub");}'''+s[b:]
(p/'LiveComparison.hpp').write_text(s)
s=Path('reversing/GangJoin355/build_probe.py').read_text();a=s.index('SOURCES=');b=s.index("if __name__",a)
files=['Tasks/TaskTypes/TaskComplexPlayHandSignalAnim','Tasks/TaskTypes/TaskSimplePlayHandSignalAnim','Tasks/TaskTypes/TaskSimpleStandStill','Tasks/TaskComplexSequence','Tasks/TaskTimer','General']
s=s[:a]+'SOURCES='+repr(['source/game_sa/'+x+ext for x in files for ext in ('.cpp','.h')])+'\n'+s[b:];s=s.replace('gang-join355','hand-signal355').replace('GangJoin355Live','HandSignal355Live');(p/'build_probe.py').write_text(s)
s=Path('reversing/GangJoin355/run_local.py').read_text().replace('gang-join355','hand-signal355').replace('GangJoinRespond pendientes','PlayHandSignalAnim pendientes');(p/'run_local.py').write_text(s)
(p/'README.md').write_text('''# CTaskComplexPlayHandSignalAnim: seis pendientes

Banco previsto de2.560pares: GetAnimId256, CreateSub256, Clone256, Next256, First512, Control1024. Pendiente de compilación y ejecución. Evidencia combo en ../Tasks355Evidence.

GetAnimId es stdcall y preserva EDX (el caller del EXE usa su valor tras la llamada). First detecta brazo ocupado, pandilla y modelos103/105/107. Control cubre carga/referencias de cuatro modelos de manos y bloque ghands; cada global se restaura. CreateSub ejercita construcción real de secuencia con dos tareas, aleatorio determinista y memoriaCD; se comparan asignaciones completas salvo vptr. La carga y referencias externas se aíslan; no reproduce animación/streaming reales.
''',encoding='utf-8')
