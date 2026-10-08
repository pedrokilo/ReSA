from pathlib import Path
p=Path('reversing/TrainCoordinates355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainLinks355/LiveComparison.hpp').read_text().replace('TrainLinks355Live','TrainCoordinates355Live').replace('train-links355','train-coordinates355').replace('#include "Train.h"','#include "Train.h"\n#include "TrainNode.h"')
a=s.index('inline const std::array<Fn,5>');b=s.index('inline void Update()',a);s=s[:a]+'inline const std::array<Fn,1> fns{{{"CTrain","FindCoorsFromPositionOnTrack",0x6F59E0,0,-1,2048}}};\n'+s[b:]
s=s.replace('Base*,5>','Base*,1>').replace('bool,5>','bool,1>').replace('n<5','n<1')
a=s.index(' const auto oldStations');b=s.index(' for(size_t n=',a)
s=s[:a]+''' const auto oldCounts=StaticRef<std::array<int32,4>>(0xC38014);const auto oldNodes=StaticRef<std::array<CTrainNode*,4>>(0xC38024);uint32 pairs=0,totalFailures=0;
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){std::array<CTrainNode,8> nodes;CVector output;
   auto run=[&](bool candidate){memset(nodes.data(),0xCD,sizeof(nodes));output=CVector{111.f,222.f,333.f};
    const auto count=i%9;const auto track=(i/9)%4;StaticRef<std::array<int32,4>>(0xC38014).fill(count);StaticRef<std::array<CTrainNode*,4>>(0xC38024).fill(nodes.data());
    for(int j=0;j<8;++j){nodes[j].x=int16((i+j)%129-64);nodes[j].y=int16((i-j)%93);nodes[j].z=int16((i+j)%31-15);nodes[j].m_nDistanceFromStart=uint16(j*30+(i/36)%7);}
    float distance=float((i/4)%81-5);if(i%17==0)distance=std::numeric_limits<float>::quiet_NaN();
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(float,int32,CVector*)>(fn.address)(distance,track,&output);
    return json{{"switch",true},{"output",Hash(&output,sizeof(output))},{"nodes",Hash(nodes.data(),sizeof(nodes))},{"counts",StaticRef<std::array<int32,4>>(0xC38014)}};
'''+s[b:]
a=s.index(' StaticRef<std::array<float,6>>');b=s.index(' for(size_t n=',a)
s=s[:a]+''' StaticRef<std::array<int32,4>>(0xC38014)=oldCounts;StaticRef<std::array<CTrainNode*,4>>(0xC38024)=oldNodes;bool restored=StaticRef<std::array<int32,4>>(0xC38014)==oldCounts&&StaticRef<std::array<CTrainNode*,4>>(0xC38024)==oldNodes;
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/TrainLinks355',fn).read_text().replace('TrainLinks355Live','TrainCoordinates355Live').replace('train-links355','train-coordinates355').replace('Train links 355','Train coordinates 355')
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('FindCoorsFromPositionOnTrack 0x6F59E0: 2.048 pares preparados; pendiente ejecución. Cuatro vías, 0-8 nodos aislados, segmentos límites/fuera, NaN, coordenadas positivas/negativas y cuantización distancias. Compara salida exacta y conservación de nodos; restaura tablas de nodos/cuentas. ASM íntegro ../VehicleAudio355Evidence/Train_6F59E0.txt.\n')
