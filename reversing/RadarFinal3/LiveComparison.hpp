#pragma once
// Banco dentro del motor (flujo simple, estilo Small2/PadMouse1/HudCamFinal2): RadarFinal3 = las 63 funciones de CRadar.
// Original (hook desactivado = codigo del EXE) frente a candidato (hook activo) con el mismo fixture determinista por caso. Se ejecuta dentro del juego
// completo (partida cargada) y se engancha en CGame::Process con probe_build.py. Colaboradores simulados con EntryPatch (direccion del EXE + simbolo de la
// DLL + su cuerpo), activados solo mientras se prueba la funcion que los usa; los estados RW pasan por una tabla de motor falsa (puntero 0xC97B24).
// Ver README.md para los dominios, el enmascarado y lo que queda fuera.
#include <map>
#include "WorldCases.hpp"

namespace RadarFinal3Live {

inline const std::array<Fn, 63> fns{{
    {"CRadar","AddBlipToLegendList",0x5859F0,0,-1,256},
    {"CRadar","CalculateBlipAlpha",0x583420,0,-1,256},
    {"CRadar","CalculateCachedSinCos",0x583670,0,-1,512},
    {"CRadar","ChangeBlipBrightness",0x583C70,0,-1,128},
    {"CRadar","ChangeBlipColour",0x583AB0,0,-1,128},
    {"CRadar","ChangeBlipDisplay",0x583D20,0,-1,128},
    {"CRadar","ChangeBlipScale",0x583CC0,0,-1,128},
    {"CRadar","ClearActualBlip-OG",0x587C10,0,-1,128},
    {"CRadar","ClearBlip",0x587CE0,0,-1,128},
    {"CRadar","ClearBlipForEntity-OG",0x587C60,0,-1,128},
    {"CRadar","ClipRadarTileCoords",0x584B00,0,-1,128},
    {"CRadar","DisplayThisBlip",0x583B40,0,-1,512},
    {"CRadar","Draw3dMarkers",0x585BF0,0,-1,512},
    {"CRadar","DrawAreaOnRadar",0x5853D0,0,-1,512},
    {"CRadar","DrawBlips",0x588050,0,-1,512},
    {"CRadar","DrawCoordBlip",0x586D60,0,-1,1024},
    {"CRadar","DrawLegend",0x5828A0,0,-1,512},
    {"CRadar","DrawMap",0x586B00,0,-1,512},
    {"CRadar","DrawRadarGangOverlay",0x586650,0,-1,256},
    {"CRadar","DrawRadarMap",0x586880,0,-1,256},
    {"CRadar","DrawRadarMask",0x585700,0,-1,128},
    {"CRadar","DrawRadarSection",0x586110,0,-1,512},
    {"CRadar","DrawRadarSectionMap",0x586520,0,-1,256},
    {"CRadar","DrawRadarSprite",0x585FF0,0,-1,256},
    {"CRadar","DrawRotatingRadarSprite",0x584850,0,-1,256},
    {"CRadar","DrawYouAreHereSprite",0x584960,0,-1,256},
    {"CRadar","GetActualBlipArrayIndex",0x582870,0,-1,128},
    {"CRadar","GetNewUniqueBlipIndex",0x582820,0,-1,128},
    {"CRadar","GetRadarTraceColour",0x584770,0,-1,128},
    {"CRadar","GetTextureCorners",0x584D90,0,-1,128},
    {"CRadar","InitFrontEndMap",0x585960,0,-1,128},
    {"CRadar","Initialise",0x587FB0,0,-1,64},
    {"CRadar","IsPointInsideRadar",0x584D40,0,-1,256},
    {"CRadar","LimitRadarPoint",0x5832F0,0,-1,256},
    {"CRadar","LimitToMap",0x583350,0,-1,256},
    {"CRadar","LineRadarBoxCollision",0x584E00,0,-1,512},
    {"CRadar","Load",0x5D53C0,0,-1,64},
    {"CRadar","LoadTextures",0x5827D0,0,-1,64},
    {"CRadar","RemoveMapSection",0x584BB0,0,-1,128},
    {"CRadar","RemoveRadarSections",0x584BF0,0,-1,64},
    {"CRadar","RequestMapSection",0x584B50,0,-1,128},
    {"CRadar","Save",0x5D5860,0,-1,64},
    {"CRadar","SetBlipAlwaysDisplayInZoom",0x583DB0,0,-1,128},
    {"CRadar","SetBlipEntryExit",0x583F00,0,-1,128},
    {"CRadar","SetBlipFade",0x583E00,0,-1,128},
    {"CRadar","SetBlipFriendly",0x583EB0,0,-1,128},
    {"CRadar","SetBlipSprite",0x583D70,0,-1,128},
    {"CRadar","SetCoordBlip",0x583820,0,-1,256},
    {"CRadar","SetCoordBlipAppearance",0x583E50,0,-1,128},
    {"CRadar","SetEntityBlip",0x5839A0,0,-1,256},
    {"CRadar","SetMapCentreToPlayerCoords",0x585B20,0,-1,128},
    {"CRadar","SetShortRangeCoordBlip",0x583920,0,-1,256},
    {"CRadar","SetupAirstripBlips",0x587D20,0,-1,512},
    {"CRadar","SetupRadarRect",0x584A80,0,-1,64},
    {"CRadar","ShowRadarMarker",0x584480,0,-1,128},
    {"CRadar","ShowRadarTrace",0x583F40,0,-1,256},
    {"CRadar","ShowRadarTraceWithHeight",0x584070,0,-1,512},
    {"CRadar","Shutdown",0x585940,0,-1,64},
    {"CRadar","StreamRadarSections-vec",0x5858D0,0,-1,256},
    {"CRadar","StreamRadarSections-xy",0x584C50,0,-1,128},
    {"CRadar","TransformRadarPointToRealWorldSpace",0x5835A0,0,-1,128},
    {"CRadar","TransformRadarPointToScreenSpace",0x583480,0,-1,256},
    {"CRadar","TransformRealWorldPointToRadarSpace",0x583530,0,-1,256},
}};
inline const std::array<CaseFn, 63> caseFns{{
    CaseAddBlipToLegendList, CaseCalculateBlipAlpha, CaseCalculateCachedSinCos, CaseChangeBlipBrightness, CaseChangeBlipColour, CaseChangeBlipDisplay, CaseChangeBlipScale,
    CaseClearActual, CaseClearBlip, CaseClearBlipForEntity, CaseClipRadarTileCoords, CaseDisplayThisBlip, CaseDraw3dMarkers, CaseDrawAreaOnRadar, CaseDrawBlips, CaseDrawCoordBlip,
    CaseDrawLegend, CaseDrawMap, CaseDrawRadarGangOverlay, CaseDrawRadarMap, CaseDrawRadarMask, CaseDrawRadarSection, CaseDrawRadarSectionMap, CaseDrawRadarSprite,
    CaseDrawRotatingRadarSprite, CaseDrawYouAreHere, CaseGetActual, CaseGetNewUnique, CaseGetRadarTraceColour, CaseGetTextureCorners, CaseInitFrontEndMap, CaseInitialise,
    CaseIsPointInsideRadar, CaseLimitRadarPoint, CaseLimitToMap, CaseLineRadarBox, CaseLoad, CaseLoadTextures, CaseRemoveMapSection, CaseRemoveRadarSections, CaseRequestMapSection,
    CaseSave, CaseSetBlipAlways, CaseSetBlipEntryExit, CaseSetBlipFade, CaseSetBlipFriendly, CaseSetBlipSprite, CaseSetCoordBlip, CaseSetCoordBlipAppearance, CaseSetEntityBlip,
    CaseSetMapCentre, CaseSetShortRangeCoordBlip, CaseSetupAirstripBlips, CaseSetupRadarRect, CaseShowRadarMarker, CaseShowRadarTrace, CaseShowRadarTraceWithHeight, CaseShutdown,
    CaseStreamVec, CaseStreamXY, CaseTransformToWorld, CaseTransformToScreen, CaseTransformToRadar,
}};

// ---------------- registro de colaboradores ----------------
inline void RegisterSites() {
    using std::bit_cast;
    AddSite("FindPlayerPed", 0x56E210, bit_cast<uintptr_t>(&FindPlayerPed), (&SFindPed));
    AddSite("FindPlayerVehicle", 0x56E0D0, bit_cast<uintptr_t>(&FindPlayerVehicle), (&SFindVeh));
    AddSite("FindPlayerCentre", 0x56E400, bit_cast<uintptr_t>(&FindPlayerCentreOfWorldForMap), (&SCentre));
    AddSite("FindPlayerHeading", 0x56E450, bit_cast<uintptr_t>(&FindPlayerHeading), (&SHeading));
    AddSite("FindPlayerSpeed", 0x56E090, bit_cast<uintptr_t>(&FindPlayerSpeed), (&SSpeed));
    AddSite("CGameLogic::IsCoopGameGoingOn", 0x441390, bit_cast<uintptr_t>(&CGameLogic::IsCoopGameGoingOn), (&SCoop));
    AddSite("CTheScripts::IsPlayerOnAMission", 0x464D50, bit_cast<uintptr_t>(&CTheScripts::IsPlayerOnAMission), (&SMission));
    AddSite("CPlayerInfo::IsPlayerInRemoteMode", 0x56DAB0, bit_cast<uintptr_t>(&CPlayerInfo::IsPlayerInRemoteMode), (&SRemote));
    AddSite("CPlayerPed::IsHidden", 0x609620, bit_cast<uintptr_t>(&CPlayerPed::IsHidden), (&SIsHidden));
    AddSite("CSprite2d::Draw(rect)", 0x728350, bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(const CRect&, const CRGBA&)>(&CSprite2d::Draw)), (&SSpriteRect));
    AddSite("CSprite2d::Draw(9)", 0x728520, bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(float, float, float, float, float, float, float, float, const CRGBA&)>(&CSprite2d::Draw)), (&SSprite9));
    AddSite("CSprite2d::DrawRect", 0x727B60, bit_cast<uintptr_t>(static_cast<void(*)(const CRect&, const CRGBA&)>(&CSprite2d::DrawRect)), (&SDrawRect));
    AddSite("CSprite2d::Draw2DPolygon", 0x7285B0, bit_cast<uintptr_t>(&CSprite2d::Draw2DPolygon), (&SPoly));
    AddSite("CSprite2d::SetVertices(n)", 0x727890, bit_cast<uintptr_t>(static_cast<void(*)(int32, const CVector2D*, const CVector2D*, const CRGBA&)>(&CSprite2d::SetVertices)), (&SSetVerts));
    AddSite("CSprite2d::SetVertices(rect)", 0x727420, bit_cast<uintptr_t>(static_cast<void(*)(const CRect&, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&)>(&CSprite2d::SetVertices)), (&SSetVertsRect));
    AddSite("CSprite2d::SetMaskVertices", 0x7279B0, bit_cast<uintptr_t>(&CSprite2d::SetMaskVertices), (&SSetMask));
    AddSite("CFont::PrintString", 0x71A700, bit_cast<uintptr_t>(&CFont::PrintString), (&SPrint));
    AddSite("CText::Get", 0x6A0050, bit_cast<uintptr_t>(&CText::Get), (&STextGet));
    AddSite("CStreaming::RequestModel", 0x4087E0, bit_cast<uintptr_t>(&CStreaming::RequestModel), (&SReqModel));
    AddSite("CStreaming::RemoveModel", 0x4089A0, bit_cast<uintptr_t>(&CStreaming::RemoveModel), (&SRemModel));
    AddSite("CTxdStore::PushCurrentTxd", 0x7316A0, bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd), (&SPushTxd));
    AddSite("CTxdStore::SetCurrentTxd", 0x7319C0, bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd), (&SSetTxd));
    AddSite("CTxdStore::PopCurrentTxd", 0x7316B0, bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd), (&SPopTxd));
    AddSite("CTxdStore::FindTxdSlot", 0x731850, bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)), (&SFindSlot));
    AddSite("CSprite2d::SetTexture(1)", 0x727270, bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(const char*)>(&CSprite2d::SetTexture)), (&SSetTex1));
    AddSite("CSprite2d::Delete", 0x727240, bit_cast<uintptr_t>(&CSprite2d::Delete), (&SDelete));
    AddSite("GetFirstTexture", 0x734940, bit_cast<uintptr_t>(&GetFirstTexture), (&SGetFirstTex));
    AddSite("C3dMarkers::PlaceMarkerCone", 0x726D40, bit_cast<uintptr_t>(&C3dMarkers::PlaceMarkerCone), (&SCone));
    AddSite("C3dMarkers::PlaceMarkerSet", 0x725BA0, bit_cast<uintptr_t>(&C3dMarkers::PlaceMarkerSet), (&SMarkerSet));
    if constexpr (std::is_same_v<decltype(&CTheScripts::ScriptDebugLine3D), void(*)(const CVector&, const CVector&, uint32, uint32)>)
        AddSite("CTheScripts::ScriptDebugLine3D", 0x485DE0, bit_cast<uintptr_t>(&CTheScripts::ScriptDebugLine3D), (&SDebugLineExe), (&SDebugLineRef));
    else
        AddSite("CTheScripts::ScriptDebugLine3D", 0x485DE0, bit_cast<uintptr_t>(&CTheScripts::ScriptDebugLine3D), (&SDebugLineExe));
    AddSite("CEntryExit::GetPositionRelativeToOutsideWorld", 0x43EA00, bit_cast<uintptr_t>(&CEntryExit::GetPositionRelativeToOutsideWorld), (&SEnexPos));
    AddSite("CTheZones::GetCurrentZoneLockedOrUnlocked", 0x572130, bit_cast<uintptr_t>(&CTheZones::GetCurrentZoneLockedOrUnlocked), (&SZoneLock));
    AddSite("CGangWars::CanPlayerStartAGangWarHere", 0x443F80, bit_cast<uintptr_t>(&CGangWars::CanPlayerStartAGangWarHere), (&SGangCan));
    AddSite("CGenericGameStorage::SaveDataToWorkBuffer", 0x5D1270, bit_cast<uintptr_t>(static_cast<bool(*)(void*, int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)), (&SSaveWB));
    AddSite("CGenericGameStorage::LoadDataFromWorkBuffer", 0x5D1300, bit_cast<uintptr_t>(static_cast<bool(*)(void*, int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)), (&SLoadWB));
    AddSite("CPlaceable::AllocateMatrix", 0x54F560, bit_cast<uintptr_t>(&CPlaceable::AllocateMatrix), (&SAllocMatrix));
    AddSite("CSimpleTransform::UpdateMatrix", 0x54F1B0, bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix), (&SUpdateMatrix));
    AddSite("CVector::Normalise", 0x59C910, bit_cast<uintptr_t>(&CVector::Normalise), (&SNormalise));
    // colaboradores que son a la vez funciones de CRadar (solo mientras se prueba otra funcion)
    AddSite("CRadar::SetupAirstripBlips", 0x587D20, bit_cast<uintptr_t>(&CRadar::SetupAirstripBlips), (&SSetupAirstrip));
    AddSite("CRadar::DrawEntityBlip", 0x587000, 0, (&SDrawEntity));
    AddSite("CRadar::DrawCoordBlip", 0x586D60, bit_cast<uintptr_t>(&CRadar::DrawCoordBlip), (&SDrawCoord));
    AddSite("CRadar::DrawRadarSprite", 0x585FF0, bit_cast<uintptr_t>(&CRadar::DrawRadarSprite), (&SDrawSprite));
    AddSite("CRadar::DrawRotatingRadarSprite", 0x584850, bit_cast<uintptr_t>(&CRadar::DrawRotatingRadarSprite), (&SDrawRot));
    AddSite("CRadar::DrawYouAreHereSprite", 0x584960, bit_cast<uintptr_t>(&CRadar::DrawYouAreHereSprite), (&SYouAre));
    AddSite("CRadar::AddBlipToLegendList", 0x5859F0, bit_cast<uintptr_t>(&CRadar::AddBlipToLegendList), (&SLegendAdd));
    AddSite("CRadar::ShowRadarTraceWithHeight", 0x584070, bit_cast<uintptr_t>(&CRadar::ShowRadarTraceWithHeight), (&STraceHeight));
    AddSite("CRadar::ShowRadarMarker", 0x584480, bit_cast<uintptr_t>(&CRadar::ShowRadarMarker), (&SMarkerDbg));
    AddSite("CRadar::DrawAreaOnRadar", 0x5853D0, bit_cast<uintptr_t>(&CRadar::DrawAreaOnRadar), (&SAreaOnRadar));
    AddSite("CRadar::DrawRadarSection", 0x586110, bit_cast<uintptr_t>(&CRadar::DrawRadarSection), (&SSection));
    AddSite("CRadar::DrawRadarGangOverlay", 0x586650, bit_cast<uintptr_t>(&CRadar::DrawRadarGangOverlay), (&SGangOverlay));
    AddSite("CRadar::DrawRadarMask", 0x585700, bit_cast<uintptr_t>(&CRadar::DrawRadarMask), (&SMask));
    AddSite("CRadar::StreamRadarSections(xy)", 0x584C50, bit_cast<uintptr_t>(static_cast<void(*)(int32, int32)>(&CRadar::StreamRadarSections)), (&SStreamXY));
    AddSite("CRadar::CalculateCachedSinCos", 0x583670, bit_cast<uintptr_t>(&CRadar::CalculateCachedSinCos), (&SCalcSinCos));
    AddSite("CRadar::DrawRadarMap", 0x586880, bit_cast<uintptr_t>(&CRadar::DrawRadarMap), (&SDrawRadarMapDbg));
}

