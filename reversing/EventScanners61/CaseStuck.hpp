// ------------------------------------------------------------------------------------------------ CPedStuckChecker::TestPedStuck (0x602C00)
// Incluido dentro de namespace EventScanners61Live desde Core.hpp.
// thiscall, 2 argumentos (ped, grupo de eventos), `ret 8`, devuelve bool (AL).
// Falsos (identicos en ambos lados): CPed::IsPlayer 0x5DF8F0, CPedIntelligence::GetTaskSwim/GetTaskJetPack/GetTaskClimb 0x601070/0x601110/0x601180, CVector::Normalise 0x59C910 (el falso
// multiplica por 0.5 para aislar la funcion), CWorld::ProcessVerticalLine 0x5674E0, CWorld::GetIsLineOfSightClear 0x56A490, CPlaceable::SetPosn(vector) 0x4241C0 (el falso escribe la posicion
// del peaton como el original), CPhysical::ApplyMoveForce(vector) 0x5429F0, CPlaceable::SetOrientation(x,y,z) 0x439A80, CGeneral::GetRadianAngleBetweenPoints 0x53CBE0, CGeneral::LimitRadianAngle
// 0x53CB50, CEventGroup::GetEventOfType 0x4AB650, CEventGroup::Add 0x4AB420 y CEntity::RegisterReference/CleanUpOldReference 0x571B70/0x571A00 (constructor y destructor de CEventStuckInAir).
// Reales: el constructor/destructor de CEventStuckInAir, las restas/sumas de vectores y SquaredMagnitude.
inline const u32 kEvSwim=DefEv("task_swim");
inline const u32 kEvJet=DefEv("task_jetpack");
inline const u32 kEvClimb=DefEv("task_climb");
inline const u32 kEvNorm=DefEv("normalise");
inline const u32 kEvPvl=DefEv("vertical_line");
inline const u32 kEvLosV=DefEv("line_of_sight");
inline const u32 kEvSetPosn=DefEv("set_posn");
inline const u32 kEvForce=DefEv("apply_move_force");
inline const u32 kEvOrient=DefEv("set_orientation");
inline const u32 kEvAngle=DefEv("angle_between");
inline const u32 kEvLimit=DefEv("limit_angle");
inline const u32 kEvGetEv=DefEv("get_event_of_type");
inline u32 gSwim{},gJet{},gClimb{},gPvlN{},gLosN{},gWaterPtr{},gAngleBits{},gLimitedBits{};
inline std::array<u32,3> gHit{};inline std::array<std::array<u32,3>,3> gCp{};inline std::array<u32,2> gLosRet{};
inline u32 gKPed{};   // direccion del peaton (para SetPosn)
inline u32 __fastcall FakeSwim(void* intel,void*){Push(kEvSwim,1,u32(uintptr_t(intel)));return gSwim;}
inline u32 __fastcall FakeJet(void* intel,void*){Push(kEvJet,1,u32(uintptr_t(intel)));return gJet;}
inline u32 __fastcall FakeClimb(void* intel,void*){Push(kEvClimb,1,u32(uintptr_t(intel)));return gClimb;}
// 0x59C910 CVector::Normalise(): thiscall sin argumentos
inline void __fastcall FakeNormalise(float* v,void*){
    u32 x{},y{},z{};memcpy(&x,v,4);memcpy(&y,v+1,4);memcpy(&z,v+2,4);
    Push(kEvNorm,0,x,y,z);
    v[0]=v[0]*0.5f;v[1]=v[1]*0.5f;v[2]=v[2]*0.5f;
}
// 0x5674E0 CWorld::ProcessVerticalLine(origin, distance, colPoint, entity, b, v, p, o, d, s, poly): cdecl
inline u32 __cdecl FakePvl(const float* origin,u32 distance,float* colPoint,void** outEntity,u32 b,u32 v,u32 p,u32 o,u32 d,u32 s,void* poly){
    u32 ox{},oy{},oz{};memcpy(&ox,origin,4);memcpy(&oy,origin+1,4);memcpy(&oz,origin+2,4);
    Push(kEvPvl,0,ox,oy,oz,distance,b&0xFFu,v&0xFFu,p&0xFFu,o&0xFFu,d&0xFFu,s&0xFFu,u32(uintptr_t(poly)));
    const unsigned i=gPvlN<3?gPvlN:2;++gPvlN;
    memcpy(colPoint,gCp[i].data(),12);
    *outEntity=nullptr;
    return gHit[i];
}
// 0x56A490 CWorld::GetIsLineOfSightClear(origin, target, 7 bool): cdecl
inline u32 __cdecl FakeLosV(const float* origin,const float* target,u32 b0,u32 b1,u32 b2,u32 b3,u32 b4,u32 b5,u32 b6){
    u32 o[3]{},t[3]{};memcpy(o,origin,12);memcpy(t,target,12);
    Push(kEvLosV,0,o[0],o[1],o[2],t[0],t[1],t[2],b0&0xFFu,b1&0xFFu,b2&0xFFu,b3&0xFFu,b4&0xFFu,b5&0xFFu,b6&0xFFu);
    const unsigned i=gLosN<2?gLosN:1;++gLosN;
    return gLosRet[i];
}
// 0x4241C0 CPlaceable::SetPosn(const CVector&): thiscall, `ret 4`. Escribe la posicion como el original (matriz+0x30 o placement +4).
inline void __fastcall FakeSetPosn(void* ped,void*,const float* v){
    u32 x{},y{},z{};memcpy(&x,v,4);memcpy(&y,v+1,4);memcpy(&z,v+2,4);
    Push(kEvSetPosn,1,u32(uintptr_t(ped)),x,y,z);
    u8* p=static_cast<u8*>(ped);u8* mat{};memcpy(&mat,p+kMatrixAt,4);
    memcpy(mat?mat+kMatPos:p+kPlacePos,v,12);
}
// 0x5429F0 CPhysical::ApplyMoveForce(CVector): thiscall con el vector por valor (3 dwords), `ret 0Ch`
inline void __fastcall FakeForce(void* ped,void*,u32 x,u32 y,u32 z){Push(kEvForce,1,u32(uintptr_t(ped)),x,y,z);}
// 0x439A80 CPlaceable::SetOrientation(x, y, z): thiscall, `ret 0Ch`
inline void __fastcall FakeOrient(void* ped,void*,u32 x,u32 y,u32 z){Push(kEvOrient,1,u32(uintptr_t(ped)),x,y,z);}
// 0x53CBE0 CGeneral::GetRadianAngleBetweenPoints(x1, y1, x2, y2): cdecl, devuelve float
inline float __cdecl FakeAngle(u32 x1,u32 y1,u32 x2,u32 y2){Push(kEvAngle,0,x1,y1,x2,y2);return FromBits(gAngleBits);}
// 0x53CB50 CGeneral::LimitRadianAngle(float): cdecl, devuelve float
inline float __cdecl FakeLimit(u32 a){Push(kEvLimit,0,a);return FromBits(gLimitedBits);}
// 0x4AB650 CEventGroup::GetEventOfType(type): thiscall, `ret 4`
inline u32 __fastcall FakeGetEv(void* group,void*,u32 type){Push(kEvGetEv,1,u32(uintptr_t(group)),type);return gWaterPtr;}

