// ------------------------------------------------------------------------------------------------ CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents (0x603720)
// Incluido dentro de namespace EventScanners61Live desde Core.hpp.
// Falsos (identicos en ambos lados): CTaskManager::GetSimplestActiveTask 0x6819D0, CTask::IsGoToTask 0x61A360, CEntity::GetColModel 0x535300, CPedGeometryAnalyser::GetIsLineOfSightClear (ped)
// 0x5F5A30, CPedIntelligence::GetMoveStateFromGoToTask 0x601D70 y CEventGroup::Add 0x4AB420. Reales (en la DLL, con su hook): CVector::FromMultiply 0x59C670, el constructor/destructor del evento.
inline const u32 kEvSimplest=DefEv("simplest");
inline const u32 kEvIsGoTo=DefEv("isgoto");
inline const u32 kEvCol=DefEv("colmodel");
inline const u32 kEvLos=DefEv("los");
inline const u32 kEvMove=DefEv("move");
inline const u32 kEvRegRef=DefEv("regref");
inline const u32 kEvCleanRef=DefEv("cleanref");
inline u32 gTaskPtr{},gGoto{},gColPtr{},gClear{},gLosWrite{},gLosDist{},gMove{};
// 0x6819D0 CTaskManager::GetSimplestActiveTask(): thiscall sin argumentos
inline u32 __fastcall FakeSimplest(void* mgr,void*){Push(kEvSimplest,1,u32(uintptr_t(mgr)));return gTaskPtr;}
// 0x61A360 CTask::IsGoToTask(task): cdecl
inline u32 __cdecl FakeIsGoto(void* task){Push(kEvIsGoTo,1,u32(uintptr_t(task)));return gGoto;}
// 0x535300 CEntity::GetColModel(): thiscall sin argumentos
inline u32 __fastcall FakeCol(void* entity,void*){Push(kEvCol,1,u32(uintptr_t(entity)));return gColPtr;}
// 0x5F5A30 CPedGeometryAnalyser::GetIsLineOfSightClear(const CPed&, const CVector&, CEntity&, float&): cdecl. El puntero de salida es de la pila del llamador: se registra su contenido previo.
inline u32 __cdecl FakeLos(void* ped,const float* pos,void* entity,float* out){
    u32 px{},py{},pz{};memcpy(&px,pos,4);memcpy(&py,pos+1,4);memcpy(&pz,pos+2,4);
    u32 before{};memcpy(&before,out,4);
    Push(kEvLos,7,u32(uintptr_t(ped)),u32(uintptr_t(pos)),u32(uintptr_t(entity)),before,px,py,pz);
    if(gLosWrite)memcpy(out,&gLosDist,4);
    return gClear;
}
// 0x601D70 CPedIntelligence::GetMoveStateFromGoToTask(): thiscall sin argumentos
inline u32 __fastcall FakeMove(void* intel,void*){Push(kEvMove,1,u32(uintptr_t(intel)));return gMove;}

// 0x571B70 CEntity::RegisterReference(CEntity**) y 0x571A00 CEntity::CleanUpOldReference(CEntity**): thiscall con un argumento (puntero a la pila del llamador: no se etiqueta)
inline void __fastcall FakeRegRef(void* entity,void*,void* ref){(void)ref;Push(kEvRegRef,1,u32(uintptr_t(entity)));}
inline void __fastcall FakeCleanRef(void* entity,void*,void* ref){(void)ref;Push(kEvCleanRef,1,u32(uintptr_t(entity)));}

