from pathlib import Path
p=Path('reversing/TrainQueries355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainLinks355/LiveComparison.hpp').read_text().replace('TrainLinks355Live','TrainQueries355Live').replace('train-links355','train-queries355').replace('#include "Train.h"','#include "Train.h"\n#include "TrainNode.h"\n#include "TheZones.h"')
s=s.replace('inline bool done{};inline std::ofstream log;','inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};\ninline uint32 __cdecl Zone(const CVector& p){trace.push_back({"zone",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z)});return uint32(scenario%5);}\ninline float __cdecl Stat(eStats stat){const auto v=scenario%17==0?std::numeric_limits<float>::quiet_NaN():float((scenario/5)%7-1);trace.push_back({"stat",int(stat),std::bit_cast<uint32>(v)});return v;}')
a=s.index('inline const std::array<Fn,5>');b=s.index('inline void Update()',a);s=s[:a]+'inline const std::array<Fn,2> fns{{{"CTrain","FindClosestTrackNode",0x6F6BD0,0,-1,1024},{"CTrain","IsNextStationAllowed",0x6F7260,0,-1,1024}}};\n'+s[b:]
s=s.replace('Base*,5>','Base*,2>').replace('bool,5>','bool,2>').replace('n<5','n<2')
a=s.index(' const auto oldStations');b=s.index(' for(size_t n=',a)
s=s[:a]+''' const auto oldStations=StaticRef<std::array<float,6>>(0xC38034);const auto oldTotal=StaticRef<float>(0xC37FEC);const auto oldCounts=StaticRef<std::array<int32,4>>(0xC38014);const auto oldNodes=StaticRef<std::array<CTrainNode*,4>>(0xC38024);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch z(0x572300,Zone),zd(bit_cast<uintptr_t>(&CTheZones::GetLevelFromPosition),Zone);
 EntryPatch t(0x558E40,Stat),td(bit_cast<uintptr_t>(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),Stat);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<std::array<uint8,sizeof(CTrain)>,8> objects{};std::array<std::array<CTrainNode,8>,4> nodes;
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(objects.data(),0xCD,sizeof(objects));memset(nodes.data(),0xCD,sizeof(nodes));const auto length=1+i%8;
    for(int t=0;t<8;++t){auto* train=reinterpret_cast<CTrain*>(objects[t].data());train->m_pNextCarriage=t+1<length?reinterpret_cast<CTrain*>(objects[t+1].data()):nullptr;train->m_pPrevCarriage=t>0?reinterpret_cast<CTrain*>(objects[t-1].data()):nullptr;train->trainFlags.bClockwiseDirection=i%2;train->m_fCurrentRailDistance=float(i%101-5)*61.f;}
    for(int t=0;t<4;++t){StaticRef<std::array<int32,4>>(0xC38014)[t]=1+(i+t)%8;StaticRef<std::array<CTrainNode*,4>>(0xC38024)[t]=nodes[t].data();for(int j=0;j<8;++j){nodes[t][j].x=int16(i%129-64+j*8+t*32);nodes[t][j].y=int16(i%73-36+j*4-t*24);nodes[t][j].z=int16(j*3+t*8);}}
    StaticRef<std::array<float,6>>(0xC38034)={200.f,800.f,1400.f,2400.f,3600.f,4800.f};StaticRef<float>(0xC37FEC)=6000.f;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};int32 outTrack=-123,ret;
    if(n==0)ret=reinterpret_cast<int32(__cdecl*)(CVector,int32*)>(fn.address)(CVector{float(i%31-15),float(i%23-11),float(i%19-9)},&outTrack);
    else ret=reinterpret_cast<bool(__cdecl*)(CTrain*)>(fn.address)(reinterpret_cast<CTrain*>(objects[(i/8)%length].data()));
    return json{{"switch",true},{"return",ret},{"outtrack",outTrack},{"objects",Hash(objects.data(),sizeof(objects))},{"nodes",Hash(nodes.data(),sizeof(nodes))},{"trace",trace}};
'''+s[b:]
s=s.replace(' StaticRef<std::array<float,6>>(0xC38034)=oldStations;', ' }\n StaticRef<std::array<int32,4>>(0xC38014)=oldCounts;StaticRef<std::array<CTrainNode*,4>>(0xC38024)=oldNodes;\n StaticRef<std::array<float,6>>(0xC38034)=oldStations;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/TrainLinks355',fn).read_text().replace('TrainLinks355Live','TrainQueries355Live').replace('train-links355','train-queries355')
 if fn=='build_probe.py':s=s.replace("'TrainNode.h'","'TrainNode.h','TheZones.cpp','TheZones.h','Stats.cpp','Stats.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('FindClosestTrackNode 0x6F6BD0 e IsNextStationAllowed 0x6F7260: 1.024 pares cada una, pendiente ejecución. Nodos aislados en cuatro vías y búsqueda con distancias finitas menores que 99.999,9: se excluye dominio original de retorno indeterminado sin mejora. Closest compara suma original z²+y²+x². Allowed incluye 1-8 vagones, dirección, distancia, nivel y stats negativos/positivos/NaN con rechazo NaN confirmado ASM. Restaura nodos y estaciones. Dependencias zone/stat simétricas; retornos y trazas exactos.\n')
