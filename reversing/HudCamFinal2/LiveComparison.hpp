#pragma once
// Banco dentro del motor (flujo simple, estilo Small2/PadMouse1/CamHud1/PickWeap1): HudCamFinal2 = las 9 funciones que dejaron fuera CamHud1 y PickWeap1:
// CHud::Draw, DrawAfterFade, DrawRadar, DrawWeaponIcon, Initialise, Shutdown; CCam::Process_1stPerson, Process_Rocket; CPickups::Update.
// Original (hook desactivado = codigo del EXE) frente a candidato (hook activo) con el mismo fixture determinista por caso. Se ejecuta dentro del juego
// completo (partida cargada) y se engancha en CGame::Process con probe_build.py. Colaboradores simulados con EntryPatch (direccion del EXE + simbolo de la
// DLL + su cuerpo), activados solo mientras se prueba la funcion que los usa; los estados RW pasan por una tabla de motor falsa (puntero 0xC97B24).
// Ver README.md para los dominios, el enmascarado y lo que queda fuera.
#include "PickupCases.hpp"
#include "CamCases.hpp"

namespace HudCamFinal2Live {

inline const std::array<Fn, 9> fns{{
    {"CHud","Draw",0x58FAE0,0,-1,1024},
    {"CHud","DrawAfterFade",0x58D490,0,-1,512},
    {"CHud","DrawRadar",0x58A330,0,-1,1024},
    {"CHud","DrawWeaponIcon",0x58D7D0,0,-1,768},
    {"CHud","Initialise",0x5BA850,0,-1,64},
    {"CHud","Shutdown",0x588850,0,-1,64},
    {"CCam","Process_1stPerson",0x517EA0,0,-1,1024},
    {"CCam","Process_Rocket",0x511B50,0,-1,1024},
    {"CPickups","Update",0x458DE0,0,-1,1024},
}};
inline const std::array<CaseFn, 9> caseFns{{
    CaseDraw, CaseAfterFade, CaseRadar, CaseWeaponIcon, CaseInitialise, CaseShutdown, CaseFirstPerson, CaseRocket, CaseUpdate,
}};

inline std::unique_ptr<Patches> PatchesFor(const std::string& cls, const std::string& name) {
    if (cls == "CHud") return HudPatches(name);
    if (cls == "CCam") return CamPatches(name);
    if (cls == "CPickups") return PickupPatches(name);
    return nullptr;
}

// Armas con modelo > 0 cuya ranura de TXD esta en uso (el EXE desreferencia la direccion 0 con una ranura libre): se leen del juego real
inline void ScanIconWeapons() {
    iconWeapons.clear();
    auto* pool = reinterpret_cast<uint8*>(CTxdStore::ms_pTxdPool); if (!pool) return;
    const uint8* objs = *reinterpret_cast<uint8**>(pool); const uint8* flags = *reinterpret_cast<uint8**>(pool + 4); const int32 cap = *reinterpret_cast<int32*>(pool + 8);
    std::set<uintptr_t> seen;
    for (int t = 1; t < 47; ++t) {
        auto* wi = CWeaponInfo::GetWeaponInfo(eWeaponType(t), eWeaponSkill::STD); if (!wi) continue;
        const int32 id = wi->m_nModelId1; if (id <= 0 || id >= 20000) continue;
        auto* mi = CModelInfo::GetModelInfo(id); if (!mi) continue;
        const int32 txd = mi->m_nTxdIndex; if (txd < 0 || txd >= cap || (flags[txd] & 0x80)) continue;
        const uintptr_t def = uintptr_t(objs) + 12u * uint32(txd);
        iconWeapons.push_back({ t, id, def });
        if (seen.insert(def).second) globals.push_back({ def, 12 });
    }
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "hudcamfinal2-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "hudcamfinal2-runtime.jsonl"); Write({ {"event", "waiting_for_player"} }); ReversingProbe::InstallFailureCapture("hudcamfinal2"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    realPed = player; realVeh = FindPlayerVehicle();
    std::string only; { std::ifstream f(dir / "hudcamfinal2-only.txt"); if (f) std::getline(f, only); }
    std::vector<Hook*> hooks(fns.size(), nullptr);
    for (size_t f = 0; f < fns.size(); ++f) if (!(hooks[f] = FindHook(fns[f].cls, fns[f].name))) { Write({ {"event", "error"}, {"reason", "hook not found"}, {"function", std::string(fns[f].cls) + "::" + fns[f].name} }); return; }
    std::vector<bool> enabled(fns.size()), locked(fns.size());
    for (size_t f = 0; f < fns.size(); ++f) { enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false); }

