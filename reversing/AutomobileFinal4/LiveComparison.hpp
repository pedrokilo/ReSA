#pragma once
// Banco dentro del motor (flujo Small2/PadMouse1/VehModelInfoFinal4): CAutomobile, 99 funciones (lista final 4).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato (hooks activos) sobre coches falsos de CAutomobile
// (0x988 bytes con vtable falsa copiada de las direcciones del EXE), model infos/handling/colmodels/marcos RW falsos y dobles
// trazados de todo lo que toca estado real (RW, audio, particulas, mundo, fisica, colision, peatones). Los dobles se parchean en
// la direccion del EXE y en el simbolo/cuerpo de la DLL (EntryPatch en almacenamiento estatico) y solo trazan en el hilo del
// banco. Cada global tocado se guarda y se restaura en cada ejecucion. Funciones repartidas en CasesG1..CasesG5 (UN banco).
// Ver README.md.
#include "CasesG1.hpp"
#include "CasesG2.hpp"
#include "CasesG3.hpp"
#include "CasesG4.hpp"
#include "CasesG5.hpp"

namespace AutomobileFinal4Live {
constexpr size_t NFN=99;
inline const std::array<Fn,NFN> fns{{
    // ---- G1 ----
    {"CAutomobile","AddMovingCollisionSpeed",0,0x871120,49,256},{"CAutomobile","BreakTowLink",0,0x871120,62,128},{"CAutomobile","ClearHeliOrientation",0x6A2460,0,-1,64},{"CAutomobile","CustomCarPlate_AfterRenderingStop",0x6A2F30,0,-1,128},{"CAutomobile","CustomCarPlate_BeforeRenderingStart",0x6A2F00,0,-1,128},{"CAutomobile","FindWheelWidth",0,0x871120,63,192},{"CAutomobile","GetAllWheelsOffGround",0x6A2F70,0,-1,128},{"CAutomobile","GetCarPitch",0x6A6050,0,-1,256},{"CAutomobile","GetCarRoll",0x6A6010,0,-1,256},{"CAutomobile","GetComponentWorldPosition",0,0x871120,25,192},{"CAutomobile","GetDooorAngleOpenRatio",0,0x871120,30,192},{"CAutomobile","GetDooorAngleOpenRatioU32",0,0x871120,29,192},{"CAutomobile","GetHeightAboveRoad",0,0x871120,53,128},{"CAutomobile","GetMovingCollisionOffset",0x6A2150,0,-1,256},{"CAutomobile","GetNumContactWheels",0,0x871120,55,128},{"CAutomobile","GetTowBarPos",0,0x871120,60,256},{"CAutomobile","GetTowHitchPos",0,0x871120,59,256},{"CAutomobile","IsComponentPresent",0,0x871120,26,128},{"CAutomobile","IsDoorClosed",0,0x871120,36,192},{"CAutomobile","IsDoorClosedU32",0,0x871120,35,192},{"CAutomobile","IsDoorFullyOpen",0,0x871120,34,192},{"CAutomobile","IsDoorFullyOpenU32",0,0x871120,33,192},{"CAutomobile","IsDoorMissing",0,0x871120,38,192},{"CAutomobile","IsDoorMissingU32",0,0x871120,37,192},{"CAutomobile","IsDoorReady",0,0x871120,32,192},{"CAutomobile","IsDoorReadyU32",0,0x871120,31,192},{"CAutomobile","IsInAir",0x6A6140,0,-1,256},{"CAutomobile","IsOpenTopCar",0,0x871120,39,128},{"CAutomobile","IsRoomForPedToLeaveCar",0,0x871120,45,256},{"CAutomobile","KnockPedOutCar",0x6A44C0,0,-1,64},{"CAutomobile","Load",0,0x871120,65,64},{"CAutomobile","Save",0,0x871120,64,64},{"CAutomobile","PlayHornIfNecessary",0x6A3820,0,-1,128},{"CAutomobile","ProcessFlyingCarStuff",0,0x871120,68,128},{"CAutomobile","ReduceHornCounter",0x6A29A0,0,-1,128},{"CAutomobile","ResetSuspension",0,0x871120,67,192},{"CAutomobile","SetAllTaxiLights",0x6A3760,0,-1,64},{"CAutomobile","SetBusDoorTimer",0x6A3860,0,-1,128},{"CAutomobile","SetHeliOrientation",0x6A2450,0,-1,64},{"CAutomobile","SetTaxiLight",0x6A3740,0,-1,64},{"CAutomobile","SetupModelNodes",0x6A0770,0,-1,192},{"CAutomobile","TellHeliToGoToCoors",0x6A2390,0,-1,128},{"CAutomobile","TellPlaneToGoToCoors",0x6A2470,0,-1,192},{"CAutomobile","Teleport",0,0x871120,14,192},
    // ---- G2 ----
    {"CAutomobile","Constructor",0x6B0A90,0,-1,512},{"CAutomobile","Destructor",0x6A61E0,0,-1,128},{"CAutomobile","SetupSuspensionLines",0,0x871120,48,256},{"CAutomobile","Fix",0,0x871120,50,256},{"CAutomobile","SetupDamageAfterLoad",0,0x871120,51,256},{"CAutomobile","FixTyre",0x6A3580,0,-1,128},{"CAutomobile","FixDoor",0x6A35A0,0,-1,256},{"CAutomobile","FixPanel",0x6A3670,0,-1,256},{"CAutomobile","SetTotalDamage",0x6A27F0,0,-1,256},{"CAutomobile","PopDoor",0x6ADEF0,0,-1,256},{"CAutomobile","PopPanel",0x6ADF80,0,-1,256},{"CAutomobile","PopBoot",0x6AF910,0,-1,192},{"CAutomobile","CloseBoot",0x6AFA20,0,-1,192},{"CAutomobile","PopBootUsingPhysics",0x6A44D0,0,-1,128},{"CAutomobile","CloseAllDoors",0x6A4520,0,-1,256},{"CAutomobile","SetBumperDamage",0x6B1350,0,-1,256},{"CAutomobile","SetPanelDamage",0x6B1480,0,-1,256},{"CAutomobile","SetDoorDamage",0x6B1600,0,-1,384},{"CAutomobile","OpenDoor",0,0x871120,27,384},{"CAutomobile","ProcessSwingingDoor",0x6A9D70,0,-1,384},{"CAutomobile","SpawnFlyingComponent",0x6A8580,0,-1,384},{"CAutomobile","RemoveBonnetInPedCollision",0x6AA200,0,-1,192},{"CAutomobile","dmgDrawCarCollidingParticles",0x6A6DC0,0,-1,192},{"CAutomobile","SetTowLink",0,0x871120,61,256},
    // ---- G3 ----
    {"CAutomobile","BoostJumpControl",0x6A3A60,0,-1,256},{"CAutomobile","NitrousControl",0x6A3EA0,0,-1,384},{"CAutomobile","StopNitroEffect",0x6A3E60,0,-1,128},{"CAutomobile","TowTruckControl",0x6A40F0,0,-1,256},{"CAutomobile","TankControl",0x6AE850,0,-1,384},{"CAutomobile","FireTruckControl",0x729B60,0,-1,256},{"CAutomobile","BlowUpCarsInPath",0x6AF110,0,-1,128},{"CAutomobile","BlowUpCar",0,0x871120,41,256},{"CAutomobile","BlowUpCarCutSceneNoExtras",0,0x871120,42,256},{"CAutomobile","ProcessCarOnFireAndExplode",0x6A7090,0,-1,384},{"CAutomobile","ScanForCrimes",0x6ADFF0,0,-1,256},{"CAutomobile","PlaceOnRoadProperly",0x6AF420,0,-1,256},{"CAutomobile","RcbanditCheck1CarWheels",0x6B3F70,0,-1,256},{"CAutomobile","RcbanditCheckHitWheels",0x6B45E0,0,-1,192},{"CAutomobile","HydraulicControl",0x6A07A0,0,-1,384},{"CAutomobile","UpdateMovingCollision",0x6A1460,0,-1,384},{"CAutomobile","ProcessHarvester",0x6A9680,0,-1,256},{"CAutomobile","ProcessAutoBusDoors",0x6A38A0,0,-1,256},{"CAutomobile","PlayCarHorn",0,0x871120,54,128},{"CAutomobile","ProcessControlInputs",0,0x871120,24,384},
    // ---- G4 ----
    {"CAutomobile","ProcessSuspension",0,0x871120,70,512},{"CAutomobile","DoBurstAndSoftGroundRatios",0,0x871120,52,256},{"CAutomobile","DoHoverSuspensionRatios",0,0x871120,69,128},{"CAutomobile","DoSoftGroundResistance",0x6A4AF0,0,-1,256},{"CAutomobile","ProcessCarWheelPair",0x6A4EC0,0,-1,1024},{"CAutomobile","ProcessBuoyancy",0x6A8C00,0,-1,512},{"CAutomobile","ProcessEntityCollision",0,0x871120,22,512},{"CAutomobile","ProcessControlCollisionCheck",0,0x871120,23,256},{"CAutomobile","VehicleDamage",0,0x871120,56,768},
    // ---- G5 ----
    {"CAutomobile","ProcessControl",0,0x871120,10,512},{"CAutomobile","ProcessAI",0,0x871120,66,512}
}};

// Registra los dobles de todos los grupos (antes de tocar el estado de los hooks). Devuelve lo que falte.
inline std::string RegisterStubs(){
    stubs.clear();
    AddStub(0,G_RNG,0x821B1E,bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FnPtr(&S_RandExe));
    AddStub(0,G_NEW,0x82119A,0,FnPtr(&S_New)); AddStub(0,G_NEW,0x821195,0,FnPtr(&S_New)); AddStub(0,G_NEW,0x8214BD,0,FnPtr(&S_Delete)); AddStub(0,G_NEW,0x8213AE,0,FnPtr(&S_Delete));
    std::string miss;
    miss+=StubsG1(); miss+=StubsG2(); miss+=StubsG3(); miss+=StubsG4(); miss+=StubsG5();
    return miss;
}

inline std::array<std::array<uint8,0x20000>,2> arenaDump{}; inline std::array<size_t,2> dumpLen{};
inline std::array<std::array<uint8,sizeof(World)>,2> worldDump{};
// Resumen compacto y determinista (nunca nulo, tambien para funciones void): retorno, nº de eventos y hashes de traza, mundo, arena y globales.
inline json Summary(const json& o){
    return json{{"ret",o["ret"]},{"events",o["trace"].size()},{"trace_fnv",Fnv64(o["trace"].dump().data(),o["trace"].dump().size())},
                {"world",o["world"]},{"arena",o["arena"]},{"arena_top",o["arena_top"]},{"globals",o["globals"]},
                {"fields_fnv",Fnv64(o.dump().data(),o.dump().size())}};
}
inline bool Layout(){
    bool ok=true; json l=json::object();
    auto chk=[&](const char* n,size_t got,size_t want){ const bool g=got==want; ok&=g; l[n]=g; };
    chk("sizeof_car",sizeof(CAutomobile),0x988); chk("car_fits",CAR_SIZE>=0x988 ? 1 : 0,1);
    chk("vt_slots",kExeVt.size(),71);
    chk("matrix",offsetof(CAutomobile,m_matrix),0x14); chk("model",offsetof(CAutomobile,m_nModelIndex),0x22);
    chk("handling",offsetof(CAutomobile,m_pHandlingData),0x384); chk("dmg",offsetof(CAutomobile,m_damageManager),0x5A0);
    chk("wheelcol",offsetof(CAutomobile,m_wheelColPoint),0x724); chk("autoflags",offsetof(CAutomobile,autoFlags),0x868);
    ok&=LayoutG1(l); ok&=LayoutG2(l); ok&=LayoutG3(l); ok&=LayoutG4(l); ok&=LayoutG5(l);
    Write({{"event","layout"},{"ok",ok},{"checks",l}});
    return ok;
}

inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"automobilefinal4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"automobilefinal4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("automobilefinal4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"automobilefinal4-only.txt"); if(f)std::getline(f,only); }

    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    if(!Layout()){ Write({{"event","error"},{"reason","layout"}}); return; }
    // Los Body() de los dobles se calculan aqui, con los hooks tal como arrancaron (activos) y ANTES de crear ningun EntryPatch.
    const auto miss=RegisterStubs(); if(!miss.empty()){ Write({{"event","error"},{"reason","stubs not found"},{"missing",miss}}); return; }

    nregs=0; Reg(&w,sizeof(World)); Reg(arena.data(),arena.size());
    saved.clear();
    Save(MODELPTRS+4u*390u,4u*250u);     // ms_modelInfoPtrs 390..639 (los model infos falsos se instalan aqui)
    InitG1(); InitG2(); InitG3(); InitG4(); InitG5();
    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    const std::vector<uint8> vtSaved(reinterpret_cast<uint8*>(VT_AUTOMOBILE),reinterpret_cast<uint8*>(VT_AUTOMOBILE)+71*4);

    const unsigned savedFP=_controlfp(0,0);
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            auto run=[&](bool candidate)->json {
                Ctx c; c.fn=&fn; c.nm=fn.name; c.i=i; c.f=uint32(f); c.candidate=candidate;
                c.st=uint32(i+1)*2654435761u^uint32(f+1)*0x9E3779B9u^0x57A1u; if(!c.st)c.st=1;
                std::memset(arena.data(),0xCD,arena.size()); nrec=0; recOverflow=false; arenaTop=0; rngState=1; tbPlayerVehicle=nullptr; tbPlayerPed=nullptr;
                std::memset(&w,0xCD,sizeof(World)); ResetVt();
                for(auto* h:hooks)h->State(candidate);
                const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                c.a= fn.slot>=0 ? vtTarget : fn.address;
                c.out=json::object(); c.ret=nullptr;
                _controlfp(_PC_24,_MCW_PC);
                const bool handled=RunG1(c)||RunG2(c)||RunG3(c)||RunG4(c)||RunG5(c);
                _controlfp(savedFP,_MCW_PC|_MCW_RC);
                json out=c.out; out["switches"]=switches; out["handled"]=handled; out["ret"]=c.ret; out["trace"]=Recs();
                out["world"]=Fnv64(&w,sizeof(World)); out["arena"]=Fnv64(arena.data(),arena.size()); out["arena_top"]=arenaTop; out["arena_head"]=ReversingProbe::Bytes(arena.data(),std::min<size_t>(arenaTop+16,0x180));
                uint64 g=1469598103934665603ull; for(auto& s:saved){ g^=Fnv64(reinterpret_cast<void*>(s.addr),s.bytes.size()); g*=1099511628211ull; } out["globals"]=g;
                dumpLen[candidate?1:0]=std::min<size_t>(arenaTop+64,arena.size()); std::memcpy(arenaDump[candidate?1:0].data(),arena.data(),dumpLen[candidate?1:0]);
                std::memcpy(worldDump[candidate?1:0].data(),&w,sizeof(World));
                RestoreAll();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["handled"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(), vals=json::object(); for(auto& [k,v]:original.items())if(candidate[k]!=v){ diff.push_back(k);
                       const auto so=v.dump(), sc=candidate[k].dump(); vals[k]={{"o",so.substr(0,300)},{"c",sc.substr(0,300)}}; }
                // bloques de 64 bytes de la arena y del mundo falso que difieren entre lados (maximo 8 / 12, con los 32 primeros bytes de cada uno)
                json blocks=json::array(); const size_t n=std::min(dumpLen[0],dumpLen[1])/64; int shown=0;
                for(size_t b=0;b<n&&shown<8;++b)if(std::memcmp(arenaDump[0].data()+64*b,arenaDump[1].data()+64*b,64)!=0){
                    blocks.push_back({{"block",b},{"offset",64*b},{"original",ReversingProbe::Bytes(arenaDump[0].data()+64*b,32)},{"candidate",ReversingProbe::Bytes(arenaDump[1].data()+64*b,32)}}); ++shown; }
                json wblocks=json::array(); shown=0;
                for(size_t b=0;b<sizeof(World)/64&&shown<12;++b)if(std::memcmp(worldDump[0].data()+64*b,worldDump[1].data()+64*b,64)!=0){
                    wblocks.push_back({{"offset",64*b},{"original",ReversingProbe::Bytes(worldDump[0].data()+64*b,64)},{"candidate",ReversingProbe::Bytes(worldDump[1].data()+64*b,64)}}); ++shown; }
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"diff_values",vals},{"diff_blocks",blocks},{"diff_world_blocks",wblocks},
                       {"arena_len",json::array({dumpLen[0],dumpLen[1]})},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    const bool restored=AllRestored()&&std::memcmp(reinterpret_cast<void*>(VT_AUTOMOBILE),vtSaved.data(),vtSaved.size())==0&&_controlfp(0,0)==savedFP;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
