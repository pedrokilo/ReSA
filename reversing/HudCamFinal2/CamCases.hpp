#pragma once
// Casos de CCam::Process_1stPerson y CCam::Process_Rocket. Objetos falsos con raw offsets (CCam 0x238 bytes, entidades, peds, vehiculos con
// tabla de ModelInfo falsa en una ranura sin uso); colaboradores de huesos, colision, mundo, agua, matrices y apuntado doblados con traza.
#include "HudCases.hpp"

namespace HudCamFinal2Live {

// ---------------- dobles ----------------
inline uint32 fixWheel{};
// CCam::Process_WheelCam(const CVector&, float, float, float) (miembro, 4 argumentos: ret 0x10)
inline uint32 __fastcall SWheelCam(void*, void*, const uint32* target, uint32 orientation, uint32 speedVar, uint32 wanted) {
    if (!Mine()) return 0; Push(T_WHEELCAM, { target[0], target[1], target[2], orientation, speedVar, wanted }); return fixWheel;
}
inline Buf<0x48> allocMatrix; inline uint8* camVehicle{};
// CPlaceable::AllocateMatrix(): el doble enlaza la matriz preparada por el fixture (el original toma una de la lista global)
inline void __fastcall SAllocMatrix(void* self, void*) { if (!Mine()) return; Push(T_ALLOCMATRIX, { P(self) - P(camVehicle) }); At<uint32>(static_cast<uint8*>(self), 0x14) = P(allocMatrix.p()); }
// CSimpleTransform::UpdateMatrix(CMatrix*) (miembro, 1 argumento)
inline void __fastcall SUpdateMatrix(void* self, void*, void* out) { if (Mine()) Push(T_UPDATEMATRIX, { P(self) - P(camVehicle), uint32(out == allocMatrix.p()) }); }
inline void __fastcall SLean(void* self, void*) { if (Mine()) Push(T_LEAN, { P(self) - P(camVehicle) }); }
inline std::array<uint32, 3> fixBone{};
// CPed::GetBonePosition(CVector*, eBoneTag, bool) (miembro, 3 argumentos: ret 0xC)
inline void __fastcall SBone(void* self, void*, uint32* out, uint32 tag, uint32 flag) {
    if (!Mine()) return; Push(T_BONE, { P(self), tag, flag & 0xFF }); out[0] = fixBone[0]; out[1] = fixBone[1]; out[2] = fixBone[2];
}
inline uint32 fixLosBits{}, losCalls{};
// CWorld::GetIsLineOfSightClear(origin, target, 5 banderas + 2 valores por defecto = 7 booleanos): cdecl, 9 dwords
inline uint32 __cdecl SLos(const float* a, const float* b, uint32 f0, uint32 f1, uint32 f2, uint32 f3, uint32 f4, uint32 f5, uint32 f6) {
    if (!Mine()) return 1;
    const uint32 v = (fixLosBits >> (losCalls++ & 31u)) & 1u;
    Push(T_LOS, { Bits(a[0]), Bits(a[1]), Bits(a[2]), Bits(b[0]), Bits(b[1]), Bits(b[2]), f0 & 0xFF, f1 & 0xFF, f2 & 0xFF, f3 & 0xFF, f4 & 0xFF, f5 & 0xFF, f6 & 0xFF, v });
    return v;
}
inline bool fixWaterHit{}; inline float fixWaterLevel{};
inline uint32 __cdecl SWater(uint32 x, uint32 y, uint32 z, float* out, uint32 touching, const void* normals) {
    if (!Mine()) return 0; Push(T_WATER, { x, y, z, touching & 0xFF, P(normals) });
    if (fixWaterHit) *out = fixWaterLevel; return fixWaterHit ? 1u : 0u;
}
// CCamera::SetMotionBlur(r, g, b, value, type) (miembro, 5 argumentos: ret 0x14)
inline void __fastcall SBlur(void* self, void*, uint32 r, uint32 g, uint32 b, uint32 value, uint32 type) { if (Mine()) Push(T_BLUR, { P(self) - 0xB6F028u, r & 0xFF, g & 0xFF, b & 0xFF, value, type }); }
inline uint32 __cdecl SNearClip(uint32 camera, uint32 value) { if (!Mine()) return camera; Push(T_NEARCLIP, { camera, value }); return camera; }

inline uint8* rocketFrame{}; inline uint8* rocketMatrixObj{}; inline uint8* rocketPedObj{};
inline void __fastcall SUpdRwFrame(void* self, void*) { if (Mine()) Push(T_UPDRWFRAME, { P(self) - P(rocketPedObj) }); }
inline void __fastcall SUpdRpHAnim(void* self, void*) { if (Mine()) Push(T_UPDRPHANIM, { P(self) - P(rocketPedObj) }); }
// CMatrix::UpdateRwMatrix(RwMatrix*) y CSimpleTransform::UpdateRwMatrix(RwMatrix*): miembros con 1 argumento (ret 4)
inline void __fastcall SMatRw(void* self, void*, void* rw) { if (Mine()) Push(T_MATRWMATRIX, { uint32(self == rocketMatrixObj), uint32(rw == rocketFrame + 0x10) }); }
inline void __fastcall SSimpleRw(void* self, void*, void* rw) { if (Mine()) Push(T_SIMPLERWMATRIX, { P(self) - P(rocketPedObj), uint32(rw == rocketFrame + 0x10) }); }
inline int32 fixLR{}, fixUD{};
// CPad::LookAroundLeftRight(CPed*) / LookAroundUpDown(CPed*): int16 en AX (el EXE hace movsx)
inline int32 __fastcall SLookLR(void*, void*, void* ped) { if (!Mine()) return 0; Push(T_LOOKLR, { P(ped) }); return fixLR; }
inline int32 __fastcall SLookUD(void*, void*, void* ped) { if (!Mine()) return 0; Push(T_LOOKUD, { P(ped) }); return fixUD; }
inline void* fixPick{};
// CWeapon::PickTargetForHeatSeekingMissile(CVector, CVector, float, CEntity*, bool, CEntity*): 10 dwords cdecl
inline void* __cdecl SPickTarget(uint32 ox, uint32 oy, uint32 oz, uint32 dx, uint32 dy, uint32 dz, uint32 mult, void* ignore, uint32 planes, void* preferred) {
    if (!Mine()) return nullptr; Push(T_PICKTARGET, { ox, oy, oz, dx, dy, dz, mult, P(ignore), planes & 0xFF, P(preferred) }); return fixPick;
}
// CWeaponEffects::MarkTarget(id, CVector, r, g, b, a, size, clearImmediately): 10 dwords cdecl
inline void __cdecl SMark(uint32 id, uint32 x, uint32 y, uint32 z, uint32 r, uint32 g, uint32 b, uint32 a, uint32 size, uint32 clear) {
    if (Mine()) Push(T_MARKTARGET, { id, x, y, z, r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF, size, clear & 0xFF });
}

// ---------------- objetos falsos ----------------
inline Buf<0x238> camBuf;
inline Buf<0x800> entPed, ownerPed, driverPed, camTargetPed, hsTarget;
inline Buf<0x700> entVeh;
inline Buf<0x48> vehMat, leanMat, hsMat, ownerMat, pedMat;
inline Buf<0x100> modelInfoBuf, dummyBuf, playerData, rwObj, rwFrame, rwObj2;
inline Buf<16> targetVec;

// =====================================================================================================================
// CCam::Process_1stPerson (0x517EA0)
// =====================================================================================================================
inline void PrepareFirstPersonGlobals(uint32& s) {
    Mem<uint8>(0xB6EC20) = uint8(Rnd(s));                                          // gbFirstPersonRunThisFrame (el EXE escribe 1)
    static constexpr float last[] = { 0.f, 0.f, 100.f, 4000.f, 5000.f, 20000.f, 1e6f, -5.f };
    Mem<float>(0x8CCD14) = last[Rnd(s) % std::size(last)];                         // s_LastWheelieTime
    static constexpr float timers[] = { 0.f, 0.f, 0.5f, 50.f, -3.f, 10.f };
    Mem<float>(0xB6F18C) = timers[Rnd(s) % std::size(timers)];                     // TheCamera.m_fAvoidTheGeometryProbsTimer
    static constexpr float ground[] = { 0.f, 0.25f, 0.5f, 0.75f, -0.2f, 0.1f, 0.49f, 0.51f };
    Mem<float>(0xB7004C) = ground[Rnd(s) % std::size(ground)];                     // s_GroundFaultProtection
    Mem<uint16>(PADS + 0x0A) = (Rnd(s) % 5 == 0) ? uint16(255) : uint16(0);        // NewState.LeftShoulder2
    Mem<uint16>(PADS + 0x0E) = (Rnd(s) % 6 == 0) ? uint16(255) : uint16(0);        // NewState.RightShoulder2
    if (Rnd(s) % 10 == 0) Mem<uint32>(0xB7CB84) = Rnd(s) | 0x80000000u;            // tiempo en ms con signo negativo: fild + 2^32
    else if (Rnd(s) % 10 == 0) Mem<uint32>(0xB7CB84) = Rnd(s) & 0x7FFFFFFFu;       // tiempos mayores que 2^24 (el float pierde precision)
    Mem<float>(0xB7C508) = float(Rnd(s) % 400) / 10.f; Mem<float>(0xB7C50C) = float(Rnd(s) % 400) / 10.f; Mem<float>(0xB7C510) = float(Rnd(s) % 400) / 10.f;
}
inline json CaseFirstPerson(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7001); FixTimer(s); PrepareFirstPersonGlobals(s);
    camBuf.Fill(0xCD); uint8* cam = camBuf.p();
    entPed.Fill(0); entVeh.Fill(0); driverPed.Fill(0); vehMat.Fill(0xCD); leanMat.Fill(0xCD); allocMatrix.Fill(0xCD); modelInfoBuf.Fill(0); dummyBuf.Fill(0); rwObj.Fill(0xCD);
    losCalls = 0; fixLosBits = Rnd(s); fixWheel = Rnd(s) & 1; fixWaterHit = (Rnd(s) & 1) != 0;
    fixBone = { Bits(PickM(s)), Bits(PickM(s)), Bits(PickM(s)) };
    At<uint8>(cam, 0x0A) = uint8(Rnd(s) % 3 == 0);                                 // m_bResetStatics
    RandVec(cam, 0x190, s); RandVec(cam, 0x1B4, s);                                // front y up (se leen en la rama de la moto)
    targetVec.Fill(0xCD); RandVec(targetVec.p(), 0, s);
    const float orientation = PickM(s), speedVar = PickM(s), speedWanted = PickM(s);
    const bool pedTarget = (Rnd(s) % 4 == 0);
    uint8* ent;
    if (pedTarget) {
        ent = entPed.p(); At<uint8>(ent, 0x36) = 3; At<float>(ent, 0x558) = PickM(s);
        if (Rnd(s) % 4 != 0) At<uint32>(ent, 0x18) = P(rwObj.p());
    } else {
        ent = entVeh.p(); camVehicle = ent;
        At<uint8>(ent, 0x36) = 2; At<uint32>(ent, 0x18) = (Rnd(s) % 8 == 0) ? 0u : P(rwObj.p());
        static constexpr uint32 types[] = { 0, 0, 5, 9, 9, 9, 3, 1, 11 };
        At<uint32>(ent, 0x590) = types[Rnd(s) % std::size(types)];
        const uint32 type = At<uint32>(ent, 0x590);
        At<uint8>(ent, 0x614) = uint8(Rnd(s));
        At<int16>(ent, 0x22) = int16(FAKE_MODEL);
        RandVec(ent, 0x04, s); RandVec(ent, 0x44, s);
        At<uint32>(ent, 0x460) = (type == 9 && Rnd(s) % 5 != 0) ? P(driverPed.p()) : 0u;
        RandMatrix(vehMat.p(), s, (Rnd(s) % 3) != 0);
        if (Rnd(s) % 3 == 0) At<float>(vehMat.p(), 0x28) = -0.5f - float(Rnd(s) % 6) / 10.f;      // eje "up" mirando hacia abajo (IsUpsideDown)
        RandMatrix(allocMatrix.p(), s, (Rnd(s) % 3) != 0); RandMatrix(leanMat.p(), s, (Rnd(s) % 3) != 0);
        At<uint32>(ent, 0x14) = (Rnd(s) % 8 == 0) ? 0u : P(vehMat.p());        // sin matriz: AllocateMatrix (doble) enlaza allocMatrix
        std::memcpy(ent + 0x5CC, leanMat.p(), 0x48);                           // CBike::m_mLeanMatrix
        // ModelInfo falso: tipo (+0x3C, 5 = barca) y puntero a la estructura de vehiculo (+0x5C) con la tabla de dummies
        At<int32>(modelInfoBuf.p(), 0x3C) = (Rnd(s) % 3 == 0) ? 5 : int32(Rnd(s) % 4);
        At<uint32>(modelInfoBuf.p(), 0x5C) = P(dummyBuf.p());
        for (int k = 0; k < 24; ++k) At<float>(dummyBuf.p(), size_t(k) * 4) = PickM(s);
        Mem<uint32>(FAKE_MODEL_SLOT) = P(modelInfoBuf.p());
        fixWaterLevel = At<float>(ent, 0x04 + 8) + PickNear(s) * 3.f;
    }
    At<uint32>(cam, 0x21C) = P(ent);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__thiscall*)(void*, const float*, float, float, float)>(addr)(cam, reinterpret_cast<const float*>(targetVec.p()), orientation, speedVar, speedWanted);
        return json{ {"cam", camBuf.Dump()}, {"ped", entPed.Dump()}, {"veh", entVeh.Dump()}, {"vmat", vehMat.Dump()}, {"alloc", allocMatrix.Dump()},
                     {"g", json{ Mem<uint8>(0xB6EC20), Bits(Mem<float>(0x8CCD14)), Bits(Mem<float>(0xB6F18C)), Bits(Mem<float>(0xB7004C)) }} };
    });
}

