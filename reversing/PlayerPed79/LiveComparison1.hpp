#pragma once
// Funcion 1 del banco PlayerPed79: CPlayerPed::FindWeaponLockOnTarget (0x60DC50, hook S). Incluido desde LiveComparison.hpp (comparte arenas, fakes de referencias y la ejecucion).
// Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y, si tienen hook, en el cuerpo de la DLL): CPed::GetWeaponSkill() 0x5E6580, CWeaponInfo::GetWeaponInfo 0x743C60,
// CWeapon::TargetWeaponRangeMultiplier 0x73B380, CGeneral::GetATanOfXY 0x53CC70, GetRadianAngleBetweenPoints 0x53CBE0, LimitRadianAngle 0x53CB50, CPad::GetPedWalkLeftRight() 0x53FC90 y
// GetPedWalkUpDown() 0x53FD30, CVector::Normalise 0x59C910 y Magnitude 0x4082C0 (sin hook: InjectHooksMain.cpp tiene CVector::InjectHooks() comentado; falso en el EXE y en el cuerpo de la DLL), CPlayerPed::EvaluateTarget 0x60D020, LOSBlockedBetweenPeds 0x60B550, CPedGroups::AreInSameGroup 0x5F7F40,
// CTagManager::IsTag 0x49CCE0 y GetAlpha 0x49CF90, CWorld::FindObjectsInRange 0x564A20, CObject::CanBeTargetted 0x59F320, CPlayerPed::CanIKReachThisTarget 0x609F80, CGameLogic::IsCoopGameGoingOn 0x441390,
// CEntity::RegisterReference 0x571B70 y CleanUpOldReference 0x571A00 (todos con hook). Reales en ambos lados: CPad::GetPad 0x53FB70 (devuelve &Pads[n]) y la aritmetica en linea de la funcion.