    // Colaboradores simulados: direccion del EXE, simbolo de la DLL (0 = sin simbolo propio) y stub. Los cuerpos se calculan ANTES de crear ningun EntryPatch.
    sites.clear(); RegisterHudSites(); RegisterCamSites(); RegisterPickupSites();
    bool known = true; json siteInfo = json::array();
    for (auto& st : sites) st.body = st.dll ? Body(st.dll) : 0;
    for (auto& st : sites) {
        const bool ok = InExe(st.exeAddr) && (st.dll == 0 || (!InExe(st.dll) && st.body != 0 && !InExe(st.body)));
        known &= ok; siteInfo.push_back({ {"name", st.name}, {"exe", st.exeAddr}, {"dll", st.dll}, {"body", st.body}, {"ok", ok} });
    }
    // bytes iniciales de todas las entradas parcheables y de las funciones probadas, para comprobar la restauracion
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (auto& st : sites) for (auto a : { st.exeAddr, st.dll, st.body }) if (a) entryBefore.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8) });
    for (auto& fn : fns) entryBefore.push_back({ fn.address, ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address), 8) });

    // objetos falsos y tablas
    objVt.fill(nullptr); objVt[0] = std::bit_cast<void*>(&SObjDtor);
    ScanIconWeapons();

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk = true; json lay = json::object();
    auto Chk = [&](const char* n, uint32 got, uint32 want) { lay[n] = got; if (got != want) { layoutOk = false; lay[std::string(n) + "_expected"] = want; } };
    {
        static RwGlobals g{}; Chk("RwGlobals.dOpenDevice.fpRenderStateSet (0x58FB22 call [ecx+20h])", uint32(reinterpret_cast<uintptr_t>(&g.dOpenDevice.fpRenderStateSet) - reinterpret_cast<uintptr_t>(&g)), 0x20);
        Chk("CCam.m_bCollisionChecksOn (0x511BE5)", uint32(offsetof(CCam, m_bCollisionChecksOn)), 3);
        Chk("CCam.m_bResetStatics (0x511B6C)", uint32(offsetof(CCam, m_bResetStatics)), 0x0A);
        Chk("CCam.m_fInitialPlayerOrientation (0x511B96)", uint32(offsetof(CCam, m_fInitialPlayerOrientation)), 0xA8);
        Chk("CCam.m_fVerticalAngle (0x511BA4)", uint32(offsetof(CCam, m_fVerticalAngle)), 0xAC);
        Chk("CCam.m_fFOV (0x511B71)", uint32(offsetof(CCam, m_fFOV)), 0xB4);
        Chk("CCam.m_fHorizontalAngle (0x511BB4)", uint32(offsetof(CCam, m_fHorizontalAngle)), 0xBC);
        Chk("CCam.m_vecFront (0x511E37)", uint32(offsetof(CCam, m_vecFront)), 0x190);
        Chk("CCam.m_vecSource (0x511C3F)", uint32(offsetof(CCam, m_vecSource)), 0x19C);
        Chk("CCam.m_vecUp (0x518391)", uint32(offsetof(CCam, m_vecUp)), 0x1B4);
        Chk("CCam.m_pCamTargetEntity (0x511B58)", uint32(offsetof(CCam, m_pCamTargetEntity)), 0x21C);
        Chk("CPlaceable.m_placement (0x5180B6 lea ecx,[edi+4])", uint32(offsetof(CPlaceable, m_placement)), 4);
        Chk("CPlaceable.m_matrix (0x5180A6 cmp [edi+14h])", uint32(offsetof(CPlaceable, m_matrix)), 0x14);
        alignas(16) static uint8 ebuf[0x80]{}; At<uint32>(ebuf, 0x18) = 0xABCD1234u;
        Chk("CEntity.m_pRwObject (0x511BE9 mov eax,[edx+18h])", uint32(reinterpret_cast<uintptr_t>(reinterpret_cast<CEntity*>(ebuf)->GetRwObject())), 0xABCD1234u);
        Chk("CEntity.m_nModelIndex (0x5180EB movsx [ecx+22h])", uint32(offsetof(CEntity, m_nModelIndex)), 0x22);
        Chk("CPed.m_fCurrentRotation (0x511B9E)", uint32(offsetof(CPed, m_fCurrentRotation)), 0x558);
        Chk("CPed.m_fAimingRotation (0x511EB6)", uint32(offsetof(CPed, m_fAimingRotation)), 0x55C);
        Chk("CPed.m_pTargetedObject (0x511B86)", uint32(offsetof(CPed, m_pTargetedObject)), 0x71C);
        Chk("CVehicle.m_pDriver (0x5181C7)", uint32(offsetof(CVehicle, m_pDriver)), 0x460);
        Chk("CVehicle.m_nVehicleType (0x517F9A)", uint32(offsetof(CVehicle, m_nVehicleType)), 0x590);
        Chk("CVehicle.m_nVehicleSubType (0x58A3B9)", uint32(offsetof(CVehicle, m_nVehicleSubType)), 0x594);
        Chk("CBike.m_mLeanMatrix (0x5180DF lea edx,[ecx+5CCh])", uint32(offsetof(CBike, m_mLeanMatrix)), 0x5CC);
        Chk("&FrontEndMenuManager.m_bHudOn (0x58FBCD)", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_bHudOn)), 0xBA6769);
        Chk("&FrontEndMenuManager.m_nRadarMode (0x58A34A)", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_nRadarMode)), 0xBA676C);
        Chk("&FrontEndMenuManager.m_bActivateMenuNextFrame (0x58FAFA)", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_bActivateMenuNextFrame)), 0xBA677B);
        Chk("&FrontEndMenuManager.m_bMenuActive (0x58D527)", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_bMenuActive)), 0xBA67A4);
        Chk("&TheCamera.m_bWideScreenOn (0x58FBB3)", uint32(reinterpret_cast<uintptr_t>(&TheCamera.m_bWideScreenOn)), 0xB6F065);
        Chk("&CHud::m_ItemToFlash (0x58A357)", uint32(reinterpret_cast<uintptr_t>(&CHud::m_ItemToFlash)), 0xBAB1DC);
        Chk("sizeof(CWeaponEffects) (gCrossHair)", uint32(sizeof(CWeaponEffects)), 0x2C);
        Chk("CWeaponEffects.m_nTimeWhenToDeactivate (0x512099 [0C8A83Ch])", uint32(offsetof(CWeaponEffects, m_nTimeWhenToDeactivate)), 4);
        Chk("CWeaponEffects.m_color (0x5120A9 [0C8A84Ch])", uint32(offsetof(CWeaponEffects, m_color)), 0x14);
        Chk("CWeaponEffects.m_fRotation (0x5120BF [0C8A85Ch])", uint32(offsetof(CWeaponEffects, m_fRotation)), 0x24);
        Chk("&gCrossHair (0x5120A9)", uint32(reinterpret_cast<uintptr_t>(gCrossHair.data())), 0xC8A838);
        Chk("sizeof(TxdDef) (0x58D82E lea ecx,[edx+ecx*4] con ecx=3*idx)", uint32(sizeof(TxdDef)), 12);
        Chk("TxdDef.m_pRwDictionary (0x58D831 mov esi,[ecx])", uint32(offsetof(TxdDef, m_pRwDictionary)), 0);
        Chk("CBaseModelInfo.m_nKey (0x58D83B mov eax,[eax+4])", uint32(offsetof(CBaseModelInfo, m_nKey)), 4);
        Chk("CBaseModelInfo.m_nTxdIndex (0x58D812 movsx [eax+0Ah])", uint32(offsetof(CBaseModelInfo, m_nTxdIndex)), 0x0A);
        alignas(16) static uint8 vmi[0x100]{}; At<uint32>(vmi, 0x5C) = 0x12345678u;
        Chk("CVehicleModelInfo.m_pVehicleStruct (0x5180FA mov eax,[eax+5Ch])", uint32(reinterpret_cast<uintptr_t>(reinterpret_cast<CVehicleModelInfo*>(vmi)->GetVehicleStruct())), 0x12345678u);
        Chk("&CPad::NewMouseControllerState.m_AmountMoved (0x511C6E mov eax,[0B73424h])", uint32(reinterpret_cast<uintptr_t>(&CPad::NewMouseControllerState.m_AmountMoved)), 0xB73424);
        Chk("&CHud::Sprites (0x5BA87E mov edi,0BAB1FCh)", uint32(reinterpret_cast<uintptr_t>(CHud::Sprites.data())), 0xBAB1FC);
        Chk("sizeof(CSprite2d) (0x5BA8A1 add edi,4)", uint32(sizeof(CSprite2d)), 4);
        Chk("&CWorld::Players (0x458F1A mov eax,[0B7CD98h])", uint32(reinterpret_cast<uintptr_t>(&CWorld::Players[0])), 0xB7CD98);
        Chk("&CPickups::aPickUps (0x458E2A add esi,9788C4h)", uint32(reinterpret_cast<uintptr_t>(CPickups::aPickUps.data())), 0x9788C0);
        Chk("sizeof(CPickup) (0x458E27 shl esi,5)", uint32(sizeof(CPickup)), 0x20);
        Write({ {"event", "layout"}, {"ok", layoutOk}, {"fields", lay} });
    }

    SaveGlobals();
    const unsigned savedFP = _controlfp(0, 0);
    Write({ {"event", "begin"}, {"sites", siteInfo}, {"known", known}, {"fp_control", savedFP}, {"icon_weapons", iconWeapons.size()} });

    bool passed = known && layoutOk; int pair = 0;
    if (known) {
        for (size_t f = 0; f < fns.size(); ++f) {
            const auto& fn = fns[f]; int failures = 0; const std::string full = std::string(fn.cls) + "::" + fn.name;
            if (!only.empty() && only != full) continue;
            auto patches = PatchesFor(fn.cls, fn.name);   // colaboradores simulados solo mientras dura la funcion
            for (int i = 0; i < fn.cases; ++i) {
                json original = caseFns[f](unsigned(i), false, hooks[f], fn.address); RestoreGlobals();
                json candidate = caseFns[f](unsigned(i), true, hooks[f], fn.address); RestoreGlobals();
                const bool ok = original == candidate && original["switches"] == true && candidate["switches"] == true;
                const std::string od = original.dump(), cd = candidate.dump();
                passed &= ok; failures += !ok;
                if (ok) Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", true}, {"accepted", true},
                                {"original", {{"trace", original["trace"]}, {"state_fnv", Fnv(od.data(), od.size())}}},
                                {"candidate", {{"trace", candidate["trace"]}, {"state_fnv", Fnv(cd.data(), cd.size())}}},
                                {"events", original["trace"].size()} });
                else { json diff = json::array(); for (auto& [k, v] : original.items()) if (candidate[k] != v) diff.push_back(k);
                    Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", false}, {"accepted", false}, {"differs", diff},
                            {"original", failures <= 2 ? original : json(nullptr)}, {"candidate", failures <= 2 ? candidate : json(nullptr)} }); }
            }
            patches.reset();   // restaura las entradas antes de seguir
            Write({ {"event", "function_summary"}, {"function", full}, {"failures", failures}, {"cases", fn.cases} });
        }
    }
    for (size_t f = 0; f < fns.size(); ++f) { hooks[f]->State(enabled[f]); hooks[f]->LockState(locked[f]); }
    RestoreGlobals();
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = _controlfp(0, 0) == savedFP;
    for (size_t j = 0; j < globals.size(); ++j) restored &= savedGlobals[j] == ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].address), globals[j].size);
    for (auto& [a, b] : entryBefore) restored &= b == ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8);
    for (size_t f = 0; f < fns.size(); ++f) restored &= hooks[f]->Hooked() == enabled[f] && hooks[f]->Locked() == locked[f];
    Write({ {"event", passed ? "complete" : "error"}, {"pairs", pair}, {"layout_ok", layoutOk}, {"state_restored", restored} });
}
}