// =====================================================================================================================
// CCam::Process_Rocket (0x511B50)
// =====================================================================================================================
inline json CaseRocket(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7002); FixTimer(s);
    camBuf.Fill(0xCD); uint8* cam = camBuf.p();
    entPed.Fill(0); ownerPed.Fill(0); camTargetPed.Fill(0); hsTarget.Fill(0); playerData.Fill(0); rwObj.Fill(0xCD); rwFrame.Fill(0xCD); ownerMat.Fill(0xCD); pedMat.Fill(0xCD); hsMat.Fill(0xCD);
    rocketFrame = rwFrame.p(); rocketPedObj = entPed.p(); rocketMatrixObj = pedMat.p();
    At<uint32>(rwObj.p(), 4) = P(rwFrame.p());                                       // RwObject.parent -> RwFrame (el modelling matrix esta en +0x10)
    losCalls = 0; fixLosBits = Rnd(s);
    fixBone = { Bits(PickM(s)), Bits(PickM(s)), Bits(PickM(s)) };
    fixLR = int32(Rnd(s) % 257) - 128; fixUD = int32(Rnd(s) % 257) - 128;
    if (Rnd(s) % 6 == 0) fixLR = 0;
    if (Rnd(s) % 6 == 0) fixUD = 0;
    // raton: sin movimiento (rama del mando) o con movimiento
    const bool mouse = (Rnd(s) % 3 == 0);
    Mem<float>(0xB73424) = mouse ? PickM(s) : 0.f; Mem<float>(0xB73428) = mouse ? (Rnd(s) % 4 == 0 ? 0.f : PickM(s)) : 0.f;
    if (mouse && Rnd(s) % 4 == 0) Mem<float>(0xB73424) = 0.f;
    Mem<float>(0xB6EC18) = PickNear(s) * 0.5f + 0.6f; Mem<float>(0xB6EC1C) = PickNear(s) * 0.5f + 0.6f;   // aceleraciones del raton
    Mem<uint8>(0xB6EC2E) = uint8(Rnd(s) & 1);                                       // m_bUseMouse3rdPerson
    Mem<uint8>(0xB70000) = uint8(Rnd(s)); Mem<uint32>(0xB6FFFC) = Rnd(s); Mem<uint32>(0xB6FFF8) = Rnd(s);
    for (size_t k = 0; k < 0x58; ++k) Mem<uint8>(0xC8A838 + k) = uint8(Rnd(s));
    Mem<uint32>(0xB6F980) = P(camTargetPed.p());                                    // TheCamera.m_pTargetPed (pedido por GetATanOfXY...)
    At<uint8>(cam, 0x0A) = uint8(Rnd(s) % 3 == 0);                                  // m_bResetStatics
    At<float>(cam, 0xBC) = PickNear(s) * 1.5f; At<float>(cam, 0xAC) = PickNear(s) * 0.7f;   // angulos (se leen y se suman)
    At<float>(cam, 0xA8) = PickM(s);
    RandVec(cam, 0x190, s); RandVec(cam, 0x1B4, s);
    const bool pedTarget = (Rnd(s) % 8 != 0);
    uint8* ent = entPed.p();
    if (pedTarget) {
        At<uint8>(ent, 0x36) = 3; At<float>(ent, 0x558) = PickM(s);
        At<uint32>(ent, 0x71C) = (Rnd(s) % 3 == 0) ? P(hsTarget.p()) : 0u;        // m_pTargetedObject
        At<uint32>(ent, 0x18) = (Rnd(s) % 4 == 0) ? 0u : P(rwObj.p());
        RandMatrix(pedMat.p(), s, false);
        At<uint32>(ent, 0x14) = (Rnd(s) & 1) ? P(pedMat.p()) : 0u; RandVec(ent, 0x04, s);
    } else {
        At<uint8>(ent, 0x36) = 2;                                                   // vehiculo: solo se fija el plano cercano
    }
    At<uint32>(cam, 0x21C) = P(ent);
    // jugador y datos de apuntado (misil guiado): +0x94 tiempo, +0x98 ultimo objetivo, +0xA0 tiempo-LOS (31 bits) | LOS (bit 31)
    At<uint32>(ownerPed.p(), 0x480) = P(playerData.p()); RandMatrix(ownerMat.p(), s, false); RandVec(ownerPed.p(), 0x04, s);
    At<uint32>(ownerPed.p(), 0x14) = (Rnd(s) & 1) ? P(ownerMat.p()) : 0u; At<uint32>(ownerPed.p(), 0x1C) = Rnd(s);
    fixPed[0] = fixPed[1] = ownerPed.p();
    const uint32 now = Mem<uint32>(0xB7CB84);
    static constexpr uint32 pressedDeltas[] = { 0, 0, 100, 1000, 1499, 1500, 1501, 4000 };
    At<uint32>(playerData.p(), 0x94) = (Rnd(s) % 4 == 0) ? 0u : now - pressedDeltas[Rnd(s) % std::size(pressedDeltas)];
    static constexpr uint32 losLow[] = { 0, 1, 2, 0x7FFFFFFF, 12345 };
    At<uint32>(playerData.p(), 0xA0) = (losLow[Rnd(s) % std::size(losLow)] & 0x7FFFFFFFu) | ((Rnd(s) & 1) ? 0x80000000u : 0u);
    RandVec(hsTarget.p(), 0x04, s); RandMatrix(hsMat.p(), s, false); At<uint32>(hsTarget.p(), 0x14) = (Rnd(s) & 1) ? P(hsMat.p()) : 0u; At<uint32>(hsTarget.p(), 0x1C) = Rnd(s);
    fixPick = (Rnd(s) % 3 == 0) ? nullptr : hsTarget.p();
    const uint32 lastKind = Rnd(s) % 3;
    At<uint32>(playerData.p(), 0x98) = lastKind == 0 ? 0u : lastKind == 1 ? P(hsTarget.p()) : P(ownerPed.p());
    const bool heat = (Rnd(s) & 1) != 0;
    const float orientation = PickM(s), speedVar = PickM(s), speedWanted = PickM(s);
    Mem<uint32>(0xC1703C) = P(rwObj2.p());                                          // camara RW falsa (solo se pasa como puntero al plano cercano)
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__thiscall*)(void*, const float*, float, float, float, bool)>(addr)(cam, reinterpret_cast<const float*>(targetVec.p()), orientation, speedVar, speedWanted, heat);
        return json{ {"cam", camBuf.Dump()}, {"ped", entPed.Dump()}, {"cam_ped", camTargetPed.Dump()}, {"owner", ownerPed.Dump()}, {"pdata", playerData.Dump()}, {"hs", hsTarget.Dump()},
                     {"cross", HexAt(0xC8A838, 0x58)}, {"g", json{ Mem<uint8>(0xB70000), Mem<uint32>(0xB6FFFC), Mem<uint32>(0xB6FFF8) }} };
    });
}

