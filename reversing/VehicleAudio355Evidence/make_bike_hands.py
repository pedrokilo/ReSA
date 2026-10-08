from pathlib import Path
p=Path('reversing/BikeHands355');p.mkdir(exist_ok=True)
s=Path('reversing/BikeRoad355/LiveComparison.hpp').read_text().replace('BikeRoad355Live','BikeHands355Live').replace('bike-road355','bike-hands355').replace('#include "World.h"','#include "World.h"\n#include "RwHelper.h"')
a=s.index('inline int scenario');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline int scenario{};
inline std::array<RwMatrix,13> bones{};inline RpHAnimHierarchy hierarchy{};
inline RpHAnimHierarchy* __cdecl Hierarchy(RpClump*){trace.push_back({"hierarchy"});return &hierarchy;}
inline int32 __cdecl Index(RpHAnimHierarchy*,int32 id){static constexpr int32 ids[]{0x18,0x19,0x1A,0x17,0x16,0x12D,0x22,0x23,0x24,0x21,0x20,0x12E};const auto index=int32(std::find(std::begin(ids),std::end(ids),id)-std::begin(ids));trace.push_back({"index",id,index});return index;}
inline RwMatrix* __cdecl Matrices(RpHAnimHierarchy*){trace.push_back({"array"});return bones.data();}
inline bool __fastcall IsPlayer(void*,void*){trace.push_back({"player"});return scenario%2!=0;}
'''+s[b:]
s=s.replace('{"CBike","PlaceOnRoadProperly",0x6BEEB0,0,-1,1024}','{"CBike","FixHandsToBars",0x6B7F90,0,-1,1024}')
s=s.replace('uint32 pairs=0,totalFailures=0;', 'std::array<CBaseModelInfo*,3> savedModels{};static constexpr int32 models[]{400,509,510};for(size_t j=0;j<3;++j)savedModels[j]=CModelInfo::ms_modelInfoPtrs[models[j]];uint32 pairs=0,totalFailures=0;')
a=s.index(' EntryPatch c(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch h(0x734A40,Hierarchy),hd(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),Hierarchy);
 EntryPatch ix(0x7C51A0,Index),ixd(bit_cast<uintptr_t>(&RpHAnimIDGetIndex),Index);
 EntryPatch ar(0x7C5120,Matrices),ard(bit_cast<uintptr_t>(&RpHAnimHierarchyGetMatrixArray),Matrices);
 EntryPatch pl(0x5DF8F0,IsPlayer),pld(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,sizeof(CPed)> ped{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> model{};alignas(16) std::array<uint8,0x400> vehicleStruct{};RwFrame frame{};
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(object.data(),0xCD,object.size());memset(ped.data(),0,ped.size());memset(matrix.data(),0,matrix.size());memset(model.data(),0,model.size());memset(vehicleStruct.data(),0,vehicleStruct.size());memset(&frame,0,sizeof(frame));memset(bones.data(),0,sizeof(bones));
    auto* self=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());self->m_nModelIndex=int16(models[(i/7)%3]);self->m_nFixLeftHand=uint8((i/3)%2);self->m_nFixRightHand=uint8((i/5)%2);std::ranges::fill(self->m_aBikeNodes,nullptr);self->m_aBikeNodes[BIKE_HANDLEBARS]=i%11==0?nullptr:&frame;
    auto* mi=reinterpret_cast<CVehicleModelInfo*>(model.data());*reinterpret_cast<void**>(model.data()+0x5C)=vehicleStruct.data();for(auto id:models)CModelInfo::ms_modelInfoPtrs[id]=mi;
    *reinterpret_cast<CVector*>(vehicleStruct.data()+0x78)=i%4==0?CVector{0.f,0.f,0.f}:CVector{float(i%5)*.125f,.5f,.25f};
    *reinterpret_cast<CVector*>(matrix.data())={1.f,0.f,0.f};*reinterpret_cast<CVector*>(matrix.data()+0x10)={0.f,1.f,0.f};*reinterpret_cast<CVector*>(matrix.data()+0x20)={0.f,0.f,1.f};*reinterpret_cast<CVector*>(matrix.data()+0x30)={float(i%7)*.25f,float(i%11)*.125f,float(i%13)*.25f};
    frame.modelling.right={1.f,0.f,0.f};frame.modelling.up={0.f,1.f,0.f};frame.modelling.at={0.f,0.f,1.f};frame.modelling.pos={.25f,.5f,.125f};
    for(size_t j=0;j<bones.size();++j){bones[j].right={1.f,0.f,0.f};bones[j].up={0.f,1.f,0.f};bones[j].at={0.f,0.f,1.f};bones[j].pos={float(j)*.125f,float(j)*.25f,float(j)*.125f};}
    hooks[n]->State(candidate);const auto* bytes=reinterpret_cast<const uint8*>(fn.address);if((bytes[0]==0xE9)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(CBike*,CPed*)>(fn.address)(self,reinterpret_cast<CPed*>(ped.data()));std::ranges::fill(self->m_aBikeNodes,nullptr);self->m_matrix=nullptr;
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"bones",Hash(bones.data(),sizeof(bones))},{"frame",Hash(&frame,sizeof(frame))},{"trace",trace}};
'''+s[b:]
s=s.replace(' bool restored=true;', ' for(size_t j=0;j<3;++j)CModelInfo::ms_modelInfoPtrs[models[j]]=savedModels[j];\n bool restored=true;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/BikeRoad355',fn).read_text().replace('bike-road355','bike-hands355').replace('BikeRoad355Live','BikeHands355Live')
 if fn=='build_probe.py':s=s.replace("'Core/Vector.h'","'Core/Vector.h','RwHelper.cpp','RwHelper.h','Entity/Ped/Ped.cpp','Entity/Ped/Ped.h','RenderWare/rw/rphanim.cpp','RenderWare/rw/rphanim.h','Models/ModelInfo.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('FixHandsToBars 0x6B7F90:1024 pares pendientes. REA+ASM completo Bike_6B7F90. Right/left independently, absent handlebar, 400/509/510 fallback constants, authored dummy, player accessories, bone mapping/array/player calls traced, full bones/object/frame hash. Identity rotations and finite binary coordinates isolate placement arithmetic; x87 scale-add preserves X/Y stored product and Z extended product. Matrix attach/product dependencies use existing implementations shared through hooks, no certification of those dependencies implied. Model slots restored. Source flag false until EXE validation.\n')
