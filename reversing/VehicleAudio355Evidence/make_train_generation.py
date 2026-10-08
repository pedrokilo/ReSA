from pathlib import Path
p=Path('reversing/TrainGeneration355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainRemove355/LiveComparison.hpp').read_text().replace('TrainRemove355Live','TrainGeneration355Live').replace('train-remove355','train-generation355').replace('#include "VehiclePool.h"','#include "VehiclePool.h"\n#include "TrainNode.h"\n#include "Streaming.h"\n#include "CullZones.h"')
a=s.index('inline ReversibleHooks');s=s[:a]+'''inline int scenario{},draw{},coorCalls{},tunnelCalls{};inline CVector playerPosition{};alignas(16) inline std::array<uint8,sizeof(CTrain)> created{};
inline CVector* __cdecl Coors(CVector* out,int32 id){++coorCalls;trace.push_back({"coors",id,coorCalls});*out=playerPosition;return out;}
inline int32 __cdecl Random(){const auto r=(scenario/11+draw++)%6;trace.push_back({"random",r});return r;}
inline int32 __cdecl Tunnel(CVector pos){const auto r=tunnelCalls++%2==0?(scenario%3==0?0:0x80):0x800;trace.push_back({"tunnel",std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z),r});return r;}
inline void __cdecl Request(int32 id,int32 flags){trace.push_back({"request",id,flags});}
inline void __cdecl Deletable(int32 id){trace.push_back({"deletable",id});}
inline void __cdecl Txd(int32 id){trace.push_back({"txd",id});}
inline void __cdecl Create(CVector pos,bool clockwise,uint32 type,CTrain** first,CTrain** last,int32 node,int32 track,bool mission){trace.push_back({"create",std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z),clockwise,type,last!=nullptr,node,track,mission});*first=reinterpret_cast<CTrain*>(created.data());if(last)*last=*first;}
inline bool __fastcall MaxSpeed(void* raw,void*,float* out){const auto r=scenario%2==0;*out=r?5.f:50.f;trace.push_back({"maximum",raw==created.data(),std::bit_cast<uint32>(*out),r});return r;}
inline void __cdecl SetSpeed(CTrain* train,float speed){trace.push_back({"speed",train==reinterpret_cast<CTrain*>(created.data()),std::bit_cast<uint32>(speed)});train->m_fTrainSpeed=speed;}
'''+s[a:]
s=s.replace('"RemoveAllTrains",0x6F6AA0','"DoTrainGenerationAndRemoval",0x6F7900')
a=s.index(' auto& poolSlot=');b=s.index(' for(size_t n=',a)
s=s[:a]+''' auto& poolSlot=StaticRef<CVehiclePool*>(0xB74494);const auto oldPool=poolSlot;std::array<uint8,0x7D> globals{};memcpy(globals.data(),reinterpret_cast<void*>(0xC37FEC),globals.size());std::array<uint8,72> stations{};memcpy(stations.data(),reinterpret_cast<void*>(0x8D48F8),72);std::array<uint8,sizeof(CPlaceable)> camera{};memcpy(camera.data(),static_cast<CPlaceable*>(&TheCamera),camera.size());const auto oldNow=StaticRef<uint32>(0xB7CB84),oldPrevious=StaticRef<uint32>(0xB7CB78);const auto oldRegion=StaticRef<int32>(0xC81314);std::array<uint8,122*20> streaming{};memcpy(streaming.data(),reinterpret_cast<void*>(0x8E4CD0+449*20),streaming.size());uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch r(0x563280,Remove),rd(bit_cast<uintptr_t>(&CWorld::Remove),Remove);
 EntryPatch p(0x56E0D0,Player),pd(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),Player);
 EntryPatch c(0x56E010,Coors),cd(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),Coors);
 EntryPatch a(0x821B1E,Random),ad(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch t(0x72D9F0,Tunnel),td(bit_cast<uintptr_t>(&CCullZones::FindTunnelAttributesForCoors),Tunnel);
 EntryPatch q(0x4087E0,Request),qd(bit_cast<uintptr_t>(&CStreaming::RequestModel),Request);
 EntryPatch d(0x409C10,Deletable),dd(bit_cast<uintptr_t>(&CStreaming::SetModelIsDeletable),Deletable);
 EntryPatch x(0x409C70,Txd),xd(bit_cast<uintptr_t>(&CStreaming::SetModelTxdIsDeletable),Txd);
 EntryPatch cr(0x6F7550,Create),crd(bit_cast<uintptr_t>(&CTrain::CreateMissionTrain),Create);
 EntryPatch ms(0x6F5BA0,MaxSpeed),msd(bit_cast<uintptr_t>(&CTrain::FindMaximumSpeedToStopAtStations),MaxSpeed);
 EntryPatch sp(0x6F5E20,SetSpeed),spd(bit_cast<uintptr_t>(&CTrain::SetTrainSpeed),SetSpeed);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('    hooks[n]->State',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,0xA18*8> objects{};std::array<uint8,8> flags{};std::array<uintptr_t,6> pool{};std::array<uintptr_t,71> vtable{};vtable[0]=bit_cast<uintptr_t>(&Delete);storage=objects.data();std::array<std::array<CTrainNode,6>,4> nodes;
   auto run=[&](bool candidate){scenario=i;draw=coorCalls=tunnelCalls=0;trace=json::array();memset(objects.data(),0xCD,objects.size());created.fill(0xCD);memset(nodes.data(),0,sizeof(nodes));memcpy(reinterpret_cast<void*>(0xC37FEC),globals.data(),globals.size());const auto count=i%4==0?0:3;pool={bit_cast<uintptr_t>(storage),bit_cast<uintptr_t>(flags.data()),uintptr_t(count),0,0,0};poolSlot=reinterpret_cast<CVehiclePool*>(pool.data());slotFlags=flags.data();flags.fill(0x80);
    for(int j=0;j<count;++j){auto* train=reinterpret_cast<CTrain*>(storage+j*0xA18);*reinterpret_cast<uintptr_t*>(train)=bit_cast<uintptr_t>(vtable.data());train->m_nVehicleType=VEHICLE_TYPE_TRAIN;train->m_matrix=nullptr;train->m_placement.m_vPosn={float(j*320),0.f,0.f};train->m_pPrevCarriage=nullptr;train->m_pNextCarriage=j==0?reinterpret_cast<CTrain*>(storage+0xA18):nullptr;train->trainFlags.bIsFrontCarriage=j!=1;train->trainFlags.bMissionTrain=i%11==0;flags[j]=0;}
    player=count&&i%7==0?reinterpret_cast<CVehicle*>(storage+0xA18):nullptr;TheCamera.m_matrix=nullptr;TheCamera.m_placement.m_vPosn={i%5==0?512.f:0.f,0.f,0.f};playerPosition={0.f,0.f,i%3==0?8.f:0.f};for(size_t k=0;k<6;++k)CTrain::aStationCoors[k]={float(k*100)+500.f,0.f,0.f};if(i%13==0)CTrain::aStationCoors[0]={0.f,0.f,0.f};
    static constexpr int16 positions[]{0,400,1440,1760,1840,2400};for(size_t track=0;track<4;++track){StaticRef<std::array<int32,4>>(0xC38014)[track]=6;StaticRef<std::array<CTrainNode*,4>>(0xC38024)[track]=nodes[track].data();for(size_t k=0;k<6;++k){nodes[track][k].x=positions[k];nodes[track][k].z=0;nodes[track][k].m_nDistanceFromStart=uint16(k*30);}}
    CTrain::bDisableRandomTrains=i%17==0;CTrain::GenTrain_Status=uint32((i/7)%3);CTrain::GenTrain_Track=uint32((i/19)%2);CTrain::GenTrain_TrainConfig=uint32((i/23)%10);CTrain::GenTrain_GenerationNode=uint32((i/29)%6);CTrain::GenTrain_Direction=uint8(i%2);StaticRef<int32>(0xC38064)=int32((i/31)%8);StaticRef<uint8>(0xC38068)=uint8(i%3==0);StaticRef<int32>(0xC81314)=i%3==0?2:1;
    StaticRef<uint32>(0xB7CB84)=uint32(i%5==0?10000:9000);StaticRef<uint32>(0xB7CB78)=uint32(i%7==0?8999:i%5==0?9999:1000);for(int id=449;id<=570;++id)StaticRef<uint8>(0x8E4CD0+id*20)=uint8(i%7==0?0:1);
'''+s[b:]
s=s.replace('return json{{"switch",true},{"objects"','return json{{"switch",true},{"globals",Hash(reinterpret_cast<void*>(0xC37FEC),0x7D)},{"created",Hash(created.data(),created.size())},{"objects"')
a=s.index(' poolSlot=oldPool;');b=s.index(' for(size_t n=',a)
s=s[:a]+''' poolSlot=oldPool;memcpy(reinterpret_cast<void*>(0xC37FEC),globals.data(),globals.size());memcpy(reinterpret_cast<void*>(0x8D48F8),stations.data(),72);memcpy(static_cast<CPlaceable*>(&TheCamera),camera.data(),camera.size());StaticRef<uint32>(0xB7CB84)=oldNow;StaticRef<uint32>(0xB7CB78)=oldPrevious;StaticRef<int32>(0xC81314)=oldRegion;memcpy(reinterpret_cast<void*>(0x8E4CD0+449*20),streaming.data(),streaming.size());
 }
 bool restored=poolSlot==oldPool;
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/TrainRemove355',name).read_text().replace('TrainRemove355Live','TrainGeneration355Live').replace('train-remove355','train-generation355')
 if name=='build_probe.py':t=t.replace("'VehiclePool.h'","'VehiclePool.h','TrainNode.cpp','TrainNode.h','Streaming.cpp','Streaming.h','CullZones.cpp','CullZones.h','General.cpp','General.h','Camera.h'")
 p.joinpath(name).write_text(t)
p.joinpath('README.md').write_text('DoTrainGenerationAndRemoval 0x6F7900: 1024 isolated EXE/DLL pairs prepared, runtime pending. Tick gates, station proximity, used/free trains, carriage removal/player protection, disabled/status states, streaming loaded/missing, both tracks, bounded node search, tunnel conditions, generation and speed assignment. Full global/vehicle/created hashes and exact dependency trace. Track globals, pool, camera, station positions, clocks, region and streaming state restored. Fixtures ensure node search reaches far nodes and creation returns a valid train; original infinite/no-memory paths excluded. Evidence ../VehicleAudio355Evidence/Train_6F7900.txt.\n')
source=Path('source/game_sa/Entity/Vehicle/Train.cpp');t=source.read_text();a=t.index('void CTrain::DoTrainGenerationAndRemoval()');b=t.index('// 0x6F8170',a);t=t[:a]+Path('reversing/VehicleAudio355Evidence/Train_GenerationNative.txt').read_text()+'\n\n'+t[b:];source.write_text(t)
