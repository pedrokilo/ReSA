from pathlib import Path
p=Path('reversing/BikeRoad355');p.mkdir(exist_ok=True)
s=Path('reversing/BikeBurst355/LiveComparison.hpp').read_text().replace('BikeBurst355Live','BikeRoad355Live').replace('bike-burst355','bike-road355').replace('#include "CarCtrl.h"','#include "CarCtrl.h"\n#include "World.h"')
a=s.index('inline int scenario');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline int scenario{},colCalls{},lineCalls{};inline CBike* current{};
alignas(16) inline std::array<uint8,sizeof(CColModel)> col{};
alignas(16) inline std::array<uint8,sizeof(CEntity)> hitEntity{};
inline CColModel* __fastcall Col(void*,void*){trace.push_back({"col",colCalls++});return reinterpret_cast<CColModel*>(col.data());}
inline bool __cdecl Vertical(const CVector& origin,float end,CColPoint& point,CEntity*& entity,bool buildings,bool vehicles,bool peds,bool objects,bool dummies,bool see,CStoredCollPoly* poly){
 const auto call=lineCalls++;const auto hit=(scenario>>call)&1;
 trace.push_back({"vertical",call,std::bit_cast<uint32>(origin.x),std::bit_cast<uint32>(origin.y),std::bit_cast<uint32>(origin.z),std::bit_cast<uint32>(end),buildings,vehicles,peds,objects,dummies,see,poly!=nullptr,hit});
 current->GetPosition().z+=.25f;point.m_vecPoint={origin.x,origin.y,float(scenario%17-8)*.25f+float(call)*.5f};point.m_nLightingB.value=uint8(scenario*17+call);entity=reinterpret_cast<CEntity*>(hitEntity.data());return hit!=0;
}
inline float __fastcall Height(void*,void*){trace.push_back({"height"});if(scenario%7==0)current->m_matrix=nullptr;return float(scenario%11)*.125f;}
'''+s[b:]
s=s.replace('{"CBike","BurstTyre",0,0x871360,44,1024}','{"CBike","PlaceOnRoadProperly",0x6BEEB0,0,-1,1024}')
a=s.index(' EntryPatch r(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch c(0x535300,Col),cd(bit_cast<uintptr_t>(&CEntity::GetColModel),Col);
 EntryPatch v(0x5674E0,Vertical),vd(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),Vertical);
 const auto heightHook=FindHook("CBike","GetHeightAboveRoad");
 const auto heightOur=heightHook?static_cast<ReversibleHooks::ReversibleHook::Virtual*>(heightHook)->GetHookOurAddress():0;
 if(!heightOur){Write({{"event","error"},{"reason","missing height body"}});return;}
 EntryPatch h(0x6B58B0,Height),hd(heightOur,Height);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,0x80> matrix{};
   auto run=[&](bool candidate){scenario=i;colCalls=lineCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(matrix.data(),0,matrix.size());memset(col.data(),0,col.size());memset(hitEntity.data(),0,hitEntity.size());
    current=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(current)=0x871360;current->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());
    auto* model=reinterpret_cast<CColModel*>(col.data());model->m_boundBox.m_vecMax.y=float(i%7+1)*.5f;model->m_boundBox.m_vecMin.y=-float((i/7)%7+1)*.5f;
    *reinterpret_cast<CVector*>(matrix.data()+0x10)={float(i%9-4)*.25f,1.f,float(i%7-3)*.125f};*reinterpret_cast<CVector*>(matrix.data()+0x30)={float(i%13-6),float(i%17-8),float(i%19-9)*.5f};
    *reinterpret_cast<uint32*>(hitEntity.data()+0x1C)=uint32(i%4)<<30;current->m_pEntityWeAreOn=nullptr;
    hooks[n]->State(candidate);const auto* bytes=reinterpret_cast<const uint8*>(fn.address);if((bytes[0]==0xE9)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(CBike*)>(fn.address)(current);const bool placement=current->m_matrix==nullptr;current->m_matrix=nullptr;current->m_pEntityWeAreOn=nullptr;
    if(!candidate&&lineCalls==2)++positiveCases;
    return json{{"switch",true},{"placement",placement},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"trace",trace}};
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/BikeBurst355',fn).read_text().replace('bike-burst355','bike-road355').replace('BikeBurst355Live','BikeRoad355Live')
 if fn=='build_probe.py':s=s.replace("'Entity/Physical.h'","'Entity/Physical.h','World.cpp','World.h','Collision/ColPoint.h','Core/Matrix.cpp','Core/Matrix.h','Core/Vector.cpp','Core/Vector.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('PlaceOnRoadProperly 0x6BEEB0: 1024 pares pendientes EXE. Combo pseudocodigo y ASM Bike_6BEEB0/Bike_RoadFull. ColModel dos llamadas, rayos delante/atras con hit/miss independientes, mutacion de Z entre llamadas, lighting y flags C0000000, base x87 fpatan/fcos/fsin exacta, midpoint Z redondeado antes multiplicar .5, altura virtual con cambio a placement. Hash objeto completo y matriz, trazas parametros y orden. Valores finitos, matriz inicialmente valida, span positivo; no pretende validar NaN ni span cero. Altura fixture binaria exacta. Hooks nuevos false hasta runtime.\n')
