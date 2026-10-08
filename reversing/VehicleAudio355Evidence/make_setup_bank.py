from pathlib import Path
p=Path('reversing/MonsterSetup355');p.mkdir(exist_ok=True)
s=Path('reversing/MonsterControl355/LiveComparison.hpp').read_text()
s=s.replace('MonsterControl355Live','MonsterSetup355Live').replace('monster-control355','monster-setup355')
s=s.replace('inline void __fastcall BaseControl', 'inline CVector wheels[4];\ninline std::array<CColDisk,4> allocated;\ninline void __fastcall Wheel(void*,void*,int32 i,CVector& out,bool local){out=wheels[i];trace.push_back({"wheel",i,local});}\ninline void* __cdecl Allocate(size_t bytes,uint32 hint){trace.push_back({"malloc",bytes,hint});return allocated.data();}\ninline void __cdecl Free(void*){trace.push_back({"free"});}\ninline void __fastcall BaseControl')
a=s.index('inline const std::array<Fn,4>');b=s.index('inline void Update()',a)
s=s[:a]+'inline const std::array<Fn,1> fns{{{"CMonsterTruck","SetupSuspensionLines",0,0x8717D8,48,1024}}};\n'+s[b:]
s=s.replace('Base*,4>','Base*,1>').replace('bool,4>','bool,1>').replace('n<4','n<1')
a=s.index(' EntryPatch c(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch w(0x4C7D20,Wheel),wd(bit_cast<uintptr_t>(&CVehicleModelInfo::GetWheelPosn),Wheel);
 EntryPatch a(0x72F420,Allocate),ad(bit_cast<uintptr_t>(static_cast<void*(*)(size_t,uint32)>(&CMemoryMgr::Malloc)),Allocate);
 EntryPatch f(0x72F430,Free),fd(bit_cast<uintptr_t>(&CMemoryMgr::Free),Free);
 auto& modelSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+400*4);const auto oldModel=modelSlot;
''' +s[b:]
a=s.index('    CTimer::ms_fTimeStep=');b=s.index('    hooks[n]->State',a)
s=s[:a]+'''    alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> mi{};auto* model=reinterpret_cast<CVehicleModelInfo*>(mi.data());
    alignas(16) std::array<uint8,sizeof(CColModel)> cm{};auto* col=reinterpret_cast<CColModel*>(cm.data());
    alignas(16) std::array<uint8,sizeof(CCollisionData)> dc{};auto* data=reinterpret_cast<CCollisionData*>(dc.data());
    tHandlingData handling{};std::array<CColDisk,4> disks;memset(disks.data(),0xCD,sizeof(disks));memset(allocated.data(),0xCD,sizeof(allocated));
    modelSlot=model;model->m_pColModel=col;model->m_fWheelSizeFront=float(i%12+1)*.125f;col->m_pColData=data;
    data->m_pDisks=i%3==0?nullptr:disks.data();data->bUsesDisks=i%3==2;data->m_nNumLines=7;
    self->m_nModelIndex=400;self->m_pHandlingData=&handling;
    handling.m_fSuspensionForceLevel=float(i%9+1)*.25f;handling.m_fSuspensionUpperLimit=.75f;handling.m_fSuspensionLowerLimit=-.5f;
    col->m_boundBox.m_vecMin=CVector{-1.f,-2.f,float(i%8-5)*.25f};col->m_boundBox.m_vecMax=CVector{1.f,2.f,1.f};col->m_boundSphere.m_fRadius=float(i%5);
    for(int j=0;j<4;++j)wheels[j]=CVector{float(j-2)*.25f,float(j%2)*.5f,float((i+j)%11)*.125f};
''' +s[b:]
s=s.replace('0x6C7D80,0x6C7D40,0x6C8250,0x6C8330','0x6C7FB0,0,0,0')
s=s.replace('return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"trace",trace}};', 'self->m_pHandlingData=nullptr;return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"disks",Hash(data->m_pDisks,4*sizeof(CColDisk))},{"flags",data->bUsesDisks},{"lines",data->m_nNumLines},{"box",Hash(&col->m_boundBox,sizeof(col->m_boundBox))},{"sphere",Hash(&col->m_boundSphere,sizeof(col->m_boundSphere))},{"trace",trace}};')
s=s.replace(' }\n }\n CTimer::', ' }\n modelSlot=oldModel;\n }\n CTimer::')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/MonsterControl355',fn).read_text().replace('MonsterControl355Live','MonsterSetup355Live').replace('monster-control355','monster-setup355').replace('Monster control 355','Monster setup 355')
 if fn=='build_probe.py':s=s.replace("'Timer.h'","'Timer.h','Models/VehicleModelInfo.cpp','MemoryMgr.cpp','Collision/ColDisk.h','Collision/ColModel.h','Collision/CollisionData.h','tHandlingData.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('Banco SetupSuspensionLines (VMT 0x8717D8 slot 48, nativo 0x6C7FB0): 1.024 pares. Pendiente de ejecución. Fixture aislado con discos existentes, ausentes o líneas anteriores; asignación/liberación y posiciones de ruedas sustituidas simétricamente. Compara objeto, discos, límites, esfera y trazas; restaura entrada de modelo 400 y hooks. ASM íntegro: ../VehicleAudio355Evidence/Monster_SetupFull.txt.\n')
