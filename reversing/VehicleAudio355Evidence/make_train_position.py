from pathlib import Path
p=Path('reversing/TrainPosition355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainCoordinates355/LiveComparison.hpp').read_text().replace('TrainCoordinates355Live','TrainPosition355Live').replace('train-coordinates355','train-position355')
s=s.replace('inline bool done{};inline std::ofstream log;','inline bool done{};inline std::ofstream log;inline json trace;inline int lightCalls{};\ninline tColLighting __fastcall Lighting(void* node,void*){const auto result=uint8(0x2F+lightCalls++*17);trace.push_back({"lighting",result});StaticRef<float>(0x8D12C0)+=.125f;return tColLighting{result};}')
s=s.replace('"FindCoorsFromPositionOnTrack",0x6F59E0','"FindPositionOnTrackFromCoors",0x6F6CC0')
a=s.index(' const auto oldCounts=');b=s.index(' for(size_t n=',a)
s=s[:a]+''' std::array<uint8,0x60> globals{};memcpy(globals.data(),reinterpret_cast<void*>(0xC37FEC),globals.size());
 const auto oldDay=StaticRef<float>(0x8D12C0);auto& modelSlot=StaticRef<void*>(0xA9B0C8+400*4);const auto oldModel=modelSlot;
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch l(0x6F5F80,Lighting),ld(bit_cast<uintptr_t>(&CTrainNode::GetLightingFromCollision),Lighting);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){std::array<CTrainNode,8> nodes;alignas(16) std::array<uint8,sizeof(CTrain)> object{};std::array<uintptr_t,16> model{};CColModel col{};CMatrix matrix;
   auto run=[&](bool candidate){trace=json::array();lightCalls=0;memset(object.data(),0xCD,object.size());memset(nodes.data(),0xCD,sizeof(nodes));auto* self=reinterpret_cast<CTrain*>(object.data());
    const auto count=i%9;const auto track=(i/9)%4;StaticRef<std::array<int32,4>>(0xC38014).fill(count);StaticRef<std::array<CTrainNode*,4>>(0xC38024).fill(nodes.data());StaticRef<std::array<float,4>>(0xC37FEC).fill(100.f);StaticRef<float>(0x8D12C0)=float((i/36)%5)*.25f;
    for(int j=0;j<8;++j){nodes[j].x=int16(j*80);nodes[j].y=int16((j%2)*16);nodes[j].z=int16(j*8);nodes[j].m_nDistanceFromStart=uint16(j*30);}
    model.fill(0);model[5]=bit_cast<uintptr_t>(&col);modelSlot=model.data();col.m_boundBox.m_vecMin={-1.f,-float(i%7),-2.f};col.m_boundBox.m_vecMax={1.f,float(i%11),2.f};self->m_nModelIndex=400;self->m_nTrackId=int8(track);
    const auto position=CVector{float((i/4)%71)-.01f,float((i/13)%9-4),0.f};self->m_matrix=i%2?nullptr:&matrix;if(self->m_matrix)matrix.GetPosition()=position;else self->m_placement.m_vPosn=position;
    self->m_vecMoveSpeed={float(i%5-2)*.125f,float((i/5)%5-2)*.25f,i%17==0?std::bit_cast<float>(0x7FC00000u):.125f};self->trainFlags.bClockwiseDirection=i%2;self->m_fCurrentRailDistance=123.f;self->m_fTrainSpeed=-234.f;self->m_fContactSurfaceBrightness=.75f;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);
    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"nodes",Hash(nodes.data(),sizeof(nodes))},{"day",std::bit_cast<uint32>(StaticRef<float>(0x8D12C0))},{"trace",trace}};
'''+s[b:]
a=s.index(' StaticRef<std::array<int32,4>>(0xC38014)=oldCounts;');b=s.index(' for(size_t n=',a)
s=s[:a]+''' }
 memcpy(reinterpret_cast<void*>(0xC37FEC),globals.data(),globals.size());StaticRef<float>(0x8D12C0)=oldDay;modelSlot=oldModel;bool restored=true;
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/TrainCoordinates355',name).read_text().replace('TrainCoordinates355Live','TrainPosition355Live').replace('train-coordinates355','train-position355')
 if name=='build_probe.py':t=t.replace("'TrainNode.h'","'TrainNode.h','TrainNode.cpp'")
 p.joinpath(name).write_text(t)
p.joinpath('README.md').write_text('FindPositionOnTrackFromCoors 0x6F6CC0: 1024 isolated EXE/DLL pairs prepared, runtime pending. Zero through eight nodes, all four tracks, matrix/placeable positions, projection bounds, near/far track, carriage length, direction, speed NaN, and changing lighting between dependency calls. Complete object/node hashes and exact lighting trace; contiguous track globals, model slot, daylight restored. Evidence ../VehicleAudio355Evidence/Train_6F6CC0.txt. x87 projection denominator retained; remaining arithmetic requires runtime comparison before approval.\n')
source=Path('source/game_sa/Entity/Vehicle/Train.cpp');t=source.read_text();a=t.index('void CTrain::FindPositionOnTrackFromCoors()');b=t.index('// 0x6F7090',a);t=t[:a]+Path('reversing/VehicleAudio355Evidence/Train_PositionNative.txt').read_text()+'\n\n'+t[b:];source.write_text(t)