// ---------------- registro de colaboradores de CCam ----------------
inline void RegisterCamSites() {
    using std::bit_cast;
    AddSite("CCam::Process_WheelCam", 0x512110, bit_cast<uintptr_t>(&CCam::Process_WheelCam), (&SWheelCam));
    AddSite("CPlaceable::AllocateMatrix", 0x54F560, bit_cast<uintptr_t>(&CPlaceable::AllocateMatrix), (&SAllocMatrix));
    AddSite("CSimpleTransform::UpdateMatrix", 0x54F1B0, bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix), (&SUpdateMatrix));
    AddSite("CBike::CalculateLeanMatrix", 0x6B7150, bit_cast<uintptr_t>(&CBike::CalculateLeanMatrix), (&SLean));
    AddSite("CPed::GetBonePosition", 0x5E4280, bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*, eBoneTag, bool)>(&CPed::GetBonePosition)), (&SBone));
    AddSite("CWorld::GetIsLineOfSightClear", 0x56A490, bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear), (&SLos));
    AddSite("CWaterLevel::GetWaterLevel", 0x6EB690, bit_cast<uintptr_t>(static_cast<bool(*)(float, float, float, float&, uint8, CVector*)>(&CWaterLevel::GetWaterLevel)), (&SWater));
    AddSite("CCamera::SetMotionBlur", 0x50BF40, bit_cast<uintptr_t>(&CCamera::SetMotionBlur), (&SBlur));
    AddSite("RwCameraSetNearClipPlane", 0x7EE1D0, 0, (&SNearClip));
    AddSite("CEntity::UpdateRwFrame", 0x532B00, bit_cast<uintptr_t>(&CEntity::UpdateRwFrame), (&SUpdRwFrame));
    AddSite("CEntity::UpdateRpHAnim", 0x532B20, bit_cast<uintptr_t>(&CEntity::UpdateRpHAnim), (&SUpdRpHAnim));
    AddSite("CMatrix::UpdateRwMatrix", 0x59AD70, bit_cast<uintptr_t>(&CMatrix::UpdateRwMatrix), (&SMatRw));
    AddSite("CSimpleTransform::UpdateRwMatrix", 0x54EF40, bit_cast<uintptr_t>(&CSimpleTransform::UpdateRwMatrix), (&SSimpleRw));
    AddSite("CPad::LookAroundLeftRight", 0x540BD0, bit_cast<uintptr_t>(&CPad::LookAroundLeftRight), (&SLookLR));
    AddSite("CPad::LookAroundUpDown", 0x540CC0, bit_cast<uintptr_t>(&CPad::LookAroundUpDown), (&SLookUD));
    AddSite("CWeapon::PickTargetForHeatSeekingMissile", 0x73F910, bit_cast<uintptr_t>(&CWeapon::PickTargetForHeatSeekingMissile), (&SPickTarget));
    AddSite("CWeaponEffects::MarkTarget", 0x742BF0, bit_cast<uintptr_t>(&CWeaponEffects::MarkTarget), (&SMark));
}
inline std::unique_ptr<Patches> CamPatches(const std::string& name) {
    if (name == "Process_1stPerson") return Use({ "CCam::Process_WheelCam", "CPlaceable::AllocateMatrix", "CSimpleTransform::UpdateMatrix", "CBike::CalculateLeanMatrix", "CPed::GetBonePosition",
        "CWorld::GetIsLineOfSightClear", "CWaterLevel::GetWaterLevel", "CCamera::SetMotionBlur" });
    if (name == "Process_Rocket") return Use({ "FindPlayerPed", "CEntity::UpdateRwFrame", "CEntity::UpdateRpHAnim", "CMatrix::UpdateRwMatrix", "CSimpleTransform::UpdateRwMatrix", "CPed::GetBonePosition",
        "CPad::LookAroundLeftRight", "CPad::LookAroundUpDown", "CWeapon::PickTargetForHeatSeekingMissile", "CWorld::GetIsLineOfSightClear", "CWeaponEffects::MarkTarget", "RwCameraSetNearClipPlane" });
    return nullptr;
}
}