namespace Pp79Live {
using namespace Pp79Fix;

// Indices de arena para las etiquetas de la funcion 1 (los de la funcion 0 no se mezclan: cada caso hace TagClear)
enum Tg1 : unsigned { T_PL, T_DATA, T_PLMAT, T_TGT, T_TGTMAT, T_INFO, T_PEDHDR, T_PEDFLAGS, T_PEDS, T_PEDMAT0, T_VEHHDR = T_PEDMAT0 + kPeds, T_VEHFLAGS, T_VEHS, T_VEHMAT0, T_OBJHDR = T_VEHMAT0 + kVehs, T_OBJFLAGS, T_OBJS,
                      T_OBJMAT0, T_BLDS = T_OBJMAT0 + kObjs, T_BLDMAT0 };

inline Arena<kPlayerArena> pl1;
inline Arena<kDataArena> data1;
inline Arena<Pp79Fix::Arena(kMatWin)> plMat1,tgtMat1;
inline Arena<Pp79Fix::Arena(kBldWin)> tgt1;
inline Arena<Pp79Fix::Arena(kInfoWin)> info1;
inline Arena<Pp79Fix::Arena(kHdrWin)> pedHdr1,vehHdr1,objHdr1;
inline Arena<Pp79Fix::Arena(kFlagsWin)> pedFlags1,vehFlags1,objFlags1;
inline Arena<Pp79Fix::Arena(kPedsWin)> peds1;
inline Arena<Pp79Fix::Arena(kVehsWin)> vehs1;
inline Arena<Pp79Fix::Arena(kObjsWin)> objs1;
inline Arena<Pp79Fix::Arena(kBldsWin)> blds1;
inline std::array<Arena<Pp79Fix::Arena(kMatWin)>,kPeds> pedMat1;
inline std::array<Arena<Pp79Fix::Arena(kMatWin)>,kVehs> vehMat1;
inline std::array<Arena<Pp79Fix::Arena(kMatWin)>,kObjs> objMat1;
inline std::array<Arena<Pp79Fix::Arena(kMatWin)>,kBlds> bldMat1;

constexpr size_t kPedStride=0x7C4,kVehStride=0xA18,kObjStride=0x19C,kBldStride=0x40;

struct State1 {
    const C1* c{};
    unsigned atanN{},multN{},magN{},evalN{},reachN{},lrN{},udN{};
    std::array<bool,kSites1+1> mutDone{};
};
inline State1 g1;
inline u32 padsBase{};
inline u32 PadTag(void* pad) {return u32((uintptr_t(pad)-padsBase)/0x134);}
inline void Mutate1(u32 site) {
    if(g1.c->mutSite!=site||g1.mutDone[site])return;
    g1.mutDone[site]=true;
    const u32 k=g1.c->mutKind;
    const auto slotOf=[]{return i32(int8(pl1.m[kGuard+0x718]));};
    if(k&1)pl1.Put32(0x71C,pl1.Get32(0x71C)?0u:u32(tgt1.Obj()));
    if(k&2){pl1.Put8(0x718,u8(g1.c->mutSlot));pl1.Put32(0x5A0+i32(int8(u8(g1.c->mutSlot)))*0x1C,g1.c->mutWtype);}
    if(k&4)info1.Put32(4,g1.c->mutRange);
    if(k&8)pl1.Put32(0x5A0+slotOf()*0x1C,g1.c->mutWtype);
}
inline u32 EntTag(const void* p) {return Tag(p);}
// ---- falsos (float: se pasan y se devuelven como patrones de 32 bits; los que devuelven float lo hacen por ST0 con un fld de dword)
inline u32 __fastcall Fk1Skill(void* self,void*) {Ev& e=PushEv(1,1);e.v[0]=Tag(self);Mutate1(1);return g1.c->skill;}                                                // 0x5E6580
inline void* __cdecl Fk1WInfo(u32 type,u32 skill) {Ev& e=PushEv(2,2);e.v[0]=type;e.v[1]=skill;Mutate1(2);return reinterpret_cast<void*>(info1.Obj());}                    // 0x743C60
inline float __cdecl Fk1Mult(void* target,void* owner) {Ev& e=PushEv(3,2);e.v[0]=Tag(target);e.v[1]=Tag(owner);Mutate1(3);return F(g1.c->mult[g1.multN++%12]);}            // 0x73B380
inline float __cdecl Fk1Atan(u32 x,u32 y) {Ev& e=PushEv(4,2);e.v[0]=x;e.v[1]=y;Mutate1(4);return F(g1.c->atan[g1.atanN++%12]);}                                       // 0x53CC70
inline u32 __fastcall Fk1WalkLR(void* self,void*) {Ev& e=PushEv(5,1);e.v[0]=PadTag(self);Mutate1(5);return g1.lrN++==0?g1.c->lr1:g1.c->lr2;}                          // 0x53FC90
inline u32 __fastcall Fk1WalkUD(void* self,void*) {Ev& e=PushEv(6,1);e.v[0]=PadTag(self);Mutate1(5);return g1.udN++==0?g1.c->ud1:g1.c->ud2;}                          // 0x53FD30
inline float __cdecl Fk1Between(u32 a,u32 b,u32 c,u32 d) {Ev& e=PushEv(7,4);e.v[0]=a;e.v[1]=b;e.v[2]=c;e.v[3]=d;Mutate1(6);return F(g1.c->between);}                   // 0x53CBE0
inline float __cdecl Fk1Limit(u32 a) {Ev& e=PushEv(8,1);e.v[0]=a;Mutate1(7);return F(g1.c->limitR);}                                                                  // 0x53CB50
inline void __fastcall Fk1Normalise(void* self,void*) {                                                                                                                // 0x59C910
    u32* v=static_cast<u32*>(self);Ev& e=PushEv(9,3);e.v[0]=v[0];e.v[1]=v[1];e.v[2]=v[2];v[0]=g1.c->norm[0];v[1]=g1.c->norm[1];v[2]=g1.c->norm[2];Mutate1(8);
}
inline float __fastcall Fk1Mag(void* self,void*) {const u32* v=static_cast<const u32*>(self);Ev& e=PushEv(10,3);e.v[0]=v[0];e.v[1]=v[1];e.v[2]=v[2];Mutate1(9);return F(g1.c->mag[g1.magN++%12]);}   // 0x4082C0
inline void __fastcall Fk1Eval(void* self,void*,void* target,u32* outTarget,u32* outPrio,u32 maxDist,u32 comp,u32 arg5) {                                              // 0x60D020, `ret 18h`
    Ev& e=PushEv(11,7);e.v[0]=Tag(self);e.v[1]=Tag(target);e.v[2]=Tag(uintptr_t(*outTarget));e.v[3]=*outPrio;e.v[4]=maxDist;e.v[5]=comp;e.v[6]=arg5&0xFFu;
    const unsigned k=g1.evalN++%12;
    if(g1.c->evalSet[k]){*outTarget=u32(uintptr_t(target));*outPrio=g1.c->evalPrio[k];}
    Mutate1(10);
}
inline u32 __cdecl Fk1Los(void* a,void* b) {Ev& e=PushEv(12,2);e.v[0]=Tag(a);e.v[1]=Tag(b);Mutate1(11);return (g1.c->los&1u)|0xA5A5A500u;}                              // 0x60B550
inline u32 __cdecl Fk1Group(void* a,void* b) {                                                                                                                         // 0x5F7F40
    Ev& e=PushEv(13,2);e.v[0]=Tag(a);e.v[1]=Tag(b);Mutate1(12);
    const uintptr_t off=uintptr_t(a)-peds1.Obj(0);   // peds1.Obj(0) es el inicio de la arena: el objeto i esta en kGuard + i*0x7C4
    const unsigned i=unsigned((off-kGuard)/kPedStride);
    return (i<kPeds?g1.c->ped[i].f&1u:0u)|0x5A5A5A00u;
}
inline u32 __cdecl Fk1IsTag(void* ent) {Ev& e=PushEv(14,1);e.v[0]=Tag(ent);Mutate1(13);const unsigned i=unsigned((uintptr_t(ent)-blds1.Obj(0)-kGuard)/kBldStride);return (i<kBlds?g1.c->bld[i].a&1u:0u)|0x77EE1100u;}   // 0x49CCE0
inline u32 __cdecl Fk1Alpha(void* ent) {Ev& e=PushEv(15,1);e.v[0]=Tag(ent);Mutate1(13);const unsigned i=unsigned((uintptr_t(ent)-blds1.Obj(0)-kGuard)/kBldStride);return (i<kBlds?g1.c->bld[i].b&0xFFu:0u)|0x5A5A5A00u;}   // 0x49CF90
inline void __cdecl Fk1Find(const u32* pt,u32 radius,u32 b2d,int16* count,u32 maxCount,void** out,u32 bld,u32 veh,u32 ped,u32 obj,u32 dum) {                              // 0x564A20
    Ev& e=PushEv(16,11);e.v[0]=pt[0];e.v[1]=pt[1];e.v[2]=pt[2];e.v[3]=radius;e.v[4]=b2d&0xFFu;e.v[5]=maxCount;e.v[6]=bld&0xFFu;e.v[7]=veh&0xFFu;e.v[8]=ped&0xFFu;e.v[9]=obj&0xFFu;e.v[10]=dum&0xFFu;
    *count=int16(g1.c->bldCount);
    for(unsigned i=0;i<g1.c->bldCount&&i<15;++i)out[i]=reinterpret_cast<void*>(blds1.Obj(kGuard+(g1.c->bldList[i]%kBlds)*kBldStride));
    Mutate1(14);
}
inline u32 __fastcall Fk1Targetted(void* obj,void*) {                                                                                                                  // 0x59F320
    Ev& e=PushEv(17,1);e.v[0]=Tag(obj);Mutate1(15);const unsigned i=unsigned((uintptr_t(obj)-objs1.Obj(0)-kGuard)/kObjStride);
    return (i<kObjs?g1.c->obj[i].a&1u:0u)|0x31000000u;
}
inline u32 __fastcall Fk1Reach(void* self,void*,u32 x,u32 y,u32 z,void* weapon,u32 flag) {                                                                              // 0x609F80, `ret 14h`
    Ev& e=PushEv(18,5);e.v[0]=x;e.v[1]=y;e.v[2]=z;e.v[3]=Tag(weapon);e.v[4]=flag&0xFFu;Mutate1(16);
    return (g1.c->reach[g1.reachN++%12]&1u)|0x5A5A5A00u;
}
inline u32 __cdecl Fk1Coop() {PushEv(19,0);Mutate1(17);return (g1.c->coop&1u)|0x1234AB00u;}                                                                            // 0x441390
inline void __fastcall Fk1Register(void* self,void*,void** ref) {Ev& e=PushEv(30,3);e.v[0]=Tag(self);e.v[1]=TagP(ref);e.v[2]=Tag(uintptr_t(ref?*ref:nullptr));}     // 0x571B70, `ret 4`
inline void __fastcall Fk1CleanUp(void* self,void*,void** ref) {Ev& e=PushEv(31,3);e.v[0]=Tag(self);e.v[1]=TagP(ref);e.v[2]=Tag(uintptr_t(ref?*ref:nullptr));}      // 0x571A00, `ret 4`

template<size_t N> inline void PutPos(Arena<N>& a,size_t base,const EntC& e,Arena<Pp79Fix::Arena(kMatWin)>& mat) {   // posicion en la matriz (+0x30) o en m_placement (+4): la otra queda con basura
    a.Put32(base+0x14,e.mt?u32(mat.Obj()):0u);
    const size_t at=e.mt?0x30:0;
    if(e.mt){mat.Put32(0x30,e.x);mat.Put32(0x34,e.y);mat.Put32(0x38,e.z);}
    else{a.Put32(base+4,e.x);a.Put32(base+8,e.y);a.Put32(base+0xC,e.z);}
    (void)at;
}

inline void CaseLock1(Env& env,Totals& t,unsigned n) {
    const C1 c=MakeC1(n);
    const json input=json::parse(JsonC1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        const u32 sd=c.seed*31u;
        pl1.Fill(sd+1);data1.Fill(sd+2);plMat1.Fill(sd+3);tgtMat1.Fill(sd+4);tgt1.Fill(sd+5);info1.Fill(sd+6);
        pedHdr1.Fill(sd+7);vehHdr1.Fill(sd+8);objHdr1.Fill(sd+9);pedFlags1.Fill(sd+10);vehFlags1.Fill(sd+11);objFlags1.Fill(sd+12);peds1.Fill(sd+13);vehs1.Fill(sd+14);objs1.Fill(sd+15);blds1.Fill(sd+16);
        for(unsigned i=0;i<kPeds;++i)pedMat1[i].Fill(sd+20+i);
        for(unsigned i=0;i<kVehs;++i)vehMat1[i].Fill(sd+30+i);
        for(unsigned i=0;i<kObjs;++i)objMat1[i].Fill(sd+40+i);
        for(unsigned i=0;i<kBlds;++i)bldMat1[i].Fill(sd+50+i);
        // jugador
        pl1.Put32(0x14,c.pmt?u32(plMat1.Obj()):0u);
        if(c.pmt){plMat1.Put32(0x10,c.fx);plMat1.Put32(0x14,c.fy);plMat1.Put32(0x18,c.fz);plMat1.Put32(0x30,c.px);plMat1.Put32(0x34,c.py);plMat1.Put32(0x38,c.pz);}
        else{pl1.Put32(4,c.px);pl1.Put32(8,c.py);pl1.Put32(0xC,c.pz);}
        pl1.Put32(0x10,c.hd);pl1.Put32(0x480,u32(data1.Obj()));pl1.Put32(0x598,c.ptype);pl1.Put8(0x718,u8(c.slot));pl1.Put32(0x71C,c.tgt?u32(tgt1.Obj()):0u);
        pl1.Put32(0x5A0+i32(int8(u8(c.slot)))*0x1C,c.wtype);
        info1.Put32(4,c.rng);
        // objetivo fijado
        {EntC te{};te.mt=c.tmt;te.x=c.tx;te.y=c.ty;te.z=c.tz;PutPos(tgt1,0,te,tgtMat1);}
        // pools
        for(unsigned i=0;i<kPeds;++i) {
            const EntC& e=c.ped[i];const size_t b=i*kPedStride;
            PutPos(peds1,b,e,pedMat1[i]);
            peds1.Put32(b+0x530,e.a);
            peds1.Put32(b+0x46C,(peds1.Get32(b+0x46C)&~0x100u)|(e.b?0x100u:0u));
            peds1.Put32(b+0x58C,e.c>=1&&e.c<=3?u32(vehs1.Obj(kGuard+(e.c-1)*kVehStride)):0u);
            peds1.Put32(b+0x470,(peds1.Get32(b+0x470)&~0x10000000u)|(e.d?0x10000000u:0u));
            peds1.Put32(b+0x598,e.e);
            pedFlags1.Put8(i,e.pr?u8(Rand(c.seed,900+i)&0x7Fu):u8(0x80u|(Rand(c.seed,910+i)&0x7Fu)));
        }
        for(unsigned i=0;i<kVehs;++i) {
            const EntC& e=c.veh[i];const size_t b=i*kVehStride;
            PutPos(vehs1,b,e,vehMat1[i]);
            vehs1.Put32(b+0x40,e.a);vehs1.Put32(b+0x594,e.b);vehs1.Put32(b+0x590,e.c);vehs1.Put8(b+0x42D,u8(e.d));
            vehFlags1.Put8(i,e.pr?u8(Rand(c.seed,920+i)&0x7Fu):u8(0x80u|(Rand(c.seed,930+i)&0x7Fu)));
        }
        for(unsigned i=0;i<kObjs;++i) {
            const EntC& e=c.obj[i];const size_t b=i*kObjStride;
            PutPos(objs1,b,e,objMat1[i]);
            objs1.Put32(b+0x18,e.c?0x00ABCD00u+i*0x10u:0u);objs1.Put8(b+0x140,u8(e.b));
            objFlags1.Put8(i,e.pr?u8(Rand(c.seed,940+i)&0x7Fu):u8(0x80u|(Rand(c.seed,950+i)&0x7Fu)));
        }
        for(unsigned i=0;i<kBlds;++i)PutPos(blds1,i*kBldStride,c.bld[i],bldMat1[i]);
        pedHdr1.Put32(0,u32(peds1.Obj()));pedHdr1.Put32(4,u32(pedFlags1.Obj()));pedHdr1.Put32(8,c.pedSize);
        vehHdr1.Put32(0,u32(vehs1.Obj()));vehHdr1.Put32(4,u32(vehFlags1.Obj()));vehHdr1.Put32(8,c.vehSize);
        objHdr1.Put32(0,u32(objs1.Obj()));objHdr1.Put32(4,u32(objFlags1.Obj()));objHdr1.Put32(8,c.objSize);
        *reinterpret_cast<uint32*>(0xB74490)=u32(pedHdr1.Obj());*reinterpret_cast<uint32*>(0xB74494)=u32(vehHdr1.Obj());*reinterpret_cast<uint32*>(0xB7449C)=u32(objHdr1.Obj());
        *reinterpret_cast<uint32*>(0xB6F178)=c.cam;*reinterpret_cast<uint8*>(0x96A8B2)=u8(c.twoPlayer);
        pl1.Snap();data1.Snap();plMat1.Snap();tgtMat1.Snap();tgt1.Snap();info1.Snap();pedHdr1.Snap();vehHdr1.Snap();objHdr1.Snap();pedFlags1.Snap();vehFlags1.Snap();objFlags1.Snap();peds1.Snap();vehs1.Snap();objs1.Snap();blds1.Snap();
        for(auto& a:pedMat1)a.Snap();for(auto& a:vehMat1)a.Snap();for(auto& a:objMat1)a.Snap();for(auto& a:bldMat1)a.Snap();
        TagClear();   // el orden sigue a Tg1
        pl1.Tagged();data1.Tagged();plMat1.Tagged();tgt1.Tagged();tgtMat1.Tagged();info1.Tagged();pedHdr1.Tagged();pedFlags1.Tagged();peds1.Tagged();for(auto& a:pedMat1)a.Tagged();
        vehHdr1.Tagged();vehFlags1.Tagged();vehs1.Tagged();for(auto& a:vehMat1)a.Tagged();objHdr1.Tagged();objFlags1.Tagged();objs1.Tagged();for(auto& a:objMat1)a.Tagged();blds1.Tagged();for(auto& a:bldMat1)a.Tagged();
        g1=State1{};g1.c=&c;nev=0;padsBase=u32(uintptr_t(CPad::GetPad(0)));
        sides[s]=Execute(env,1,0x60DC50,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(pl1.Obj()),c.edx,c.stack,0);});
        json obs;
        json ch=json::array({pl1.Changes(),data1.Changes(),tgt1.Changes(),info1.Changes()});
        obs["changes"]=ch;
        obs["events"]=EvsJson();
        obs["ret_al"]=sides[s].reg.eax&0xFFu;
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[1],n,input,rows,sides);
}

