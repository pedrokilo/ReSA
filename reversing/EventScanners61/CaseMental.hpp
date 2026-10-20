// ------------------------------------------------------------------------------------------------ CMentalState::Process (0x6008A0)
// Incluido dentro de namespace EventScanners61Live desde Core.hpp.
alignas(16) inline std::array<u8,kMsArena> msArena{},msRef{};
alignas(16) inline std::array<u8,kMpedArena> mpedArena{},mpedRef{};
alignas(16) inline std::array<u8,kMvehArena> mvehArena{},mvehRef{};
alignas(16) inline std::array<u8,kMintArena> mintArena{},mintRef{};

inline json MentalInput(const M& m) {
    return {{"family",m.family},{"anger",m.anger},{"last_anger",m.lastAnger},{"ped_hp",m.pedHp},{"old_ped_hp",m.oldPedHp},{"veh_hp",m.vehHp},{"old_veh_hp",m.oldVehHp},
            {"ped_health_bits",m.pedHealthBits},{"veh_health_bits",m.vehHealthBits},{"in_vehicle",m.inVehicle},{"veh_null",m.vehNull},{"flags_noise",m.flagsNoise},
            {"timer_fill",m.timerFill},{"eax",m.eax},{"edx",m.edx},{"stack_seed",m.stack}};
}
inline void CaseMental(Env& env,Totals& t,unsigned n) {
    const M m=MakeM(n);
    u8* const ms=msArena.data()+kMsOff;u8* const ped=mpedArena.data()+kMpedOff;u8* const veh=mvehArena.data()+kMvehOff;u8* const intel=mintArena.data()+kMintOff;
    regions={{uintptr_t(ms),kMsBytes,"M"},{uintptr_t(ped),kMpedBytes,"D"},{uintptr_t(veh),kMvehBytes,"V"},{uintptr_t(intel),kMintBytes,"I"}};
    FillWords(msRef.data(),kMsArena,Rand(n,50));FillWords(mpedRef.data(),kMpedArena,Rand(n,51));FillWords(mvehRef.data(),kMvehArena,Rand(n,52));FillWords(mintRef.data(),kMintArena,Rand(n,53));
    u8* const rms=msRef.data()+kMsOff;u8* const rped=mpedRef.data()+kMpedOff;u8* const rveh=mvehRef.data()+kMvehOff;
    // estado mental: bytes dados; el temporizador (+2..+0xF) con basura que no debe tocarse
    rms[kMsAnger]=u8(m.anger);rms[kMsLastAnger]=u8(m.lastAnger);rms[kMsPedHp]=u8(m.pedHp);rms[kMsOldPedHp]=u8(m.oldPedHp);rms[kMsVehHp]=u8(m.vehHp);rms[kMsOldVehHp]=u8(m.oldVehHp);
    // peaton: bandera de "en vehiculo" (bit 8 de +0x46C), inteligencia, salud y vehiculo
    u32 flags=m.flagsNoise;flags=m.inVehicle?(flags|0x100u):(flags&~0x100u);memcpy(rped+kPedFlagsAt,&flags,4);
    const u32 intelPtr=u32(uintptr_t(intel));memcpy(rped+kPedIntelAt,&intelPtr,4);
    memcpy(rped+kPedHealthAt,&m.pedHealthBits,4);
    const u32 vehPtr=m.vehNull?0u:u32(uintptr_t(veh));memcpy(rped+kPedVehicleAt,&vehPtr,4);
    memcpy(rveh+kVehHealthAt,&m.vehHealthBits,4);
    const json input=MentalInput(m);
    Side sides[2];json rows[2];
    const u32 args[1]{u32(uintptr_t(ped))};
    const std::vector<const Stub*> use{&env.stubs[S_ADD]};
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        msArena=msRef;mpedArena=mpedRef;mvehArena=mvehRef;mintArena=mintRef;
        nev=0;
        sides[s]=Execute(env,*env.route[0],0x6008A0,use,candidate,[&](uintptr_t entry){return Gate(entry,args,1,m.eax,u32(uintptr_t(ms)),m.edx,m.stack,0);});
        json obs;
        obs["state"]=HexBytes(ms,kMsBytes);
        obs["state_guards_ok"]=GuardsOk(msArena,msRef,kMsOff,kMsBytes);
        obs["events"]=EventsJson();obs["nevents"]=nev;
        obs["others_intact"]=mpedArena==mpedRef&&mvehArena==mvehRef&&mintArena==mintRef;
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(env,t,"CMentalState::Process",n,input,rows,sides);
}
