from pathlib import Path
p=Path('reversing/BikeSetup355');p.mkdir(exist_ok=True)
s=Path('reversing/BikeHands355/LiveComparison.hpp').read_text().replace('BikeHands355Live','BikeSetup355Live').replace('bike-hands355','bike-setup355')
a=s.index('inline int scenario');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline RwMatrix* __cdecl Transform(RwMatrix* matrix,const RwMatrix* parent,RwOpCombineType type){trace.push_back({"transform",int(type),std::bit_cast<uint32>(parent->pos.x),std::bit_cast<uint32>(parent->pos.y),std::bit_cast<uint32>(parent->pos.z)});matrix->pos.x+=parent->pos.x;matrix->pos.y+=parent->pos.y;matrix->pos.z+=parent->pos.z;return matrix;}
'''+s[b:]
s=s.replace('{"CBike","FixHandsToBars",0x6B7F90,0,-1,1024}','{"CBike","SetupSuspensionLines",0,0x871360,48,1024}')
a=s.index('std::array<CBaseModelInfo*,3> savedModels');b=s.index('uint32 pairs=',a)
s=s[:a]+'auto& modelSlot=CModelInfo::ms_modelInfoPtrs[400];const auto savedModel=modelSlot;'+s[b:]
a=s.index(' EntryPatch h(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch t(0x7F25A0,Transform),td(bit_cast<uintptr_t>(&RwMatrixTransform),Transform);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> model{};alignas(16) std::array<uint8,sizeof(CColModel)> col{};alignas(16) std::array<uint8,sizeof(CCollisionData)> data{};alignas(16) std::array<uint8,sizeof(tHandlingData)> handling{};std::array<CColLine,4> lines{};std::array<CColSphere,3> spheres{};std::array<RwFrame,6> frames{};RpClump clump{};
   auto run=[&](bool candidate){trace=json::array();memset(object.data(),0xCD,object.size());memset(model.data(),0,model.size());memset(col.data(),0,col.size());memset(data.data(),0,data.size());memset(handling.data(),0,handling.size());memset(lines.data(),0,sizeof(lines));memset(spheres.data(),0,sizeof(spheres));memset(frames.data(),0,sizeof(frames));memset(&clump,0,sizeof(clump));
    auto* self=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_nModelIndex=400;self->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());auto* mi=reinterpret_cast<CVehicleModelInfo*>(model.data());auto* cm=reinterpret_cast<CColModel*>(col.data());auto* dc=reinterpret_cast<CCollisionData*>(data.data());modelSlot=mi;*reinterpret_cast<void**>(model.data()+0x14)=cm;cm->m_pColData=dc;dc->m_pLines=lines.data();dc->m_nNumSpheres=uint16(i%4);dc->m_pSpheres=spheres.data();
    const bool authored=i%2!=0;for(size_t j=0;j<4;++j){lines[j].m_vecStart={float(j)*.25f,float(j)*.5f,authored?float(j)*.125f:std::bit_cast<float>(0x47C34FFFu)};lines[j].m_vecEnd={0.f,0.f,0.f};}if(i%3==0)lines[1].m_vecStart.z=std::bit_cast<float>(0x47C34FFFu);
    mi->m_fWheelSizeFront=float(i%7+1)*.25f;mi->m_fWheelSizeRear=float(i%5+1)*.25f;self->m_aWheelOrigHeights={-.25f,-.5f};self->m_pHandlingData->m_fSuspensionUpperLimit=.5f;self->m_pHandlingData->m_fSuspensionLowerLimit=-float(i%4+1)*.25f;self->m_pHandlingData->m_fSuspensionForceLevel=float(i%5+1)*.5f;self->m_pHandlingData->m_nHandlingFlags=static_cast<eVehicleHandlingFlags>(i%2?0x40000000u:0u);
    cm->m_boundBox.m_vecMin={-1.f,-2.f,-.125f};cm->m_boundBox.m_vecMax={6.f,8.f,0.f};cm->m_boundSphere.m_fRadius=i%3==0?20.f:1.f;
    for(size_t j=0;j<3;++j){spheres[j].m_vecCenter={0.f,0.f,float(int(j)-1)*.25f};spheres[j].m_fRadius=float(j+1)*.25f;}
    for(size_t j=0;j<6;++j){frames[j].modelling.right={1.f,0.f,0.f};frames[j].modelling.up={0.f,1.f,0.f};frames[j].modelling.at={0.f,0.f,1.f};frames[j].modelling.pos={float(j)*.25f,float(j)*.5f,float(j)*.25f};}
    clump.object.parent=&frames[5];*reinterpret_cast<void**>(object.data()+0x18)=&clump;std::ranges::fill(self->m_aBikeNodes,nullptr);self->m_aBikeNodes[BIKE_WHEEL_FRONT]=&frames[0];self->m_aBikeNodes[BIKE_WHEEL_REAR]=&frames[1];self->m_aBikeNodes[BIKE_FORKS_FRONT]=&frames[2];self->m_aBikeNodes[BIKE_FORKS_REAR]=i%5==0?nullptr:&frames[3];frames[0].object.parent=i%3==0?nullptr:&frames[4];frames[1].object.parent=i%4==0?nullptr:&frames[4];frames[4].object.parent=&frames[5];
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6B89B0)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(CBike*)>(address)(self);self->m_pHandlingData=nullptr;*reinterpret_cast<void**>(object.data()+0x18)=nullptr;std::ranges::fill(self->m_aBikeNodes,nullptr);
    if(!candidate)++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"lines",Hash(lines.data(),sizeof(lines))},{"spheres",Hash(spheres.data(),sizeof(spheres))},{"box",Hash(&cm->m_boundBox,sizeof(cm->m_boundBox))},{"radius",std::bit_cast<uint32>(cm->m_boundSphere.m_fRadius)},{"trace",trace}};
'''+s[b:]
s=s.replace(' for(size_t j=0;j<3;++j)CModelInfo::ms_modelInfoPtrs[models[j]]=savedModels[j];',' modelSlot=savedModel;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/BikeHands355',fn).read_text().replace('bike-hands355','bike-setup355').replace('BikeHands355Live','BikeSetup355Live')
 if fn=='build_probe.py':s=s.replace("'Models/ModelInfo.h'","'Models/ModelInfo.h','Collision/CollisionData.h','Collision/ColModel.h','Collision/ColLine.h','tHandlingData.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('SetupSuspensionLines 0x6B89B0 VMT871360 slot48:1024 pares pendientes. ASM integral Bike_SetupFull; combo no pseudocodigo. Existing/sentinel lines, front/rear wheels and nested frame transforms, optional rear fork, cached origins/fork offsets/swing length, upper/lower/force levels, sphere radius/contact clamping and bounding updates. Full object/lines/spheres/box hashes and transform trace, modelslot400 restored. Fixtures finite nonzero force and valid four lines; max bounding corner6/8/0 gives exact radius10 to isolate suspension. Invalid frames/empty lines/NaN not covered. New hook false until runtime.\n')
