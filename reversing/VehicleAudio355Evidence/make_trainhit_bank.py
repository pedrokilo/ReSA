from pathlib import Path
p=Path('reversing/TrainHit355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainSkip355/LiveComparison.hpp').read_text().replace('TrainSkip355Live','TrainHit355Live').replace('train-skip355','train-hit355')
a=s.index('inline int stationCalls');b=s.index('inline void Write',a)
s=s[:a]+'''struct Node{CPhysical* item;Node* next;};inline std::array<Node,8> nodes;
alignas(16) inline std::array<std::array<uint8,0x200>,8> objects;inline std::array<uintptr_t,5> vtable;
inline int Index(const void* item){for(int i=0;i<8;++i)if(item==objects[i].data())return i;return -1;}
inline void __fastcall Static(CEntity* entity,void*,bool value){const auto index=Index(entity);trace.push_back({"static",index,value,*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(entity)+0x1C)});entity->m_bIsStatic=value;if(scenario%3==0&&index>=0)nodes[index].next=nullptr;}
inline void __fastcall Moving(CPhysical* entity,void*){trace.push_back({"moving",Index(entity),entity->m_bIsStatic});entity->physicalFlags.bDisableSimpleCollision=true;}
'''+s[b:]
s=s.replace('{"CTrain","SkipToNextAllowedStation",0x6F72F0,0,-1,1024}', '{"CTrain","TrainHitStuff<CPtrListSingleLink<CPhysical*>>",0x6F5CF0,0,-1,1024}')
s=s.replace('uint32 pairs=0,totalFailures=0;', 'std::array<uint8,12> saved;memcpy(saved.data(),reinterpret_cast<void*>(0x8CD6C8),saved.size());uint32 pairs=0,totalFailures=0;')
a=s.index(' EntryPatch z(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' vtable[4]=bit_cast<uintptr_t>(&Static);
 EntryPatch a(0x542800,Moving),ad(bit_cast<uintptr_t>(&CPhysical::AddToMovingList),Moving);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(objects.data(),0xCD,sizeof(objects));memset(reinterpret_cast<void*>(0x8CD6C8),0xCD,saved.size());const auto count=i%9;
    StaticRef<uint16>(0x8CD6C8)=0;StaticRef<uint16>(0x8CD6CC)=500;StaticRef<uint16>(0x8CD6D0)=65535;
    for(int j=0;j<8;++j){auto* physical=reinterpret_cast<CPhysical*>(objects[j].data());*reinterpret_cast<uintptr_t*>(physical)=uintptr_t(vtable.data());*reinterpret_cast<uint32*>(objects[j].data()+0x1C)=std::array<uint32,4>{0,4,0x40000,0x40004}[(i/9+j)%4];objects[j][0x36]=uint8((i/36+j)%3==0?4:(i/36+j)%3);physical->m_nModelIndex=std::array<int16,4>{-1,0,500,501}[(i/108+j)%4];nodes[j].item=physical;nodes[j].next=j+1<count?&nodes[j+1]:nullptr;}
    Node* list=count?nodes.data():nullptr;CEntity* excluded=(i/7)%3==0?reinterpret_cast<CEntity*>(objects[(i/3)%8].data()):nullptr;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(Node**,CEntity*)>(fn.address)(&list,excluded);
    if(!candidate&&!trace.empty())++positiveCases;for(auto& object:objects)*reinterpret_cast<uintptr_t*>(object.data())=0;
    std::array<int,8> next;for(int j=0;j<8;++j)next[j]=nodes[j].next?int(nodes[j].next-nodes.data()):-1;
    return json{{"switch",true},{"objects",Hash(objects.data(),sizeof(objects))},{"next",next},{"models",Hash(reinterpret_cast<void*>(0x8CD6C8),saved.size())},{"trace",trace}};
'''+s[b:]
s=s.replace(' bool restored=true;', ' memcpy(reinterpret_cast<void*>(0x8CD6C8),saved.data(),saved.size());bool restored=true;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/TrainSkip355',fn).read_text().replace('train-skip355','train-hit355').replace('TrainSkip355Live','TrainHit355Live')
 if fn=='build_probe.py':s=s.replace("'GameLogic.h'", "'GameLogic.h','Entity/Physical.cpp','Entity/Physical.h','Entity/Entity.h','Core/PtrList.h','Core/PtrListSingleLink.h','Core/PtrNodeSingleLink.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('TrainHitStuff<CPtrListSingleLink<CPhysical*>>0x6F5CF0:1024 pares pendientes. List0..8/exclusionself, type/static/waiting, signedmodel-1 versus table65535, matchingmodels0/500. Fakevtable4SetIsStatic muta enlaceNext para comprobar que el recorrido lo captura antes, Moving paired con sideeffect. Fullphysicalobjects/nodeNext/globalmodelindices contiguous12 restaurados. Sin lista/pool real. Nativa aplicada hookfalse.\n')
import re
p=Path('source/game_sa/Entity/Vehicle/Train.cpp');s=p.read_text();a=s.index('template<typename PtrListType>\nvoid TrainHitStuff');brace=s.index('{',a);b=brace+1;d=1
while d:d+=(s[b]=='{')-(s[b]=='}');b+=1
p.write_text(s[:a]+Path('reversing/VehicleAudio355Evidence/Train_HitNative.txt').read_text().strip()+s[b:])
p=Path('reversing/VehicleAudio355Evidence/inventory.py');s=p.read_text().replace("'TrainCtor355']","'TrainCtor355','TrainHit355']");p.write_text(s)
print('TrainHit355 prepared and applied')