alignas(16) inline std::array<u8,kVsArena> vsArena{},vsRef{};
alignas(16) inline std::array<u8,kVpedArena> vpedArena{},vpedRef{};
alignas(16) inline std::array<u8,kVmatArena> vpmArena{},vpmRef{};     // matriz del peaton
alignas(16) inline std::array<u8,kVvehArena> vvehArena{},vvehRef{};
alignas(16) inline std::array<u8,kVmatArena> vvmArena{},vvmRef{};     // matriz del vehiculo
alignas(16) inline std::array<u8,kVintArena> vintArena{},vintRef{};
alignas(16) inline std::array<u8,kVtaskArena> vtaskArena{},vtaskRef{};
alignas(16) inline std::array<u8,kVcolArena> vcolArena{},vcolRef{};

inline json VpcInput(const V& v) {
    return {{"family",v.family},{"now",v.now},{"t_start",v.tStart},{"t_interval",v.tInterval},{"t_started",v.tStarted},{"t_stopped",v.tStopped},{"task_null",v.taskNull},{"is_goto",v.isGoto},
            {"veh_null",v.vehNull},{"ped_mat_null",v.pedMatNull},{"veh_type",v.vehType},{"flags3",v.flags3},
            {"veh_pos",std::vector<u32>(v.vehPos,v.vehPos+3)},{"vm",std::vector<u32>(v.vm,v.vm+9)},{"mn",std::vector<u32>(v.mn,v.mn+3)},{"mx",std::vector<u32>(v.mx,v.mx+3)},
            {"ped_pos",std::vector<u32>(v.pedPos,v.pedPos+3)},{"tp",std::vector<u32>(v.tp,v.tp+3)},{"clear",v.clear},{"los_write",v.losWrite},{"los_dist",v.losDist},{"move",v.move},
            {"eax",v.eax},{"edx",v.edx},{"stack_seed",v.stack}};
}
inline void CaseVpc(Env& env,Totals& t,unsigned n) {
    const V v=MakeV(n);
    u8* const sc=vsArena.data()+kVsOff;u8* const ped=vpedArena.data()+kVpedOff;u8* const pm=vpmArena.data()+kVmatOff;u8* const veh=vvehArena.data()+kVvehOff;u8* const vm=vvmArena.data()+kVmatOff;
    u8* const intel=vintArena.data()+kVintOff;u8* const task=vtaskArena.data()+kVtaskOff;u8* const col=vcolArena.data()+kVcolOff;
    regions={{uintptr_t(sc),kVsBytes,"S"},{uintptr_t(ped),kVpedBytes,"D"},{uintptr_t(pm),kVmatBytes,"DM"},{uintptr_t(veh),kVvehBytes,"V"},{uintptr_t(vm),kVmatBytes,"VM"},
            {uintptr_t(intel),kVintBytes,"I"},{uintptr_t(task),kVtaskBytes,"T"},{uintptr_t(col),kVcolBytes,"C"}};
    FillWords(vsRef.data(),kVsArena,Rand(n,100));FillWords(vpedRef.data(),kVpedArena,Rand(n,101));FillWords(vpmRef.data(),kVmatArena,Rand(n,102));FillWords(vvehRef.data(),kVvehArena,Rand(n,103));
    FillWords(vvmRef.data(),kVmatArena,Rand(n,104));FillWords(vintRef.data(),kVintArena,Rand(n,105));FillWords(vtaskRef.data(),kVtaskArena,Rand(n,106));FillWords(vcolRef.data(),kVcolArena,Rand(n,107));
    u8* const rs=vsRef.data()+kVsOff;u8* const rped=vpedRef.data()+kVpedOff;u8* const rpm=vpmRef.data()+kVmatOff;u8* const rveh=vvehRef.data()+kVvehOff;u8* const rvm=vvmRef.data()+kVmatOff;
    u8* const rint=vintRef.data()+kVintOff;u8* const rtask=vtaskRef.data()+kVtaskOff;u8* const rcol=vcolRef.data()+kVcolOff;
    // temporizador del escaner: +0 inicio, +4 intervalo, +8 started (byte), +9 stopped (byte)
    memcpy(rs,&v.tStart,4);memcpy(rs+4,&v.tInterval,4);rs[8]=u8(v.tStarted);rs[9]=u8(v.tStopped);
    // peaton: posicion del placement (+4) y matriz (+0x14) con su posicion (+0x30); si no hay matriz la posicion sale del placement
    const u32 pmPtr=v.pedMatNull?0u:u32(uintptr_t(pm));memcpy(rped+kMatrixAt,&pmPtr,4);
    if(v.pedMatNull)memcpy(rped+kPlacePos,v.pedPos,12);else memcpy(rped+kPlacePos,v.pedMatPos,12);
    memcpy(rpm+kMatPos,v.pedPos,12);
    rped[kPedFlags3At]=u8(v.flags3);
    const u32 intelPtr=u32(uintptr_t(intel));memcpy(rped+kPedIntelAt,&intelPtr,4);
    // vehiculo: siempre con matriz (el EXE la usa sin comprobar nulo); la posicion del placement es basura
    const u32 vmPtr=u32(uintptr_t(vm));memcpy(rveh+kMatrixAt,&vmPtr,4);memcpy(rveh+kPlacePos,v.vehPlace,12);
    memcpy(rveh+kVehTypeAt,&v.vehType,4);
    memcpy(rvm+0x00,v.vm+0,12);memcpy(rvm+0x10,v.vm+3,12);memcpy(rvm+kMatUp,v.vm+6,12);memcpy(rvm+kMatPos,v.vehPos,12);
    const u32 vehPtr=v.vehNull?0u:u32(uintptr_t(veh));memcpy(rint+kIntClosestVeh,&vehPtr,4);
    memcpy(rtask+kTaskTargetAt,v.tp,12);
    memcpy(rcol,v.mn,12);memcpy(rcol+12,v.mx,12);
    gTaskPtr=v.taskNull?0u:u32(uintptr_t(task));gGoto=v.isGoto;gColPtr=u32(uintptr_t(col));gClear=v.clear;gLosWrite=v.losWrite;gLosDist=v.losDist;gMove=v.move;
    const json input=VpcInput(v);
    Side sides[2];json rows[2];
    const u32 args[3]{u32(uintptr_t(ped)),u32(uintptr_t(intel)),16u};   // entities y count: la funcion no los lee
    const std::vector<const Stub*> use{&env.stubs[S_SIMPLEST],&env.stubs[S_ISGOTO],&env.stubs[S_COLMODEL],&env.stubs[S_LOS],&env.stubs[S_MOVE],&env.stubs[S_ADD],&env.stubs[S_REGREF],&env.stubs[S_CLEANREF]};
    const uint32 savedTime=*reinterpret_cast<uint32*>(kTimeGlobal);
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vsArena=vsRef;vpedArena=vpedRef;vpmArena=vpmRef;vvehArena=vvehRef;vvmArena=vvmRef;vintArena=vintRef;vtaskArena=vtaskRef;vcolArena=vcolRef;
        *reinterpret_cast<uint32*>(kTimeGlobal)=v.now;
        nev=0;
        sides[s]=Execute(env,*env.route[1],0x603720,use,candidate,[&](uintptr_t entry){return Gate(entry,args,3,v.eax,u32(uintptr_t(sc)),v.edx,v.stack,0);});
        *reinterpret_cast<uint32*>(kTimeGlobal)=savedTime;
        json obs;
        obs["timer"]=HexBytes(sc,kVsBytes);
        obs["timer_guards_ok"]=GuardsOk(vsArena,vsRef,kVsOff,kVsBytes);
        obs["events"]=EventsJson();obs["nevents"]=nev;
        obs["others_intact"]=vpedArena==vpedRef&&vpmArena==vpmRef&&vvehArena==vvehRef&&vvmArena==vvmRef&&vintArena==vintRef&&vtaskArena==vtaskRef&&vcolArena==vcolRef;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&*reinterpret_cast<uint32*>(kTimeGlobal)==savedTime;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents",n,input,rows,sides);
}
