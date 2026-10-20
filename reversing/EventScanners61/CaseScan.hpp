// ------------------------------------------------------------------------------------------------ CEventScanner::ScanForEvents (0x607E30)
// Incluido dentro de namespace EventScanners61Live desde Core.hpp.
// Falsos (identicos en ambos lados): los seis escaneres no reconstruidos (0x606580, 0x606890, 0x607D80, 0x6060A0, 0x603E70, 0x603BF0), CVehiclePotentialCollisionScanner::Scan... 0x603720,
// CMentalState::Process 0x6008A0, CPedGeometryAnalyser::IsInAir 0x5F1CB0, CEventHandlerHistory::GetCurrentEvent 0x4B8CA0, CPed::IsPlayer 0x5DF8F0, CTaskManager::GetSimplestActiveTask 0x6819D0,
// CTaskManager::FindTaskByType 0x6817D0, CWeapon::GenerateDamageEvent 0x73A530, CWorld::FindPlayerPed 0x56E210, CPed::IsAlive 0x5E0170, CEventGroup::HasEventOfType 0x4AB5E0 y
// CEventGroup::Add 0x4AB420, CEntity::RegisterReference/CleanUpOldReference. Las llamadas virtuales (MakeAbortable de la tarea, GetEventType del evento actual) van por vtables falsas de arena.
// Reales: los constructores/destructores de los eventos (CEventInAir/OnFire/Escalator/AreaCodes/PedEnteredMyVehicle/Damage).
inline const u32 kEvVpcScan=DefEv("vpc");
inline const u32 kEvScan606580=DefEv("scan_606580");
inline const u32 kEvObj=DefEv("scan_object");
inline const u32 kEvAcq=DefEv("scan_acquaintance");
inline const u32 kEvAttr=DefEv("scan_attractor");
inline const u32 kEvFireScan=DefEv("scan_fire");
inline const u32 kEvSexy=DefEv("scan_sexy");
inline const u32 kEvMental=DefEv("mental_process");
inline const u32 kEvIsInAir=DefEv("is_in_air");
inline const u32 kEvGetCur=DefEv("get_current_event");
inline const u32 kEvCurType=DefEv("current_event_type");
inline const u32 kEvIsPlayer=DefEv("is_player");
inline const u32 kEvAbort=DefEv("make_abortable");
inline const u32 kEvGenDmg=DefEv("generate_damage");
inline const u32 kEvFindTask=DefEv("find_task");
inline const u32 kEvFindPlayer=DefEv("find_player");
inline const u32 kEvIsAlive=DefEv("is_alive");
inline u32 gIsInAir{},gCurPtr{},gCurType{},gIsPlayer{},gAbort{},gFindRet[4]{},gFindN{},gPlayerPtr{},gIsAlive{},gHasEvent{};
// 0x603720 CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents(ped, entities, count): thiscall, `ret 0Ch`
inline void __fastcall FakeVpcScan(void* self,void*,void* ped,void* entities,u32 count){Push(kEvVpcScan,7,u32(uintptr_t(self)),u32(uintptr_t(ped)),u32(uintptr_t(entities)),count);}
// 0x606580: funcion sin this (stdcall con dos argumentos, ECX sin uso)
inline void __fastcall Fake606580(void*,void*,void* ped,void* closest){Push(kEvScan606580,3,u32(uintptr_t(ped)),u32(uintptr_t(closest)));}
inline void __fastcall FakeObjScan(void* self,void*,void* ped){Push(kEvObj,3,u32(uintptr_t(self)),u32(uintptr_t(ped)));}
inline void __fastcall FakeAcqScan(void* self,void*,void* ped,void* entities,u32 count){Push(kEvAcq,7,u32(uintptr_t(self)),u32(uintptr_t(ped)),u32(uintptr_t(entities)),count);}
inline void __fastcall FakeAttrScan(void* self,void*,void* ped){Push(kEvAttr,3,u32(uintptr_t(self)),u32(uintptr_t(ped)));}
inline void __fastcall FakeFireScan(void* self,void*,void* ped){Push(kEvFireScan,3,u32(uintptr_t(self)),u32(uintptr_t(ped)));}
inline void __fastcall FakeSexyScan(void* self,void*,void* ped,void* entities,u32 count){Push(kEvSexy,7,u32(uintptr_t(self)),u32(uintptr_t(ped)),u32(uintptr_t(entities)),count);}
inline void __fastcall FakeMental(void* self,void*,void* ped){Push(kEvMental,3,u32(uintptr_t(self)),u32(uintptr_t(ped)));}
// 0x5F1CB0 CPedGeometryAnalyser::IsInAir(const CPed&): cdecl
inline u32 __cdecl FakeIsInAir(void* ped){Push(kEvIsInAir,1,u32(uintptr_t(ped)));return gIsInAir;}
// 0x4B8CA0 CEventHandlerHistory::GetCurrentEvent(): thiscall sin argumentos
inline u32 __fastcall FakeGetCur(void* hist,void*){Push(kEvGetCur,1,u32(uintptr_t(hist)));return gCurPtr;}
// GetEventType del evento actual (ranura 1 de su vtable falsa)
inline u32 __fastcall FakeCurType(void* ev,void*){Push(kEvCurType,1,u32(uintptr_t(ev)));return gCurType;}
// 0x5DF8F0 CPed::IsPlayer(): thiscall
inline u32 __fastcall FakeIsPlayer(void* ped,void*){Push(kEvIsPlayer,1,u32(uintptr_t(ped)));return gIsPlayer;}
// CTask::MakeAbortable(ped, priority, event) (ranura 6): thiscall, `ret 0Ch`; del evento se registra su tipo (llamada virtual)
inline u32 __fastcall FakeMakeAbortable(void* task,void*,void* ped,u32 priority,void* ev){
    using Fn=int32(__thiscall*)(void*);
    u32 type=0xFFFFFFFFu;
    if(ev){void** vt=*reinterpret_cast<void***>(ev);type=u32(reinterpret_cast<Fn>(vt[1])(ev));}
    Push(kEvAbort,3,u32(uintptr_t(task)),u32(uintptr_t(ped)),priority,type);
    return gAbort;
}
// 0x73A530 CWeapon::GenerateDamageEvent(victim, creator, weaponType, damageFactor, pedPiece, direction): cdecl
inline u32 __cdecl FakeGenDmg(void* victim,void* creator,u32 weapon,i32 damage,u32 piece,u32 dir){Push(kEvGenDmg,3,u32(uintptr_t(victim)),u32(uintptr_t(creator)),weapon,u32(damage),piece,dir&0xFFu);return 1;}
// 0x6817D0 CTaskManager::FindTaskByType(primary, type) const: thiscall, `ret 8`
inline u32 __fastcall FakeFindTask(void* mgr,void*,u32 primary,u32 type){Push(kEvFindTask,1,u32(uintptr_t(mgr)),primary,type);return gFindN<4?gFindRet[gFindN++]:0u;}
// 0x56E210 FindPlayerPed(playerId): cdecl
inline u32 __cdecl FakeFindPlayer(i32 id){Push(kEvFindPlayer,0,u32(id));return gPlayerPtr;}
// 0x5E0170 CPed::IsAlive(): thiscall
inline u32 __fastcall FakeIsAlive(void* ped,void*){Push(kEvIsAlive,1,u32(uintptr_t(ped)));return gIsAlive;}
// 0x4AB5E0 CEventGroup::HasEventOfType(CEvent*) const: thiscall, `ret 4`
inline u32 __fastcall FakeHasEvent(void* group,void*,void* ev){DumpEvent(kEvHas,group,ev,0,false);return gHasEvent;}