// Colaboradores de cada funcion (nunca incluyen la funcion probada)
inline std::unique_ptr<Patches> PatchesFor(const std::string& name) {
    static const std::map<std::string, std::vector<const char*>> m = {
        {"Initialise", {"CTxdStore::FindTxdSlot"}},
        {"Shutdown", {"CSprite2d::Delete", "CStreaming::RemoveModel"}},
        {"LoadTextures", {"CTxdStore::PushCurrentTxd", "CTxdStore::SetCurrentTxd", "CTxdStore::PopCurrentTxd", "CTxdStore::FindTxdSlot", "CSprite2d::SetTexture(1)"}},
        {"DrawLegend", {"CFont::PrintString", "CText::Get", "CSprite2d::Draw(rect)", "CSprite2d::Draw2DPolygon", "CSprite2d::DrawRect"}},
        {"CalculateCachedSinCos", {"CPlaceable::AllocateMatrix", "CSimpleTransform::UpdateMatrix", "CVector::Normalise"}},
        {"InitFrontEndMap", {"CPlaceable::AllocateMatrix", "CSimpleTransform::UpdateMatrix", "CVector::Normalise"}},
        {"DisplayThisBlip", {"FindPlayerPed"}},
        {"ShowRadarTrace", {"CSprite2d::DrawRect"}},
        {"ShowRadarTraceWithHeight", {"CSprite2d::DrawRect", "CSprite2d::Draw2DPolygon"}},
        {"ShowRadarMarker", {"CTheScripts::ScriptDebugLine3D"}},
        {"DrawRotatingRadarSprite", {"CSprite2d::Draw(9)"}},
        {"DrawYouAreHereSprite", {"FindPlayerHeading", "CRadar::DrawRotatingRadarSprite"}},
        {"RequestMapSection", {"CStreaming::RequestModel", "CStreaming::RemoveModel"}},
        {"RemoveMapSection", {"CStreaming::RequestModel", "CStreaming::RemoveModel"}},
        {"RemoveRadarSections", {"CStreaming::RequestModel", "CStreaming::RemoveModel"}},
        {"StreamRadarSections-xy", {"CStreaming::RequestModel", "CStreaming::RemoveModel"}},
        {"StreamRadarSections-vec", {"CStreaming::RequestModel", "CStreaming::RemoveModel"}},
        {"DrawAreaOnRadar", {"CSprite2d::SetVertices(n)"}},
        {"DrawRadarMask", {"CSprite2d::SetMaskVertices"}},
        {"SetMapCentreToPlayerCoords", {"FindPlayerPed", "FindPlayerCentre"}},
        {"Draw3dMarkers", {"FindPlayerPed", "CTheScripts::IsPlayerOnAMission", "C3dMarkers::PlaceMarkerCone", "C3dMarkers::PlaceMarkerSet"}},
        {"DrawRadarSprite", {"CSprite2d::Draw(rect)", "FindPlayerPed"}},
        {"DrawRadarSection", {"CSprite2d::SetVertices(n)", "GetFirstTexture"}},
        {"DrawRadarSectionMap", {"CSprite2d::SetVertices(rect)", "GetFirstTexture"}},
        {"DrawRadarGangOverlay", {"CRadar::DrawAreaOnRadar", "CGangWars::CanPlayerStartAGangWarHere"}},
        {"DrawRadarMap", {"CRadar::DrawRadarMask", "CRadar::StreamRadarSections(xy)", "CRadar::DrawRadarSection", "CRadar::DrawRadarGangOverlay", "CRadar::DrawAreaOnRadar", "FindPlayerVehicle", "FindPlayerCentre"}},
        {"DrawMap", {"CRadar::CalculateCachedSinCos", "CRadar::DrawRadarMap", "FindPlayerPed", "FindPlayerVehicle", "FindPlayerCentre", "FindPlayerSpeed", "CGameLogic::IsCoopGameGoingOn", "CPlayerInfo::IsPlayerInRemoteMode"}},
        {"DrawCoordBlip", {"CTheScripts::IsPlayerOnAMission", "CRadar::ShowRadarMarker", "CRadar::DrawRadarSprite", "CRadar::ShowRadarTraceWithHeight", "CRadar::AddBlipToLegendList", "CTheZones::GetCurrentZoneLockedOrUnlocked",
            "CEntryExit::GetPositionRelativeToOutsideWorld", "FindPlayerCentre"}},
        {"SetupAirstripBlips", {"FindPlayerVehicle"}},
        {"DrawBlips", {"CRadar::SetupAirstripBlips", "CRadar::DrawEntityBlip", "CRadar::DrawCoordBlip", "CRadar::DrawRadarSprite", "CRadar::DrawRotatingRadarSprite", "CRadar::DrawYouAreHereSprite", "CSprite2d::DrawRect",
            "FindPlayerPed", "FindPlayerVehicle", "FindPlayerCentre", "FindPlayerHeading", "CPlayerPed::IsHidden"}},
        {"Load", {"CTxdStore::FindTxdSlot", "CGenericGameStorage::LoadDataFromWorkBuffer"}},
        {"Save", {"CGenericGameStorage::SaveDataToWorkBuffer"}},
    };
    // nombres de las funciones con sufijo (ClearActualBlip-OG...) no necesitan colaboradores
    const auto it = m.find(name);
    if (it == m.end()) return std::make_unique<Patches>();
    std::vector<const char*> v = it->second;
    auto p = std::make_unique<Patches>();
    for (auto* n : v) for (auto& s : sites) if (std::strcmp(s.name, n) == 0) {
        p->Add(s.exeAddr, s.stub);
        if (s.dll) { p->Add(s.dll, s.dllStub); if (s.body != s.dll) p->Add(s.body, s.dllStub); }
    }
    return p;
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "radarfinal3-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "radarfinal3-runtime.jsonl"); Write({ {"event", "waiting_for_player"} }); ReversingProbe::InstallFailureCapture("radarfinal3"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    realPed = player; realVeh = FindPlayerVehicle();
    std::string only; { std::ifstream f(dir / "radarfinal3-only.txt"); if (f) std::getline(f, only); }
    std::vector<Hook*> hooks(fns.size(), nullptr);
    for (size_t f = 0; f < fns.size(); ++f) if (!(hooks[f] = FindHook(fns[f].cls, fns[f].name))) { Write({ {"event", "error"}, {"reason", "hook not found"}, {"function", std::string(fns[f].cls) + "::" + fns[f].name} }); return; }
    std::vector<bool> enabled(fns.size()), locked(fns.size());
    for (size_t f = 0; f < fns.size(); ++f) { enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false); }

    // Colaboradores simulados. Los cuerpos se calculan ANTES de crear ningun EntryPatch.
    sites.clear(); RegisterSites();
    bool known = true; json siteInfo = json::array();
    for (auto& st : sites) st.body = st.dll ? Body(st.dll) : 0;
    for (auto& st : sites) {
        const bool ok = InExe(st.exeAddr) && (st.dll == 0 || (!InExe(st.dll) && st.body != 0 && !InExe(st.body)));
        known &= ok; siteInfo.push_back({ {"name", st.name}, {"exe", st.exeAddr}, {"dll", st.dll}, {"body", st.body}, {"ok", ok} });
    }
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (auto& st : sites) for (auto a : { st.exeAddr, st.dll, st.body }) if (a) entryBefore.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8) });
    for (auto& fn : fns) entryBefore.push_back({ fn.address, ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address), 8) });

    InitFakeTexts(); InitPools();

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk = true; json lay = json::object();
    auto Chk = [&](const char* n, uint32 got, uint32 want) { lay[n] = got; if (got != want) { layoutOk = false; lay[std::string(n) + "_expected"] = want; } };
    {
        static RwGlobals g{}; Chk("RwGlobals.dOpenDevice.zBufferFar (0x5857F2 [ecx+1Ch])", uint32(reinterpret_cast<uintptr_t>(&g.dOpenDevice.zBufferFar) - reinterpret_cast<uintptr_t>(&g)), 0x1C);
        Chk("RwGlobals.dOpenDevice.fpRenderStateSet (call [ecx+20h])", uint32(reinterpret_cast<uintptr_t>(&g.dOpenDevice.fpRenderStateSet) - reinterpret_cast<uintptr_t>(&g)), 0x20);
        Chk("RwGlobals.dOpenDevice.fpIm2DRenderPrimitive (734E90 jmp [eax+30h])", uint32(reinterpret_cast<uintptr_t>(&g.dOpenDevice.fpIm2DRenderPrimitive) - reinterpret_cast<uintptr_t>(&g)), 0x30);
        Chk("sizeof(tRadarTrace)", uint32(sizeof(tRadarTrace)), 0x28);
        Chk("tRadarTrace.m_nColour", uint32(offsetof(tRadarTrace, m_nColour)), 0); Chk("tRadarTrace.m_nEntityHandle", uint32(offsetof(tRadarTrace, m_nEntityHandle)), 4);
        Chk("tRadarTrace.m_vPosition", uint32(offsetof(tRadarTrace, m_vPosition)), 8); Chk("tRadarTrace.m_nCounter (0x582829)", uint32(offsetof(tRadarTrace, m_nCounter)), 0x14);
        Chk("tRadarTrace.m_fSphereRadius", uint32(offsetof(tRadarTrace, m_fSphereRadius)), 0x18); Chk("tRadarTrace.m_nBlipSize", uint32(offsetof(tRadarTrace, m_nBlipSize)), 0x1C);
        Chk("tRadarTrace.m_pEntryExit", uint32(offsetof(tRadarTrace, m_pEntryExit)), 0x20); Chk("tRadarTrace.m_nBlipSprite", uint32(offsetof(tRadarTrace, m_nBlipSprite)), 0x24);
        {   // bits de las dos bytes de banderas (0x25: brillante 1, seguimiento 2, corto alcance 4, amistoso 8, permanecer 0x10, fundido 0x20, aspecto de coordenada 0xC0; 0x26: pantalla 3, tipo 0x3C, aspecto 0xC0)
            tRadarTrace t{}; std::memset(&t, 0, sizeof(t));
            t.m_bBright = true; t.m_bTrackingBlip = true; Chk("flags1 bright|tracking", At<uint8>(reinterpret_cast<uint8*>(&t), 0x25), 3);
            std::memset(&t, 0, sizeof(t)); t.m_bShortRange = true; t.m_bFriendly = true; Chk("flags1 shortrange|friendly", At<uint8>(reinterpret_cast<uint8*>(&t), 0x25), 0xC);
            std::memset(&t, 0, sizeof(t)); t.m_bBlipRemain = true; t.m_bBlipFade = true; Chk("flags1 remain|fade", At<uint8>(reinterpret_cast<uint8*>(&t), 0x25), 0x30);
            std::memset(&t, 0, sizeof(t)); t.m_nCoordBlipAppearance = 2; Chk("flags1 coordAppearance=2", At<uint8>(reinterpret_cast<uint8*>(&t), 0x25), 0x80);
            std::memset(&t, 0, sizeof(t)); t.m_nBlipDisplayFlag = BLIP_DISPLAY_BOTH; t.m_nBlipType = BLIP_CONTACT_POINT; t.m_nAppearance = BLIP_FLAG_UNK; Chk("flags2 display=3,type=5,appearance=2", At<uint8>(reinterpret_cast<uint8*>(&t), 0x26), 3 | (5 << 2) | (2 << 6));
        }
        Chk("&CRadar::ms_RadarTrace", uint32(reinterpret_cast<uintptr_t>(CRadar::ms_RadarTrace.data())), 0xBA86F0);
        Chk("&CRadar::RadarBlipSprites", uint32(reinterpret_cast<uintptr_t>(CRadar::RadarBlipSprites.data())), 0xBAA250);
        Chk("sizeof(CSprite2d)", uint32(sizeof(CSprite2d)), 4);
        Chk("&CRadar::MapLegendList", uint32(reinterpret_cast<uintptr_t>(CRadar::MapLegendList.data())), 0xBA8318);
        Chk("&CRadar::ArrowBlipColour", uint32(reinterpret_cast<uintptr_t>(CRadar::ArrowBlipColour.data())), 0xBA86D4);
        Chk("&CRadar::m_radarRange", uint32(reinterpret_cast<uintptr_t>(&CRadar::m_radarRange)), 0xBA8314);
        Chk("&CRadar::cachedSin", uint32(reinterpret_cast<uintptr_t>(&CRadar::cachedSin)), 0xBA830C); Chk("&CRadar::cachedCos", uint32(reinterpret_cast<uintptr_t>(&CRadar::cachedCos)), 0xBA8308);
        Chk("&CRadar::vec2DRadarOrigin", uint32(reinterpret_cast<uintptr_t>(&CRadar::vec2DRadarOrigin)), 0xBAA248);
        Chk("&CRadar::m_radarRect", uint32(reinterpret_cast<uintptr_t>(&CRadar::m_radarRect)), 0x8D0920);
        Chk("&FrontEndMenuManager.m_bDrawingMap", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_bDrawingMap)), 0xBA67A1);
        Chk("&FrontEndMenuManager.m_fMapZoom", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_fMapZoom)), 0xBA67AC);
        Chk("&FrontEndMenuManager.m_vMapOrigin", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_vMapOrigin)), 0xBA67B0);
        Chk("&FrontEndMenuManager.m_vMousePos", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_vMousePos)), 0xBA67B8);
        Chk("&FrontEndMenuManager.m_bMapLoaded", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_bMapLoaded)), 0xBA67C0);
        Chk("&FrontEndMenuManager.m_ShowLocationsBlips", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_ShowLocationsBlips)), 0xBA678D);
        Chk("&FrontEndMenuManager.m_ShowMissionBlips", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_ShowMissionBlips)), 0xBA678F);
        Chk("&FrontEndMenuManager.m_abPrefsMapBlips[4]", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_abPrefsMapBlips[4])), 0xBA6791);
        Chk("&FrontEndMenuManager.m_nRadarMode", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_nRadarMode)), 0xBA676C);
        Chk("&FrontEndMenuManager.m_DisplayTheMouse", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_DisplayTheMouse)), 0xBA6800);
        Chk("&FrontEndMenuManager.m_nMousePosX", uint32(reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_nMousePosX)), 0xBA6804);
        Chk("&CTheScripts::RadarZoomValue", uint32(reinterpret_cast<uintptr_t>(&CTheScripts::RadarZoomValue)), 0xA444A3);
        Chk("&CTheScripts::DbgFlag", uint32(reinterpret_cast<uintptr_t>(&CTheScripts::DbgFlag)), 0x859CF8);
        Chk("&CTheZones::NavigationZoneArray", uint32(reinterpret_cast<uintptr_t>(CTheZones::NavigationZoneArray.data())), 0xBA3798);
        Chk("CZone.m_fX1 (0x586707 [edi])", uint32(offsetof(CZone, m_fX1)), 0x10); Chk("CZone.m_ZoneInfoIndex (0x5866D0 [edi+0Ch])", uint32(offsetof(CZone, m_ZoneInfoIndex)), 0x1C);
        Chk("sizeof(CZoneInfo)", uint32(sizeof(CZoneInfo)), 0x11); Chk("CZoneInfo.ZoneColor", uint32(offsetof(CZoneInfo, ZoneColor)), 0xB);
        Chk("&CPad::NewMouseControllerState (0x5881A2)", uint32(reinterpret_cast<uintptr_t>(&CPad::NewMouseControllerState)), 0xB73418);
        Chk("&TheCamera.m_nActiveCam (0x583688 [0B6F081h])", uint32(reinterpret_cast<uintptr_t>(&TheCamera.m_nActiveCam)), 0xB6F081);
        Chk("&TheCamera.m_matrix (0x5837E0 [0B6F03Ch])", uint32(reinterpret_cast<uintptr_t>(&TheCamera.m_matrix)), 0xB6F03C);
        Chk("&TheCamera.m_placement.m_fHeading (0x5837F5 [0B6F038h])", uint32(reinterpret_cast<uintptr_t>(&TheCamera.m_placement.m_fHeading)), 0xB6F038);
        Chk("&TheCamera.m_aCams[0].m_nMode (0x5836B4 cmp word [..0B6F1A8h],10h)", uint32(reinterpret_cast<uintptr_t>(&TheCamera.m_aCams[0].m_nMode)), 0xB6F1A8);
        Chk("&TheCamera.m_aCams[0].m_pCamTargetEntity (0x583704 [..0B6F3B8h])", uint32(reinterpret_cast<uintptr_t>(&TheCamera.m_aCams[0].m_pCamTargetEntity)), 0xB6F3B8);
        Chk("&TheCamera.m_aCams[0].m_vecSourceBeforeLookBehind (0x58371B [..0B6F344h])", uint32(reinterpret_cast<uintptr_t>(&TheCamera.m_aCams[0].m_vecSourceBeforeLookBehind)), 0xB6F344);
        Chk("sizeof(CCam)", uint32(sizeof(CCam)), 0x238);
        Chk("&TheCamera.m_mCameraMatrix (0x584483 [0B6F99Ch])", uint32(reinterpret_cast<uintptr_t>(&TheCamera.m_mCameraMatrix)), 0xB6F99C);
        Chk("&CWorld::Players (0x588528 mov edi,0B7CD98h)", uint32(reinterpret_cast<uintptr_t>(&CWorld::Players[0])), 0xB7CD98);
        Chk("sizeof(CPlayerInfo)", uint32(sizeof(CPlayerInfo)), 0x190);
        Chk("&CWorld::PlayerInFocus", uint32(reinterpret_cast<uintptr_t>(&CWorld::PlayerInFocus)), 0xB7CD74);
        Chk("&CPickups::aPickUps", uint32(reinterpret_cast<uintptr_t>(CPickups::aPickUps.data())), 0x9788C0);
        Chk("sizeof(CPickup)", uint32(sizeof(CPickup)), 0x20);
        Chk("CBaseModelInfo.m_pColModel (0x535330 [edx+14h])", uint32(offsetof(CBaseModelInfo, m_pColModel)), 0x14);
        Chk("CEntity.m_nModelIndex (0x585D3E cmp word [ebx+22h],229h)", uint32(offsetof(CEntity, m_nModelIndex)), 0x22);
        Chk("CVehicle.m_nVehicleSubType (0x586B5F [eax+594h])", uint32(offsetof(CVehicle, m_nVehicleSubType)), 0x594);
        Chk("CVehicle.m_vehicleSpecialColIndex (0x53530F [ecx+48Bh])", uint32(offsetof(CVehicle, m_vehicleSpecialColIndex)), 0x48B);
        Chk("CPed.m_pVehicle (0x585DD8 [eax+58Ch])", uint32(offsetof(CPed, m_pVehicle)), 0x58C);
        Chk("&MODEL_NEVADA", uint32(MODEL_NEVADA), 0x229); Chk("&MODEL_VORTEX", uint32(MODEL_VORTEX), 0x21B);
        // pools falsos: la zancada de la DLL (GetAt(1) - GetAt(0)) debe ser la del EXE; los punteros reales se guardan y se restauran
        {
            const uint32 oldPed = Mem<uint32>(0xB74490), oldVeh = Mem<uint32>(0xB74494), oldObj = Mem<uint32>(0xB7449C);
            statesVeh.p()[0] = 1; statesVeh.p()[1] = 2; statesPed.p()[0] = 1; statesPed.p()[1] = 2; statesObj.p()[0] = 1; statesObj.p()[1] = 2;
            Mem<uint32>(0xB74494) = P(poolVeh.p()); Mem<uint32>(0xB74490) = P(poolPed.p()); Mem<uint32>(0xB7449C) = P(poolObj.p());
            Chk("sizeof(CPool<CVehicle>::storage) (0x585CF2 imul 0A18h)", uint32(reinterpret_cast<uintptr_t>(GetVehiclePool()->GetAt(1)) - reinterpret_cast<uintptr_t>(GetVehiclePool()->GetAt(0))), 0xA18);
            Chk("sizeof(CPool<CPed>::storage) (0x585DC0 imul 7C4h)", uint32(reinterpret_cast<uintptr_t>(GetPedPool()->GetAt(1)) - reinterpret_cast<uintptr_t>(GetPedPool()->GetAt(0))), 0x7C4);
            Chk("sizeof(CPool<CObject>::storage) (0x585D06 imul 19Ch)", uint32(reinterpret_cast<uintptr_t>(GetObjectPool()->GetAt(1)) - reinterpret_cast<uintptr_t>(GetObjectPool()->GetAt(0))), 0x19C);
            Mem<uint32>(0xB74490) = oldPed; Mem<uint32>(0xB74494) = oldVeh; Mem<uint32>(0xB7449C) = oldObj;
        }
        Write({ {"event", "layout"}, {"ok", layoutOk}, {"fields", lay} });
    }
    SaveGlobals();
    const unsigned savedFP = _controlfp(0, 0);
    Write({ {"event", "begin"}, {"sites", siteInfo}, {"known", known}, {"fp_control", savedFP} });

    bool passed = known && layoutOk; int pair = 0;
    if (known) {
        for (size_t f = 0; f < fns.size(); ++f) {
            const auto& fn = fns[f]; int failures = 0; const std::string full = std::string(fn.cls) + "::" + fn.name;
            if (!only.empty() && only != full) continue;
            auto patches = PatchesFor(fn.name);   // colaboradores simulados solo mientras dura la funcion
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
