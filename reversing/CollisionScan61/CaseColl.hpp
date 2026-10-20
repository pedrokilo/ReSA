// ------------------------------------------------------------------------------------------------ CCollisionEventScanner::ScanForCollisionEvents (0x604500)
// Incluido dentro de namespace CollisionScan61Live desde Core.hpp.
// Falsos (identicos en ambos lados): CTaskManager::GetSimplestActiveTask 0x6819D0, CTask::IsGoToTask 0x61A360, CPedIntelligence::GetMoveStateFromGoToTask 0x601D70, CEntity::GetColModel 0x535300,
// CPed::IsPlayer 0x5DF8F0 (por peaton: victima o danador), CPed::KillPedWithCar 0x5F0360, CPlaceable::GetHeading 0x441DB0, CGeneral::LimitRadianAngle 0x53CB50, CVector::Normalise 0x59C910
// (el falso multiplica por 0.5 para aislar la funcion), CEventDamage::AffectsPed 0x4B35A0 (virtual), CPedDamageResponseCalculator::ComputeDamageResponse 0x4B5AC0,
// CPedGeometryAnalyser::ComputePedHitSide 0x5F3640, CPed::Say 0x5EFFE0, CPedIntelligence::IncrementAngerAtPlayer 0x421050, CAcquaintance::GetAcquaintances 0x608970, CPedType::GetPedFlag 0x608830,
// CPedClothesDesc::GetIsWearingBalaclava 0x5A7950, CPed::GetLocalDirection 0x5DEF60, CWeapon::GenerateDamageEvent 0x73A530, CEventGroup::Add 0x4AB420 y CEntity::RegisterReference /
// CleanUpOldReference 0x571B70 / 0x571A00 (constructores y destructores de los eventos).
// Reales: los constructores y destructores de los eventos, el constructor del calculador de dano, CEntity::GetIsStatic / GetIsTypePhysical, GetEventGlobalGroup y la aritmetica de vectores
// (0x406DA0, 0x40FE60, 0x40FE90, 0x40FDB0, 0x4119D0, 0x59C890: el EXE ejecuta las suyas y la DLL las operaciones en C++).
inline const u32 kEvSimplest=DefEv("simplest");
inline const u32 kEvIsGoTo=DefEv("isgoto");
inline const u32 kEvMove=DefEv("move");
inline const u32 kEvCol=DefEv("colmodel");
inline const u32 kEvRegRef=DefEv("regref");
inline const u32 kEvCleanRef=DefEv("cleanref");
inline const u32 kEvIsPlayer=DefEv("is_player");
inline const u32 kEvGenDmg=DefEv("generate_damage");
inline const u32 kEvNorm=DefEv("normalise");
inline const u32 kEvLimit=DefEv("limit_angle");
inline const u32 kEvKill=DefEv("kill_ped_with_car");
inline const u32 kEvHeading=DefEv("heading");
inline const u32 kEvAffects=DefEv("affects_ped");
inline const u32 kEvCompute=DefEv("compute_damage");
inline const u32 kEvHitSide=DefEv("hit_side");
inline const u32 kEvSay=DefEv("say");
inline const u32 kEvAnger=DefEv("anger");
inline const u32 kEvAcq=DefEv("acquaintances");
inline const u32 kEvPedFlag=DefEv("ped_flag");
inline const u32 kEvBala=DefEv("balaclava");
inline const u32 kEvLocalDir=DefEv("local_direction");
inline u32 gTaskPtr{},gGoto{},gMoveRet[2]{},gMoveN{},gColPtr{},gVictim{},gDamager{},gIsPlayerV{},gIsPlayerD{},gHeading{},gLimited{},gHitSide{},gAffects{},gBala{},gLocalDir{};
inline u32 gAcq[2]{},gAcqN{},gPedFlagRet[2]{},gPedFlagN{};
inline void* gLastEv{};
// 0x6819D0 CTaskManager::GetSimplestActiveTask(): thiscall sin argumentos
inline u32 __fastcall FakeSimplest(void* mgr,void*){Push(kEvSimplest,1,u32(uintptr_t(mgr)));return gTaskPtr;}
// 0x61A360 CTask::IsGoToTask(task): cdecl
inline u32 __cdecl FakeIsGoto(void* task){Push(kEvIsGoTo,1,u32(uintptr_t(task)));return gGoto;}
// 0x601D70 CPedIntelligence::GetMoveStateFromGoToTask(): thiscall sin argumentos (devuelve en orden de llamada)
inline u32 __fastcall FakeMove(void* intel,void*){Push(kEvMove,1,u32(uintptr_t(intel)));return gMoveN<2?gMoveRet[gMoveN++]:0u;}
// 0x535300 CEntity::GetColModel(): thiscall sin argumentos
inline u32 __fastcall FakeCol(void* entity,void*){Push(kEvCol,1,u32(uintptr_t(entity)));return gColPtr;}
// 0x571B70 CEntity::RegisterReference(CEntity**) y 0x571A00 CEntity::CleanUpOldReference(CEntity**): thiscall con un argumento (puntero a un campo del evento de pila: no se etiqueta)
inline void __fastcall FakeRegRef(void* entity,void*,void* ref){(void)ref;Push(kEvRegRef,1,u32(uintptr_t(entity)));}
inline void __fastcall FakeCleanRef(void* entity,void*,void* ref){(void)ref;Push(kEvCleanRef,1,u32(uintptr_t(entity)));}
// 0x5DF8F0 CPed::IsPlayer(): thiscall; el resultado depende del peaton (victima o danador)
inline u32 __fastcall FakeIsPlayer(void* ped,void*){Push(kEvIsPlayer,1,u32(uintptr_t(ped)));const u32 p=u32(uintptr_t(ped));return p==gVictim?gIsPlayerV:(p==gDamager?gIsPlayerD:0u);}
// 0x73A530 CWeapon::GenerateDamageEvent(victim, creator, weaponType, damageFactor, pedPiece, direction): cdecl
inline u32 __cdecl FakeGenDmg(void* victim,void* creator,u32 weapon,i32 damage,u32 piece,u32 dir){Push(kEvGenDmg,3,u32(uintptr_t(victim)),u32(uintptr_t(creator)),weapon,u32(damage),piece,dir&0xFFu);return 1;}
// 0x59C910 CVector::Normalise(): thiscall sin argumentos
inline void __fastcall FakeNormalise(float* v,void*){
    u32 x{},y{},z{};memcpy(&x,v,4);memcpy(&y,v+1,4);memcpy(&z,v+2,4);
    Push(kEvNorm,0,x,y,z);
    v[0]=v[0]*0.5f;v[1]=v[1]*0.5f;v[2]=v[2]*0.5f;
}
// 0x53CB50 CGeneral::LimitRadianAngle(float): cdecl, devuelve float
inline float __cdecl FakeLimit(u32 a){Push(kEvLimit,0,a);return FromBits(gLimited);}
// 0x5F0360 CPed::KillPedWithCar(CVehicle*, float, bool): thiscall, `ret 0Ch`
inline void __fastcall FakeKill(void* ped,void*,void* car,u32 damage,u32 flag){Push(kEvKill,3,u32(uintptr_t(ped)),u32(uintptr_t(car)),damage,flag&0xFFu);}
// 0x441DB0 CPlaceable::GetHeading(): thiscall sin argumentos, devuelve float (ST0)
inline float __fastcall FakeHeading(void* self,void*){Push(kEvHeading,1,u32(uintptr_t(self)));return FromBits(gHeading);}
// 0x4B35A0 CEventDamage::AffectsPed(CPed*): thiscall, `ret 4`; se vuelca el evento (llamadas virtuales sobre el propio evento) y el peaton va en la ranura del grupo
inline u32 __fastcall FakeAffects(void* ev,void*,void* ped){DumpEvent(kEvAffects,ped,ev,0,false);gLastEv=ev;return gAffects;}
// 0x4B5AC0 CPedDamageResponseCalculator::ComputeDamageResponse(CPed*, CPedDamageResponse&, bool): thiscall, `ret 0Ch`; el calculador: +0 danador, +4 factor, +8 pieza, +0xC arma, +0x10 hablar
inline void __fastcall FakeCompute(void* calc,void*,void* ped,void* resp,u32 spine){
    const u8* c=static_cast<const u8*>(calc);u32 who{},factor{},part{},weapon{};memcpy(&who,c,4);memcpy(&factor,c+4,4);memcpy(&part,c+8,4);memcpy(&weapon,c+12,4);
    const u32 off=u32(uintptr_t(resp))-u32(uintptr_t(gLastEv));
    Push(kEvCompute,(1u<<0)|(1u<<5),who,factor,part,weapon,u32(c[0x10]),u32(uintptr_t(ped)),off,spine&0xFFu);
}
// 0x5F3640 CPedGeometryAnalyser::ComputePedHitSide(const CPed&, const CPhysical&): cdecl
inline u32 __cdecl FakeHitSide(void* ped,void* phys){Push(kEvHitSide,3,u32(uintptr_t(ped)),u32(uintptr_t(phys)));return gHitSide;}
// 0x5EFFE0 CPed::Say(ctx, startTimeDelay, probability, overrideSilence, isForceAudible, isFrontEnd): thiscall, `ret 18h`
inline u32 __fastcall FakeSay(void* self,void*,u32 ctx,u32 delay,u32 prob,u32 o,u32 f,u32 i){Push(kEvSay,1,u32(uintptr_t(self)),ctx,delay,prob,o&0xFFu,f&0xFFu,i&0xFFu);return 0;}
// 0x421050 CPedIntelligence::IncrementAngerAtPlayer(uint8): thiscall, `ret 4`
inline u32 __fastcall FakeAnger(void* intel,void*,u32 anger){Push(kEvAnger,1,u32(uintptr_t(intel)),anger&0xFFu);return 0;}
// 0x608970 CAcquaintance::GetAcquaintances(id): thiscall, `ret 4`
inline u32 __fastcall FakeAcq(void* acq,void*,u32 id){Push(kEvAcq,1,u32(uintptr_t(acq)),id);return gAcqN<2?gAcq[gAcqN++]:0u;}
// 0x608830 CPedType::GetPedFlag(pedType): cdecl
inline u32 __cdecl FakePedFlag(u32 type){Push(kEvPedFlag,0,type);return gPedFlagN<2?gPedFlagRet[gPedFlagN++]:0u;}
// 0x5A7950 CPedClothesDesc::GetIsWearingBalaclava(): thiscall sin argumentos
inline u32 __fastcall FakeBala(void* self,void*){Push(kEvBala,1,u32(uintptr_t(self)));return gBala;}
// 0x5DEF60 CPed::GetLocalDirection(const CVector2D&): thiscall, `ret 4`; el vector es de la pila del llamador: se registran sus dos floats
inline u32 __fastcall FakeLocalDir(void* ped,void*,const float* v){u32 x{},y{};memcpy(&x,v,4);memcpy(&y,v+1,4);Push(kEvLocalDir,1,u32(uintptr_t(ped)),x,y);return gLocalDir;}

