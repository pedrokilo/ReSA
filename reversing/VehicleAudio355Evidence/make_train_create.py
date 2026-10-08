from pathlib import Path
p=Path('reversing/TrainCreate355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainPassenger355/LiveComparison.hpp').read_text().replace('TrainPassenger355Live','TrainCreate355Live').replace('train-passenger355','train-create355').replace('#include "Population.h"','#include "Population.h"\n#include <reversiblehooks/ReversibleHook/Simple.h>')
a=s.index('inline bool done{};');b=s.index('inline ReversibleHooks',a)
helper=Path('reversing/TrainCtor355/LiveComparison.hpp').read_text();x=helper.index('inline uintptr_t moduleBase');y=helper.index('inline void* __fastcall Base',x)
s=s[:a]+'''inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},used{};inline uint8* storage{};inline uint8* matrices{};inline uintptr_t* table{};inline CPed* driver{};
inline int Index(const void* ptr){return ptr?int((static_cast<const uint8*>(ptr)-storage)/0xA18):-1;}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
'''+helper[x:y]+'''inline void* __cdecl Allocate(unsigned size){trace.push_back({"allocate",size,used});return storage+(used++)*0xA18;}
inline void* __fastcall Constructor(void* raw,void*,int32 model,eVehicleCreatedBy by){const auto index=Index(raw);trace.push_back({"construct",index,model,int(by)});memset(raw,0xCD,0x6AC);auto* train=static_cast<CTrain*>(raw);*reinterpret_cast<uintptr_t*>(raw)=uintptr_t(table);train->m_nModelIndex=int16(model);train->m_matrix=reinterpret_cast<CMatrixLink*>(matrices+index*0x80);train->m_pDriver=nullptr;return raw;}
inline void __fastcall Translate(void* raw,void*,CVector pos){trace.push_back({"translate",int((static_cast<uint8*>(raw)-matrices)/0x80),std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z)});memcpy(static_cast<uint8*>(raw)+0x30,&pos,12);}
inline int32 __cdecl Closest(CVector pos,int32* track){*track=(scenario/7)%4;const auto node=(scenario/11)%8;trace.push_back({"closest",std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z),*track,node});return node;}
inline void __fastcall Position(void* raw,void*){auto* train=static_cast<CTrain*>(raw);const auto& pos=train->GetPosition();trace.push_back({"position",Index(raw),std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z)});train->m_fCurrentRailDistance=77.25f;}
inline void __fastcall Register(void* raw,void*,CEntity** ref){trace.push_back({"register",Index(raw),Index(*ref),uint32(reinterpret_cast<uint8*>(ref)-storage)%0xA18});}
inline void __fastcall Control(void* raw,void*){auto* train=static_cast<CTrain*>(raw);trace.push_back({"control",Index(raw),std::bit_cast<uint32>(train->m_fCurrentRailDistance),std::bit_cast<uint32>(train->m_fLength),train->m_nTrainFlags,Index(train->m_pPrevCarriage),Index(train->m_pNextCarriage)});train->m_fCurrentRailDistance+=.25f;train->m_fTrainSpeed=.125f;}
inline void __cdecl Remove(CEntity* raw){trace.push_back({"remove",Index(raw)});}
inline void __cdecl Add(CEntity* raw){trace.push_back({"add",Index(raw)});}
inline CPed* __cdecl Spawn(CVehicle* raw,bool asDriver,int32 rating,int32 seat,bool male,bool criminal){trace.push_back({"spawn",Index(raw),asDriver,rating,seat,male,criminal});raw->m_pDriver=scenario%17==0?nullptr:driver;return raw->m_pDriver;}
inline void __fastcall Decision(void* raw,void*,int32 type){trace.push_back({"decision",type});static_cast<CPedIntelligence*>(raw)->m_nDecisionMakerType=type;}
'''+s[b:]
s=s.replace('"RemoveRandomPassenger",0x6F6850','"CreateMissionTrain",0x6F7550')
a=s.index(' const auto oldReplay=');b=s.index(' for(size_t n=',a)
s=s[:a]+''' InitModule();auto* constructorHook=FindHook("CTrain","Constructor");const auto constructorBody=constructorHook?FirstCall(uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(constructorHook)->GetHookOurAddress())):0;if(!constructorBody){hooks[0]->State(enabled[0]);hooks[0]->LockState(locked[0]);Write({{"event","error"},{"reason","native train constructor resolution failed"}});return;}
 const auto oldCounts=StaticRef<std::array<int32,4>>(0xC38014);const auto oldNodes=StaticRef<std::array<CTrainNode*,4>>(0xC38024);static constexpr std::array<int32,5> ids{449,537,538,569,570};std::array<void*,5> oldModels{};for(size_t k=0;k<5;++k)oldModels[k]=StaticRef<void*>(0xA9B0C8+ids[k]*4);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x6E2D50,Allocate),ad(bit_cast<uintptr_t>(static_cast<void*(*)(unsigned)>(&CVehicle::operator new)),Allocate);
 EntryPatch c(0x6F6030,Constructor),cd(constructorBody,Constructor);
 EntryPatch m(0x59AF40,Translate),md(bit_cast<uintptr_t>(&CMatrix::SetTranslate),Translate);
 EntryPatch n(0x6F6BD0,Closest),nd(bit_cast<uintptr_t>(&CTrain::FindClosestTrackNode),Closest);
 EntryPatch p(0x6F6CC0,Position),pd(bit_cast<uintptr_t>(&CTrain::FindPositionOnTrackFromCoors),Position);
 EntryPatch r(0x571B70,Register),rd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Register);
 EntryPatch w(0x563280,Remove),wd(bit_cast<uintptr_t>(&CWorld::Remove),Remove);
 EntryPatch wa(0x563220,Add),wad(bit_cast<uintptr_t>(&CWorld::Add),Add);
 EntryPatch s(0x613A00,Spawn),sd(bit_cast<uintptr_t>(&CPopulation::AddPedInCar),Spawn);
 EntryPatch d(0x600B50,Decision),dd(bit_cast<uintptr_t>(&CPedIntelligence::SetPedDecisionMakerType),Decision);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('    hooks[n]->State',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,8*0xA18> objects{};alignas(16) std::array<uint8,8*0x80> transforms{};alignas(16) std::array<uint8,sizeof(CPed)> ped{};alignas(16) std::array<uint8,sizeof(CPedIntelligence)> intellect{};std::array<CTrainNode,8> nodes;std::array<std::array<uintptr_t,16>,5> models{};alignas(16) std::array<std::array<uint8,sizeof(CColModel)>,5> cols{};std::array<uintptr_t,66> vtable{};vtable[10]=bit_cast<uintptr_t>(&Control);CTrain* first{};CTrain* last{};
   auto run=[&](bool candidate){scenario=i;used=0;trace=json::array();objects.fill(0xCD);transforms.fill(0);ped.fill(0);intellect.fill(0);memset(nodes.data(),0,sizeof(nodes));memset(models.data(),0,sizeof(models));memset(cols.data(),0,sizeof(cols));storage=objects.data();matrices=transforms.data();table=vtable.data();driver=reinterpret_cast<CPed*>(ped.data());*reinterpret_cast<CPedIntelligence**>(ped.data()+0x47C)=reinterpret_cast<CPedIntelligence*>(intellect.data());first=last=nullptr;
    StaticRef<std::array<int32,4>>(0xC38014).fill(8);StaticRef<std::array<CTrainNode*,4>>(0xC38024).fill(nodes.data());for(size_t k=0;k<8;++k)nodes[k].m_nDistanceFromStart=uint16(k*30);
    for(size_t k=0;k<5;++k){models[k][5]=bit_cast<uintptr_t>(cols[k].data());StaticRef<void*>(0xA9B0C8+ids[k]*4)=models[k].data();auto* col=reinterpret_cast<CColModel*>(cols[k].data());col->m_boundBox.m_vecMin={-1.f,-float(k+1),-2.f};col->m_boundBox.m_vecMax={1.f,float(k+4),2.f};}
    const auto node=i%5==0?-1:(i/3)%8;const auto track=(i/5)%4;const auto type=uint32((i/7)%10);auto* lastOut=i%3==0?nullptr:i%7==0?&first:&last;const auto pos=CVector{float(i%7),float(i%11),float(i%3)};
'''+s[b:]
s=s.replace('reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);','reinterpret_cast<void(__cdecl*)(CVector,bool,uint32,CTrain**,CTrain**,int32,int32,bool)>(fn.address)(pos,i%2!=0,type,&first,lastOut,node,track,(i/13)%2!=0);')
a=s.index('    if(!candidate&&');b=s.index('   };const auto original',a)
s=s[:a]+'''    if(!candidate&&used>0)++positiveCases;
    return json{{"switch",true},{"first",Index(first)},{"last",Index(last)},{"objects",Hash(objects.data(),objects.size())},{"transforms",Hash(transforms.data(),transforms.size())},{"ped",Hash(ped.data(),ped.size())},{"intellect",Hash(intellect.data(),intellect.size())},{"trace",trace}};
'''+s[b:]
a=s.index(' StaticRef<uint8>(0xA43088)=oldReplay;');b=s.index(' for(size_t n=',a)
s=s[:a]+''' StaticRef<std::array<int32,4>>(0xC38014)=oldCounts;StaticRef<std::array<CTrainNode*,4>>(0xC38024)=oldNodes;for(size_t k=0;k<5;++k)StaticRef<void*>(0xA9B0C8+ids[k]*4)=oldModels[k];bool restored=true;
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/TrainPassenger355',name).read_text().replace('TrainPassenger355Live','TrainCreate355Live').replace('train-passenger355','train-create355')
 if name=='build_probe.py':t=t.replace("'PedIntelligence.h'","'PedIntelligence.cpp','PedIntelligence.h','Entity/Vehicle/Vehicle.cpp','Entity/Vehicle/Vehicle.h','Core/Matrix.cpp','Core/Matrix.h','Models/BaseModelInfo.h','Collision/ColModel.h'")
 p.joinpath(name).write_text(t)
p.joinpath('README.md').write_text('CreateMissionTrain 0x6F7550: 1024 EXE/DLL pairs prepared, runtime pending. Ten configs, closest/explicit node, tracks/directions, mission/random, first/last output alias, carriage spacing, virtual control, references, reverse world reinsert, driver creation and streak flags. Full objects/transforms/driver/intelligence hash plus exact dependency trace. Constructor resolves actual DLL body before symmetric patching, fails safely if unresolved. Track globals and five model slots restored. Valid allocation and mandatory first output fixtures; original OOM/null-output crashes excluded. Evidence ../VehicleAudio355Evidence/Train_6F7550.txt.\n')
source=Path('source/game_sa/Entity/Vehicle/Train.cpp');t=source.read_text();a=t.index('void CTrain::CreateMissionTrain(');b=t.index('// 0x6F7900',a);t=t[:a]+Path('reversing/VehicleAudio355Evidence/Train_CreateNative.txt').read_text()+'\n\n'+t[b:];source.write_text(t)
