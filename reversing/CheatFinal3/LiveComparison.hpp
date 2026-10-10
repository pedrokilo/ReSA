#pragma once
// Banco dentro del motor (estilo WeaponFinal2/PadMouse1): las 72 funciones de CCheat (GTA SA 1.0 US) mas CCheat::HealthCheat (0x438D60, sin hook:
// original = codigo del EXE, candidato = funcion de la DLL llamada directamente).
// Original (hook de la funcion desactivado: codigo del EXE) frente a candidato (hook activo), mismos fixtures deterministas por caso,
// estado comparado (globales de trucos, reloj, clima, estadisticas, jugadores, pools de peds/vehiculos falsos intercambiados temporalmente,
// lista de referencias falsa) y traza de todas las llamadas salientes con sus argumentos. Ver README.md.
#include "Cases.hpp"

namespace CheatFinal3Live {

inline const std::array<Fn,73> fns{{
    {"CCheat","AddToCheatString",0x438480,0,-1,1024},
    {"CCheat","AdrenalineCheat",0x439880,0,-1,128},
    {"CCheat","AllCarsAreGreatCheat",0x4394E0,0,-1,128},
    {"CCheat","AllCarsAreShitCheat",0x4394B0,0,-1,128},
    {"CCheat","ApacheCheat",0x43A550,0,-1,64},
    {"CCheat","ApplyCheat",0x438370,0,-1,256},
    {"CCheat","BeachPartyCheat",0x439230,0,-1,192},
    {"CCheat","BlackCarsCheat",0x4390F0,0,-1,128},
    {"CCheat","BlowUpCarsCheat",0x439D80,0,-1,256},
    {"CCheat","CloudyWeatherCheat",0x438F60,0,-1,64},
    {"CCheat","CountrysideInvasionCheat",0x439F60,0,-1,192},
    {"CCheat","DoCheats",0x439AF0,0,-1,512},
    {"CCheat","DozerCheat",0x43A660,0,-1,64},
    {"CCheat","DrivebyCheat",0x4398D0,0,-1,192},
    {"CCheat","DuskCheat",0x439540,0,-1,128},
    {"CCheat","ElvisLivesCheat",0x4391D0,0,-1,128},
    {"CCheat","EverybodyAttacksPlayerCheat",0x439C70,0,-1,256},
    {"CCheat","ExtraSunnyWeatherCheat",0x438F50,0,-1,64},
    {"CCheat","FastTimeCheat",0x438F90,0,-1,192},
    {"CCheat","FatCheat",0x439110,0,-1,128},
    {"CCheat","FlyboyCheat",0x43A530,0,-1,64},
    {"CCheat","FoggyWeatherCheat",0x438F80,0,-1,64},
    {"CCheat","FunhouseCheat",0x439720,0,-1,192},
    {"CCheat","GangLandCheat",0x4393D0,0,-1,128},
    {"CCheat","GangsCheat",0x439360,0,-1,128},
    {"CCheat","GolfcartCheat",0x43A520,0,-1,64},
    {"CCheat","HandleSpecialCheats",0x439A10,0,-1,512},
    {"CCheat","HearseCheat",0x43A4F0,0,-1,64},
    {"CCheat","IsZoneStreamingAllowed",0x407410,0,-1,256},
    {"CCheat","JetpackCheat",0x439600,0,-1,192},
    {"CCheat","LoveConquersAllCheat",0x4393F0,0,-1,128},
    {"CCheat","LovefistCheat",0x43A500,0,-1,64},
    {"CCheat","MayhemCheat",0x439B20,0,-1,256},
    {"CCheat","MidnightCheat",0x439510,0,-1,128},
    {"CCheat","MoneyArmourHealthCheat",0x438E40,0,-1,192},
    {"CCheat","MonsterTruckCheat",0x43A680,0,-1,64},
    {"CCheat","MuscleCheat",0x439150,0,-1,128},
    {"CCheat","NinjaCheat",0x439E50,0,-1,192},
    {"CCheat","NotWantedCheat",0x4396C0,0,-1,128},
    {"CCheat","ParachuteCheat",0x4395B0,0,-1,128},
    {"CCheat","PinkCarsCheat",0x4390D0,0,-1,128},
    {"CCheat","PredatorCheat",0x4395A0,0,-1,64},
    {"CCheat","QuadCheat",0x43A560,0,-1,64},
    {"CCheat","RainyWeatherCheat",0x438F70,0,-1,64},
    {"CCheat","ResetCheats",0x438450,0,-1,128},
    {"CCheat","RiotCheat",0x439710,0,-1,128},
    {"CCheat","SandstormCheat",0x439590,0,-1,64},
    {"CCheat","SkinnyCheat",0x439190,0,-1,128},
    {"CCheat","SlowTimeCheat",0x438FC0,0,-1,192},
    {"CCheat","StaminaCheat",0x439930,0,-1,64},
    {"CCheat","StockCar2Cheat",0x43A4C0,0,-1,64},
    {"CCheat","StockCar3Cheat",0x43A4D0,0,-1,64},
    {"CCheat","StockCar4Cheat",0x43A4E0,0,-1,64},
    {"CCheat","StockCarCheat",0x43A4B0,0,-1,64},
    {"CCheat","StormCheat",0x439570,0,-1,64},
    {"CCheat","StuntPlaneCheat",0x43A670,0,-1,64},
    {"CCheat","SuicideCheat",0x438FF0,0,-1,192},
    {"CCheat","SunnyWeatherCheat",0x438F40,0,-1,64},
    {"CCheat","TankCheat",0x43A4A0,0,-1,64},
    {"CCheat","TankerCheat",0x43A570,0,-1,192},
    {"CCheat","TrashmasterCheat",0x43A510,0,-1,64},
    {"CCheat","VehicleCheat",0x43A0B0,0,-1,1024},
    {"CCheat","VehicleSkillsCheat",0x4399D0,0,-1,64},
    {"CCheat","VillagePeopleCheat",0x439DD0,0,-1,192},
    {"CCheat","VortexCheat",0x43A540,0,-1,64},
    {"CCheat","WantedCheat",0x4396F0,0,-1,64},
    {"CCheat","WantedLevelDownCheat",0x438F20,0,-1,64},
    {"CCheat","WantedLevelUpCheat",0x438E90,0,-1,192},
    {"CCheat","WeaponCheat1",0x4385B0,0,-1,192},
    {"CCheat","WeaponCheat2",0x438890,0,-1,192},
    {"CCheat","WeaponCheat3",0x438B30,0,-1,192},
    {"CCheat","WeaponSkillsCheat",0x439940,0,-1,64},
    {"CCheat","HealthCheat",0x438D60,0,-1,256},
}};

inline void SnapRegions(){
    Snap(kCheatString,0x80);                                   // cadena (30), 2 bytes de relleno, banderas (92) y m_bHasPlayerCheated (0x96918C)
    Snap(kHashKeys,4*kTotalCheats); Snap(kFuncs,4*kTotalCheats);
    Snap(kTimeScale,4); Snap(kTimeMs,4); Snap(kClockSec,8); Snap(kPopGang,1); Snap(kHeatHaze,1); Snap(kRain,4); Snap(kWind,4);
    Snap(kPlayers,0x190*2); Snap(kPlayerInFocus,1);
    Snap(kStreamInfo+20u*uint32(kStreamLo),20u*uint32(kStreamHi-kStreamLo));
    Snap(kModelInfo+4u*uint32(kModelLo),4u*uint32(kModelHi-kModelLo));
    Snap(kPedPoolPtr,8); Snap(kCutscene,1); Snap(kAdrenMI,2);
    Snap(kKeyNew,0x270); Snap(kKeyOld,0x270);
    Snap(reinterpret_cast<uintptr_t>(&CReferences::pEmptyList),4); }

// ---- actualizacion principal ----
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cheatfinal3-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cheatfinal3-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cheatfinal3");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"cheatfinal3-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,fns.size()> hooks{};
    for(size_t f=0;f<fns.size();++f){
        hooks[f]=FindHook(fns[f].cls,fns[f].name);
        if(!hooks[f]&&std::string(fns[f].name)!="HealthCheat"){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;} }