inline void RegisterFn1(Env& env,bool& ready,json& registered,json& bodies) {
    const uintptr_t skillBody=Addr(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill));
    const uintptr_t winfoBody=Addr(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo));
    const uintptr_t multBody=Addr(&CWeapon::TargetWeaponRangeMultiplier);
    const uintptr_t atanBody=Addr(&CGeneral::GetATanOfXY);
    const uintptr_t lrBody=Addr(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkLeftRight));
    const uintptr_t udBody=Addr(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkUpDown));
    const uintptr_t betweenBody=Addr(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints));
    const uintptr_t limitBody=Addr(&CGeneral::LimitRadianAngle);
    const uintptr_t normBody=Addr(&CVector::Normalise);
    const uintptr_t magBody=Addr(&CVector::Magnitude);
    const uintptr_t evalBody=Addr(&CPlayerPed::EvaluateTarget);
    const uintptr_t losBody=Addr(&LOSBlockedBetweenPeds);
    const uintptr_t groupBody=Addr(&CPedGroups::AreInSameGroup);
    const uintptr_t isTagBody=Addr(&CTagManager::IsTag);
    const uintptr_t alphaBody=Addr(static_cast<uint8(*)(const CEntity&)>(&CTagManager::GetAlpha));
    const uintptr_t findBody=Addr(&CWorld::FindObjectsInRange);
    const uintptr_t targettedBody=Addr(&CObject::CanBeTargetted);
    const uintptr_t reachBody=Addr(&CPlayerPed::CanIKReachThisTarget);
    const uintptr_t coopBody=Addr(&CGameLogic::IsCoopGameGoingOn);
    const uintptr_t registerBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference));
    const uintptr_t cleanBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference));
    env.route[1].reset(new HookRoute("CPlayerPed","FindWeaponLockOnTarget",0x60DC50,HookRoute::RegisteredTarget{}));
    registered[kFn[1]]=env.route[1]->Registered();bodies[kFn[1]]=env.route[1]->Body()!=0;
    ready&=env.route[1]->Registered()&&env.route[1]->Body()&&ModuleOf(env.route[1]->Body())==env.asi;
    ready&=skillBody&&winfoBody&&multBody&&atanBody&&lrBody&&udBody&&betweenBody&&limitBody&&normBody&&magBody&&evalBody&&losBody&&groupBody&&isTagBody&&alphaBody&&findBody&&targettedBody&&reachBody&&coopBody&&registerBody&&cleanBody;
    env.stubs[1]={{Pair{0x5E6580,skillBody,true},reinterpret_cast<void*>(&Fk1Skill)},
                  {Pair{0x743C60,winfoBody,true},reinterpret_cast<void*>(&Fk1WInfo)},
                  {Pair{0x73B380,multBody,true},reinterpret_cast<void*>(&Fk1Mult)},
                  {Pair{0x53CC70,atanBody,true},reinterpret_cast<void*>(&Fk1Atan)},
                  {Pair{0x53FC90,lrBody,true},reinterpret_cast<void*>(&Fk1WalkLR)},
                  {Pair{0x53FD30,udBody,true},reinterpret_cast<void*>(&Fk1WalkUD)},
                  {Pair{0x53CBE0,betweenBody,true},reinterpret_cast<void*>(&Fk1Between)},
                  {Pair{0x53CB50,limitBody,true},reinterpret_cast<void*>(&Fk1Limit)},
                  {Pair{0x59C910,normBody,false},reinterpret_cast<void*>(&Fk1Normalise)},
                  {Pair{0x4082C0,magBody,false},reinterpret_cast<void*>(&Fk1Mag)},
                  {Pair{0x60D020,evalBody,true},reinterpret_cast<void*>(&Fk1Eval)},
                  {Pair{0x60B550,losBody,true},reinterpret_cast<void*>(&Fk1Los)},
                  {Pair{0x5F7F40,groupBody,true},reinterpret_cast<void*>(&Fk1Group)},
                  {Pair{0x49CCE0,isTagBody,true},reinterpret_cast<void*>(&Fk1IsTag)},
                  {Pair{0x49CF90,alphaBody,true},reinterpret_cast<void*>(&Fk1Alpha)},
                  {Pair{0x564A20,findBody,true},reinterpret_cast<void*>(&Fk1Find)},
                  {Pair{0x59F320,targettedBody,true},reinterpret_cast<void*>(&Fk1Targetted)},
                  {Pair{0x609F80,reachBody,true},reinterpret_cast<void*>(&Fk1Reach)},
                  {Pair{0x441390,coopBody,true},reinterpret_cast<void*>(&Fk1Coop)},
                  {Pair{0x571B70,registerBody,true},reinterpret_cast<void*>(&Fk1Register)},
                  {Pair{0x571A00,cleanBody,true},reinterpret_cast<void*>(&Fk1CleanUp)}};
    env.watches.emplace_back(0x60DC50,16);
}
}
