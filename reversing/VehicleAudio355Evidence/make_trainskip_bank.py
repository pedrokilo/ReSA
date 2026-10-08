from pathlib import Path
p=Path('reversing/TrainSkip355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainQueries355/LiveComparison.hpp').read_text().replace('TrainQueries355Live','TrainSkip355Live').replace('train-queries355','train-skip355').replace('#include "TheZones.h"','#include "TheZones.h"\n#include "Streaming.h"\n#include "GameLogic.h"')
a=s.index('inline uint32 __cdecl Zone');b=s.index('inline void Write',a)
s=s[:a]+'''inline int stationCalls{};inline CTrain* activeEngine{};
inline void __cdecl Next(bool clockwise,float distance,float* outDistance,int32* station){*station=(scenario+stationCalls++)%6;*outDistance=distance+(clockwise?100.f:-100.f);trace.push_back({"next",clockwise,std::bit_cast<uint32>(distance),std::bit_cast<uint32>(*outDistance),*station});}
inline uint32 __cdecl Zone(const CVector& p){const auto level=stationCalls<=scenario%4?3u:0u;trace.push_back({"zone",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z),level});return level;}
inline float __cdecl Stat(eStats stat){trace.push_back({"stat",int(stat)});return 1.f;}
inline void __cdecl Scene(const CVector& p){trace.push_back({"scene",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z)});activeEngine->GetPosition().x+=3.f;}
inline void __cdecl Load(bool priority){trace.push_back({"load",priority});activeEngine->GetPosition().y-=4.f;}
inline void __cdecl Time(uint32 minutes){trace.push_back({"time",minutes});}
'''+s[b:]
a=s.index('inline const std::array<Fn,2>');b=s.index('inline void Update()',a);s=s[:a]+'inline const std::array<Fn,1> fns{{{"CTrain","SkipToNextAllowedStation",0x6F72F0,0,-1,1024}}};\n'+s[b:]
s=s.replace('Base*,2>','Base*,1>').replace('bool,2>','bool,1>').replace('n<2','n<1')
a=s.index(' const auto oldStations');b=s.index(' {\n using ReversingProbe',a);s=s[:a]+' uint32 pairs=0,totalFailures=0;\n'+s[b:]
a=s.index(' EntryPatch z(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch z(0x572300,Zone),zd(bit_cast<uintptr_t>(&CTheZones::GetLevelFromPosition),Zone);
 EntryPatch t(0x558E40,Stat),td(bit_cast<uintptr_t>(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),Stat);
 EntryPatch a(0x6F5F00,Next),ad(bit_cast<uintptr_t>(&CTrain::FindNextStationPositionInDirection),Next);
 EntryPatch b(0x40EB70,Scene),bd(bit_cast<uintptr_t>(&CStreaming::LoadScene),Scene);
 EntryPatch c(0x40EA10,Load),cd(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),Load);
 EntryPatch d(0x4414C0,Time),dd(bit_cast<uintptr_t>(&CGameLogic::PassTime),Time);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<std::array<uint8,sizeof(CTrain)>,8> objects{};alignas(16) std::array<uint8,0x80> matrix{};
   auto run=[&](bool candidate){scenario=i;stationCalls=0;trace=json::array();memset(objects.data(),0xCD,sizeof(objects));memset(matrix.data(),0,matrix.size());const auto length=1+i%8;
    for(int t=0;t<8;++t){auto* train=reinterpret_cast<CTrain*>(objects[t].data());train->m_pNextCarriage=t+1<length?reinterpret_cast<CTrain*>(objects[t+1].data()):nullptr;train->m_pPrevCarriage=t>0?reinterpret_cast<CTrain*>(objects[t-1].data()):nullptr;train->trainFlags.bClockwiseDirection=i%2;train->m_fCurrentRailDistance=float(i%101-5)*61.f;*reinterpret_cast<uintptr_t*>(objects[t].data()+0x14)=0;*reinterpret_cast<CVector*>(objects[t].data()+4)=CVector{float(i%331-165),float(i%241-120),float(i%19)};}
    activeEngine=reinterpret_cast<CTrain*>(objects[0].data());if((i/8)%2){*reinterpret_cast<uintptr_t*>(objects[0].data()+0x14)=uintptr_t(matrix.data());*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{float(i%331-165),float(i%241-120),float(i%19)};}
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(CTrain*)>(fn.address)(reinterpret_cast<CTrain*>(objects[(i/16)%length].data()));
    return json{{"switch",true},{"objects",Hash(objects.data(),sizeof(objects))},{"matrix",Hash(matrix.data(),matrix.size())},{"trace",trace}};
'''+s[b:]
a=s.index(' StaticRef<std::array<int32,4>>');b=s.index(' for(size_t n=',a);s=s[:a]+' bool restored=true;\n'+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/TrainQueries355',fn).read_text().replace('train-queries355','train-skip355').replace('TrainQueries355Live','TrainSkip355Live')
 if fn=='build_probe.py':s=s.replace("'Stats.h'", "'Stats.h','Streaming.cpp','Streaming.h','GameLogic.cpp','GameLogic.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('SkipToNextAllowedStation 0x6F72F0:1024 pares preparados. Cadena1-8 previa, clockwise, iteraciones rechazo0-3, estación0-5, matrix/placeable, raildistance. NextStation/Zones/Stats/Streaming/PassTime simétricos; LoadScene/LoadAll alteran posición antes de distancia/tiempo para exigir orden. No globals/pool real modificados. NaNStats excluido porque EXE repite indefinidamente; distancia finite y conversión _ftol2 low32. Cuerpo Train_SkipNative.txt todavía no aplicado.\n')
print('TrainSkip355 prepared')
