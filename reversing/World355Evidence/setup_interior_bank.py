from pathlib import Path
p=Path('reversing/InteriorCore355');p.mkdir(exist_ok=True)
for name in ['build_probe.py','run_local.py']:
 s=Path('reversing/ProjectileCore355',name).read_text(encoding='utf-8-sig').replace('ProjectileCore355','InteriorCore355').replace('projectile-core355','interior-core355')
 if name=='build_probe.py':s=s.replace("'source/game_sa/ProjectileInfo.cpp', 'source/game_sa/ProjectileInfo.h', 'source/game_sa/Entity/Object/Projectile.h'","'source/game_sa/Interior/InteriorGroup_c.cpp', 'source/game_sa/Interior/InteriorGroup_c.h', 'source/game_sa/Interior/Interior_c.h', 'source/game_sa/Interior/InteriorInfo_t.h'")
 (p/name).write_text(s,encoding='utf8')
s=Path('reversing/DecisionPending/LiveComparison.hpp').read_text().replace('DecisionPendingLive','InteriorCore355Live').replace('decision-pending','interior-core355').replace('#include "DecisionMakers/Decision.h"','#include "Interior/InteriorGroup_c.h"\n#include "Interior/Interior_c.h"')
start=s.index('inline const std::array<Fn,2>');end=s.index('inline ReversibleHooks',start)
names=[('Init',0x5947E0),('AddInterior',0x594840),('GetPed',0x594B90),('GetEntity',0x594BD0),('ContainsInteriorType',0x595250),('GetNumInteriorInfos',0x594920),('GetRandomInterior',0x5948C0),('FindInteriorInfo',0x594970),('CalcIsVisible',0x595200),('Setup',0x595320)]
s=s[:start]+'inline const std::array<Fn,10> fns{{\n'+''.join('{"InteriorGroup_c","%s",0x%X,0,-1,512},\n'%(n,a) for n,a in names)+'}};\n'+s[end:]
start=s.index('    alignas(CDecision)');end=s.index('\n   };',start)
s=s[:start]+'''    alignas(InteriorGroup_c) std::array<uint8,sizeof(InteriorGroup_c)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<InteriorGroup_c*>(raw.data());
    alignas(Interior_c) std::array<std::array<uint8,sizeof(Interior_c)>,8> interiors{};
    std::array<std::array<uint8,64>,8> boxes{};
    for(size_t k=0;k<8;++k){auto* in=reinterpret_cast<Interior_c*>(interiors[k].data());self->m_interiors[k]=(i+k)%3==0?nullptr:in;in->m_box=reinterpret_cast<tEffectInterior*>(boxes[k].data());boxes[k][0]=uint8((i+k)%8);in->m_interiorInfosCount=int8((i+k)%9-2);for(int j=0;j<16;++j){auto* info=&in->m_interiorInfos[j];*reinterpret_cast<int8*>(&info->Type)=int8((i+j+k)%13-2);info->IsInUse=(i+j)%3==0;}}
    self->m_numInteriors=uint8(i%9);self->m_isVisible=(i%2)!=0;self->m_lastIsVisible=(i%3)!=0;
    if(fn.address==0x595200)for(size_t k=0;k<8;++k)self->m_interiors[k]=reinterpret_cast<Interior_c*>(interiors[k].data());
    for(size_t k=0;k<16;++k)self->m_peds[k]=reinterpret_cast<CPed*>(0x100000+uint32(k)*256);
    self->m_pEntity=reinterpret_cast<CEntity*>(0x223344);
    rngValue=uint32((i*53)%32768);trace=json::array();base=uintptr_t(interiors.data());
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    uintptr_t result=0;InteriorInfo_t* outInfo=reinterpret_cast<InteriorInfo_t*>(0x123);Interior_c* outInterior=reinterpret_cast<Interior_c*>(0x456);
    if(fn.address==0x5947E0)reinterpret_cast<void(__thiscall*)(InteriorGroup_c*,CEntity*,int32)>(fn.address)(self,reinterpret_cast<CEntity*>(0x665544),i*37);
    else if(fn.address==0x594840)result=reinterpret_cast<int32(__thiscall*)(InteriorGroup_c*,Interior_c*)>(fn.address)(self,reinterpret_cast<Interior_c*>(interiors[0].data()));
    else if(fn.address==0x594B90)result=uintptr_t(reinterpret_cast<CPed*(__thiscall*)(InteriorGroup_c*,int32)>(fn.address)(self,i%16));
    else if(fn.address==0x594BD0)result=uintptr_t(reinterpret_cast<CEntity*(__thiscall*)(InteriorGroup_c*)>(fn.address)(self));
    else if(fn.address==0x595250)result=reinterpret_cast<int8(__thiscall*)(InteriorGroup_c*,int32)>(fn.address)(self,i%10-1);
    else if(fn.address==0x594920)result=reinterpret_cast<int32(__thiscall*)(InteriorGroup_c*,int32)>(fn.address)(self,i%13-2);
    else if(fn.address==0x5948C0)result=uintptr_t(reinterpret_cast<int32(__thiscall*)(InteriorGroup_c*)>(fn.address)(self));
    else if(fn.address==0x594970)result=reinterpret_cast<bool(__thiscall*)(InteriorGroup_c*,eInteriorInfoType,InteriorInfo_t**,Interior_c**)>(fn.address)(self,eInteriorInfoType(i%13-2),&outInfo,&outInterior);
    else reinterpret_cast<void(__thiscall*)(InteriorGroup_c*)>(fn.address)(self);
    return json{{"bytes",ReversingProbe::Bytes(raw.data(),raw.size())},{"result",result},{"out_info",uintptr_t(outInfo)},{"out_interior",uintptr_t(outInterior)},{"trace",trace},{"switches",switched}};''' +s[end:]
s=s.replace(' bool passed=true;uint32 pairCount=0;',''' bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x821B1E,Random),ad(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch b(0x5929F0,Visible),bd(bit_cast<uintptr_t>(&Interior_c::IsVisible),Visible);
 EntryPatch c(0x5950D0,Anims),cd(bit_cast<uintptr_t>(&InteriorGroup_c::ReferenceAnims),Anims);''')
s=s.replace('inline bool done{};', '''inline uint32 rngValue{};inline uintptr_t base{};
inline int32 __cdecl Random(){trace.push_back({"random",rngValue});return int32(rngValue);}
inline bool __fastcall Visible(Interior_c* self,void*){const auto index=(uintptr_t(self)-base)/sizeof(Interior_c);trace.push_back({"visible",index});return index%3==rngValue%3;}
inline void __fastcall Anims(InteriorGroup_c*,void*){trace.push_back({"reference_anims"});}
inline bool done{};''')
# Trace must be declared before stub definitions.
s=s.replace('inline uint32 rngValue{};', 'inline json trace;inline uint32 rngValue{};').replace('inline bool done{}; inline std::ofstream log; inline json trace;','inline bool done{}; inline std::ofstream log;')
(p/'LiveComparison.hpp').write_text(s,encoding='utf8')