alignas(16) inline std::array<uintptr_t,kVtSlots> vtScanTask{},vtScanEv{};
inline void InitScanVtables() {
    for(unsigned k=0;k<kVtSlots;++k){vtScanTask[k]=uintptr_t(kTraps[k]);vtScanEv[k]=uintptr_t(kTraps[k]);}
    vtScanTask[6]=uintptr_t(&FakeMakeAbortable);
    vtScanEv[1]=uintptr_t(&FakeCurType);
}

alignas(16) inline std::array<u8,kEsArena> esArena{},esRef{};
alignas(16) inline std::array<u8,kSpedArena> spArena{},spRef{};
alignas(16) inline std::array<u8,kSintArena> siArena{},siRef{};
alignas(16) inline std::array<u8,kSentArena> scArena{},scRef{};      // contacto del peaton
alignas(16) inline std::array<u8,kSentArena> tcArena{},tcRef{};      // contacto del objetivo
alignas(16) inline std::array<u8,kStgtArena> tgArena{},tgRef{};
alignas(16) inline std::array<u8,kSktArena> ktArena{},ktRef{};
alignas(16) inline std::array<u8,kSvehArena> svArena{},svRef{};
alignas(16) inline std::array<u8,kSplArena> plArena{},plRef{};
alignas(16) inline std::array<u8,kSfireArena> sfArena{},sfRef{};
alignas(16) inline std::array<u8,kStaskArena> tkArena{},tkRef{};
alignas(16) inline std::array<u8,kSevArena> evArena{},evRef{};