    // --- cuerpos de la DLL de los colaboradores (se calculan TODOS antes de crear ningun EntryPatch, con los hooks en su estado inicial) ---
    D.fpp=Body(&FindPlayerPed); D.fpv=Body(&FindPlayerVehicle);
    D.reqModel=Body(&CStreaming::RequestModel); D.loadAll=Body(&CStreaming::LoadAllRequestedModels); D.setDel=Body(&CStreaming::SetModelIsDeletable);
    D.setTxdDel=Body(&CStreaming::SetModelTxdIsDeletable); D.streamPeds=Body(&CStreaming::StreamPedsIntoRandomSlots); D.reclass=Body(&CStreaming::ReclassifyLoadedCars);
    D.setTex=Body(static_cast<void(CPedClothesDesc::*)(const char*,const char*,eClothesTexturePart)>(&CPedClothesDesc::SetTextureAndModel));
    D.rebuild=Body(&CClothes::RebuildPlayer); D.weather=Body(&CWeather::ForceWeatherNow); D.release=Body(&CWeather::ReleaseWeather);
    D.setStat=Body(&CStats::SetStatValue); D.incStat=Body(&CStats::IncrementStat); D.helpMsg=Body(&CHud::SetHelpMessage);
    D.textGet=Body(static_cast<const GxtChar*(CText::*)(const char*)>(&CText::Get));
    D.giveWeapon=Body(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon));
    D.setCur=Body(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon));
    D.giveDelayed=Body(&CPed::GiveDelayedWeapon); D.cheatWanted=Body(&CPlayerPed::CheatWantedLevel); D.goodies=Body(&CPickups::GivePlayerGoodiesWithPickUpMI);
    D.vehNew=Body(static_cast<void*(*)(unsigned)>(&CVehicle::operator new));
    D.taskNew=Body(static_cast<void*(*)(size_t)>(&CTask::operator new)); D.taskDel=Body(static_cast<void(*)(void*)>(&CTask::operator delete));
    D.jetTask=Body(&CPedIntelligence::GetTaskJetPack);
    D.groupAdd=Body(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add));
    D.affects=VtBody(0x85AD78,5); D.compute=Body(&CPedDamageResponseCalculator::ComputeDamageResponse);
    D.setAcq=Body(&CAcquaintance::SetAsAcquaintance); D.setTypeAcq=Body(&CPedType::SetPedTypeAsAcquaintance);
    D.beach=Body(&CCheat::BeachPartyCheat); D.funhouse=Body(&CCheat::FunhouseCheat); D.country=Body(&CCheat::CountrysideInvasionCheat);
    D.handleSpecial=Body(&CCheat::HandleSpecialCheats); D.everybody=Body(&CCheat::EverybodyAttacksPlayerCheat); D.health=Body(&CCheat::HealthCheat);
    D.addKey=Body(&CCheat::AddToCheatString); D.vehicleCheat=Body(&CCheat::VehicleCheat);
    // direccion de la DLL de HealthCheat (sin hook) para el candidato
    const uintptr_t healthDll=Addr(&CCheat::HealthCheat);
    {   struct Nmv { const char* n; uintptr_t v; };
        const Nmv all[]{{"fpp",D.fpp},{"fpv",D.fpv},{"reqModel",D.reqModel},{"loadAll",D.loadAll},{"setDel",D.setDel},{"setTxdDel",D.setTxdDel},{"streamPeds",D.streamPeds},{"reclass",D.reclass},
            {"setTex",D.setTex},{"rebuild",D.rebuild},{"weather",D.weather},{"release",D.release},{"setStat",D.setStat},{"incStat",D.incStat},{"helpMsg",D.helpMsg},{"textGet",D.textGet},
            {"giveWeapon",D.giveWeapon},{"setCur",D.setCur},{"giveDelayed",D.giveDelayed},{"cheatWanted",D.cheatWanted},{"goodies",D.goodies},{"vehNew",D.vehNew},{"taskNew",D.taskNew},{"taskDel",D.taskDel},
            {"jetTask",D.jetTask},{"groupAdd",D.groupAdd},{"compute",D.compute},{"setAcq",D.setAcq},{"setTypeAcq",D.setTypeAcq},{"beach",D.beach},{"funhouse",D.funhouse},{"country",D.country},
            {"handleSpecial",D.handleSpecial},{"everybody",D.everybody},{"health",D.health},{"addKey",D.addKey},{"vehicleCheat",D.vehicleCheat}};
        json missing=json::array(); std::set<uintptr_t> seen; json dup=json::array();
        for(auto& e:all){ if(!e.v||!InDll(e.v))missing.push_back(e.n); else if(!seen.insert(e.v).second)dup.push_back(e.n); }
        if(!missing.empty()){ Write({{"event","error"},{"reason","DLL bodies not found"},{"missing",missing},{"duplicates",dup}}); return; }
        if(!dup.empty())Write({{"event","note"},{"reason","bodies compartidos por mas de un colaborador"},{"duplicates",dup}}); }

    InitVtables(); NameBuffers();

    std::array<bool,fns.size()> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f)if(hooks[f]){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // disposicion de campos de la DLL frente a los desplazamientos del EXE y despacho virtual
    bool layoutOk=true;
    { json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      auto Off=[](const void* base,const void* m){ return uint32(reinterpret_cast<const uint8*>(m)-reinterpret_cast<const uint8*>(base)); };
      alignas(16) static uint8 eb[0x1000]{};
      auto Clear=[&]{ std::memset(eb,0,sizeof(eb)); };
      // --- peds y jugador ---
      { Clear(); auto* e=reinterpret_cast<CEntity*>(eb);
        Chk("CEntity.m_AreaCode (0x43A0D2 mov cl,[eax+2Fh])",[&]{ Clear(); At<uint8>(eb,0x2F)=7; return uint32(uint8(e->GetAreaCode())); }(),7);
        Chk("CPlaceable.m_matrix (0x43A35D [eax+14h])",uint32(offsetof(CPlaceable,m_matrix)),0x14); Chk("CPlaceable.m_placement (0x43A36C lea eax,[edi+4])",uint32(offsetof(CPlaceable,m_placement)),4);
        auto* p=reinterpret_cast<CPed*>(eb);
        Chk("CPed.m_nPedState (0x439831 [eax+530h])",Off(p,&p->m_nPedState),0x530); Chk("CPed.m_fHealth (0x438D83 [eax+540h])",Off(p,&p->m_fHealth),0x540);
        Chk("CPed.m_fArmour (0x438E80 [eax+548h])",Off(p,&p->m_fArmour),0x548); Chk("CPed.m_fCurrentRotation (0x43A3E5 [eax+558h])",Off(p,&p->m_fCurrentRotation),0x558);
        Chk("CPed.m_nPedType (IsPlayer 0x5DF8F0)",Off(p,&p->m_nPedType),0x598); Chk("CPed.m_aWeapons[4].m_Type (0x4398EB [eax+610h])",Off(p,&p->m_aWeapons[4].m_Type),0x610);
        Chk("CPed.GetAcquaintance (0x439BDC lea ebp,[edi+4E0h])",Off(p,&p->GetAcquaintance()),0x4E0);
        Chk("CPed.GetIntelligence (0x43961F [eax+47Ch])",[&]{ Clear(); At<void*>(eb,0x47C)=fPlayerI[0].data(); return U(p->GetIntelligence()); }(),U(fPlayerI[0].data()));
        Chk("CPed.GetPlayerData (0x4398AF [eax+480h])",[&]{ Clear(); At<void*>(eb,0x480)=fPD[0].data(); return U(p->GetPlayerData()); }(),U(fPD[0].data()));
        Chk("CPed.GetEventGroup (0x439696 lea ecx,[eax+68h])",[&]{ Clear(); At<void*>(eb,0x47C)=fPlayerI[0].data(); return U(&p->GetEventGroup()); }(),U(fPlayerI[0].data()+0x68));
        Chk("CPedIntelligence.m_pedScanner (0x439BEF lea ecx,[eax+124h])",[&]{ Clear(); At<void*>(eb,0x47C)=fPlayerI[0].data(); return U(&p->GetIntelligence()->GetPedScanner()); }(),U(fPlayerI[0].data()+0x124));
        Chk("CPedScanner.GetClosestPedInRange (0x5FFF20 mov eax,[ecx+4Ch])",[&]{ Clear(); At<void*>(eb,0x47C)=fPlayerI[0].data(); std::memset(fPlayerI[0].data(),0,0x400); At<void*>(fPlayerI[0].data(),0x124+0x4C)=fJTok.data();
            const uint32 v=U(p->GetIntelligence()->GetPedScanner().GetClosestPedInRange()); std::memset(fPlayerI[0].data(),0,0x400); return v; }(),U(fJTok.data()));
        auto* pd=reinterpret_cast<CPlayerPedData*>(eb);
        Chk("CPlayerPedData.m_pWanted (0x438E9C mov eax,[eax])",Off(pd,&pd->m_pWanted),0); Chk("CPlayerPedData.m_pPedClothesDesc (0x439298 [ecx+4])",Off(pd,&pd->m_pPedClothesDesc),4);
        Chk("CPlayerPedData.m_bAdrenaline (0x4398B8 and ecx,0FFFFFFFDh)",[&]{ Clear(); pd->m_bAdrenaline=true; return At<uint32>(eb,0x34); }(),2);
        auto* w=reinterpret_cast<CWanted*>(eb); Chk("CWanted.m_WantedLevel (0x438EAA [eax+2Ch])",Off(w,&w->m_WantedLevel),0x2C); }
      // --- jugadores globales ---
      Chk("&CWorld::Players",U(&CWorld::Players[0]),0xB7CD98); Chk("&CWorld::PlayerInFocus",U(&CWorld::PlayerInFocus),0xB7CD74);
      Chk("CPlayerInfo.m_nMoney (0x438E4E [eax+0B7CE50h])",U(&CWorld::Players[0].m_nMoney),0xB7CE50); Chk("CPlayerInfo.m_nMaxHealth (0x438D61 [0B7CEE7h])",U(&CWorld::Players[0].m_nMaxHealth),0xB7CEE7);
      Chk("CPlayerInfo.m_nMaxArmour (0x438E41 [0B7CEE8h])",U(&CWorld::Players[0].m_nMaxArmour),0xB7CEE8); Chk("sizeof(CPlayerInfo)",uint32(sizeof(CPlayerInfo)),0x190);
      // --- vehiculos ---
      { Clear(); auto* v=reinterpret_cast<CVehicle*>(eb);
        Chk("CVehicle.m_fHealth (0x438D9C mov [eax+4C0h])",Off(v,&v->m_fHealth),0x4C0); Chk("CVehicle.m_nVehicleType (0x43A426 [esi+590h])",Off(v,&v->m_nVehicleType),0x590);
        Chk("CVehicle.m_nDoorLock (0x43A40C [esi+4F8h])",Off(v,&v->m_nDoorLock),0x4F8);
        auto* b=reinterpret_cast<CBike*>(eb); Chk("CBike.m_BlowUpTimer (0x438E1F [eax+7BCh])",Off(b,&b->m_BlowUpTimer),0x7BC);
        Chk("CBike.bikeFlags.bOnSideStand (0x43A269 or byte [eax+614h],10h)",[&]{ Clear(); b->bikeFlags.bOnSideStand=true; return uint32(At<uint8>(eb,0x614)); }(),0x10);
        auto* a=reinterpret_cast<CAutomobile*>(eb); Chk("CAutomobile.m_fBurnTimer (0x438DFC [eax+8E4h])",Off(a,&a->m_fBurnTimer),0x8E4);
        auto* mi=reinterpret_cast<CVehicleModelInfo*>(eb); Chk("CVehicleModelInfo.m_nVehicleType (0x43A169 [eax+3Ch])",Off(mi,&mi->m_nVehicleType),0x3C);
        Chk("sizeof(CMonsterTruck) (0x43A182 push 99Ch)",uint32(sizeof(CMonsterTruck)),0x99C); Chk("sizeof(CQuadBike) (0x43A1B2 push 9BCh)",uint32(sizeof(CQuadBike)),0x9BC);
        Chk("sizeof(CHeli) (0x43A1E2 push 0A18h)",uint32(sizeof(CHeli)),0xA18); Chk("sizeof(CPlane) (0x43A212 push 0A04h)",uint32(sizeof(CPlane)),0xA04);
        Chk("sizeof(CBoat) (0x43A2D9 push 7E8h)",uint32(sizeof(CBoat)),0x7E8); Chk("sizeof(CBike) (0x43A242 push 814h)",uint32(sizeof(CBike)),0x814);
        Chk("sizeof(CBmx) (0x43A274 push 838h)",uint32(sizeof(CBmx)),0x838); Chk("sizeof(CTrailer) (0x43A2B0 push 9F4h; TankerCheat 0x43A5C2)",uint32(sizeof(CTrailer)),0x9F4);
        Chk("sizeof(CAutomobile) (0x43A302 push 988h)",uint32(sizeof(CAutomobile)),0x988); }
      // --- streaming, modelos, teclado ---
      { auto& info=CStreaming::GetInfo(0);
        Chk("&CStreaming::ms_aInfoForModel",U(&info),0x8E4CC0); Chk("sizeof(CStreamingInfo)",uint32(sizeof(CStreamingInfo)),0x14);
        Chk("CStreamingInfo.m_Flags (0x43A128 movzx ebx,byte [ebp+8E4CC6h])",Off(&info,&info.m_Flags),6); Chk("CStreamingInfo.m_LoadState (0x43A13C mov al,[ebp+8E4CD0h])",Off(&info,&info.m_LoadState),0x10);
        Chk("&CModelInfo::ms_modelInfoPtrs",U(&CModelInfo::ms_modelInfoPtrs[0]),0xA9B0C8);
        Chk("&CPad::NewKeyState",U(&CPad::NewKeyState),0xB73190); Chk("&CPad::OldKeyState",U(&CPad::OldKeyState),0xB72F20);
        Chk("CKeyboardState.standardKeys (0x439AF3 cmp word [esi*2+0B731A8h],0)",Off(&CPad::NewKeyState,&CPad::NewKeyState.standardKeys),0x18); Chk("sizeof(CKeyboardState)",uint32(sizeof(CKeyboardState)),0x270); }
      // --- trucos ---
      Chk("&CCheat::m_aCheatFunctions",U(&CCheat::m_aCheatFunctions[0]),0x8A5B58); Chk("&CCheat::m_aCheatHashKeys",U(&CCheat::m_aCheatHashKeys[0]),0x8A5CC8);
      Chk("&CCheat::m_CheatString",U(&CCheat::m_CheatString[0]),0x969110); Chk("&CCheat::m_aCheatsActive",U(&CCheat::m_aCheatsActive[0]),0x969130);
      Chk("&CCheat::m_bHasPlayerCheated",U(&CCheat::m_bHasPlayerCheated),0x96918C); Chk("TOTAL_CHEATS (0x43851A cmp esi,5Ch)",uint32(TOTAL_CHEATS),92);
      Chk("CCheat::CHEAT_STRING_SIZE",uint32(CCheat::CHEAT_STRING_SIZE),30); Chk("CCheat::CHEAT_MIN_HASH_SIZE (0x4384D0 cmp eax,6)",uint32(CCheat::CHEAT_MIN_HASH_SIZE),6);
      Chk("&CCutsceneMgr::ms_running",U(&CCutsceneMgr::ms_running),0xB5F851); Chk("&CTimer::ms_fTimeScale",U(&CTimer::ms_fTimeScale),0xB7CB64);
      Chk("&CPopulation::m_bOnlyCreateRandomGangMembers",U(&CPopulation::m_bOnlyCreateRandomGangMembers),0xC0FCB3); Chk("&CPostEffects::m_bHeatHazeFX",U(&CPostEffects::m_bHeatHazeFX),0xC402BA);
      Chk("&CWeather::Rain",U(&CWeather::Rain),0xC81324); Chk("&CWeather::Wind",U(&CWeather::Wind),0xC812F0);
      Chk("&CClock::ms_nGameClockHours",U(&CClock::ms_nGameClockHours),0xB70153); Chk("&CClock::ms_nGameClockMinutes",U(&CClock::ms_nGameClockMinutes),0xB70152); Chk("&CClock::ms_nGameClockSeconds",U(&CClock::ms_nGameClockSeconds),0xB70150);
      Chk("&ModelIndices::MI_PICKUP_ADRENALINE",U(&ModelIndices::MI_PICKUP_ADRENALINE),kAdrenMI);
      // --- pools falsos: cabecera {almacen, estados, capacidad} y zancadas del EXE ---
      { std::array<uint8,64> st{}; alignas(16) static uint8 store[3*kPedStride]{};
        CPedPool pp(3,store,st.data());
        Chk("CPool.m_Storage (0x439B90 [ebp])",At<uint32>(&pp,0),U(store)); Chk("CPool.m_SlotState (0x439B93 [ebp+4])",At<uint32>(&pp,4),U(st.data())); Chk("CPool.m_Capacity (0x439B74 [ebp+8])",At<uint32>(&pp,8),3);
        Chk("sizeof(CCopPed) (zancada del pool de peds 0x439B85 imul esi,7C4h)",uint32(sizeof(CCopPed)),uint32(kPedStride)); Chk("CPedPool.GetObjectSize",uint32(pp.GetObjectSize()),uint32(kPedStride));
        std::array<uint8,8> sv{}; alignas(16) static uint8 vs[2*kVehStride]{}; CVehiclePool vp(2,vs,sv.data());
        Chk("CVehiclePool.GetObjectSize (0x439D92 imul edi,edi,0A18h)",uint32(vp.GetObjectSize()),uint32(kVehStride)); }
      // --- eventos y calculadora de dano ---
      { Clear(); auto* e=reinterpret_cast<CEventAcquaintancePedHate*>(eb);
        Chk("CEvent.m_nTimeActive",Off(e,&e->m_nTimeActive),4); Chk("CEvent.m_bValid",Off(e,&e->m_bValid),8);
        Chk("CEventEditableResponse.m_bAddToEventGroup",Off(e,&e->m_bAddToEventGroup),0xC); Chk("CEventEditableResponse.m_TaskId (0x439C29 mov word [esp+2Eh],3E8h)",Off(e,&e->m_TaskId),0xE);
        Chk("CEventEditableResponse.m_FacialExpressionType",Off(e,&e->m_FacialExpressionType),0x10); Chk("CEventAcquaintancePed.m_AcquaintancePed",Off(e,&e->m_AcquaintancePed),0x14);
        Chk("sizeof(CEventAcquaintancePedHate)",uint32(sizeof(CEventAcquaintancePedHate)),0x18);
        auto* s=reinterpret_cast<CEventScriptCommand*>(eb);
        Chk("CEventScriptCommand.m_primaryTaskIndex (0x4B0A12 [eax+0Ch])",Off(s,&s->m_primaryTaskIndex),0xC); Chk("CEventScriptCommand.m_task (0x4B0A1F [eax+10h])",Off(s,&s->m_task),0x10);
        Chk("CEventScriptCommand.m_affectsDeadPeds (0x4B0A22 [eax+14h])",Off(s,&s->m_affectsDeadPeds),0x14);
        auto* d=reinterpret_cast<CEventDamage*>(eb);
        Chk("CEventDamage.m_pSourceEntity",Off(d,&d->m_pSourceEntity),0x14); Chk("CEventDamage.m_nStartTime",Off(d,&d->m_nStartTime),0x18); Chk("CEventDamage.m_weaponType",Off(d,&d->m_weaponType),0x1C);
        Chk("CEventDamage.m_pedPieceType",Off(d,&d->m_pedPieceType),0x20); Chk("CEventDamage.m_ucDirection",Off(d,&d->m_ucDirection),0x24);
        Chk("CEventDamage.m_damageResponse (0x439051 lea ecx,[esp+50h])",Off(d,&d->m_damageResponse),0x38);
        auto* r0=&d->m_damageResponse; Chk("CPedDamageResponse.m_bDamageCalculated (0x43907B mov byte [esp+56h],1)",Off(r0,&r0->m_bDamageCalculated),0xA);
        auto* c=reinterpret_cast<CPedDamageResponseCalculator*>(eb); Chk("CPedDamageResponseCalculator.m_pDamager",Off(c,&c->m_pDamager),0); Chk("CPedDamageResponseCalculator.m_fDamageFactor",Off(c,&c->m_fDamageFactor),4);
        Chk("CPedDamageResponseCalculator.m_bodyPart",Off(c,&c->m_bodyPart),8); Chk("CPedDamageResponseCalculator.m_weaponType",Off(c,&c->m_weaponType),0xC); Chk("CPedDamageResponseCalculator.m_bSpeak",Off(c,&c->m_bSpeak),0x10); }
      // --- enumeraciones y constantes ---
      Chk("PED_TYPE_PLAYER1/PLAYER2/CIVMALE/PROSTITUTE",uint32(PED_TYPE_PLAYER1)*1000000u+uint32(PED_TYPE_PLAYER2)*10000u+uint32(PED_TYPE_CIVMALE)*100u+uint32(PED_TYPE_PROSTITUTE),1u*10000u+4u*100u+22u);
      Chk("ACQUAINTANCE_HATE (0x439B5E push 4)",uint32(ACQUAINTANCE_HATE),4); Chk("TASK_COMPLEX_KILL_PED_ON_FOOT (0x439C29 3E8h)",uint32(TASK_COMPLEX_KILL_PED_ON_FOOT),0x3E8);
      Chk("TASK_PRIMARY_PRIMARY (0x439664 push 3)",uint32(TASK_PRIMARY_PRIMARY),3); Chk("PEDSTATE_DRIVING (cmp ecx,32h)",uint32(PEDSTATE_DRIVING),0x32);
      Chk("WEAPON_KATANA/PARACHUTE/MICRO_UZI",uint32(WEAPON_KATANA)*10000u+uint32(WEAPON_PARACHUTE)*100u+uint32(WEAPON_MICRO_UZI),8u*10000u+46u*100u+28u);
      Chk("STAT_TIMES_CHEATED (0x438560 push 89h)",uint32(STAT_TIMES_CHEATED),0x89); Chk("STAT_FAT/MUSCLE/STAMINA",uint32(STAT_FAT)*10000u+uint32(STAT_MUSCLE)*100u+uint32(STAT_STAMINA),21u*10000u+23u*100u+22u);
      Chk("STREAMING_GAME_REQUIRED (0x43A14E test bl,2)",uint32(STREAMING_GAME_REQUIRED),2); Chk("RANDOM_VEHICLE (0x43A1A3 push 1)",uint32(RANDOM_VEHICLE),1);
      Chk("CARLOCK_UNLOCKED (0x43A40C mov [esi+4F8h],1)",uint32(CARLOCK_UNLOCKED),1); Chk("STATUS_ABANDONED (0x43A405 or dl,20h)",uint32(STATUS_ABANDONED),4);
      Chk("AREA_CODE_NORMAL_WORLD",uint32(AREA_CODE_NORMAL_WORLD),0);
      Chk("VEHICLE_TYPE_BIKE/BOAT/AUTOMOBILE",uint32(VEHICLE_TYPE_BIKE)*100u+uint32(VEHICLE_TYPE_BOAT)*10u+uint32(VEHICLE_TYPE_AUTOMOBILE),9u*100u+5u*10u);
      Chk("CLOTHES_TEXTURE_TORSO/HEAD/LEGS/SHOES/GLASSES/HATS/SPECIAL",uint32(CLOTHES_TEXTURE_TORSO)*10000000u+uint32(CLOTHES_TEXTURE_HEAD)*1000000u+uint32(CLOTHES_TEXTURE_LEGS)*100000u+uint32(CLOTHES_TEXTURE_SHOES)*10000u+uint32(CLOTHES_TEXTURE_GLASSES)*100u+uint32(CLOTHES_TEXTURE_HATS)*1u,
          0u*10000000u+1u*1000000u+2u*100000u+3u*10000u+15u*100u+16u);
      Chk("CLOTHES_TEXTURE_SPECIAL (0x4393F0 push 11h)",uint32(CLOTHES_TEXTURE_SPECIAL),0x11);
      Chk("WEATHER_EXTRASUNNY_LA/SUNNY_LA/CLOUDY_LA/FOGGY_SF/RAINY_COUNTRYSIDE/SANDSTORM_DESERT",uint32(WEATHER_EXTRASUNNY_LA)*1000000u+uint32(WEATHER_SUNNY_LA)*100000u+uint32(WEATHER_CLOUDY_LA)*10000u+uint32(WEATHER_FOGGY_SF)*100u+uint32(WEATHER_RAINY_COUNTRYSIDE)*1u+uint32(WEATHER_SANDSTORM_DESERT)*10000000u,
          0u+1u*100000u+4u*10000u+9u*100u+16u+19u*10000000u);
      Chk("MODEL_PETROTR/PETRO/GUN_PARA/KATANA",uint32(MODEL_PETROTR)*1000000u+uint32(MODEL_PETRO)*1000u+uint32(MODEL_GUN_PARA)+uint32(MODEL_KATANA),584u*1000000u+514u*1000u+371u+339u);
      Chk("UNLOAD_MODEL",uint32(UNLOAD_MODEL),0xFFFFFFFEu);
      // --- despacho virtual: el EXE llama por la ranura (BlowUpCar 41: call [edx+0A4h]; Fix 50: jmp [edx+0C8h]) y la DLL tiene que usar la misma ---
      { trace=json::array(); Clear(); At<void*>(eb,0)=vehVt.data(); reinterpret_cast<CVehicle*>(eb)->BlowUpCar(nullptr,false);
        Chk("DLL: CVehicle::BlowUpCar usa la ranura 41 de la vtable",uint32(trace.size()==1&&trace[0][0]=="blow_up_car"),1);
        trace=json::array(); Clear(); At<void*>(eb,0)=vehVt.data(); reinterpret_cast<CBike*>(eb)->Fix();
        Chk("DLL: CBike::Fix (CVehicle::Fix) usa la ranura 50 de la vtable",uint32(trace.size()==1&&trace[0][0]=="vehicle_fix"),1);
        trace=json::array(); Clear(); At<void*>(eb,0)=vehVt.data(); reinterpret_cast<CAutomobile*>(eb)->Fix();
        Chk("DLL: CAutomobile::Fix usa la ranura 50 de la vtable",uint32(trace.size()==1&&trace[0][0]=="vehicle_fix"),1);
        trace=json::array(); }
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty())SnapRegions();
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        Patches P; AddCommon(P,nm);     // dobles de los colaboradores: una vez por funcion
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2B7Au; if(!stt)stt=1; Rng r{stt};
                trace=json::array();
                bool switches=true; uintptr_t target=fn.address;
                if(hooks[f]){ hooks[f]->State(candidate); switches= ReversingProbe::JumpsToDll(fn.address)==candidate; }   // despues de State(), no antes
                else { switches= !ReversingProbe::JumpsToDll(fn.address); if(candidate)target=healthDll; }                   // HealthCheat: sin hook
                json out={{"switches",switches}};
                _controlfp(_PC_24,_MCW_PC);                  // x87 a 24 bits como el juego
                out["state"]=RunCase(nm,target,r);
                const unsigned fp=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                out["fp"]=fp; out["trace"]=trace;
                RestoreRegions();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& r)->json{ const auto s=r.dump(); return std::format("{:016x}",Fnv(s.data(),s.size())); };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                json sd=json::array(); if(original.contains("state")&&candidate.contains("state")&&original["state"].is_object())for(auto& [k,v]:original["state"].items())if(candidate["state"][k]!=v)sd.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"state_differs",sd},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f)if(hooks[f]){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    bool restored=RegionsRestored()&&_controlfp(0,0)==savedFP;
    for(size_t f=0;f<fns.size();++f)if(hooks[f])restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",restored}});
}
}