alignas(16) inline std::array<u8,kKcArena> kcArena{},kcRef{};
alignas(16) inline std::array<u8,kKpedArena> kpArena{},kpRef{};
alignas(16) inline std::array<u8,kKmatArena> kmArena{},kmRef{};
alignas(16) inline std::array<u8,kKentArena> kdArena{},kdRef{};      // entidad que hizo dano
alignas(16) inline std::array<u8,kKentArena> keArena{},keRef{};      // entidad colisionada [0]
alignas(16) inline std::array<u8,kKwatArena> kwArena{},kwRef{};
alignas(16) inline std::array<u8,kKgrpArena> kgArena{},kgRef{};
alignas(16) inline std::array<u8,kKintArena> kiArena{},kiRef{};

inline json StuckInput(const K& k) {
    return {{"family",k.family},{"ent_flags",k.entFlags},{"attached_null",k.attachedNull},{"ped_state",k.pedState},{"flags_46c",k.flags46C},{"flags_470",k.flags470},{"dmg_kind",k.dmgKind},
            {"dmg_bits",k.dmgBits},{"impact_z",k.impactZBits},{"num_coll",k.numColl},{"coll_kind",k.collKind},{"seed",k.seed},{"ped_mat_null",k.pedMatNull},
            {"pos",std::vector<u32>(k.pos,k.pos+3)},{"last",std::vector<u32>(k.lastPt,k.lastPt+3)},{"is_player",k.isPlayer},{"swim",k.swim},{"jet",k.jet},{"climb",k.climb},
            {"radius",k.radius},{"state",k.state},{"ts",k.tsBits},{"frame",k.frame},{"hit",std::vector<u32>(k.hit,k.hit+3)},
            {"cp",std::vector<std::vector<u32>>{{k.cp[0][0],k.cp[0][1],k.cp[0][2]},{k.cp[1][0],k.cp[1][1],k.cp[1][2]},{k.cp[2][0],k.cp[2][1],k.cp[2][2]}}},
            {"los",std::vector<u32>(k.los,k.los+2)},{"water_kind",k.waterKind},{"accel",k.accelBits},{"angle",k.angleBits},{"limited",k.limitedBits},{"eax",k.eax},{"edx",k.edx},{"stack_seed",k.stack}};
}
inline void CaseStuck(Env& env,Totals& t,unsigned n) {
    const K k=MakeK(n);
    u8* const kc=kcArena.data()+kKcOff;u8* const ped=kpArena.data()+kKpedOff;u8* const pm=kmArena.data()+kKmatOff;u8* const de=kdArena.data()+kKentOff;u8* const ce=keArena.data()+kKentOff;
    u8* const wt=kwArena.data()+kKwatOff;u8* const gr=kgArena.data()+kKgrpOff;u8* const in=kiArena.data()+kKintOff;
    regions={{uintptr_t(kc),kKcBytes,"K"},{uintptr_t(ped),kKpedBytes,"D"},{uintptr_t(pm),kKmatBytes,"DM"},{uintptr_t(de),kKentBytes,"DE"},{uintptr_t(ce),kKentBytes,"CE"},
            {uintptr_t(wt),kKwatBytes,"W"},{uintptr_t(gr),kKgrpBytes,"G"},{uintptr_t(in),kKintBytes,"I"}};
    FillWords(kcRef.data(),kKcArena,Rand(n,200));FillWords(kpRef.data(),kKpedArena,Rand(n,201));FillWords(kmRef.data(),kKmatArena,Rand(n,202));FillWords(kdRef.data(),kKentArena,Rand(n,203));
    FillWords(keRef.data(),kKentArena,Rand(n,204));FillWords(kwRef.data(),kKwatArena,Rand(n,205));FillWords(kgRef.data(),kKgrpArena,Rand(n,206));FillWords(kiRef.data(),kKintArena,Rand(n,207));
    u8* const rkc=kcRef.data()+kKcOff;u8* const rped=kpRef.data()+kKpedOff;u8* const rpm=kmRef.data()+kKmatOff;u8* const rde=kdRef.data()+kKentOff;u8* const rce=keRef.data()+kKentOff;u8* const rwt=kwRef.data()+kKwatOff;
    memcpy(rkc,k.lastPt,12);{const u16 r=u16(k.radius),st=u16(k.state);memcpy(rkc+12,&r,2);memcpy(rkc+14,&st,2);}
    memcpy(rped+kEntFlagsAt,&k.entFlags,4);rped[kEntSeedAt]=u8(k.seed);
    const u32 attached=k.attachedNull?0u:u32(uintptr_t(ce));memcpy(rped+kPhysAttachedAt,&attached,4);
    memcpy(rped+kPedStateAt,&k.pedState,4);memcpy(rped+kPedFlagsAt,&k.flags46C,4);memcpy(rped+kPedFlags2At,&k.flags470,4);
    memcpy(rped+kPhysDmgIntAt,&k.dmgBits,4);
    const u32 dmgPtr=k.dmgKind==0?0u:u32(uintptr_t(de));memcpy(rped+kPhysDmgEntAt,&dmgPtr,4);
    rde[kEntTypeAt]=u8((rde[kEntTypeAt]&~7u)|(k.dmgKind==1?3u:k.dmgKind==2?1u:0u));
    memcpy(rped+kPhysImpactZAt,&k.impactZBits,4);rped[kPhysNumCollAt]=u8(k.numColl);
    const u32 collPtr=k.collKind==0?0u:u32(uintptr_t(ce));memcpy(rped+kPhysCollEntAt,&collPtr,4);
    rce[kEntTypeAt]=u8((rce[kEntTypeAt]&~7u)|(k.collKind==1?1u:k.collKind==2?3u:0u));
    const u32 pmPtr=k.pedMatNull?0u:u32(uintptr_t(pm));memcpy(rped+kMatrixAt,&pmPtr,4);
    if(k.pedMatNull)memcpy(rped+kPlacePos,k.pos,12);else {memcpy(rped+kPlacePos,k.placeOther,12);memcpy(rpm+kMatPos,k.pos,12);}
    const u32 intelPtr=u32(uintptr_t(in));memcpy(rped+kPedIntelAt,&intelPtr,4);
    memcpy(rwt+0xC,&k.accelBits,4);
    gSwim=k.swim;gJet=k.jet;gClimb=k.climb;gIsPlayer=k.isPlayer;gPvlN=0;gLosN=0;gWaterPtr=k.waterKind?u32(uintptr_t(wt)):0u;gAngleBits=k.angleBits;gLimitedBits=k.limitedBits;
    for(int i=0;i<3;++i){gHit[i]=k.hit[i];for(int j=0;j<3;++j)gCp[i][j]=k.cp[i][j];}
    gLosRet={k.los[0],k.los[1]};gKPed=u32(uintptr_t(ped));
    const json input=StuckInput(k);
    Side sides[2];json rows[2];
    const u32 args[2]{u32(uintptr_t(ped)),u32(uintptr_t(gr))};
    const std::vector<const Stub*> use{&env.stubs[S_ISPLAYER],&env.stubs[S_SWIM],&env.stubs[S_JET],&env.stubs[S_CLIMB],&env.stubs[S_NORM],&env.stubs[S_PVL],&env.stubs[S_LOSV],&env.stubs[S_SETPOSN],
        &env.stubs[S_FORCE],&env.stubs[S_ORIENT],&env.stubs[S_ANGLE],&env.stubs[S_LIMIT],&env.stubs[S_GETEV],&env.stubs[S_ADD],&env.stubs[S_REGREF],&env.stubs[S_CLEANREF]};
    const uint32 savedFrame=*reinterpret_cast<uint32*>(kFrameGlobal),savedStep=*reinterpret_cast<uint32*>(kStepGlobal);
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        kcArena=kcRef;kpArena=kpRef;kmArena=kmRef;kdArena=kdRef;keArena=keRef;kwArena=kwRef;kgArena=kgRef;kiArena=kiRef;
        *reinterpret_cast<uint32*>(kFrameGlobal)=k.frame;*reinterpret_cast<uint32*>(kStepGlobal)=k.tsBits;
        nev=0;gPvlN=0;gLosN=0;
        sides[sd]=Execute(env,*env.route[3],0x602C00,use,candidate,[&](uintptr_t entry){return Gate(entry,args,2,k.eax,u32(uintptr_t(kc)),k.edx,k.stack,0);});
        const uint32 frameAfter=*reinterpret_cast<uint32*>(kFrameGlobal),stepAfter=*reinterpret_cast<uint32*>(kStepGlobal);
        *reinterpret_cast<uint32*>(kFrameGlobal)=savedFrame;*reinterpret_cast<uint32*>(kStepGlobal)=savedStep;
        json obs;
        obs["ret"]=sides[sd].reg.eax&0xFFu;
        obs["checker"]=HexBytes(kc,kKcBytes);obs["checker_guards_ok"]=GuardsOk(kcArena,kcRef,kKcOff,kKcBytes);
        obs["events"]=EventsJson();obs["nevents"]=nev;
        obs["ped_changes"]=Changes(kpArena,kpRef,kKpedOff,kKpedBytes);obs["ped_guards_ok"]=GuardsOk(kpArena,kpRef,kKpedOff,kKpedBytes);
        obs["mat_changes"]=Changes(kmArena,kmRef,kKmatOff,kKmatBytes);obs["mat_guards_ok"]=GuardsOk(kmArena,kmRef,kKmatOff,kKmatBytes);
        {u32 r0{},r1{};memcpy(&r0,ped+kPedCurRotAt,4);memcpy(&r1,ped+kPedAimRotAt,4);obs["rot"]=std::vector<u32>{r0,r1};}
        obs["globals_unchanged"]=frameAfter==k.frame&&stepAfter==k.tsBits;
        obs["others_intact"]=kdArena==kdRef&&keArena==keRef&&kwArena==kwRef&&kgArena==kgRef&&kiArena==kiRef;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[sd].reg.ecx;obs["edx"]=sides[sd].reg.edx;}
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&*reinterpret_cast<uint32*>(kFrameGlobal)==savedFrame&&*reinterpret_cast<uint32*>(kStepGlobal)==savedStep;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[sd]=out;
    }
    Finish(env,t,"CPedStuckChecker::TestPedStuck",n,input,rows,sides);
}