inline json ScanInput(const S& s) {
    return {{"family",s.family},{"now",s.now},{"next_scan",s.nextScan},{"dead_timer",s.deadTimer},{"flags",s.flags},{"is_in_air",s.isInAir},{"has_cur",s.hasCur},{"cur_type",s.curType},
            {"has_fire",s.hasFire},{"creator_kind",s.creatorKind},{"is_player",s.isPlayer},{"has_task",s.hasTask},{"abort",s.abort},
            {"find",std::vector<u32>(s.find,s.find+4)},{"target_null",s.targetNull},{"tgt_contact_null",s.tgtContactNull},{"tgt_area",s.tgtArea},{"ped_contact_null",s.pedContactNull},
            {"ped_area",s.pedArea},{"model_index",s.modelIndex},{"g1",s.g1},{"g2",s.g2},{"veh_null",s.vehNull},{"veh_type",s.vehType},{"pl_contact_is_veh",s.plContactIsVeh},
            {"health_bits",s.healthBits},{"is_alive",s.isAlive},{"has_event",s.hasEvent},{"closest_kind",s.closestKind},{"eax",s.eax},{"edx",s.edx},{"stack_seed",s.stack}};
}
inline void CaseScan(Env& env,Totals& t,unsigned n) {
    const S s=MakeS(n);
    u8* const es=esArena.data()+kEsOff;u8* const ped=spArena.data()+kSpedOff;u8* const intel=siArena.data()+kSintOff;u8* const pc=scArena.data()+kSentOff;u8* const tc=tcArena.data()+kSentOff;
    u8* const tg=tgArena.data()+kStgtOff;u8* const kt=ktArena.data()+kSktOff;u8* const veh=svArena.data()+kSvehOff;u8* const pl=plArena.data()+kSplOff;u8* const fire=sfArena.data()+kSfireOff;
    u8* const tk=tkArena.data()+kStaskOff;u8* const ev=evArena.data()+kSevOff;
    regions={{uintptr_t(es),kEsBytes,"E"},{uintptr_t(ped),kSpedBytes,"D"},{uintptr_t(intel),kSintBytes,"I"},{uintptr_t(pc),kSentBytes,"C"},{uintptr_t(tc),kSentBytes,"TC"},{uintptr_t(tg),kStgtBytes,"TG"},
            {uintptr_t(kt),kSktBytes,"KT"},{uintptr_t(veh),kSvehBytes,"V"},{uintptr_t(pl),kSplBytes,"PL"},{uintptr_t(fire),kSfireBytes,"F"},{uintptr_t(tk),kStaskBytes,"TK"},{uintptr_t(ev),kSevBytes,"EV"}};
    FillWords(esRef.data(),kEsArena,Rand(n,100));FillWords(spRef.data(),kSpedArena,Rand(n,101));FillWords(siRef.data(),kSintArena,Rand(n,102));FillWords(scRef.data(),kSentArena,Rand(n,103));
    FillWords(tcRef.data(),kSentArena,Rand(n,104));FillWords(tgRef.data(),kStgtArena,Rand(n,105));FillWords(ktRef.data(),kSktArena,Rand(n,106));FillWords(svRef.data(),kSvehArena,Rand(n,107));
    FillWords(plRef.data(),kSplArena,Rand(n,108));FillWords(sfRef.data(),kSfireArena,Rand(n,109));FillWords(tkRef.data(),kStaskArena,Rand(n,110));FillWords(evRef.data(),kSevArena,Rand(n,111));
    u8* const res=esRef.data()+kEsOff;u8* const rped=spRef.data()+kSpedOff;u8* const rint=siRef.data()+kSintOff;u8* const rpc=scRef.data()+kSentOff;u8* const rtc=tcRef.data()+kSentOff;
    u8* const rtg=tgRef.data()+kStgtOff;u8* const rkt=ktRef.data()+kSktOff;u8* const rveh=svRef.data()+kSvehOff;u8* const rpl=plRef.data()+kSplOff;u8* const rfire=sfRef.data()+kSfireOff;
    u8* const rtk=tkRef.data()+kStaskOff;u8* const rev=evRef.data()+kSevOff;
    (void)res;
    memcpy(res,&s.nextScan,4);
    memcpy(rped+kPedFlagsAt,&s.flags,4);
    const u32 intelPtr=u32(uintptr_t(intel));memcpy(rped+kPedIntelAt,&intelPtr,4);
    memcpy(rped+kPedHealthAt,&s.healthBits,4);
    const u32 contactPtr=s.pedContactNull?0u:u32(uintptr_t(pc));memcpy(rped+kPedContactAt,&contactPtr,4);
    const u32 vehPtr=s.vehNull?0u:u32(uintptr_t(veh));memcpy(rped+kPedVehicleAt,&vehPtr,4);
    const u32 firePtr=s.hasFire?u32(uintptr_t(fire)):0u;memcpy(rped+kPedFireAt,&firePtr,4);
    const u32 closest=s.closestKind==0?0u:s.closestKind==1?u32(uintptr_t(tg)):0xA5A5A5A5u;memcpy(rint+kIntPedClosestAt,&closest,4);
    const u16 mi=u16(s.modelIndex);memcpy(rpc+kEntModelAt,&mi,2);rpc[kEntAreaAt]=u8(s.pedArea);
    rtc[kEntAreaAt]=u8(s.tgtArea);
    const u32 tcPtr=s.tgtContactNull?0u:u32(uintptr_t(tc));memcpy(rtg+kPedContactAt,&tcPtr,4);
    const u32 tgPtr=s.targetNull?0u:u32(uintptr_t(tg));memcpy(rkt+kKillTargetAt,&tgPtr,4);
    memcpy(rveh+kVehTypeAt2,&s.vehType,4);
    const u32 plContact=s.plContactIsVeh?u32(uintptr_t(veh)):u32(uintptr_t(pc));memcpy(rpl+kPedContactAt,&plContact,4);
    const u32 creator=s.creatorKind==0?0u:s.creatorKind==1?u32(uintptr_t(pl)):0x5A5A5A5Au;memcpy(rfire+kFireStartedAt,&creator,4);
    const u32 vtt=u32(uintptr_t(vtScanTask.data()));memcpy(rtk,&vtt,4);
    const u32 vte=u32(uintptr_t(vtScanEv.data()));memcpy(rev,&vte,4);
    gIsInAir=s.isInAir;gCurPtr=s.hasCur?u32(uintptr_t(ev)):0u;gCurType=s.curType;gIsPlayer=s.isPlayer;gTaskPtr=s.hasTask?u32(uintptr_t(tk)):0u;gAbort=s.abort;
    for(int i=0;i<4;++i)gFindRet[i]=s.find[i]?u32(uintptr_t(kt)):0u;
    gPlayerPtr=u32(uintptr_t(pl));gIsAlive=s.isAlive;gHasEvent=s.hasEvent;
    const json input=ScanInput(s);
    Side sides[2];json rows[2];
    const u32 args[1]{u32(uintptr_t(ped))};
    const std::vector<const Stub*> use{&env.stubs[S_VPCSCAN],&env.stubs[S_X606580],&env.stubs[S_OBJSCAN],&env.stubs[S_ACQSCAN],&env.stubs[S_ATTRSCAN],&env.stubs[S_FIRESCAN],&env.stubs[S_SEXYSCAN],
        &env.stubs[S_MENTAL],&env.stubs[S_ISINAIR],&env.stubs[S_GETCUR],&env.stubs[S_ISPLAYER],&env.stubs[S_SIMPLEST],&env.stubs[S_FINDTASK],&env.stubs[S_GENDMG],&env.stubs[S_FINDPLAYER],
        &env.stubs[S_ISALIVE],&env.stubs[S_HASEVENT],&env.stubs[S_ADD],&env.stubs[S_REGREF],&env.stubs[S_CLEANREF]};
    const uint32 savedTime=*reinterpret_cast<uint32*>(kTimeGlobal),savedDead=*reinterpret_cast<uint32*>(kDeadTimerGlobal);
    const uint16 savedG1=*reinterpret_cast<uint16*>(kEscalatorG1),savedG2=*reinterpret_cast<uint16*>(kEscalatorG2);
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        esArena=esRef;spArena=spRef;siArena=siRef;scArena=scRef;tcArena=tcRef;tgArena=tgRef;ktArena=ktRef;svArena=svRef;plArena=plRef;sfArena=sfRef;tkArena=tkRef;evArena=evRef;
        *reinterpret_cast<uint32*>(kTimeGlobal)=s.now;*reinterpret_cast<uint32*>(kDeadTimerGlobal)=s.deadTimer;
        *reinterpret_cast<uint16*>(kEscalatorG1)=u16(s.g1);*reinterpret_cast<uint16*>(kEscalatorG2)=u16(s.g2);
        nev=0;gFindN=0;
        sides[sd]=Execute(env,*env.route[2],0x607E30,use,candidate,[&](uintptr_t entry){return Gate(entry,args,1,s.eax,u32(uintptr_t(es)),s.edx,s.stack,0);});
        const uint32 deadAfter=*reinterpret_cast<uint32*>(kDeadTimerGlobal),timeAfter=*reinterpret_cast<uint32*>(kTimeGlobal);
        *reinterpret_cast<uint32*>(kTimeGlobal)=savedTime;*reinterpret_cast<uint32*>(kDeadTimerGlobal)=savedDead;
        *reinterpret_cast<uint16*>(kEscalatorG1)=savedG1;*reinterpret_cast<uint16*>(kEscalatorG2)=savedG2;
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;
        obs["ped_changes"]=Changes(spArena,spRef,kSpedOff,kSpedBytes);obs["ped_guards_ok"]=GuardsOk(spArena,spRef,kSpedOff,kSpedBytes);
        obs["dead_timer"]=deadAfter;obs["time_after"]=timeAfter;
        obs["others_intact"]=esArena==esRef&&siArena==siRef&&scArena==scRef&&tcArena==tcRef&&tgArena==tgRef&&ktArena==ktRef&&svArena==svRef&&plArena==plRef&&sfArena==sfRef&&tkArena==tkRef&&evArena==evRef;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[sd].reg.ecx;obs["edx"]=sides[sd].reg.edx;}
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&*reinterpret_cast<uint32*>(kTimeGlobal)==savedTime&&*reinterpret_cast<uint32*>(kDeadTimerGlobal)==savedDead
            &&*reinterpret_cast<uint16*>(kEscalatorG1)==savedG1&&*reinterpret_cast<uint16*>(kEscalatorG2)==savedG2;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[sd]=out;
    }
    Finish(env,t,"CEventScanner::ScanForEvents",n,input,rows,sides);
}
