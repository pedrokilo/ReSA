from pathlib import Path
p=Path('reversing/BikeWheel355');p.mkdir(exist_ok=True)
s=Path('reversing/MonsterSetup355/LiveComparison.hpp').read_text().replace('MonsterSetup355Live','BikeWheel355Live').replace('monster-setup355','bike-wheel355').replace('#include "MonsterTruck.h"','#include "Bike.h"')
a=s.index('inline CVector wheels');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''alignas(16) inline std::array<uint8,sizeof(CColModel)> originalCol{};
inline CColModel* __fastcall Col(void*,void*){trace.push_back({"col"});return reinterpret_cast<CColModel*>(originalCol.data());}
inline RwMatrix* __cdecl Transform(RwMatrix* matrix,const RwMatrix* parent,RwOpCombineType type){trace.push_back({"transform",int(type),std::bit_cast<uint32>(parent->pos.x),std::bit_cast<uint32>(parent->pos.y),std::bit_cast<uint32>(parent->pos.z)});matrix->pos.x+=parent->pos.x;matrix->pos.y+=parent->pos.y;matrix->pos.z+=parent->pos.z;return matrix;}
'''+s[b:]
s=s.replace('{"CMonsterTruck","SetupSuspensionLines",0,0x8717D8,48,1024}','{"CBike","SetUpWheelColModel",0,0x871360,70,1024}')
a=s.index(' const auto oldStep=');b=s.index(' for(size_t n=',a)
s=s[:a]+''' auto& modelSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+400*4);const auto oldModel=modelSlot;uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch c(0x535300,Col),cd(bit_cast<uintptr_t>(&CEntity::GetColModel),Col);
 EntryPatch t(0x7F25A0,Transform),td(bit_cast<uintptr_t>(&RwMatrixTransform),Transform);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> mi{};alignas(16) std::array<uint8,sizeof(CColModel)> cm{};alignas(16) std::array<uint8,sizeof(CCollisionData)> dc{};std::array<CColSphere,2> spheres;std::array<RwFrame,5> frames{};
   auto run=[&](bool candidate){trace=json::array();memset(object.data(),0xCD,object.size());memset(mi.data(),0,mi.size());memset(cm.data(),0xCD,cm.size());memset(dc.data(),0xCD,dc.size());memset(spheres.data(),0xCD,sizeof(spheres));memset(originalCol.data(),0xCD,originalCol.size());memset(frames.data(),0,sizeof(frames));
    auto* self=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_nModelIndex=400;
    auto* model=reinterpret_cast<CVehicleModelInfo*>(mi.data());modelSlot=model;model->m_fWheelSizeFront=float(i%15+1)*.125f;model->m_fWheelSizeRear=float(i%11+1)*.125f;
    auto* col=reinterpret_cast<CColModel*>(cm.data());auto* data=reinterpret_cast<CCollisionData*>(dc.data());col->m_pColData=data;data->m_pSpheres=spheres.data();
    auto* source=reinterpret_cast<CColModel*>(originalCol.data());source->m_boundBox.m_vecMin=CVector{-1.f,-2.f,float(i%9-4)};source->m_boundBox.m_vecMax=CVector{1.f,2.f,float(i%11)};source->m_boundSphere.m_vecCenter=CVector{3.f,4.f,5.f};source->m_boundSphere.m_fRadius=float(i%7);
    for(int j=0;j<5;++j){frames[j].modelling.right={1.f,0.f,0.f};frames[j].modelling.up={0.f,1.f,0.f};frames[j].modelling.at={0.f,0.f,1.f};frames[j].modelling.pos={float(i%7+j),float(j-2),float(i%11-j)};}
    self->m_aBikeNodes[BIKE_CHASSIS]=&frames[0];self->m_aBikeNodes[BIKE_WHEEL_FRONT]=&frames[1];self->m_aBikeNodes[BIKE_WHEEL_REAR]=&frames[2];
    frames[1].object.parent=i%3==0?nullptr:i%3==1?&frames[0]:&frames[3];frames[2].object.parent=i%4==0?nullptr:i%4==1?&frames[0]:&frames[4];frames[3].object.parent=&frames[0];frames[4].object.parent=i%4==3?&frames[3]:&frames[0];
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6B67E0)!=candidate)return json{{"switch",false}};
    const auto ret=reinterpret_cast<bool(__thiscall*)(void*,CColModel*)>(address)(self,col);std::ranges::fill(self->m_aBikeNodes,nullptr);
    return json{{"switch",true},{"return",ret},{"object",Hash(object.data()+4,object.size()-4)},{"spheres",Hash(spheres.data(),sizeof(spheres))},{"count",data->m_nNumSpheres},{"box",Hash(&col->m_boundBox,sizeof(col->m_boundBox))},{"sphere",Hash(&col->m_boundSphere,sizeof(col->m_boundSphere))},{"trace",trace}};
'''+s[b:]
a=s.index(' CTimer::ms_fTimeStep=');b=s.index(' for(size_t n=',a);s=s[:a]+' bool restored=modelSlot==oldModel;\n'+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/MonsterSetup355',fn).read_text().replace('MonsterSetup355Live','BikeWheel355Live').replace('monster-setup355','bike-wheel355')
 if fn=='build_probe.py':
  a=s.index('SOURCES=');b=s.index("if __name__",a);s=s[:a]+"SOURCES=[G+p for p in ('Entity/Vehicle/Bike.cpp','Entity/Vehicle/Bike.h','Entity/Entity.cpp','Entity/Entity.h','Models/VehicleModelInfo.h','Collision/ColSphere.cpp','Collision/ColSphere.h','RenderWare/rw/rwcore.cpp','RenderWare/rw/rwcore.h')]\n"+s[b:]
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('SetUpWheelColModel VMT 0x871360: 1.024 pares preparados, pendiente ejecución. Radios distintos, caja/esfera conservadas y jerarquías de frame ausentes, raíz y 1-2 padres; transformaciones sustituidas simétricamente y registradas. Compara esferas/metadatos completos y contador. Evidencia ASM íntegro ../VehicleAudio355Evidence/Bike_WheelColFull.txt.\n')
