from pathlib import Path
p=Path('reversing/VehicleAudio355Evidence/Train_ReadNative.txt');s=p.read_text().replace('0xB530, "r"','0xB530, "rb"');p.write_text(s)
p=Path('reversing/TrainRead355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainInit355/LiveComparison.hpp').read_text().replace('TrainInit355Live','TrainRead355Live').replace('train-init355','train-read355').replace('#include "GameLogic.h"','#include "GameLogic.h"\n#include "FileMgr.h"')
a=s.index('inline std::array<std::array<CTrainNode');b=s.index('inline void Write',a)
s=s[:a]+'''alignas(16) inline std::array<uint8,80> createdNodes{},existingNodes{};
alignas(16) inline std::array<uint8,0xB530> fileBuffer{};
inline std::array<char,1024> text{};
inline std::array<std::array<uint32,4>,8> calls{};inline size_t callCount{};inline int countFixture{};
inline void Record(uint32 op,uint32 a=0,uint32 b=0,uint32 c=0){if(callCount<calls.size())calls[callCount++]={op,a,b,c};}
inline void* __cdecl NewArray(size_t size){Record(1,uint32(size));return size==0xB530?fileBuffer.data():createdNodes.data();}
inline void __cdecl DeleteArray(void* memory){Record(3,memory==fileBuffer.data()?1u:memory==createdNodes.data()?2u:3u);}
inline size_t __cdecl Load(const char* filename,uint8* buffer,size_t size,const char* mode){Record(2,strcmp(filename,"fixture.dat")==0,uint32(size),strcmp(mode,"rb")==0);const auto length=strlen(text.data());memcpy(buffer,text.data(),length+1);return length;}
'''+s[b:]
s=s.replace('{"CTrain","InitTrains",0x6F7440,0,-1,1024}', '{"CTrain","ReadAndInterpretTrackFile",0x6F55D0,0,-1,1024}')
s=s.replace('std::array<uint8,0x60> saved;memcpy(saved.data(),reinterpret_cast<void*>(0xC37FEC),saved.size());', 'std::array<uint8,352> savedString;std::array<uint8,72> savedStations;memcpy(savedString.data(),reinterpret_cast<void*>(0xB71670),savedString.size());memcpy(savedStations.data(),reinterpret_cast<void*>(0x8D48F8),savedStations.size());')
a=s.index(' EntryPatch a(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch a(0x821195,NewArray),b(0x8213AE,DeleteArray);
 EntryPatch c(0x538890,Load),cd(bit_cast<uintptr_t>(&CFileMgr::LoadFile),Load);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){scenario=i;callCount=0;trace=json::array();memset(createdNodes.data(),0xCD,createdNodes.size());memset(existingNodes.data(),0xCD,existingNodes.size());memset(fileBuffer.data(),0xCD,fileBuffer.size());memset(reinterpret_cast<void*>(0xB71670),0xCD,savedString.size());memset(reinterpret_cast<void*>(0x8D48F8),0xCD,savedStations.size());
    countFixture=(i/2)%9;const auto processed=(i/18)%2!=0;size_t length=size_t(sprintf(text.data(),processed?"processed\\n%d\\n":"%d\\n",countFixture));
    for(int j=0;j<countFixture;++j){const auto x=(i%101-50)*.125f+j*.25f,y=(i%73-36)*.25f-j*.125f,z=(i%31-15)*.5f;length+=size_t(sprintf(text.data()+length,"%.3f %.3f %.3f %d\\n",double(x),double(y),double(z),j%3==0));auto* node=reinterpret_cast<CTrainNode*>(existingNodes.data())+j;node->x=int16(x*8.f);node->y=int16(y*8.f);node->z=int16(z*8.f);}
    CTrainNode* nodes=i%2?reinterpret_cast<CTrainNode*>(existingNodes.data()):nullptr;int32 count=countFixture;float total=-123.f;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(const char*,CTrainNode**,int32*,float*,int32)>(fn.address)("fixture.dat",&nodes,&count,&total,(i/36)%4);
    for(size_t call=0;call<callCount;++call)trace.push_back(calls[call]);if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"count",count},{"total",std::bit_cast<uint32>(total)},{"node_identity",nodes==reinterpret_cast<CTrainNode*>(createdNodes.data())?1:nodes==reinterpret_cast<CTrainNode*>(existingNodes.data())?2:3},{"created",Hash(createdNodes.data(),createdNodes.size())},{"existing",Hash(existingNodes.data(),existingNodes.size())},{"buffer",Hash(fileBuffer.data(),fileBuffer.size())},{"string",Hash(reinterpret_cast<void*>(0xB71670),savedString.size())},{"stations",Hash(reinterpret_cast<void*>(0x8D48F8),savedStations.size())},{"trace",trace}};
'''+s[b:]
a=s.index(' memcpy(reinterpret_cast<void*>(0xC37FEC)');b=s.index(' for(size_t n=',a)
s=s[:a]+''' memcpy(reinterpret_cast<void*>(0xB71670),savedString.data(),savedString.size());memcpy(reinterpret_cast<void*>(0x8D48F8),savedStations.data(),savedStations.size());bool restored=true;
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/TrainInit355',fn).read_text().replace('train-init355','train-read355').replace('TrainInit355Live','TrainRead355Live')
 if fn=='build_probe.py':s=s.replace("'GameLogic.h'", "'GameLogic.h','FileMgr.cpp','FileMgr.h','TrainNode.cpp'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('ReadAndInterpretTrackFile0x6F55D0:1024 pares pendientes. Count0..8, processed/nonprocessed, nodesyaexistentes/nuevos, marcado stations y skip0..3, coords cuantizadossigned, distancias2D cerrando loop. NewArray/DeleteArray/LoadFile aislados con trace fijo sin asignaciones en heapstub. Constructor nodos escribe solo lightingFound byte9, filas sin terminador como EXE. gString352 y estaciones72 globals restaurados; archivo memoria fake, sin I/Oreal. Malformed/filemissing y OOM excluidos porque EXE puede sobreleer/crash. Fuente draft Train_ReadNative.txt; requiere aplicar mapeo aStationCoors antes build.\n')
print('TrainRead355 prepared')