alignas(16) inline std::array<u8,kCsArena> csArena{},csRef{};
alignas(16) inline std::array<u8,kVArena> vArena{},vRef{};
alignas(16) inline std::array<u8,kVmArena> vmArena{},vmRef{};
alignas(16) inline std::array<u8,kIArena> iArena{},iRef{};
alignas(16) inline std::array<u8,kTArena> tArena{},tRef{};
alignas(16) inline std::array<u8,kDArena> dArena{},dRef{};
alignas(16) inline std::array<u8,kDmArena> dmArena{},dmRef{};
alignas(16) inline std::array<u8,kDiArena> diArena{},diRef{};
alignas(16) inline std::array<u8,kColArena> colArena{},colRef{};
alignas(16) inline std::array<u8,kPdArena> pdArena{},pdRef{};
alignas(16) inline std::array<u8,kClArena> clArena{},clRef{};
alignas(16) inline std::array<u8,kAArena> aArena{},aRef{};
alignas(16) inline std::array<u8,kGArena> gArena{},gRef{};

inline json CollInput(const C& c) {
    const auto v3=[](const u32* p){return std::vector<u32>(p,p+3);};
    return {{"family",c.family},{"hit_by_car",c.hitByCar},{"dmg_bits",c.dmgBits},{"dmg_null",c.dmgNull},{"kind",c.kind},{"type_noise",c.typeNoise},{"e0",v3(c.e0)},{"ec",v3(c.ec)},{"piece",c.piece},
            {"attach_kind",c.attachKind},{"attach_type",c.attachType},{"flags_46c",c.flags46C},{"ent_flags",c.entFlags},{"standing_on",c.standingOn},{"mass",c.massBits},{"shift_x",c.shiftX},{"shift_y",c.shiftY},
            {"has_player",c.hasPlayer},{"ped_mat_null",c.pedMatNull},{"pos",v3(c.pos)},{"place",v3(c.place)},{"ped_type",c.pedType},{"contact_kind",c.contactKind},
            {"task_null",c.taskNull},{"is_goto",c.isGoto},{"task_move",c.taskMove},
            {"d_flags",c.dFlags1C},{"d_phys",c.dPhys40},{"d_speed",v3(c.dSpeed)},{"d_turn_z",c.dTurnZ},{"d_subtype",c.dSubtype},{"d_tow",c.dTow},{"d_att_kind",c.dAttKind},{"d_ped_type",c.dPedType},{"d_dam_null",c.dDamNull},
            {"d_vm",std::vector<u32>(c.dVm,c.dVm+9)},{"d_vpos",v3(c.dVpos)},{"box_min",v3(c.boxMin)},{"box_max",v3(c.boxMax)},{"heading",c.headingBits},{"limited",c.limitedBits},
            {"is_player_v",c.isPlayerV},{"is_player_d",c.isPlayerD},{"hit_side",c.hitSide},{"affects",c.affects},{"bala",c.bala},{"local_dir",c.localDir},
            {"acq",std::vector<u32>(c.acq,c.acq+2)},{"ped_flag",std::vector<u32>(c.pedFlag,c.pedFlag+2)},{"move",std::vector<u32>(c.move,c.move+2)},
            {"now",c.now},{"last_a",c.lastA},{"last_b",c.lastB},{"eax",c.eax},{"edx",c.edx},{"stack_seed",c.stack}};
}
inline void CaseColl(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    u8* const cs=csArena.data()+kCsOff;u8* const v=vArena.data()+kVOff;u8* const vm=vmArena.data()+kVmOff;u8* const in=iArena.data()+kIOff;u8* const tk=tArena.data()+kTOff;
    u8* const d=dArena.data()+kDOff;u8* const dm=dmArena.data()+kDmOff;u8* const di=diArena.data()+kDiOff;u8* const col=colArena.data()+kColOff;u8* const pd=pdArena.data()+kPdOff;
    u8* const cl=clArena.data()+kClOff;u8* const at=aArena.data()+kAOff;u8* const gr=gArena.data()+kGOff;
    const u32 globalGroup=u32(uintptr_t(GetEventGlobalGroup()));
    regions={{uintptr_t(cs),kCsBytes,"S"},{uintptr_t(v),kVBytes,"V"},{uintptr_t(vm),kVmBytes,"VM"},{uintptr_t(in),kIBytes,"I"},{uintptr_t(tk),kTBytes,"T"},{uintptr_t(d),kDBytes,"D"},
            {uintptr_t(dm),kDmBytes,"DM"},{uintptr_t(di),kDiBytes,"DI"},{uintptr_t(col),kColBytes,"C"},{uintptr_t(pd),kPdBytes,"P"},{uintptr_t(cl),kClBytes,"CL"},{uintptr_t(at),kABytes,"A"},
            {uintptr_t(gr),kGBytes,"G"},{uintptr_t(globalGroup),4,"GG"}};
    FillWords(csRef.data(),kCsArena,Rand(n,200));FillWords(vRef.data(),kVArena,Rand(n,201));FillWords(vmRef.data(),kVmArena,Rand(n,202));FillWords(iRef.data(),kIArena,Rand(n,203));
    FillWords(tRef.data(),kTArena,Rand(n,204));FillWords(dRef.data(),kDArena,Rand(n,205));FillWords(dmRef.data(),kDmArena,Rand(n,206));FillWords(diRef.data(),kDiArena,Rand(n,207));
    FillWords(colRef.data(),kColArena,Rand(n,208));FillWords(pdRef.data(),kPdArena,Rand(n,209));FillWords(clRef.data(),kClArena,Rand(n,210));FillWords(aRef.data(),kAArena,Rand(n,211));
    FillWords(gRef.data(),kGArena,Rand(n,212));
    u8* const rcs=csRef.data()+kCsOff;u8* const rv=vRef.data()+kVOff;u8* const rvm=vmRef.data()+kVmOff;u8* const rin=iRef.data()+kIOff;u8* const rtk=tRef.data()+kTOff;
    u8* const rd=dRef.data()+kDOff;u8* const rdm=dmRef.data()+kDmOff;u8* const rdi=diRef.data()+kDiOff;u8* const rcol=colRef.data()+kColOff;u8* const rpd=pdRef.data()+kPdOff;
    u8* const rcl=clRef.data()+kClOff;u8* const rat=aRef.data()+kAOff;
    const auto put32=[](u8* base,u32 off,u32 value){memcpy(base+off,&value,4);};
    const auto ptr=[](const void* p){return u32(uintptr_t(p));};
    rcs[0]=u8(c.hitByCar);
    // victima
    put32(rv,kEntMatAt,c.pedMatNull?0u:ptr(vm));
    if(c.pedMatNull)memcpy(rv+kEntPosAt,c.place,12);else memcpy(rv+kEntPosAt,c.place,12);
    memcpy(rvm+0x00,c.vmRows+0,12);memcpy(rvm+0x10,c.vmRows+3,12);memcpy(rvm+kMatUp,c.vmRows+6,12);memcpy(rvm+kMatPos,c.pos,12);
    if(c.pedMatNull)memcpy(rv+kEntPosAt,c.pos,12);
    put32(rv,kEntFlagsAt,c.entFlags);
    put32(rv,kPhysDmgIntAt,c.dmgBits);put32(rv,kPhysDmgEntAt,c.dmgNull?0u:ptr(d));
    memcpy(rv+kPhysImpactAt,c.e0,12);memcpy(rv+kPhysPosnAt,c.ec,12);
    {const u16 piece=u16(c.piece);memcpy(rv+kPhysPieceAt,&piece,2);}
    put32(rv,kPhysAttachedAt,c.attachKind?ptr(at):0u);rat[kEntTypeAt]=u8(c.attachType);
    put32(rv,kPhysMassAt,c.massBits);put32(rv,kPedShiftAt,c.shiftX);put32(rv,kPedShiftAt+4,c.shiftY);
    put32(rv,kPedFlagsAt,c.flags46C);put32(rv,kPedIntelAt,ptr(in));put32(rv,kPedPlayerAt,c.hasPlayer?ptr(pd):0u);put32(rpd,4,ptr(cl));
    put32(rv,kPedStandingOnAt,c.standingOn);
    put32(rv,kPedContactAt,c.contactKind==0?0u:c.contactKind==1?ptr(d):c.contactKind==2?ptr(at):0x5A5A5A5Au);
    put32(rv,kPedTypeAt,c.pedType);
    put32(rtk,8,c.taskMove);
    // danador
    rd[kEntTypeAt]=u8((c.kind&7u)|(c.typeNoise&0xF8u));
    put32(rd,kEntFlagsAt,c.dFlags1C);put32(rd,kEntMatAt,ptr(dm));
    memcpy(rdm+0x00,c.dVm+0,12);memcpy(rdm+0x10,c.dVm+3,12);memcpy(rdm+kMatUp,c.dVm+6,12);memcpy(rdm+kMatPos,c.dVpos,12);
    memcpy(rd+kEntPosAt,c.dVplace,12);
    put32(rd,kPhysFlagsAt,c.dPhys40);memcpy(rd+kPhysSpeedAt,c.dSpeed,12);put32(rd,kPhysTurnZAt,c.dTurnZ);
    put32(rd,kVehSubtypeAt,c.dSubtype);put32(rd,kVehTowingAt,c.dTow);
    put32(rd,kPhysAttachedAt,c.dAttKind==0?0u:c.dAttKind==1?ptr(d):c.dAttKind==2?ptr(at):0x77777777u);
    put32(rd,kPhysDmgEntAt,c.dDamNull?0u:ptr(at));
    put32(rd,kPedIntelAt,ptr(di));put32(rd,kPedTypeAt,c.dPedType);
    memcpy(rcol,c.boxMin,12);memcpy(rcol+12,c.boxMax,12);
    gTaskPtr=c.taskNull?0u:ptr(tk);gGoto=c.isGoto;gMoveRet[0]=c.move[0];gMoveRet[1]=c.move[1];gColPtr=ptr(col);gVictim=ptr(v);gDamager=ptr(d);gIsPlayerV=c.isPlayerV;gIsPlayerD=c.isPlayerD;
    gHeading=c.headingBits;gLimited=c.limitedBits;gHitSide=c.hitSide;gAffects=c.affects;gBala=c.bala;gLocalDir=c.localDir;gAcq[0]=c.acq[0];gAcq[1]=c.acq[1];gPedFlagRet[0]=c.pedFlag[0];gPedFlagRet[1]=c.pedFlag[1];
    const json input=CollInput(c);
    Side sides[2];json rows[2];
    const u32 args[2]{ptr(v),ptr(gr)};
    const std::vector<const Stub*> use{&env.stubs[S_ADD],&env.stubs[S_SIMPLEST],&env.stubs[S_ISGOTO],&env.stubs[S_COLMODEL],&env.stubs[S_MOVE],&env.stubs[S_REGREF],&env.stubs[S_CLEANREF],&env.stubs[S_ISPLAYER],
        &env.stubs[S_GENDMG],&env.stubs[S_NORM],&env.stubs[S_LIMIT],&env.stubs[S_KILL],&env.stubs[S_HEADING],&env.stubs[S_AFFECTS],&env.stubs[S_COMPUTE],&env.stubs[S_HITSIDE],&env.stubs[S_SAY],
        &env.stubs[S_ANGER],&env.stubs[S_ACQ],&env.stubs[S_PEDFLAG],&env.stubs[S_BALA],&env.stubs[S_LOCALDIR]};
    const uint32 savedTime=*reinterpret_cast<uint32*>(kTimeGlobal),savedA=*reinterpret_cast<uint32*>(kLastNoiseA),savedB=*reinterpret_cast<uint32*>(kLastNoiseB);
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        csArena=csRef;vArena=vRef;vmArena=vmRef;iArena=iRef;tArena=tRef;dArena=dRef;dmArena=dmRef;diArena=diRef;colArena=colRef;pdArena=pdRef;clArena=clRef;aArena=aRef;gArena=gRef;
        *reinterpret_cast<uint32*>(kTimeGlobal)=c.now;*reinterpret_cast<uint32*>(kLastNoiseA)=c.lastA;*reinterpret_cast<uint32*>(kLastNoiseB)=c.lastB;
        nev=0;gMoveN=0;gAcqN=0;gPedFlagN=0;gLastEv=nullptr;
        sides[sd]=Execute(env,*env.route[0],0x604500,use,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,ptr(cs),c.edx,c.stack,0);});
        const uint32 timeAfter=*reinterpret_cast<uint32*>(kTimeGlobal),afterA=*reinterpret_cast<uint32*>(kLastNoiseA),afterB=*reinterpret_cast<uint32*>(kLastNoiseB);
        *reinterpret_cast<uint32*>(kTimeGlobal)=savedTime;*reinterpret_cast<uint32*>(kLastNoiseA)=savedA;*reinterpret_cast<uint32*>(kLastNoiseB)=savedB;
        json obs;
        obs["scanner"]=cs[0];obs["scanner_guards_ok"]=GuardsOk(csArena,csRef,kCsOff,kCsBytes);
        obs["events"]=EventsJson();obs["nevents"]=nev;
        {
            // 0x128: CPhysical::m_pEntityIgnoredCollision (puntero: se etiqueta); el resto de cambios del peaton se listan tal cual
            json ch=json::array();
            for(u32 i=kVOff;i<kVOff+kVBytes;++i)if(vArena[i]!=vRef[i]&&!(i-kVOff>=kPhysIgnoredAt&&i-kVOff<kPhysIgnoredAt+4))ch.push_back({int(i)-int(kVOff),vArena[i]});
            obs["ped_changes"]=ch;
            u32 now32{},ref32{};memcpy(&now32,v+kPhysIgnoredAt,4);memcpy(&ref32,rv+kPhysIgnoredAt,4);
            obs["ignored"]=now32!=ref32?Name(now32):json();
        }
        obs["ped_guards_ok"]=GuardsOk(vArena,vRef,kVOff,kVBytes);
        obs["last_a"]=afterA;obs["last_b"]=afterB;obs["time_unchanged"]=timeAfter==c.now;
        obs["others_intact"]=vmArena==vmRef&&iArena==iRef&&tArena==tRef&&dArena==dRef&&dmArena==dmRef&&diArena==diRef&&colArena==colRef&&pdArena==pdRef&&clArena==clRef&&aArena==aRef&&gArena==gRef;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[sd].reg.ecx;obs["edx"]=sides[sd].reg.edx;}
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&*reinterpret_cast<uint32*>(kTimeGlobal)==savedTime&&*reinterpret_cast<uint32*>(kLastNoiseA)==savedA&&*reinterpret_cast<uint32*>(kLastNoiseB)==savedB;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[sd]=out;
    }
    Finish(env,t,"CCollisionEventScanner::ScanForCollisionEvents",n,input,rows,sides);
}
