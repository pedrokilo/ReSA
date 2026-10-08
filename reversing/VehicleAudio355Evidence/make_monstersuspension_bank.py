from pathlib import Path
p=Path('reversing/MonsterSuspension355');p.mkdir(exist_ok=True)
s=Path('reversing/MonsterControl355/LiveComparison.hpp').read_text().replace('MonsterControl355Live','MonsterSuspension355Live').replace('monster-control355','monster-suspension355')
a=s.index('inline void __fastcall BaseControl');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''alignas(16) inline std::array<std::array<uint8,sizeof(CVehicle)>,4> entities{};
inline int Owner(void* p){for(int i=0;i<4;++i)if(p==entities[i].data())return i+1;return 0;}
inline json Vector(const CVector& v){return json::array({std::bit_cast<uint32>(v.x),std::bit_cast<uint32>(v.y),std::bit_cast<uint32>(v.z)});}
inline bool __fastcall Spring(CPhysical* self,void*,float force,CVector& direction,CVector& point,float compression,float bias,CVector& normal,float& limit){trace.push_back({"spring",std::bit_cast<uint32>(force),Vector(direction),Vector(point),std::bit_cast<uint32>(compression),std::bit_cast<uint32>(bias),Vector(normal)});limit=force*(1.f-compression)*bias;self->m_vecMoveSpeed.x+=force*.01f;return true;}
inline CVector* __fastcall Speed(CPhysical* self,void*,CVector* out,CVector point){trace.push_back({"speed",Owner(self),Vector(point)});*out=self->m_vecMoveSpeed+point*.125f;return out;}
inline bool __fastcall Damp(CPhysical* self,void*,float force,float limit,CVector& direction,CVector& point,CVector& speed){trace.push_back({"damp",std::bit_cast<uint32>(force),std::bit_cast<uint32>(limit),Vector(direction),Vector(point),Vector(speed)});self->m_vecTurnSpeed+=speed*.03125f;return true;}
inline void __fastcall Damage(CVehicle* self,void*,float intensity,eVehicleCollisionComponent piece,CEntity*,CVector* point,CVector* normal,eWeaponType weapon){trace.push_back({"damage",Owner(self),std::bit_cast<uint32>(intensity),int(piece),Vector(*point),Vector(*normal),int(weapon)});self->m_fMass+=.125f;}
inline void __fastcall Force(CPhysical* self,void*,CVector force,CVector point,bool update){trace.push_back({"force",Owner(self),Vector(force),Vector(point),update});self->m_vecMoveSpeed+=force*.001f;}
'''+s[b:]
a=s.index('inline const std::array<Fn,4>');b=s.index('inline void Update()',a);s=s[:a]+'inline const std::array<Fn,1> fns{{{"CMonsterTruck","ProcessSuspension",0,0x8717D8,70,1024}}};\n'+s[b:]
s=s.replace('Base*,4>','Base*,1>').replace('bool,4>','bool,1>').replace('n<4','n<1')
a=s.index(' const auto oldStep=');b=s.index(' for(size_t n=',a)
s=s[:a]+''' uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x543D60,Spring),ad(bit_cast<uintptr_t>(&CPhysical::ApplySpringCollisionAlt),Spring);
 EntryPatch b(0x542CE0,Speed),bd(bit_cast<uintptr_t>(&CPhysical::GetSpeed),Speed);
 EntryPatch c(0x543E90,Damp),cd(bit_cast<uintptr_t>(&CPhysical::ApplySpringDampening),Damp);
 EntryPatch d(0x542B50,Force),dd(bit_cast<uintptr_t>(&CPhysical::ApplyForce),Force);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CMonsterTruck)> object{};alignas(16) std::array<std::array<uint8,0x80>,5> matrices{};std::array<uintptr_t,71> vtable{};vtable[56]=bit_cast<uintptr_t>(&Damage);tHandlingData handling{};
   auto run=[&](bool candidate){trace=json::array();memset(object.data(),0xCD,object.size());memset(entities.data(),0xCD,sizeof(entities));memset(matrices.data(),0,sizeof(matrices));auto* self=reinterpret_cast<CMonsterTruck*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x8717D8;*reinterpret_cast<uintptr_t*>(object.data()+0x14)=uintptr_t(matrices[0].data());
    *reinterpret_cast<CVector*>(matrices[0].data()+0x20)=CVector{.125f,.25f,1.f};*reinterpret_cast<CVector*>(matrices[0].data()+0x30)=CVector{1.f,2.f,3.f};self->m_pHandlingData=&handling;handling.m_fSuspensionForceLevel=float(i%7+1)*.125f;handling.m_fSuspensionDampingLevel=float(i%5)*.0625f;handling.m_fSuspensionBiasBetweenFrontAndRear=float(i%5)*.25f;
    self->m_vecMoveSpeed=CVector{float(i%11-5)*.125f,.25f,-.125f};self->m_vecTurnSpeed=CVector{};self->m_fMass=100.f+float(i%13)*20.f;
    for(int j=0;j<4;++j){auto* entity=reinterpret_cast<CVehicle*>(entities[j].data());*reinterpret_cast<uintptr_t*>(entity)=uintptr_t(vtable.data());*reinterpret_cast<uintptr_t*>(entities[j].data()+0x14)=(i+j)%2?uintptr_t(matrices[j+1].data()):0;entities[j][0x36]=uint8((i+j)%3==0?3:2);*reinterpret_cast<CVector*>(entities[j].data()+4)=CVector{float(j),2.f,-1.f};*reinterpret_cast<CVector*>(matrices[j+1].data()+0x30)=CVector{float(j),2.f,-1.f};entity->m_fMass=50.f+float(j)*10.f;entity->m_vecMoveSpeed=CVector{float(j)*.25f,-.125f,.25f};
     self->m_apWheelCollisionEntity[j]=(i+j)%4==0?nullptr:entity;self->m_vWheelCollisionPos[j]=CVector{float(j),-.5f,.125f};self->m_fWheelsSuspensionCompression[j]=std::array<float,8>{-1.f,0.f,.25f,.5f,.75f,1.f,1.25f,std::numeric_limits<float>::quiet_NaN()}[(i+j)%8];auto& cp=self->m_wheelColPoint[j];cp.m_vecPoint=CVector{float(j)+2.f,float(i%7),.5f};cp.m_vecNormal=CVector{.25f,-.125f,std::array<float,5>{0.f,.25f,.35f,.5f,1.f}[(i+j)%5]};cp.m_nPieceTypeB=uint8(13+j);
    }
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6C83A0)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(address)(self);self->m_pHandlingData=nullptr;for(int j=0;j<4;++j)*reinterpret_cast<uintptr_t*>(entities[j].data())=0;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"entities",Hash(entities.data(),sizeof(entities))},{"trace",trace}};
'''+s[b:]
a=s.index(' CTimer::ms_fTimeStep=');b=s.index(' for(size_t n=',a);s=s[:a]+' bool restored=true;\n'+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/MonsterControl355',fn).read_text().replace('MonsterControl355Live','MonsterSuspension355Live').replace('monster-control355','monster-suspension355')
 if fn=='build_probe.py':s=s.replace("'Timer.h'","'Timer.h','Entity/Physical.cpp','Entity/Physical.h','tHandlingData.h','Collision/ColPoint.h','Core/Vector.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('ProcessSuspension VMT 0x8717D8 slot70 (0x6C83A0): 1.024 pares preparados, pendiente ejecución. Cuatro ruedas, compresión negativa/cero/parcial/1/mayor/NaN, normales z0/.25/.35/.5/1, entidades nulas/vehículo/ped, posiciones placeable/matriz. Springs, GetSpeed, dampening y force simétricos, VehicleDamage vía VMT de fixtures. Spring altera velocidad antes del pase GetSpeed; Damage altera masa antes de fuerza para verificar orden. Compara objeto y entidades completos/trazas; ancho XY quarter, Z completo según ASM. Ninguna entidad real. Evidencia ../VehicleAudio355Evidence/Monster_SuspensionFull.txt.\n')
