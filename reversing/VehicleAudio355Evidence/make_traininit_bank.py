from pathlib import Path
p=Path('reversing/TrainInit355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainSkip355/LiveComparison.hpp').read_text().replace('TrainSkip355Live','TrainInit355Live').replace('train-skip355','train-init355')
a=s.index('inline int stationCalls');b=s.index('inline void Write',a)
s=s[:a]+'''inline std::array<std::array<CTrainNode,8>,4> nodes;
inline int closestCalls{};
inline void __cdecl Read(const char* filename,CTrainNode** out,int32* count,float* total,int32 track){trace.push_back({"read",filename,track,uintptr_t(out)==0xC38024+track*4,uintptr_t(count)==0xC38014+track*4,uintptr_t(total)==0xC37FEC+track*4});*out=nodes[track].data();*count=8;*total=float(1000+track*100+scenario);}
inline int32 __cdecl Closest(CVector position,int32* track){const auto index=(scenario+closestCalls++)%8;*track=(scenario+closestCalls)%4;nodes[0][index].m_nDistanceFromStart=uint16(nodes[0][index].m_nDistanceFromStart+3);trace.push_back({"closest",std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z),index,*track});return index;}
'''+s[b:]
s=s.replace('{"CTrain","SkipToNextAllowedStation",0x6F72F0,0,-1,1024}', '{"CTrain","InitTrains",0x6F7440,0,-1,1024}')
s=s.replace('uint32 pairs=0,totalFailures=0;', 'std::array<uint8,0x60> saved;memcpy(saved.data(),reinterpret_cast<void*>(0xC37FEC),saved.size());uint32 pairs=0,totalFailures=0;')
a=s.index(' EntryPatch z(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch a(0x6F55D0,Read),ad(bit_cast<uintptr_t>(&CTrain::ReadAndInterpretTrackFile),Read);
 EntryPatch b(0x6F6BD0,Closest),bd(bit_cast<uintptr_t>(&CTrain::FindClosestTrackNode),Closest);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){scenario=i;closestCalls=0;trace=json::array();memset(nodes.data(),0xCD,sizeof(nodes));memset(reinterpret_cast<void*>(0xC37FEC),0xCD,saved.size());
    for(size_t track=0;track<4;++track){StaticRef<std::array<CTrainNode*,4>>(0xC38024)[track]=(i&(1<<track))?nodes[track].data():nullptr;StaticRef<std::array<int32,4>>(0xC38014)[track]=int32(track)+1;StaticRef<std::array<float,4>>(0xC37FEC)[track]=float(track)*123.f;for(size_t j=0;j<8;++j)nodes[track][j].m_nDistanceFromStart=uint16(scenario*17+int(track)*137+int(j)*31);}
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)()>(fn.address)();
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"globals",Hash(reinterpret_cast<void*>(0xC37FEC),saved.size())},{"nodes",Hash(nodes.data(),sizeof(nodes))},{"trace",trace}};
'''+s[b:]
s=s.replace(' bool restored=true;', ' memcpy(reinterpret_cast<void*>(0xC37FEC),saved.data(),saved.size());bool restored=memcmp(reinterpret_cast<void*>(0xC37FEC),saved.data(),saved.size())==0;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/TrainSkip355',fn).read_text().replace('train-skip355','train-init355').replace('TrainSkip355Live','TrainInit355Live');p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('InitTrains0x6F7440:1024 pares. Todas16 combinaciones tracks presentes/ausentes, filename y punteros salida por track, nodos0..7, retornoClosest track0..3 deliberadamente ignorado EXE, node0capturado antesloop6station. Read/Closest simetricos, Closest modifica distancia antes de lectura. Snapshot/restauracion GLOBAL CONTIGUO C37FEC..C3804C incluye totalLength/generation/count/pointers/stationDist, nodosaislados. Sin files/pool real. Nativa aplicada hookfalse pendiente ejecucion.\n')
import re
p=Path('source/game_sa/Entity/Vehicle/Train.cpp');s=p.read_text();m=re.search(r'[^\n]*::InitTrains\([^\n]*\)\s*\{',s);a=s.index('{',m.start());b=a+1;depth=1
while depth:depth+=(s[b]=='{')-(s[b]=='}');b+=1
p.write_text(s[:m.start()]+Path('reversing/VehicleAudio355Evidence/Train_InitNative.txt').read_text().strip()+s[b:])
p=Path('reversing/VehicleAudio355Evidence/inventory.py');s=p.read_text().replace("'BikeRatios355']","'BikeRatios355','TrainInit355']");p.write_text(s)
print('TrainInit355 prepared and native applied')
