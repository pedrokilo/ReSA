/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CVehicle
 * Reconstrucción: COMPLETA — 161/161 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 161/161 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include <optional>
#include <functional>
#include <extensions/utility.hpp>

#include "Vehicle.h"
#include "Garages.h"
#include "ModelIndices.h"
#include "CustomCarPlateMgr.h"
#include "Buoyancy.h"
#include "CarCtrl.h"
#include "VehicleSaveStructure.h"
#include "Radar.h"
#include "RideAnims.h"
#include "Rope.h"
#include "Ropes.h"
#include "Ragdoll/IKChainManager.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "Shadows.h"
#include "CustomBuildingDNPipeline.h"
#include "PedClothesDesc.h"
#include "X87Math.h"

auto& planeRotorDmgTimeMS = StaticRef<uint32>(0xC1CC1C);

auto& fBurstTyreMod = StaticRef<float>(0x8D34B4);                // 0.13f
auto& fBurstSpeedMax = StaticRef<float>(0x8D34B8);               // 0.3f
auto& CAR_NOS_EXTRA_SKID_LOSS = StaticRef<float>(0x8D34BC);      // 0.9f
auto& WS_TRAC_FRAC_LIMIT = StaticRef<float>(0x8D34C0);           // 0.3f
auto& WS_ALREADY_SPINNING_LOSS = StaticRef<float>(0x8D34C4);     // 0.2f
auto& fBurstBikeTyreMod = StaticRef<float>(0x8D34C8);            // 0.05f
auto& fBurstBikeSpeedMax = StaticRef<float>(0x8D34CC);           // 0.12f
auto& fTweakBikeWheelTurnForce = StaticRef<float>(0x8D34D0);     // 2.0f
auto& AUTOGYRO_ROTORSPIN_MULT = StaticRef<float>(0x8D34D4);      // 0.006f
auto& AUTOGYRO_ROTORSPIN_MULTLIMIT = StaticRef<float>(0x8D34D8); // 0.25f
auto& AUTOGYRO_ROTORSPIN_DAMP = StaticRef<float>(0x8D34DC);      // 0.997f
auto& AUTOGYRO_ROTORLIFT_MULT = StaticRef<float>(0x8D34E0);      // 4.5f
auto& AUTOGYRO_ROTORLIFT_FALLOFF = StaticRef<float>(0x8D34E4);   // 0.75f
auto& AUTOGYRO_ROTORTILT_ANGLE = StaticRef<float>(0x8D34E8);     // 0.25f
auto& ROTOR_SEMI_THICKNESS = StaticRef<float>(0x8D34EC);         // 0.05f
float* gfSpeedMult = (float*)0x8D34F8;                   // float fSpeedMult[5] = { 0.8f, 0.75f, 0.85f, 0.9f, 0.85f, 0.85f }
auto& fDamagePosSpeedShift = StaticRef<float>(0x8D3510);         // 0.4f
auto& DIFF_LIMIT = StaticRef<float>(0x8D35B4);                   // 0.8f
auto& DIFF_SPRING_MULT_X = StaticRef<float>(0x8D35B8);           // 0.05f
auto& DIFF_SPRING_MULT_Y = StaticRef<float>(0x8D35BC);           // 0.05f
auto& DIFF_SPRING_MULT_Z = StaticRef<float>(0x8D35C0);           // 0.1f
auto& DIFF_SPRING_COMPRESS_MULT = StaticRef<float>(0x8D35C4);    // 2.0f
auto& VehicleGunOffset = StaticRef<std::array<CVector, 14>>(0x8D35D4); // maybe [12]

// Auxiliares del banco CVehE: reproducen el orden de operaciones (x87 a 24 bits) de los helpers del EXE
namespace {
// Como el original: _ftol2 (0x821B40) trunca hacia cero a int64; se usa la palabra baja
int32 VehEFtol(float f) { return static_cast<int32>(static_cast<int64>(f)); }

// Como el original: CMatrix * CVector (0x59C890): x = z, y, x; y = z, x, y; z = z, x, y; mas la posicion
CVector VehEMatPoint(const CMatrix& m, const CVector& v) {
    return CVector{
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// Como el original: Multiply3x3 (0x59C790) con las sumas en el orden del EXE
CVector VehEMatVec(const CMatrix& m, const CVector& v) {
    return CVector{
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y,
    };
}

// Como el original: transformacion inversa de un vector (0x59C810)
CVector VehEInvVec(const CMatrix& m, const CVector& v) {
    return CVector{
        (m.GetRight().y * v.y + m.GetRight().z * v.z) + v.x * m.GetRight().x,
        (m.GetForward().y * v.y + m.GetForward().x * v.x) + m.GetForward().z * v.z,
        (m.GetUp().y * v.y + m.GetUp().x * v.x) + m.GetUp().z * v.z,
    };
}

// Como el original: producto escalar de 0x40FDB0 (sumas z, y, x)
float VehEDot(const CVector& a, const CVector& b) {
    return (a.z * b.z + a.y * b.y) + a.x * b.x;
}

// Como el original: producto vectorial de 0x59C730
CVector VehECross(const CVector& a, const CVector& b) {
    return CVector{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

// Como el original: pow del CRT del EXE (_CIpow, 0x822130, precision x87 completa) y resultado guardado como float
float VehEPow(float base, float exponent) {
    return CnvDecayPowCpp(base, exponent); // fld base; fld exponent; call 0x822130; fstp dword: pow del EXE con un solo redondeo a float
}

// Como el original: fsin y fcos del x87 sobre el mismo valor (precision completa, el resultado se redondea al guardarlo)
void VehESinCos(float angle, float& outSin, float& outCos) {
    outSin = TrigSimpleSinCpp(angle);
    outCos = TrigSimpleCosCpp(angle);
}

// Como el original: CVector2D::Normalise (0x44E480); con suma de cuadrados no positiva solo pone x = 1
void VehENormalise2D(CVector2D& v) {
    const float sumSq = v.x * v.x + v.y * v.y;
    if (!(sumSq > 0.0f) && !std::isnan(sumSq)) {
        v.x = 1.0f;
        return;
    }
    const float inv = 1.0f / std::sqrt(sumSq);
    v.x = inv * v.x;
    v.y = inv * v.y;
}

// Como el original: CVector::Normalise (0x59C910); con suma de cuadrados no positiva solo pone x = 1
void VehENormalise(CVector& v) {
    const float sumSq = (v.x * v.x + v.y * v.y) + v.z * v.z;
    if (!(sumSq > 0.0f) && !std::isnan(sumSq)) {
        v.x = 1.0f;
        return;
    }
    const float inv = 1.0f / std::sqrt(sumSq);
    v.x = inv * v.x;
    v.y = inv * v.y;
    v.z = inv * v.z;
}

// Como el original: CVector::NormaliseAndMag (0x59C970); devuelve 1 / (1 / raiz), no la raiz
float VehENormaliseAndMag(CVector& v) {
    const float sumSq = (v.x * v.x + v.y * v.y) + v.z * v.z;
    if (!(sumSq > 0.0f) && !std::isnan(sumSq)) {
        v.x = 1.0f;
        return 1.0f;
    }
    const float inv = 1.0f / std::sqrt(sumSq);
    v.x = inv * v.x;
    v.y = inv * v.y;
    v.z = inv * v.z;
    return 1.0f / inv;
}
} // namespace

void CVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CVehicle, 0x871e80, 66);
    RH_ScopedCategory("Vehicle");

    RH_ScopedOverloadedInstall(operator new, "anon", 0x6E2D50, void* (*)(unsigned));
    RH_ScopedInstall(Constructor, 0x6D5F10);
    RH_ScopedInstall(Destructor, 0x6E2B40);

    RH_ScopedVMTInstall(SetModelIndex, 0x6D6A40);
    RH_ScopedVMTInstall(DeleteRwObject, 0x6D6410);
    RH_ScopedVMTInstall(SpecialEntityPreCollisionStuff, 0x6D6640);
    RH_ScopedVMTInstall(SpecialEntityCalcCollisionSteps, 0x6D0E90);
    RH_ScopedVMTInstall(SetupLighting, 0x553F20);
    RH_ScopedVMTInstall(RemoveLighting, 0x5533D0);
    RH_ScopedVMTInstall(PreRender, 0x6D6480);
    RH_ScopedVMTInstall(Render, 0x6D0E60);
    RH_ScopedVMTInstall(ProcessOpenDoor, 0x6D56C0);
    RH_ScopedVMTInstall(GetHeightAboveRoad, 0x6D63F0);
    RH_ScopedVMTInstall(CanPedStepOutCar, 0x6D1F30);
    RH_ScopedVMTInstall(CanPedJumpOutCar, 0x6D2030);
    RH_ScopedVMTInstall(GetTowHitchPos, 0x6DFB70);
    RH_ScopedVMTInstall(GetTowBarPos, 0x6DFBE0);
    RH_ScopedVMTInstall(Save, 0x5D4760, {.enabled = false });
    RH_ScopedVMTInstall(Load, 0x5D2900, {.enabled = false });

    RH_ScopedVMTInstall(ProcessDrivingAnims, 0x6DF4A0);

    RH_ScopedOverloadedInstall(IsPassenger, "Ped", 0x6D1BD0, bool(CVehicle::*)(CPed*) const);
    RH_ScopedOverloadedInstall(IsPassenger, "ModelID", 0x6D1C00, bool(CVehicle::*)(int32) const);
    RH_ScopedOverloadedInstall(IsDriver, "Ped", 0x6D1C40, bool(CVehicle::*)(const CPed*) const);
    RH_ScopedOverloadedInstall(IsDriver, "ModelID", 0x6D1C60, bool(CVehicle::*)(int32) const);
    RH_ScopedInstall(Shutdown, 0x6D0B40);
    RH_ScopedInstall(GetRemapIndex, 0x6D0B70);
    RH_ScopedInstall(SetRemap, 0x6D0C00);
    RH_ScopedInstall(SetCollisionLighting, 0x6D0CA0);
    RH_ScopedInstall(UpdateLightingFromStoredPolys, 0x6D0CC0);
    RH_ScopedInstall(CalculateLightingFromCollision, 0x6D0CF0);
    RH_ScopedInstall(ResetAfterRender, 0x6D0E20);
    RH_ScopedInstall(ProcessWheel, 0x6D6C00);
    RH_ScopedInstall(ApplyBoatWaterResistance, 0x6D2740);
    RH_ScopedInstall(ProcessBoatControl, 0x6DBCE0);
    RH_ScopedInstall(ChangeLawEnforcerState, 0x6D2330);
    RH_ScopedNamedInstall(GetVehicleAppearance_Reversed, "GetVehicleAppearance", 0x6D1080);
    RH_ScopedInstall(DoHeadLightBeam, 0x6E0E20);
    RH_ScopedInstall(GetPlaneNumGuns, 0x6D3F30); // ??: register problem?

    RH_ScopedInstall(CustomCarPlate_TextureCreate, 0x6D10E0);
    RH_ScopedInstall(CustomCarPlate_TextureDestroy, 0x6D1150);
    RH_ScopedInstall(CanBeDeleted, 0x6D1180);
    RH_ScopedInstall(ProcessWheelRotation, 0x6D1230);
    RH_ScopedInstall(CanVehicleBeDamaged, 0x6D1280);
    RH_ScopedInstall(ProcessDelayedExplosion, 0x6D1340);
    RH_ScopedOverloadedInstall(AddPassenger, "Auto-Seat", 0x6D13A0, bool(CVehicle::*)(CPed*));
    RH_ScopedOverloadedInstall(AddPassenger, "Fixed-Seat", 0x6D14D0, bool(CVehicle::*)(CPed*, uint8));
    RH_ScopedInstall(RemovePassenger, 0x6D1610);
    RH_ScopedInstall(SetDriver, 0x6D16A0);
    RH_ScopedInstall(RemoveDriver, 0x6D1950);
    RH_ScopedInstall(SetUpDriver, 0x6D1A50);
    RH_ScopedInstall(SetupPassenger, 0x6D1AA0);
    RH_ScopedInstall(KillPedsInVehicle, 0x6D1C80);
    RH_ScopedInstall(IsUpsideDown, 0x6D1D90);
    RH_ScopedInstall(IsOnItsSide, 0x6D1DD0);
    RH_ScopedInstall(CanPedOpenLocks, 0x6D1E20);
    RH_ScopedInstall(CanDoorsBeDamaged, 0x6D1E60);
    RH_ScopedInstall(CanPedEnterCar, 0x6D1E80);
    RH_ScopedInstall(ProcessCarAlarm, 0x6D21F0);
    RH_ScopedInstall(DestroyVehicleAndDriverAndPassengers, 0x6D2250);
    RH_ScopedInstall(IsVehicleNormal, 0x6D22F0);
    RH_ScopedInstall(IsLawEnforcementVehicle, 0x6D2370);
    RH_ScopedInstall(ExtinguishCarFire, 0x6D2460);
    RH_ScopedInstall(ActivateBomb, 0x6D24F0);
    RH_ScopedInstall(ActivateBombWhenEntered, 0x6D2570);
    RH_ScopedInstall(CarHasRoof, 0x6D25D0);
    RH_ScopedInstall(HeightAboveCeiling, 0x6D2600);
    RH_ScopedInstall(SetComponentVisibility, 0x6D2700);
    RH_ScopedInstall(SetComponentAtomicAlpha, 0x6D2960);
    RH_ScopedInstall(UpdateClumpAlpha, 0x6D2980);
    RH_ScopedInstall(UpdatePassengerList, 0x6D29E0);
    RH_ScopedInstall(PickRandomPassenger, 0x6D2A10);
    RH_ScopedInstall(AddDamagedVehicleParticles, 0x6D2A80);
    RH_ScopedInstall(MakeDirty, 0x6D2BF0);
    RH_ScopedInstall(AddWheelDirtAndWater, 0x6D2D50);
    RH_ScopedInstall(SetGettingInFlags, 0x6D3000);
    RH_ScopedInstall(SetGettingOutFlags, 0x6D3020);
    RH_ScopedInstall(ClearGettingInFlags, 0x6D3040);
    RH_ScopedInstall(ClearGettingOutFlags, 0x6D3060);
    RH_ScopedInstall(SetWindowOpenFlag, 0x6D3080);
    RH_ScopedInstall(ClearWindowOpenFlag, 0x6D30B0);
    RH_ScopedInstall(SetVehicleUpgradeFlags, 0x6D30E0);
    RH_ScopedInstall(ClearVehicleUpgradeFlags, 0x6D3210);
    RH_ScopedInstall(CreateUpgradeAtomic, 0x6D3510);
    RH_ScopedInstall(RemoveUpgrade, 0x6D3630);
    RH_ScopedInstall(GetUpgrade, 0x6D3650);
    RH_ScopedInstall(CreateReplacementAtomic, 0x6D3700);
    RH_ScopedInstall(AddReplacementUpgrade, 0x6D3830);
    RH_ScopedInstall(RemoveReplacementUpgrade, 0x6D39E0);
    RH_ScopedInstall(GetReplacementUpgrade, 0x6D3A50);
    RH_ScopedInstall(RemoveAllUpgrades, 0x6D3AB0);
    RH_ScopedInstall(GetSpareHasslePosId, 0x6D3AE0);
    RH_ScopedInstall(SetHasslePosId, 0x6D3B30);
    RH_ScopedInstall(InitWinch, 0x6D3B60);
    RH_ScopedInstall(UpdateWinch, 0x6D3B80);
    RH_ScopedInstall(RemoveWinch, 0x6D3C70);
    RH_ScopedInstall(RenderDriverAndPassengers, 0x6D3D60);
    RH_ScopedInstall(PreRenderDriverAndPassengers, 0x6D3DB0);
    RH_ScopedInstall(GetPlaneGunsAutoAimAngle, 0x6D3E00);
    RH_ScopedInstall(SetFiringRateMultiplier, 0x6D4010);
    RH_ScopedInstall(GetFiringRateMultiplier, 0x6D4090);
    RH_ScopedInstall(GetPlaneGunsRateOfFire, 0x6D40E0);
    RH_ScopedInstall(GetPlaneGunsPosition, 0x6D4290);
    RH_ScopedInstall(GetPlaneOrdnanceRateOfFire, 0x6D4590);
    RH_ScopedInstall(GetPlaneOrdnancePosition, 0x6D46E0);
    RH_ScopedInstall(SelectPlaneWeapon, 0x6D4900);
    RH_ScopedInstall(DoPlaneGunFireFX, 0x6D4AD0);
    RH_ScopedInstall(FirePlaneGuns, 0x6D4D30);
    RH_ScopedInstall(FireUnguidedMissile, 0x6D5110);
    RH_ScopedInstall(CanBeDriven, 0x6D5400);
    RH_ScopedInstall(ReactToVehicleDamage, 0x6D5490);
    RH_ScopedInstall(GetVehicleLightsStatus, 0x6D55C0);
    RH_ScopedInstall(CanPedLeanOut, 0x6D5CF0);
    RH_ScopedInstall(SetVehicleCreatedBy, 0x6D5D70);
    RH_ScopedInstall(SetupRender, 0x6D64F0);
    // RH_ScopedInstall(ProcessBikeWheel, 0x6D73B0);
    RH_ScopedInstall(FindTyreNearestPoint, 0x6D7BC0);
    // RH_ScopedInstall(InflictDamage, 0x6D7C90);
    RH_ScopedInstall(KillPedsGettingInVehicle, 0x6D82F0);
    RH_ScopedInstall(UsesSiren, 0x6D8470);
    RH_ScopedInstall(IsSphereTouchingVehicle, 0x6D84D0);
    RH_ScopedInstall(FlyingControl, 0x6D85F0);
    RH_ScopedInstall(BladeColSectorList<CPtrListSingleLink<CEntity*>>, 0x6DAF00);
    RH_ScopedInstall(SetComponentRotation, 0x6DBA30);
    RH_ScopedInstall(SetTransmissionRotation, 0x6DBBB0);
    RH_ScopedInstall(DoBoatSplashes, 0x6DD130);
    // RH_ScopedInstall(DoSunGlare, 0x6DD6F0);
    RH_ScopedInstall(AddWaterSplashParticles, 0x6DDF60);
    RH_ScopedInstall(AddExhaustParticles, 0x6DE240);
    // RH_ScopedInstall(AddSingleWheelParticles, 0x6DE880);
    RH_ScopedInstall(GetSpecialColModel, 0x6DF3D0);
    RH_ScopedInstall(RemoveVehicleUpgrade, 0x6DF930);
    RH_ScopedInstall(AddUpgrade, 0x6DFA20);
    // RH_ScopedInstall(UpdateTrailerLink, 0x6DFC50);
    // RH_ScopedInstall(UpdateTractorLink, 0x6E0050);
    RH_ScopedInstall(ScanAndMarkTargetForHeatSeekingMissile, 0x6E0400);
    RH_ScopedInstall(FireHeatSeakingMissile, 0x6E05C0);
    RH_ScopedInstall(PossiblyDropFreeFallBombForPlayer, 0x6E07E0);
    RH_ScopedInstall(ProcessSirenAndHorn, 0x6E0950);
    RH_ScopedInstall(DoHeadLightEffect, 0x6E0A50);
    RH_ScopedInstall(DoHeadLightReflectionSingle, 0x6E1440);
    RH_ScopedInstall(DoHeadLightReflectionTwin, 0x6E1600);
    RH_ScopedInstall(DoHeadLightReflection, 0x6E1720);
    RH_ScopedInstall(DoTailLightEffect, 0x6E1780);
    RH_ScopedInstall(DoVehicleLights, 0x6E1A60);
    RH_ScopedInstall(FillVehicleWithPeds, 0x6E2900);
    RH_ScopedInstall(DoBladeCollision, 0x6E2E50);
    RH_ScopedInstall(AddVehicleUpgrade, 0x6E3290);
    RH_ScopedInstall(SetupUpgradesAfterLoad, 0x6E3400);
    RH_ScopedInstall(GetPlaneWeaponFiringStatus, 0x6E3440);
    RH_ScopedInstall(ProcessWeapons, 0x6E3950);
    RH_ScopedInstall(DoFixedMachineGuns, 0x73F400);
    // RH_ScopedInstall(FireFixedMachineGuns, 0x73DF00);
    // RH_ScopedInstall(DoDriveByShootings, 0x741FD0);
    RH_ScopedInstall(ReleasePickedUpEntityWithWinch, 0x6D3CB0);
    RH_ScopedInstall(PickUpEntityWithWinch, 0x6D3CD0);
    RH_ScopedInstall(QueryPickedUpEntityWithWinch, 0x6D3CF0);
    RH_ScopedInstall(GetRopeHeightForHeli, 0x6D3D10);
    RH_ScopedInstall(SetRopeHeightForHeli, 0x6D3D30);

    RH_ScopedGlobalOverloadedInstall(SetVehicleAtomicVisibilityCB, "Object", 0x6D2690, RwObject*(*)(RwObject*, void*));
    RH_ScopedGlobalOverloadedInstall(SetVehicleAtomicVisibilityCB, "Frame", 0x6D26D0, RwFrame*(*)(RwFrame*, void*));
    // RH_ScopedGlobalInstall(SetCompAlphaCB, 0x6D2950);
    RH_ScopedGlobalInstall(IsVehiclePointerValid, 0x6E38F0);
    RH_ScopedGlobalInstall(IsValidModForVehicle, 0x49B010);
    RH_ScopedGlobalInstall(RemoveUpgradeCB, 0x6D3300);
    RH_ScopedGlobalInstall(FindUpgradeCB, 0x6D3370);
    RH_ScopedGlobalOverloadedInstall(RemoveObjectsCB, "Object", 0x6D33B0, RwObject*(*)(RwObject*, void*));
    RH_ScopedGlobalOverloadedInstall(RemoveObjectsCB, "Frame", 0x6D3420, RwFrame*(*)(RwFrame*, void*));
    RH_ScopedGlobalInstall(CopyObjectsCB, 0x6D3450);
    RH_ScopedGlobalInstall(FindReplacementUpgradeCB, 0x6D3490);
    RH_ScopedGlobalInstall(RemoveAllUpgradesCB, 0x6D34D0);
}

// 0x6D5F10
CVehicle::CVehicle(eVehicleCreatedBy createdBy) : CPhysical(), m_vehicleAudio(), m_autoPilot() {
    m_bHasPreRenderEffects = true;
    SetTypeVehicle();

    m_fRawSteerAngle = 0.0f;
    m_fSteerAngle = 0.0f; // Como el original: tambien se pone a cero (0x494)
    m_f2ndSteerAngle = 0.0f;
    m_nCurrentGear = 1;
    m_fGearChangeCount = 0.0f;
    m_fWheelSpinForAudio = 0.0f;
    m_nCreatedBy = createdBy;
    m_nForcedRandomRouteSeed = 0;

    m_nVehicleUpperFlags = 0;
    m_nVehicleLowerFlags = 0;
    vehicleFlags.bFreebies = true;
    vehicleFlags.bIsHandbrakeOn = true;
    vehicleFlags.bEngineOn = true;
    vehicleFlags.bCanBeDamaged = true;
    vehicleFlags.bParking = false;
    vehicleFlags.bRestingOnPhysical = false;
    vehicleFlags.bCreatedAsPoliceVehicle = false;
    vehicleFlags.bVehicleCanBeTargettedByHS = true;
    vehicleFlags.bWinchCanPickMeUp = true;
    vehicleFlags.bPetrolTankIsWeakPoint = true;
    vehicleFlags.bConsideredByPlayer = true;
    vehicleFlags.bDoesProvideCover = true;
    vehicleFlags.bUsedForReplay = false;
    vehicleFlags.bDontSetColourWhenRemapping = false;
    vehicleFlags.bUseCarCheats = false;
    vehicleFlags.bHasBeenResprayed = false;
    vehicleFlags.bNeverUseSmallerRemovalRange = false;
    vehicleFlags.bDriverLastFrame = false;

    auto fRand = static_cast<float>(CGeneral::GetRandomNumber()) / static_cast<float>(RAND_MAX);
    vehicleFlags.bCanPark = fRand < 0.0F; //BUG: Seemingly never true, CGeneral::GetRandomNumber() strips the sign bit to always be 0

    CCarCtrl::UpdateCarCount(this, false);
    m_nExtendedRemovalRange = 0;
    m_fHealth = 1000.0f;
    m_pDriver = nullptr;
    m_nNumPassengers = 0;
    m_nMaxPassengers = 8;
    m_nNumGettingIn = 0;
    m_nGettingInFlags = 0;
    m_nGettingOutFlags = 0;

    m_nBombOnBoard = 0;
    m_nOverrideLights = eVehicleOverrideLightsState::NO_CAR_LIGHT_OVERRIDE;
    m_ropeType = 0;
    m_nGunsCycleIndex = 0;
    m_nOrdnanceCycleIndex = 0; // Como el original: el EXE borra los dos indices de ciclo (&0xF0 del byte 0x4A9)
    physicalFlags.bCanBeCollidedWith = true;

    m_nLastWeaponDamageType = -1;
    m_vehicleSpecialColIndex = -1;

    m_pWhoInstalledBombOnMe = nullptr;
    m_wBombTimer = 0;
    m_pWhoDetonatedMe = nullptr;
    m_nTimeWhenBlowedUp = 0;

    m_nPacMansCollected = 0;
    m_pFire = nullptr;
    m_nGunFiringTime = 0;
    m_nCopsInCarTimer = 0;
    m_nUsedForCover = 0;
    m_HornCounter = 0;
    m_HornPattern = 0;
    m_nCarHornTimer = 0;
    field_4EC = 0;
    m_pTowingVehicle = nullptr;
    m_pVehicleBeingTowed = nullptr;
    m_nTimeTillWeNeedThisCar = 0;
    m_nAlarmState = 0;
    m_nDoorLock = eCarLock::CARLOCK_UNLOCKED;
    m_nProjectileWeaponFiringTime = 0;
    m_nAdditionalProjectileWeaponFiringTime = 0;
    m_nTimeForMinigunFiring = 0;
    m_pLastDamageEntity = nullptr;
    m_pEntityWeAreOn = nullptr;
    m_fVehicleRearGroundZ = 0.0f;
    m_fVehicleFrontGroundZ = 0.0f;
    field_511 = 0;
    field_512 = 0;
    m_comedyControlState = eComedyControlState::INACTIVE;
    m_FrontCollPoly.valid = false;
    m_RearCollPoly.valid = false;
    m_pHandlingData = nullptr;
    m_nHandlingFlagsIntValue = static_cast<eVehicleHandlingFlags>(0);
    m_autoPilot.m_nTempAction = TEMPACT_NONE;
    m_autoPilot.SetCarMission(MISSION_NONE, 0);
    m_autoPilot.carCtrlFlags.bAvoidLevelTransitions = false;
    m_nRemapTxd = -1;
    m_nPreviousRemapTxd = -1;
    m_pRemapTexture = nullptr;
    m_pOverheatParticle = nullptr;
    m_pFireParticle = nullptr;
    m_pDustParticle = nullptr;
    m_pCustomCarPlate = nullptr;
    m_anUpgrades.fill(-1);
    m_fWheelScale = 1.0f;
    m_nWindowsOpenFlags = 0;
    m_nNitroBoosts = 0;
    m_nHasslePosId = 0;
    m_nVehicleWeaponInUse = CAR_WEAPON_NOT_USED;
    m_fDirtLevel = (float)((CGeneral::GetRandomNumber() % 15));
    m_nCreationTime = CTimer::GetTimeInMS();
    SetCollisionLighting(tColLighting(0x48));
}

// 0x6E2B40
CVehicle::~CVehicle() {
    CReplay::RecordVehicleDeleted(this);
    m_nAlarmState = 0;
    DeleteRwObject(); // V1053 Calling the 'DeleteRwObject' virtual function in the destructor may lead to unexpected result at runtime.
    CRadar::ClearBlipForEntity(eBlipType::BLIP_CAR, GetVehiclePool()->GetRef(this));

    if (m_pDriver) {
        m_pDriver->FlagToDestroyWhenNextProcessed();
    }


    for (const auto passenger : GetPassengers()) {
        if (passenger) {
            passenger->FlagToDestroyWhenNextProcessed();
        }
    }

    if (m_pFire) {
        m_pFire->Extinguish();
        m_pFire = nullptr;
    }

    CCarCtrl::UpdateCarCount(this, true);
    if (vehicleFlags.bIsAmbulanceOnDuty) {
        --CCarCtrl::NumAmbulancesOnDuty;
        vehicleFlags.bIsAmbulanceOnDuty = false;
    }

    if (vehicleFlags.bIsFireTruckOnDuty) {
        --CCarCtrl::NumFireTrucksOnDuty;
        vehicleFlags.bIsFireTruckOnDuty = false;
    }

    if (m_vehicleSpecialColIndex > -1) {
        m_aSpecialColVehicle.data()[m_vehicleSpecialColIndex] = nullptr; // Como el original: sin comprobar el rango
        m_vehicleSpecialColIndex = -1;
    }

    // Como el original: los tres punteros se ponen a cero (antes se anulaba una copia)
    if (m_pOverheatParticle) {
        g_fxMan.DestroyFxSystem(m_pOverheatParticle);
        m_pOverheatParticle = nullptr;
    }
    if (m_pFireParticle) {
        g_fxMan.DestroyFxSystem(m_pFireParticle);
        m_pFireParticle = nullptr;
    }
    if (m_pDustParticle) {
        g_fxMan.DestroyFxSystem(m_pDustParticle);
        m_pDustParticle = nullptr;
    }

    if (m_pCustomCarPlate) {
        RwTextureDestroy(m_pCustomCarPlate);
        m_pCustomCarPlate = nullptr;
    }

    const auto iRopeInd = CRopes::FindRope(GetRopeID()); // Como el original: this + 0x1D (&m_nFlags + 1), no this + 1
    if (iRopeInd >= 0) {
        CRopes::GetRope(iRopeInd).Remove();
    }

    if (!physicalFlags.bRenderScorched && m_fHealth < 250.0F) {
        CDarkel::RegisterCarBlownUpByPlayer(*this, 0);
    }
}

// Como el original (0x6E2D50: mov ecx,[0xB74494]; jmp 0x6E2A50): size no se consulta; el pool real usa almacenamiento de 0xA18 bytes.
// CVehiclePool::New reconstruida se instala separadamente en 0x6E2A50. El EXE sale con ECX/EDX tal como los deja New (salto, no llamada), asi que
// la llamada a New tiene que ser lo ultimo que se ejecuta: sin comprobaciones de ejecucion (RTC) ni cookie /GS, que insertarian codigo tras ella.
#pragma runtime_checks("", off)
__declspec(safebuffers) void* CVehicle::operator new(unsigned size) {
    return GetVehiclePool()->New();
}
#pragma runtime_checks("", restore)

void CVehicle::operator delete(void* data) {
    GetVehiclePool()->Delete(static_cast<CVehicle*>(data));
}

void* CVehicle::operator new(unsigned size, int32 poolRef) {
    return GetVehiclePool()->NewAt(poolRef);
}

void CVehicle::operator delete(void* data, int32 poolRef) {
    GetVehiclePool()->Delete(static_cast<CVehicle*>(data));
}

// 0x6D6A40
void CVehicle::SetModelIndex(uint32 index) {
    CEntity::SetModelIndex(index);
    auto mi = CModelInfo::GetModelInfo(index)->AsVehicleModelInfoPtr();
    CustomCarPlate_TextureCreate(mi);
    for (auto i = 0u; i < std::size(m_anExtras); i++) {
        m_anExtras[i] = CVehicleModelInfo::ms_compsUsed[i];
    }
    m_nMaxPassengers = CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors(index);
    switch (m_nModelIndex) {
    case MODEL_RCBANDIT:
    case MODEL_RCBARON:
    case MODEL_RCRAIDER:
    case MODEL_RCGOBLIN:
    case MODEL_RCTIGER:
        vehicleFlags.bIsRCVehicle = true;
        break;
    default:
        // Como el original (0x6D6AAB): solo se borra bIsRCVehicle (and 0x7F); bCreatedAsPoliceVehicle no se toca
        vehicleFlags.bIsRCVehicle = false;
        break;
    }

    // Set up weapons
    switch (m_nModelIndex) {
    case MODEL_RUSTLER:
    case MODEL_STUNT:
        m_nVehicleWeaponInUse = CAR_WEAPON_HEAVY_GUN;
        break;
    case MODEL_BEAGLE:
        m_nVehicleWeaponInUse = CAR_WEAPON_FREEFALL_BOMB;
        break;
    case MODEL_HYDRA:
    case MODEL_TORNADO:
        m_nVehicleWeaponInUse = CAR_WEAPON_LOCK_ON_ROCKET;
        break;
    }
}

// 0x6D6410
void CVehicle::DeleteRwObject() {
    SetRemapTexDictionary(-1);
    RemoveAllUpgrades();
    CEntity::DeleteRwObject();
}

// 0x6D6640
void CVehicle::SpecialEntityPreCollisionStuff(CPhysical* colPhysical, bool bIgnoreStuckCheck, bool& bCollisionDisabled,
    bool& bCollidedEntityCollisionIgnored, bool& bCollidedEntityUnableToMove, bool& bThisOrCollidedEntityStuck) {
    if (colPhysical->GetIsTypePed()
        && colPhysical->AsPed()->bKnockedOffBike
        && colPhysical->AsPed()->m_pVehicle == this)
    {
        bCollisionDisabled = true;
        return;
    }

    // Como el original: el EXE prueba el bit 24 de las banderas fisicas (bDontCollideWithFlyers), no bSubmergedInWater
    if (physicalFlags.bDontCollideWithFlyers
        && GetStatus() != STATUS_PLAYER
        && (GetStatus() != STATUS_REMOTE_CONTROLLED && colPhysical->DoesNotCollideWithFlyers())) // BUG:? Seems like it should check for it being heli
    {
        bCollisionDisabled = true;
        return;
    }

    if (m_pEntityIgnoredCollision == colPhysical || colPhysical->m_pEntityIgnoredCollision == this) {
        bCollidedEntityCollisionIgnored = true;
        physicalFlags.bSkipLineCol = true;
        return;
    }

    if (m_pAttachedTo == colPhysical) {
        bCollidedEntityCollisionIgnored = true;
        return;
    }

    if (colPhysical->m_pAttachedTo == this) {
        bCollisionDisabled = true;
        physicalFlags.bSkipLineCol = true;
        return;
    }

    if (physicalFlags.bDisableCollisionForce && colPhysical->physicalFlags.bDisableCollisionForce) {
        bCollisionDisabled = true;
        return;
    }

    if (GetIsStuck()
        && colPhysical->GetIsTypeVehicle()
        && (colPhysical->AsVehicle()->physicalFlags.bDisableCollisionForce && !colPhysical->AsVehicle()->physicalFlags.bCollidable)
    ) {
        bCollidedEntityCollisionIgnored = true;
        physicalFlags.bSkipLineCol = true;
        return;
    }

    if (colPhysical->IsImmovable()) {
        if (bIgnoreStuckCheck)
            bCollidedEntityCollisionIgnored = true;
        else if (GetIsStuck() || colPhysical->GetIsStuck())
            bThisOrCollidedEntityStuck = true;

        return;
    }

    if (colPhysical->GetIsTypeObject())
    {
        if (colPhysical->AsObject()->IsFallenLampPost())
        {
            bCollisionDisabled = true;
            colPhysical->AsObject()->m_pEntityIgnoredCollision = this;
            return; // Como el original: salta a la comprobacion final, que con bCollisionDisabled no hace nada
        }
        else
        {
            if (colPhysical->IsModelTempCollision())
            {
                bCollisionDisabled = true;
                return;
            }

            if (colPhysical->AsObject()->IsTemporary()
                || colPhysical->AsObject()->IsExploded()
                || !colPhysical->GetIsStatic())
            {
                if (IsConstructionVehicle())
                {
                    if (GetIsStuck() || colPhysical->GetIsStuck())
                        bThisOrCollidedEntityStuck = true;
                }
                else if (!colPhysical->AsObject()->CanBeSmashed() && !IsBike())
                {
                    auto tempMat = CMatrix();
                    auto* cm = colPhysical->GetColModel();
                    auto& vecMax = cm->GetBoundingBox().m_vecMax;
                    if (vecMax.x < 1.0F && vecMax.y < 1.0F && vecMax.z < 1.0F)
                    {
                        const auto vecSize = cm->GetBoundingBox().GetSize();
                        const auto vecTransformed = VehEMatPoint(*colPhysical->m_matrix, vecSize); // Como el original: 0x59C890

                        if (GetPosition().z > vecTransformed.z)
                            bCollidedEntityCollisionIgnored = true;
                        else
                        {
                            Invert(*m_matrix, tempMat);
                            if (VehEMatPoint(tempMat, vecTransformed).z < 0.0F) // `m_matrix->GetUp().Dot(vecTransformed)` should work too
                                bCollidedEntityCollisionIgnored = true;
                        }
                    }
                }
            }

            if (!bCollidedEntityCollisionIgnored
                && !bCollisionDisabled
                && !bThisOrCollidedEntityStuck
                && colPhysical->GetIsStuck())
            {
                bCollidedEntityUnableToMove = true;
            }
            return;
        }
    }

    if (colPhysical->IsRCCar()) {
        bCollidedEntityCollisionIgnored = true;
        physicalFlags.bSkipLineCol = true;
        return;
    }

    if (IsRCCar() && (colPhysical->GetIsTypeVehicle() || colPhysical->GetIsTypePed())) {
        bCollidedEntityCollisionIgnored = true;
        physicalFlags.bSkipLineCol = true;
        return;
    }

    if (colPhysical == m_pTowingVehicle || colPhysical == m_pVehicleBeingTowed) {
        bThisOrCollidedEntityStuck = true;
        physicalFlags.bSkipLineCol = true;
        return;
    }

    if (colPhysical->GetIsStuck()) {
        bCollidedEntityUnableToMove = true;
        return;
    }
}

// 0x6D0E90
uint8 CVehicle::SpecialEntityCalcCollisionSteps(bool& bProcessCollisionBeforeSettingTimeStep, bool& unk2) {
    if (physicalFlags.bDisableCollisionForce) {
        return 1;
    }

    // Como el original: (|v|^2 * paso) * paso (no |v|^2 * sq(paso)); el limite es 0x871E34 (el siguiente float tras 0.16) y con NaN tambien devuelve 1
    const float fMoveSquared = (m_vecMoveSpeed.SquaredMagnitude() * CTimer::GetTimeStep()) * CTimer::GetTimeStep();
    if (!(fMoveSquared >= std::bit_cast<float>(0x3E23D70Bu))) {
        return 1;
    }

    float fMove = std::sqrt(fMoveSquared);
    if (!TreatAsPlayerForCollisions()) {
        if (fMoveSquared <= std::bit_cast<float>(0x3EA3D70Bu)) { // 0x871E30
            fMove *= (10.0F / 4.0F);
        } else {
            fMove *= (10.0F / 3.0F);
        }
    } else if (IsBike()) {
        fMove *= (10.0F / 1.5F);
    } else {
        fMove *= (10.0F / 2.0F);
    }

    // Como el original: ceil del CRT sobre el double del producto y _ftol2; se usa el byte bajo
    const auto steps = static_cast<uint8>(static_cast<int64>(std::ceil(static_cast<double>(fMove))));

    auto& bbox = CEntity::GetColModel()->GetBoundingBox();
    const auto& mat = *m_matrix;

    // Como el original: |dot * paso| se calcula como |dot| * paso / dimension, el dot suma z, y, x y el maximo queda con el primero si es estrictamente mayor
    auto fLongestDir = (std::fabs(VehEDot(m_vecMoveSpeed, mat.GetForward())) * CTimer::GetTimeStep()) / bbox.GetLength();
    const auto fWidthDir = (std::fabs(VehEDot(m_vecMoveSpeed, mat.GetRight())) * CTimer::GetTimeStep()) / bbox.GetWidth();
    if (!(fLongestDir > fWidthDir)) {
        fLongestDir = fWidthDir;
    }
    const auto fHeightDir = (std::fabs(VehEDot(m_vecMoveSpeed, mat.GetUp())) * CTimer::GetTimeStep()) / bbox.GetHeight();
    if (!(fLongestDir > fHeightDir)) {
        fLongestDir = fHeightDir;
    }

    if (IsBike()) {
        fLongestDir *= 1.5F;
    }

    if (fLongestDir < 1.0F) {
        bProcessCollisionBeforeSettingTimeStep = true;
    } else if (fLongestDir < 2.0F) {
        unk2 = true;
    }

    return steps;
}

// 0x6D6480
void CVehicle::PreRender() {
    if (!IsTrain())
        CalculateLightingFromCollision();

    PreRenderDriverAndPassengers();
    if (CModelInfo::GetModelInfo(m_nModelIndex)->m_n2dfxCount)
        CEntity::ProcessLightsForEntity();

    m_renderLights.m_bRightFront = false;
    m_renderLights.m_bLeftFront = false;
    m_renderLights.m_bRightRear = false;
    m_renderLights.m_bLeftRear = false;

    const auto fCoeff = CPhysical::GetLightingFromCol(false) * 0.4F;
    GetVehicleModelInfo()->SetEnvMapCoeff(fCoeff);
}

// 0x6D0E60
void CVehicle::Render() {
    auto* mi = GetVehicleModelInfo();
    const auto iDirtLevel = static_cast<int32>(m_fDirtLevel) & 0xF;
    CVehicleModelInfo::SetDirtTextures(mi, iDirtLevel);

    CEntity::Render();
}

// 0x553F20
bool CVehicle::SetupLighting() {
    ActivateDirectional();
    return CRenderer::SetupLightingForEntity(this);
}

// 0x5533D0
void CVehicle::RemoveLighting(bool bRemove) {
    if (!physicalFlags.bRenderScorched)
        CPointLights::RemoveLightsAffectingObject();

    SetAmbientColours();
    DeActivateDirectional();
}

// 0x6D56C0
void CVehicle::ProcessOpenDoor(CPed* ped, uint32 doorComponentId_, uint32 animGroup, uint32 animId, float fTime) {
    auto doorComponentId = (int32)doorComponentId_; // silence warns, todo: OpenDoor receives int32, why?
    eDoors iCheckedDoor = [&] {
        switch (doorComponentId) {
        case COMPONENT_DOOR_RF: return DOOR_RIGHT_FRONT;
        case COMPONENT_DOOR_LR: return DOOR_RIGHT_REAR;
        case COMPONENT_DOOR_RR: return DOOR_LEFT_FRONT;
        case COMPONENT_WING_LF: return DOOR_LEFT_REAR;
        default:
            // Como el original: usa los bits del float fTime como numero de puerta (sin assert)
            return static_cast<eDoors>(std::bit_cast<uint32>(fTime));
        }
    }();

    if (IsDoorMissing(iCheckedDoor)) {
        return;
    }

    const auto group = static_cast<AssocGroupId>(m_pHandlingData->m_nAnimGroup);
    float fAnimStart, fAnimEnd;
    switch (animId) {
    case ANIM_ID_CAR_OPEN_LHS:
    case ANIM_ID_CAR_OPEN_RHS:
    case ANIM_ID_CAR_OPEN_LHS_1:
    case ANIM_ID_CAR_OPEN_RHS_1: {
        CVehicleAnimGroupData::GetInOutTimings(group, eInOutTimingMode::OPEN_START, &fAnimStart, &fAnimEnd);
        if (fTime < fAnimStart) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 0.0F, false);
        } else if (fTime > fAnimEnd) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 1.0F, true);
        } else if (fTime > fAnimStart && fTime < fAnimEnd) {
            const auto fNewRatio = invLerp(fAnimStart, fAnimEnd, fTime);
            const auto fCurRatio = GetDooorAngleOpenRatio(iCheckedDoor);
            if (fCurRatio < fNewRatio) {
                OpenDoor(ped, doorComponentId, iCheckedDoor, fNewRatio, true);
            }
        }

        return;
    }
    case ANIM_ID_CAR_CLOSE_LHS_0:
    case ANIM_ID_CAR_CLOSE_RHS_0:
    case ANIM_ID_CAR_CLOSE_LHS_1:
    case ANIM_ID_CAR_CLOSE_RHS_1: {
        // Como el original (0x6D5940): sin comparar con la apertura actual (eso lo hace CLOSEDOOR)
        CVehicleAnimGroupData::GetInOutTimings(group, eInOutTimingMode::CLOSE_STOP, &fAnimStart, &fAnimEnd);
        if (fTime < fAnimStart) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 1.0F, true);
        } else if (fTime > fAnimEnd) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 0.0F, true);
        } else if (fTime > fAnimStart && fTime < fAnimEnd) {
            const auto fNewRatio = 1.0F - invLerp(fAnimStart, fAnimEnd, fTime);
            OpenDoor(ped, doorComponentId, iCheckedDoor, fNewRatio, true);
        }
        return;
    }
    case ANIM_ID_CAR_CLOSEDOOR_LHS_0:
    case ANIM_ID_CAR_CLOSEDOOR_RHS_0:
    case ANIM_ID_CAR_CLOSEDOOR_LHS_1:
    case ANIM_ID_CAR_CLOSEDOOR_RHS_1: {
        // Como el original (0x6D57E2): solo cierra si la apertura actual es mayor que la nueva
        CVehicleAnimGroupData::GetInOutTimings(group, eInOutTimingMode::OPEN_STOP, &fAnimStart, &fAnimEnd);
        if (fTime < fAnimStart) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 1.0F, true);
        } else if (fTime > fAnimEnd) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 0.0F, true);
        } else if (fTime > fAnimStart && fTime < fAnimEnd) {
            const auto fNewRatio = 1.0F - invLerp(fAnimStart, fAnimEnd, fTime);
            const auto fCurRatio = GetDooorAngleOpenRatio(iCheckedDoor);
            if (fCurRatio > fNewRatio) {
                OpenDoor(ped, doorComponentId, iCheckedDoor, fNewRatio, true);
            }
        }
        return;
    }
    case ANIM_ID_CAR_GETOUT_LHS_0:
    case ANIM_ID_CAR_GETOUT_RHS_0:
    case ANIM_ID_CAR_GETOUT_LHS_1:
    case ANIM_ID_CAR_GETOUT_RHS_1: {
        CVehicleAnimGroupData::GetInOutTimings(group, eInOutTimingMode::CLOST_START, &fAnimStart, &fAnimEnd);
        if (fTime < fAnimStart) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 0.0F, true);
        } else if (fTime > fAnimEnd) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 1.0F, true);
        } else if (fTime > fAnimStart && fTime < fAnimEnd) {
            const auto fNewRatio = invLerp(fAnimStart, fAnimEnd, fTime);
            const auto fCurRatio = GetDooorAngleOpenRatio(iCheckedDoor);
            if (fCurRatio < fNewRatio) {
                OpenDoor(ped, doorComponentId, iCheckedDoor, fNewRatio, true);
            }
        }
        return;
    }
    case ANIM_ID_CAR_JACKEDLHS:
    case ANIM_ID_CAR_JACKEDRHS:
        if (fTime < 0.1F) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 0.0F, true);
        } else if (fTime > 0.4F) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 1.0F, true);
        } else if (fTime > 0.1F && fTime < 0.4F) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, (fTime - 0.1F) * (10.0F / 3.0F), true);
        }
        return;

    case ANIM_ID_CAR_FALLOUT_LHS:
    case ANIM_ID_CAR_FALLOUT_RHS:
        if (fTime < 0.1F) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 0.0F, true);
        } else if (fTime > 0.4F) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, 1.0F, true);
        } else if (fTime > 0.1F && fTime < 0.4F) {
            OpenDoor(ped, doorComponentId, iCheckedDoor, (fTime - 0.1F) * (10.0F / 3.0F), true);
        }

        return;

    case ANIM_ID_CAR_PULLOUT_LHS:
    case ANIM_ID_CAR_PULLOUT_RHS:
    case ANIM_ID_UNKNOWN_15:
        switch (animGroup) {
        case ANIM_GROUP_STDCARAMIMS:
        case ANIM_GROUP_LOWCARAMIMS:
        case ANIM_GROUP_TRKCARANIMS:
        case ANIM_GROUP_COACHCARANIMS:
        case ANIM_GROUP_BUSCARANIMS:
        case ANIM_GROUP_CONVCARANIMS:
        case ANIM_GROUP_MTRKCARANIMS:
        case ANIM_GROUP_STDTALLCARAMIMS:
        case ANIM_GROUP_BFINJCARAMIMS:
            OpenDoor(ped, doorComponentId, iCheckedDoor, 1.0F, true);
        }
        return;

    case ANIM_ID_CAR_ROLLOUT_LHS:
    case ANIM_ID_CAR_ROLLOUT_RHS: {
        switch (animGroup) {
        case ANIM_GROUP_STDCARAMIMS:
        case ANIM_GROUP_LOWCARAMIMS:
        case ANIM_GROUP_TRKCARANIMS:
        case ANIM_GROUP_RUSTPLANEANIMS:
        case ANIM_GROUP_COACHCARANIMS:
        case ANIM_GROUP_CONVCARANIMS:
        case ANIM_GROUP_MTRKCARANIMS:
        case ANIM_GROUP_STDTALLCARAMIMS:
        case ANIM_GROUP_BFINJCARAMIMS:
            fAnimStart = 0.01F;
            fAnimEnd = 0.1F;
            break;
        default:
            NOTSA_UNREACHABLE();
        }

        if (fTime < fAnimStart)
            OpenDoor(ped, doorComponentId, iCheckedDoor, 0.0F, true);
        else if (fTime > fAnimEnd)
            OpenDoor(ped, doorComponentId, iCheckedDoor, 1.0F, true);
        else if (fTime > fAnimStart && fTime < fAnimEnd) {
            const auto fNewRatio = invLerp(fAnimStart, fAnimEnd, fTime);
            const auto fCurRatio = GetDooorAngleOpenRatio(iCheckedDoor);
            if (fCurRatio < fNewRatio) {
                OpenDoor(ped, doorComponentId, iCheckedDoor, fNewRatio, true);
            }
        }

        return;
    }

    case ANIM_ID_CAR_ROLLDOOR: {
        float fAnimEnd2;
        switch (animGroup) {
        case ANIM_GROUP_STDCARAMIMS:
        case ANIM_GROUP_LOWCARAMIMS:
        case ANIM_GROUP_TRKCARANIMS:
        case ANIM_GROUP_RUSTPLANEANIMS:
        case ANIM_GROUP_COACHCARANIMS:
        case ANIM_GROUP_CONVCARANIMS:
        case ANIM_GROUP_MTRKCARANIMS:
        case ANIM_GROUP_STDTALLCARAMIMS:
        case ANIM_GROUP_BFINJCARAMIMS:
            fAnimStart = 0.05f;
            fAnimEnd = 0.3f;
            fAnimEnd2 = 0.475f;
            break;
        default:
            NOTSA_UNREACHABLE();
        }

        if (fTime > fAnimEnd2)
            OpenDoor(ped, doorComponentId, iCheckedDoor, 0.0F, true);
        else if (fTime > fAnimStart && fTime < fAnimEnd) {
            const auto fNewRatio = invLerp(fAnimStart, 0.2F, fTime);
            const auto fCurRatio = GetDooorAngleOpenRatio(iCheckedDoor);
            if (fCurRatio < fNewRatio) {
                OpenDoor(ped, doorComponentId, iCheckedDoor, fNewRatio, true);
            }
        } else if (fTime > fAnimEnd && fTime < fAnimEnd2) {
            const auto fNewRatio = 1.0F - invLerp(fAnimEnd, fAnimEnd2, fTime);
            const auto fCurRatio = GetDooorAngleOpenRatio(iCheckedDoor);
            if (fCurRatio > fNewRatio)
                OpenDoor(ped, doorComponentId, iCheckedDoor, fNewRatio, true);
        }

        return;
    }
    }
}

// 0x6DF4A0
void CVehicle::ProcessDrivingAnims(CPed* driver, bool blend) {
    if (m_bOffscreen || !driver->IsPlayer())
        return;

    auto* radioTuneAnim = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_CAR_TUNE_RADIO);
    if (blend) {
        radioTuneAnim = CAnimManager::BlendAnimation(driver->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_CAR_TUNE_RADIO, 4.0F);
    }

    if (radioTuneAnim)
        return;

    CRideAnims* usedAnims;
    if (vehicleFlags.bLowVehicle)
        usedAnims = &aDriveAnimIdsLow;
    else if (IsBoat() && !m_pHandlingData->m_bSitInBoat)
        usedAnims = &aDriveAnimIdsBoat;
    else if (CVehicleAnimGroupData::UsesKartDrivingAnims(static_cast<AssocGroupId>(m_pHandlingData->m_nAnimGroup)))
        usedAnims = &aDriveAnimIdsKart;
    else if (CVehicleAnimGroupData::UsesTruckDrivingAnims(static_cast<AssocGroupId>(m_pHandlingData->m_nAnimGroup)))
        usedAnims = &aDriveAnimIdsTruck;
    else {
        // Preserve the executable's x87 rounding and stat-read order: fsqrt((x*x + y*y) + z*z) (0x6DF504..0x6DF52D), stored as float.
        const float fSpeed = DestroyCarDistanceCpp(&m_vecMoveSpeed.x);
        if (CStats::GetStatValue(eStats::STAT_DRIVING_SKILL) >= 100.0F) {
            usedAnims = fSpeed <= 0.4F ? &aDriveAnimIdsProSlow : &aDriveAnimIdsPro;
        } else if (CStats::GetStatValue(eStats::STAT_DRIVING_SKILL) >= 50.0F) {
            usedAnims = fSpeed <= 0.4F ? &aDriveAnimIdsStdSlow : &aDriveAnimIdsStd;
        } else {
            usedAnims = fSpeed <= 0.4F ? &aDriveAnimIdsBadSlow : &aDriveAnimIdsBad;
        }
    }

    // 0x6DF620
    auto* idleAnim      = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), usedAnims->idle);
    auto* lookLeftAnim  = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), usedAnims->left);
    auto* lookRightAnim = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), usedAnims->right);
    auto* lookBackAnim  = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), usedAnims->back);

    if (!idleAnim) {
        if (RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_CAR_SIT)) {
            CAnimManager::BlendAnimation(driver->GetRpClump(), ANIM_GROUP_DEFAULT, usedAnims->idle, 4.0F);
        }
        return;
    }

    if (idleAnim->m_BlendAmount < 1.0F)
        return;

    if (lookBackAnim
        && CCamera::GetActiveCamera().m_nMode == eCamMode::MODE_1STPERSON
        && CCamera::GetActiveCamera().m_nDirectionWasLooking == eLookingDirection::LOOKING_DIRECTION_BEHIND
    ) {
        lookBackAnim->m_BlendDelta = -1000.0F;
    }

    // 0x6DF6DA
    auto* driveByAnim = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_DRIVEBY_L);
    if (!driveByAnim) driveByAnim = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_DRIVEBY_R);
    if (!driveByAnim) driveByAnim = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_DRIVEBYL_L);
    if (!driveByAnim) driveByAnim = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_DRIVEBYL_R);

    if (!vehicleFlags.bLowVehicle
        && m_GasPedal < 0.0F
        && !driveByAnim
        && GetVehicleAppearance() != VEHICLE_APPEARANCE_HELI
        && GetVehicleAppearance() != VEHICLE_APPEARANCE_PLANE
    ) {
        if ((CCamera::GetActiveCamera().m_nMode != eCamMode::MODE_1STPERSON
            || CCamera::GetActiveCamera().m_nDirectionWasLooking != eLookingDirection::LOOKING_DIRECTION_BEHIND)
            && (!lookBackAnim || lookBackAnim->m_BlendAmount < 1.0F && lookBackAnim->m_BlendDelta <= 0.0F)
        ) {
            CAnimManager::BlendAnimation(driver->GetRpClump(), ANIM_GROUP_DEFAULT, usedAnims->back, 4.0F);
        }
        return;
    }

    if (m_fSteerAngle == 0.0F || driveByAnim) {
        if (lookLeftAnim)  lookLeftAnim->m_BlendDelta  = -4.0F;
        if (lookRightAnim) lookRightAnim->m_BlendDelta = -4.0F;
        if (lookBackAnim)  lookBackAnim->m_BlendDelta  = -4.0F;
        return;
    }

    const auto angle = m_fSteerAngle;
    const float scale = angle < 0.0F ? -0x1.a3ac1p+0F : 0x1.a3ac1p+0F;
    float fUsedAngle = X87TimesFloatCpp(angle, scale); // fld angle; fmul [0x871FD0/0x871FD4]
    fUsedAngle = std::clamp(fUsedAngle, 0.0F, 1.0F);

    if (m_fSteerAngle < 0) {
        if (lookLeftAnim) {
            lookLeftAnim->m_BlendAmount = 0.0F;
            lookLeftAnim->m_BlendDelta  = 0.0F;
        }
        if (lookRightAnim) {
            lookRightAnim->m_BlendAmount = fUsedAngle;
            lookRightAnim->m_BlendDelta  = 0.0F;
        } else {
            CAnimManager::BlendAnimation(driver->GetRpClump(), ANIM_GROUP_DEFAULT, usedAnims->right, 4.0F);
        }
    } else {
        if (lookRightAnim) {
            lookRightAnim->m_BlendAmount = 0.0f;
            lookRightAnim->m_BlendDelta = 0.0F;
        }
        if (lookLeftAnim) {
            lookLeftAnim->m_BlendAmount = fUsedAngle;
            lookLeftAnim->m_BlendDelta = 0.0F;
        } else {
            CAnimManager::BlendAnimation(driver->GetRpClump(), ANIM_GROUP_DEFAULT, usedAnims->left, 4.0F);
        }
    }

    if (lookBackAnim) {
        lookBackAnim->m_BlendDelta = -4.0F;
    }
}

// 0x871F54// 0x6D63F0
float CVehicle::GetHeightAboveRoad() {
    return CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundingBox().m_vecMin.z * -1.0F;
}

// 0x6D1F30
bool CVehicle::CanPedStepOutCar(bool bIgnoreSpeedUpright) const {
    auto const fUpZ = m_matrix->GetUp().z;
    if (std::fabs(fUpZ) <= 0.1F) {
        if (std::fabs(m_vecMoveSpeed.z) > 0.05F || m_vecMoveSpeed.Magnitude2D() > 0.01F || m_vecTurnSpeed.SquaredMagnitude() > 0.0004F) { // 0.02F / 50.0f
            return false;
        }
        return true;
    }

    if (IsBoat())
        return true;

    // Como el original: devuelve verdadero cuando la velocidad de giro NO supera el limite (con bIgnoreSpeedUpright; la DLL
    // devolvia lo contrario) y las comparaciones son "no mayor que" (fcomp + test ah,41h): un NaN cuenta como dentro del limite.
    if (bIgnoreSpeedUpright)
        return !(m_vecTurnSpeed.SquaredMagnitude() > 0.0004F);

    return !(m_vecMoveSpeed.Magnitude2D() > 0.01F) &&
           !(std::fabs(m_vecMoveSpeed.z) > 0.05F) &&
           !(m_vecTurnSpeed.SquaredMagnitude() > 0.0004F);
}

// 0x6D2030
bool CVehicle::CanPedJumpOutCar(CPed* ped) {
    // Como el original: las comparaciones son las del x87 (fcomp + test ah,5 / 41h), de modo que un NaN no se trata como "menor".
    if (IsBike()) {
        const auto* const passenger0 = m_apPassengers.data()[0];
        if (passenger0 && ped != passenger0)
            return false;

        return !(m_vecMoveSpeed.SquaredMagnitude2D() < 0.07F);
    }

    const auto fHorSpeedSquared = m_vecMoveSpeed.SquaredMagnitude2D();
    bool bInTheAir = IsSubPlane() || IsSubHeli();
    if (!bInTheAir && IsAutomobile()) {
        bInTheAir = m_matrix->GetUp().z < 0.3F && m_nLastCollisionTime > CTimer::GetTimeInMS() - 1000;
    }

    if (!bInTheAir) {
        if (fHorSpeedSquared < 0.1F)
            return false;

        return !(fHorSpeedSquared > 0.5F);
    }

    if (!(fHorSpeedSquared < 0.1F))
        return true;

    if (CanPedStepOutCar(false))
        return false;

    m_vecTurnSpeed *= 0.9F; // BUG(PRONE): We are modifying vehicle move and turn speeds in function that seems that it should be const
    const auto fMoveSpeedSquared = m_vecMoveSpeed.SquaredMagnitude();
    const auto fTimeStep = CTimer::GetTimeStep();
    // Como el original: 0.01f * |v|^2 frente a ts*ts*6.4000007e-05f (0x871E38 = 0.008f^2 en float), sin dividir entre 100.
    if (fTimeStep * fTimeStep * std::bit_cast<float>(0x388637BEu) < 0.01F * fMoveSpeedSquared) {
        m_vecMoveSpeed *= 0.9F;
        return false;
    }

    auto fMoveMult = fTimeStep / std::sqrt(fMoveSpeedSquared) * 0.016F;
    fMoveMult = 1.0F - fMoveMult;
    if (0.0F > fMoveMult) // el EXE no toca un NaN
        fMoveMult = 0.0F;
    m_vecMoveSpeed *= fMoveMult;
    return false;
}

// Como el original (0x59C890, CMatrix * CVector): x = ((at*z + up*y) + right*x) + pos; y y z = ((at*z + right*x) + up*y) + pos.
// TransformPoint suma en otro orden.
static CVector CVehTowTransformPoint(const CMatrix& m, const CVector& v) {
    const float x = (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x + m.GetPosition().x;
    const float y = (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y + m.GetPosition().y;
    const float z = (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y + m.GetPosition().z;
    return { x, y, z };
}

// 0x871F6C// 0x6DFB70
bool CVehicle::GetTowHitchPos(CVector& outPos, bool bCheckModelInfo, CVehicle* vehicle) {
    if (!bCheckModelInfo)
        return false;

    auto const fColFront = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundingBox().m_vecMax.y;
    outPos.Set(0.0F, fColFront + 1.0F, 0.0F);
    outPos = CVehTowTransformPoint(*m_matrix, outPos);
    return true;
}

// 0x871F70// 0x6DFBE0
bool CVehicle::GetTowBarPos(CVector& outPos, bool bCheckModelInfo, CVehicle* vehicle) {
    if (!bCheckModelInfo)
        return false;

    auto const fColRear = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundingBox().m_vecMin.y;
    outPos.Set(0.0F, fColRear - 1.0F, 0.0F);
    outPos = CVehTowTransformPoint(*m_matrix, outPos);
    return true;
}

// 0x871F80// 0x5D4760
bool CVehicle::Save() {
    uint32 size = sizeof(CVehicleSaveStructure);
    CVehicleSaveStructure data;
    data.Construct(this);
    CGenericGameStorage::SaveDataToWorkBuffer(&size, sizeof(uint32)); // Unused, game ignores it on load and uses const value
    CGenericGameStorage::SaveDataToWorkBuffer(&data, size);
    return true;
}

// 0x871F84// 0x5D2900
bool CVehicle::Load() {
    uint32 size;
    CVehicleSaveStructure data;
    CGenericGameStorage::LoadDataFromWorkBuffer(&size, sizeof(uint32));
    CGenericGameStorage::LoadDataFromWorkBuffer(&data, sizeof(CVehicleSaveStructure)); // BUG: Should use the value readen line above this, not constant
    data.Extract(this);
    return true;
}

// 0x6D0B40
void CVehicle::Shutdown() {
    for (auto& specialColModel : m_aSpecialColModel) {
        if (specialColModel.m_pColData) {
            specialColModel.RemoveCollisionVolumes();
        }
    }
}

// -1 if no remap index
// 0x6D0B70
int32 CVehicle::GetRemapIndex() {
    // Como el original (0x6D0B70): el modelo se lee sin comprobar tipo; GetNumRemaps se llama una vez al empezar y otra
    // por cada iteración fallida (no dos veces seguidas); el índice del bucle es un int16 (movsx) y no se comprueba el rango.
    auto* mi = CModelInfo::GetModelInfo(m_nModelIndex)->AsVehicleModelInfoPtr();
    int32 i = 0;
    if (mi->GetNumRemaps() > 0) {
        do {
            if (mi->m_anRemapTxds.data()[(int16)i] == m_nPreviousRemapTxd) {
                return i;
            }
            ++i;
        } while (i < mi->GetNumRemaps());
    }
    return -1;
}

// 0x6D0BC0
void CVehicle::SetRemapTexDictionary(int32 txdId) {
    if (txdId != m_nPreviousRemapTxd) {
        if (txdId == -1) {
            m_pRemapTexture = nullptr;
            CTxdStore::RemoveRef(m_nPreviousRemapTxd);
            m_nPreviousRemapTxd = -1;
        }
        m_nRemapTxd = txdId;
    }
}

// index for m_awRemapTxds[] array
// 0x6D0C00
void CVehicle::SetRemap(int32 remapIndex) {
    if (remapIndex == -1) {
        SetRemapTexDictionary(-1);
    } else {
        // Como el original (0x6D0C00): el índice se trunca a int16 (movsx) y no se comprueba el rango
        auto const infoRemapInd = GetVehicleModelInfo()->m_anRemapTxds.data()[(int16)remapIndex];
        SetRemapTexDictionary(infoRemapInd);
    }
}

// 0x6D0CA0
void CVehicle::SetCollisionLighting(tColLighting lighting) {
    std::ranges::fill(m_anCollisionLighting, lighting);
}

// 0x6D0CC0
void CVehicle::UpdateLightingFromStoredPolys() {
    m_anCollisionLighting[0] = m_FrontCollPoly.ligthing;
    m_anCollisionLighting[1] = m_FrontCollPoly.ligthing;

    m_anCollisionLighting[2] = m_RearCollPoly.ligthing;
    m_anCollisionLighting[3] = m_RearCollPoly.ligthing;
}

// Como el original (0x59C890, CMatrix * CVector, usado por el grupo D de CVehicle): x = ((at*z + up*y) + right*x) + pos;
// y y z = ((at*z + right*x) + up*y) + pos. TransformPoint suma en otro orden.
static CVector CVehDMatPoint(const CMatrix& m, const CVector& v) {
    const float x = (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x + m.GetPosition().x;
    const float y = (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y + m.GetPosition().y;
    const float z = (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y + m.GetPosition().z;
    return { x, y, z };
}

// 0x6D0CF0
void CVehicle::CalculateLightingFromCollision() {
    // Como el original: GetCurrentLighting en linea con 1/30 (0x858F10) y (dia*k)*(1 - b) + (noche*k)*b, suma acumulada y * 0.25
    const float balance = CCustomBuildingDNPipeline::m_fDNBalanceParam;
    const float k = std::bit_cast<float>(0x3D088889u);
    float fAvgLight = 0.0F;
    for (auto i = 0; i < 4; i++) {
        const auto lighting = m_anCollisionLighting[i].value;
        const float term = (static_cast<float>(lighting & 0xF) * k) * (1.0F - balance)
                         + (static_cast<float>(lighting >> 4) * k) * balance;
        fAvgLight = i == 0 ? term : fAvgLight + term;
    }

    m_fContactSurfaceBrightness = fAvgLight * 0.25F;
}

// 0x6D0E20
void CVehicle::ResetAfterRender() {
    RwRenderStateSet(RwRenderState::rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
    CVehicleModelInfo::ResetEditableMaterials((RpClump*)GetRwObject());

    if (IsAutomobile()) {
        auto* const mi = GetVehicleModelInfo();
        assert(mi != nullptr);
        AsAutomobile()->CustomCarPlate_AfterRenderingStop(mi);
    }
}

// Native ProcessDrivingAnims keeps ECX and EDX live across both calls. The C++ ABI
// allows them to be clobbered, so preserve them at the executable boundary: the EXE function (0x6D1080) only
// writes EAX, so on exit ECX = this, EDX = entry EDX and EAX = result. ResidueRegsCpp is the last call (see
// contrib/audit_register_residue.py, object Vehicle); no RTC / GS cookie after it.
#pragma runtime_checks("", off)
__declspec(safebuffers) eVehicleAppearance __fastcall CVehicle::GetVehicleAppearance_Reversed(const CVehicle* self, void* entryEdx) {
    const uint32 result = static_cast<uint32>(self->GetVehicleAppearance());
    return static_cast<eVehicleAppearance>(ResidueRegsCpp(static_cast<uint32>(reinterpret_cast<uintptr_t>(self)), static_cast<uint32>(reinterpret_cast<uintptr_t>(entryEdx)), result));
}
#pragma runtime_checks("", restore)

// 0x6D1080
eVehicleAppearance CVehicle::GetVehicleAppearance() const {
    uint32 flags = (
        m_pHandlingData->m_nModelFlags &
        (
            VEHICLE_HANDLING_MODEL_IS_BOAT |
            VEHICLE_HANDLING_MODEL_IS_PLANE |
            VEHICLE_HANDLING_MODEL_IS_HELI |
            VEHICLE_HANDLING_MODEL_IS_BIKE
        )
    );

    if (flags <= VEHICLE_HANDLING_MODEL_IS_HELI) {
        switch (flags) {
        case VEHICLE_HANDLING_MODEL_IS_HELI: return VEHICLE_APPEARANCE_HELI;
        case VEHICLE_HANDLING_MODEL_NONE:    return VEHICLE_APPEARANCE_AUTOMOBILE;
        case VEHICLE_HANDLING_MODEL_IS_BIKE: return VEHICLE_APPEARANCE_BIKE;
        default:                             return VEHICLE_APPEARANCE_NONE;
        }
    }

    if (flags == VEHICLE_HANDLING_MODEL_IS_PLANE)
        return VEHICLE_APPEARANCE_PLANE;

    if (flags != VEHICLE_HANDLING_MODEL_IS_BOAT)
        return VEHICLE_APPEARANCE_NONE;

    return VEHICLE_APPEARANCE_BOAT;
}

// returns false if vehicle model has no car plate material
// 0x6D10E0
bool CVehicle::CustomCarPlate_TextureCreate(CVehicleModelInfo* model) {
    m_pCustomCarPlate = nullptr;

    if (!model->m_pPlateMaterial) {
        return false;
    }

    if (const auto text = model->GetCustomCarPlateText()) {
        m_pCustomCarPlate = CCustomCarPlateMgr::CreatePlateTexture(text, model->m_nPlateType);
        model->SetCustomCarPlateText(nullptr); // Como el original (0x6D111D): borra el texto del modelo (push 0), no lo vuelve a poner
        model->m_nPlateType = -1;
    } else {
        m_pCustomCarPlate = RpMaterialGetTexture(model->m_pPlateMaterial);
        RwTextureAddRef(m_pCustomCarPlate);
    }

    return true;
}

// 0x6D1150
void CVehicle::CustomCarPlate_TextureDestroy() {
    if (m_pCustomCarPlate) {
        RwTextureDestroy(m_pCustomCarPlate);
        m_pCustomCarPlate = nullptr;
    }
}

// 0x6D1180
bool CVehicle::CanBeDeleted() {
    if (m_nNumGettingIn || m_nGettingOutFlags)
        return false;

    if (m_pDriver) {
        if (m_pDriver->IsCreatedByMission())
            return false;

        if (!m_pDriver->IsStateDriving() && !m_pDriver->IsStateDead())
            return false;
    }

    for (auto passenger : m_apPassengers) {
        if (passenger) {
            if (passenger->IsCreatedByMission())
                return false;

            if (!passenger->IsStateDriving() && !passenger->IsStateDead()) // OG: checked twice
                return false;
        }
    }

    switch (GetCreatedBy()) {
    case MISSION_VEHICLE:
    case PERMANENT_VEHICLE:
        return false;
    default:
        return true;
    }
}

// 0x6D1230
float CVehicle::ProcessWheelRotation(tWheelState wheelState, const CVector& arg1, const CVector& arg2, float arg3) {
    if (wheelState == WHEEL_STATE_SPINNING)
        return -1.1f;

    if (wheelState == WHEEL_STATE_FIXED)
        return 0.0f;

    // Como el original: producto escalar sumado en orden z, y, x
    const float angle = ((arg1.z * arg2.z + arg1.y * arg2.y) + arg1.x * arg2.x) / arg3;
    return -angle;
}

// 0x6D1280
bool CVehicle::CanVehicleBeDamaged(CEntity* damager, eWeaponType weapon, bool& bDamagedDueToFireOrExplosionOrBullet) {
    if (!vehicleFlags.bCanBeDamaged)
        return false;

    // Como el original (0x6D1280): FindPlayerPed/FindPlayerVehicle se piden solo cuando hacen falta (y FindPlayerPed hasta tres veces).
    if (GetStatus() != STATUS_PLAYER && physicalFlags.bInvulnerable) {
        if (damager != FindPlayerPed() && damager != FindPlayerVehicle()) {
            return false;
        }
    }

    if (FindPlayerPed()) {
        if (damager == FindPlayerPed() && FindPlayerPed()->m_pAttachedTo == this) {
            return false;
        }
    }

    bDamagedDueToFireOrExplosionOrBullet = false;
    if (!CanPhysicalBeDamaged(weapon, &bDamagedDueToFireOrExplosionOrBullet)) {
        return false;
    }

    // Como el original: "no menor que 250" (un NaN en la salud no impide el dano).
    return !bDamagedDueToFireOrExplosionOrBullet || GetStatus() != STATUS_PLAYER || !(m_fHealth < 250.0f);
}

// 0x6D1340
void CVehicle::ProcessDelayedExplosion() {
    // Como el original: el temporizador se trata como uint16 (movzx + ja) y el periodo es un entero de 32 bits (_ftol2),
    // no un int16; si el temporizador es mayor que el periodo se resta, si no se pone a cero.
    const uint16 timer = (uint16)m_wBombTimer;
    if (!timer) {
        return;
    }

    const int32 period = (int32)(int64)(CTimer::GetTimeStep() * (100.0f / 6.0f));
    if ((uint32)timer > (uint32)period) {
        m_wBombTimer = (int16)(uint16)((uint32)timer - (uint32)period);
    } else {
        m_wBombTimer = 0;
    }

    if (!(uint16)m_wBombTimer) {
        BlowUpCar(m_pWhoDetonatedMe, false);
    }
}

// NOTSA
void CVehicle::ApplyTurnForceToOccupantOnEntry(CPed* passenger) {
    // Como el original (0x6D13BE / 0x6D1503 / 0x6D1463): las constantes se multiplican en float (-0.02f, -0.1f, -0.2f), no se divide.
    switch (m_nVehicleType) {
    case VEHICLE_TYPE_BIKE: {
        const auto& up  = m_matrix->GetUp();
        const auto& fwd = m_matrix->GetForward();
        const auto  k   = passenger->m_fMass * -0.02f;
        ApplyTurnForce(
            CVector{ k * up.x, k * up.y, k * up.z },
            CVector{ fwd.x * -0.1f, fwd.y * -0.1f, fwd.z * -0.1f } // Behind the bike
        );
        break;
    }
    default: {
        const auto dy = passenger->m_matrix->GetPosition().y - m_matrix->GetPosition().y;
        const auto dx = passenger->m_matrix->GetPosition().x - m_matrix->GetPosition().x;
        ApplyTurnForce(
            CVector{ 0.f, 0.f, passenger->m_fMass * -0.2f },
            CVector{ dx, dy, 0.f }
        );
        break;
    }
    }
}

// 0x6D13A0
bool CVehicle::AddPassenger(CPed* passenger) {
    ApplyTurnForceToOccupantOnEntry(passenger);

    // Como el original: el bucle llega hasta m_nMaxPassengers (sin limitarse a los 8 del array).
    for (int32 i = 0; i < (int32)m_nMaxPassengers; i++) {
        CPed*& seat = m_apPassengers.data()[i];
        if (!seat) {
            seat = passenger;
            CEntity::RegisterReference(seat);
            m_nNumPassengers++;
            return true;
        }
    }

    // No empty seats
    return false;
}

// 0x6D14D0
bool CVehicle::AddPassenger(CPed* passenger, uint8 seatIdx) {
    if (vehicleFlags.bIsBus) {
        return AddPassenger(passenger);
    }

    // Como el original: la fuerza de giro se aplica siempre, antes de comprobar el asiento.
    ApplyTurnForceToOccupantOnEntry(passenger);

    // Check if seat is valid
    if (seatIdx >= m_nMaxPassengers) {
        return false;
    }

    // Check if anyone is already in that seat
    CPed*& seat = m_apPassengers.data()[seatIdx];
    if (seat) {
        return false;
    }

    // Place passenger into seat, and add ref
    seat = passenger;
    CEntity::RegisterReference(seat);
    m_nNumPassengers++;

    return true;
}

// 0x6D1610
void CVehicle::RemovePassenger(CPed* passenger) {
    if (!passenger) {
        return;
    }

    // Como el original: el tren recorre los 8 asientos y el resto m_nMaxPassengers (sin limitarlo a 8, por eso .data()[]);
    // al encontrarlo limpia la referencia, vacia el asiento y baja el contador (byte, sin comprobar que sea > 0).
    const uint32 numSeats = IsTrain() ? 8u : (uint32)m_nMaxPassengers;
    for (uint32 seat = 0; seat < numSeats; seat++) {
        if (m_apPassengers.data()[seat] == passenger) {
            CEntity::SafeCleanUpRef(m_apPassengers.data()[seat]);
            m_apPassengers.data()[seat] = nullptr;
            m_nNumPassengers--;
            return;
        }
    }
}

// 0x6D16A0
void CVehicle::SetDriver(CPed* driver) {
    CEntity::ChangeEntityReference(m_pDriver, driver);

    if (vehicleFlags.bFreebies && driver == FindPlayerPed()) {
        vehicleFlags.bFreebies = false;

        switch (m_nModelIndex)
        {
        case MODEL_AMBULAN: {
            FindPlayerInfo(0).AddHealth(20);
            break;
        }
        case MODEL_TAXI:
        case MODEL_CABBIE: {
            FindPlayerInfo().m_nMoney += 12;
            break;
        }
        case MODEL_ENFORCER: {
            // Como el original: fcom + test ah,41h; solo sube si maxArmour > armadura (con NaN o igual no cambia)
            const float maxArmour = (float)FindPlayerInfo(0).m_nMaxArmour;
            if (maxArmour > driver->m_fArmour) {
                driver->m_fArmour = maxArmour;
            }
            break;
        }
        case MODEL_CADDY: {
            // Pirulax:
            // `driver->IsPlayer()` check is useless here, because the precondition
            // to ever reach this code is `driver == FindPlayerPed()`
            if (!driver->IsPlayer() || driver->AsPlayer()->DoesPlayerWantNewWeapon(eWeaponType::WEAPON_GOLFCLUB, true)) {
                CStreaming::RequestModel(MODEL_GOLFCLUB, STREAMING_GAME_REQUIRED);
            }
            break;
        }
        case MODEL_HOTDOG: {
            CStats::IncrementStat(STAT_CALORIES, 40.0f);
            break;
        }
        case MODEL_COPCARLA:
        case MODEL_COPCARSF:
        case MODEL_COPCARVG:
        case MODEL_COPCARRU: {
            CStreaming::RequestModel(MODEL_CHROMEGUN, STREAMING_GAME_REQUIRED);
            vehicleFlags.bFreebies = true;
            break;
        }
        default:
            break;
        }
    }

    // Como el original (0x6D17D5): el conductor NO usa ApplyTurnForceToOccupantOnEntry (esa es la del pasajero): la fuerza
    // es masa * -0.02f; en moto es ApplyMoveForce(arriba * (masa * -0.02f)), y en el resto ApplyTurnForce((0, 0, masa * -0.02f),
    // (dx, dy, 0)) con dx, dy = posicion del conductor - posicion del vehiculo (matrices sin comprobar).
    if (m_nVehicleType == VEHICLE_TYPE_BIKE) {
        const float k = driver->m_fMass * -0.02f;
        const CVector& up = m_matrix->GetUp();
        const CVector force{ up.x * k, up.y * k, up.z * k };
        ApplyMoveForce(force);
    } else {
        const float dy = driver->m_matrix->GetPosition().y - m_matrix->GetPosition().y;
        const float dx = driver->m_matrix->GetPosition().x - m_matrix->GetPosition().x;
        const float fz = driver->m_fMass * -0.02f;
        ApplyTurnForce(CVector{ 0.0f, 0.0f, fz }, CVector{ dx, dy, 0.0f });
    }
}

// 0x6D1950
void CVehicle::RemoveDriver(bool dontTurnEngineOff) {
    SetStatus(STATUS_ABANDONED);

    if (!dontTurnEngineOff) {
        if (!m_pDriver || !m_pDriver->IsPlayer()) {
            vehicleFlags.bEngineOn = false;
        }
    }

    if (const auto playerPed = FindPlayerPed();  m_pDriver == playerPed) {
        switch (m_nModelIndex) {
        case MODEL_CADDY: {
            if (CStreaming::IsModelLoaded(MODEL_GOLFCLUB)) {
                if (playerPed->DoesPlayerWantNewWeapon(eWeaponType::WEAPON_GOLFCLUB, true)) {
                    playerPed->GiveWeapon(WEAPON_GOLFCLUB, 1, true);
                }
                CStreaming::SetModelIsDeletable(MODEL_GOLFCLUB);
            }
            break;
        }
        case MODEL_COPCARLA:
        case MODEL_COPCARSF:
        case MODEL_COPCARVG:
        case MODEL_COPCARRU: {
            if (CStreaming::IsModelLoaded(MODEL_CHROMEGUN)) {
                if (vehicleFlags.bFreebies) {
                    if (playerPed->DoesPlayerWantNewWeapon(eWeaponType::WEAPON_SHOTGUN, true)) {
                        playerPed->GiveWeapon(eWeaponType::WEAPON_SHOTGUN, 5, true);
                    } else {
                        playerPed->GrantAmmo(eWeaponType::WEAPON_SHOTGUN, 5);
                    }
                    vehicleFlags.bFreebies = false;
                }
                // Como el original: SetModelIsDeletable se llama siempre que el modelo este cargado, tambien sin bFreebies
                CStreaming::SetModelIsDeletable(MODEL_CHROMEGUN);
            }
            break;
        }
        }
    }

    CEntity::ClearReference(m_pDriver);
}

// 0x6D1A50
CPed* CVehicle::SetUpDriver(int32 gangPedType, bool createAsMale, bool createAsCriminal) {
    if (m_pDriver) {
        return m_pDriver;
    }

    if (IsCreatedBy(eVehicleCreatedBy::RANDOM_VEHICLE)) {
        CPopulation::AddPedInCar(this, true, gangPedType, 0, createAsMale, createAsCriminal);
        return m_pDriver;
    }

    return nullptr;
}

// 0x6D1AA0
CPed* CVehicle::SetupPassenger(int32 seatIdx, int32 gangPedType, bool createAsMale, bool createAsCriminal) {
    // Como el original: el asiento se lee sin comprobar el rango (.data()[]).
    if (const auto passenger = m_apPassengers.data()[seatIdx]) {
        return passenger;
    }

    switch (m_nModelIndex) {
    case MODEL_TAXI:
    case MODEL_CABBIE:
    case MODEL_STRETCH: {
        if (!seatIdx) {
            // Como el original: RemovePassenger sobre el asiento 0 (ya vacio, asi que no hace nada, pero se llama)
            RemovePassenger(m_apPassengers.data()[0]);
            return nullptr;
        }
    }
    }

    CPed* psgrAdded = CPopulation::AddPedInCar(this, false, gangPedType, seatIdx, createAsMale, createAsCriminal);

    // Como el original (0x6D1B22): comparaciones con signo sobre el tipo del peaton. No se comprueban los modelos de los
    // ocupantes para medicos, bomberos, policias, bandas (GANG1..GANG10) ni, siendo criminal, con gangPedType en GANG8..SPECIAL.
    const int32 pedType = (int32)psgrAdded->m_nPedType;
    bool shouldCheckModels = true;
    if (pedType == (int32)PED_TYPE_MEDIC || pedType == (int32)PED_TYPE_FIREMAN || pedType == (int32)PED_TYPE_COP) {
        shouldCheckModels = false;
    } else if (pedType >= (int32)PED_TYPE_GANG1 && pedType <= (int32)PED_TYPE_GANG10) {
        shouldCheckModels = false;
    } else if (pedType == (int32)PED_TYPE_CRIMINAL && gangPedType >= (int32)PED_TYPE_GANG8 && gangPedType <= (int32)PED_TYPE_SPECIAL) {
        shouldCheckModels = false;
    }
    if (!shouldCheckModels) {
        return psgrAdded;
    }

    // Como el original: se quita al pasajero recien anadido si el conductor o alguno de los asientos anteriores (sin contar
    // al propio pasajero) tiene el mismo modelo; el recorrido de asientos sigue con psgrAdded ya nulo, sin efecto.
    const int16 psgrModel = (int16)psgrAdded->m_nModelIndex;
    if (m_pDriver && (int16)m_pDriver->m_nModelIndex == psgrModel) {
        RemovePassenger(psgrAdded);
        CPopulation::RemovePed(psgrAdded);
        return nullptr;
    }
    for (int32 seat = 0; seat < seatIdx; seat++) {
        CPed* const occupant = m_apPassengers.data()[seat];
        if (occupant && psgrAdded && occupant != psgrAdded && (int16)occupant->m_nModelIndex == psgrModel) {
            RemovePassenger(psgrAdded);
            CPopulation::RemovePed(psgrAdded);
            psgrAdded = nullptr;
        }
    }
    return psgrAdded;
}

// 0x6D1BD0
bool CVehicle::IsPassenger(CPed* ped) const {
    if (!ped)
        return false;

    for (const auto& passenger : m_apPassengers) {
        if (passenger == ped) {
            return true;
        }
    }
    return false;
}

// 0x6D1C00
bool CVehicle::IsPassenger(int32 modelIndex) const {
    for (const auto& passenger : m_apPassengers) {
        // Como el original: movsx del indice de modelo del peaton (con signo)
        if (passenger && (int16)passenger->m_nModelIndex == modelIndex) {
            return true;
        }
    }
    return false;
}

bool CVehicle::IsPedOfModelInside(eModelID model) const {
    return IsDriver(model) || IsPassenger(model);
}

bool CVehicle::IsDriver(const CPed* ped) const {
    return ped && ped == m_pDriver;
}

bool CVehicle::IsDriver(int32 modelIndex) const {
    // Como el original: movsx del indice de modelo del peaton (con signo)
    return m_pDriver && (int16)m_pDriver->m_nModelIndex == modelIndex;
}

/*!
* @addr 0x6D1C80
* @brief Kill all peds in the vehicle, and dispatch an event as if they were killed by an explosion
*/
void CVehicle::KillPedsInVehicle() {
    const auto ProcessOccupant = [this](CPed* occupant) {
        if (occupant) {
            if (!CGameLogic::IsCoopGameGoingOn()) {
                CDarkel::RegisterKillByPlayer(*occupant, WEAPON_EXPLOSION, false, 0);
            }
            CEventVehicleDied event{ this };
            occupant->GetIntelligence()->m_eventGroup.Add(&event);
        }
    };

    ProcessOccupant(m_pDriver);
    rng::for_each(GetMaxPassengerSeats(), ProcessOccupant);
}

// 0x6D1D90
bool CVehicle::IsUpsideDown() const {
    return m_matrix->GetUp().z <= -0.9f;
}

// 0x6D1DD0
bool CVehicle::IsOnItsSide() const {
    return m_matrix->GetRight().z >= 0.8f || m_matrix->GetRight().z <= -0.8f;
}

// 0x6D1E20
bool CVehicle::CanPedOpenLocks(const CPed* ped) const {
    switch (m_nDoorLock) {
    case CARLOCK_LOCKED:
    case CARLOCK_COP_CAR:
    case CARLOCK_LOCKED_PLAYER_INSIDE:
    case CARLOCK_SKIP_SHUT_DOORS:
        return false;
    case CARLOCK_LOCKOUT_PLAYER_ONLY:
        return !ped->IsPlayer();
    default:
        return true;
    }
}

// 0x6D1E60
bool CVehicle::CanDoorsBeDamaged() const {
    // TODO: ranges::contains({...}, m_nDoorLock)
    switch (m_nDoorLock) {
    case CARLOCK_NOT_USED:
    case CARLOCK_UNLOCKED:
    case CARLOCK_SKIP_SHUT_DOORS:
        return true;
    default:
        return false;
    }
}

// 0x6D1E80
bool CVehicle::CanPedEnterCar() {
    // Como el original (0x6D1E80): con el vehiculo de lado (|up.z| <= 0.1, no moto) no se puede entrar; en el resto de casos
    // solo si no se mueve ni gira (0x863244 = 0.04000000283f, el siguiente float a 0.04f; "no mayor que": un NaN pasa).
    const auto upZ = GetUp().z;
    if (!IsBike() && !(upZ > 0.1f || std::isnan(upZ)) && !(upZ < -0.1f)) {
        return false;
    }

    constexpr auto limit = std::bit_cast<float>(0x3D23D70Bu);
    return !(m_vecMoveSpeed.SquaredMagnitude() > limit) &&
           !(m_vecTurnSpeed.SquaredMagnitude() > limit);
}

// 0x6D21F0
void CVehicle::ProcessCarAlarm() {
    switch (m_nAlarmState) {
    case 0:
    case std::numeric_limits<decltype(m_nAlarmState)>::max(): { // Doing this in case we ever the underlying type.
        return;
    }
    }

    // Como el original (0x6D21F0): el contador de la alarma se RESTA el paso en milisegundos (timestep * 0.02f * 1000.0f,
    // truncado a entero de 32 bits con _ftol2); si el contador es menor que el paso (comparacion sin signo) se pone a cero
    // junto con la bocina.
    const uint16 alarm = m_nAlarmState;
    const float stepMs = CTimer::GetTimeStep() * 0.02f * 1000.0f;
    const int32 ts = (int32)(int64)stepMs;
    if ((uint32)alarm >= (uint32)ts) {
        m_nAlarmState = (uint16)((uint32)alarm - (uint32)ts);
    } else {
        m_nAlarmState = 0;
        m_HornCounter = 0;
    }
}

// 0x6D2250
void CVehicle::DestroyVehicleAndDriverAndPassengers(CVehicle* vehicle) {
    const auto ProcessOccupant = [](CPed* occupant) {
        if (occupant) {
            if (!CGameLogic::IsCoopGameGoingOn()) {
                CDarkel::RegisterKillByPlayer(*occupant, WEAPON_UNIDENTIFIED, false, 0);
            }
            occupant->FlagToDestroyWhenNextProcessed();
        }
    };

    // Como el original: los ocupantes son los de `vehicle` (la funcion es estatica, sin this)
    ProcessOccupant(vehicle->m_pDriver);
    rng::for_each(vehicle->GetMaxPassengerSeats(), ProcessOccupant);

    CWorld::Remove(vehicle);
    delete vehicle;
}

// 0x6D22F0
bool CVehicle::IsVehicleNormal() {
    if (m_pDriver
        && !m_nNumPassengers
        && GetStatus() != STATUS_WRECKED
        && GetVehicleModelInfo()->m_nVehicleClass != VEHICLE_CLASS_IGNORE
    ) {
        return true;
    }
    return false;
}

// 0x6D2330
void CVehicle::ChangeLawEnforcerState(bool bIsEnforcer) {
    if (bIsEnforcer) {
        if (!vehicleFlags.bIsLawEnforcer) {
            vehicleFlags.bIsLawEnforcer = true;
            ++CCarCtrl::NumLawEnforcerCars;
        }
    }
    else if (vehicleFlags.bIsLawEnforcer){
        vehicleFlags.bIsLawEnforcer = false;
        --CCarCtrl::NumLawEnforcerCars;
    }
}

// 0x6D2370
bool CVehicle::IsLawEnforcementVehicle() const {
    switch (m_nModelIndex) {
    case MODEL_ENFORCER:
    case MODEL_PREDATOR:
    case MODEL_RHINO:
    case MODEL_BARRACKS:
    case MODEL_FBIRANCH:
    case MODEL_COPBIKE:
    case MODEL_FBITRUCK:
    case MODEL_COPCARLA:
    case MODEL_COPCARSF:
    case MODEL_COPCARVG:
    case MODEL_COPCARRU:
    case MODEL_SWATVAN:
        return true;
    default:
        return false;
    }
}

// unused
// 0x6D2450
bool CVehicle::ShufflePassengersToMakeSpace() {
    return true;
}

// 0x6D2460
void CVehicle::ExtinguishCarFire() {
    if (GetStatus() != STATUS_WRECKED) {
        // Como el original: un NaN tambien se sube a 300 (fcomp + test ah,41h)
        if (!(m_fHealth > 300.0f))
            m_fHealth = 300.0f;
    }

    if (m_pFire) {
        m_pFire->SetIsScript(false);
        m_pFire->Extinguish();
        m_pFire = nullptr;
    }

    if (IsAutomobile()) {
        if (AsAutomobile()->m_damageManager.GetEngineStatus() >= 225) {
            AsAutomobile()->m_damageManager.SetEngineStatus(215);
        }
        AsAutomobile()->m_fBurnTimer = 0.0f;
    }
}

// 0x6D24F0
void CVehicle::ActivateBomb() {
    switch (m_nBombOnBoard) {
    case BOMB_TIMED_NOT_ACTIVATED: {
        m_nBombOnBoard = BOMB_TIMED_ACTIVATED;
        m_wBombTimer = 7000;
        m_pWhoDetonatedMe = FindPlayerPed();
        break;
    }
    case BOMB_IGNITION: {
        m_nBombOnBoard = BOMB_IGNITION_ACTIVATED;
        break;
    }
    default: {
        return;
    }
    }
    CGarages::TriggerMessage("GA_12", -1, 3000u); // "Bomb armed"
}

// 0x6D2570
void CVehicle::ActivateBombWhenEntered() {
    if (m_pDriver) {
        if (!vehicleFlags.bDriverLastFrame && m_nBombOnBoard == BOMB_IGNITION_ACTIVATED) { // If the driver just entered and there's an ignition bomb...
            m_wBombTimer = 1000;
            m_pWhoDetonatedMe = m_pWhoInstalledBombOnMe; // NOTE: `m_pWhoInstalledBombOnMe` isn't set in `ActivateBomb` weird...
            CEntity::SafeRegisterRef(m_pWhoDetonatedMe); // Como el original: solo registra si no es nulo
        }
    }
    vehicleFlags.bDriverLastFrame = m_pDriver != nullptr;
}

// 0x6D25D0
bool CVehicle::CarHasRoof() {
    return !m_pHandlingData->m_bConvertible || !m_anExtras[0] || !m_anExtras[1];
}

// 0x6D2600
float CVehicle::HeightAboveCeiling(float height, eFlightModel flightModel) {
    // Como el original: los modelos 1 (BARON) y 2 (RC) comparten rama; las comparaciones son "height < X" (un NaN
    // pasa de largo y devuelve NaN en la rama del Baron o NaN - 800 en la otra).
    if (flightModel == eFlightModel::FLIGHT_MODEL_BARON || flightModel == eFlightModel::FLIGHT_MODEL_RC) {
        if (height < 500.f) {
            return -1.f;
        }
        if (height < 950.f) {
            return height - 500.f;
        }
        if (height < 1500.f) {
            return -1.f;
        }
        return (height - 1000.f) + 500.f;
    }
    if (height < 800.f) {
        return -1.f;
    }
    return height - 800.f;
}

// 0x6D2690
RwObject* SetVehicleAtomicVisibilityCB(RwObject* object, void* data) {
    assert(RwObjectGetType(object) == rpATOMIC);
    const auto atomic      = reinterpret_cast<RpAtomic*>(object);
    const auto toSet = std::bit_cast<eAtomicComponentFlag>(data);
    // Como el original (0x6D2690): usa GetAtomicId (0x732370), no GetUserValue, y lo llama dos veces (test al,3 y luego and eax,3)
    if (CVisibilityPlugins::GetAtomicId(atomic) & ATOMIC_MASK) {
        const auto current = CVisibilityPlugins::GetAtomicId(atomic) & ATOMIC_MASK;
        RpAtomicSetFlags(atomic, current != toSet ? 0 : rpATOMICRENDER);
    }
    return object;
}

// 0x6D26D0
RwFrame* SetVehicleAtomicVisibilityCB(RwFrame* frame, void* data) {
    RwFrameForAllObjects(frame, SetVehicleAtomicVisibilityCB, data);
    RwFrameForAllChildren(frame, SetVehicleAtomicVisibilityCB, data);
    return frame;
}

// 0x6D2700
void CVehicle::SetComponentVisibility(RwFrame* component, uint32 visibilityState) { // see eAtomicComponentFlag
    assert(visibilityState == ATOMIC_NONE || visibilityState == ATOMIC_OK || visibilityState == ATOMIC_DAMAGED);
    if (component) {
        if (visibilityState == eAtomicComponentFlag::ATOMIC_DAMAGED) {
            vehicleFlags.bIsDamaged = true;
        }
        RwFrameForAllObjects(component, SetVehicleAtomicVisibilityCB, (void*)visibilityState);
        RwFrameForAllChildren(component, SetVehicleAtomicVisibilityCB, (void*)visibilityState);
    }
}

// 0x6D2740
void CVehicle::ApplyBoatWaterResistance(tBoatHandlingData* boatHandling, float fImmersionDepth) {
    // Como el original: ((profundidad * fuerza de suspension) * profundidad) * masa * 0.001 (sin sq() ni division entre 1000)
    float fSpeedMult = fImmersionDepth * m_pHandlingData->m_fSuspensionForceLevel;
    fSpeedMult       = fSpeedMult * fImmersionDepth;
    fSpeedMult       = fSpeedMult * m_fMass;
    fSpeedMult       = fSpeedMult * 0.001F; // 0x858CDC
    if (m_nModelIndex == MODEL_SKIMMER) {
        fSpeedMult *= 30.0F; // 0x871DDC
    }

    // Como el original: producto escalar con la suma z, y, x
    const auto& mat = *m_matrix;
    const auto fMoveDotProduct = VehEDot(m_vecMoveSpeed, mat.GetForward());
    fSpeedMult = fSpeedMult * (fMoveDotProduct * fMoveDotProduct + 0.05F);
    fSpeedMult += 1.0F;
    if (fSpeedMult < 0.0F) { // fabs sin tocar NaN (fcom + fchs)
        fSpeedMult = -fSpeedMult;
    }
    fSpeedMult = 1.0F / fSpeedMult;

    // Como el original: pow del CRT del EXE (_CIpow) por componente
    const float fUsedTimeStep = CTimer::GetTimeStep() * 0.5F;
    const CVector vecSpeedMult{
        VehEPow(fSpeedMult * boatHandling->m_vecMoveRes.x, fUsedTimeStep),
        VehEPow(fSpeedMult * boatHandling->m_vecMoveRes.y, fUsedTimeStep),
        VehEPow(fSpeedMult * boatHandling->m_vecMoveRes.z, fUsedTimeStep),
    };

    // Como el original: inversa de la matriz (0x59C810), producto por componente y de vuelta a mundo (0x59C790)
    m_vecMoveSpeed   = VehEInvVec(mat, m_vecMoveSpeed);
    m_vecMoveSpeed.x = vecSpeedMult.x * m_vecMoveSpeed.x;
    m_vecMoveSpeed.y = vecSpeedMult.y * m_vecMoveSpeed.y;
    m_vecMoveSpeed.z = vecSpeedMult.z * m_vecMoveSpeed.z;

    const auto fMassMult = ((vecSpeedMult.y - 1.0F) * m_vecMoveSpeed.y) * m_fMass;
    m_vecMoveSpeed       = VehEMatVec(mat, m_vecMoveSpeed);

    const CVector vecDown{ mat.GetUp().x * -1.0F, mat.GetUp().y * -1.0F, mat.GetUp().z * -1.0F };
    const CVector vecTurnForce{ mat.GetForward().x * fMassMult, mat.GetForward().y * fMassMult, mat.GetForward().z * fMassMult };
    ApplyTurnForce(vecTurnForce, vecDown);

    // Como el original: con velocidad z <= 0 (o NaN) se amortigua con (1 - z') * 0.5 + z'
    if (!(m_vecMoveSpeed.z > 0.0F)) {
        m_vecMoveSpeed.z *= ((1.0F - vecSpeedMult.z) * 0.5F + vecSpeedMult.z);
    } else {
        m_vecMoveSpeed.z *= vecSpeedMult.z;
    }
}

// 0x6D2950
RpMaterial* SetCompAlphaCB(RpMaterial* material, void* data) {
    return ((RpMaterial * (__cdecl*)(RpMaterial*, void*))0x6D2950)(material, data);
}

// 0x6D2960
void CVehicle::SetComponentAtomicAlpha(RpAtomic* atomic, int32 alpha) {
    auto geometry = atomic->geometry;
    geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;
    RpGeometryForAllMaterials(geometry, SetCompAlphaCB, reinterpret_cast<void*>(alpha));
}

CVehicleModelInfo* CVehicle::GetVehicleModelInfo() const {
    return CModelInfo::GetModelInfo(m_nModelIndex)->AsVehicleModelInfoPtr();
}

CVector CVehicle::GetDummyPositionObjSpace(eVehicleDummy dummy) const {
    return GetVehicleModelInfo()->GetModelDummyPosition(dummy);
}

// if bWorldSpace is true, returns the position in world-space
// otherwise in model-space
CVector CVehicle::GetDummyPosition(eVehicleDummy dummy, bool bWorldSpace) {
    CVector pos = GetDummyPositionObjSpace(dummy);
    if (bWorldSpace)
        pos = GetMatrix().TransformPoint(pos); // transform to world-space
    return pos;
}

// 0x6D2980
void CVehicle::UpdateClumpAlpha() {
    const auto GetAlphaToSet = [this] {
        const auto curr = CVisibilityPlugins::GetClumpAlpha(GetRpClump());
        if (vehicleFlags.bFadeOut) {
            return std::max(0, curr - 8);
        } else if (curr < 255) {
            return std::min(255, curr + 16);
        }
        return curr; // Como el original (0x6D29B0): con alfa >= 255 se conserva tal cual (no se recorta a 255)
    };
    CVisibilityPlugins::SetClumpAlpha(GetRpClump(), GetAlphaToSet());
}

// 0x6D29E0
void CVehicle::UpdatePassengerList() {
    // No sure what's the point of this
    // It checks if there should be any passengers
    // If there's none only then it sets the number of them to 0.. weird.
    if (m_nNumPassengers) {
        if (rng::all_of(m_apPassengers, [](auto&& p) { return p == nullptr; })) {
            m_nNumPassengers = 0;
        }
    }
}

// 0x6D2A10
CPed* CVehicle::PickRandomPassenger() {
    // TODO: Add a function for this to random.hpp

    // Como el original (0x6D2A10): el inicio es (int)(rand() * (1/32768) * 8), es decir GetRandomIntInRange(0, 8)
    // (GetRandomNumberInRange<uint> reparte con 1/32767 sobre [0, 7] y puede dar otro asiento).
    const int32 rnd = CGeneral::GetRandomIntInRange(0, (int32)std::size(m_apPassengers));
    for (int32 i = 0; i < (int32)std::size(m_apPassengers); i++) {
        if (const auto psgr = m_apPassengers.data()[(rnd + i) % (int32)std::size(m_apPassengers)]) {
            return psgr;
        }
    }

    return nullptr;
}

// 0x6D2A80
void CVehicle::AddDamagedVehicleParticles() {
    if (IsSubPlane())
        return;

    if (m_fHealth >= 650.0f || m_fHealth < 250.0f || physicalFlags.bSubmergedInWater) {
        FxSystem_c::SafeKillAndClear(m_pOverheatParticle);
        return;
    }

    auto* const matrix = GetRpClump()
        ? RwFrameGetMatrix(RpClumpGetFrame(GetRpClump()))
        : nullptr;

    if (!m_pOverheatParticle && matrix) {
        m_pOverheatParticle = g_fxMan.CreateFxSystem(
            m_pHandlingData->m_transmissionData.m_nEngineType == 'E'
                ? "overheat_car_electric"
                : "overheat_car",
            GetVehicleModelInfo()->GetModelDummyPosition(DUMMY_ENGINE),
            matrix,
            false
        );
        if (m_pOverheatParticle) {
            m_pOverheatParticle->Play();
        }
    }

    if (m_pOverheatParticle) {
        // Como el original: multiplica por la constante 0x86C340 (no divide entre 400)
        m_pOverheatParticle->SetConstTime(1u, 1.0f - (m_fHealth - 250.0f) * std::bit_cast<float>(0x3B23D70Au));
        CVector velocity = m_vecMoveSpeed * 50.0f;
        m_pOverheatParticle->SetVelAdd(velocity);
    }
}

// 0x6D2BF0
void CVehicle::MakeDirty(CColPoint& colPoint) {
    // Como el original: comparaciones "no mayor que" (los NaN salen como en el x87) y productos en float (ts * 0.01f, no ts / 50).
    if (g_surfaceInfos.IsWater(colPoint.m_nSurfaceTypeB) || CWeather::IsRainy()) {
        if (!(m_fDirtLevel > 1.0f)) {
            return;
        }
        m_fDirtLevel = m_fDirtLevel - CTimer::ms_fTimeStep * 0.01f;
        return;
    }

    if (g_surfaceInfos.MakesCarDirty(colPoint.m_nSurfaceTypeB)) {
        if (!(m_vecMoveSpeed.Magnitude2D() > 0.06f)) {
            return;
        }
        const auto dirt = CTimer::GetTimeStep() * 0.003f + m_fDirtLevel;
        m_fDirtLevel = 15.0f < dirt ? 15.0f : dirt;
        return;
    }

    if (g_surfaceInfos.MakesCarClean(colPoint.m_nSurfaceTypeB)) {
        if (!(m_vecMoveSpeed.Magnitude2D() > 0.04f) || !(m_fDirtLevel > 4.0f)) {
            return;
        }
        const auto dirt = m_fDirtLevel - CTimer::GetTimeStep() * 0.02f;
        m_fDirtLevel = 4.0f > dirt ? 4.0f : dirt;
    }
}

// 0x6D2D50
bool CVehicle::AddWheelDirtAndWater(CColPoint& colPoint, bool isProduceWheelDrops, bool isWheelsSpinning, bool isWheelInWater) {
    // Como el original (0x6D2D50): todas las consultas de superficie usan m_nSurfaceTypeB (byte 0x23)
    if (!isProduceWheelDrops && !g_surfaceInfos.IsSand(colPoint.m_nSurfaceTypeB)) {
        return false;
    }

    if (isWheelInWater) {
        g_fx.AddWheelSpray(this, colPoint.m_vecPoint, isWheelsSpinning, true, m_fContactSurfaceBrightness);
        return false;
    }

    const auto CreateFxForSurface = [&](auto CheckSurface, auto AddFx) {
        if ((g_surfaceInfos.*CheckSurface)(colPoint.m_nSurfaceTypeB)) {
            (g_fx.*AddFx)(this, colPoint.m_vecPoint, isWheelsSpinning, m_fContactSurfaceBrightness);
            return true;
        }
        return false;
    };
    if (CreateFxForSurface(&SurfaceInfos_c::CreatesWheelGrass,  &Fx_c::AddWheelGrass)) {
        return false;
    }
    if (CreateFxForSurface(&SurfaceInfos_c::CreatesWheelGravel, &Fx_c::AddWheelGravel)) {
        return true;  // The only odd one, wonder why
    }
    if (CreateFxForSurface(&SurfaceInfos_c::CreatesWheelMud, &Fx_c::AddWheelMud)) {
        return false;
    }
    // Como el original: el azar solo se tira si la superficie es polvo/arena, y en ese caso siempre devuelve false
    if (g_surfaceInfos.CreatesWheelDust(colPoint.m_nSurfaceTypeB)) {
        if (CWeather::WetRoads <= 0.0f || CGeneral::GetRandomNumberInRange(CWeather::WetRoads, 1.01f) <= 0.5f) {
            g_fx.AddWheelDust(this, colPoint.m_vecPoint, isWheelsSpinning, m_fContactSurfaceBrightness);
        }
        return false;
    }
    if (g_surfaceInfos.CreatesWheelSand(colPoint.m_nSurfaceTypeB)) {
        if (CWeather::WetRoads <= 0.0f || CGeneral::GetRandomNumberInRange(CWeather::WetRoads, 1.01f) <= 0.5f) {
            g_fx.AddWheelSand(this, colPoint.m_vecPoint, isWheelsSpinning, m_fContactSurfaceBrightness);
        }
        return false;
    }
    if (g_surfaceInfos.CreatesWheelSpray(colPoint.m_nSurfaceTypeB)) {
        if (CWeather::WetRoads > 0.4f && !CCullZones::CamNoRain()) {
            g_fx.AddWheelSpray(this, colPoint.m_vecPoint, isWheelsSpinning, false, m_fContactSurfaceBrightness);
            return false;
        }
    }
    return true;
}

// 0x6D3000
void CVehicle::SetGettingInFlags(uint8 doorId) {
    m_nGettingInFlags |= doorId;
}

// 0x6D3020
void CVehicle::SetGettingOutFlags(uint8 doorId) {
    m_nGettingOutFlags |= doorId;
}

// 0x6D3040
void CVehicle::ClearGettingInFlags(uint8 doorId) {
    m_nGettingInFlags &= ~doorId;
}

// 0x6D3060
void CVehicle::ClearGettingOutFlags(uint8 doorId) {
    m_nGettingOutFlags &= ~doorId;
}

// 0x6D3080
void CVehicle::SetWindowOpenFlag(uint8 doorId) {
    auto frameFromId = CClumpModelInfo::GetFrameFromId(GetRpClump(), doorId);
    if (frameFromId) {
        RwFrameForAllObjects(frameFromId, CVehicleModelInfo::SetAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_DONT_RENDER_ALPHA);
    }
}

// 0x6D30B0
void CVehicle::ClearWindowOpenFlag(uint8 doorId) {
    auto frameFromId = CClumpModelInfo::GetFrameFromId(GetRpClump(), doorId);
    if (frameFromId) {
        RwFrameForAllObjects(frameFromId, CVehicleModelInfo::ClearAtomicFlagCB, (void*)eAtomicComponentFlag::ATOMIC_DONT_RENDER_ALPHA);
    }
}

// 0x6D30E0
bool CVehicle::SetVehicleUpgradeFlags(int32 upgradeModelIndex, int32 modId, int32& resultModelIndex) {
    // At the one and only place this function is called from
    // componentIndex == CModelInfo::GetModelInfo(upgradeModelIndex)->AsVehicleModelInfo().CarMod
    // Now, I'm not sure what value it has, so..

    switch (modId) {
    case 16: {
        if (handlingFlags.bHydraulicInst) {
            resultModelIndex = upgradeModelIndex;
        }

        handlingFlags.bHydraulicInst = true;
        m_nFakePhysics = false;
        m_vecMoveSpeed.z = 0.0f;

        return true;
    }
    case 15: {
        if (!IsAutomobile()) {
            return false;
        }

        const auto GetNitroValue = [&]() -> int8 {
            if (upgradeModelIndex == ModelIndices::MI_NITRO_BOTTLE_LARGE) {
                return 5;
            } else if (upgradeModelIndex == ModelIndices::MI_NITRO_BOTTLE_DOUBLE) {
                return 10;
            }
            return 2;
        };

        if (handlingFlags.bNosInst) {
            resultModelIndex = ModelIndices::MI_NITRO_BOTTLE_SMALL;
        }

        AsAutomobile()->NitrousControl(GetNitroValue());

        return GetModelInfo()->AsVehicleModelInfoPtr()->m_pVehicleStruct->m_aUpgrades[15].m_nParentComponentId < 0;
    }
    case 17: {
        if (m_vehicleAudio.m_AuSettings.RadioType != AE_RT_CIVILIAN || vehicleFlags.bUpgradedStereo) {
            resultModelIndex = upgradeModelIndex;
            return true;
        }

        auto& bs = m_vehicleAudio.m_AuSettings.BassSetting;
        switch (bs) {
        case eBassSetting::CUT:    bs = eBassSetting::NORMAL; break;
        case eBassSetting::NORMAL: bs = eBassSetting::BOOST;  break;
        case eBassSetting::BOOST:  return true;
        }
        AudioEngine.SetRadioBassSetting(bs);

        vehicleFlags.bUpgradedStereo = true;

        return true;
    }
    default: {
        return false;
    }
    }
}

// 0x6D3210
bool CVehicle::ClearVehicleUpgradeFlags(int32 arg0, int32 modId) {
    // See `SetVehicleUpgradeFlags` for a comment on what `componentIndex` is

    switch (modId) {
    case 17: { // 0x6D3270
        // Como el original (0x6D326D): actúa solo con RadioType == AE_RT_CIVILIAN (el EXE salta si es distinto de 0) y la mejora puesta
        if (m_vehicleAudio.m_AuSettings.RadioType == AE_RT_CIVILIAN && vehicleFlags.bUpgradedStereo) {
            auto& bs = m_vehicleAudio.m_AuSettings.BassSetting;
            switch (bs) {
            case eBassSetting::BOOST:  bs = eBassSetting::NORMAL; break;
            case eBassSetting::NORMAL: bs = eBassSetting::CUT;    break;
            }
            AudioEngine.SetRadioBassSetting(bs);
            vehicleFlags.bUpgradedStereo = false;
        }
        return true;
    }
    case 15: { // 0x6D32C6
        if (!IsAutomobile()) {
            return false;
        }

        AsAutomobile()->NitrousControl(-1);

        return GetModelInfo()->AsVehicleModelInfoPtr()->m_pVehicleStruct->m_aUpgrades[15].m_nParentComponentId < 0;
    }
    case 16: { // 0x6D321C
        if (handlingFlags.bHydraulicInst) {
            auto& specColIdx = m_vehicleSpecialColIndex;
            if (specColIdx > -1) {
                m_aSpecialColVehicle.data()[specColIdx] = nullptr; // Como el original: índice con signo de 8 bits sin comprobar el rango
                specColIdx = -1;

                SetupSuspensionLines();
                m_nFakePhysics = false;
                m_vecMoveSpeed.z = .02f;
            }
        }
        handlingFlags.bHydraulicInst = false;
        return true;
    }
    default: {
        return false;
    }
    }
}

// 0x6D3300
// Como el original (cdecl, callback de RpClumpForAllAtomics; siempre devuelve el atomo): solo actua sobre atomos con ATOMIC_UPGRADE (0x732370, una vez) cuyo modelo
// (0x732260, primera llamada) tiene CarMod == 'data' (el identificador se pasa como valor, sin signo) y no usa VehDummy; entonces vuelve a pedir el modelo
// (0x732260, segunda llamada), lee clump (+0x3C) y marco (+4), quita y destruye el atomo, destruye el marco SIEMPRE y libera una referencia del modelo si no es nulo.
RpAtomic* RemoveUpgradeCB(RpAtomic* atomic, void* data) {
    if (!(CVisibilityPlugins::GetAtomicId(atomic) & ATOMIC_UPGRADE)) {
        return atomic;
    }
    const CBaseModelInfo* const info = CVisibilityPlugins::GetModelInfo(atomic);
    const uint32 carMod = info->CarMod;
    const uint32 usesVehDummy = info->bUsesVehDummy;
    if (reinterpret_cast<uint32>(data) != carMod || usesVehDummy) {
        return atomic;
    }
    CBaseModelInfo* const owner = CVisibilityPlugins::GetModelInfo(atomic);
    RpClump* const clump = atomic->clump;
    RwFrame* const frame = RpAtomicGetFrame(atomic);
    RpClumpRemoveAtomic(clump, atomic);
    RpAtomicDestroy(atomic);
    RwFrameDestroy(frame);
    if (owner) {
        owner->RemoveRef();
    }
    return atomic;
}

// 0x6D3370
// Como el original (cdecl, callback de RpClumpForAllAtomics): un atomo con ATOMIC_UPGRADE (0x732370, una vez) cuyo modelo (0x732260) tiene CarMod == data[0]
// (comparado sin signo) se guarda en data[1] y detiene el recorrido (devuelve nullptr); cualquier otro se devuelve para seguir.
RpAtomic* FindUpgradeCB(RpAtomic* atomic, void* data) {
    if (CVisibilityPlugins::GetAtomicId(atomic) & ATOMIC_UPGRADE) {
        const uint32 carMod = CVisibilityPlugins::GetModelInfo(atomic)->CarMod;
        if (*static_cast<uint32*>(data) == carMod) {
            static_cast<RpAtomic**>(data)[1] = atomic;
            return nullptr;
        }
    }
    return atomic;
}

// 0x6D33B0
RwObject* RemoveObjectsCB(RwObject* object, void* data) {
    if (RwObjectGetType(object) != rpATOMIC) {
        return object;
    }

    const auto atomic = reinterpret_cast<RpAtomic*>(object);
    const auto flags = CVisibilityPlugins::GetAtomicId(atomic);
    *static_cast<int32*>(data) = flags;
    if (flags & eAtomicComponentFlag::ATOMIC_UPGRADE) {
        return object;
    }

    const auto modelInfo = CVisibilityPlugins::GetModelInfo(atomic);
    const auto frame = RpAtomicGetFrame(atomic);
    RpClumpRemoveAtomic(RpAtomicGetClump(atomic), atomic);
    RpAtomicDestroy(atomic);
    if (!CVisibilityPlugins::GetFrameHierarchyId(frame)) {
        RwFrameDestroy(frame);
    }
    if (modelInfo) {
        modelInfo->RemoveRef();
    }
    return object;
}

// 0x6D3420
RwFrame* RemoveObjectsCB(RwFrame* frame, void* data) {
    RwFrameForAllObjects(frame, RemoveObjectsCB, data);
    RwFrameForAllChildren(frame, RemoveObjectsCB, data);
    return frame;
}

// 0x6D3450
static auto& CopyObjectsCB_TargetClump = StaticRef<RpClump*>(0xC1CB58);
RwObject* CopyObjectsCB(RwObject* object, void* data) {
    const auto frame = (RwFrame*)data;

    if (RwObjectGetType(object) == rpATOMIC) {
        const auto atomic = (RpAtomic*)object;
        const auto clone = RpAtomicClone(atomic);
        RpClumpAddAtomic(CopyObjectsCB_TargetClump, clone);
        RpAtomicSetFrame(clone, frame);
    }

    return object;
}

// 0x6D3490
RwObject* FindReplacementUpgradeCB(RwObject* object, void* data) {
    // Como el original (cdecl, callback de RwFrameForAllObjects): solo un atomo (tipo 1) cuyo id (0x732370, una vez) no tiene el bit 0x800 (ATOMIC_UPGRADE)
    // y cuyo indice de modelo (0x732250, solo entonces) no es -1 se guarda en data[1] (+4 de tCompSearchStructById) y detiene el recorrido (devuelve nullptr).
    // Cualquier otro objeto se devuelve tal cual para seguir. El iterador del EXE solo lee EAX a la vuelta.
    if (RwObjectGetType(object) == rpATOMIC) {
        auto* const atomic = reinterpret_cast<RpAtomic*>(object);
        if (!(CVisibilityPlugins::GetAtomicId(atomic) & ATOMIC_UPGRADE) && CVisibilityPlugins::GetModelInfoIndex(atomic) != -1) {
            reinterpret_cast<RwObject**>(data)[1] = object;
            return nullptr;
        }
    }
    return object;
}

// 0x6D34D0
RpAtomic* RemoveAllUpgradesCB(RpAtomic* atomic, void* data) {
    auto* mi = CVisibilityPlugins::GetModelInfo(atomic);
    if (mi) {
        RpClumpRemoveAtomic(atomic->clump, atomic);
        RpAtomicDestroy(atomic);
        mi->RemoveRef();
    }
    return atomic;
}

// From [0x6D35BC - 0x6D3611]
// Como el original: CreateUpgradeAtomic (0x6D35EA) marca además ATOMIC_ALPHA; CreateReplacementAtomic (0x6D37F6) y AddReplacementUpgrade no
static void SetupUpgradeAtomicRendering(RpAtomic* atomic, bool setAlphaFlag) {
    RpMaterial* hasAlphaMaterial = nullptr;
    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), CVehicleModelInfo::HasAlphaMaterialCB, &hasAlphaMaterial);
    if (hasAlphaMaterial) {
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailAlphaCB);
        if (setAlphaFlag) {
            CVisibilityPlugins::SetAtomicFlag(atomic, ATOMIC_ALPHA);
        }
    } else {
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, CVisibilityPlugins::RenderVehicleHiDetailCB);
    }

    CVehicleModelInfo::SetRenderPipelinesCB(atomic, nullptr);
}

// 0x6D3510
RpAtomic* CVehicle::CreateUpgradeAtomic(CBaseModelInfo* mi, const UpgradePosnDesc* upgradePosn, RwFrame* parentComponent, bool isDamaged) {
    if (isDamaged) {
        CDamageAtomicModelInfo::ms_bCreateDamagedVersion = true;
    }

    const auto atomic = reinterpret_cast<RpAtomic*>(mi->CreateInstance()); // Trust me, CreateInstace returns an atomic in this case
    const auto frame = RpAtomicGetFrame(atomic);
    const auto mat   = RwFrameGetMatrix(frame);

    // Update it's position
    upgradePosn->m_qRotation.Get(mat);
    RwV3dAssign(RwMatrixGetPos(mat), &upgradePosn->m_vPosition);
    RwMatrixUpdate(mat);

    // Update us and parent frame
    RpClumpAddAtomic(GetRpClump(), atomic);
    RwFrameAddChild(parentComponent, frame);

    mi->AddRef();

    CVisibilityPlugins::SetAtomicId(atomic, isDamaged ? eAtomicComponentFlag::ATOMIC_DAMAGED : eAtomicComponentFlag::ATOMIC_OK);
    CVisibilityPlugins::SetAtomicFlag(atomic, eAtomicComponentFlag::ATOMIC_UPGRADE);
    CVisibilityPlugins::SetAtomicFlag(atomic, eAtomicComponentFlag::ATOMIC_DONT_CULL);

    SetupUpgradeAtomicRendering(atomic, true);

    CDamageAtomicModelInfo::ms_bCreateDamagedVersion = false;
    return atomic;
}

// 0x6D3630
void CVehicle::RemoveUpgrade(int32 upgradeId) {
    // Como el original (0x6D3630): el identificador se pasa como valor en 'data' (RemoveUpgradeCB lo compara directamente), no su dirección
    RpClumpForAllAtomics(GetRpClump(), RemoveUpgradeCB, std::bit_cast<void*>(upgradeId));
}

// 0x6D3650
int32 CVehicle::GetUpgrade(int32 upgradeId) {
    struct { int32 upgradeId; RpAtomic* atomic; } data = { upgradeId, nullptr };
    RpClumpForAllAtomics(GetRpClump(), FindUpgradeCB, &data);
    if (data.atomic) {
        return CVisibilityPlugins::GetModelInfoIndex(data.atomic);
    }

    switch (upgradeId) {
    case 15:
        if (handlingFlags.bNosInst) {
            return ModelIndices::MI_NITRO_BOTTLE_SMALL;
        }
        break;
    case 16:
        if (handlingFlags.bHydraulicInst) {
            return ModelIndices::MI_HYDRAULICS;
        }
        break;
    case 17:
        if (vehicleFlags.bUpgradedStereo) {
            return ModelIndices::MI_STEREO_UPGRADE;
        }
        break;
    }
    return -1;

}

// 0x6D3700
RpAtomic* CVehicle::CreateReplacementAtomic(CBaseModelInfo* mi, RwFrame* parentFrame, eAtomicComponentFlag flags, bool isDamaged, bool bIsWheel) {
    if (isDamaged) {
        CDamageAtomicModelInfo::ms_bCreateDamagedVersion = true;
    }

    const auto atomic = reinterpret_cast<RpAtomic*>(mi->CreateInstance());
    const auto frame = RpAtomicGetFrame(atomic);

    mi->AddRef();

    // Update us and parent frame
    RpClumpAddAtomic(GetRpClump(), atomic);

    if (bIsWheel) {
        // Como el original (0x6D3743): diagonal a 1 y el resto de los 12 floats a 0 (sin tocar pad1/pad2/pad3), flags |= 0x20003 y luego RwMatrixUpdate
        const auto mat = RwFrameGetMatrix(frame);
        mat->at.z = 1.0f;
        mat->up.y = 1.0f;
        mat->right.x = 1.0f;
        const auto matFlags = mat->flags | (rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEMASK);
        mat->up.x = 0.0f;
        mat->right.z = 0.0f;
        mat->right.y = 0.0f;
        mat->at.y = 0.0f;
        mat->at.x = 0.0f;
        mat->up.z = 0.0f;
        mat->pos.z = 0.0f;
        mat->pos.y = 0.0f;
        mat->pos.x = 0.0f;
        mat->flags = matFlags;
        RwMatrixUpdate(mat);
        CVisibilityPlugins::SetFrameHierarchyId(frame, 0);
        RwFrameAddChild(parentFrame, frame);
    } else {
        RpAtomicSetFrame(atomic, parentFrame);
        RwFrameDestroy(frame);
    }

    CVisibilityPlugins::SetAtomicId(atomic, flags & ~eAtomicComponentFlag::ATOMIC_MASK);
    CVisibilityPlugins::SetAtomicFlag(atomic, isDamaged ? eAtomicComponentFlag::ATOMIC_DAMAGED : eAtomicComponentFlag::ATOMIC_OK);

    SetupUpgradeAtomicRendering(atomic, false);

    CDamageAtomicModelInfo::ms_bCreateDamagedVersion = false;

    return atomic;
}

// 0x6D3830
void CVehicle::AddReplacementUpgrade(int32 modelIndex, int32 nodeId) {
    // Como el original (0x6D3830, thiscall, ret 8, sin valor de retorno): primero se vacia el marco del componente (RemoveObjectsCB deja en 'flags' el
    // identificador del ultimo atomo quitado), despues se crea el atomo de la mejora a partir del model info, se cuelga del marco y se configura su render.
    auto* const frame = CClumpModelInfo::GetFrameFromId(GetRpClump(), nodeId);
    int32 flags = 0;
    RwFrameForAllObjects(frame, static_cast<RwObjectCallBack>(RemoveObjectsCB), &flags);
    RwFrameForAllChildren(frame, static_cast<RwFrameCallBack>(RemoveObjectsCB), &flags);

    auto* const mi = CModelInfo::ms_modelInfoPtrs[modelIndex];
    auto* const atomic = reinterpret_cast<RpAtomic*>(mi->CreateInstance());
    mi->AddRef();
    auto* const oldFrame = RpAtomicGetFrame(atomic);
    RpClumpAddAtomic(GetRpClump(), atomic);
    RpAtomicSetFrame(atomic, frame);
    RwFrameDestroy(oldFrame);
    CVisibilityPlugins::SetAtomicId(atomic, flags & ~ATOMIC_MASK);
    CVisibilityPlugins::SetAtomicFlag(atomic, ATOMIC_OK);
    SetupUpgradeAtomicRendering(atomic, false); // 0x6D38D5 - 0x6D390B
    CDamageAtomicModelInfo::ms_bCreateDamagedVersion = false;

    if (nodeId == 0x13) {
        if (m_pHandlingData->m_bDoubleExhaust) {
            // Como el original: el marco del atomo nuevo recibe x = -2 * x del marco del componente (0x858B18 = -2.0f, leido tras la llamada)
            auto* const replacement = CreateReplacementAtomic(mi, frame, static_cast<eAtomicComponentFlag>(flags), false, true);
            auto* const matrix = RwFrameGetMatrix(RpAtomicGetFrame(replacement));
            // x87 del EXE (fld/fmul/fstp, 24 bits): los desnormales se conservan aunque el juego corra con MXCSR FTZ (ayudante de X87Math.cpp, compilado con /arch:IA32)
            AddReplUpgradeExhaustXCpp(&RwFrameGetMatrix(frame)->pos.x, &matrix->pos.x);
            RwMatrixUpdate(matrix);
        }
    } else if (nodeId == 0xC || nodeId == 0xD) {
        auto* const info = static_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[static_cast<int16>(m_nModelIndex)]);
        CCustomCarPlateMgr::SetupClumpAfterVehicleUpgrade(GetRpClump(), info->m_pPlateMaterial, info->m_nPlateType);
    }

    if (mi->AsDamageAtomicModelInfoPtr()) {
        CreateReplacementAtomic(mi, frame, static_cast<eAtomicComponentFlag>(flags), true, false);
        if (frame) {
            RwFrameForAllObjects(frame, static_cast<RwObjectCallBack>(SetVehicleAtomicVisibilityCB), std::bit_cast<void*>(1));
            RwFrameForAllChildren(frame, static_cast<RwFrameCallBack>(SetVehicleAtomicVisibilityCB), std::bit_cast<void*>(1));
        }
    }
}

// 0x6D39E0
void CVehicle::RemoveReplacementUpgrade(int32 frameId) {
    auto frameOfUpgrade = CClumpModelInfo::GetFrameFromId(GetRpClump(), frameId);
    int32 atomicFlags = 0;
    RwFrameForAllObjects(frameOfUpgrade, RemoveObjectsCB, &atomicFlags);
    RwFrameForAllChildren(frameOfUpgrade, RemoveObjectsCB, &atomicFlags);

    // Como el original (0x6D3A1D): primero se busca el marco del modelo y después se guarda el clump destino
    const auto modelFrame = CClumpModelInfo::GetFrameFromId(GetModelInfo()->GetRpClump(), frameId);
    CopyObjectsCB_TargetClump = GetRpClump();
    RwFrameForAllObjects(modelFrame, CopyObjectsCB, frameOfUpgrade);
}

// 0x6D3A50
int32 CVehicle::GetReplacementUpgrade(int32 nodeId) {
    auto frame = CClumpModelInfo::GetFrameFromId(GetRpClump(), nodeId);
    tCompSearchStructById data = { nodeId, nullptr };
    RwFrameForAllObjects(frame, FindReplacementUpgradeCB, &data);
    if (data.m_pFrame)
        return CVisibilityPlugins::GetModelInfoIndex((RpAtomic*)data.m_pFrame);
    else
        return -1;
}

// 0x6D3AB0
void CVehicle::RemoveAllUpgrades() {
    RpClumpForAllAtomics(GetRpClump(), RemoveAllUpgradesCB, nullptr);
    m_anUpgrades.fill(-1);
}

// 0x6D3AE0
int32 CVehicle::GetSpareHasslePosId() const {
    const auto numberOfPositions = [&] {
        switch (m_nVehicleSubType) {
        case eVehicleType::VEHICLE_TYPE_BIKE:
        case eVehicleType::VEHICLE_TYPE_BMX:
        case eVehicleType::VEHICLE_TYPE_QUAD:
            return 2;
        case eVehicleType::VEHICLE_TYPE_AUTOMOBILE:
            return 6;
        default:
            return 0;
        }
    }();

    for (auto i = 0; i < numberOfPositions; i++) {
        if ((m_nHasslePosId  & (1 << i)) == 0) {
            return i;
        }
    }

    return -1;
}

// 0x6D3B30
void CVehicle::SetHasslePosId(int32 hasslePos, bool enable) {
    if (enable) {
        m_nHasslePosId |= 1 << hasslePos;
    } else {
        m_nHasslePosId ^= 1 << hasslePos;
    }
}

// 0x6D3B60
void CVehicle::InitWinch(int32 winchType) {
    m_ropeType = winchType;
}

// 0x6D3B80
void CVehicle::UpdateWinch() {
    if (!m_ropeType) {
        return;
    }

    const auto ropeID = GetRopeID();

    const auto GetZAndSegmentCount = [&, this]() -> std::pair<float, int32> {
        const auto baseLen = (eRopeType)m_ropeType == eRopeType::MAGNET ? -0.2f : -0.6f;
        if (const auto ropeIdx = CRopes::FindRope(ropeID); ropeIdx >= 0) { // Inverted condition
            const auto& rope = CRopes::GetRope(ropeIdx);
            // Como el original: _ftol2 (entero con signo, trunca) y fild al restar
            const auto segCount = VehEFtol(rope.m_fSegmentLength * 32.f);
            return { (rope.m_fMass * rope.m_fSegmentLength) - ((float)segCount * rope.m_fTotalLength) + baseLen, segCount };
        }
        return { baseLen, 0 };
    };

    const auto [pointZ, segCount] = GetZAndSegmentCount();
    CRopes::RegisterRope(
        ropeID,
        m_ropeType,
        VehEMatPoint(*m_matrix, CVector{ 0.f, 0.f, pointZ }), // Como el original: 0x59C890 con el puntero de matriz, sin reservarla
        false,
        (uint8)segCount,
        1u,
        this,
        20'000u
    );
}

// 0x6D3C70
void CVehicle::RemoveWinch() {
    // Como el original: la cuerda se busca con this + 0xA320 (el EXE usa &this[29] de un CVehicle de 0x5A0 bytes) y no con GetRopeID();
    // la funcion no se usa en el juego
    if (const auto ropeIdx = CRopes::FindRope((uint32)((uintptr_t)this + 0xA320)); ropeIdx >= 0) {
        CRopes::GetRope(ropeIdx).Remove();
    }

    m_ropeType = 0;
}

CVector CVehicle::GetDriverSeatDummyPositionOS() const {
    return GetDummyPositionObjSpace(
        IsBoat() ? DUMMY_LIGHT_FRONT_MAIN : DUMMY_SEAT_FRONT
    );
}

CVector CVehicle::GetDriverSeatDummyPositionWS() {
    return GetMatrix().TransformPoint(GetDriverSeatDummyPositionOS());
}

CVehicleAnimGroup& CVehicle::GetAnimGroup() const {
    return CVehicleAnimGroupData::GetVehicleAnimGroup(m_pHandlingData->m_nAnimGroup);
}

AssocGroupId CVehicle::GetAnimGroupId() const {
    return m_pHandlingData->GetAnimGroupId();
}

// 0x6D3CB0
void CVehicle::ReleasePickedUpEntityWithWinch() const {
    return CRopes::GetRope(CRopes::FindRope(GetRopeID())) /* Como el original: FindRope sin comprobar el resultado */.ReleasePickedUpObject();
}

// 0x6D3CD0
void CVehicle::PickUpEntityWithWinch(CEntity* entity) const {
    return CRopes::GetRope(CRopes::FindRope(GetRopeID())) /* Como el original: FindRope sin comprobar el resultado */.PickUpObject(entity);
}

// 0x6D3CF0
CEntity* CVehicle::QueryPickedUpEntityWithWinch() const {
    return CRopes::GetRope(CRopes::FindRope(GetRopeID())) /* Como el original: FindRope sin comprobar el resultado */.m_pRopeAttachObject;
}

// 0x6D3D10
float CVehicle::GetRopeHeightForHeli() const {
    return CRopes::GetRope(CRopes::FindRope(GetRopeID())) /* Como el original: FindRope sin comprobar el resultado */.m_fSegmentLength;
}

// 0x6D3D30
void CVehicle::SetRopeHeightForHeli(float height) const {
    CRopes::GetRope(CRopes::FindRope(GetRopeID())) /* Como el original: FindRope sin comprobar el resultado */.m_fSegmentLength = height;
}

// 0x6D3D60
void CVehicle::RenderDriverAndPassengers() {
    if (m_pDriver && m_pDriver->m_nPedState == PEDSTATE_DRIVING) {
        m_pDriver->Render();
    }

    for (auto& passenger : m_apPassengers) {
        if (passenger && passenger->m_nPedState == PEDSTATE_DRIVING) {
            passenger->Render();
        }
    }
}

// 0x6D3DB0
void CVehicle::PreRenderDriverAndPassengers() {
    if (m_pDriver && m_pDriver->m_nPedState == PEDSTATE_DRIVING) {
        m_pDriver->PreRenderAfterTest();
    }

    for (auto& passenger : m_apPassengers) {
        if (passenger && passenger->m_nPedState == PEDSTATE_DRIVING) {
            passenger->PreRenderAfterTest();
        }
    }
}

// 0x6D3E00
float CVehicle::GetPlaneGunsAutoAimAngle() {
    switch (m_nModelIndex) {
    case MODEL_SEASPAR:  return 10.0f;
    case MODEL_RUSTLER:
    case MODEL_MAVERICK:
    case MODEL_POLMAV:
    case MODEL_HYDRA:
    case MODEL_CARGOBOB:
    case MODEL_TORNADO:  return 15.0f;
    case MODEL_RCTIGER:  return 20.0f;
    case MODEL_HUNTER:   return 25.0f;
    case MODEL_RCBARON:  return 30.0f;
    default:
        return 0.0f;
    }
}

// 0x6D3F30
int32 CVehicle::GetPlaneNumGuns() {
    switch (m_nModelIndex) {
    case MODEL_HUNTER:
    case MODEL_SEASPAR:
    case MODEL_RCBARON:
    case MODEL_MAVERICK:
    case MODEL_POLMAV:
    case MODEL_CARGOBOB:
        return 1;
    case MODEL_RUSTLER:
        return 6;
    case MODEL_HYDRA:
    case MODEL_TORNADO:
        return 2;
    default:
        return 0;
    }
}

// 0x6D4010
void CVehicle::SetFiringRateMultiplier(float multiplier) {
    multiplier = std::clamp(multiplier, 0.0f, 15.9375f);
    switch (m_nVehicleSubType) {
    case VEHICLE_TYPE_PLANE:
        AsPlane()->m_nFiringMultiplier = uint8(multiplier * 16.0f);
        break;
    case VEHICLE_TYPE_HELI:
        AsHeli()->m_nFiringMultiplier = uint8(multiplier * 16.0f);
        break;
    }
}

// 0x6D4090
float CVehicle::GetFiringRateMultiplier() {
    switch (m_nVehicleSubType) {
    case VEHICLE_TYPE_PLANE:
        return float(AsPlane()->m_nFiringMultiplier) / 16.0f;
    case VEHICLE_TYPE_HELI:
        return float(AsHeli()->m_nFiringMultiplier) / 16.0f;
    default:
        return 1.0f;
    }
}

// 0x6D40E0
uint32 CVehicle::GetPlaneGunsRateOfFire() {
    // Como el original: _ftol2 (trunca a int64) y se devuelve la palabra baja
    const auto mult = GetFiringRateMultiplier();
    switch (m_nModelIndex) {
    case MODEL_SEASPAR:
    case MODEL_RCBARON:
        return uint32(VehEFtol(40.0f / mult));
    case MODEL_RUSTLER:
        return uint32(VehEFtol(80.0f / mult));
    case MODEL_HYDRA:
        return uint32(VehEFtol(17.0f / mult));
    case MODEL_CARGOBOB:
        return uint32(VehEFtol(100.0f / mult));
    case MODEL_TORNADO:
        return uint32(VehEFtol(45.0f / mult));
    default:
        return uint32(VehEFtol(60.0f / mult));
    }
}

// 0x6D4290
CVector CVehicle::GetPlaneGunsPosition(int32 gunId) {
    CVector pos = GetModelInfo()->AsVehicleModelInfoPtr()->GetModelDummyPosition(DUMMY_VEHICLE_GUN);
    if (pos.IsZero()) {
        switch (m_nModelIndex) {
        case MODEL_HUNTER:   pos = VehicleGunOffset[0]; break;
        case MODEL_SEASPAR:  pos = CVector{ -0.5f, 2.4f, -0.785f }; break; // 0x8D35F8
        case MODEL_RCBARON:  pos = CVector{ 0.0f, 0.45f, 0.0f }; break;    // 0x8D3634
        case MODEL_RUSTLER:  pos = CVector{ 2.19f, 1.5f, -0.58f }; break;  // 0x8D3610
        case MODEL_MAVERICK: pos = CVector{ 0.0f, 2.85f, -0.5f }; break;   // 0x8D35E0
        case MODEL_POLMAV:   pos = CVector{ 0.0f, 2.85f, -0.5f }; break;   // 0x8D35EC [Values same as the maverick's]
        case MODEL_HYDRA:    pos = CVector{ 1.48f, 0.44f, -0.52f }; break; // 0x8D3628
        case MODEL_CARGOBOB: pos = CVector{ 0.0f, 6.87f, -1.65f }; break;  // 0x8D3604
        case MODEL_TORNADO:  pos = StaticRef<CVector>(0xC1CC2C); break;    // Como el original: se lee la global (a cero)
        default:             pos = CVector{ 0.f, 0.f, 0.f }; break;
        }
    }

    // Como el original: el desplazamiento del Rustler (0x8D361C = 0.2, 0, 0) por (gunId - 1) se suma TAMBIEN a la posicion del dummy del modelo
    if (m_nModelIndex == MODEL_RUSTLER) {
        const auto n = (float)(gunId - 1);
        pos.x = (0.2f * n) + pos.x;
        pos.y = (0.0f * n) + pos.y;
        pos.z = (0.0f * n) + pos.z;
    }
    return pos;
}

// 0x6D4590
uint32 CVehicle::GetPlaneOrdnanceRateOfFire(eOrdnanceType type) {
    // Como el original: _ftol2 (trunca a int64) y se devuelve la palabra baja
    const auto mult = GetFiringRateMultiplier();
    switch (m_nModelIndex) {
    case MODEL_HUNTER:
        return (uint32)VehEFtol((float)500 / mult);
    case MODEL_SEASPAR:
    case MODEL_RUSTLER:
        return (uint32)VehEFtol((float)1000 / mult);
    case MODEL_HYDRA:
        return (uint32)VehEFtol((float)(type != 1 ? 1000 : 500) / mult); // todo: heat trap / missile?
    default:
        return (uint32)VehEFtol((float)350 / mult);
    }
}

// 0x6D46E0
CVector CVehicle::GetPlaneOrdnancePosition(eOrdnanceType type) {
    // CVector result;
    // ((void(__thiscall*)(CVehicle*, CVector*, eOrdnanceType))0x6D46E0)(this, &result, type);
    // return result;

    const auto pos = GetVehicleModelInfo()->GetModelDummyPosition(DUMMY_VEHICLE_GUN);
    if (!pos.IsZero()) {
        return pos;
    }

    constexpr CVector HUNTER_ORDNANCE_POS  = { 2.17f, 1.00f, -0.80f }; // 0x8D3640
    constexpr CVector RUSTLER_ORDNANCE_POS = { 2.19f, 1.50f, -0.58f }; // 0x8D364C
    constexpr CVector HYDRA_1_ORDNANCE_POS = { 3.70f, 0.98f, -1.02f }; // 0x8D3658
    constexpr CVector HYDRA_2_ORDNANCE_POS = { 3.92f, 0.98f, -1.02f }; // 0x8D3664

    // Como el original: se leen las globales (a cero)
    const CVector SEASPAR_ORDNANCE_POS = StaticRef<CVector>(0xC1CC38);
    const CVector RCBARON_ORDNANCE_POS = StaticRef<CVector>(0xC1CC50);
    const CVector TORNADO_ORDNANCE_POS = StaticRef<CVector>(0xC1CC44);

    switch (m_nModelIndex) {
    case MODEL_HUNTER:  return HUNTER_ORDNANCE_POS;
    case MODEL_SEASPAR: return SEASPAR_ORDNANCE_POS;
    case MODEL_RCBARON: return RCBARON_ORDNANCE_POS;
    case MODEL_RUSTLER: return RUSTLER_ORDNANCE_POS;
    case MODEL_HYDRA:
        if (type == 1) {
            return HYDRA_1_ORDNANCE_POS;
        } else if (type == 2) {
            return HYDRA_2_ORDNANCE_POS;
        }
        return pos;
    case MODEL_TORNADO: return TORNADO_ORDNANCE_POS;
    default:
        return {};
    }
}

// 0x6D4900
void CVehicle::SelectPlaneWeapon(bool bChange, eOrdnanceType type) {
    const auto GetWeaponToUse = [&, this] {
        switch (m_nModelIndex) {
        case MODEL_TORNADO: // Originally a separate case at the bottom, but since they both do the same, I moved it here.
        case MODEL_HUNTER:
            if (type == 1) {
                return CAR_WEAPON_DOUBLE_ROCKET;
            } else {
                return bChange ? CAR_WEAPON_HEAVY_GUN : m_nVehicleWeaponInUse;
            }
        case MODEL_SEASPAR:
        case MODEL_RCBARON:
            return bChange ? CAR_WEAPON_HEAVY_GUN : m_nVehicleWeaponInUse;

        case MODEL_RUSTLER:
            if (type == 1) {
                return CAR_WEAPON_FREEFALL_BOMB;
            } else {
                return bChange ? CAR_WEAPON_HEAVY_GUN : m_nVehicleWeaponInUse;
            }
        case MODEL_HYDRA: {
            switch (type) {
            case 1:
                return CAR_WEAPON_DOUBLE_ROCKET;
            case 2:
                return CAR_WEAPON_LOCK_ON_ROCKET;
            default:
                return bChange ? CAR_WEAPON_HEAVY_GUN : m_nVehicleWeaponInUse;
            }
        }
        default:
            return m_nVehicleWeaponInUse;
        }
    };
    m_nVehicleWeaponInUse = GetWeaponToUse();
}

// 0x6D4AD0
void CVehicle::DoPlaneGunFireFX(CWeapon* weapon, CVector& particlePos, CVector& gunshellPos, int32 fxIdx) {
    const auto DoFx = [&](auto& parts) {
        if (!parts) {
            const auto nguns = GetPlaneNumGuns();
            parts = new FxSystem_c*[nguns];
            // Como el original (0x6D4B30): bucle do-while que vuelve a llamar a GetPlaneNumGuns en cada vuelta y escribe al menos una vez.
            // (0x6D4B23: antes del bucle vuelve a llamar a GetPlaneNumGuns y lo salta si es <= 0)
            if ((int32)GetPlaneNumGuns() > 0) {
                for (int32 i = 0; ; ) {
                    parts[i] = nullptr;
                    i++;
                    if (!(i < (int32)GetPlaneNumGuns())) {
                        break;
                    }
                }
            }
        }

        if (parts) {
            auto& part = parts[fxIdx];
            if (!part) {
                // Como el original: matriz nula si no hay clump
                part = g_fxMan.CreateFxSystem(
                    "gunflash",
                    CVector{0.f, 0.f, 0.f},
                    GetRpClump() ? RwFrameGetMatrix(RpClumpGetFrame(GetRpClump())) : nullptr,
                    false
                );
            }
            if (part) {
                part->SetOffsetPos(particlePos);
                part->Play();
            }
        }

        if (s_bPlaneGunsEjectShellCasings) {
            weapon->AddGunshell(this, gunshellPos, { 0.f, 0.1f }, 0.5f);
        }

        CPointLights::AddLight(0, gunshellPos, { 0.f, 0.f, 0.f }, 3.f, 0.5f, 0.4f, 0.f);
    };


    switch (m_nVehicleSubType) {
    case VEHICLE_TYPE_PLANE: {
        DoFx(AsPlane()->m_pGunParticles);
        break;
    }
    case VEHICLE_TYPE_HELI: {
        DoFx(AsHeli()->m_ppGunflashFx); // Como el original (0x6D4BC4): el array de fogonazos del helicóptero está en +0xA0C, no en m_pParticlesList (+0x9D4)
        break;
    }
    default: {
        return;
    }
    }
}

// 0x6D4D30
void CVehicle::FirePlaneGuns() {
    if (!GetPlaneNumGuns()) {
        return;
    }

    if (CTimer::GetTimeInMS() <= m_nGunFiringTime + GetPlaneGunsRateOfFire()) {
        return;
    }

    // Como el original: (es doble, frecuencia del temblor, arma) por modelo; el Rustler avanza el ciclo de ametralladoras antes de crear el arma
    bool        isDoubleGun{};
    uint8       freq{};
    eWeaponType weaponType{};
    switch (m_nModelIndex) {
    case MODEL_HUNTER:
    case MODEL_CARGOBOB:
        isDoubleGun = false;
        freq        = 160u;
        weaponType  = WEAPON_MINIGUN;
        break;
    case MODEL_SEASPAR:
    case MODEL_RCBARON:
    case MODEL_MAVERICK:
    case MODEL_POLMAV:
        isDoubleGun = false;
        freq        = 92u;
        weaponType  = WEAPON_M4;
        break;
    case MODEL_RUSTLER:
        if (m_nGunsCycleIndex + 1 == 3) {
            m_nGunsCycleIndex = 0;
        } else {
            m_nGunsCycleIndex++;
        }
        isDoubleGun = true;
        freq        = 224u; // 0xE0
        weaponType  = WEAPON_M4;
        break;
    case MODEL_HYDRA:
    case MODEL_TORNADO:
        isDoubleGun = true;
        freq        = 128u; // 0x80
        weaponType  = WEAPON_MINIGUN;
        break;
    default:
        return;
    }

    CWeapon weapon{ weaponType, 5000 };

    auto planeGunPosMS = GetPlaneGunsPosition(m_nGunsCycleIndex); // MS = Model Space

    // Como el original: velocidad * paso + matriz * posicion (0x59C890)
    const auto CalcGunShellPos = [&]() {
        const CVector velocityOffset{
            CTimer::GetTimeStep() * m_vecMoveSpeed.x,
            CTimer::GetTimeStep() * m_vecMoveSpeed.y,
            CTimer::GetTimeStep() * m_vecMoveSpeed.z,
        };
        const auto transformed = VehEMatPoint(*m_matrix, planeGunPosMS);
        return CVector{ velocityOffset.x + transformed.x, velocityOffset.y + transformed.y, velocityOffset.z + transformed.z };
    };

    // Como el original: primero se dispara y despues se dibuja el efecto; la segunda ametralladora usa la posicion con x negada
    auto gunShellPos = CalcGunShellPos();
    weapon.FireInstantHit(this, &gunShellPos, &gunShellPos);
    if (!isDoubleGun) {
        DoPlaneGunFireFX(&weapon, planeGunPosMS, gunShellPos, m_nGunsCycleIndex);
    } else {
        DoPlaneGunFireFX(&weapon, planeGunPosMS, gunShellPos, m_nGunsCycleIndex * 2);

        const auto secondGunPos = GetPlaneGunsPosition(m_nGunsCycleIndex);
        planeGunPosMS = CVector{ -secondGunPos.x, secondGunPos.y, secondGunPos.z };
        gunShellPos   = CalcGunShellPos();
        weapon.FireInstantHit(this, &gunShellPos, &gunShellPos);
        DoPlaneGunFireFX(&weapon, planeGunPosMS, gunShellPos, m_nGunsCycleIndex * 2 + 1);
    }

    // Shake pad (if necessary)
    // Como el original: mando 0 con el control remoto; si no, el del conductor jugador (solo tipos de peaton 0 y 1)
    if (GetStatus() == STATUS_REMOTE_CONTROLLED) {
        CPad::GetPad(0)->StartShake(240, freq, 0);
    } else if (m_pDriver && m_pDriver->IsPlayer()) {
        if (m_pDriver->m_nPedType == PED_TYPE_PLAYER1) {
            CPad::GetPad(0)->StartShake(240, freq, 0);
        } else if (m_pDriver->m_nPedType == PED_TYPE_PLAYER2) {
            CPad::GetPad(1)->StartShake(240, freq, 0);
        }
    }

    AudioEngine.ReportWeaponEvent(AE_WEAPON_FIRE_PLANE, weaponType == WEAPON_MINIGUN ? WEAPON_M4 : weaponType, this); // Unsure about why the change the M4 => minigun but okay

    m_nGunFiringTime = CTimer::GetTimeInMS();
}

// 0x6D5110
void CVehicle::FireUnguidedMissile(eOrdnanceType type, bool bCheckTime) {
    auto& firingTimeForOrdnanceType = type == 1 ? m_nProjectileWeaponFiringTime : m_nAdditionalProjectileWeaponFiringTime;

    if (bCheckTime) {
        if (CTimer::GetTimeInMS() <= firingTimeForOrdnanceType + GetPlaneOrdnanceRateOfFire(type)) {
            return;
        }
    }

    switch (m_nModelIndex) {
    case MODEL_HUNTER:
    case MODEL_HYDRA:
    case MODEL_TORNADO:
        break;
    default:
        return;
    }

    CWeapon weapon{ WEAPON_RLAUNCHER, 5000 };

    // Como el original: el segundo misil sale del lado opuesto (la x de la posicion se niega)
    for (auto i = 0; i < 2; i++) {
        auto ordnancePosMS = GetPlaneOrdnancePosition(type);
        if (i == 1) {
            ordnancePosMS.x = -ordnancePosMS.x;
        }

        const auto& mat          = *m_matrix;
        const auto  ordnancePos  = VehEMatPoint(mat, ordnancePosMS);
        // This places a point somewhere in front of us, depending on our velocity's direction
        // Como el original: max(0, dot) con el dot en el orden z, y, x; (adelante * velocidad) * paso, y despues se suma la posicion
        const auto  dot          = VehEDot(m_vecMoveSpeed, mat.GetForward());
        const auto  forwardSpeed = dot < 0.0f ? 0.0f : dot;
        const CVector origin{
            ((mat.GetForward().x * forwardSpeed) * CTimer::GetTimeStep()) + ordnancePos.x,
            ((mat.GetForward().y * forwardSpeed) * CTimer::GetTimeStep()) + ordnancePos.y,
            ((mat.GetForward().z * forwardSpeed) * CTimer::GetTimeStep()) + ordnancePos.z,
        };
        weapon.FireProjectile(this, origin);
    }

    // Como el original: solo los tipos de peaton 0 y 1 del conductor jugador, un solo temblor tras los dos misiles
    if (m_pDriver && m_pDriver->IsPlayer()) {
        if (m_pDriver->m_nPedType == PED_TYPE_PLAYER1) {
            CPad::GetPad(0)->StartShake(240, 160u, 0);
        } else if (m_pDriver->m_nPedType == PED_TYPE_PLAYER2) {
            CPad::GetPad(1)->StartShake(240, 160u, 0);
        }
    }

    firingTimeForOrdnanceType = CTimer::GetTimeInMS();
}

// 0x6D5400
bool CVehicle::CanBeDriven() const {
    if (IsSubTrailer() || IsSubTrain() && AsTrain()->m_nTrackId || vehicleFlags.bIsRCVehicle) {
        return false;
    }
    // Como el original (0x6D5400): el hueco (0 o 4) lo decide el tipo del model info (5 = barco), no el tipo del vehiculo.
    const auto* const mi = GetVehicleModelInfo();
    const auto& dummy = mi->GetModelDummyPosition(mi->m_nVehicleType == VEHICLE_TYPE_BOAT ? DUMMY_LIGHT_FRONT_MAIN : DUMMY_SEAT_FRONT);
    // Como el original: los cuadrados se acumulan en registros x87 (exponente ampliado), asi que un componente diminuto (p. ej. 1e-30f)
    // no se redondea a 0 como en float; se usa double para no perder esos valores.
    return (double)dummy.z * dummy.z + (double)dummy.y * dummy.y + (double)dummy.x * dummy.x > 0.0;
}

// 0x6D5490
void CVehicle::ReactToVehicleDamage(CPed* dmgCauser) {
    // Como el original: el azar se consume en este orden: tiempo del conductor (aunque no haya conductor), cara o cruz
    // (solo con conductor y pasajero), tiempo del pasajero (siempre) y cara o cruz del pasajero. El tiempo es
    // 2000 + (int)(rand() * (1/32768) * 3000) (GetRandomIntInRange), no el reparto de GetRandomNumberInRange.
    const auto Look = [](CPed* pedThatLooks, CEntity* pedToLookAt, int32 time) {
        g_ikChainMan.LookAt("ReactToVhclDam", pedThatLooks, pedToLookAt, time, BONE_HEAD, nullptr, false, 0.25f, 500, 3, false);
    };

    int32 time = CGeneral::GetRandomIntInRange(2000, 5000);
    if (m_pDriver) {
        if (m_apPassengers.data()[0]) {
            const bool flip = CGeneral::DoCoinFlip();
            Look(m_pDriver, flip ? dmgCauser : m_apPassengers.data()[0], time);
        } else {
            Look(m_pDriver, dmgCauser, time);
        }
    }

    time = CGeneral::GetRandomIntInRange(2000, 5000);
    if (m_apPassengers.data()[0]) {
        const bool flip = CGeneral::DoCoinFlip();
        Look(m_apPassengers.data()[0], flip ? dmgCauser : m_pDriver, time);
    }
}

// 0x6D55C0
bool CVehicle::GetVehicleLightsStatus() {
    // Como el original (0x6D55C0): mismo orden de evaluacion (CamNoRain se consulta aunque ya haya luces)
    const auto hours   = CClock::GetGameClockHours();
    const auto minutes = CClock::GetGameClockMinutes();
    const uint16 seedLow6 = m_nRandomSeed & 0x3F;

    bool lights = false;
    if (hours > 20) {
        lights = true;
    } else if (hours > 19 && static_cast<uint16>(minutes) > seedLow6) {
        lights = true;
    } else if (hours < 6) {
        lights = true;
    } else if (hours < 7 && static_cast<uint16>(minutes) < seedLow6) {
        lights = true;
    } else {
        const auto treshold = static_cast<float>(m_nRandomSeed) * std::bit_cast<float>(0x37A7C5ACu); // 0x871E78
        if (CWeather::Foggyness > treshold || CWeather::WetRoads > treshold) {
            lights = true;
        }
    }

    if (m_fContactSurfaceBrightness < 0.05f && CCullZones::CamNoRain()) {
        lights = true;
    }

    // Las bandas no usan luces en su zona
    if (lights
        && m_pDriver
        && static_cast<int32>(m_pDriver->m_nPedType) >= PED_TYPE_GANG1
        && static_cast<int32>(m_pDriver->m_nPedType) <= PED_TYPE_GANG10
        && (m_pDriver->m_nRandomSeed & 1)
        && CPopCycle::m_bCurrentZoneIsGangArea
    ) {
        return false;
    }

    return lights;
}

// 0x6D5CF0
bool CVehicle::CanPedLeanOut(CPed* ped) {
    switch (m_pHandlingData->m_nAnimGroup) {
    case ANIM_GROUP_COLT45:
        return notsa::contains(std::array{ m_pDriver, m_apPassengers[0] }, ped);
    case ANIM_GROUP_COLT_COP:
    case ANIM_GROUP_COLT45PRO:
    case ANIM_GROUP_SAWNOFF:
    case ANIM_GROUP_SAWNOFFPRO:
    case ANIM_GROUP_SILENCED:
        return false;
    default: {
        switch (m_nVehicleSubType) {
        case VEHICLE_TYPE_HELI:
        case VEHICLE_TYPE_PLANE:
        case VEHICLE_TYPE_TRAIN:
        case VEHICLE_TYPE_BOAT:
            return false;
        default:
            return true;
        }
    }
    }
}

// 0x6D5D70
void CVehicle::SetVehicleCreatedBy(eVehicleCreatedBy createdBy) {
    if (GetCreatedBy() != createdBy) {
        CCarCtrl::UpdateCarCount(this, true);
        m_nCreatedBy = createdBy;
        CCarCtrl::UpdateCarCount(this, false);
    }
}

// unknown
float CVehicle::GetNewSteeringAmt() {
    return 0.0f;
}

// 0x6D64F0
void CVehicle::SetupRender() {
    const auto mi = GetModelInfo()->AsVehicleModelInfoPtr();

    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(TRUE));

    if (IsAutomobile()) {
        AsAutomobile()->CustomCarPlate_BeforeRenderingStart(*mi);
    }

    // Handle remap TXD
    if (m_nRemapTxd < 0) {
        vehicleFlags.bDontSetColourWhenRemapping = false;
    } else {
        // Make sure it's loaded, if not, request it to be loaded
        if (CStreaming::IsModelLoaded(TXDToModelId(m_nRemapTxd))) {
            // If there was a remap texture set, remove it
            if (m_pRemapTexture) {
                m_pRemapTexture = nullptr;
                CTxdStore::RemoveRef(m_nPreviousRemapTxd);
            }

            // Add ref to current txd
            CTxdStore::AddRef(m_nRemapTxd);

            m_nPreviousRemapTxd = m_nRemapTxd;

            // Set this to -1. From the looks of it, this
            // should be set during the frame or something
            // and then it's loaded here
            // instead of having to load the txd and all that mid-frame
            m_nRemapTxd = -1;

            // Store texture of current txd
            m_pRemapTexture = GetFirstTexture(CTxdStore::GetTxd(m_nPreviousRemapTxd));

            if (!vehicleFlags.bDontSetColourWhenRemapping) {
                m_nPrimaryColor = 1;
            }
        } else {
            CStreaming::RequestModel(TXDToModelId(m_nRemapTxd), STREAMING_KEEP_IN_MEMORY);
        }
    }

    mi->SetVehicleColour(
        m_nPrimaryColor,
        m_nSecondaryColor,
        m_nTertiaryColor,
        m_nQuaternaryColor
    );

    CVehicleModelInfo::SetupLightFlags(this);
    CVehicleModelInfo::ms_pRemapTexture = m_pRemapTexture;
    CVehicleModelInfo::SetEditableMaterials(GetRpClump());
}

// 0x6D6C00
void CVehicle::ProcessWheel(CVector& wheelFwd, CVector& wheelRight,
                            CVector& wheelContactSpeed, CVector& wheelContactPoint,
                            int32 wheelsOnGround,
                            float thrust, float brake, float adhesion,
                            int8 wheelId, float* wheelSpeed,
                            tWheelState* wheelState, uint16 wheelStatus
) {
    static auto& bBraking = StaticRef<bool>(0xC1CDAE); // false
    static auto& bDriving = StaticRef<bool>(0xC1CDAD); // false
    static auto& bAlreadySkidding = StaticRef<bool>(0xC1CDAC); // false

    float right = 0.0f;
    float fwd = 0.0f;
    // Como el original: los productos escalares se suman en el orden del EXE ((y + x) + z para fwd, (y + z) + x para right)
    float contactSpeedFwd = ((wheelFwd.y * wheelContactSpeed.y) + (wheelFwd.x * wheelContactSpeed.x)) + wheelContactSpeed.z * wheelFwd.z;
    float contactSpeedRight = ((wheelRight.y * wheelContactSpeed.y) + (wheelRight.z * wheelContactSpeed.z)) + wheelRight.x * wheelContactSpeed.x;

    bBraking = brake != 0.0f;
    bDriving = !bBraking;
    if (bDriving && thrust == 0.0f)
        bDriving = false;

    adhesion *= CTimer::GetTimeStep();
    if (*wheelState != WHEEL_STATE_NORMAL) {
        bAlreadySkidding = true;
        adhesion *= m_pHandlingData->m_fTractionLoss;
        if (*wheelState == WHEEL_STATE_SPINNING) {
            if (GetStatus() == STATUS_PLAYER || GetStatus() == STATUS_REMOTE_CONTROLLED)
                adhesion *= (1.0f - fabs(m_GasPedal) * WS_ALREADY_SPINNING_LOSS);
        }
    }

    *wheelState = WHEEL_STATE_NORMAL;

    if (contactSpeedRight != 0.0f) {
        right = -(contactSpeedRight / wheelsOnGround);
        if (wheelStatus == WHEEL_STATUS_BURST) {
            float fwdspeed = std::min(contactSpeedFwd, fBurstSpeedMax);
            right += fwdspeed * CGeneral::GetRandomNumberInRange(-fBurstTyreMod, fBurstTyreMod) ;
        }
    }

    if (bDriving) {
        fwd = thrust;
        // Como el original: sin std::clamp (el EXE no comprueba que adhesion sea positiva)
        if (right > 0.0f) {
            if (!(right <= adhesion)) {
                right = adhesion;
            }
        } else if (!(right >= -adhesion)) {
            right = -adhesion;
        }
    }
    else if (contactSpeedFwd != 0.0f) {
        fwd = -contactSpeedFwd / wheelsOnGround;
        if (!bBraking && std::fabs(m_GasPedal) < 0.01f) {
            if (IsBike())
                brake = gHandlingDataMgr.fWheelFriction * 0.6f / (m_pHandlingData->m_fMass + 200.0f);
            else if (IsSubPlane())
                brake = 0.0f;
            else {
                brake = gHandlingDataMgr.fWheelFriction / m_pHandlingData->m_fMass;

                // Como el original (0x6D6EA2): compara la masa (no el freno) con 500
                if (m_pHandlingData->m_fMass < 500.0f)
                    brake *= 0.1f;
                else if (m_nModelIndex == MODEL_RCBANDIT)
                    brake *= 0.2f;
            }
        }
        if (brake > adhesion) {
            if (std::fabs(contactSpeedFwd) > 0.005f) {
                *wheelState = WHEEL_STATE_FIXED;
            }
        } else if (fwd > 0.0f) {
            // Como el original: sin std::clamp
            if (!(fwd <= brake)) {
                fwd = brake;
            }
        } else if (!(fwd >= -brake)) {
            fwd = -brake;
        }
    }

    float speedSq = right * right + fwd * fwd;
    if (speedSq > adhesion * adhesion) {
        if (*wheelState != WHEEL_STATE_FIXED) {
            float tractionLimit = WS_TRAC_FRAC_LIMIT;
            if (contactSpeedFwd > 0.15f && (!wheelId || wheelId == CAR_WHEEL_FRONT_RIGHT)) {
                tractionLimit += tractionLimit;
            }
            if (bDriving && tractionLimit * adhesion < std::fabs(fwd))
                *wheelState = WHEEL_STATE_SPINNING;
            else
                *wheelState = WHEEL_STATE_SKIDDING;
        }
        float tractionLoss = m_pHandlingData->m_fTractionLoss;
        if (bAlreadySkidding) {
            tractionLoss = 1.0f;
        } else if (*wheelState == WHEEL_STATE_SPINNING) {
            if (GetStatus() == STATUS_PLAYER || GetStatus() == STATUS_REMOTE_CONTROLLED) {
                tractionLoss = tractionLoss * (1.0f - std::fabs(m_GasPedal) * WS_ALREADY_SPINNING_LOSS);
            }
        }
        float l = sqrt(speedSq);
        // Como el original: factor = (tractionLoss / l) * adhesion
        const float factor = (tractionLoss / l) * adhesion;
        fwd *= factor;
        right *= factor;
    }

    if (fwd != 0.0f || right != 0.0f) {
        bool separateTurnForce = false;
        CVector totalSpeed = fwd * wheelFwd + right * wheelRight;
        CVector turnDirection  = totalSpeed;

        if (m_pHandlingData->m_fSuspensionAntiDiveMultiplier > 0.0f) {
            if (bBraking) {
                separateTurnForce = true;
                turnDirection -= (m_pHandlingData->m_fSuspensionAntiDiveMultiplier * wheelFwd * fwd);
            }
            else if (bDriving) {
                separateTurnForce = true;
                turnDirection -= (0.5f * m_pHandlingData->m_fSuspensionAntiDiveMultiplier * wheelFwd * fwd);
            }
        }

        // Como el original: modulos sumados en orden (z + y) + x y normalizacion con 1 / modulo (si no es > 0 solo x = 1)
        CVector direction = totalSpeed;
        float speed = std::sqrt((totalSpeed.z * totalSpeed.z + totalSpeed.y * totalSpeed.y) + totalSpeed.x * totalSpeed.x);
        float turnSpeed = speed;
        if (separateTurnForce)
            turnSpeed = std::sqrt((turnDirection.z * turnDirection.z + turnDirection.y * turnDirection.y) + turnDirection.x * turnDirection.x);
        if (speed > 0.0f) {
            const float recip = 1.0f / speed;
            direction.x = totalSpeed.x * recip;
            direction.y = totalSpeed.y * recip;
            direction.z = totalSpeed.z * recip;
        } else {
            direction.x = 1.0f;
        }
        if (separateTurnForce) {
            if (turnSpeed > 0.0f) {
                const float recip = 1.0f / turnSpeed;
                turnDirection.x = turnDirection.x * recip;
                turnDirection.y = turnDirection.y * recip;
                turnDirection.z = turnDirection.z * recip;
            } else {
                turnDirection.x = 1.0f;
            }
        } else {
            turnDirection = direction;
        }

        float force = speed * m_fMass;
        float turnForce = turnSpeed * GetMass(wheelContactPoint, turnDirection);
        ApplyMoveForce(force * direction);
        ApplyTurnForce(turnForce * turnDirection, wheelContactPoint);
    }
}

// 0x6D73B0
void CVehicle::ProcessBikeWheel(CVector& wheelFwd, CVector& wheelRight, CVector& wheelContactSpeed, CVector& wheelContactPoint, int32 wheelsOnGround, float thrust, float brake,
                                float adhesion, float destabTraction, int8 wheelId, float* wheelSpeed, tWheelState* wheelState, eBikeWheelSpecial special, uint16 wheelStatus) {
    plugin::CallMethod<0x6D73B0, CVehicle*, CVector&, CVector&, CVector&, CVector&, int32, float, float, float, float, char, float*, tWheelState*, eBikeWheelSpecial, uint16>(
        this, wheelFwd, wheelRight, wheelContactSpeed, wheelContactPoint, wheelsOnGround, thrust, brake, destabTraction, adhesion, wheelId, wheelSpeed, wheelState, special,
        wheelStatus);
}

// 0x6D7BC0
auto CVehicle::FindTyreNearestPoint(CVector2D point) -> eNearestCarWheel {
    // Como el original: cada producto escalar suma tambien z * 0.0f (un z NaN/inf lo vuelve NaN) y el orden es y primero, luego x.
    const auto& pos = GetPosition();
    const float relX = point.x - pos.x;
    const float relY = point.y - pos.y;
    const auto& fwd = GetForward();
    const auto& right = GetRight();
    const float fwdDot = (relY * fwd.y + relX * fwd.x) + fwd.z * 0.0f;
    const float rightDot = (relY * right.y + relX * right.x) + right.z * 0.0f;
    const bool isFront = fwdDot > 0.f;
    if (IsBike()) { // only distinguishes front vs rear
        return isFront ? eNearestCarWheel::FRONT_LEFT : eNearestCarWheel::REAR_LEFT;
    }
    const bool isRight = rightDot > 0.f;
    return isFront
        ? isRight ? eNearestCarWheel::FRONT_RIGHT : eNearestCarWheel::FRONT_LEFT
        : isRight ? eNearestCarWheel::REAR_RIGHT : eNearestCarWheel::REAR_LEFT;
}

// 0x6D7C90
void CVehicle::InflictDamage(CEntity* damager, eWeaponType weapon, float intensity, CVector coords) {
    ((void(__thiscall*)(CVehicle*, CEntity*, eWeaponType, float, CVector))0x6D7C90)(this, damager, weapon, intensity, coords);
}

// 0x6D82F0
void CVehicle::KillPedsGettingInVehicle() {
    // Como el original: se recorre el pool del ultimo hueco al primero y se omiten los peatones que van en un vehiculo
    // o que usan colision (bit 0 de m_nFlags, no bIsStanding).
    auto* const pedPool = GetPedPool();
    for (int32 slot = (int32)pedPool->GetSize(); slot-- > 0;) {
        CPed* const pedPtr = pedPool->GetAt(slot);
        if (!pedPtr) {
            continue;
        }
        auto& ped = *pedPtr;
        if (ped.bInVehicle || ped.m_bUsesCollision) {
            continue;
        }

        if (const auto task = static_cast<CTaskComplexEnterCar*>(ped.GetTaskManager().Find<CTaskComplexEnterCarAsDriver, CTaskComplexEnterCarAsPassenger>()); // Como el original: primero 701 (conductor) y luego 700
            !task || task->GetTargetCar() != this
        ) {
            continue;
        }

        CPedDamageResponseCalculator dmgRespCalc{ &ped, 1000.f, WEAPON_EXPLOSION, PED_PIECE_TORSO, false };
        CEventDamage dmgEvent{ &ped, CTimer::GetTimeInMS(), WEAPON_EXPLOSION, PED_PIECE_TORSO, 0, false, (bool)ped.bInVehicle };
        if (dmgEvent.AffectsPed(&ped)) {
            dmgRespCalc.ComputeDamageResponse(&ped, dmgEvent.m_damageResponse, true);
        } else {
            dmgEvent.m_damageResponse.m_bDamageCalculated = true;
        }
        ped.GetEventGroup().Add(&dmgEvent);
    }
}

// 0x6D8470
bool CVehicle::UsesSiren() {
    switch (m_nModelIndex) {
    case MODEL_FIRETRUK:
    case MODEL_AMBULAN:
    case MODEL_MRWHOOP:
        return true;
    case MODEL_RHINO:
        return false;
    default:
        return IsLawEnforcementVehicle() != false;
    }
}

// 0x6D84D0
bool CVehicle::IsSphereTouchingVehicle(CVector posn, float radius) {
    const auto dist = posn - GetPosition();
    const auto cm = GetColModel();

    // Como el original: cada producto escalar suma z, luego y y por ultimo x (DotProduct suma x, y, z)
    const auto DotZYX = [&](const CVector& v) { return (dist.z * v.z + dist.y * v.y) + dist.x * v.x; };

    const auto dotRight = DotZYX(GetRight());
    if (dotRight < cm->m_boundBox.m_vecMin.x - radius ||
        dotRight > cm->m_boundBox.m_vecMax.x + radius
    ) {
        return false;
    }

    const auto dotFwd = DotZYX(GetForward());
    if (dotFwd < cm->m_boundBox.m_vecMin.y - radius ||
        dotFwd > cm->m_boundBox.m_vecMax.y + radius
    ) {
        return false;
    }

    const auto dotUp = DotZYX(GetUp());
    if (dotUp < cm->m_boundBox.m_vecMin.z - radius ||
        dotUp > cm->m_boundBox.m_vecMax.z + radius
    ) {
        return false;
    }

    return true;
}

// ============================================================================================================================================================
// CVehicle::FlyingControl (0x6D85F0, 10484 bytes). Reconstruccion en C++ con la maquina x87 de software FlcX87 (X87Math.h/.cpp): el EXE ejecuta el calculo en el x87 a 24 bits de
// precision y deja valores en los registros muertos, banderas acumuladas y codigos de condicion; FlcX87 reproduce cada instruccion con aritmetica entera y FlcX87::Commit() deja ese
// estado final en el hardware. Las secuencias que el EXE toma de funciones pequenas (operadores de CVector y CMatrix, _CIpow, _CIasin, _CIacos, __ftol2) estan en FlcVec* / FlcMat* /
// FlcCrt* (X87Math.cpp). Los colaboradores (CPhysical::ApplyTurnForce / ApplyMoveForce / GetSpeed, CPad, HeightAboveCeiling, GetColModel) son llamadas ordinarias.
//
// Rutas (flightModel, argumento 1; el EXE las elige con la tabla de bytes 0x6DAEF4 y la tabla de saltos 0x6DAEE4):
//   0           -> FlcRouteAirship   (0x6D8715..0x6D8B18)  [el juego no la usa]
//   1, 3, 4, 5  -> FlcRouteWing      (0x6D8B1D..0x6D986E)  1 = Baron/RC, 3 = avion y coche, 4 = sin uso, 5 = bote
//   2, 6, 8     -> FlcRouteRotor     (0x6D986E..0x6DA900)  6 = helicoptero e Hydra, 2 y 8 sin uso
//   7, > 8      -> solo prologo y cola
//   (todas)     -> cola comun FlcTail (0x6DA900..0x6DAED7)
// ============================================================================================================================================================

namespace {
#define M32(a) (*(uint32*)(uintptr_t)(uint32)(a))   // memoria de 32 bits por direccion (rutas traducidas del EXE)
// ---- Constantes del EXE (.rdata). Los operandos de memoria del x87 son referencias a estos objetos.
constexpr float  kFlcZero         = 0.0f;                 // 0x858B50
constexpr float  kFlcOne          = 1.0f;                 // 0x858624
constexpr float  kFlcMinusOne     = -1.0f;                // 0x858C1C
constexpr float  kFlcHalf         = 0.5f;                 // 0x858B8C
constexpr float  kFlcSix          = 6.0f;                 // 0x858B44
constexpr float  kFlcSpeedClamp   = 1.5f;                 // 0x858CE8: velocidad maxima por paso
constexpr float  kFlcClampSpeedSq = 2.25f;                // 0x86E104
constexpr float  kFlcTurnClamp    = 0.2f;                 // 0x858CC4: giro maximo por paso
constexpr float  kFlcClampTurnSq  = 0.04f;                // 0x858CEC

// ---- Campo del vehiculo por desplazamiento (los que no tienen miembro con nombre en CVehicle)
template<class T> inline T& FlcAt(CVehicle& v, size_t offset) { return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(&v) + offset); }

// ---- Reproduce `test ah, mask` seguido de jp / jnp: el indicador de paridad es el de los 8 bits bajos del resultado.
inline bool FlcParityEven(unsigned int value) {
    value &= 0xFF;
    value ^= value >> 4;
    value ^= value >> 2;
    value ^= value >> 1;
    return (value & 1) == 0;
}

// ---- Variables locales del EXE que comparten el prologo, las rutas y la cola ([F+x] = desplazamiento desde la pila tras el prologo).
struct FlcFrame {
    CVehicle*            v;                // ESI
    FlcX87               fp;
    tFlyingHandlingData* fh;               // [ESI+0x388]
    eFlightModel         model;            // [F+0x264]
    float                skid;             // [F+0x268] leftRightSkid
    float                upDown;           // [F+0x26C] steeringUpDown
    float                sideways;         // [F+0x270] steeringLeftRight
    float                accel;            // [F+0x274] accelerationBreakStatus
    CPad*                pad;              // [F+0x14]: mando del conductor jugador
    float                vel[3];           // [F+0x40]: velocidad + deriva del viento
    float                com[3];           // [F+0x50]: centro de masas en el mundo (Multiply3x3 de la matriz por el centro de masas)
};

// =========================================================================================================================================
// Cola comun (0x6DA900..0x6DAED7): amortiguacion del giro y la velocidad, fuerzas de giro y recorte de velocidades.
// =========================================================================================================================================
// Una fuerza de giro por eje (0x6DAB58, 0x6DAC2B, 0x6DACFD): fuerza = (d * -eje) * masaDeGiro aplicada en el punto centroDeMasas + palanca.
void FlcApplyAxisTurn(FlcFrame& f, const float& d, const float* axis, const float* lever) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    float n2, n0d, force[3], point[3];
    fp.Fld(axis[0]); fp.Fmul(kFlcMinusOne);
    fp.Fld(axis[1]); fp.Fmul(kFlcMinusOne);
    fp.Fld(axis[2]); fp.Fmul(kFlcMinusOne);
    fp.Fstp(n2);
    fp.Fld(d); fp.Arith('*', 0, 0, 2, false); fp.Fstp(n0d);
    fp.Fld(d); fp.Arith('*', 0, 0, 1, false); fp.FstpSt(2); fp.FstpSt(0);
    fp.Fld(d); fp.Fmul(n2);
    fp.Fld(v.m_fTurnMass);
    fp.Fld(n0d); fp.Arith('*', 0, 0, 1, false); fp.Fstp(force[0]);
    fp.Fxch(2); fp.Arith('*', 0, 0, 2, false); fp.Fstp(force[1]);
    fp.Arith('*', 0, 0, 1, false); fp.Fstp(force[2]);
    fp.FstpSt(0);
    fp.Fld(f.com[0]); fp.Fadd(lever[0]); fp.Fstp(point[0]);
    fp.Fld(f.com[1]); fp.Fadd(lever[1]); fp.Fstp(point[1]);
    fp.Fld(f.com[2]); fp.Fadd(lever[2]); fp.Fstp(point[2]);
    v.ApplyTurnForce(CVector{ force[0], force[1], force[2] }, CVector{ point[0], point[1], point[2] });
}

// Recorta |vector| a `limit` cuando |vector|^2 > thresholdSq: vector = (vector * limit) * (1 / |vector|) (0x6DADBA..0x6DAE47 y 0x6DAE4A..0x6DAED4)
void FlcClampVector(FlcX87& fp, float* vec, const float& thresholdSq, const float& limit) {
    float magSq, cz, o0, o1, o2;
    fp.Fld(vec[2]); fp.Fld(vec[1]); fp.Fld(vec[0]);
    fp.FldSt(0); fp.Arith('*', 0, 0, 1, false);
    fp.FldSt(2); fp.Arith('*', 0, 0, 3, false);
    fp.Arith('+', 1, 1, 0, true);
    fp.FldSt(3); fp.Arith('*', 0, 0, 4, false);
    fp.Arith('+', 1, 1, 0, true);
    fp.Fst(magSq);
    fp.FstpSt(3); fp.FstpSt(0); fp.FstpSt(0);
    fp.FcomMem(thresholdSq, true);
    if (fp.Ah() & 0x41) return;                                    // jne: |v|^2 <= umbral o NaN
    fp.Fld(vec[0]); fp.Fmul(limit);
    fp.Fld(vec[1]); fp.Fmul(limit);
    fp.Fld(vec[2]); fp.Fmul(limit);
    fp.Fstp(cz);
    fp.Fld(magSq); fp.Fsqrt(); fp.Fdivr(kFlcOne); fp.Fst(magSq);
    fp.Arith('*', 0, 0, 2, false); fp.Fstp(o0);
    memcpy(&vec[0], &o0, 4);
    fp.Fmul(magSq); fp.Fstp(o1);
    fp.FstpSt(0);
    memcpy(&vec[1], &o1, 4);
    fp.Fld(cz); fp.Fmul(magSq); fp.Fstp(o2);
    memcpy(&vec[2], &o2, 4);
}

void FlcTail(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    const tFlyingHandlingData* fh = v.m_pFlyingHandlingData;
    const float* m = reinterpret_cast<const float*>(v.m_matrix);
    float t[3], l[3], w[3], d[3] = { 0.f, 0.f, 0.f }, scale;
    // amortiguacion de giro por eje: t = turnRes ^ timeStep
    fp.Fld(fh->m_vecTurnRes.x); fp.Fld(CTimer::ms_fTimeStep); FlcCrtPow(fp); fp.Fstp(t[0]);
    fp.Fld(fh->m_vecTurnRes.y); fp.Fld(CTimer::ms_fTimeStep); FlcCrtPow(fp); fp.Fstp(t[1]);
    fp.Fld(fh->m_vecTurnRes.z); fp.Fld(CTimer::ms_fTimeStep); FlcCrtPow(fp); fp.Fstp(t[2]);
    FlcMatInvMul3x3(fp, l, m, &v.m_vecTurnSpeed.x);                // giro en ejes locales (0x59C810)
    scale = kFlcOne;                                               // [F+0x10]
    switch (f.model) {
    case FLIGHT_MODEL_PLANE: case FLIGHT_MODEL_UNK2: case FLIGHT_MODEL_BOAT:     // 3, 4, 5: 2 * |velocidad|
        FlcVecMagnitude(fp, &v.m_vecMoveSpeed.x); fp.Arith('+', 0, 0, 0, false); fp.Fstp(scale); break;
    case FLIGHT_MODEL_BARON:                                                     // 1: |velocidad| * 6
        FlcVecMagnitude(fp, &v.m_vecMoveSpeed.x); fp.Fmul(kFlcSix); fp.Fstp(scale); break;
    case FLIGHT_MODEL_HELI: case eFlightModel(8):                                // 6 y 8: |velocidad| + 1
        FlcVecMagnitude(fp, &v.m_vecMoveSpeed.x); fp.Fadd(kFlcOne); fp.Fstp(scale); break;
    default: break;
    }
    // un eje cada vez: con resistencia de velocidad > 0 el incremento de giro es d = (t / (1 + l*l*s)) ^ timeStep * l - l; si no, l = t ^ escala * l
    const float speedRes[3] = { fh->m_vecSpeedRes.x, fh->m_vecSpeedRes.y, fh->m_vecSpeedRes.z };
    for (int i = 0; i < 3; ++i) {
        fp.Fld(speedRes[i]); fp.FcomMem(kFlcZero, true);
        if (fp.Ah() & 0x41) {                                      // speedRes <= 0 o NaN
            fp.Fld(t[i]); fp.Fld(scale); FlcCrtPow(fp); fp.Fmul(l[i]); fp.Fstp(l[i]);
        } else {
            fp.Fld(l[i]); fp.Fmul(l[i]); fp.Fmul(speedRes[i]); fp.Fadd(kFlcOne); fp.Fdivr(t[i]);
            fp.Fld(CTimer::ms_fTimeStep); FlcCrtPow(fp); fp.Fmul(l[i]); fp.Fsub(l[i]); fp.Fstp(d[i]);
        }
    }
    FlcMatMul3x3(fp, w, m, l);                                     // giro de vuelta a ejes del mundo (0x59C790)
    memcpy(&v.m_vecTurnSpeed.x, &w[0], 4); memcpy(&v.m_vecTurnSpeed.y, &w[1], 4); memcpy(&v.m_vecTurnSpeed.z, &w[2], 4);
    fp.Fld(d[0]); fp.Fmul(scale); fp.Fstp(d[0]);
    fp.Fld(d[1]); fp.Fmul(scale); fp.Fstp(d[1]);
    fp.Fld(d[2]); fp.Fmul(scale); fp.Fstp(d[2]);
    // fuerzas de giro: eje X con -adelante y palanca arriba; eje Y con -arriba y palanca derecha; eje Z con -derecha y palanca adelante
    fp.Fld(d[0]); fp.FcomMem(kFlcZero, true);
    if (FlcParityEven(fp.Ah() & 0x44)) FlcApplyAxisTurn(f, d[0], &m[4], &m[8]);        // jnp: sin fuerza si d == 0
    fp.Fld(d[1]); fp.FcomMem(kFlcZero, true);
    if (FlcParityEven(fp.Ah() & 0x44)) FlcApplyAxisTurn(f, d[1], &m[8], &m[0]);
    fp.Fld(d[2]); fp.FcomMem(kFlcZero, true);
    if (FlcParityEven(fp.Ah() & 0x44)) FlcApplyAxisTurn(f, d[2], &m[0], &m[4]);
    FlcClampVector(fp, &v.m_vecMoveSpeed.x, kFlcClampSpeedSq, kFlcSpeedClamp);
    FlcClampVector(fp, &v.m_vecTurnSpeed.x, kFlcClampTurnSq, kFlcTurnClamp);
}

// =========================================================================================================================================
// Ruta 0 (flightModel == 0, 0x6D8715..0x6D8B18). El juego nunca la usa. Tres fuerzas de giro proporcionales a la deriva lateral y a la velocidad al cuadrado,
// dos de empuje (ApplyMoveForce) y el amortiguamiento del giro sobre Y.
// =========================================================================================================================================
constexpr double kFlcD001      = 0.001;       // 0x871FB0
constexpr double kFlcD003      = 0.003;       // 0x871FA8
constexpr double kFlcD05       = 0.5;         // 0x859060
constexpr double kFlcD005      = 0.05;        // 0x859068
constexpr double kFlcD35       = 3.5;         // 0x871F98
constexpr double kFlcD0072     = 0.0072000003419816494;   // 0x871F90 (0x3F7D7DBF60000000)
constexpr double kFlcD09f      = 0.89999997615814209;     // 0x86C308 (0.9f como double)
constexpr float  kFlcMinus4    = -4.0f;       // 0x859C3C
constexpr float  kFlcMinus128th = -0.0078125f; // 0x871FA0
constexpr float  kFlcF001      = 0.001f;      // 0x858CDC
constexpr float  kFlcF002      = 0.002f;      // 0x858F44
constexpr float  kFlcF008      = 0.008f;      // 0x863984
constexpr float  kFlcTwo       = 2.0f;        // (inmediato 0x40000000)

void FlcRouteAirship(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    const float* m = reinterpret_cast<const float*>(v.m_matrix);      // derecha m[0..2], adelante m[4..6], arriba m[8..10]
    float lat, c, scaleArg, right[3], fwd[3], t1[3], t2[3], t3[3];
    // deriva lateral normalizada: lat = -(vel . derecha) / |vel|
    fp.Fld(f.vel[2]); fp.Fmul(m[2]); fp.Fld(f.vel[1]); fp.Fmul(m[1]); fp.Arith('+', 1, 1, 0, true); fp.Fld(f.vel[0]); fp.Fmul(m[0]); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(kFlcMinusOne); fp.Fstp(lat);
    FlcVecMagnitude(fp, f.vel);
    fp.Fdivr(lat);
    memcpy(fwd, &m[4], 12);
    fp.Fstp(lat);
    // c = (|vel|^2 * (adelante . vel)) * (adelante . vel)
    fp.Fld(fwd[1]); fp.Fmul(f.vel[1]); fp.Fld(fwd[0]); fp.Fmul(f.vel[0]); fp.Arith('+', 1, 1, 0, true); fp.Fld(fwd[2]); fp.Fmul(f.vel[2]); fp.Arith('+', 1, 1, 0, true);
    fp.Fstp(c);
    FlcVecMagnitudeSquared(fp, f.vel);
    fp.Fmul(c);
    memcpy(right, &m[0], 12);
    fp.Fmul(c);
    fp.Fstp(c);
    // giro: fuerza = derecha * (((v494 * 0.001 + lat * 0.003) * dt * c) * turnMass) en el punto adelante * -4
    fp.Fld(FlcAt<float>(v, 0x494)); fp.FmulD(kFlcD001); fp.Fld(lat); fp.FmulD(kFlcD003); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(CTimer::ms_fTimeStep); fp.Fmul(c); fp.Fmul(v.m_fTurnMass); fp.Fstp(scaleArg);
    FlcVecScale(fp, right, scaleArg);                                                 // 0x40FEF0
    fp.Fld(fwd[0]); fp.Fmul(kFlcMinus4); fp.Fstp(fwd[0]);
    fp.Fld(fwd[1]); fp.Fmul(kFlcMinus4); fp.Fstp(fwd[1]);
    fp.Fld(fwd[2]); fp.Fmul(kFlcMinus4); fp.Fstp(fwd[2]);
    v.ApplyTurnForce(CVector{ right[0], right[1], right[2] }, CVector{ fwd[0], fwd[1], fwd[2] });   // 0x6D883E
    // empuje lateral: derecha * ((dt * masa * c * lat) * 0.2)
    fp.Fld(CTimer::ms_fTimeStep); fp.Fmul(v.m_fMass);
    memcpy(right, &m[0], 12);
    fp.Fmul(c); fp.Fmul(lat); fp.Fmul(kFlcTurnClamp); fp.Fstp(scaleArg);
    FlcVecScale(fp, right, scaleArg);
    v.ApplyMoveForce(CVector{ right[0], right[1], right[2] });                        // 0x6D889C
    FlcVecMulLeft(fp, t1, kFlcTwo, &m[8]);                                            // 0x40FE90: 2 * arriba
    v.ApplyTurnForce(CVector{ right[0], right[1], right[2] }, CVector{ t1[0], t1[1], t1[2] });       // 0x6D88DB
    // deriva vertical: lat = -(vel . arriba) / |vel|
    fp.Fld(f.vel[2]); fp.Fmul(m[10]); fp.Fld(f.vel[1]); fp.Fmul(m[9]); fp.Arith('+', 1, 1, 0, true); fp.Fld(f.vel[0]); fp.Fmul(m[8]); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(kFlcMinusOne); fp.Fstp(lat);
    FlcVecMagnitude(fp, f.vel);
    fp.Fdivr(lat);
    fp.Fstp(lat);
    fp.Fld(kFlcZero);
    if (f.pad) {
        fp.Pop();
        const int steer = f.pad->GetSteeringUpDown();                                 // 0x53FBD0 (int16 con signo)
        fp.FldI(steer); fp.Fmul(kFlcMinus128th);
    }
    fp.Fmul(kFlcF001);
    memcpy(right, &m[8], 12);
    fp.Fld(lat); fp.Fmul(kFlcF002); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(CTimer::ms_fTimeStep); fp.Fmul(c); fp.Fmul(v.m_fTurnMass); fp.Fstp(scaleArg);
    FlcVecScale(fp, right, scaleArg);
    v.ApplyTurnForce(CVector{ right[0], right[1], right[2] }, CVector{ fwd[0], fwd[1], fwd[2] });   // 0x6D89BE
    // empuje vertical: c = ((lat * 3.5 + 0.5) * masa * dt * c) * 0.05, limitado a dt * masa * 0.0072 si el techo esta por debajo y la fuerza es pequena
    fp.Fld(lat); fp.FmulD(kFlcD35);
    const uint8 statusBits = uint8(v.GetStatus());
    fp.FaddD(kFlcD05); fp.Fmul(v.m_fMass); fp.Fmul(CTimer::ms_fTimeStep); fp.Fmul(c); fp.FmulD(kFlcD005); fp.Fstp(c);
    if (statusBits == 0 || statusBits == 8) {
        fp.Fld(CTimer::ms_fTimeStep); fp.Fmul(v.m_fMass); fp.Fmul(kFlcF008); fp.FcomMem(c, true);
        if (!FlcParityEven(fp.Ah() & 5)) {                                            // jp 0x6D8A57
            const float height = v.HeightAboveCeiling(v.GetPosition().z, eFlightModel(0));            // 0x6D8A2F
            fp.FldRet(height); fp.FcomMem(kFlcZero, true);
            if (!(fp.Ah() & 0x41)) { fp.Fld(v.m_fMass); fp.Fmul(CTimer::ms_fTimeStep); fp.FmulD(kFlcD0072); fp.Fstp(c); }
        }
    }
    FlcVecMulLeft(fp, t1, c, &m[8]);                                                  // 0x6D8A6B: c * arriba
    FlcVecMulLeft(fp, t2, kFlcTwo, &m[8]);                                            // 0x6D8A95: 2 * arriba
    FlcVecAdd(fp, t3, t2, f.com);                                                     // 0x6D8AA6: punto = 2 * arriba + centro de masas
    v.ApplyMoveForce(CVector{ t1[0], t1[1], t1[2] });                                 // 0x6D8ACF
    v.ApplyTurnForce(CVector{ t1[0], t1[1], t1[2] }, CVector{ t3[0], t3[1], t3[2] }); // 0x6D8AFC
    fp.FldD(kFlcD09f); fp.Fld(CTimer::ms_fTimeStep); FlcCrtPow(fp);                   // 0.9 ^ dt
    fp.Fmul(v.m_vecTurnSpeed.y); fp.Fstp(v.m_vecTurnSpeed.y);
}
void FlcRouteWing(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    alignas(16) uint8 stk[0xB00] = {};                      // pila x86 del EXE (el marco [F+x] empieza en stk + 0x800)
    uint32 eax = 0, ecx = 0, edx = 0, ebx = (uint32)(uintptr_t)f.pad, esi = (uint32)(uintptr_t)f.v, edi = 0, ebp = 0;
    uint32 esp = (uint32)(uintptr_t)stk + 0x800;            // F
    uint32 fA = 0, fB = 0, fR = 0;
    M32(esp + 0x264) = (uint32)f.model; memcpy(stk + 0x800 + 0x268, &f.skid, 4); memcpy(stk + 0x800 + 0x26C, &f.upDown, 4);
    memcpy(stk + 0x800 + 0x270, &f.sideways, 4); memcpy(stk + 0x800 + 0x274, &f.accel, 4);
    M32(esp + 0x14) = ebx;                                  // mando del conductor
    memcpy(stk + 0x800 + 0x40, f.vel, 12);                  // velocidad + deriva del viento
    memcpy(stk + 0x800 + 0x50, f.com, 12);                  // centro de masas en el mundo
    M32(esp + 0x4C) = (uint32)(uintptr_t)&v.m_vecMoveSpeed;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8b1d: fld dword ptr [esp + 0x268]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d8b24: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8b2a: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d8b2c: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d8b5f;   // 6d8b2f: jp 0x6d8b5f
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8b31: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0x0u);   // 6d8b33: mov dword ptr [esp + 0x268], 0
    if (fR == 0) goto L_6d8b5f;   // 6d8b3e: je 0x6d8b5f
    ecx = ebx;   // 6d8b40: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringLeftRight();   // 6d8b42: call 0x53fb80
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8b47: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6d8b4a: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8b4e: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d8b52: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8b58: fstp dword ptr [esp + 0x268]
L_6d8b5f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8b5f: fld dword ptr [esp + 0x26c]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d8b66: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8b6c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d8b6e: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d8be3;   // 6d8b71: jp 0x6d8be3
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8b73: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x26c) = (uint32)(0x0u);   // 6d8b75: mov dword ptr [esp + 0x26c], 0
    if (fR == 0) goto L_6d8be3;   // 6d8b80: je 0x6d8be3
    ecx = ebx;   // 6d8b82: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringUpDown();   // 6d8b84: call 0x53fbd0
    eax = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8b89: movsx eax, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6d8b8c: mov dword ptr [esp + 0x14], eax
    ecx = ebx;   // 6d8b90: mov ecx, ebx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8b92: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d8b96: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8b9c: fstp dword ptr [esp + 0x26c]
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d8ba3: call 0x53fc10
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8ba8: movsx ecx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6d8bab: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8baf: fild dword ptr [esp + 0x14]
    fp.Fabs();   // 6d8bb3: fabs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d8bb5: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8bbb: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8bbd: test ah, 0x41
    if (fR != 0) goto L_6d8be3;   // 6d8bc0: jne 0x6d8be3
    ecx = ebx;   // 6d8bc2: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d8bc4: call 0x53fc10
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8bc9: movsx edx, ax
    fR = (uint32)(0 - edx); edx = fR;   // 6d8bcc: neg edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6d8bce: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8bd2: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d8bd6: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8bdc: fstp dword ptr [esp + 0x26c]
L_6d8be3:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8be3: fld dword ptr [esp + 0x26c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8bea: fld dword ptr [esp + 0x268]
    fp.Fpatan();   // 6d8bf1: fpatan 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8bf3: fstp dword ptr [esp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858624)));   // 6d8bf7: fld dword ptr [0x858624]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8bfd: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf8)), true);   // 6d8c01: fcomp dword ptr [0x86daf8]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c07: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c09: test ah, 0x41
    if (fR != 0) goto L_6d8c2a;   // 6d8c0c: jne 0x6d8c2a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c0e: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x859ab0)), true);   // 6d8c12: fcomp dword ptr [0x859ab0]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c18: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c1a: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d8c2a;   // 6d8c1d: jp 0x6d8c2a
    fp.FstpSt(0);   // 6d8c1f: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c21: fld dword ptr [esp + 0x10]
    goto L_6d8cb5;   // 6d8c25: jmp 0x6d8cb5
L_6d8c2a:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c2a: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x859ab0)), true);   // 6d8c2e: fcomp dword ptr [0x859ab0]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c34: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c36: test ah, 0x41
    if (fR != 0) goto L_6d8c5a;   // 6d8c39: jne 0x6d8c5a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c3b: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf4)), true);   // 6d8c3f: fcomp dword ptr [0x86daf4]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c45: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c47: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d8c5a;   // 6d8c4a: jp 0x6d8c5a
    fp.FstpSt(0);   // 6d8c4c: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c4e: fld dword ptr [esp + 0x10]
    fp.Fsub((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 - m   // 6d8c52: fsub dword ptr [0x858fe4]
    goto L_6d8cb5;   // 6d8c58: jmp 0x6d8cb5
L_6d8c5a:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c5a: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf4)), true);   // 6d8c5e: fcomp dword ptr [0x86daf4]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c64: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c66: test ah, 0x41
    if (fR != 0) goto L_6d8c79;   // 6d8c69: jne 0x6d8c79
    fp.FstpSt(0);   // 6d8c6b: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c6d: fld dword ptr [esp + 0x10]
    fp.Fsub((*(float*)(uintptr_t)(uint32)(0x858cb8)));   // ST0 = ST0 - m   // 6d8c71: fsub dword ptr [0x858cb8]
    goto L_6d8cb5;   // 6d8c77: jmp 0x6d8cb5
L_6d8c79:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c79: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf0)), true);   // 6d8c7d: fcomp dword ptr [0x86daf0]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c83: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c85: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d8c98;   // 6d8c88: jp 0x6d8c98
    fp.FstpSt(0);   // 6d8c8a: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c8c: fld dword ptr [esp + 0x10]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x858cb8)));   // ST0 = ST0 + m   // 6d8c90: fadd dword ptr [0x858cb8]
    goto L_6d8cb5;   // 6d8c96: jmp 0x6d8cb5
L_6d8c98:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c98: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf8)), true);   // 6d8c9c: fcomp dword ptr [0x86daf8]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8ca2: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8ca4: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8cbd;   // 6d8ca7: jp 0x6d8cbd
    fp.FstpSt(0);   // 6d8ca9: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8cab: fld dword ptr [esp + 0x10]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 + m   // 6d8caf: fadd dword ptr [0x858fe4]
L_6d8cb5:;
    fp.Fcos();   // 6d8cb5: fcos 
    fp.Fdivr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m / ST0   // 6d8cb7: fdivr dword ptr [0x858624]
L_6d8cbd:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8cbd: fld dword ptr [esp + 0x268]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d8cc4: mov eax, dword ptr [esi + 0x14]
    fp.Arith('*', 0, 0, 1, false);   // 6d8cc7: fmul st(1)
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d8cc9: add eax, 0x10
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d8ccc: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d8cce: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d8cd1: mov eax, dword ptr [eax + 8]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8cd4: fstp dword ptr [esp + 0x268]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(ecx);   // 6d8cdb: mov dword ptr [esp + 0x28], ecx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // ST0 = ST0 * m   // 6d8cdf: fmul dword ptr [esp + 0x26c]
    ecx = esi;   // 6d8ce6: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d8ce8: mov dword ptr [esp + 0x2c], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(eax);   // 6d8cec: mov dword ptr [esp + 0x30], eax
    fp.Fchs();   // 6d8cf0: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8cf2: fstp dword ptr [esp + 0x26c]
    eax = (uint32)(uintptr_t)v.GetColModel();   // 6d8cf9: call 0x535300
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 4)));   // 6d8cfe: fld dword ptr [eax + 4]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x264));   // 6d8d01: mov edi, dword ptr [esp + 0x264]
    fA = edi; fB = 0x1u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8d08: cmp edi, 1
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d8d0b: fld dword ptr [esp + 0x28]
    fp.Arith('*', 0, 0, 1, false);   // 6d8d0f: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d8d11: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d8d15: fld dword ptr [esp + 0x2c]
    fp.Arith('*', 0, 0, 1, false);   // 6d8d19: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d8d1b: fstp dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d8d1f: fld dword ptr [esp + 0x30]
    fp.Arith('*', 0, 0, 1, false);   // 6d8d23: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d8d25: fstp dword ptr [esp + 0x24]
    fp.FstpSt(0);   // 6d8d29: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d8d2b: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // ST0 = ST0 * m   // 6d8d2f: fmul dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d8d33: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // ST0 = ST0 * m   // 6d8d37: fmul dword ptr [esp + 0x40]
    fp.Arith('+', 1, 1, 0, true);   // 6d8d3b: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d8d3d: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // ST0 = ST0 * m   // 6d8d41: fmul dword ptr [esp + 0x48]
    fp.Arith('+', 1, 1, 0, true);   // 6d8d45: faddp st(1)
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8d47: fst dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // ST0 = ST0 * m   // 6d8d4b: fmul dword ptr [esp + 0x10]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d8d4f: fstp dword ptr [esp + 0x18]
    if (fR != 0) goto L_6d8db7;   // 6d8d53: jne 0x6d8db7
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d8d55: mov ecx, dword ptr [0xb7cb5c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d8d5b: mov edx, dword ptr [esi + 0x8c]
    esp -= 4; M32(esp) = ecx;   // 6d8d61: push ecx
    esp -= 4; M32(esp) = edx;   // 6d8d62: push edx
    eax = (uint32)(esp + 0x3c);   // 6d8d63: lea eax, [esp + 0x3c]
    esp -= 4; M32(esp) = eax;   // 6d8d67: push eax
    ecx = (uint32)(esp + 0x194);   // 6d8d68: lea ecx, [esp + 0x194]
    esp -= 4; M32(esp) = ecx;   // 6d8d6f: push ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x44) = (uint32)(0x0u);   // 6d8d70: mov dword ptr [esp + 0x44], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x48) = (uint32)(0x0u);   // 6d8d78: mov dword ptr [esp + 0x48], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x4c) = (uint32)(0x3B83126Fu);   // 6d8d80: mov dword ptr [esp + 0x4c], 0x3b83126f
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d8d88: call 0x40fec0
    esp += 0xCu;   // 6d8d8d: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d8d90: push eax
    edx = (uint32)(esp + 0xd0);   // 6d8d91: lea edx, [esp + 0xd0]
    esp -= 4; M32(esp) = edx;   // 6d8d98: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d8d99: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d8d9e: mov edx, dword ptr [eax]
    ecx = esp;   // 6d8da0: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d8da2: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d8da4: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d8da7: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d8daa: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d8dad: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d8db0: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d8db2: call 0x5429f0
L_6d8db7:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8db7: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d8dbe: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8dc4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d8dc6: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d8e05;   // 6d8dc9: jp 0x6d8e05
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8dcb: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x274) = (uint32)(0x0u);   // 6d8dcd: mov dword ptr [esp + 0x274], 0
    if (fR == 0) goto L_6d8e05;   // 6d8dd8: je 0x6d8e05
    ecx = ebx;   // 6d8dda: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetAccelerate();   // 6d8ddc: call 0x5403f0
    ecx = ebx;   // 6d8de1: mov ecx, ebx
    ebp = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8de3: movsx ebp, ax
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetBrake();   // 6d8de6: call 0x540080
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8deb: movsx ecx, ax
    fA = ebp; fB = ecx; fR = (fA - fB) & 0xFFFFFFFFu; ebp = fR;   // 6d8dee: sub ebp, ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ebp);   // 6d8df0: mov dword ptr [esp + 0x14], ebp
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8df4: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859a3c)));   // ST0 = ST0 * m   // 6d8df8: fmul dword ptr [0x859a3c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8dfe: fstp dword ptr [esp + 0x274]
L_6d8e05:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x590));   // 6d8e05: mov ecx, dword ptr [esi + 0x590]
    fA = ecx; fB = ecx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8e0b: test ecx, ecx
    eax = 0x3F800000u;   // 6d8e0d: mov eax, 0x3f800000
    if (fR != 0) goto L_6d8efe;   // 6d8e12: jne 0x6d8efe
    ebx = (ebx & 0xFFFF0000u) | (((uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22))) & 0xFFFFu);   // 6d8e18: mov bx, word ptr [esi + 0x22]
    fA = (ebx & 0xFFFFu); fB = 0x21Bu; fR = (fA - fB) & 0xFFFFu;   // 6d8e1c: cmp bx, 0x21b
    if (fR == 0) goto L_6d8e6f;   // 6d8e21: je 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7d4)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e23: cmp dword ptr [esi + 0x7d4], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e29: jne 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7d8)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e2b: cmp dword ptr [esi + 0x7d8], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e31: jne 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7dc)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e33: cmp dword ptr [esi + 0x7dc], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e39: jne 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7e0)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e3b: cmp dword ptr [esi + 0x7e0], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e41: jne 0x6d8e6f
    fA = (ebx & 0xFFFFu); fB = 0x1CCu; fR = (fA - fB) & 0xFFFFu;   // 6d8e43: cmp bx, 0x1cc
    if (fR != 0) goto L_6d8efe;   // 6d8e48: jne 0x6d8efe
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x40));   // 6d8e4e: mov eax, dword ptr [esi + 0x40]
    fA = ((eax >> 8) & 0xFFu); fB = 0x1u; fR = (fA & fB) & 0xFFu;   // 6d8e51: test ah, 1
    if (fR == 0) goto L_6d8efe;   // 6d8e54: je 0x6d8efe
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8e5a: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858cc4)), true);   // 6d8e5e: fcomp dword ptr [0x858cc4]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8e64: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8e66: test ah, 0x41
    if (fR == 0) goto L_6d8efe;   // 6d8e69: je 0x6d8efe
L_6d8e6f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8e6f: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8e76: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8e7c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8e7e: test ah, 0x41
    if (fR == 0) goto L_6d8efe;   // 6d8e81: je 0x6d8efe
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8e83: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8e8a: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8e90: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8e92: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8eee;   // 6d8e95: jp 0x6d8eee
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d8e97: mov edx, dword ptr [esp + 0x4c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d8e9b: mov eax, dword ptr [esi + 0x14]
    esp -= 4; M32(esp) = edx;   // 6d8e9e: push edx
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d8e9f: add eax, 0x10
    esp -= 4; M32(esp) = eax;   // 6d8ea2: push eax
    FlcVecDot(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4));   // 6d8ea3: call 0x40fdb0
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b38)), true);   // 6d8ea8: fcomp dword ptr [0x858b38]
    esp += 0x8u;   // 6d8eae: add esp, 8
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8eb1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8eb3: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8eee;   // 6d8eb6: jp 0x6d8eee
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8eb8: fld dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x871f88)));   // ST0 = ST0 * m   // 6d8ebc: fmul dword ptr [0x871f88]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // ST0 = m - ST0   // 6d8ec2: fsubr dword ptr [esp + 0x274]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8ec9: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d8ecf: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8ed1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8ed3: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8ee0;   // 6d8ed6: jp 0x6d8ee0
    fp.FstpSt(0);   // 6d8ed8: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8eda: fld dword ptr [0x858b50]
L_6d8ee0:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d8ee0: mov ecx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d8ee6: fmul dword ptr [ecx + 4]
    goto L_6d8fa6;   // 6d8ee9: jmp 0x6d8fa6
L_6d8eee:;
    *(uint32*)(uintptr_t)(uint32)(esp + 0x274) = (uint32)(0x0u);   // 6d8eee: mov dword ptr [esp + 0x274], 0
    goto L_6d8fad;   // 6d8ef9: jmp 0x6d8fad
L_6d8efe:;
    ebx = (ebx & 0xFFFF0000u) | (((uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22))) & 0xFFFFu);   // 6d8efe: mov bx, word ptr [esi + 0x22]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8f02: fld dword ptr [esp + 0x274]
    fA = (ebx & 0xFFFFu); fB = 0x21Bu; fR = (fA - fB) & 0xFFFFu;   // 6d8f09: cmp bx, 0x21b
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d8f0e: mov ecx, dword ptr [esi + 0x388]
    if (fR != 0) goto L_6d8f1b;   // 6d8f14: jne 0x6d8f1b
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d8f16: fmul dword ptr [ecx + 4]
    goto L_6d8f2a;   // 6d8f19: jmp 0x6d8f2a
L_6d8f1b:;
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = ST0 + m   // 6d8f1b: fadd dword ptr [0x858624]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d8f21: fmul dword ptr [ecx + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b8c)));   // ST0 = ST0 * m   // 6d8f24: fmul dword ptr [0x858b8c]
L_6d8f2a:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8f2a: fstp dword ptr [esp + 0x274]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8f31: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8f35: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f3b: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8f3d: test ah, 0x41
    if (fR != 0) goto L_6d8fad;   // 6d8f40: jne 0x6d8fad
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // 6d8f42: fld dword ptr [ecx + 8]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d8f45: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f4b: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8f4d: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8fad;   // 6d8f50: jp 0x6d8fad
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // 6d8f52: fld dword ptr [ecx + 8]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8f55: fcomp dword ptr [0x858b50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8f5b: fld dword ptr [esp + 0x10]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f5f: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x1u; fR = (fA & fB) & 0xFFu;   // 6d8f61: test ah, 1
    if (fR != 0) goto L_6d8f75;   // 6d8f64: jne 0x6d8f75
    fp.Fsub((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 - m   // 6d8f66: fsub dword ptr [ecx + 8]
    fp.FldSt(0);   // 6d8f69: fld st(0)
    fp.Arith('*', 1, 1, 0, true);   // 6d8f6b: fmulp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858f50)));   // ST0 = ST0 * m   // 6d8f6d: fmul dword ptr [0x858f50]
    goto L_6d8f82;   // 6d8f73: jmp 0x6d8f82
L_6d8f75:;
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 + m   // 6d8f75: fadd dword ptr [ecx + 8]
    fp.FldSt(0);   // 6d8f78: fld st(0)
    fp.Arith('*', 1, 1, 0, true);   // 6d8f7a: fmulp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b3c)));   // ST0 = ST0 * m   // 6d8f7c: fmul dword ptr [0x858b3c]
L_6d8f82:;
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d8f82: fsubr dword ptr [0x858624]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8f88: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d8f8e: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f90: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8f92: test ah, 0x41
    if (fR != 0) goto L_6d8f9f;   // 6d8f95: jne 0x6d8f9f
    fp.FstpSt(0);   // 6d8f97: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8f99: fld dword ptr [0x858b50]
L_6d8f9f:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // ST0 = ST0 * m   // 6d8f9f: fmul dword ptr [esp + 0x274]
L_6d8fa6:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fa6: fstp dword ptr [esp + 0x274]
L_6d8fad:;
    fA = edi; fB = 0x4u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8fad: cmp edi, 4
    if (fR != 0) goto L_6d8fc1;   // 6d8fb0: jne 0x6d8fc1
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fb2: fld dword ptr [esp + 0x274]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c24)));   // ST0 = ST0 * m   // 6d8fb9: fmul dword ptr [0x858c24]
    goto L_6d8fd3;   // 6d8fbf: jmp 0x6d8fd3
L_6d8fc1:;
    fA = edi; fB = 0x5u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8fc1: cmp edi, 5
    if (fR != 0) goto L_6d8fda;   // 6d8fc4: jne 0x6d8fda
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fc6: fld dword ptr [esp + 0x274]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b1c)));   // ST0 = ST0 * m   // 6d8fcd: fmul dword ptr [0x858b1c]
L_6d8fd3:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fd3: fstp dword ptr [esp + 0x274]
L_6d8fda:;
    fA = (ebx & 0xFFFFu); fB = 0x208u; fR = (fA - fB) & 0xFFFFu;   // 6d8fda: cmp bx, 0x208
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d8fdf: mov edi, dword ptr [esi + 0x14]
    edx = (uint32)(edi + 0x10);   // 6d8fe2: lea edx, [edi + 0x10]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx));   // 6d8fe5: mov eax, dword ptr [edx]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 4));   // 6d8fe7: mov ecx, dword ptr [edx + 4]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 8));   // 6d8fea: mov edx, dword ptr [edx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(eax);   // 6d8fed: mov dword ptr [esp + 0x28], eax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(ecx);   // 6d8ff1: mov dword ptr [esp + 0x2c], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(edx);   // 6d8ff5: mov dword ptr [esp + 0x30], edx
    if (fR != 0) goto L_6d9084;   // 6d8ff9: jne 0x6d9084
    eax = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x86c));   // 6d8fff: movzx eax, word ptr [esi + 0x86c]
    ecx = (uint32)(int32_t)(int16_t)((uint32)(*(uint16*)(uintptr_t)(uint32)(0x8d33c4)));   // 6d9006: movsx ecx, word ptr [0x8d33c4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6d900d: mov dword ptr [esp + 0x14], eax
    edx = (uint32)(edi + 0x20);   // 6d9011: lea edx, [edi + 0x20]
    esp -= 4; M32(esp) = edx;   // 6d9014: push edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9015: fild dword ptr [esp + 0x18]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6d9019: mov dword ptr [esp + 0x18], ecx
    esp -= 4; M32(esp) = ecx;   // 6d901d: push ecx
    eax = (uint32)(esp + 0x250);   // 6d901e: lea eax, [esp + 0x250]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 * m   // 6d9025: fmul dword ptr [0x858fe4]
    fp.FidivI((*(int*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d902b: fidiv dword ptr [esp + 0x1c]
    fp.FldSt(0);   // 6d902f: fld st(0)
    fp.Fcos();   // 6d9031: fcos 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9033: fstp dword ptr [esp + 0x1c]
    fp.Fsin();   // 6d9037: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9039: fstp dword ptr [esp]
    esp -= 4; M32(esp) = eax;   // 6d903c: push eax
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d903d: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x20));   // 6d9042: mov ecx, dword ptr [esp + 0x20]
    esp += 0xCu;   // 6d9046: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9049: push eax
    fA = edi; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d904a: add edi, 0x10
    esp -= 4; M32(esp) = edi;   // 6d904d: push edi
    esp -= 4; M32(esp) = ecx;   // 6d904e: push ecx
    edx = (uint32)(esp + 0xec);   // 6d904f: lea edx, [esp + 0xec]
    esp -= 4; M32(esp) = edx;   // 6d9056: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9057: call 0x40fe90
    esp += 0xCu;   // 6d905c: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d905f: push eax
    eax = (uint32)(esp + 0x1a8);   // 6d9060: lea eax, [esp + 0x1a8]
    esp -= 4; M32(esp) = eax;   // 6d9067: push eax
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9068: call 0x40fe30
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d906d: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d906f: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9072: mov eax, dword ptr [eax + 8]
    esp += 0xCu;   // 6d9075: add esp, 0xc
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(ecx);   // 6d9078: mov dword ptr [esp + 0x28], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d907c: mov dword ptr [esp + 0x2c], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(eax);   // 6d9080: mov dword ptr [esp + 0x30], eax
L_6d9084:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9084: fld dword ptr [esp + 0x28]
    esp -= 0xCu;   // 6d9088: sub esp, 0xc
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d908b: fmul dword ptr [0x863984]
    ecx = esp;   // 6d9091: mov ecx, esp
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9093: fld dword ptr [esp + 0x38]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9097: fmul dword ptr [0x863984]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d909d: fld dword ptr [esp + 0x3c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d90a1: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d90a7: fstp dword ptr [esp + 0x48]
    fp.Fxch(1);   // 6d90ab: fxch st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x280)));   // ST0 = ST0 * m   // 6d90ad: fmul dword ptr [esp + 0x280]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d90b4: fstp dword ptr [esp + 0x34]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x280)));   // ST0 = ST0 * m   // 6d90b8: fmul dword ptr [esp + 0x280]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d90bf: fld dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x280)));   // ST0 = ST0 * m   // 6d90c3: fmul dword ptr [esp + 0x280]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // 6d90ca: fld dword ptr [esi + 0x8c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d90d0: fld dword ptr [esp + 0x34]
    fp.Arith('*', 0, 0, 1, false);   // 6d90d4: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d90d6: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6d90da: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6d90dc: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d90de: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6d90e2: fmul st(1)
    fp.Fxch(1);   // 6d90e4: fxch st(1)
    fp.FstpSt(0);   // 6d90e6: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d90e8: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d90ec: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d90f2: fstp dword ptr [esp + 0x34]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6d90f6: mov edx, dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d90fa: fld dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d90fe: mov dword ptr [ecx], edx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9100: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9106: fstp dword ptr [esp + 0x38]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6d910a: mov eax, dword ptr [esp + 0x38]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d910e: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6d9114: mov dword ptr [ecx + 4], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d9117: fstp dword ptr [esp + 0x3c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6d911b: mov edx, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d911f: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6d9122: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9124: call 0x5429f0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9129: fld dword ptr [esp + 0x48]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d912d: mov eax, dword ptr [esi + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 * m   // 6d9130: fmul dword ptr [eax + 8]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9133: mov ecx, dword ptr [eax]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9135: fld dword ptr [esp + 0x44]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9139: mov edx, dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6d913c: fmul dword ptr [eax + 4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(ecx);   // 6d913f: mov dword ptr [esp + 0x28], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9143: mov ecx, dword ptr [esi + 0x388]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d9149: mov dword ptr [esp + 0x2c], edx
    fp.Arith('+', 1, 1, 0, true);   // 6d914d: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d914f: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6d9153: fmul dword ptr [eax]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9155: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(eax);   // 6d9158: mov dword ptr [esp + 0x30], eax
    fp.Arith('+', 1, 1, 0, true);   // 6d915c: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d915e: fmul dword ptr [0x858c1c]
    fp.FldSt(0);   // 6d9164: fld st(0)
    fp.Fabs();   // 6d9166: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x14)));   // ST0 = ST0 * m   // 6d9168: fmul dword ptr [ecx + 0x14]
    fp.Arith('*', 1, 1, 0, true);   // 6d916b: fmulp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d916d: fld dword ptr [esp + 0x28]
    fp.Arith('*', 0, 0, 1, false);   // 6d9171: fmul st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9173: fld dword ptr [esp + 0x2c]
    fp.Arith('*', 0, 0, 2, false);   // 6d9177: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9179: fstp dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d917d: fld dword ptr [esp + 0x30]
    fp.Arith('*', 0, 0, 2, false);   // 6d9181: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9183: fstp dword ptr [esp + 0x30]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9187: mov edx, dword ptr [esi + 0x8c]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6d918d: mov dword ptr [esp + 0x14], edx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6d9191: fmul dword ptr [esp + 0x14]
    esp -= 0xCu;   // 6d9195: sub esp, 0xc
    eax = esp;   // 6d9198: mov eax, esp
    fp.FstpSt(1);   // 6d919a: fstp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d919c: fld dword ptr [esp + 0x38]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6d91a0: fmul dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d91a4: fld dword ptr [esp + 0x3c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6d91a8: fmul dword ptr [esp + 0x20]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d91ac: fstp dword ptr [esp + 0x70]
    fp.Fxch(1);   // 6d91b0: fxch st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d91b2: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d91b8: fstp dword ptr [esp + 0x40]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d91bc: mov ecx, dword ptr [esp + 0x40]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6d91c0: mov dword ptr [eax], ecx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d91c2: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d91c8: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d91cc: fld dword ptr [esp + 0x70]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d91d0: mov edx, dword ptr [esp + 0x44]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d91d4: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6d91da: mov dword ptr [eax + 4], edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d91dd: fstp dword ptr [esp + 0x48]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d91e1: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d91e5: mov dword ptr [eax + 8], ecx
    ecx = esi;   // 6d91e8: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d91ea: call 0x5429f0
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d91ef: mov edi, dword ptr [esi + 0x14]
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x1c));   // 6d91f2: mov ebx, dword ptr [esp + 0x1c]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x20));   // 6d91f6: mov ebp, dword ptr [esp + 0x20]
    edx = edi;   // 6d91fa: mov edx, edi
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx));   // 6d91fc: mov eax, dword ptr [edx]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 4));   // 6d91fe: mov ecx, dword ptr [edx + 4]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 8));   // 6d9201: mov edx, dword ptr [edx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(eax);   // 6d9204: mov dword ptr [esp + 0x28], eax
    esp -= 0xCu;   // 6d9208: sub esp, 0xc
    eax = esp;   // 6d920b: mov eax, esp
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ebx);   // 6d920d: mov dword ptr [eax], ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x38) = (uint32)(ecx);   // 6d920f: mov dword ptr [esp + 0x38], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x30));   // 6d9213: mov ecx, dword ptr [esp + 0x30]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x3c) = (uint32)(edx);   // 6d9217: mov dword ptr [esp + 0x3c], edx
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(ebp);   // 6d921b: mov dword ptr [eax + 4], ebp
    edx = (uint32)(esp + 0x104);   // 6d921e: lea edx, [esp + 0x104]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d9225: mov dword ptr [eax + 8], ecx
    esp -= 4; M32(esp) = edx;   // 6d9228: push edx
    ecx = esi;   // 6d9229: mov ecx, esi
    { CVector* out_ = (CVector*)(uintptr_t)M32(esp + 0); *out_ = v.GetSpeed(CVector{ (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)), (*(float*)(uintptr_t)(esp + 12)) }); eax = (uint32)(uintptr_t)out_; esp += 0x10; }   // 6d922b: call 0x542ce0
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 8)));   // 6d9230: fld dword ptr [eax + 8]
    fA = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22)); fB = 0x21Bu; fR = (fA - fB) & 0xFFFFu;   // 6d9233: cmp word ptr [esi + 0x22], 0x21b
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9239: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 4)));   // 6d923c: fld dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d923f: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9242: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax)));   // 6d9244: fld dword ptr [eax]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9246: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9248: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d924a: fmul dword ptr [0x858c1c]
    if (fR != 0) goto L_6d92b6;   // 6d9250: jne 0x6d92b6
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9252: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x274)), true);   // 6d9256: fcomp dword ptr [esp + 0x274]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d925d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d925f: test ah, 0x41
    if (fR != 0) goto L_6d926a;   // 6d9262: jne 0x6d926a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9264: fld dword ptr [esp + 0x10]
    goto L_6d9271;   // 6d9268: jmp 0x6d9271
L_6d926a:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d926a: fld dword ptr [esp + 0x274]
L_6d9271:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9271: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9275: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d927b: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d927d: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d92a3;   // 6d9280: jp 0x6d92a3
    fp.FstpSt(0);   // 6d9282: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9284: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x274)), true);   // 6d9288: fcomp dword ptr [esp + 0x274]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d928f: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9291: test ah, 5
    if (FlcParityEven(fR)) goto L_6d929c;   // 6d9294: jp 0x6d929c
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9296: fld dword ptr [esp + 0x10]
    goto L_6d92a3;   // 6d929a: jmp 0x6d92a3
L_6d929c:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d929c: fld dword ptr [esp + 0x274]
L_6d92a3:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d92a3: mov eax, dword ptr [esi + 0x388]
    fp.FldSt(1);   // 6d92a9: fld st(1)
    fp.Fabs();   // 6d92ab: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 * m   // 6d92ad: fmul dword ptr [eax + 0x10]
    fp.Arith('*', 0, 0, 2, false);   // 6d92b0: fmul st(2)
    fp.Fxch(1);   // 6d92b2: fxch st(1)
    goto L_6d92c9;   // 6d92b4: jmp 0x6d92c9
L_6d92b6:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d92b6: mov eax, dword ptr [esi + 0x388]
    fp.FldSt(0);   // 6d92bc: fld st(0)
    fp.Fabs();   // 6d92be: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 * m   // 6d92c0: fmul dword ptr [eax + 0x10]
    fp.Arith('*', 0, 0, 1, false);   // 6d92c3: fmul st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d92c5: fld dword ptr [esp + 0x10]
L_6d92c9:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0xc)));   // ST0 = ST0 * m   // 6d92c9: fmul dword ptr [eax + 0xc]
    esp -= 4; M32(esp) = ecx;   // 6d92cc: push ecx
    ecx = (uint32)(esp + 0x2c);   // 6d92cd: lea ecx, [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // ST0 = ST0 * m   // 6d92d1: fmul dword ptr [esp + 0x26c]
    fp.Arith('+', 1, 1, 0, true);   // 6d92d8: faddp st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d92da: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d92e0: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d92e6: fstp dword ptr [esp]
    fp.FstpSt(0);   // 6d92e9: fstp st(0)
    FlcVecScale(fp, (float*)(uintptr_t)ecx, (*(float*)(uintptr_t)(esp + 0))); eax = ecx; esp += 4;   // 6d92eb: call 0x40fef0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d92f0: fld dword ptr [esp + 0x50]
    esp -= 0xCu;   // 6d92f4: sub esp, 0xc
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // ST0 = ST0 + m   // 6d92f7: fadd dword ptr [esp + 0x28]
    eax = esp;   // 6d92fb: mov eax, esp
    esp -= 0xCu;   // 6d92fd: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6d9300: fstp dword ptr [esp + 0x4c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d9304: mov ecx, dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6d9308: fld dword ptr [esp + 0x6c]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6d930c: mov dword ptr [eax], ecx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // ST0 = ST0 + m   // 6d930e: fadd dword ptr [esp + 0x38]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d9312: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d9316: fld dword ptr [esp + 0x70]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6d931a: mov edx, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // ST0 = ST0 + m   // 6d931e: fadd dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6d9322: mov dword ptr [eax + 4], edx
    edx = esp;   // 6d9325: mov edx, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d9327: fstp dword ptr [esp + 0x54]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6d932b: mov ecx, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d932f: mov dword ptr [eax + 8], ecx
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d9332: mov eax, dword ptr [esp + 0x40]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9336: mov ecx, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6d933a: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d933c: mov eax, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d9340: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6d9343: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9345: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d9348: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6d934d: fld dword ptr [esp + 0x270]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9354: mov edi, dword ptr [esi + 0x14]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d9357: fcomp dword ptr [0x871f8c]
    fp.Fld(CTimer::ms_fTimeStep);   // 6d935d: fld dword ptr [0xb7cb5c]
    ecx = edi;   // 6d9363: mov ecx, edi
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx));   // 6d9365: mov edx, dword ptr [ecx]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 4));   // 6d9367: mov eax, dword ptr [ecx + 4]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 8));   // 6d936a: mov ecx, dword ptr [ecx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(eax);   // 6d936d: mov dword ptr [esp + 0x2c], eax
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9371: fnstsw ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(ecx);   // 6d9373: mov dword ptr [esp + 0x30], ecx
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d9377: test ah, 0x44
    esp -= 4; M32(esp) = ecx;   // 6d937a: push ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d937b: mov dword ptr [esp + 0x2c], edx
    ecx = (uint32)(esp + 0x2c);   // 6d937f: lea ecx, [esp + 0x2c]
    if (FlcParityEven(fR)) goto L_6d939b;   // 6d9383: jp 0x6d939b
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9385: mov edx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edx + 0x18)));   // ST0 = ST0 * m   // 6d938b: fmul dword ptr [edx + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6d938e: fmul dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // ST0 = ST0 * m   // 6d9392: fmul dword ptr [esp + 0x26c]
    goto L_6d93af;   // 6d9399: jmp 0x6d93af
L_6d939b:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d939b: mov eax, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 * m   // 6d93a1: fmul dword ptr [eax + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6d93a4: fmul dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // ST0 = ST0 * m   // 6d93a8: fmul dword ptr [esp + 0x274]
L_6d93af:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d93af: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d93b5: fstp dword ptr [esp]
    FlcVecScale(fp, (float*)(uintptr_t)ecx, (*(float*)(uintptr_t)(esp + 0))); eax = ecx; esp += 4;   // 6d93b8: call 0x40fef0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d93bd: fld dword ptr [esp + 0x50]
    esp -= 0xCu;   // 6d93c1: sub esp, 0xc
    fp.Fadd((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 + m   // 6d93c4: fadd dword ptr [edi + 0x20]
    ecx = esp;   // 6d93c7: mov ecx, esp
    esp -= 0xCu;   // 6d93c9: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6d93cc: fstp dword ptr [esp + 0x4c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d93d0: mov edx, dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6d93d4: fld dword ptr [esp + 0x6c]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d93d8: mov dword ptr [ecx], edx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(edi + 0x24)));   // ST0 = ST0 + m   // 6d93da: fadd dword ptr [edi + 0x24]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d93dd: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d93e1: fld dword ptr [esp + 0x70]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6d93e5: mov eax, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(edi + 0x28)));   // ST0 = ST0 + m   // 6d93e9: fadd dword ptr [edi + 0x28]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6d93ec: mov dword ptr [ecx + 4], eax
    eax = esp;   // 6d93ef: mov eax, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d93f1: fstp dword ptr [esp + 0x54]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6d93f5: mov edx, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d93f9: mov dword ptr [ecx + 8], edx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d93fc: mov ecx, dword ptr [esp + 0x40]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9400: mov edx, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6d9404: mov dword ptr [eax], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d9406: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6d940a: mov dword ptr [eax + 4], edx
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d940d: mov dword ptr [eax + 8], ecx
    ecx = esi;   // 6d9410: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d9412: call 0x542a50
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9417: mov eax, dword ptr [esi + 0x14]
    edx = (uint32)(esp + 0x74);   // 6d941a: lea edx, [esp + 0x74]
    esp -= 4; M32(esp) = edx;   // 6d941e: push edx
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d941f: add eax, 0x10
    esp -= 4; M32(esp) = eax;   // 6d9422: push eax
    eax = (uint32)(esp + 0x208);   // 6d9423: lea eax, [esp + 0x208]
    esp -= 4; M32(esp) = eax;   // 6d942a: push eax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x80) = (uint32)(0x0u);   // 6d942b: mov dword ptr [esp + 0x80], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x84) = (uint32)(0x0u);   // 6d9436: mov dword ptr [esp + 0x84], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x88) = (uint32)(0x3F800000u);   // 6d9441: mov dword ptr [esp + 0x88], 0x3f800000
    FlcVecCross(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d944c: call 0x59c730
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858624)));   // 6d9451: fld dword ptr [0x858624]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9457: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9459: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d945c: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x34) = (uint32)(ecx);   // 6d945f: mov dword ptr [esp + 0x34], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9463: mov ecx, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x28)));   // 6d9466: fld dword ptr [ecx + 0x28]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x3c) = (uint32)(eax);   // 6d9469: mov dword ptr [esp + 0x3c], eax
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d946d: fcomp dword ptr [0x858b50]
    esp += 0xCu;   // 6d9473: add esp, 0xc
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d9476: mov dword ptr [esp + 0x2c], edx
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d947a: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d947c: test ah, 0x41
    if (fR == 0) goto L_6d94ab;   // 6d947f: je 0x6d94ab
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9481: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d9485: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d948b: fstp dword ptr [esp + 0x28]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d948f: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d9493: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9499: fstp dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d949d: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d94a1: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d94a7: fstp dword ptr [esp + 0x30]
L_6d94ab:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // 6d94ab: fld dword ptr [ecx + 8]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d94ae: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d94b4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d94b6: test ah, 0x41
    if (fR != 0) goto L_6d94c3;   // 6d94b9: jne 0x6d94c3
    fp.FstpSt(0);   // 6d94bb: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // 6d94bd: fld dword ptr [0x858c1c]
L_6d94c3:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x18)));   // 6d94c3: fld dword ptr [ecx + 0x18]
    esp -= 4; M32(esp) = ecx;   // 6d94c6: push ecx
    fp.Fabs();   // 6d94c7: fabs 
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d94c9: fsubr dword ptr [0x858624]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d94cf: fld dword ptr [esp + 0x34]
    edx = (uint32)(esp + 0x114);   // 6d94d3: lea edx, [esp + 0x114]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6d94da: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d94dd: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d94e1: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d94e4: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d94e6: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6d94ea: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6d94ec: faddp st(1)
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d94ee: fsubr dword ptr [0x858624]
    fp.Arith('*', 0, 0, 2, false);   // 6d94f4: fmul st(2)
    fp.Arith('*', 1, 1, 0, true);   // 6d94f6: fmulp st(1)
    fp.FstpSt(1);   // 6d94f8: fstp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d94fa: fld dword ptr [esp + 0x54]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 0x20)));   // ST0 = ST0 + m   // 6d94fe: fadd dword ptr [ecx + 0x20]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9501: fstp dword ptr [esp + 0x38]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x58)));   // 6d9505: fld dword ptr [esp + 0x58]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 0x24)));   // ST0 = ST0 + m   // 6d9509: fadd dword ptr [ecx + 0x24]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d950c: fstp dword ptr [esp + 0x3c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x5c)));   // 6d9510: fld dword ptr [esp + 0x5c]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 0x28)));   // ST0 = ST0 + m   // 6d9514: fadd dword ptr [ecx + 0x28]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9517: mov ecx, dword ptr [esi + 0x388]
    esp -= 4; M32(esp) = ecx;   // 6d951d: push ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d951e: fstp dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x1c)));   // ST0 = ST0 * m   // 6d9522: fmul dword ptr [ecx + 0x1c]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9525: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d952b: fmul dword ptr [esi + 0x90]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b8c)));   // ST0 = ST0 * m   // 6d9531: fmul dword ptr [0x858b8c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9537: fstp dword ptr [esp]
    esp -= 4; M32(esp) = edx;   // 6d953a: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d953b: call 0x40fe90
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d9540: mov edx, dword ptr [esp + 0x40]
    ecx = esp;   // 6d9544: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d9546: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9548: mov edx, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d954c: mov dword ptr [ecx + 4], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d954f: mov edx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d9553: mov dword ptr [ecx + 8], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9556: mov edx, dword ptr [eax]
    esp -= 0xCu;   // 6d9558: sub esp, 0xc
    ecx = esp;   // 6d955b: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d955d: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d955f: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9562: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d9565: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d9568: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d956b: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d956d: call 0x542a50
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9572: mov edi, dword ptr [esi + 0x14]
    ecx = (uint32)(edi + 0x20);   // 6d9575: lea ecx, [edi + 0x20]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx));   // 6d9578: mov edx, dword ptr [ecx]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 4));   // 6d957a: mov eax, dword ptr [ecx + 4]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 8));   // 6d957d: mov ecx, dword ptr [ecx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(edx);   // 6d9580: mov dword ptr [esp + 0x28], edx
    esp -= 0xCu;   // 6d9584: sub esp, 0xc
    edx = esp;   // 6d9587: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(esp + 0x3c) = (uint32)(ecx);   // 6d9589: mov dword ptr [esp + 0x3c], ecx
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ebx);   // 6d958d: mov dword ptr [edx], ebx
    ecx = (uint32)(esp + 0x1c4);   // 6d958f: lea ecx, [esp + 0x1c4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x38) = (uint32)(eax);   // 6d9596: mov dword ptr [esp + 0x38], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x30));   // 6d959a: mov eax, dword ptr [esp + 0x30]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ebp);   // 6d959e: mov dword ptr [edx + 4], ebp
    esp -= 4; M32(esp) = ecx;   // 6d95a1: push ecx
    ecx = esi;   // 6d95a2: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d95a4: mov dword ptr [edx + 8], eax
    { CVector* out_ = (CVector*)(uintptr_t)M32(esp + 0); *out_ = v.GetSpeed(CVector{ (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)), (*(float*)(uintptr_t)(esp + 12)) }); eax = (uint32)(uintptr_t)out_; esp += 0x10; }   // 6d95a7: call 0x542ce0
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 8)));   // 6d95ac: fld dword ptr [eax + 8]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x28)));   // ST0 = ST0 * m   // 6d95af: fmul dword ptr [edi + 0x28]
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 4)));   // 6d95b2: fld dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x24)));   // ST0 = ST0 * m   // 6d95b5: fmul dword ptr [edi + 0x24]
    fp.Arith('+', 1, 1, 0, true);   // 6d95b8: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax)));   // 6d95ba: fld dword ptr [eax]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d95bc: mov eax, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 * m   // 6d95c2: fmul dword ptr [edi + 0x20]
    esp -= 0xCu;   // 6d95c5: sub esp, 0xc
    edx = esp;   // 6d95c8: mov edx, esp
    esp -= 0xCu;   // 6d95ca: sub esp, 0xc
    fp.Arith('+', 1, 1, 0, true);   // 6d95cd: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d95cf: fmul dword ptr [0x858c1c]
    fp.FldSt(0);   // 6d95d5: fld st(0)
    fp.Fabs();   // 6d95d7: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x24)));   // ST0 = ST0 * m   // 6d95d9: fmul dword ptr [eax + 0x24]
    fp.Arith('*', 1, 1, 0, true);   // 6d95dc: fmulp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d95de: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x20)));   // ST0 = ST0 * m   // 6d95e2: fmul dword ptr [eax + 0x20]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // ST0 = ST0 * m   // 6d95e5: fmul dword ptr [esp + 0x284]
    fp.Arith('+', 1, 1, 0, true);   // 6d95ec: faddp st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d95ee: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d95f4: fmul dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d95fa: fld dword ptr [esp + 0x40]
    fp.Arith('*', 0, 0, 1, false);   // 6d95fe: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d9600: fstp dword ptr [esp + 0x40]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9604: fld dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6d9608: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d960a: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d960e: fld dword ptr [esp + 0x48]
    fp.Arith('*', 0, 0, 1, false);   // 6d9612: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9614: fstp dword ptr [esp + 0x48]
    fp.FstpSt(0);   // 6d9618: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x68)));   // 6d961a: fld dword ptr [esp + 0x68]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // ST0 = ST0 + m   // 6d961e: fadd dword ptr [esp + 0x34]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6d9622: fstp dword ptr [esp + 0x4c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d9626: mov eax, dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6d962a: fld dword ptr [esp + 0x6c]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6d962e: mov dword ptr [edx], eax
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // ST0 = ST0 + m   // 6d9630: fadd dword ptr [esp + 0x38]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d9634: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d9638: fld dword ptr [esp + 0x70]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6d963c: mov ecx, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // ST0 = ST0 + m   // 6d9640: fadd dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d9644: mov dword ptr [edx + 4], ecx
    ecx = esp;   // 6d9647: mov ecx, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d9649: fstp dword ptr [esp + 0x54]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6d964d: mov eax, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9651: mov dword ptr [edx + 8], eax
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d9654: mov edx, dword ptr [esp + 0x40]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9658: mov eax, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d965c: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d965e: mov edx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6d9662: mov dword ptr [ecx + 4], eax
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d9665: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6d9668: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d966a: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d966f: fld dword ptr [esp + 0x48]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9673: mov edi, dword ptr [esi + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x28)));   // ST0 = ST0 * m   // 6d9676: fmul dword ptr [edi + 0x28]
    ecx = (uint32)(esp + 0x40);   // 6d9679: lea ecx, [esp + 0x40]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d967d: fld dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x24)));   // ST0 = ST0 * m   // 6d9681: fmul dword ptr [edi + 0x24]
    fp.Arith('+', 1, 1, 0, true);   // 6d9684: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d9686: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 * m   // 6d968a: fmul dword ptr [edi + 0x20]
    fp.Arith('+', 1, 1, 0, true);   // 6d968d: faddp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d968f: fstp dword ptr [esp + 0x14]
    FlcVecMagnitude(fp, (float*)(uintptr_t)ecx);   // 6d9693: call 0x4082c0
    esp -= 4; M32(esp) = ecx;   // 6d9698: push ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9699: fstp dword ptr [esp]
    esp -= 4; M32(esp) = 0x3C23D70Au;   // 6d969c: push 0x3c23d70a
    FlcFloatMax(fp, (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)));   // 6d96a1: call 0x420800
    fp.Fdivr((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // ST0 = m / ST0   // 6d96a6: fdivr dword ptr [esp + 0x1c]
    esp += 0x4u;   // 6d96aa: add esp, 4
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d96ad: fstp dword ptr [esp]
    esp -= 4; M32(esp) = 0x3F800000u;   // 6d96b0: push 0x3f800000
    FlcFloatMin(fp, (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)));   // 6d96b5: call 0x404330
    esp += 0x4u;   // 6d96ba: add esp, 4
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d96bd: fstp dword ptr [esp]
    esp -= 4; M32(esp) = 0xBF800000u;   // 6d96c0: push 0xbf800000
    FlcFloatMax(fp, (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)));   // 6d96c5: call 0x420800
    esp += 0x8u;   // 6d96ca: add esp, 8
    FlcCrtAsin(fp);   // 6d96cd: call 0x821e70
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x594));   // 6d96d2: mov ebx, dword ptr [esi + 0x594]
    fA = ebx; fB = 0x4u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d96d8: cmp ebx, 4
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d96db: fmul dword ptr [0x858c1c]
    if (fR != 0) goto L_6d970d;   // 6d96e1: jne 0x6d970d
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871d98)), false);   // 6d96e3: fcom dword ptr [0x871d98]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d96e9: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d96eb: test ah, 0x41
    if (fR != 0) goto L_6d970d;   // 6d96ee: jne 0x6d970d
    fp.Fld(CTimer::ms_fTimeStep);   // 6d96f0: fld dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b38)));   // ST0 = ST0 * m   // 6d96f6: fmul dword ptr [0x858b38]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c4c)));   // ST0 = ST0 * m   // 6d96fc: fmul dword ptr [0x858c4c]
    eax = (uint32)FlcCrtFtol2(fp);   // 6d9702: call 0x821b40
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x9a0)); fB = eax; fR = (fA + fB) & 0xFFFFFFFFu; *(uint32*)(uintptr_t)(uint32)(esi + 0x9a0) = (uint32)(fR);   // 6d9707: add dword ptr [esi + 0x9a0], eax
L_6d970d:;
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x264));   // 6d970d: mov edx, dword ptr [esp + 0x264]
    fA = edx; fB = 0x1u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d9714: cmp edx, 1
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9717: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x28)));   // 6d971d: fld dword ptr [ecx + 0x28]
    if (fR != 0) goto L_6d974f;   // 6d9720: jne 0x6d974f
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8d3670)));   // 6d9722: fld dword ptr [0x8d3670]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9728: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d972e: fstp dword ptr [esp + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9732: fld dword ptr [esp + 0x18]
    fp.Arith('*', 0, 0, 1, false);   // 6d9736: fmul st(1)
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x14)), true);   // 6d9738: fcomp dword ptr [esp + 0x14]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d973c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d973e: test ah, 0x41
    if (fR != 0) goto L_6d976a;   // 6d9741: jne 0x6d976a
    fp.FstpSt(0);   // 6d9743: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d9745: fld dword ptr [esp + 0x14]
    fp.Fdiv((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 / m   // 6d9749: fdiv dword ptr [esp + 0x18]
    goto L_6d976a;   // 6d974d: jmp 0x6d976a
L_6d974f:;
    fA = ebx; fB = 0x4u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d974f: cmp ebx, 4
    if (fR != 0) goto L_6d976a;   // 6d9752: jne 0x6d976a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x9cc)));   // 6d9754: fld dword ptr [esi + 0x9cc]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d975a: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9760: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9762: test ah, 5
    if (FlcParityEven(fR)) goto L_6d976a;   // 6d9765: jp 0x6d976a
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x34)));   // ST0 = ST0 * m   // 6d9767: fmul dword ptr [ecx + 0x34]
L_6d976a:;
    fp.Fxch(1);   // 6d976a: fxch st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x2c)));   // ST0 = ST0 * m   // 6d976c: fmul dword ptr [ecx + 0x2c]
    fp.Arith('+', 0, 0, 1, false);   // 6d976f: fadd st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // ST0 = ST0 * m   // 6d9771: fmul dword ptr [esi + 0x8c]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9777: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 * m   // 6d977d: fmul dword ptr [esp + 0x18]
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d9781: fst dword ptr [esp + 0x14]
    fp.FstpSt(1);   // 6d9785: fstp st(1)
    fp.Fld(CTimer::ms_fTimeStep);   // 6d9787: fld dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // ST0 = ST0 * m   // 6d978d: fmul dword ptr [esi + 0x8c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9793: fmul dword ptr [0x863984]
    fp.Fxch(1);   // 6d9799: fxch st(1)
    fp.Fcompp();   // 6d979b: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d979d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d979f: test ah, 0x41
    if (fR != 0) goto L_6d983a;   // 6d97a2: jne 0x6d983a
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d97a8: test edi, edi
    eax = (uint32)(edi + 0x30);   // 6d97aa: lea eax, [edi + 0x30]
    if (fR != 0) goto L_6d97b2;   // 6d97ad: jne 0x6d97b2
    eax = (uint32)(esi + 4);   // 6d97af: lea eax, [esi + 4]
L_6d97b2:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d97b2: mov eax, dword ptr [eax + 8]
    esp -= 4; M32(esp) = edx;   // 6d97b5: push edx
    esp -= 4; M32(esp) = eax;   // 6d97b6: push eax
    ecx = esi;   // 6d97b7: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d97b9: call 0x6d2600
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d97be: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d97c4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d97c6: test ah, 0x41
    if (fR != 0) goto L_6d983a;   // 6d97c9: jne 0x6d983a
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d97cb: test edi, edi
    eax = (uint32)(edi + 0x30);   // 6d97cd: lea eax, [edi + 0x30]
    if (fR != 0) goto L_6d97d5;   // 6d97d0: jne 0x6d97d5
    eax = (uint32)(esi + 4);   // 6d97d2: lea eax, [esi + 4]
L_6d97d5:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d97d5: mov ecx, dword ptr [eax + 8]
    esp -= 4; M32(esp) = edx;   // 6d97d8: push edx
    esp -= 4; M32(esp) = ecx;   // 6d97d9: push ecx
    ecx = esi;   // 6d97da: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d97dc: call 0x6d2600
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b38)));   // ST0 = ST0 * m   // 6d97e1: fmul dword ptr [0x858b38]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d97e7: fsubr dword ptr [0x858624]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d97ed: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d97f3: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d97f5: test ah, 5
    if (FlcParityEven(fR)) goto L_6d9802;   // 6d97f8: jp 0x6d9802
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d97fa: fld dword ptr [0x858b50]
    goto L_6d9824;   // 6d9800: jmp 0x6d9824
L_6d9802:;
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9802: test edi, edi
    eax = (uint32)(edi + 0x30);   // 6d9804: lea eax, [edi + 0x30]
    if (fR != 0) goto L_6d980c;   // 6d9807: jne 0x6d980c
    eax = (uint32)(esi + 4);   // 6d9809: lea eax, [esi + 4]
L_6d980c:;
    esp -= 4; M32(esp) = edx;   // 6d980c: push edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d980d: mov edx, dword ptr [eax + 8]
    esp -= 4; M32(esp) = edx;   // 6d9810: push edx
    ecx = esi;   // 6d9811: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d9813: call 0x6d2600
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b38)));   // ST0 = ST0 * m   // 6d9818: fmul dword ptr [0x858b38]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d981e: fsubr dword ptr [0x858624]
L_6d9824:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // ST0 = ST0 * m   // 6d9824: fmul dword ptr [esi + 0x8c]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d982a: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9830: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d9836: fstp dword ptr [esp + 0x14]
L_6d983a:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x14));   // 6d983a: mov eax, dword ptr [esp + 0x14]
    fA = edi; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d983e: add edi, 0x20
    esp -= 4; M32(esp) = edi;   // 6d9841: push edi
    esp -= 4; M32(esp) = eax;   // 6d9842: push eax
    ecx = (uint32)(esp + 0x94);   // 6d9843: lea ecx, [esp + 0x94]
    esp -= 4; M32(esp) = ecx;   // 6d984a: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d984b: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9850: mov ecx, dword ptr [eax]
    edx = esp;   // 6d9852: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6d9854: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9856: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9859: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d985c: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6d985f: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9861: mov dword ptr [edx + 8], eax
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9864: call 0x5429f0
    return;   // 6d9869: jmp 0x6da900
}
void FlcRouteRotor(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    alignas(16) uint8 stk[0xB00] = {};                      // pila x86 del EXE (el marco [F+x] empieza en stk + 0x800)
    uint32 eax = 0, ecx = 0, edx = 0, ebx = (uint32)(uintptr_t)f.pad, esi = (uint32)(uintptr_t)f.v, edi = 0, ebp = 0;
    uint32 esp = (uint32)(uintptr_t)stk + 0x800;            // F
    uint32 fA = 0, fB = 0, fR = 0;
    M32(esp + 0x264) = (uint32)f.model; memcpy(stk + 0x800 + 0x268, &f.skid, 4); memcpy(stk + 0x800 + 0x26C, &f.upDown, 4);
    memcpy(stk + 0x800 + 0x270, &f.sideways, 4); memcpy(stk + 0x800 + 0x274, &f.accel, 4);
    M32(esp + 0x14) = ebx;                                  // mando del conductor
    memcpy(stk + 0x800 + 0x40, f.vel, 12);                  // velocidad + deriva del viento
    memcpy(stk + 0x800 + 0x50, f.com, 12);                  // centro de masas en el mundo
    M32(esp + 0x4C) = (uint32)(uintptr_t)&v.m_vecMoveSpeed;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d986e: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x3c)));   // 6d9874: fld dword ptr [ecx + 0x3c]
    fp.Fld(CTimer::ms_fTimeStep);   // 6d9877: fld dword ptr [0xb7cb5c]
    FlcCrtPow(fp);   // 6d987d: call 0x822130
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d9882: mov eax, dword ptr [esp + 0x4c]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x264));   // 6d9886: mov ebp, dword ptr [esp + 0x264]
    fA = ebp; fB = 0x8u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d988d: cmp ebp, 8
    fp.FldSt(0);   // 6d9890: fld st(0)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6d9892: fmul dword ptr [eax]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(eax)));   // 6d9894: fstp dword ptr [eax]
    fp.FldSt(0);   // 6d9896: fld st(0)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x48)));   // ST0 = ST0 * m   // 6d9898: fmul dword ptr [esi + 0x48]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esi + 0x48)));   // 6d989b: fstp dword ptr [esi + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x4c)));   // ST0 = ST0 * m   // 6d989e: fmul dword ptr [esi + 0x4c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esi + 0x4c)));   // 6d98a1: fstp dword ptr [esi + 0x4c]
    if (fR != 0) goto L_6d9b9d;   // 6d98a4: jne 0x6d9b9d
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d98aa: mov eax, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d98ad: fld dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 * m   // 6d98b1: fmul dword ptr [eax + 0x18]
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d98b4: add eax, 0x10
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d98b7: fld dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6d98bb: fmul dword ptr [eax + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d98be: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d98c0: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6d98c4: fmul dword ptr [eax]
    fp.Arith('+', 1, 1, 0, true);   // 6d98c6: faddp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d98c8: fstp dword ptr [esp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d98cc: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), false);   // 6d98d3: fcom dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d98d9: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d98db: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d9912;   // 6d98de: jp 0x6d9912
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d98e0: test ebx, ebx
    fp.FstpSt(0);   // 6d98e2: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d98e4: fld dword ptr [0x858b50]
    if (fR == 0) goto L_6d9912;   // 6d98ea: je 0x6d9912
    ecx = ebx;   // 6d98ec: mov ecx, ebx
    fp.FstpSt(0);   // 6d98ee: fstp st(0)
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetAccelerate();   // 6d98f0: call 0x5403f0
    ecx = ebx;   // 6d98f5: mov ecx, ebx
    edi = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d98f7: movsx edi, ax
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetBrake();   // 6d98fa: call 0x540080
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d98ff: movsx edx, ax
    fA = edi; fB = edx; fR = (fA - fB) & 0xFFFFFFFFu; edi = fR;   // 6d9902: sub edi, edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edi);   // 6d9904: mov dword ptr [esp + 0x18], edi
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9908: fild dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859a3c)));   // ST0 = ST0 * m   // 6d990c: fmul dword ptr [0x859a3c]
L_6d9912:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9912: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9916: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d991c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d991e: test ah, 0x41
    if (fR == 0) goto L_6d9965;   // 6d9921: je 0x6d9965
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), false);   // 6d9923: fcom dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9929: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d992b: test ah, 0x41
    if (fR == 0) goto L_6d9965;   // 6d992e: je 0x6d9965
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9930: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9936: fld dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6d993a: fmul dword ptr [ecx + 8]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859000)));   // ST0 = ST0 * m   // 6d993d: fmul dword ptr [0x859000]
    fp.Arith('-', 1, 1, 0, true);   // 6d9943: fsubp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d9945: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d994b: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d994d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d994f: test ah, 5
    if (FlcParityEven(fR)) goto L_6d995c;   // 6d9952: jp 0x6d995c
    fp.FstpSt(0);   // 6d9954: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d9956: fld dword ptr [0x858b50]
L_6d995c:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d995c: fmul dword ptr [ecx + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d995f: fstp dword ptr [esp + 0x18]
    goto L_6d997d;   // 6d9963: jmp 0x6d997d
L_6d9965:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9965: mov eax, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d996b: fld dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 * m   // 6d996f: fmul dword ptr [eax + 8]
    fp.Arith('-', 0, 1, 0, false);   // 6d9972: fsubr st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6d9974: fmul dword ptr [eax + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9977: fstp dword ptr [esp + 0x18]
    fp.FstpSt(0);   // 6d997b: fstp st(0)
L_6d997d:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d997d: mov eax, dword ptr [0xb7cb5c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9982: mov ecx, dword ptr [esi + 0x8c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x18));   // 6d9988: mov edx, dword ptr [esp + 0x18]
    esp -= 4; M32(esp) = eax;   // 6d998c: push eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d998d: mov eax, dword ptr [esi + 0x14]
    esp -= 4; M32(esp) = ecx;   // 6d9990: push ecx
    esp -= 4; M32(esp) = edx;   // 6d9991: push edx
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d9992: add eax, 0x10
    esp -= 4; M32(esp) = eax;   // 6d9995: push eax
    ecx = (uint32)(esp + 0x240);   // 6d9996: lea ecx, [esp + 0x240]
    esp -= 4; M32(esp) = ecx;   // 6d999d: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d999e: call 0x40fec0
    esp += 0xCu;   // 6d99a3: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d99a6: push eax
    edx = (uint32)(esp + 0x14c);   // 6d99a7: lea edx, [esp + 0x14c]
    esp -= 4; M32(esp) = edx;   // 6d99ae: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d99af: call 0x40fec0
    esp += 0xCu;   // 6d99b4: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d99b7: push eax
    eax = (uint32)(esp + 0x1d8);   // 6d99b8: lea eax, [esp + 0x1d8]
    esp -= 4; M32(esp) = eax;   // 6d99bf: push eax
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d99c0: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d99c5: mov edx, dword ptr [eax]
    ecx = esp;   // 6d99c7: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d99c9: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d99cb: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d99ce: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d99d1: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d99d4: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d99d7: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d99d9: call 0x5429f0
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d99de: mov edi, dword ptr [esi + 0x14]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0x8d34e8));   // 6d99e1: mov edx, dword ptr [0x8d34e8]
    ecx = (uint32)(edi + 0x10);   // 6d99e7: lea ecx, [edi + 0x10]
    esp -= 4; M32(esp) = ecx;   // 6d99ea: push ecx
    esp -= 4; M32(esp) = edx;   // 6d99eb: push edx
    eax = (uint32)(esp + 0x160);   // 6d99ec: lea eax, [esp + 0x160]
    esp -= 4; M32(esp) = eax;   // 6d99f3: push eax
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d99f4: call 0x40fe90
    esp -= 4; M32(esp) = eax;   // 6d99f9: push eax
    fA = edi; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d99fa: add edi, 0x20
    ecx = (uint32)(esp + 0x2c);   // 6d99fd: lea ecx, [esp + 0x2c]
    esp -= 4; M32(esp) = edi;   // 6d9a01: push edi
    esp -= 4; M32(esp) = ecx;   // 6d9a02: push ecx
    FlcVecSub(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9a03: call 0x40fe60
    esp += 0x18u;   // 6d9a08: add esp, 0x18
    ecx = (uint32)(esp + 0x1c);   // 6d9a0b: lea ecx, [esp + 0x1c]
    FlcVecNormalise(fp, (float*)(uintptr_t)ecx);   // 6d9a0f: call 0x59c910
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9a14: fld dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // ST0 = ST0 * m   // 6d9a18: fmul dword ptr [esp + 0x40]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x590));   // 6d9a1c: mov eax, dword ptr [esi + 0x590]
    fA = eax; fB = eax; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9a22: test eax, eax
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d9a24: fld dword ptr [esp + 0x24]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // ST0 = ST0 * m   // 6d9a28: fmul dword ptr [esp + 0x48]
    fp.Arith('+', 1, 1, 0, true);   // 6d9a2c: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9a2e: fld dword ptr [esp + 0x20]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // ST0 = ST0 * m   // 6d9a32: fmul dword ptr [esp + 0x44]
    fp.Arith('+', 1, 1, 0, true);   // 6d9a36: faddp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9a38: fstp dword ptr [esp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8717ac)));   // 6d9a3c: fld dword ptr [0x8717ac]
    if (fR != 0) goto L_6d9b04;   // 6d9a42: jne 0x6d9b04
    fp.FstpSt(0);   // 6d9a48: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8d34d8)));   // 6d9a4a: fld dword ptr [0x8d34d8]
    fp.Fchs();   // 6d9a50: fchs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x10)), false);   // 6d9a52: fcom dword ptr [esp + 0x10]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9a56: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9a58: test ah, 0x41
    if (fR != 0) goto L_6d9a61;   // 6d9a5b: jne 0x6d9a61
    fp.FldSt(0);   // 6d9a5d: fld st(0)
    goto L_6d9a65;   // 6d9a5f: jmp 0x6d9a65
L_6d9a61:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9a61: fld dword ptr [esp + 0x10]
L_6d9a65:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d9a65: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d9a6b: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9a6d: fnstsw ax
    fp.FstpSt(0);   // 6d9a6f: fstp st(0)
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9a71: test ah, 5
    if (FlcParityEven(fR)) goto L_6d9ad6;   // 6d9a74: jp 0x6d9ad6
    *(uint32*)(uintptr_t)(uint32)(esp + 0x10) = (uint32)(0x0u);   // 6d9a76: mov dword ptr [esp + 0x10], 0
L_6d9a7e:;
    fp.FstpSt(0);   // 6d9a7e: fstp st(0)
L_6d9a80:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9a80: fld dword ptr [esp + 0x10]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9a84: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x8d34d4)));   // ST0 = ST0 * m   // 6d9a8a: fmul dword ptr [0x8d34d4]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // ST0 = m - ST0   // 6d9a90: fsubr dword ptr [esi + 0x84c]
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9a96: fst dword ptr [esp + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // 6d9a9a: fstp dword ptr [esi + 0x84c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8d34dc)));   // 6d9aa0: fld dword ptr [0x8d34dc]
    fp.Fld(CTimer::ms_fTimeStep);   // 6d9aa6: fld dword ptr [0xb7cb5c]
    FlcCrtPow(fp);   // 6d9aac: call 0x822130
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 * m   // 6d9ab1: fmul dword ptr [esp + 0x18]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858ee8)), false);   // 6d9ab5: fcom dword ptr [0x858ee8]
    fp.Fst((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // 6d9abb: fst dword ptr [esi + 0x84c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9ac1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9ac3: test ah, 0x41
    if (fR != 0) goto L_6d9ae7;   // 6d9ac6: jne 0x6d9ae7
    fp.FstpSt(0);   // 6d9ac8: fstp st(0)
    *(uint32*)(uintptr_t)(uint32)(esi + 0x84c) = (uint32)(0x3ECCCCCDu);   // 6d9aca: mov dword ptr [esi + 0x84c], 0x3ecccccd
    goto L_6d9afe;   // 6d9ad4: jmp 0x6d9afe
L_6d9ad6:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x10)), false);   // 6d9ad6: fcom dword ptr [esp + 0x10]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9ada: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9adc: test ah, 0x41
    if (fR != 0) goto L_6d9a7e;   // 6d9adf: jne 0x6d9a7e
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9ae1: fstp dword ptr [esp + 0x10]
    goto L_6d9a80;   // 6d9ae5: jmp 0x6d9a80
L_6d9ae7:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x859018)), true);   // 6d9ae7: fcomp dword ptr [0x859018]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9aed: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9aef: test ah, 5
    if (FlcParityEven(fR)) goto L_6d9afe;   // 6d9af2: jp 0x6d9afe
    *(uint32*)(uintptr_t)(uint32)(esi + 0x84c) = (uint32)(0x3DA3D70Au);   // 6d9af4: mov dword ptr [esi + 0x84c], 0x3da3d70a
L_6d9afe:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // 6d9afe: fld dword ptr [esi + 0x84c]
L_6d9b04:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9b04: fld dword ptr [esp + 0x1c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d9b08: mov edx, dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9b0e: fmul dword ptr [0x863984]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9b14: mov eax, dword ptr [esi + 0x8c]
    esp -= 4; M32(esp) = edx;   // 6d9b1a: push edx
    esp -= 4; M32(esp) = eax;   // 6d9b1b: push eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d9b1c: fstp dword ptr [esp + 0x3c]
    esp -= 4; M32(esp) = ecx;   // 6d9b20: push ecx
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9b21: fld dword ptr [esp + 0x2c]
    ecx = (uint32)(esp + 0x40);   // 6d9b25: lea ecx, [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9b29: fmul dword ptr [0x863984]
    edx = (uint32)(esp + 0x224);   // 6d9b2f: lea edx, [esp + 0x224]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9b36: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9b3a: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9b3e: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9b44: fstp dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x8d34e0)));   // ST0 = ST0 * m   // 6d9b48: fmul dword ptr [0x8d34e0]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9b4e: fld dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x8d34e4)));   // ST0 = ST0 * m   // 6d9b52: fmul dword ptr [0x8d34e4]
    fp.Arith('-', 1, 1, 0, true);   // 6d9b58: fsubp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9b5a: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6d9b5d: push ecx
    esp -= 4; M32(esp) = edx;   // 6d9b5e: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9b5f: call 0x40fec0
    esp += 0xCu;   // 6d9b64: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9b67: push eax
    eax = (uint32)(esp + 0x17c);   // 6d9b68: lea eax, [esp + 0x17c]
    esp -= 4; M32(esp) = eax;   // 6d9b6f: push eax
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9b70: call 0x40fec0
    esp += 0xCu;   // 6d9b75: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9b78: push eax
    ecx = (uint32)(esp + 0xa0);   // 6d9b79: lea ecx, [esp + 0xa0]
    esp -= 4; M32(esp) = ecx;   // 6d9b80: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9b81: call 0x40fec0
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9b86: mov ecx, dword ptr [eax]
    edx = esp;   // 6d9b88: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6d9b8a: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9b8c: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9b8f: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d9b92: mov dword ptr [edx + 4], ecx
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9b95: mov dword ptr [edx + 8], eax
    goto L_6d9ef2;   // 6d9b98: jmp 0x6d9ef2
L_6d9b9d:;
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9b9d: mov edi, dword ptr [esi + 0x14]
    ecx = (uint32)(edi + 0x20);   // 6d9ba0: lea ecx, [edi + 0x20]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx));   // 6d9ba3: mov edx, dword ptr [ecx]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 4));   // 6d9ba5: mov eax, dword ptr [ecx + 4]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 8));   // 6d9ba8: mov ecx, dword ptr [ecx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(eax);   // 6d9bab: mov dword ptr [esp + 0x20], eax
    fA = (uint32)(*(uint8*)(uintptr_t)(uint32)(esi + 0x42b)); fB = 0x10u; fR = (fA & fB) & 0xFFu;   // 6d9baf: test byte ptr [esi + 0x42b], 0x10
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(edx);   // 6d9bb6: mov dword ptr [esp + 0x1c], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(ecx);   // 6d9bba: mov dword ptr [esp + 0x24], ecx
    if (fR == 0) goto L_6d9c04;   // 6d9bbe: je 0x6d9c04
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9bc0: fld dword ptr [esp + 0x1c]
    FlcCrtAsin(fp);   // 6d9bc4: call 0x821e70
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b90)));   // ST0 = ST0 * m   // 6d9bc9: fmul dword ptr [0x858b90]
    fp.Fsin();   // 6d9bcf: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9bd1: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9bd5: fld dword ptr [esp + 0x20]
    FlcCrtAsin(fp);   // 6d9bd9: call 0x821e70
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b90)));   // ST0 = ST0 * m   // 6d9bde: fmul dword ptr [0x858b90]
    fp.Fsin();   // 6d9be4: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9be6: fstp dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d9bea: fld dword ptr [esp + 0x24]
    FlcCrtAcos(fp);   // 6d9bee: call 0x822380
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b90)));   // ST0 = ST0 * m   // 6d9bf3: fmul dword ptr [0x858b90]
    fp.Fcos();   // 6d9bf9: fcos 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d9bfb: fstp dword ptr [esp + 0x24]
    goto L_6d9c95;   // 6d9bff: jmp 0x6d9c95
L_6d9c04:;
    fA = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22)); fB = 0x208u; fR = (fA - fB) & 0xFFFFu;   // 6d9c04: cmp word ptr [esi + 0x22], 0x208
    if (fR != 0) goto L_6d9c95;   // 6d9c0a: jne 0x6d9c95
    edx = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x86c));   // 6d9c10: movzx edx, word ptr [esi + 0x86c]
    eax = (uint32)(int32_t)(int16_t)((uint32)(*(uint16*)(uintptr_t)(uint32)(0x8d33c4)));   // 6d9c17: movsx eax, word ptr [0x8d33c4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edx);   // 6d9c1e: mov dword ptr [esp + 0x18], edx
    ecx = (uint32)(edi + 0x20);   // 6d9c22: lea ecx, [edi + 0x20]
    esp -= 4; M32(esp) = ecx;   // 6d9c25: push ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9c26: fild dword ptr [esp + 0x1c]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(eax);   // 6d9c2a: mov dword ptr [esp + 0x1c], eax
    esp -= 4; M32(esp) = ecx;   // 6d9c2e: push ecx
    edx = (uint32)(esp + 0xac);   // 6d9c2f: lea edx, [esp + 0xac]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 * m   // 6d9c36: fmul dword ptr [0x858fe4]
    fp.FidivI((*(int*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9c3c: fidiv dword ptr [esp + 0x20]
    fp.FldSt(0);   // 6d9c40: fld st(0)
    fp.Fcos();   // 6d9c42: fcos 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9c44: fstp dword ptr [esp + 0x20]
    fp.Fsin();   // 6d9c48: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9c4a: fstp dword ptr [esp]
    esp -= 4; M32(esp) = edx;   // 6d9c4d: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9c4e: call 0x40fe90
    esp += 0xCu;   // 6d9c53: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9c56: push eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x1c));   // 6d9c57: mov eax, dword ptr [esp + 0x1c]
    fA = edi; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d9c5b: add edi, 0x10
    esp -= 4; M32(esp) = edi;   // 6d9c5e: push edi
    esp -= 4; M32(esp) = eax;   // 6d9c5f: push eax
    ecx = (uint32)(esp + 0xc8);   // 6d9c60: lea ecx, [esp + 0xc8]
    esp -= 4; M32(esp) = ecx;   // 6d9c67: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9c68: call 0x40fe90
    esp += 0xCu;   // 6d9c6d: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9c70: push eax
    edx = (uint32)(esp + 0xdc);   // 6d9c71: lea edx, [esp + 0xdc]
    esp -= 4; M32(esp) = edx;   // 6d9c78: push edx
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9c79: call 0x40fe30
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9c7e: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9c80: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9c83: mov eax, dword ptr [eax + 8]
    esp += 0xCu;   // 6d9c86: add esp, 0xc
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(ecx);   // 6d9c89: mov dword ptr [esp + 0x1c], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(edx);   // 6d9c8d: mov dword ptr [esp + 0x20], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(eax);   // 6d9c91: mov dword ptr [esp + 0x24], eax
L_6d9c95:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9c95: fld dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // ST0 = ST0 * m   // 6d9c99: fmul dword ptr [esp + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9c9d: fld dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6d9ca1: fmul dword ptr [esp + 0x20]
    fp.Arith('+', 1, 1, 0, true);   // 6d9ca5: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9ca7: fld dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // ST0 = ST0 * m   // 6d9cab: fmul dword ptr [esp + 0x40]
    fp.Arith('+', 1, 1, 0, true);   // 6d9caf: faddp st(1)
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9cb1: fst dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9cb5: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9cbb: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9cbd: test ah, 0x41
    if (fR != 0) goto L_6d9ccc;   // 6d9cc0: jne 0x6d9ccc
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9cc2: fld dword ptr [esp + 0x10]
    fp.Arith('+', 0, 0, 0, false);   // 6d9cc6: fadd st(0), st(0)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9cc8: fstp dword ptr [esp + 0x10]
L_6d9ccc:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9ccc: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d9cd3: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9cd9: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d9cdb: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d9d5a;   // 6d9cde: jp 0x6d9d5a
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9ce0: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x274) = (uint32)(0x0u);   // 6d9ce2: mov dword ptr [esp + 0x274], 0
    if (fR == 0) goto L_6d9d5a;   // 6d9ced: je 0x6d9d5a
    ecx = ebx;   // 6d9cef: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetAccelerate();   // 6d9cf1: call 0x5403f0
    ecx = ebx;   // 6d9cf6: mov ecx, ebx
    edi = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9cf8: movsx edi, ax
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetBrake();   // 6d9cfb: call 0x540080
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9d00: movsx ecx, ax
    fA = edi; fB = ecx; fR = (fA - fB) & 0xFFFFFFFFu; edi = fR;   // 6d9d03: sub edi, ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edi);   // 6d9d05: mov dword ptr [esp + 0x18], edi
    ecx = ebx;   // 6d9d09: mov ecx, ebx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9d0b: fild dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859a3c)));   // ST0 = ST0 * m   // 6d9d0f: fmul dword ptr [0x859a3c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9d15: fstp dword ptr [esp + 0x274]
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d9d1c: call 0x53fc10
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9d21: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edx);   // 6d9d24: mov dword ptr [esp + 0x18], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9d28: fild dword ptr [esp + 0x18]
    fp.Fabs();   // 6d9d2c: fabs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d9d2e: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9d34: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9d36: test ah, 0x41
    if (fR != 0) goto L_6d9d5a;   // 6d9d39: jne 0x6d9d5a
    ecx = ebx;   // 6d9d3b: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d9d3d: call 0x53fc10
    eax = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9d42: movsx eax, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(eax);   // 6d9d45: mov dword ptr [esp + 0x18], eax
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9d49: fild dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d9d4d: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9d53: fstp dword ptr [esp + 0x274]
L_6d9d5a:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9d5a: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9d60: fld dword ptr [esp + 0x274]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d9d67: fmul dword ptr [ecx + 4]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d9d6a: mov edx, dword ptr [0xb7cb5c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9d70: mov eax, dword ptr [esi + 0x8c]
    esp -= 4; M32(esp) = edx;   // 6d9d76: push edx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x8631d0)));   // ST0 = ST0 + m   // 6d9d77: fadd dword ptr [0x8631d0]
    esp -= 4; M32(esp) = eax;   // 6d9d7d: push eax
    esp -= 4; M32(esp) = 0x3C03126Fu;   // 6d9d7e: push 0x3c03126f
    esp -= 4; M32(esp) = ecx;   // 6d9d83: push ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9d84: fstp dword ptr [esp + 0x28]
    ecx = (uint32)(esp + 0x44);   // 6d9d88: lea ecx, [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b8c)));   // 6d9d8c: fld dword ptr [0x858b8c]
    edx = (uint32)(esp + 0xfc);   // 6d9d92: lea edx, [esp + 0xfc]
    fp.Fsub((*(float*)(uintptr_t)(uint32)(esi + 0x4c)));   // ST0 = ST0 - m   // 6d9d99: fsub dword ptr [esi + 0x4c]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x44) = (uint32)(0x0u);   // 6d9d9c: mov dword ptr [esp + 0x44], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x48) = (uint32)(0x0u);   // 6d9da4: mov dword ptr [esp + 0x48], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x4c) = (uint32)(0x3F800000u);   // 6d9dac: mov dword ptr [esp + 0x4c], 0x3f800000
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9db4: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6d9db7: push ecx
    esp -= 4; M32(esp) = edx;   // 6d9db8: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9db9: call 0x40fec0
    esp += 0xCu;   // 6d9dbe: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9dc1: push eax
    eax = (uint32)(esp + 0x114);   // 6d9dc2: lea eax, [esp + 0x114]
    esp -= 4; M32(esp) = eax;   // 6d9dc9: push eax
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9dca: call 0x40fec0
    esp += 0xCu;   // 6d9dcf: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9dd2: push eax
    ecx = (uint32)(esp + 0x128);   // 6d9dd3: lea ecx, [esp + 0x128]
    esp -= 4; M32(esp) = ecx;   // 6d9dda: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9ddb: call 0x40fec0
    esp += 0xCu;   // 6d9de0: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9de3: push eax
    edx = (uint32)(esp + 0x13c);   // 6d9de4: lea edx, [esp + 0x13c]
    esp -= 4; M32(esp) = edx;   // 6d9deb: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9dec: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9df1: mov edx, dword ptr [eax]
    ecx = esp;   // 6d9df3: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d9df5: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9df7: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9dfa: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d9dfd: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d9e00: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d9e03: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9e05: call 0x5429f0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9e0a: fld dword ptr [esp + 0x10]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9e0e: mov ecx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6d9e14: fmul dword ptr [ecx + 8]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9e17: mov edx, dword ptr [esi + 0x14]
    fA = edx; fB = edx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9e1a: test edx, edx
    eax = (uint32)(edx + 0x30);   // 6d9e1c: lea eax, [edx + 0x30]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = m - ST0   // 6d9e1f: fsubr dword ptr [esp + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9e23: fstp dword ptr [esp + 0x18]
    if (fR != 0) goto L_6d9e2c;   // 6d9e27: jne 0x6d9e2c
    eax = (uint32)(esi + 4);   // 6d9e29: lea eax, [esi + 4]
L_6d9e2c:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9e2c: mov eax, dword ptr [eax + 8]
    esp -= 4; M32(esp) = ebp;   // 6d9e2f: push ebp
    esp -= 4; M32(esp) = eax;   // 6d9e30: push eax
    ecx = esi;   // 6d9e31: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d9e33: call 0x6d2600
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9e38: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9e3e: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9e40: test ah, 0x41
    if (fR != 0) goto L_6d9e6f;   // 6d9e43: jne 0x6d9e6f
    fA = edx; fB = edx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9e45: test edx, edx
    eax = (uint32)(edx + 0x30);   // 6d9e47: lea eax, [edx + 0x30]
    if (fR != 0) goto L_6d9e4f;   // 6d9e4a: jne 0x6d9e4f
    eax = (uint32)(esi + 4);   // 6d9e4c: lea eax, [esi + 4]
L_6d9e4f:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9e4f: mov ecx, dword ptr [eax + 8]
    esp -= 4; M32(esp) = ebp;   // 6d9e52: push ebp
    esp -= 4; M32(esp) = ecx;   // 6d9e53: push ecx
    ecx = esi;   // 6d9e54: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d9e56: call 0x6d2600
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x85862c)));   // ST0 = ST0 + m   // 6d9e5b: fadd dword ptr [0x85862c]
    fp.Fdivr((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = m / ST0   // 6d9e61: fdivr dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x85862c)));   // ST0 = ST0 * m   // 6d9e65: fmul dword ptr [0x85862c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9e6b: fstp dword ptr [esp + 0x18]
L_6d9e6f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9e6f: fld dword ptr [esp + 0x1c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d9e73: mov edx, dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9e79: fmul dword ptr [0x863984]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9e7f: mov eax, dword ptr [esi + 0x8c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x18));   // 6d9e85: mov ecx, dword ptr [esp + 0x18]
    esp -= 4; M32(esp) = edx;   // 6d9e89: push edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9e8a: fstp dword ptr [esp + 0x38]
    esp -= 4; M32(esp) = eax;   // 6d9e8e: push eax
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9e8f: fld dword ptr [esp + 0x28]
    esp -= 4; M32(esp) = ecx;   // 6d9e93: push ecx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9e94: fmul dword ptr [0x863984]
    edx = (uint32)(esp + 0x40);   // 6d9e9a: lea edx, [esp + 0x40]
    esp -= 4; M32(esp) = edx;   // 6d9e9e: push edx
    eax = (uint32)(esp + 0x15c);   // 6d9e9f: lea eax, [esp + 0x15c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9ea6: fstp dword ptr [esp + 0x48]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d9eaa: fld dword ptr [esp + 0x34]
    esp -= 4; M32(esp) = eax;   // 6d9eae: push eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9eaf: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d9eb5: fstp dword ptr [esp + 0x50]
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9eb9: call 0x40fec0
    esp += 0xCu;   // 6d9ebe: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9ec1: push eax
    ecx = (uint32)(esp + 0x170);   // 6d9ec2: lea ecx, [esp + 0x170]
    esp -= 4; M32(esp) = ecx;   // 6d9ec9: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9eca: call 0x40fec0
    esp += 0xCu;   // 6d9ecf: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9ed2: push eax
    edx = (uint32)(esp + 0x184);   // 6d9ed3: lea edx, [esp + 0x184]
    esp -= 4; M32(esp) = edx;   // 6d9eda: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9edb: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9ee0: mov edx, dword ptr [eax]
    ecx = esp;   // 6d9ee2: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d9ee4: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9ee6: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9ee9: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d9eec: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d9eef: mov dword ptr [ecx + 8], eax
L_6d9ef2:;
    ecx = esi;   // 6d9ef2: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9ef4: call 0x5429f0
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9ef9: mov ebx, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 0x28)));   // 6d9efc: fld dword ptr [ebx + 0x28]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9eff: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9f05: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9f07: test ah, 0x41
    if (fR != 0) goto L_6da142;   // 6d9f0a: jne 0x6da142
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9f10: mov ecx, dword ptr [esi + 0x388]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(0x0u);   // 6d9f16: mov dword ptr [esp + 0x28], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(0x0u);   // 6d9f1e: mov dword ptr [esp + 0x2c], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(0x3F800000u);   // 6d9f26: mov dword ptr [esp + 0x30], 0x3f800000
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 0x38));   // 6d9f2e: mov edx, dword ptr [ecx + 0x38]
    esp -= 4; M32(esp) = 0xC813E0u;   // 6d9f31: push 0xc813e0
    esp -= 4; M32(esp) = edx;   // 6d9f36: push edx
    eax = (uint32)(esp + 0x19c);   // 6d9f37: lea eax, [esp + 0x19c]
    esp -= 4; M32(esp) = eax;   // 6d9f3e: push eax
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9f3f: call 0x40fe90
    esp += 0xCu;   // 6d9f44: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9f47: push eax
    ecx = (uint32)(esp + 0x2c);   // 6d9f48: lea ecx, [esp + 0x2c]
    FlcVecAddTo(fp, (float*)(uintptr_t)ecx, (float*)(uintptr_t)M32(esp + 0)); eax = ecx; esp += 4;   // 6d9f4c: call 0x411a00
    ecx = (uint32)(esp + 0x28);   // 6d9f51: lea ecx, [esp + 0x28]
    FlcVecNormalise(fp, (float*)(uintptr_t)ecx);   // 6d9f55: call 0x59c910
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9f5a: mov ebp, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebp + 0x28)));   // 6d9f60: fld dword ptr [ebp + 0x28]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9f63: mov edi, dword ptr [esi + 0x14]
    fp.Fchs();   // 6d9f66: fchs 
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9f68: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9f6c: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9f6f: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9f73: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9f76: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9f78: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9f7c: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9f7e: faddp st(1)
    fp.FldSt(1);   // 6d9f80: fld st(1)
    fp.Fcompp();   // 6d9f82: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9f84: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9f86: test ah, 0x41
    if (fR != 0) goto L_6d9f8f;   // 6d9f89: jne 0x6d9f8f
    fp.FldSt(0);   // 6d9f8b: fld st(0)
    goto L_6d9fa7;   // 6d9f8d: jmp 0x6d9fa7
L_6d9f8f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9f8f: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9f93: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9f96: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9f9a: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9f9d: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9f9f: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9fa3: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fa5: faddp st(1)
L_6d9fa7:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(ebp + 0x28)), true);   // 6d9fa7: fcomp dword ptr [ebp + 0x28]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9faa: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9fac: test ah, 0x41
    if (fR != 0) goto L_6d9fbc;   // 6d9faf: jne 0x6d9fbc
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebp + 0x28));   // 6d9fb1: mov ecx, dword ptr [ebp + 0x28]
    fp.FstpSt(0);   // 6d9fb4: fstp st(0)
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6d9fb6: mov dword ptr [esp + 0x18], ecx
    goto L_6d9ffd;   // 6d9fba: jmp 0x6d9ffd
L_6d9fbc:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9fbc: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9fc0: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9fc3: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9fc7: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fca: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9fcc: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9fd0: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fd2: faddp st(1)
    fp.FldSt(1);   // 6d9fd4: fld st(1)
    fp.Fcompp();   // 6d9fd6: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9fd8: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9fda: test ah, 0x41
    if (fR == 0) goto L_6d9ff9;   // 6d9fdd: je 0x6d9ff9
    fp.FstpSt(0);   // 6d9fdf: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9fe1: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9fe5: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9fe8: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9fec: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fef: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9ff1: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9ff5: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9ff7: faddp st(1)
L_6d9ff9:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9ff9: fstp dword ptr [esp + 0x18]
L_6d9ffd:;
    edx = (uint32)(esp + 0x50);   // 6d9ffd: lea edx, [esp + 0x50]
    esp -= 4; M32(esp) = edx;   // 6da001: push edx
    eax = (uint32)(esp + 0x1b0);   // 6da002: lea eax, [esp + 0x1b0]
    esp -= 4; M32(esp) = edi;   // 6da009: push edi
    esp -= 4; M32(esp) = eax;   // 6da00a: push eax
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da00b: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da010: fld dword ptr [esp + 0x24]
    fp.Fchs();   // 6da014: fchs 
    esp += 0xCu;   // 6da016: add esp, 0xc
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebp + 0x2c)));   // ST0 = ST0 * m   // 6da019: fmul dword ptr [ebp + 0x2c]
    fA = edi; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6da01c: add edi, 0x20
    esp -= 4; M32(esp) = edi;   // 6da01f: push edi
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da020: fmul dword ptr [0xb7cb5c]
    esp -= 4; M32(esp) = ecx;   // 6da026: push ecx
    ecx = (uint32)(esp + 0x1cc);   // 6da027: lea ecx, [esp + 0x1cc]
    ebx = eax;   // 6da02e: mov ebx, eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da030: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6da036: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6da039: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da03a: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx));   // 6da03f: mov ecx, dword ptr [ebx]
    edx = esp;   // 6da041: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da043: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx + 4));   // 6da045: mov ecx, dword ptr [ebx + 4]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da048: mov dword ptr [edx + 4], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx + 8));   // 6da04b: mov ecx, dword ptr [ebx + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(ecx);   // 6da04e: mov dword ptr [edx + 8], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da051: mov ecx, dword ptr [eax]
    esp -= 0xCu;   // 6da053: sub esp, 0xc
    edx = esp;   // 6da056: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da058: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da05a: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da05d: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da060: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da063: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da065: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da068: call 0x542a50
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da06d: mov ebx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 0x28)));   // 6da073: fld dword ptr [ebx + 0x28]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da076: mov ebp, dword ptr [esi + 0x14]
    fp.Fchs();   // 6da079: fchs 
    ecx = (uint32)(ebp + 0x10);   // 6da07b: lea ecx, [ebp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da07e: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da082: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da085: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da089: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da08c: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da08e: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da092: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da094: faddp st(1)
    fp.FldSt(1);   // 6da096: fld st(1)
    fp.Fcompp();   // 6da098: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da09a: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da09c: test ah, 0x41
    if (fR != 0) goto L_6da0a5;   // 6da09f: jne 0x6da0a5
    fp.FldSt(0);   // 6da0a1: fld st(0)
    goto L_6da0bd;   // 6da0a3: jmp 0x6da0bd
L_6da0a5:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da0a5: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da0a9: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da0ac: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da0b0: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da0b3: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da0b5: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da0b9: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da0bb: faddp st(1)
L_6da0bd:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(ebx + 0x28)), true);   // 6da0bd: fcomp dword ptr [ebx + 0x28]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da0c0: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da0c2: test ah, 0x41
    if (fR != 0) goto L_6da0d2;   // 6da0c5: jne 0x6da0d2
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx + 0x28));   // 6da0c7: mov edx, dword ptr [ebx + 0x28]
    fp.FstpSt(0);   // 6da0ca: fstp st(0)
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edx);   // 6da0cc: mov dword ptr [esp + 0x18], edx
    goto L_6da113;   // 6da0d0: jmp 0x6da113
L_6da0d2:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da0d2: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da0d6: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da0d9: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da0dd: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da0e0: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da0e2: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da0e6: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da0e8: faddp st(1)
    fp.FldSt(1);   // 6da0ea: fld st(1)
    fp.Fcompp();   // 6da0ec: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da0ee: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da0f0: test ah, 0x41
    if (fR == 0) goto L_6da10f;   // 6da0f3: je 0x6da10f
    fp.FstpSt(0);   // 6da0f5: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da0f7: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da0fb: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da0fe: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da102: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da105: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da107: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da10b: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da10d: faddp st(1)
L_6da10f:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da10f: fstp dword ptr [esp + 0x18]
L_6da113:;
    eax = (uint32)(esp + 0x50);   // 6da113: lea eax, [esp + 0x50]
    esp -= 4; M32(esp) = eax;   // 6da117: push eax
    esp -= 4; M32(esp) = ecx;   // 6da118: push ecx
    ecx = (uint32)(esp + 0x1e4);   // 6da119: lea ecx, [esp + 0x1e4]
    esp -= 4; M32(esp) = ecx;   // 6da120: push ecx
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da121: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da126: fld dword ptr [esp + 0x24]
    esp += 0xCu;   // 6da12a: add esp, 0xc
    fp.Fchs();   // 6da12d: fchs 
    fA = ebp; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; ebp = fR;   // 6da12f: add ebp, 0x20
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebx + 0x2c)));   // ST0 = ST0 * m   // 6da132: fmul dword ptr [ebx + 0x2c]
    esp -= 4; M32(esp) = ebp;   // 6da135: push ebp
    edx = (uint32)(esp + 0x1f8);   // 6da136: lea edx, [esp + 0x1f8]
    goto L_6da22e;   // 6da13d: jmp 0x6da22e
L_6da142:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 8)));   // 6da142: fld dword ptr [ebx + 8]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da145: mov ebp, dword ptr [esi + 0x388]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6da14b: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da151: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6da153: test ah, 5
    if (FlcParityEven(fR)) goto L_6da161;   // 6da156: jp 0x6da161
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebp + 0x28));   // 6da158: mov ecx, dword ptr [ebp + 0x28]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6da15b: mov dword ptr [esp + 0x18], ecx
    goto L_6da16a;   // 6da15f: jmp 0x6da16a
L_6da161:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebp + 0x28)));   // 6da161: fld dword ptr [ebp + 0x28]
    fp.Fchs();   // 6da164: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da166: fstp dword ptr [esp + 0x18]
L_6da16a:;
    edx = (uint32)(esp + 0x50);   // 6da16a: lea edx, [esp + 0x50]
    esp -= 4; M32(esp) = edx;   // 6da16e: push edx
    eax = (uint32)(esp + 0x210);   // 6da16f: lea eax, [esp + 0x210]
    esp -= 4; M32(esp) = ebx;   // 6da176: push ebx
    esp -= 4; M32(esp) = eax;   // 6da177: push eax
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da178: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da17d: fld dword ptr [esp + 0x24]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebp + 0x2c)));   // ST0 = ST0 * m   // 6da181: fmul dword ptr [ebp + 0x2c]
    esp += 0xCu;   // 6da184: add esp, 0xc
    fA = ebx; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; ebx = fR;   // 6da187: add ebx, 0x20
    esp -= 4; M32(esp) = ebx;   // 6da18a: push ebx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da18b: fmul dword ptr [0xb7cb5c]
    esp -= 4; M32(esp) = ecx;   // 6da191: push ecx
    ecx = (uint32)(esp + 0x22c);   // 6da192: lea ecx, [esp + 0x22c]
    edi = eax;   // 6da199: mov edi, eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da19b: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6da1a1: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6da1a4: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da1a5: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi));   // 6da1aa: mov ecx, dword ptr [edi]
    edx = esp;   // 6da1ac: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da1ae: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 4));   // 6da1b0: mov ecx, dword ptr [edi + 4]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da1b3: mov dword ptr [edx + 4], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 8));   // 6da1b6: mov ecx, dword ptr [edi + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(ecx);   // 6da1b9: mov dword ptr [edx + 8], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da1bc: mov ecx, dword ptr [eax]
    esp -= 0xCu;   // 6da1be: sub esp, 0xc
    edx = esp;   // 6da1c1: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da1c3: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da1c5: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da1c8: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da1cb: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da1ce: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da1d0: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da1d3: call 0x542a50
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da1d8: mov ebx, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 0x18)));   // 6da1db: fld dword ptr [ebx + 0x18]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da1de: mov ebp, dword ptr [esi + 0x388]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6da1e4: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da1ea: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6da1ec: test ah, 5
    if (FlcParityEven(fR)) goto L_6da1fa;   // 6da1ef: jp 0x6da1fa
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebp + 0x28));   // 6da1f1: mov ecx, dword ptr [ebp + 0x28]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6da1f4: mov dword ptr [esp + 0x18], ecx
    goto L_6da203;   // 6da1f8: jmp 0x6da203
L_6da1fa:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebp + 0x28)));   // 6da1fa: fld dword ptr [ebp + 0x28]
    fp.Fchs();   // 6da1fd: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da1ff: fstp dword ptr [esp + 0x18]
L_6da203:;
    edx = (uint32)(esp + 0x50);   // 6da203: lea edx, [esp + 0x50]
    esp -= 4; M32(esp) = edx;   // 6da207: push edx
    eax = (uint32)(ebx + 0x10);   // 6da208: lea eax, [ebx + 0x10]
    esp -= 4; M32(esp) = eax;   // 6da20b: push eax
    ecx = (uint32)(esp + 0x244);   // 6da20c: lea ecx, [esp + 0x244]
    esp -= 4; M32(esp) = ecx;   // 6da213: push ecx
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da214: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da219: fld dword ptr [esp + 0x24]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebp + 0x2c)));   // ST0 = ST0 * m   // 6da21d: fmul dword ptr [ebp + 0x2c]
    esp += 0xCu;   // 6da220: add esp, 0xc
    fA = ebx; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; ebx = fR;   // 6da223: add ebx, 0x20
    esp -= 4; M32(esp) = ebx;   // 6da226: push ebx
    edx = (uint32)(esp + 0x84);   // 6da227: lea edx, [esp + 0x84]
L_6da22e:;
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da22e: fmul dword ptr [0xb7cb5c]
    esp -= 4; M32(esp) = ecx;   // 6da234: push ecx
    edi = eax;   // 6da235: mov edi, eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da237: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6da23d: fstp dword ptr [esp]
    esp -= 4; M32(esp) = edx;   // 6da240: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da241: call 0x40fe90
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi));   // 6da246: mov edx, dword ptr [edi]
    ecx = esp;   // 6da248: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da24a: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 4));   // 6da24c: mov edx, dword ptr [edi + 4]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6da24f: mov dword ptr [ecx + 4], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 8));   // 6da252: mov edx, dword ptr [edi + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6da255: mov dword ptr [ecx + 8], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da258: mov edx, dword ptr [eax]
    esp -= 0xCu;   // 6da25a: sub esp, 0xc
    ecx = esp;   // 6da25d: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da25f: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da261: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da264: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6da267: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6da26a: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6da26d: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da26f: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da274: fld dword ptr [esp + 0x26c]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da27b: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFFFF00u) | (((uint32)(*(uint8*)(uintptr_t)(uint32)(0xc1c970))) & 0xFFu);   // 6da281: mov al, byte ptr [0xc1c970]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x14));   // 6da286: mov edi, dword ptr [esp + 0x14]
    fA = (eax & 0xFFu); fB = (eax & 0xFFu); fR = (fA & fB) & 0xFFu;   // 6da28a: test al, al
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da28c: fnstsw ax
    if (fR != 0) goto L_6da3a1;   // 6da28e: jne 0x6da3a1
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da294: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da2c7;   // 6da297: jp 0x6da2c7
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da299: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x26c) = (uint32)(0x0u);   // 6da29b: mov dword ptr [esp + 0x26c], 0
    if (fR == 0) goto L_6da2c7;   // 6da2a6: je 0x6da2c7
    ecx = edi;   // 6da2a8: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringUpDown();   // 6da2aa: call 0x53fbd0
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da2af: movsx ecx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6da2b2: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da2b6: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da2ba: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da2c0: fstp dword ptr [esp + 0x26c]
L_6da2c7:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da2c7: fld dword ptr [esp + 0x270]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da2ce: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da2d4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da2d6: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da30b;   // 6da2d9: jp 0x6da30b
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da2db: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x270) = (uint32)(0x0u);   // 6da2dd: mov dword ptr [esp + 0x270], 0
    if (fR == 0) goto L_6da30b;   // 6da2e8: je 0x6da30b
    ecx = edi;   // 6da2ea: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringLeftRight();   // 6da2ec: call 0x53fb80
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da2f1: movsx edx, ax
    fR = (uint32)(0 - edx); edx = fR;   // 6da2f4: neg edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da2f6: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da2fa: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da2fe: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da304: fstp dword ptr [esp + 0x270]
L_6da30b:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da30b: fld dword ptr [esp + 0x268]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da312: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da318: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da31a: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da468;   // 6da31d: jp 0x6da468
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da323: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0x0u);   // 6da325: mov dword ptr [esp + 0x268], 0
    if (fR == 0) goto L_6da468;   // 6da330: je 0x6da468
    ecx = edi;   // 6da336: mov ecx, edi
    eax = ((CPad*)(uintptr_t)ecx)->GetLookRight() ? 1u : 0u;   // 6da338: call 0x53fe10
    eax = (eax & 0xFFu);   // 6da33d: movzx eax, al
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6da340: mov dword ptr [esp + 0x14], eax
    ecx = edi;   // 6da344: mov ecx, edi
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da346: fild dword ptr [esp + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da34a: fstp dword ptr [esp + 0x268]
    eax = ((CPad*)(uintptr_t)ecx)->GetLookLeft() ? 1u : 0u;   // 6da351: call 0x53fdd0
    fA = (eax & 0xFFu); fB = (eax & 0xFFu); fR = (fA & fB) & 0xFFu;   // 6da356: test al, al
    if (fR == 0) goto L_6da365;   // 6da358: je 0x6da365
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0xBF800000u);   // 6da35a: mov dword ptr [esp + 0x268], 0xbf800000
L_6da365:;
    ecx = edi;   // 6da365: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunLeftRight();   // 6da367: call 0x53fc50
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da36c: movsx ecx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6da36f: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da373: fild dword ptr [esp + 0x14]
    fp.Fabs();   // 6da377: fabs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6da379: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da37f: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da381: test ah, 0x41
    if (fR != 0) goto L_6da468;   // 6da384: jne 0x6da468
    ecx = edi;   // 6da38a: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunLeftRight();   // 6da38c: call 0x53fc50
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da391: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da394: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da398: fild dword ptr [esp + 0x14]
    goto L_6da45b;   // 6da39c: jmp 0x6da45b
L_6da3a1:;
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da3a1: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da3d4;   // 6da3a4: jp 0x6da3d4
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da3a6: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x26c) = (uint32)(0x0u);   // 6da3a8: mov dword ptr [esp + 0x26c], 0
    if (fR == 0) goto L_6da3d4;   // 6da3b3: je 0x6da3d4
    ecx = edi;   // 6da3b5: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringUpDown();   // 6da3b7: call 0x53fbd0
    eax = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da3bc: movsx eax, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6da3bf: mov dword ptr [esp + 0x14], eax
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da3c3: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da3c7: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da3cd: fstp dword ptr [esp + 0x26c]
L_6da3d4:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da3d4: fld dword ptr [esp + 0x270]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da3db: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da3e1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da3e3: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da410;   // 6da3e6: jp 0x6da410
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da3e8: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x270) = (uint32)(0x0u);   // 6da3ea: mov dword ptr [esp + 0x270], 0
    if (fR == 0) goto L_6da410;   // 6da3f5: je 0x6da410
    ecx = edi;   // 6da3f7: mov ecx, edi
    eax = ((CPad*)(uintptr_t)ecx)->GetLookLeft() ? 1u : 0u;   // 6da3f9: call 0x53fdd0
    ecx = (eax & 0xFFu);   // 6da3fe: movzx ecx, al
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6da401: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da405: fild dword ptr [esp + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da409: fstp dword ptr [esp + 0x270]
L_6da410:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da410: fld dword ptr [esp + 0x268]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da417: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da41d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da41f: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da468;   // 6da422: jp 0x6da468
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da424: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0x0u);   // 6da426: mov dword ptr [esp + 0x268], 0
    if (fR == 0) goto L_6da468;   // 6da431: je 0x6da468
    ecx = edi;   // 6da433: mov ecx, edi
    eax = ((CPad*)(uintptr_t)ecx)->GetLookRight() ? 1u : 0u;   // 6da435: call 0x53fe10
    fA = (eax & 0xFFu); fB = (eax & 0xFFu); fR = (fA & fB) & 0xFFu;   // 6da43a: test al, al
    if (fR == 0) goto L_6da449;   // 6da43c: je 0x6da449
    *(uint32*)(uintptr_t)(uint32)(esp + 0x270) = (uint32)(0xBF800000u);   // 6da43e: mov dword ptr [esp + 0x270], 0xbf800000
L_6da449:;
    ecx = edi;   // 6da449: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringLeftRight();   // 6da44b: call 0x53fb80
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da450: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da453: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da457: fild dword ptr [esp + 0x14]
L_6da45b:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da45b: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da461: fstp dword ptr [esp + 0x268]
L_6da468:;
    fA = (uint32)(*(uint8*)(uintptr_t)(uint32)(esi + 0x42b)); fB = 0x10u; fR = (fA & fB) & 0xFFu;   // 6da468: test byte ptr [esi + 0x42b], 0x10
    if (fR == 0) goto L_6da4ae;   // 6da46f: je 0x6da4ae
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da471: mov edi, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // 6da477: fld dword ptr [edi + 0x20]
    fp.Fdiv((*(float*)(uintptr_t)(uint32)(edi + 0x2c)));   // ST0 = ST0 / m   // 6da47a: fdiv dword ptr [edi + 0x2c]
    FlcCrtAsin(fp);   // 6da47d: call 0x821e70
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c84)));   // ST0 = ST0 * m   // 6da482: fmul dword ptr [0x858c84]
    fp.Fsin();   // 6da488: fsin 
    fp.Fdiv((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 / m   // 6da48a: fdiv dword ptr [edi + 0x20]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x2c)));   // ST0 = ST0 * m   // 6da48d: fmul dword ptr [edi + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da490: fld dword ptr [esp + 0x26c]
    fp.Arith('*', 0, 0, 1, false);   // 6da497: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da499: fstp dword ptr [esp + 0x26c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // ST0 = ST0 * m   // 6da4a0: fmul dword ptr [esp + 0x270]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da4a7: fstp dword ptr [esp + 0x270]
L_6da4ae:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da4ae: mov eax, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da4b1: fld dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 + m   // 6da4b5: fadd dword ptr [eax + 0x10]
    esp -= 0xCu;   // 6da4b8: sub esp, 0xc
    edx = esp;   // 6da4bb: mov edx, esp
    esp -= 0xCu;   // 6da4bd: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6da4c0: fstp dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6da4c4: fld dword ptr [esp + 0x6c]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 0x14)));   // ST0 = ST0 + m   // 6da4c8: fadd dword ptr [eax + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da4cb: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6da4cf: fld dword ptr [esp + 0x70]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 + m   // 6da4d3: fadd dword ptr [eax + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da4d6: fstp dword ptr [esp + 0x54]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // 6da4da: fld dword ptr [esp + 0x284]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x20)));   // ST0 = ST0 * m   // 6da4e1: fmul dword ptr [eax + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // 6da4e4: fld dword ptr [esp + 0x284]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x24)));   // ST0 = ST0 * m   // 6da4eb: fmul dword ptr [eax + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // 6da4ee: fld dword ptr [esp + 0x284]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x28)));   // ST0 = ST0 * m   // 6da4f5: fmul dword ptr [eax + 0x28]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da4f8: mov eax, dword ptr [esi + 0x388]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 0x20));   // 6da4fe: mov ecx, dword ptr [eax + 0x20]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(ecx);   // 6da501: mov dword ptr [esp + 0x2c], ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da505: fstp dword ptr [esp + 0x7c]
    fp.Fxch(1);   // 6da509: fxch st(1)
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6da50b: mov eax, dword ptr [esp + 0x4c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da50f: fmul dword ptr [esp + 0x2c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6da513: mov ecx, dword ptr [esp + 0x50]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da517: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6da519: mov eax, dword ptr [esp + 0x54]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da51d: fstp dword ptr [esp + 0x34]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da521: mov dword ptr [edx + 4], ecx
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da524: mov dword ptr [edx + 8], eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da527: fmul dword ptr [esp + 0x2c]
    ecx = esp;   // 6da52b: mov ecx, esp
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da52d: fld dword ptr [esp + 0x7c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da531: fmul dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // 6da535: fld dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da53b: fld dword ptr [esp + 0x34]
    fp.Arith('*', 0, 0, 1, false);   // 6da53f: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da541: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6da545: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6da547: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da549: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6da54d: fmul st(1)
    fp.Fxch(1);   // 6da54f: fxch st(1)
    fp.FstpSt(0);   // 6da551: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da553: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da557: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da55d: fstp dword ptr [esp + 0x34]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da561: mov edx, dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da565: fld dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da569: mov dword ptr [ecx], edx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da56b: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da571: fstp dword ptr [esp + 0x38]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da575: mov eax, dword ptr [esp + 0x38]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da579: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6da57f: mov dword ptr [ecx + 4], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da582: fstp dword ptr [esp + 0x3c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da586: mov edx, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6da58a: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6da58d: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da58f: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da594: fld dword ptr [esp + 0x50]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da598: mov eax, dword ptr [esi + 0x14]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 + m   // 6da59b: fadd dword ptr [eax]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da59d: fstp dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da5a1: fld dword ptr [esp + 0x54]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 + m   // 6da5a5: fadd dword ptr [eax + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da5a8: fstp dword ptr [esp + 0x38]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x58)));   // 6da5ac: fld dword ptr [esp + 0x58]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 + m   // 6da5b0: fadd dword ptr [eax + 8]
    esp -= 0xCu;   // 6da5b3: sub esp, 0xc
    edx = esp;   // 6da5b6: mov edx, esp
    esp -= 0xCu;   // 6da5b8: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da5bb: fstp dword ptr [esp + 0x54]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x288)));   // 6da5bf: fld dword ptr [esp + 0x288]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x20)));   // ST0 = ST0 * m   // 6da5c6: fmul dword ptr [eax + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x288)));   // 6da5c9: fld dword ptr [esp + 0x288]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x24)));   // ST0 = ST0 * m   // 6da5d0: fmul dword ptr [eax + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x288)));   // 6da5d3: fld dword ptr [esp + 0x288]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x28)));   // ST0 = ST0 * m   // 6da5da: fmul dword ptr [eax + 0x28]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da5dd: mov eax, dword ptr [esi + 0x388]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 0x18));   // 6da5e3: mov ecx, dword ptr [eax + 0x18]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(ecx);   // 6da5e6: mov dword ptr [esp + 0x2c], ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da5ea: fstp dword ptr [esp + 0x7c]
    fp.Fxch(1);   // 6da5ee: fxch st(1)
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6da5f0: mov eax, dword ptr [esp + 0x4c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da5f4: fmul dword ptr [esp + 0x2c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6da5f8: mov ecx, dword ptr [esp + 0x50]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da5fc: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6da5fe: mov eax, dword ptr [esp + 0x54]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da602: fstp dword ptr [esp + 0x34]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da606: mov dword ptr [edx + 4], ecx
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da609: mov dword ptr [edx + 8], eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da60c: fmul dword ptr [esp + 0x2c]
    ecx = esp;   // 6da610: mov ecx, esp
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da612: fld dword ptr [esp + 0x7c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da616: fmul dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // 6da61a: fld dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da620: fld dword ptr [esp + 0x34]
    fp.Arith('*', 0, 0, 1, false);   // 6da624: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da626: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6da62a: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6da62c: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da62e: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6da632: fmul st(1)
    fp.Fxch(1);   // 6da634: fxch st(1)
    fp.FstpSt(0);   // 6da636: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da638: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da63c: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da642: fstp dword ptr [esp + 0x34]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da646: mov edx, dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da64a: fld dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da64e: mov dword ptr [ecx], edx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da650: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da656: fstp dword ptr [esp + 0x38]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da65a: mov eax, dword ptr [esp + 0x38]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da65e: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6da664: mov dword ptr [ecx + 4], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da667: fstp dword ptr [esp + 0x3c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da66b: mov edx, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6da66f: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6da672: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da674: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6da679: fld dword ptr [esp + 0x48]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da67d: mov eax, dword ptr [esi + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 * m   // 6da680: fmul dword ptr [eax + 8]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da683: mov ecx, dword ptr [eax]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da685: fld dword ptr [esp + 0x44]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da689: mov edx, dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6da68c: fmul dword ptr [eax + 4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(ecx);   // 6da68f: mov dword ptr [esp + 0x1c], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(edx);   // 6da693: mov dword ptr [esp + 0x20], edx
    fp.Arith('+', 1, 1, 0, true);   // 6da697: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da699: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6da69d: fmul dword ptr [eax]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da69f: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(eax);   // 6da6a2: mov dword ptr [esp + 0x24], eax
    fp.Arith('+', 1, 1, 0, true);   // 6da6a6: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6da6a8: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da6ae: fstp dword ptr [esp + 0x18]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da6b2: fld dword ptr [esp + 0x18]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da6b6: mov ecx, dword ptr [esi + 0x388]
    fp.Fabs();   // 6da6bc: fabs 
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6da6be: mov edx, dword ptr [esi + 0x8c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6da6c4: fstp dword ptr [esp + 0x10]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da6c8: mov dword ptr [esp + 0x14], edx
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6da6cc: fld dword ptr [esp + 0x10]
    esp -= 0xCu;   // 6da6d0: sub esp, 0xc
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x14)));   // ST0 = ST0 * m   // 6da6d3: fmul dword ptr [ecx + 0x14]
    eax = esp;   // 6da6d6: mov eax, esp
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // ST0 = ST0 * m   // 6da6d8: fmul dword ptr [esp + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da6dc: fld dword ptr [esp + 0x28]
    fp.Arith('*', 0, 0, 1, false);   // 6da6e0: fmul st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da6e2: fld dword ptr [esp + 0x2c]
    fp.Arith('*', 0, 0, 2, false);   // 6da6e6: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da6e8: fstp dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da6ec: fld dword ptr [esp + 0x30]
    fp.Arith('*', 0, 0, 2, false);   // 6da6f0: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da6f2: fstp dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6da6f6: fmul dword ptr [esp + 0x20]
    fp.FstpSt(1);   // 6da6fa: fstp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da6fc: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6da700: fmul dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da704: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6da708: fmul dword ptr [esp + 0x20]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6da70c: fstp dword ptr [esp + 0x70]
    fp.Fxch(1);   // 6da710: fxch st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da712: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da718: fstp dword ptr [esp + 0x40]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6da71c: mov ecx, dword ptr [esp + 0x40]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6da720: mov dword ptr [eax], ecx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da722: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da728: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6da72c: fld dword ptr [esp + 0x70]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6da730: mov edx, dword ptr [esp + 0x44]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da734: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6da73a: mov dword ptr [eax + 4], edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6da73d: fstp dword ptr [esp + 0x48]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6da741: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6da745: mov dword ptr [eax + 8], ecx
    ecx = esi;   // 6da748: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6da74a: call 0x5429f0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6da74f: fld dword ptr [esp + 0x10]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da753: mov eax, dword ptr [esi + 0x14]
    edx = eax;   // 6da756: mov edx, eax
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx));   // 6da758: mov ecx, dword ptr [edx]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(ecx);   // 6da75a: mov dword ptr [esp + 0x1c], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 4));   // 6da75e: mov ecx, dword ptr [edx + 4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(ecx);   // 6da761: mov dword ptr [esp + 0x20], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da765: mov ecx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x10)));   // ST0 = ST0 * m   // 6da76b: fmul dword ptr [ecx + 0x10]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 8));   // 6da76e: mov edx, dword ptr [edx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(edx);   // 6da771: mov dword ptr [esp + 0x24], edx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 * m   // 6da775: fmul dword ptr [esp + 0x18]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da779: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0xc)));   // ST0 = ST0 * m   // 6da780: fmul dword ptr [ecx + 0xc]
    fp.Arith('+', 1, 1, 0, true);   // 6da783: faddp st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da785: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da78b: fmul dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da791: fld dword ptr [esp + 0x1c]
    fp.Arith('*', 0, 0, 1, false);   // 6da795: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da797: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6da79b: fld dword ptr [esp + 0x20]
    fp.Arith('*', 0, 0, 1, false);   // 6da79f: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6da7a1: fstp dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da7a5: fld dword ptr [esp + 0x24]
    fp.Arith('*', 0, 0, 1, false);   // 6da7a9: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da7ab: fstp dword ptr [esp + 0x24]
    fp.FstpSt(0);   // 6da7af: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // 6da7b1: fld dword ptr [eax + 0x10]
    fp.Fchs();   // 6da7b4: fchs 
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 0x14)));   // 6da7b6: fld dword ptr [eax + 0x14]
    fp.Fchs();   // 6da7b9: fchs 
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // 6da7bb: fld dword ptr [eax + 0x18]
    fp.Fchs();   // 6da7be: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x64)));   // 6da7c0: fstp dword ptr [esp + 0x64]
    fp.Fxch(1);   // 6da7c4: fxch st(1)
    esp -= 0xCu;   // 6da7c6: sub esp, 0xc
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x5c)));   // ST0 = ST0 + m   // 6da7c9: fadd dword ptr [esp + 0x5c]
    eax = esp;   // 6da7cd: mov eax, esp
    esp -= 0xCu;   // 6da7cf: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6da7d2: fstp dword ptr [esp + 0x4c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6da7d6: mov ecx, dword ptr [esp + 0x4c]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6da7da: mov dword ptr [eax], ecx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // ST0 = ST0 + m   // 6da7dc: fadd dword ptr [esp + 0x6c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da7e0: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da7e4: fld dword ptr [esp + 0x7c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6da7e8: mov edx, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // ST0 = ST0 + m   // 6da7ec: fadd dword ptr [esp + 0x70]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6da7f0: mov dword ptr [eax + 4], edx
    edx = esp;   // 6da7f3: mov edx, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da7f5: fstp dword ptr [esp + 0x54]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6da7f9: mov ecx, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6da7fd: mov dword ptr [eax + 8], ecx
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da800: mov eax, dword ptr [esp + 0x34]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da804: mov ecx, dword ptr [esp + 0x38]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da808: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da80a: mov eax, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da80e: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da811: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da813: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da816: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da81b: fld dword ptr [esp + 0x50]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da81f: mov eax, dword ptr [esi + 0x14]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 + m   // 6da822: fadd dword ptr [eax]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da824: mov ecx, dword ptr [esi + 0x388]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 0xc));   // 6da82a: mov edx, dword ptr [ecx + 0xc]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da82d: mov dword ptr [esp + 0x14], edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da831: fstp dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da835: fld dword ptr [esp + 0x54]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 + m   // 6da839: fadd dword ptr [eax + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da83c: fstp dword ptr [esp + 0x38]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x58)));   // 6da840: fld dword ptr [esp + 0x58]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 + m   // 6da844: fadd dword ptr [eax + 8]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da847: fstp dword ptr [esp + 0x3c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da84b: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 * m   // 6da852: fmul dword ptr [eax + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da855: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x14)));   // ST0 = ST0 * m   // 6da85c: fmul dword ptr [eax + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da85f: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 * m   // 6da866: fmul dword ptr [eax + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x64)));   // 6da869: fstp dword ptr [esp + 0x64]
    fp.Fxch(1);   // 6da86d: fxch st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6da86f: fmul dword ptr [esp + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da873: fstp dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6da877: fmul dword ptr [esp + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x64)));   // 6da87b: fld dword ptr [esp + 0x64]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6da87f: fmul dword ptr [esp + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // 6da883: fld dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da889: fld dword ptr [esp + 0x1c]
    fp.Arith('*', 0, 0, 1, false);   // 6da88d: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da88f: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6da893: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6da895: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da897: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6da89b: fmul st(1)
    fp.Fxch(1);   // 6da89d: fxch st(1)
    fp.FstpSt(0);   // 6da89f: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da8a1: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da8a5: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da8ab: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da8af: fld dword ptr [esp + 0x44]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da8b3: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6da8b9: fstp dword ptr [esp + 0x20]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da8bd: mov ecx, dword ptr [esp + 0x34]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da8c1: fmul dword ptr [0xb7cb5c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da8c7: mov edx, dword ptr [esp + 0x38]
    esp -= 0xCu;   // 6da8cb: sub esp, 0xc
    eax = esp;   // 6da8ce: mov eax, esp
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6da8d0: mov dword ptr [eax], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6da8d2: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6da8d6: mov dword ptr [eax + 4], edx
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6da8d9: mov dword ptr [eax + 8], ecx
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x28));   // 6da8dc: mov eax, dword ptr [esp + 0x28]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x2c));   // 6da8e0: mov ecx, dword ptr [esp + 0x2c]
    esp -= 0xCu;   // 6da8e4: sub esp, 0xc
    edx = esp;   // 6da8e7: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da8e9: mov dword ptr [edx], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da8eb: fstp dword ptr [esp + 0x3c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da8ef: mov eax, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da8f3: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da8f6: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da8f8: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da8fb: call 0x542a50
}
}  // namespace
#undef M32

static_assert(offsetof(CVehicle, m_pFlyingHandlingData) == 0x388 && offsetof(CVehicle, m_pDriver) == 0x460 && offsetof(CVehicle, m_nCreatedBy) == 0x4A4 &&
              offsetof(CVehicle, m_vecMoveSpeed) == 0x44 && offsetof(CVehicle, m_vecTurnSpeed) == 0x50 && offsetof(CVehicle, m_fMass) == 0x8C && offsetof(CVehicle, m_fTurnMass) == 0x90 &&
              offsetof(CVehicle, m_vecCentreOfMass) == 0xA4 && offsetof(CVehicle, m_matrix) == 0x14,
              "FlyingControl lee estos desplazamientos del EXE");

// 0x6D85F0
void CVehicle::FlyingControl(eFlightModel flightModel, float leftRightSkid, float steeringUpDown, float steeringLeftRight, float accelerationBreakStatus) {
    FlcFrame f{ this, FlcX87{}, m_pFlyingHandlingData, flightModel, leftRightSkid, steeringUpDown, steeringLeftRight, accelerationBreakStatus, nullptr, {}, {} };
    FlcX87& fp = f.fp;
    if (!f.fh) { fp.Commit(); return; }                            // 0x6D8604: sin datos de vuelo
    fp.Fld(CTimer::ms_fTimeStep); fp.FcomMem(kFlcZero, true);
    if (!FlcParityEven(fp.Ah() & 0x41)) { fp.Commit(); return; }   // 0x6D861B: jnp, sale con tiempo <= 0
    // 0x6D8621: el mando solo se lee con el vehiculo del jugador y un conductor jugador
    if (GetStatus() == STATUS_PLAYER && m_pDriver && m_pDriver->IsPlayer())
        f.pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    // 0x6D8652: velocidad + deriva del viento (un vehiculo de mision no la tiene)
    float wind[3] = { 0.f, 0.f, 0.f };
    f.vel[1] = 0.f; f.vel[2] = 0.f;
    fp.Fld(kFlcZero);
    if (m_nCreatedBy != 2) {
        fp.Pop();
        fp.Fld(f.fh->m_fWindMult);
        fp.Fld(CWeather::WindDir.x); fp.Arith('*', 0, 0, 1, false); fp.Fstp(wind[0]);
        fp.Fld(CWeather::WindDir.y); fp.Arith('*', 0, 0, 1, false); fp.Fstp(wind[1]);
        fp.Fmul(CWeather::WindDir.z); fp.Fstp(wind[2]);
        fp.Fld(wind[0]); fp.Fchs();
        fp.Fld(wind[1]); fp.Fchs(); fp.Fstp(f.vel[1]);
        fp.Fld(wind[2]); fp.Fchs(); fp.Fstp(f.vel[2]);
    }
    fp.Fadd(m_vecMoveSpeed.x); fp.Fstp(f.vel[0]);
    fp.Fld(f.vel[1]); fp.Fadd(m_vecMoveSpeed.y); fp.Fstp(f.vel[1]);
    fp.Fld(f.vel[2]); fp.Fadd(m_vecMoveSpeed.z); fp.Fstp(f.vel[2]);
    FlcMatMul3x3(fp, f.com, reinterpret_cast<const float*>(m_matrix), &m_vecCentreOfMass.x);   // 0x59C790
    switch (flightModel) {
    case FLIGHT_MODEL_UNK: FlcRouteAirship(f); break;
    case FLIGHT_MODEL_BARON: case FLIGHT_MODEL_PLANE: case FLIGHT_MODEL_UNK2: case FLIGHT_MODEL_BOAT: FlcRouteWing(f); break;
    case FLIGHT_MODEL_RC: case FLIGHT_MODEL_HELI: case eFlightModel(8): FlcRouteRotor(f); break;
    default: break;
    }
    FlcTail(f);
    fp.Commit();
}

// 0x6DAF00
// always returns `false`, and `rotorType` is always `-3`
template<typename PtrListType>
bool CVehicle::BladeColSectorList(PtrListType& ptrList, CColModel& colModel, CMatrix& matrix, int16 rotorType, float damageMult) {
    // Como el original: el grosor del rotor es 0x871D9C (0.2); la direccion y el grosor (un solo eje) salen del tipo de rotor
    // (-3, -2, -1, 1, 2, 3 = abajo, atras, izquierda, derecha, adelante, arriba). Con otro valor (0 incluido) el original deja
    // la direccion sin inicializar; aqui queda a cero.
    constexpr auto ROTOR_THICKNESS = 0.2f;
    CVector rotorUp{};
    CVector rotorSizeOS{};
    switch (rotorType) {
    case -3: rotorUp = CVector{ -matrix.GetUp().x, -matrix.GetUp().y, -matrix.GetUp().z };                rotorSizeOS.z = -ROTOR_THICKNESS; break; // down
    case -2: rotorUp = CVector{ -matrix.GetForward().x, -matrix.GetForward().y, -matrix.GetForward().z }; rotorSizeOS.y = -ROTOR_THICKNESS; break; // backwards
    case -1: rotorUp = CVector{ -matrix.GetRight().x, -matrix.GetRight().y, -matrix.GetRight().z };       rotorSizeOS.x = -ROTOR_THICKNESS; break; // left
    case  1: rotorUp = matrix.GetRight();                                                                 rotorSizeOS.x =  ROTOR_THICKNESS; break; // right
    case  2: rotorUp = matrix.GetForward();                                                               rotorSizeOS.y =  ROTOR_THICKNESS; break; // forward
    case  3: rotorUp = matrix.GetUp();                                                                    rotorSizeOS.z =  ROTOR_THICKNESS; break; // up
    default: break;
    }

    // Como el original: Multiply3x3 (0x59C790) y matriz * punto (0x59C890) con el orden de sumas del EXE
    const auto rotorSize      = VehEMatVec(matrix, rotorSizeOS);
    const auto colModelCenter = VehEMatPoint(matrix, colModel.GetBoundCenter());

    if (ptrList.IsEmpty()) {
        return false;
    }

    for (auto* entity : ptrList) {
        if (static_cast<CEntity*>(entity) == static_cast<CEntity*>(this) || !entity->m_bUsesCollision) {
            continue;
        }

        if (entity->IsScanCodeCurrent()) {
            continue;
        }
        entity->SetCurrentScanCode();

        // Como el original: el clump se pasa tal cual (sin la comprobacion de tipo de GetRpClump)
        auto entityCM = entity->GetIsTypePed()
            ? entity->GetModelInfo()->AsPedModelInfoPtr()->AnimatePedColModelSkinned(reinterpret_cast<RpClump*>(entity->GetRwObject()))
            : entity->GetColModel();

        if (entity->GetIsTypeObject() && entity->AsObject()->m_nObjectType == eObjectType::OBJECT_TEMPORARY) {
            continue;
        }

        if (!entityCM) {
            continue;
        }

        const auto numColls = CCollision::ProcessColModels(
            matrix, colModel,
            entity->GetMatrix(), *entityCM,
            CWorld::m_aTempColPts,
            nullptr,
            nullptr,
            false
        );
        if (numColls <= 0) {
            continue;
        }

        if (entity->GetIsTypePed()) { // 0x6DB207
            auto& ped = *entity->AsPed();

            // Como el original: direccion 2D (este vehiculo - peaton) normalizada con CVector2D::Normalise (0x44E480)
            CVector2D dirToPed{ GetPosition().x - ped.GetPosition().x, GetPosition().y - ped.GetPosition().y };
            VehENormalise2D(dirToPed);
            const auto localDir = ped.GetLocalDirection(dirToPed);

            if (!ped.m_pAttachedTo) { // 0x6DB24C
                // Como el original: se quita bIsStanding (bit 0 de las banderas del peaton) antes de empujarlo
                ped.bIsStanding = false;
                ped.ApplyMoveForce(CVector{ dirToPed.x * -5.f, dirToPed.y * -5.f, 5.f });
            }

            // Como el original: el calculador de respuesta se construye antes que el evento
            CPedDamageResponseCalculator dmgCalculator{ this, 1000.f, WEAPON_RUNOVERBYCAR, PED_PIECE_TORSO, false };
            CEventDamage dmgEvent{ // 0x6DB2CE
                this,
                CTimer::GetTimeInMS(),
                WEAPON_RUNOVERBYCAR,
                PED_PIECE_TORSO,
                static_cast<uint8>(localDir),
                false,
                false
            };
            dmgEvent.ComputeDamageResponseIfAffectsPed( // 0x6DB326
                &ped,
                dmgCalculator,
                true
            );
            ped.GetEventGroup().Add(&dmgEvent, false); // Como el original: se pasa el propio evento (no una copia)

            if (CLocalisation::Blood()) { // 0x6DB34D
                // Como el original: la sangre sale de la posicion del PEATON; la direccion es (x * 0.01, y * 0.01, 0.01) y el desplazamiento (x * 0.35, y * 0.35, 0.6)
                CVector bloodDir{ dirToPed.x * 0.01f, dirToPed.y * 0.01f, 0.01f };
                CVector bloodPos{ dirToPed.x * 0.35f + ped.GetPosition().x, dirToPed.y * 0.35f + ped.GetPosition().y, ped.GetPosition().z + 0.6f };
                if (ped.GetIsOnScreen()) {
                    g_fx.AddBlood(bloodPos, bloodDir, 16, ped.m_fContactSurfaceBrightness);
                }
            }
        } else if (entity->m_nModelIndex != eModelID::MODEL_MISSILE) { // 0x6DB44F
            bool  wasAnyCPValid{};
            float automobileCollisionDmgIntensity{};
            CVector cpOnRotor{}; //!< Col point on the rotor

            const auto prevElasticity = std::exchange(m_fElasticity, 1.f);

            for (auto i = 0; i < numColls; i++) { // 0x6DB474
                const auto& cp = CWorld::m_aTempColPts.data()[i];

                // Como el original: la copia del punto se reutiliza para el sonido posterior aunque el punto no sea valido
                cpOnRotor = cp.m_vecPoint;

                const CVector colDir{ cp.m_vecPoint.x - colModelCenter.x, cp.m_vecPoint.y - colModelCenter.y, cp.m_vecPoint.z - colModelCenter.z };
                // Como el original: el producto escalar con la direccion del rotor suma x, z, y
                const auto colDirOnRotorUp = (colDir.x * rotorUp.x + rotorUp.z * colDir.z) + rotorUp.y * colDir.y;

                // Como el original: el segundo producto escalar suma z, y, x
                if ( std::abs(colDirOnRotorUp) > ROTOR_SEMI_THICKNESS * 2.f
                  && std::abs(colDirOnRotorUp) > std::abs(VehEDot(colDir, cp.m_vecNormal)) * 0.3f
                ) {
                    continue;
                }

                wasAnyCPValid = true;

                const CVector rotorUpOnDir{ rotorUp.x * colDirOnRotorUp, rotorUp.y * colDirOnRotorUp, rotorUp.z * colDirOnRotorUp };
                cpOnRotor = CVector{ cpOnRotor.x - rotorUpOnDir.x, cpOnRotor.y - rotorUpOnDir.y, cpOnRotor.z - rotorUpOnDir.z };

                const CVector cpOnRotorDir{ cpOnRotor.x - colModelCenter.x, cpOnRotor.y - colModelCenter.y, cpOnRotor.z - colModelCenter.z };
                const auto cross = VehECross(rotorSize, cpOnRotorDir);
                auto colForceDir = CVector{ cross.x + m_vecMoveSpeed.x, cross.y + m_vecMoveSpeed.y, cross.z + m_vecMoveSpeed.z };

                // Como el original: NormaliseAndMag (0x59C970) normaliza colForceDir en el sitio y devuelve 1 / (1 / raiz)
                const auto sparkForce = VehENormaliseAndMag(colForceDir) * 15.f;
                g_fx.AddSparks(
                    cpOnRotor,
                    colForceDir,
                    sparkForce,
                    16,
                    CVector{},
                    eSparkType::SPARK_PARTICLE_SPARK,
                    0.2f,
                    1.f
                );

                if (IsAutomobile()) {
                    const auto au = AsAutomobile();
                    if (!(au->m_fHeliRotorSpeed > 0.15f)) {
                        if (au->m_fHeliRotorSpeed < 0.15f / 2.f && au->m_fHeliRotorSpeed > 0.f) {
                            au->m_fHeliRotorSpeed *= -1.f;
                        }
                    } else {
                        ApplySoftCollision(entity, cp, automobileCollisionDmgIntensity);
                        // Como el original: la fuerza de giro es el producto vectorial (sin la velocidad ni normalizar) por turnMass * -0.0005
                        const auto turnForceMult = -0.0005f * m_fTurnMass;
                        ApplyTurnForce(CVector{ cross.x * turnForceMult, cross.y * turnForceMult, cross.z * turnForceMult }, cpOnRotorDir);
                        au->m_fHeliRotorSpeed = 0.15f;
                    }
                }

                // Como el original: (100 * masa) * (1 / 3000) (0x865030), el mayor de los dos y por damageMult
                const auto dmgIntensity = (100.f * m_fMass) * std::bit_cast<float>(0x39AEC33Eu);
                SetDamagedPieceRecord(
                    (dmgIntensity > automobileCollisionDmgIntensity ? dmgIntensity : automobileCollisionDmgIntensity) * damageMult,
                    entity,
                    cp,
                    1.f
                );
            }

            // Como el original: el sonido se prueba cuando el momento no esta en [planeRotorDmgTimeMS - 2000, planeRotorDmgTimeMS]
            // (extremos incluidos); la rama de peaton de esta prueba nunca se cumple aqui porque los peatones salen antes
            if (wasAnyCPValid) {
                const auto timeMs = CTimer::GetTimeInMS();
                if (timeMs > planeRotorDmgTimeMS || timeMs < planeRotorDmgTimeMS - 2000) {
                    const auto ReportCollision = [&](const CVector& pos) {
                        AudioEngine.ReportCollision(
                            this,
                            entity,
                            SURFACE_CAR_PANEL,
                            SURFACE_CAR,
                            pos,
                            nullptr,
                            0.15f,
                            1.f,
                            false,
                            false
                        );
                    };
                    if (GetStatus() == STATUS_REMOTE_CONTROLLED) {
                        ReportCollision(cpOnRotor);
                    } else if (GetStatus() == STATUS_PLAYER) {
                        // Como el original: normaliza con CVector::Normalise (0x59C910); la camara se pide dos veces
                        const auto& firstCamPos = *TheCamera.GetGameCamPosition();
                        CVector dirToCP{ cpOnRotor.x - firstCamPos.x, cpOnRotor.y - firstCamPos.y, cpOnRotor.z - firstCamPos.z };
                        VehENormalise(dirToCP);
                        const CVector dirScaled{ dirToCP.x * 4.f, dirToCP.y * 4.f, dirToCP.z * 4.f };
                        const auto& gameCamPos = *TheCamera.GetGameCamPosition();
                        ReportCollision(CVector{ dirScaled.x + gameCamPos.x, dirScaled.y + gameCamPos.y, dirScaled.z + gameCamPos.z });
                    }
                    // Como el original: 0x407180 (CGeneral::GetRandomIntInRange), no la plantilla entera
                    planeRotorDmgTimeMS = CTimer::GetTimeInMS() + CGeneral::GetRandomIntInRange(150, 250);
                }
            }

            m_fElasticity = prevElasticity;
        }
    }

    return false;
}

// 0x6DBA30
void CVehicle::SetComponentRotation(RwFrame* component, eRotationAxis axis, float angle, bool bSetRotate) {
    if (!component) {
        return;
    }

    CMatrix mat{ RwFrameGetMatrix(component) };
    const CVector savedPos = mat.GetPosition();

    // Como el original: SetRotateX/Y/Z completos (ponen la posición a 0) o, sin bSetRotate, la posición se
    // anula sumando su opuesto y RotateX/Y/Z la rota; al final se le suma la posición guardada. Un eje
    // distinto de 0/1/2 no rota nada.
    if (bSetRotate) {
        switch (axis) {
        case AXIS_Z: mat.SetRotateZ(angle); break;
        case AXIS_Y: mat.SetRotateY(angle); break;
        case AXIS_X: mat.SetRotateX(angle); break;
        default:     break;
        }
    } else {
        const CVector negPos = -savedPos;
        mat.GetPosition().x = mat.GetPosition().x + negPos.x;
        mat.GetPosition().y = mat.GetPosition().y + negPos.y;
        mat.GetPosition().z = mat.GetPosition().z + negPos.z;
        switch (axis) {
        case AXIS_Z: mat.RotateZ(angle); break;
        case AXIS_Y: mat.RotateY(angle); break;
        case AXIS_X: mat.RotateX(angle); break;
        default:     break;
        }
    }
    mat.GetPosition().x = mat.GetPosition().x + savedPos.x;
    mat.GetPosition().y = mat.GetPosition().y + savedPos.y;
    mat.GetPosition().z = mat.GetPosition().z + savedPos.z;

    mat.UpdateRW();
}

// 0x6DBBB0
void CVehicle::SetTransmissionRotation(RwFrame* component, float angleL, float angleR, CVector wheelPos, bool isFront) {
    if (component) {
        CMatrix mat(&component->modelling);
        CVector savedPos = mat.GetPosition();
        // Como el original (0x6DBBFB..0x6DBC67): -fpatan((L + R) * 0.5 - z, y - wy) y fpatan(L - R, |x| + |x|) con ST0 sin redondear y un solo fstp float
        // cada uno; PI exacto del EXE (0x858CB8 = 0x40490FDB) sumado en x87. Los dos angulos se calculan antes de SetRotateX/RotateY.
        float angleX, angleY;
        VehicleTransmissionAnglesCpp(angleL, angleR, mat.GetPosition().y, &wheelPos.x, isFront, &angleX, &angleY);
        mat.SetRotateX(angleX);
        mat.RotateY(angleY);
        // Como el original (0x6DBC87..0x6DBCA7): fld pos; fadd saved; fstp por componente (x87: conserva subnormales y elige el NaN de mayor mantisa)
        mat.GetPosition().x = RpNanAddCpp(mat.GetPosition().x, savedPos.x);
        mat.GetPosition().y = RpNanAddCpp(mat.GetPosition().y, savedPos.y);
        mat.GetPosition().z = RpNanAddCpp(mat.GetPosition().z, savedPos.z);
        mat.UpdateRW();
    }
}

// 0x6DBCE0
void CVehicle::ProcessBoatControl(tBoatHandlingData* boatHandling, float* fLastWaterImmersionDepth, bool bCollidedWithWorld, bool bPostCollision) {
    // Como el original: todos los productos de vector por escalar y los productos escalares siguen el orden de operaciones del EXE
    const auto MulComp = [](const CVector& v, float s) { return CVector{ v.x * s, v.y * s, v.z * s }; };
    const auto timeStep = CTimer::GetTimeStep();

    CVector vecBuoyancyTurnPoint{};
    CVector vecBuoyancyForce{};
    auto* const boat = IsSubBoat() ? AsBoat() : nullptr;
    if (!mod_Buoyancy.ProcessBuoyancyBoat(this, m_fBuoyancyConstant, &vecBuoyancyTurnPoint, &vecBuoyancyForce, bCollidedWithWorld)) {
        physicalFlags.bSubmergedInWater = false;
        if (boat) {
            boat->m_nBoatFlags.bBoatInWater = false;
        }
        return;
    }

    bool bOnWater = false;
    // Como el original: 0x871FBC (el siguiente float tras 0.0008) y, con NaN, tampoco queda sumergido
    if (!(timeStep * m_fMass * std::bit_cast<float>(0x3A51B718u) < vecBuoyancyForce.z)) {
        physicalFlags.bSubmergedInWater = false;
        vehicleFlags.bIsDrowning        = false;
    } else {
        physicalFlags.bSubmergedInWater = true;
        bOnWater = true;

        // Como el original: bIsDrowning solo se limpia cuando NO se cumple la condicion de ahogo (si se cumple se queda a true)
        if (GetUp().z < -0.6F
            && std::fabs(m_vecMoveSpeed.x) < 0.05
            && std::fabs(m_vecMoveSpeed.y) < 0.05
        ) {
            vehicleFlags.bIsDrowning = true;
            if (m_pDriver) {
                m_pDriver->physicalFlags.bTouchingWater = true;
                if (m_pDriver->IsPlayer()) {
                    m_pDriver->AsPlayer()->HandlePlayerBreath(true, 1.0F);
                } else {
                    // Como el original: el calculador se construye siempre, antes que el evento y que AffectsPed
                    auto pedDamageResponseCalc = CPedDamageResponseCalculator(this, timeStep, eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, false);
                    auto damageEvent = CEventDamage(this, CTimer::GetTimeInMS(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, 0, false, true);
                    if (damageEvent.AffectsPed(m_pDriver)) {
                        pedDamageResponseCalc.ComputeDamageResponse(m_pDriver, damageEvent.m_damageResponse, true);
                    } else {
                        damageEvent.m_damageResponse.m_bDamageCalculated = true;
                    }

                    m_pDriver->GetEventGroup().Add(&damageEvent, false);
                }
            }
        } else {
            vehicleFlags.bIsDrowning = false;
        }
    }

    // 0x6DBF0A
    auto vecUsedBuoyancyForce = vecBuoyancyForce;
    const auto fImmersionDepth = mod_Buoyancy.m_fEntityWaterImmersion;
    if (m_nModelIndex == MODEL_SKIMMER
        && GetUp().z < -0.5F
        && std::fabs(m_vecMoveSpeed.x) < 0.2F
        && std::fabs(m_vecMoveSpeed.y) < 0.2F
    ) {
        vecUsedBuoyancyForce = MulComp(vecBuoyancyForce, 0.03F);
    }
    CPhysical::ApplyMoveForce(vecUsedBuoyancyForce);

    if (bCollidedWithWorld) {
        CPhysical::ApplyTurnForce(MulComp(vecBuoyancyForce, 0.4F), vecBuoyancyTurnPoint);
    }

    // 0x6DC00B
    if (m_nModelIndex == MODEL_SKIMMER) {
        // Como el original: los limites usan las constantes 0x871FB8 (8e-5, el siguiente float tras 8e-5) y 0.4 * 0.008, no divisiones entre 125
        const auto fCheckedMass = timeStep * m_fMass;
        if (m_f2ndSteerAngle != 0.0F
            || (GetForward().z < -0.5F
            && GetUp().z > -0.5F
            && m_vecMoveSpeed.z < -0.15F
            && std::bit_cast<float>(0x38A7C5ADu) * fCheckedMass < vecBuoyancyForce.z
            && vecBuoyancyForce.z < (0.4F * fCheckedMass) * 0.008F)
        ) {
            bOnWater = false;

            const auto fTurnForceMult = (((GetForward().z * m_fTurnMass) * timeStep) * -0.00017F) * vecBuoyancyForce.z;
            CPhysical::ApplyTurnForce(MulComp(GetForward(), fTurnForceMult), GetUp());

            const auto fMoveForceMult = (VehEDot(m_vecMoveSpeed, GetForward()) * -0.5F) * m_fMass;
            CPhysical::ApplyMoveForce(MulComp(GetForward(), fMoveForceMult));

            const auto fTimeMs = (float)CTimer::GetTimeInMS();
            if (m_f2ndSteerAngle == 0.0F) { // todo: missing checks for CTimer 0x6DC195 ?
                m_f2ndSteerAngle = fTimeMs + 300.0F;
            }
            else if (fTimeMs > m_f2ndSteerAngle) { // Como el original: estrictamente mayor
                m_f2ndSteerAngle = 0.0F;
            }
        }
    }

    // 0x6DC1E2
    if (!bPostCollision && bOnWater && GetUp().z > 0.0F) {
        auto fMoveForce = m_vecMoveSpeed.SquaredMagnitude() * boatHandling->m_fAqPlaneForce * timeStep * vecBuoyancyForce.z * 0.5F;
        if (m_nModelIndex == MODEL_SKIMMER)
            fMoveForce *= (m_GasPedal + 1.0F);
        else if (!(m_GasPedal > 0.05F))
            fMoveForce = 0.0F;
        else
            fMoveForce *= m_GasPedal;

        // Como el original: el limite multiplica por 0.008 (0x863984) en vez de dividir entre 125
        const auto fMaxMoveForce = ((timeStep * boatHandling->m_fAqPlaneLimit) * m_fMass) * 0.008F;
        fMoveForce = (fMaxMoveForce < fMoveForce) ? fMaxMoveForce : fMoveForce;

        const auto vecUsedMoveForce = MulComp(GetUp(), fMoveForce);
        CPhysical::ApplyMoveForce(vecUsedMoveForce);

        const auto vecOffset = MulComp(GetForward(), boatHandling->m_fAqPlaneOffset);
        const CVector vecTurnPoint{ vecBuoyancyTurnPoint.x - vecOffset.x, vecBuoyancyTurnPoint.y - vecOffset.y, vecBuoyancyTurnPoint.z - vecOffset.z };
        CPhysical::ApplyTurnForce(vecUsedMoveForce, vecTurnPoint);
    }

    CPad* pad = nullptr;
    if (GetStatus() == STATUS_PLAYER && m_pDriver && m_pDriver->IsPlayer()) {
        pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    }

    // 0x6DC3AF
    if (GetUp().z > -0.6F) {
        float fMoveSpeed = 1.0F;
        if (!(std::fabs(m_GasPedal) > 0.05F)) {
            fMoveSpeed = std::sqrt(m_vecMoveSpeed.x * m_vecMoveSpeed.x + m_vecMoveSpeed.y * m_vecMoveSpeed.y);
        }

        if (std::fabs(m_GasPedal) > 0.05F || fMoveSpeed > 0.01F) {
            // Como el original: la comparacion usa el double 0.05 (0x859068), no el float
            if (boat && bOnWater && static_cast<double>(fMoveSpeed) > 0.05) {
                [[maybe_unused]] auto* const unusedColModel = CEntity::GetColModel(); // El EXE llama a GetColModel aunque no use el resultado
                boat->AddWakePoint(GetPosition());
            }

            auto fTraction = 1.0F;
            if (GetStatus() == STATUS_PLAYER) {
                auto fTractionLoss = VehEDot(m_vecMoveSpeed, GetForward()) * m_pHandlingData->m_fTractionBias;
                if (pad->GetHandBrake())
                    fTractionLoss *= 0.5F;

                fTraction = 1.0F - fTractionLoss;
                if (fTraction < 0.0F) {
                    fTraction = 0.0F;
                } else if (fTraction > 1.0F) {
                    fTraction = 1.0F;
                }
            }

            // Como el original: fsin y fcos del x87 sobre el mismo valor
            const auto fSteerAngleChange = -(fTraction * m_fSteerAngle);
            float fSteerAngleSin, fSteerAngleCos;
            VehESinCos(fSteerAngleChange, fSteerAngleSin, fSteerAngleCos);

            const auto& vecBoundingMin = CEntity::GetColModel()->m_boundBox.m_vecMin;
            CVector vecThrustPoint(0.0F, vecBoundingMin.y * boatHandling->m_fThrustY, vecBoundingMin.z * boatHandling->m_fThrustZ);
            const auto vecTransformedThrustPoint = VehEMatVec(*m_matrix, vecThrustPoint);

            const CVector vecWorldThrustPos{
                vecTransformedThrustPoint.x + GetPosition().x,
                vecTransformedThrustPoint.y + GetPosition().y,
                vecTransformedThrustPoint.z + GetPosition().z,
            };
            // Como el original: la variable comparte la pila con fMoveSpeed, asi que conserva su valor si GetWaterLevel no la escribe
            float fWaterLevel = fMoveSpeed;
            CWaterLevel::GetWaterLevel(vecWorldThrustPos, fWaterLevel, true); // warn: result not checked
            if (!(vecWorldThrustPos.z - 0.5F < fWaterLevel)) {
                if (boat)
                    boat->m_nBoatFlags.bBoatEngineInWater = false;
            }
            else {
                auto fThrustDepth = fWaterLevel - vecWorldThrustPos.z + 0.5F;
                fThrustDepth = fThrustDepth > 1.0F ? 1.0F : fThrustDepth * fThrustDepth;

                if (boat)
                    boat->m_nBoatFlags.bBoatEngineInWater = true;

                bool bIsSlowingDown = false;
                auto fGasState = std::fabs(m_GasPedal);
                if (!(fGasState > 0.01F) || m_nModelIndex == MODEL_SKIMMER) {
                    bIsSlowingDown = true;
                }
                else {
                    if (fGasState < 0.5F)
                        bIsSlowingDown = true;

                    // Como el original: |steer| con comparacion + cambio de signo y z = |steer| * -1
                    auto fSteerAngle = m_fSteerAngle;
                    if (fSteerAngle < 0.0F)
                        fSteerAngle = -fSteerAngle;
                    CVector vecSteer(-fSteerAngleSin, fSteerAngleCos, fSteerAngle * -1.0F);
                    // Como el original: cada componente se multiplica secuencialmente (profundidad, gas, 40, aceleracion, masa)
                    auto vecSteerMoveForce = VehEMatVec(*m_matrix, vecSteer);
                    vecSteerMoveForce = MulComp(vecSteerMoveForce, fThrustDepth);
                    vecSteerMoveForce = MulComp(vecSteerMoveForce, m_GasPedal);
                    vecSteerMoveForce = MulComp(vecSteerMoveForce, 40.0F);
                    vecSteerMoveForce = MulComp(vecSteerMoveForce, m_pHandlingData->m_transmissionData.m_EngineAcceleration);
                    vecSteerMoveForce = MulComp(vecSteerMoveForce, m_fMass);

                    if (vecSteerMoveForce.z > 0.2F)
                        vecSteerMoveForce.z = sq(1.2F - vecSteerMoveForce.z) + 0.2F;

                    if (bPostCollision) {
                        if (m_GasPedal < 0.0F) {
                            vecSteerMoveForce.x *= 5.0F;
                            vecSteerMoveForce.y *= 5.0F;
                        }

                        // Como el original: solo se anula si es negativo (conserva -0 y NaN)
                        if (vecSteerMoveForce.z < 0.0F)
                            vecSteerMoveForce.z = 0.0F;
                        CPhysical::ApplyMoveForce(MulComp(vecSteerMoveForce, timeStep));
                    }
                    else {
                        CPhysical::ApplyMoveForce(MulComp(vecSteerMoveForce, timeStep));

                        const auto vecThrustAppZ = MulComp(GetUp(), boatHandling->m_fThrustAppZ);
                        const CVector vecTurnForcePoint{
                            vecTransformedThrustPoint.x - vecThrustAppZ.x,
                            vecTransformedThrustPoint.y - vecThrustAppZ.y,
                            vecTransformedThrustPoint.z - vecThrustAppZ.z,
                        };
                        CPhysical::ApplyTurnForce(MulComp(vecSteerMoveForce, timeStep), vecTurnForcePoint);

                        // Como el original: el producto escalar con la derecha suma y, z, x
                        const auto fSteerDotRight = (vecSteerMoveForce.y * GetRight().y + vecSteerMoveForce.z * GetRight().z) + vecSteerMoveForce.x * GetRight().x;
                        const auto fTractionSide = -(fSteerDotRight * m_pHandlingData->m_fTractionMultiplier);
                        const auto vecTurnForceSide = MulComp(MulComp(GetRight(), fTractionSide), timeStep);
                        CPhysical::ApplyTurnForce(vecTurnForceSide, GetUp());
                    }

                    // This code does nothing, pero el EXE si llama a GetColModel
                    if (m_GasPedal > 0.0F && GetStatus() == STATUS_PLAYER) {
                        [[maybe_unused]] auto* const unusedColModel = CEntity::GetColModel();
                    }
                }

                if (!bPostCollision && bIsSlowingDown) {
                    auto fTractionLoss = VehEDot(m_vecMoveSpeed, GetForward()) * m_pHandlingData->m_fTractionLoss;
                    const auto fTurnMassLimit = m_fTurnMass * 0.01F;
                    if (fTractionLoss > fTurnMassLimit) {
                        fTractionLoss = fTurnMassLimit;
                    }

                    if (fGasState > 0.01F) {
                        fTractionLoss = (0.55F - fGasState) * fTractionLoss;
                        if (GetStatus() == STATUS_PLAYER)
                            fTractionLoss *= 2.6F;
                        else
                            fTractionLoss *= 5.0F;
                    }

                    if (m_GasPedal < 0.0f && fTractionLoss > 0.0f ||
                        m_GasPedal > 0.0f && fTractionLoss < 0.0f
                    ) {
                        fTractionLoss *= -1.0F;
                    }

                    CVector vecTractionLoss(-fSteerAngleSin, 0.0F, 0.0F);
                    vecTractionLoss = MulComp(vecTractionLoss, fTractionLoss);
                    // Como el original: (matriz * vector) * profundidad y despues * paso, componente a componente
                    const auto vecTractionLossTransformed = MulComp(MulComp(VehEMatVec(*m_matrix, vecTractionLoss), fThrustDepth), timeStep);

                    CPhysical::ApplyMoveForce(vecTractionLossTransformed);
                    CPhysical::ApplyTurnForce(vecTractionLossTransformed, vecTransformedThrustPoint);

                    // Como el original: max(0.01, paso) como (0.01 > paso) ? 0.01 : paso, y el factor escalar se calcula antes de multiplicar por la derecha
                    const auto fUsedTimeStep = (0.01F > timeStep) ? 0.01F : timeStep;
                    const auto fTurnScalar = (((fUsedTimeStep / fTraction) * fTractionLoss) * fSteerAngleSin) * -0.75F;
                    const auto vecTurn = MulComp(GetRight(), fTurnScalar);
                    CPhysical::ApplyTurnForce(vecTurn, GetUp());
                }
            }
        }
    }

    if (m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear != 0.0F) {
        auto right = VehECross(GetForward(), CVector::ZAxisVector());

        // Como el original: el producto escalar con la derecha suma y, z, x y el factor final es -(... * 0.1)
        const auto mult =
              ((right.y * m_vecMoveSpeed.y + right.z * m_vecMoveSpeed.z) + right.x * m_vecMoveSpeed.x)
            * m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear
            * timeStep
            * fImmersionDepth
            * m_fMass
            * -0.1F;

        const auto x = right.x * 0.3F;
        right.x -= right.y * 0.3F;
        right.y += x;

        CPhysical::ApplyMoveForce(MulComp(right, mult));
    }

    if (GetStatus() == STATUS_PLAYER && pad->GetHandBrake()) {
        auto fDirDotProd = VehEDot(m_vecMoveSpeed, GetForward());
        if (fDirDotProd > 0.0F) {
            auto fMoveForceMult = fDirDotProd * m_pHandlingData->m_fSuspensionLowerLimit * timeStep * fImmersionDepth * m_fMass * -0.1F;
            auto vecMoveForce = MulComp(GetForward(), fMoveForceMult);
            CPhysical::ApplyMoveForce(vecMoveForce);
        }
    }

    if (bOnWater && !bPostCollision && !bCollidedWithWorld) {
        ApplyBoatWaterResistance(boatHandling, fImmersionDepth);
    }

    // 0x6DCD63
    if ((m_nModelIndex != MODEL_SKIMMER || m_f2ndSteerAngle == 0.0F) && !bCollidedWithWorld) {
        // Como el original: pow del CRT del EXE (_CIpow) por componente y producto por la velocidad en el espacio del modelo
        const CVector vecTurnRes{
            VehEPow(boatHandling->m_vecTurnRes.x, timeStep),
            VehEPow(boatHandling->m_vecTurnRes.y, timeStep),
            VehEPow(boatHandling->m_vecTurnRes.z, timeStep),
        };
        m_vecTurnSpeed = VehEInvVec(*m_matrix, m_vecTurnSpeed);

        float fMult = vecTurnRes.x / (m_vecTurnSpeed.x * m_vecTurnSpeed.x * 1000.0F + 1.0F) * m_vecTurnSpeed.x - m_vecTurnSpeed.x;
        m_vecTurnSpeed.y = vecTurnRes.y * m_vecTurnSpeed.y;
        m_vecTurnSpeed.z = vecTurnRes.z * m_vecTurnSpeed.z;

        fMult *= m_fTurnMass;
        auto vecTurnForce = MulComp(GetUp(), fMult);

        m_vecTurnSpeed = VehEMatVec(*m_matrix, m_vecTurnSpeed);
        auto vecCentreOfMass = VehEMatVec(*m_matrix, m_vecCentreOfMass);
        const CVector vecTurnPoint{ GetForward().x + vecCentreOfMass.x, GetForward().y + vecCentreOfMass.y, GetForward().z + vecCentreOfMass.z };
        CPhysical::ApplyTurnForce(vecTurnForce, vecTurnPoint);
    }

    // Handle wave collision
    // Como el original: la diferencia por 10000 se trunca con _ftol2 y se reduce a 16 bits con signo (movsx edi,di)
    const auto fWaveMult = (float)(int16)VehEFtol((fImmersionDepth - *fLastWaterImmersionDepth) * 10000.0F);
    if (!bPostCollision && bOnWater && GetUp().z > 0.0F) {
        auto fAudioVolume = fWaveMult * boatHandling->m_fWaveAudioMult;
        if (fAudioVolume > 200.0F)
            m_vehicleAudio.AddAudioEvent(eAudioEvents::AE_BOAT_HIT_WAVE, fAudioVolume);

        if (fWaveMult > 200.0F) {
            // Como el original: * 0.001 en vez de / 1000, limite por suma y conserva -0 y NaN al comparar con 0
            auto fZComp = m_vecMoveSpeed.SquaredMagnitude() * fWaveMult * 0.001F;
            CVector vecWaveMoveForce(0.0F, 0.0F, fZComp);

            if (vecWaveMoveForce.z + m_vecMoveSpeed.z > m_pHandlingData->m_fBrakeDeceleration)
                vecWaveMoveForce.z = m_pHandlingData->m_fBrakeDeceleration - m_vecMoveSpeed.z;
            if (vecWaveMoveForce.z < 0.0F)
                vecWaveMoveForce.z = 0.0F;

            auto fMoveDotProd = VehEDot(m_vecMoveSpeed, GetForward());
            auto fTurnForceMult = fWaveMult * m_pHandlingData->m_fBrakeBias * -0.01F;

            auto vecMoveForce = MulComp(MulComp(GetForward(), fTurnForceMult), fMoveDotProd);
            vecWaveMoveForce = CVector{ vecWaveMoveForce.x + vecMoveForce.x, vecWaveMoveForce.y + vecMoveForce.y, vecWaveMoveForce.z + vecMoveForce.z };
            vecWaveMoveForce = MulComp(vecWaveMoveForce, m_fMass);

            CPhysical::ApplyMoveForce(vecWaveMoveForce);
            CPhysical::ApplyTurnForce(vecWaveMoveForce, vecBuoyancyTurnPoint);
        }
    }

    *fLastWaterImmersionDepth = fImmersionDepth;
    if (boat) {
        boat->m_fxBuoyancyForce = vecBuoyancyForce;
        boat->m_nBoatFlags.bBoatInWater = bOnWater;
    }
    else if (IsAutomobile()) {
        this->AsAutomobile()->m_fDoomHorizontalRotation = vecBuoyancyForce.Magnitude();
    }
}

// 0x6DD130
void CVehicle::DoBoatSplashes(float fWaterDamping) {
    // Como el original (0x6DD130): constantes leidas del EXE y cuentas en el mismo orden
    const auto speedDist = m_vecMoveSpeed.SquaredMagnitude();
    if (speedDist <= std::bit_cast<float>(0x3B23D70Bu) /* 0x858CD8 */ || GetUp().z <= 0.0f || TheCamera.GetLookingForwardFirstPerson() || !IsVisible()) {
        return;
    }

    if (m_autoPilot.m_nCarMission == MISSION_CRUISE && (CTimer::m_FrameCounter & 2) != 0) {
        return;
    }

    auto vec = GetPosition() - TheCamera.GetPosition(); // -> DistanceBetweenPoints2D()
    vec.z = 0.0f;
    const auto dist = vec.Magnitude();
    if (dist >= 80.0f)
        return;

    auto v9 = std::sqrt(speedDist) * 0.075f * fWaterDamping;
    if (m_nModelIndex == MODEL_SKIMMER) {
        v9 = std::min(v9 * 3.0f, 0.5f);
    } else if (v9 > 1.0f) {
        v9 = 1.0f;
    }

    if (v9 <= 0.15f) {
        return;
    }

    auto v48 = v9 * 0.75f;
    if (m_autoPilot.m_nCarMission == MISSION_CRUISE) {
        const auto v10 = v48 + v48;
        v48 = v10 >= 1.0f ? 1.0f : v10;
    }

    // Como el original: el alfa es un byte (truncado con _ftol2, tope 0x40)
    auto alpha8 = static_cast<uint8>(static_cast<int64>(v48 * 128.0f));
    if (alpha8 >= 0x40) {
        alpha8 = 0x40;
    }

    const auto v12      = v48 * 10.0f;
    const auto sizeTmp  = v12 + 0.75f;
    const auto lifeRnd  = CGeneral::GetRandomNumberInRange(0.8f, 1.2f);
    const auto life0    = lifeRnd * ((v48 + v48) + 0.3f); // Como el original: el fld [esp+14h] ocurre antes de liberar los argumentos y lee v48
    if (dist > 50.0f) {
        alpha8 = static_cast<uint8>(static_cast<int64>(((80.0f - dist) * std::bit_cast<float>(0x3D088889u) /* 0x858F10 */) * static_cast<float>(alpha8)));
    }

    FxPrtMult_c particleData(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);

    const auto alpha = static_cast<float>(alpha8) * std::bit_cast<float>(0x3B808081u); // 0x859A3C
    particleData.m_Color.alpha = alpha >= 1.0f ? 1.0f : alpha;

    const auto size = sizeTmp * 0.1f;
    particleData.m_fSize = size >= 1.0f ? 1.0f : size;

    const auto life = life0 * 0.2f;
    particleData.m_fLife = life >= 1.0f ? 1.0f : life;

    const CVector& colMin = GetColModel()->GetBoundingBox().m_vecMin;
    const CVector& colMax = GetColModel()->GetBoundingBox().m_vecMax;

    auto X_MULT = 0.7f;
    auto Z_MULT = 0.0f;
    if (m_nModelIndex == MODEL_SKIMMER) {
        X_MULT = 0.25f; // 0x8D3678
        Z_MULT = 0.85f; // 0x8D3674
    }

    const auto baseVel = CVector{ -GetForward().x, -GetForward().y, -GetForward().z };

    // Como el original: CMatrix * CVector (0x59C890) suma en su propio orden
    CVector p0 = CVehDMatPoint(*m_matrix, CVector{ colMin.x * X_MULT, colMax.y / 2.0f, colMin.z * Z_MULT });
    auto vel0 = baseVel * CGeneral::GetRandomNumberInRange(0.8f, 1.2f);
    vel0 -= GetRight() * CGeneral::GetRandomNumberInRange(0.3f, 0.7f); // minus
    vel0 += GetUp() * CGeneral::GetRandomNumberInRange(0.8f, 1.2f);
    vel0 *= v12;
    g_fx.m_BoatSplash->AddParticle(p0, vel0, 0.0f, particleData);

    CVector p1 = CVehDMatPoint(*m_matrix, CVector{ colMax.x * X_MULT, colMax.y / 2.0f, colMin.z * Z_MULT });
    auto vel1 = baseVel * CGeneral::GetRandomNumberInRange(0.8f, 1.2f);
    vel1 += GetRight() * CGeneral::GetRandomNumberInRange(0.3f, 0.7f);  // plus
    vel1 += GetUp() * CGeneral::GetRandomNumberInRange(0.8f, 1.2f);
    vel1 *= v12;
    g_fx.m_BoatSplash->AddParticle(p1, vel1, 0.0f, particleData);
}

// 0x6DD6F0
void CVehicle::DoSunGlare() {
    return ((void(__thiscall*)(CVehicle*))0x6DD6F0)(this);

    /*
    * Below code should be good so far, I'm lazy to finish it, srry.
    if (physicalFlags.bDestroyed || GetUp().z < 0.f || GetVehicleAppearance() != eVehicleAppearance::VEHICLE_APPEARANCE_AUTOMOBILE || CWeather::SunGlare <= 0.f) {
        return;
    }
    */
}

// 0x6DDF60
void CVehicle::AddWaterSplashParticles() {
    if (!IsPointInSphere(GetPosition(), TheCamera.GetPosition(), 10.f)) {
        return;
    }

    auto fxPrtMult = FxPrtMult_c{ 1.0f, 1.0f, 1.0f, 0.35f, 0.02f, 0.0f, 0.03f };
    const auto& cd = *GetColModel()->m_pColData;
    for (const auto& tri : cd.GetTris()) {
        // Get and transform triangle vertices to world space
        auto vertices = cd.GetTriVertices(tri);
        for (auto& v : vertices) {
            v = CVehDMatPoint(*m_matrix, v); // Como el original: CMatrix * CVector (0x59C890)
        }

        const auto v0v1 = vertices[1] - vertices[0];
        const auto v1v2 = vertices[2] - vertices[1];

        // Como el original: 1 - (int)(lluvia * -2); dos rand() directos (r1 y r2) y el punto es v0 + v0v1*r1 + v1v2*(r2*r1)
        for (int32 i = 1 - static_cast<int32>(CWeather::Rain * -2.0f); i > 0; i--) {
            const float r1 = static_cast<float>(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000100u); // 0x858C7C
            const float r2 = static_cast<float>(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000100u);
            const float t  = r2 * r1;
            const CVector pos{
                (v0v1.x * r1 + vertices[0].x) + v1v2.x * t,
                (v0v1.y * r1 + vertices[0].y) + v1v2.y * t,
                (v0v1.z * r1 + vertices[0].z) + v1v2.z * t
            };
            g_fx.m_Splash->AddParticle(pos, CVector{ 0.0f, 0.0f, 0.0f }, 0.f, fxPrtMult);
        }
    }
}

// 0x6DE240
void CVehicle::AddExhaustParticles() {
    if (m_bOffscreen)
        return;

    float dist = DistanceBetweenPointsSquared(GetPosition(), TheCamera.GetPosition());
    if (dist > 256.0f || dist > 64.0f && !((CTimer::GetFrameCounter() + m_nModelIndex) & 1)
    ) {
        return;
    }
    auto mi = GetVehicleModelInfo();
    CVector firstExhaustPos = mi->GetModelDummyPosition(DUMMY_EXHAUST);
    CVector secondExhaustPos = firstExhaustPos;
    secondExhaustPos.x *= -1.0f;
    CMatrix entityMatrix (*m_matrix);
    bool bHasDoubleExhaust = m_pHandlingData->m_bDoubleExhaust;
    if (IsSubBike()) {
        auto* bike = AsBike();
        bike->CalculateLeanMatrix();
        entityMatrix = bike->m_mLeanMatrix;
        switch (m_nModelIndex) {
        case MODEL_FCR900:
            if (m_anExtras[0] == 1 || m_anExtras[0] == 2)
                bHasDoubleExhaust = true;
            break;
        case MODEL_NRG500:
            if (!m_anExtras[0] || m_anExtras[0] == 1)
                secondExhaustPos = mi->GetModelDummyPosition(DUMMY_EXHAUST_SECONDARY);
            break;
        case MODEL_BF400:
            if (m_anExtras[0] == 2)
                bHasDoubleExhaust = true;
            break;
        }
    }

    if (firstExhaustPos == 0.0f) {
        return;
    }

    // Como el original: producto escalar sumado en orden z, y, x; la componente z de la velocidad queda a 0
    CVector vecParticleVelocity{ 0.0f, 0.0f, 0.0f };
    if (((GetForward().z * m_vecMoveSpeed.z) + (GetForward().y * m_vecMoveSpeed.y)) + GetForward().x * m_vecMoveSpeed.x >= 0.05f) {
        vecParticleVelocity.x = m_vecMoveSpeed.x * 30.0f;
        vecParticleVelocity.y = m_vecMoveSpeed.y * 30.0f;
    } else {
        // Como el original: estatica local del EXE (valor 0xC1CDB4, guarda 0xC1CDB8 bit 0), se inicializa una sola vez
        auto& randomFactorInit = StaticRef<uint32>(0xC1CDB8);
        auto& randomFactor     = StaticRef<float>(0xC1CDB4);
        if (!(randomFactorInit & 1)) {
            randomFactorInit |= 1;
            randomFactor = CGeneral::GetRandomNumberInRange(-1.8f, -0.9f);
        }
        vecParticleVelocity.x = randomFactor * GetForward().x;
        vecParticleVelocity.y = randomFactor * GetForward().y;
    }

    // Como el original: CMatrix * CVector (0x59C890)
    firstExhaustPos = CVehDMatPoint(entityMatrix, firstExhaustPos);
    bool bFirstExhaustSubmergedInWater = false;
    bool bSecondExhaustSubmergedInWater = false;
    float pLevel = 0.0f;
    if (physicalFlags.bTouchingWater && CWaterLevel::GetWaterLevel(firstExhaustPos, pLevel, true) &&
        pLevel >= firstExhaustPos.z) {
        bFirstExhaustSubmergedInWater = true;
    }

    if (bHasDoubleExhaust) {
        secondExhaustPos = CVehDMatPoint(entityMatrix, secondExhaustPos);
        if (physicalFlags.bTouchingWater && CWaterLevel::GetWaterLevel(secondExhaustPos, pLevel, true) &&
            pLevel >= secondExhaustPos.z) {
            bSecondExhaustSubmergedInWater = true;
        }
    }

    if (CGeneral::GetRandomNumberInRange(1.0f, 3.0f) * (m_GasPedal + 1.1f) <= 2.5f)
        return;

    float fMoveSpeed = m_vecMoveSpeed.Magnitude() * 0.5f;
    float particleAlpha = 0.0f;
    if (0.25f - fMoveSpeed >= 0.0f) {
        particleAlpha = 0.25f - fMoveSpeed;
    }
    float fLife = std::max(0.2f - fMoveSpeed, 0.0f);
    FxPrtMult_c fxPrt(0.9f, 0.9f, 1.0f, particleAlpha, 0.2f, 1.0f, fLife);

    for (auto i = 0; i < 2; i++) {
        FxSystem_c* firstExhaustFxSystem = g_fx.m_SmokeII3expand;
        if (bFirstExhaustSubmergedInWater) {
            fxPrt.m_Color.alpha = particleAlpha * 0.5f;
            fxPrt.m_fSize = 0.6f;
            firstExhaustFxSystem = g_fx.m_Bubble;
        }
        firstExhaustFxSystem->AddParticle(firstExhaustPos, vecParticleVelocity, 0.0f, fxPrt, -1.0f, m_fContactSurfaceBrightness);
        if (bHasDoubleExhaust) {
            FxSystem_c* secondExhaustFxSystem = g_fx.m_SmokeII3expand;
            if (bSecondExhaustSubmergedInWater) {
                fxPrt.m_Color.alpha = particleAlpha * 0.5f;
                fxPrt.m_fSize = 0.6f;
                secondExhaustFxSystem = g_fx.m_Bubble;
            }
            secondExhaustFxSystem->AddParticle(secondExhaustPos, vecParticleVelocity, 0.0f, fxPrt, -1.0f, m_fContactSurfaceBrightness);
        }

        if (m_GasPedal > 0.5f && m_nCurrentGear < 3) {
            if (CGeneral::GetRandomNumber() % 2) {
                FxSystem_c* secondaryExhaustFxSystem = g_fx.m_SmokeII3expand;
                if (bFirstExhaustSubmergedInWater) {
                    fxPrt.m_Color.alpha = particleAlpha * 0.5f;
                    fxPrt.m_fSize = 0.6f;
                    secondaryExhaustFxSystem = g_fx.m_Bubble;
                }
                secondaryExhaustFxSystem->AddParticle(firstExhaustPos, vecParticleVelocity, 0.0f, fxPrt, -1.0f, m_fContactSurfaceBrightness);
            } else if (bHasDoubleExhaust) {
                FxSystem_c* secondaryExhaustFxSystem = g_fx.m_SmokeII3expand;
                if (bSecondExhaustSubmergedInWater) {
                    fxPrt.m_Color.alpha = particleAlpha * 0.5f;
                    fxPrt.m_fSize = 0.6f;
                    secondaryExhaustFxSystem = g_fx.m_Bubble;
                }
                secondaryExhaustFxSystem->AddParticle(secondExhaustPos, vecParticleVelocity, 0.0f, fxPrt, -1.0f, m_fContactSurfaceBrightness);
            }
        }
    }
}

// always return false?
// 0x6DE880
bool CVehicle::AddSingleWheelParticles(tWheelState wheelState, uint32 arg1, float arg2, float arg3, CColPoint* arg4, CVector* arg5, float arg6, int32 arg7, uint32 surfaceType, bool* bloodState, uint32 arg10) {
    return ((bool(__thiscall*)(CVehicle*, tWheelState, uint32, float, float, CColPoint*, CVector*, float, int32, uint32, bool*, uint32))0x6DE880)(this, wheelState, arg1, arg2, arg3, arg4, arg5, arg6, arg7, surfaceType, bloodState, arg10);
}

// 0x6DF3D0
bool CVehicle::GetSpecialColModel() {
    // Como el original (0x6DF3D9): el índice (int8 con signo) se usa sin comprobar el rango
    if (m_vehicleSpecialColIndex > -1 && m_aSpecialColVehicle.data()[m_vehicleSpecialColIndex] == this) {
        return true;
    }

    const auto specialCMSlot = rng::find(m_aSpecialColVehicle, nullptr);
    if (specialCMSlot == m_aSpecialColVehicle.end()) {
        return false;
    }

    const auto specialCMIdx = rng::distance(m_aSpecialColVehicle.begin(), specialCMSlot);
    m_vehicleSpecialColIndex = specialCMIdx;
    physicalFlags.bAddMovingCollisionSpeed = true;
    *specialCMSlot = this;
    CEntity::RegisterReference(*specialCMSlot);

    auto& cm = m_aSpecialColModel[m_vehicleSpecialColIndex];
    CCollision::RemoveTrianglePlanes(&cm); // Como el original (0x6DF438): llama a 0x4185A0 con el CColModel
    if (!cm.m_pColData) {
        cm.AllocateData();
    }
    cm = *GetModelInfo()->m_pColModel;

    m_aSpecialHydraulicData[specialCMIdx].m_fSuspensionExtendedUpperLimit = 100.f;
    rng::fill(m_aSpecialHydraulicData[specialCMIdx].m_aWheelSuspension, 0.0f);
    return true;
}

// 0x6DF930
void CVehicle::RemoveVehicleUpgrade(int32 upgradeModelIndex) {
    // Como el original (0x6DF930..0x6DFA1D): si ClearVehicleUpgradeFlags (0x6D3210) lo resuelve termina; si no, quita el componente del modelo (marco de reemplazo con bUsesVehDummy
    // o atomos con RemoveUpgradeCB, que se pasa con el CarMod como valor en 'data') y el del modelo enlazado (ms_linkedUpgrades), y limpia la mejora de m_anUpgrades.
    CBaseModelInfo* const upgradeInfo = CModelInfo::ms_modelInfoPtrs[upgradeModelIndex];
    if (ClearVehicleUpgradeFlags(upgradeModelIndex, upgradeInfo->CarMod)) {
        return;
    }

    const int32 linkedModelIndex = CVehicleModelInfo::ms_linkedUpgrades.FindOtherUpgrade(static_cast<int16>(upgradeModelIndex));

    const int32 carMod = upgradeInfo->CarMod;
    if (upgradeInfo->bUsesVehDummy) {
        RemoveReplacementUpgrade(carMod);
        if (carMod == 2) {
            m_fWheelScale = 1.0f;
            RemoveReplacementUpgrade(5);
            RemoveReplacementUpgrade(4);
            RemoveReplacementUpgrade(7);
        }
    } else {
        RpClumpForAllAtomics(GetRpClump(), RemoveUpgradeCB, std::bit_cast<void*>(carMod));
    }

    if (linkedModelIndex != -1) {
        CBaseModelInfo* const linkedInfo = CModelInfo::ms_modelInfoPtrs[linkedModelIndex];
        const int32 linkedCarMod = linkedInfo->CarMod;
        if (linkedInfo->bUsesVehDummy) {
            RemoveReplacementUpgrade(linkedCarMod);
        } else {
            RpClumpForAllAtomics(GetRpClump(), RemoveUpgradeCB, std::bit_cast<void*>(linkedCarMod));
        }
    }

    // El EXE recorre las 15 ranuras (0x43A, int16 con signo extendido) y borra todas las que coinciden
    for (auto& upgrade : m_anUpgrades) {
        if (upgrade == upgradeModelIndex) {
            upgrade = -1;
        }
    }
}

// 0x6DFA20
void CVehicle::AddUpgrade(int32 modelIndex, int32 upgradeIndex) {
    // Como el original (0x6DFA20..0x6DFB6A): quita los atomos de mejora previos del indice, crea el atomo de la mejora en el marco padre y, salvo si el padre es el componente 1,
    // una segunda copia (danada) con el descriptor alterno (0->3, 1->4, 2->5, 6->7, 12->13 si su padre != -1); deja visible el marco y reaplica la matricula.
    auto* const vehicleInfo = static_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[static_cast<int16>(m_nModelIndex)]);
    CBaseModelInfo* const upgradeInfo = CModelInfo::ms_modelInfoPtrs[modelIndex];
    const UpgradePosnDesc* desc = vehicleInfo->m_pVehicleStruct->m_aUpgrades.data() + upgradeIndex;

    RwFrame* const frame = CClumpModelInfo::GetFrameFromId(GetRpClump(), desc->m_nParentComponentId);
    RpClumpForAllAtomics(GetRpClump(), RemoveUpgradeCB, std::bit_cast<void*>(upgradeIndex));
    CreateUpgradeAtomic(upgradeInfo, desc, frame, false);

    if (desc->m_nParentComponentId == 1) {
        return;
    }

    // El EXE relee m_pVehicleStruct del modelo del vehiculo en cada rama
    const UpgradePosnDesc* const upgrades = vehicleInfo->m_pVehicleStruct->m_aUpgrades.data();
    switch (upgradeIndex) {
    case 0:  if (upgrades[3].m_nParentComponentId  != -1) desc = &upgrades[3];  break;
    case 1:  if (upgrades[4].m_nParentComponentId  != -1) desc = &upgrades[4];  break;
    case 2:  if (upgrades[5].m_nParentComponentId  != -1) desc = &upgrades[5];  break;
    case 6:  if (upgrades[7].m_nParentComponentId  != -1) desc = &upgrades[7];  break;
    case 12: if (upgrades[13].m_nParentComponentId != -1) desc = &upgrades[13]; break;
    default: break;
    }

    CreateUpgradeAtomic(upgradeInfo, desc, frame, true);

    if (frame) {
        RwFrameForAllObjects(frame, static_cast<RwObjectCallBack>(SetVehicleAtomicVisibilityCB), std::bit_cast<void*>(1));
        RwFrameForAllChildren(frame, static_cast<RwFrameCallBack>(SetVehicleAtomicVisibilityCB), std::bit_cast<void*>(1));
    }

    auto* const info = static_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[static_cast<int16>(m_nModelIndex)]);
    CCustomCarPlateMgr::SetupClumpAfterVehicleUpgrade(GetRpClump(), info->m_pPlateMaterial, info->m_nPlateType);
}

// 0x6DFC50
void CVehicle::UpdateTrailerLink(bool arg0, bool arg1) {
    ((void(__thiscall*)(CVehicle*, bool, bool))0x6DFC50)(this, arg0, arg1);
}

// 0x6E0050
void CVehicle::UpdateTractorLink(bool arg0, bool arg1) {
    ((void(__thiscall*)(CVehicle*, bool, bool))0x6E0050)(this, arg0, arg1);
}

// 0x6E0400
CEntity* CVehicle::ScanAndMarkTargetForHeatSeekingMissile(CEntity* entity) {
    // Como el original (0x6E0400..0x6E05BA): pide el objetivo a PickTargetForHeatSeekingMissile (origen = posicion, direccion = matriz + 0x10 SIN comprobar nulo, multiplicador 1.2f,
    // ignora este vehiculo, aviones con prioridad, objetivo previo = entity); en los aviones (subtipo 4) el dword 0x9E4 guarda en el bit 31 el ultimo resultado de la linea de vision y en los
    // 31 bits bajos un 1 si ya paso el primer milisegundo; la vision solo se vuelve a calcular si pasaron mas de 1000 ms (menos 1 si los 31 bits bajos son 0).
    const CVector origin    = GetPosition();
    const CVector direction = *reinterpret_cast<const CVector*>(reinterpret_cast<uintptr_t>(m_matrix) + 0x10);
    CEntity* const target   = CWeapon::PickTargetForHeatSeekingMissile(origin, direction, 1.2f, this, true, entity);

    const bool isPlane = m_nVehicleSubType == VEHICLE_TYPE_PLANE;
    auto* const planeState = reinterpret_cast<uint32*>(reinterpret_cast<uintptr_t>(this) + 0x9E4);
    const uint32 slack = isPlane && (*planeState & 0x7FFFFFFFu) == 0 ? 1u : 0u;

    bool visible;
    if (target && CTimer::GetTimeInMS() - slack > 1000u) {
        // Como el original: durante la prueba ni el vehiculo ni el objetivo usan colision (solo el bit 0 se restaura al valor que tenia)
        const bool targetUsedCollision = target->m_bUsesCollision;
        const bool selfUsedCollision   = m_bUsesCollision;
        m_bUsesCollision       = false;
        target->m_bUsesCollision = false;
        const CVector& to   = target->GetPosition();
        const CVector& from = GetPosition();
        visible = CWorld::GetIsLineOfSightClear(from, to, true, true, false, true, false, true, false);
        m_bUsesCollision       = selfUsedCollision;
        target->m_bUsesCollision = targetUsedCollision;
        if (isPlane) {
            *planeState = (CTimer::GetTimeInMS() > 1u ? 1u : 0u) | (uint32(visible) << 31);
        }
    } else {
        visible = isPlane ? (*planeState >> 31) != 0 : true;
    }

    if (!visible) {
        CWeaponEffects::ClearCrossHairImmediately(0);
        return nullptr;
    }
    // Como el original (0x6E0564..0x6E0568): sin objetivo, o con el vehiculo no controlado por el jugador (estado != 0), solo se borra la mira
    if (!target || GetStatus() != STATUS_PLAYER) {
        CWeaponEffects::ClearCrossHairImmediately(0);
        return target;
    }
    CWeaponEffects::MarkTarget(0, target->GetPosition(), 255, 255, 255, 100, 1.3f, true);
    return target;
}

// 0x6E05C0
void CVehicle::FireHeatSeakingMissile(CEntity* targetEntity, eOrdnanceType type, bool arg2) {
    // Como el original (0x6E05C0..0x6E07CE): con arg2 respeta el intervalo del arma (GetPlaneOrdnanceRateOfFire, comparacion sin signo); lee la posicion (matriz + 0x30, o la del placeable si no hay matriz)
    // ANTES de pedir la de la ordenanza; el lado de la ordenanza alterna con m_nOrdnanceCycleIndex (0 -> x sin negar y el indice pasa a 1; otro valor -> x negada y el indice pasa a 0); la direccion que pasa es matriz + 0x10
    // sin comprobar nulo; el origen del misil (velocidad hacia delante recortada a 0 * direccion * paso, mas ordenanza y posicion) se calcula con la x87 del EXE (X87Math); arma 20 (cohorte buscador de calor),
    // temblor del mando 0/1 si el conductor es el jugador y hora del disparo en 0x4FC (tipo 1) o 0x500 (resto).
    const uint32 lastFiringTime = type == static_cast<eOrdnanceType>(1) ? m_nProjectileWeaponFiringTime : m_nAdditionalProjectileWeaponFiringTime;
    if (arg2 && CTimer::GetTimeInMS() <= GetPlaneOrdnanceRateOfFire(type) + lastFiringTime) {
        return;
    }

    const CVector position = GetPosition();
    const CVector ordnance = GetPlaneOrdnancePosition(type);

    const bool secondSide = m_nOrdnanceCycleIndex != 0;
    m_nOrdnanceCycleIndex = secondSide ? 0 : 1;

    // El EXE usa la matriz sin comprobar nulo (+0x10 direccion, +0x44.. velocidad): se lee por desplazamiento para no anadir comprobaciones
    const auto* const direction = reinterpret_cast<const CVector*>(reinterpret_cast<uintptr_t>(m_matrix) + 0x10);
    CVector spawn;
    HeatFire73SpawnCpp(reinterpret_cast<const float*>(&m_vecMoveSpeed), reinterpret_cast<const float*>(direction), reinterpret_cast<const float*>(&ordnance), secondSide, reinterpret_cast<const float*>(&position), &CTimer::ms_fTimeStep, reinterpret_cast<float*>(&spawn));
    // Como el original (0x6E06D5..0x6E0749): la direccion se copia a un local y se pasa su direccion
    const CVector directionCopy = *direction;
    CProjectileInfo::AddProjectile(this, static_cast<eWeaponType>(20), spawn, 0.0f, &directionCopy, targetEntity);

    if (m_pDriver && m_pDriver->IsPlayer()) {
        const auto pedType = m_pDriver->m_nPedType;
        if (pedType == PED_TYPE_PLAYER1 || pedType == PED_TYPE_PLAYER2) {
            CPad::GetPad(pedType)->StartShake(240, 160, 0);
        }
    }

    if (type == static_cast<eOrdnanceType>(1)) {
        m_nProjectileWeaponFiringTime = CTimer::GetTimeInMS();
    } else {
        m_nAdditionalProjectileWeaponFiringTime = CTimer::GetTimeInMS();
    }
}

// 0x6E07E0
void CVehicle::PossiblyDropFreeFallBombForPlayer(eOrdnanceType type, bool arg1) {
    // Como el original (0x6E07E0..0x6E0944): con arg1 respeta el intervalo del arma (GetPlaneOrdnanceRateOfFire); suelta una bomba (arma 21) en la posicion de la ordenanza alternando el
    // lado (m_nOrdnanceCycleIndex 0/1), hace vibrar el mando 0/1 si el conductor es el jugador y anota la hora del disparo (0x4FC para el tipo 1, 0x500 para los demas).
    const uint32 lastFiringTime = type == static_cast<eOrdnanceType>(1) ? m_nProjectileWeaponFiringTime : m_nAdditionalProjectileWeaponFiringTime;
    if (arg1 && CTimer::GetTimeInMS() <= GetPlaneOrdnanceRateOfFire(type) + lastFiringTime) {
        return;
    }

    // El EXE lee la posicion (matriz + 0x30, o la del placeable si no hay matriz) ANTES de pedir la de la ordenanza; la direccion que pasa es matriz + 0x10 sin comprobar nulo
    const CVector position = GetPosition();
    const CVector ordnance = GetPlaneOrdnancePosition(type);

    const bool    secondSide = m_nOrdnanceCycleIndex != 0;
    const CVector spawn{ position.x + (secondSide ? -ordnance.x : ordnance.x), ordnance.y + position.y, ordnance.z + position.z };
    m_nOrdnanceCycleIndex = secondSide ? 0 : 1;

    const auto* const direction = reinterpret_cast<const CVector*>(reinterpret_cast<uintptr_t>(m_matrix) + 0x10);
    CProjectileInfo::AddProjectile(this, static_cast<eWeaponType>(21), spawn, 0.0f, direction, nullptr);

    if (m_pDriver && m_pDriver->IsPlayer()) {
        const auto pedType = m_pDriver->m_nPedType;
        if (pedType == PED_TYPE_PLAYER1 || pedType == PED_TYPE_PLAYER2) {
            CPad::GetPad(pedType)->StartShake(240, 160, 0);
        }
    }

    if (type == static_cast<eOrdnanceType>(1)) {
        m_nProjectileWeaponFiringTime = CTimer::GetTimeInMS();
    } else {
        m_nAdditionalProjectileWeaponFiringTime = CTimer::GetTimeInMS();
    }
}

// 0x6E0950
void CVehicle::ProcessSirenAndHorn(bool arg0) {
    // Como el original (0x6E0950..0x6E0A43): con sirena (UsesSiren, 0x6D8470) lee el historial de bocina del pad 0 (bHornHistory/iCurrHornHistory; el indice es un byte sin acotar a 5) y
    // decide m_HornCounter: con la entrada actual pulsada vale 1 si las de (i+4)%5 e (i+3)%5 tambien lo estan; con la actual suelta, si (i+4)%5 esta pulsada e (i+1)%5 suelta, queda a 0 y
    // conmuta bSirenOrAlarm; en cualquier otro caso 0. Sin sirena y con arg0: salvo alarma activa (distinta de 0 y 0xFFFF) en un vehiculo no destruido, copia GetHorn() del pad 0.
    if (UsesSiren()) {
        CPad& pad = CPad::Pads[0];
        const int32 index = static_cast<uint8>(pad.iCurrHornHistory);
        const char* const history = pad.bHornHistory.data();
        if (history[index] == 0) {
            if (history[(index + 4) % 5] == 0 || history[(index + 1) % 5] != 0) {
                m_HornCounter = 0;
                return;
            }
            m_HornCounter = 0;
            vehicleFlags.bSirenOrAlarm = !vehicleFlags.bSirenOrAlarm;
            return;
        }
        if (history[(index + 4) % 5] == 0 || history[(index + 3) % 5] == 0) {
            m_HornCounter = 0;
            return;
        }
        m_HornCounter = 1;
        return;
    }

    if (!arg0 || !CanUpdateHornCounter()) {
        return;
    }

    m_HornCounter = CPad::Pads[0].GetHorn() ? 1 : 0;
}

// Cuerpo de DoHeadLightEffect (0x6E0A50) y de las dos copias en linea que DoVehicleLights (0x6E1A60) trae con lightId = 1 (el EXE no llama a 0x6E0A50 para ellas). Las dos son la misma
// secuencia x87 salvo el tamano de la corona: la funcion calcula ((1 - distancia / 150) * 0.4) * sqrtDot y las copias en linea ((1 - distancia / 150) * sqrtDot) * 0.4 (otro redondeo);
// `inlinedCopy` elige la segunda. Cada tramo x87 del EXE esta en un bloque de X87Math.cpp (VehHeadTail*): dejan las banderas, C0-C3 y los registros muertos como el EXE.
static bool VehicleHeadLightEffect(CVehicle& vehicle, bool inlinedCopy, int32 lightId, CMatrix& vehicleMatrix, uint8 isRight, uint8 disabledOrAlarm) {
    // Como el original: la posicion del dummy es el elemento 2 * lightId del modelo; con lightId == 1 y un vector exactamente nulo (comparacion ordenada: un NaN no cuenta como cero) no se dibuja nada
    const auto& dummy = vehicle.GetVehicleModelInfo()->GetModelDummyPosition(static_cast<eVehicleDummy>(2 * lightId));
    if (lightId == 1 && VehHeadTailDummyZeroCpp(&dummy.x)) {
        return false;
    }

    // Como el original: toda la geometria es aritmetica x87 (punto + frente * 0.05, CMatrix * CVector de 0x59C890, direccion a la camara con 0x59C970 y producto escalar con el frente de la matriz)
    float offset[3], magnitude, dot;
    VehHeadTailHeadGeometryCpp(&dummy.x, vehicle.m_matrix ? &vehicle.m_matrix->GetForward().x : nullptr, vehicle.m_placement.m_fHeading, isRight == 0, reinterpret_cast<const float*>(&vehicleMatrix),
                               &TheCamera.GetPosition().x, offset, &magnitude, &dot);

    // Como el original: devuelve "disabledOrAlarm == 0" (salvo la salida del dummy nulo, que devuelve false); la geometria se calcula siempre antes de mirar el argumento
    if (disabledOrAlarm != 0) {
        return false;
    }
    if (!VehHeadTailPositiveCpp(&dot)) {
        return true;
    }
    // En primera persona el jugador no ve las coronas de su propio vehiculo
    if (CCamera::GetActiveCamera().m_nMode == MODE_1STPERSON && &vehicle == FindPlayerVehicle(-1, false)) {
        return true;
    }

    // Como el original: un tren que no es el tranvia (subtipo 6, modelo != 0x1C1) usa otro limite, otros tamanos y otro alfa
    const bool isTrain = vehicle.m_nVehicleSubType == VEHICLE_TYPE_TRAIN && vehicle.m_nModelIndex != MODEL_TRAM;
    const bool halogen = (static_cast<uint32>(vehicle.m_pHandlingData->m_nHandlingFlags) & VEHICLE_HANDLING_HALOGEN_LIGHTS) != 0;
    float      sqrtDot, size, range;
    uint32     red, blue;
    const CVector position{ offset[0], offset[1], offset[2] };
    // Como el original: el id de la corona es la direccion del vehiculo + lado + 2 * indice (+ 4 la linea de luz)
    const auto id = static_cast<uint32>(reinterpret_cast<uintptr_t>(&vehicle)) + isRight + static_cast<uint32>(lightId) * 2;

    // La linea de luz solo si sqrtDot > limite (0.9; 0.85 en un tren) y la distancia a la camara es menor de 40 (la segunda comparacion solo se hace si pasa la primera)
    if (VehHeadTailLimitCpp(&dot, isTrain, &sqrtDot) && VehHeadTailNearCpp(&magnitude)) {
        VehHeadTailRangeStoreCpp(&TheCamera.m_fLODDistMultiplier, &range);
        CCoronas::RegisterCorona(
            id + 4, &vehicle,
            static_cast<uint8>(halogen ? 0x96 : 0xA0), static_cast<uint8>(halogen ? 0x96 : 0xA0), static_cast<uint8>(halogen ? 0xC3 : 0x8C), 0xFF,
            position, isTrain ? 0.3f : 0.075f, range,
            CORONATYPE_HEADLIGHTLINE, FLARETYPE_NONE, CORREFL_NONE, LOSCHECK_OFF, TRAIL_OFF,
            sqrtDot, false, 0.3f, false, 15.0f, false, false
        );
    }
    VehHeadTailHeadCoronaCpp(&magnitude, &sqrtDot, &TheCamera.m_fLODDistMultiplier, isTrain, halogen, inlinedCopy, &size, &range, &red, &blue);
    CCoronas::RegisterCorona(
        id, &vehicle,
        static_cast<uint8>(red), static_cast<uint8>(red), static_cast<uint8>(blue), 0x80,
        position, size, range,
        CORONATYPE_HEADLIGHT, FLARETYPE_NONE, CORREFL_SIMPLE, LOSCHECK_OFF, TRAIL_OFF,
        sqrtDot, false, 0.5f, false, 15.0f, false, false
    );
    return true;
}

// 0x6E0A50
bool CVehicle::DoHeadLightEffect(int32 lightId, CMatrix& vehicleMatrix, uint8 isRight, uint8 disabledOrAlarm) {
    return VehicleHeadLightEffect(*this, false, lightId, vehicleMatrix, isRight, disabledOrAlarm);
}

// 0x6E0E20
void CVehicle::DoHeadLightBeam(eVehicleDummy dummyId, CMatrix& matrix, bool arg2) {
    const auto* mi = GetVehicleModelInfo();
    CVector pointModelSpace = mi->GetModelDummyPosition(static_cast<eVehicleDummy>(2 * dummyId));
    if (dummyId == DUMMY_LIGHT_REAR_MAIN && pointModelSpace.IsZero())
        return;

    // Como el original: CMatrix * CVector (0x59C890) y producto escalar sumado en orden z, y, x
    CVector point = CVehDMatPoint(matrix, pointModelSpace);
    if (!arg2) {
        point -= 2 * pointModelSpace.x * matrix.GetRight();
    }
    const CVector pointToCamDir = Normalized(TheCamera.GetPosition() - point);
    const auto    dotToCam = ((pointToCamDir.z * matrix.GetForward().z) + (pointToCamDir.y * matrix.GetForward().y)) + pointToCamDir.x * matrix.GetForward().x;
    const auto    alpha = (uint8)((1.0f - std::fabs(dotToCam)) * 32.0f);

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,            RWRSTATE(rwSHADEMODEGOURAUD));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,        RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATECULLMODE,             RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONGREATER));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(FALSE));

    const float   angleMult   = ModelIndices::IsForklift((int32)m_nModelIndex) ? 0.5f : 0.15f;
    const CVector lightNormal = Normalized(matrix.GetForward() - matrix.GetUp() * angleMult);
    const CVector lightRight  = Normalized(CrossProduct(lightNormal, pointToCamDir));
    const CVector lightPos    = point - matrix.GetForward() * 0.1f;

    const CVector posn[] = {
        lightPos - lightRight * 0.05f,
        lightPos + lightRight * 0.05f,
        lightPos + lightNormal * 3.0f - lightRight * 0.5f,
        lightPos + lightNormal * 3.0f + lightRight * 0.5f,
        lightPos + lightNormal * 0.2f
    };
    const uint8 alphas[] = { alpha, alpha, 0, 0, alpha };

    // Como el original: los vertices e indices se escriben en los buffers temporales globales (0xC4D958 / 0xC4B958)
    for (auto i = 0u; i < std::size(posn); i++) {
        auto& vertex = TempBufferVertices.m_3d[i];
        vertex.objVertex.x = posn[i].x;
        vertex.objVertex.y = posn[i].y;
        vertex.objVertex.z = posn[i].z;
        vertex.color = (static_cast<uint32>(alphas[i]) << 24) | 0xFFFFFFu;
    }

    static constexpr RxVertexIndex indices[] = { 0, 1, 4, 1, 3, 4, 2, 3, 4, 0, 2, 4 };
    for (auto i = 0u; i < std::size(indices); i++) {
        aTempBufferIndices[i] = indices[i];
    }
    uiTempBufferIndicesStored  = static_cast<int32>(std::size(indices));
    uiTempBufferVerticesStored = static_cast<int32>(std::size(posn));

    if (RwIm3DTransform(TempBufferVertices.m_3d, std::size(posn), nullptr, rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ))
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
        RwIm3DEnd();
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,           RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,              RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,             RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,     RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,              RWRSTATE(rwCULLMODECULLBACK));
}

// Como el original: vector 2D normalizado en linea; si la longitud al cuadrado es 0 no se normaliza (se multiplica por 0)
static CVector2D CVehNormalise2DInline(float x, float y) {
    const float lenSq = y * y + x * x;
    float recip = lenSq;
    if (lenSq != 0.0F) {
        recip = 1.0F / std::sqrt(lenSq);
    }
    return { x * recip, y * recip };
}

// 0x6E1440
void CVehicle::DoHeadLightReflectionSingle(CMatrix& lightMat, bool bRight) {
    auto vehOffset = GetDummyPositionObjSpace(DUMMY_LIGHT_FRONT_MAIN);
    if (!bRight) {
        vehOffset.x *= -1.f;
    }
    const auto lightFwd2D = CVehNormalise2DInline(lightMat.GetForward().x, lightMat.GetForward().y);
    const auto lightRight2D = CVehNormalise2DInline(lightMat.GetRight().x, lightMat.GetRight().y);
    // Como el original: comprueba el subtipo (0x594), no el tipo
    const auto lightSize = (IsSubBike() || GetModelId() == MODEL_QUAD)
        ? 1.25f
        : std::fabs(vehOffset.x) * 4.0f;

    const float offsetDistance = (lightSize + lightSize + 1.0f) + vehOffset.y;

    const auto shdwFront = lightFwd2D * (lightSize * 2.0f);
    const auto shdwSide  = (lightFwd2D * lightSize).GetPerpRight();

    // Como el original: x = (der.x * off.x) + ((fwd.x * dist) + pos.x)
    const auto pos = GetPosition();
    CShadows::StoreCarLightShadow(
        this,
        reinterpret_cast<int32>(&m_matrix) + 2,
        gpShadowHeadLightsTex2,
        CVector(
            (lightRight2D.x * vehOffset.x) + ((lightFwd2D.x * offsetDistance) + pos.x),
            (lightRight2D.y * vehOffset.x) + ((lightFwd2D.y * offsetDistance) + pos.y),
            pos.z + 2.0f
        ),
        shdwFront.x, shdwFront.y,
        shdwSide.x, shdwSide.y,
        45, 45, 45,
        7.0f
    );
}

// 0x6E1600
void CVehicle::DoHeadLightReflectionTwin(CMatrix& matrix) {
    const auto& vehOffset = GetDummyPositionObjSpace(DUMMY_LIGHT_FRONT_MAIN);
    const auto lightFwd2D = CVehNormalise2DInline(matrix.GetForward().x, matrix.GetForward().y);
    const auto lightSize  = vehOffset.x * 4.0f;

    const auto offsetDistance = (lightSize + lightSize + 1.0f) + vehOffset.y;

    const auto shdwFront = lightFwd2D * (lightSize * 2.0f);
    const auto shdwSide  = (lightFwd2D * lightSize).GetPerpRight();

    CShadows::StoreCarLightShadow(
        this,
        reinterpret_cast<int32>(&m_matrix) + 2,
        gpShadowHeadLightsTex,
        CVector((lightFwd2D.x * offsetDistance) + GetPosition().x, (lightFwd2D.y * offsetDistance) + GetPosition().y, GetPosition().z + 2.0f),
        shdwFront.x, shdwFront.y,
        shdwSide.x, shdwSide.y,
        45, 45, 45,
        7.0f
    );
}

// 0x6E1720
void CVehicle::DoHeadLightReflection(CMatrix& matrix, uint32 flags, bool left, bool right) {
    if (flags & 1) {
        if (left && right) {
            DoHeadLightReflectionTwin(matrix);
        } else if (left || right) {
            DoHeadLightReflectionSingle(matrix, right);
        }
    } else {
        // Como el original: fuera del Combine siempre dibuja el reflejo derecho (no comprueba right)
        if (m_nModelIndex == MODEL_COMBINE) {
            DoHeadLightReflectionTwin(matrix);
        } else {
            DoHeadLightReflectionSingle(matrix, true);
        }
    }
}

// 0x6E1780
bool CVehicle::DoTailLightEffect(int32 lightId, CMatrix& matrix, uint8 isRight, uint8 disabledOrAlarm, uint32 unused, uint8 staticEmission) {
    // Como el original: el Stallion (0x1B7) y el Sabre (0x1DB) con el parachoques trasero danado (panel 6) no tienen luz trasera
    if (m_nModelIndex == MODEL_STALLION || m_nModelIndex == MODEL_SABRE) {
        if (AsAutomobile()->m_damageManager.GetPanelStatus(REAR_BUMPER) != 0) {
            return false;
        }
    }

    // Como el original: la posicion del dummy es el elemento 2 * lightId + 1 del modelo; con lightId == 1 y un vector exactamente nulo no se dibuja nada
    const auto& dummy = GetVehicleModelInfo()->GetModelDummyPosition(static_cast<eVehicleDummy>(2 * lightId + 1));
    if (lightId == 1 && dummy.x == 0.0f && dummy.y == 0.0f && dummy.z == 0.0f) {
        return false;
    }

    // Como el original: la geometria es aritmetica x87 (CMatrix * CVector de 0x59C890, direccion a la camara con 0x59C970 y producto escalar con el trasero de la matriz)
    float offset[3], magnitude, dot;
    VehHeadTailTailGeometryCpp(&dummy.x, isRight == 0, reinterpret_cast<const float*>(&matrix), &TheCamera.GetPosition().x, offset, &magnitude, &dot);

    if (disabledOrAlarm != 0 || !(dot > 0.0f)) {
        return false;
    }
    // En primera persona el jugador no ve las coronas de su propio vehiculo
    if (CCamera::GetActiveCamera().m_nMode == MODE_1STPERSON && this == FindPlayerVehicle(-1, false)) {
        return false;
    }

    // Como el original: un tren que no es el tranvia (subtipo 6, modelo != 0x1C1) usa otro alfa y otro tamano. El rojo es 128 * alfa con el freno apretado (sin freno de mano y con conductor),
    // 96 * alfa con staticEmission y 0 en otro caso; el resultado es true solo en los dos primeros casos
    const bool isTrain = m_nVehicleSubType == VEHICLE_TYPE_TRAIN && m_nModelIndex != MODEL_TRAM;
    const int  mode    = (m_BrakePedal > 0.0f && !vehicleFlags.bIsHandbrakeOn && m_pDriver) ? 1 : (staticEmission ? 2 : 0);
    float      size, range;
    uint32     red;
    VehHeadTailTailCoronaCpp(magnitude, dot, TheCamera.m_fLODDistMultiplier, isTrain, mode, &size, &range, &red);
    const float   sqrtDot = std::sqrt(dot);
    const CVector position{ offset[0], offset[1], offset[2] };

    // Como el original: el id de la corona es la direccion del vehiculo + lado + 2 * indice + 8
    CCoronas::RegisterCorona(
        static_cast<uint32>(reinterpret_cast<uintptr_t>(this)) + isRight + static_cast<uint32>(lightId) * 2 + 8, this,
        static_cast<uint8>(red), 0, 0, 0x80,
        position, size, range,
        CORONATYPE_HEADLIGHT, FLARETYPE_NONE, CORREFL_SIMPLE, LOSCHECK_OFF, TRAIL_OFF,
        sqrtDot, false, 0.5f, false, 15.0f, false, false
    );
    return mode != 0;
}

// 0x6E1A60
void CVehicle::DoVehicleLights(CMatrix& matrix, eVehicleLightsFlags flags) {
    // Como el original (0x6E1A60, 3737 bytes, thiscall con ret 8): las comparaciones y la aritmetica x87 del EXE estan en bloques de X87Math.cpp (VehLights*, VehHeadTail*) que dejan las
    // banderas, C0-C3 y los registros muertos como el EXE; el resto (banderas, ramas, llamadas a los colaboradores) se conserva en este orden.
    if (ms_forceVehicleLightsOff) {
        return;
    }

    // Luces automaticas: el dia, la hora y la lluvia deciden (GetVehicleLightsStatus); un vehiculo abandonado lejos de la camara las apaga y uno destrozado no cambia
    const bool lightsStatus = GetVehicleLightsStatus();
    if (lightsStatus != vehicleFlags.bLightsOn && GetStatus() != STATUS_WRECKED) {
        if (GetStatus() != STATUS_ABANDONED || IsSubTrain()) {
            vehicleFlags.bLightsOn = lightsStatus;
        } else if (vehicleFlags.bLightsOn) {
            if (VehLightsAbandonedFarCpp(&TheCamera.GetPosition().x, &GetPosition().x)) {
                vehicleFlags.bLightsOn = false;
            }
        }
    }

    // Como el original: forzar apagadas limpia la bandera; forzar encendidas la deja en forceLightsOn; la alarma (m_nAlarmState distinto de 0 y 0xFFFF en un vehiculo no destrozado) manda
    // sobre forceLightsOn: parpadea con el bit 8 del tiempo (encendidas con el bit puesto, sin luces con el bit limpio)
    bool forceLightsOn = false, alarmDark = false;
    if (m_nOverrideLights == FORCE_CAR_LIGHTS_OFF) {
        vehicleFlags.bLightsOn = false;
    } else if (m_nOverrideLights == FORCE_CAR_LIGHTS_ON) {
        forceLightsOn = true;
    }
    if (!CanUpdateHornCounter()) {
        forceLightsOn = (CTimer::GetTimeInMS() & 0x100) != 0;
        alarmDark     = !forceLightsOn;
    }

    // ZR-350: los faros escamoteables suben hasta 0.698 rad (40 grados) con las luces encendidas o la alarma y bajan si no; hasta que no estan arriba no se dibuja nada
    if (m_nModelIndex == MODEL_ZR350 && GetStatus() != STATUS_WRECKED && vehicleFlags.bEngineOn) {
        float& propRotate = AsAutomobile()->m_fPropRotate;
        if (vehicleFlags.bLightsOn || forceLightsOn || !CanUpdateHornCounter()) {
            if (VehLightsPropRiseCpp(&CTimer::ms_fTimeStep, &propRotate)) {
                return;
            }
        } else {
            VehLightsPropFallCpp(&CTimer::ms_fTimeStep, &propRotate);
        }
    }

    // Como el original: la mitad baja de m_renderLights se borra siempre; el estado de cada luz (solo en automoviles, el EXE lee el CDamageManager de +0x5A0 sin mas comprobacion) se
    // pide en el orden delantera derecha (1), delantera izquierda (0, solo con TWIN), trasera (3) y trasera izquierda (3 otra vez, solo con TWIN: el EXE usa el mismo indice, no el 2)
    m_nRenderLightsFlags &= 0xF0;
    CAutomobile* const automobile = IsAutomobile() ? AsAutomobile() : nullptr;
    const bool ignoreDamage = (flags & VEHICLE_LIGHTS_IGNORE_DAMAGE) != 0;
    const bool twin         = (flags & VEHICLE_LIGHTS_TWIN) != 0;
    const auto IsLightOk = [&](eLights light) {
        return ignoreDamage || (automobile && automobile->m_damageManager.GetLightStatus(light) == VEHICLE_LIGHT_OK);
    };
    bool okFrontRight = IsLightOk(LIGHT_FRONT_RIGHT);
    bool okFrontLeft  = twin && IsLightOk(LIGHT_FRONT_LEFT);
    bool okRearRight  = IsLightOk(LIGHT_REAR_LEFT);
    bool okRearLeft   = twin && IsLightOk(LIGHT_REAR_LEFT);
    if (flags & VEHICLE_LIGHTS_DISABLE_FRONT) {
        okFrontRight = okFrontLeft = false;
    }
    if (flags & VEHICLE_LIGHTS_DISABLE_REAR) {
        okRearRight = okRearLeft = false;
    }

    if (!vehicleFlags.bEngineOn) {
        return;
    }

    // Luces apagadas, sin forzado ni alarma: solo la parte dinamica de la luz trasera (freno)
    if (!vehicleFlags.bLightsOn && !forceLightsOn && !alarmDark) {
        m_renderLights.m_bRightRear = DoTailLightEffect(0, matrix, 1, !okRearRight, flags, 0);
        if (m_renderLights.m_bRightRear) {
            DoTailLightEffect(1, matrix, 1, !okRearRight, flags, 0);
        }
        if (twin) {
            m_renderLights.m_bLeftRear = DoTailLightEffect(0, matrix, 0, !okRearLeft, flags, 0);
            if (m_renderLights.m_bLeftRear) {
                DoTailLightEffect(1, matrix, 0, !okRearLeft, flags, 0);
            }
        }
        return;
    }

    // Luces encendidas: la luz de cada lado se dibuja "apagada" (disabled) si la alarma esta en su fase oscura o la luz esta rota/desactivada. La segunda luz delantera solo si dibujo la primera
    const bool darkFrontRight = alarmDark || !okFrontRight;
    const bool darkFrontLeft  = alarmDark || !okFrontLeft;
    m_renderLights.m_bRightFront = DoHeadLightEffect(0, matrix, 1, darkFrontRight);
    m_renderLights.m_bLeftFront  = DoHeadLightEffect(0, matrix, 0, darkFrontLeft);
    if (m_renderLights.m_bRightFront) {
        VehicleHeadLightEffect(*this, true, 1, matrix, 1, darkFrontRight);   // copia en linea del EXE (0x6E1F31): no llama a 0x6E0A50
    }
    if (m_renderLights.m_bLeftFront) {
        VehicleHeadLightEffect(*this, true, 1, matrix, 0, darkFrontLeft);    // copia en linea del EXE (0x6E22C2)
    }

    // Parte estatica de la luz trasera
    const bool darkRearRight = alarmDark || !okRearRight;
    m_renderLights.m_bRightRear = DoTailLightEffect(0, matrix, 1, darkRearRight, flags, 1);
    if (m_renderLights.m_bRightRear) {
        DoTailLightEffect(1, matrix, 1, darkRearRight, flags, 1);
    }
    if (twin) {
        const bool darkRearLeft = alarmDark || !okRearLeft;
        m_renderLights.m_bLeftRear = DoTailLightEffect(0, matrix, 0, darkRearLeft, flags, 1);
        if (m_renderLights.m_bLeftRear) {
            DoTailLightEffect(1, matrix, 0, darkRearLeft, flags, 1);
        }
    }

    // Reflejos de los faros en el suelo (no en trenes); con la alarma en su fase oscura se acaba aqui
    if (!IsSubTrain()) {
        if (alarmDark) {
            return;
        }
        DoHeadLightReflection(matrix, flags, okFrontLeft, okFrontRight);
    } else if (alarmDark) {
        return;
    }

    // Luz direccional delantera (la niebla solo si el vehiculo va despacio: |v|^2 < 0.2025) y trasera de frenado
    if (okFrontRight || okFrontLeft) {
        CPointLights::AddLight(PLTYPE_DIRECTIONAL, GetPosition(), matrix.GetForward(), 20.0f, 1.0f, 1.0f, 1.0f, VehLightsSlowCpp(&m_vecMoveSpeed.x) ? 1 : 0, false, nullptr);
    }
    float trailPoint[3];
    VehLightsTrailPointCpp(&GetPosition().x, &matrix.GetForward().x, trailPoint);
    if ((okRearRight || okRearLeft) && VehLightsBrakeCpp(&m_BrakePedal) && !vehicleFlags.bIsHandbrakeOn && m_pDriver) {
        float backward[3];
        VehLightsNegateCpp(&matrix.GetForward().x, backward);
        CPointLights::AddLight(PLTYPE_DIRECTIONAL, CVector{ trailPoint[0], trailPoint[1], trailPoint[2] }, CVector{ backward[0], backward[1], backward[2] }, 10.0f,
                               0.1f, 0.02f, 0.02f, 0, false, this);   // 0x8D368C, 0x8D3690, 0x8D3694
    }
}

// unused
// 0x6E2900
void CVehicle::FillVehicleWithPeds(bool setClothesToAfro) {
    // Como el original (0x6E2900): el jugador se lee de CWorld::Players[0].m_pPed al entrar.
    const auto playerPed = FindPlayerPed(PED_TYPE_PLAYER1);
    eModelID modelId;
    if (setClothesToAfro) {
        CStats::SetStatValue(STAT_FAT, 1000.0f);
        playerPed->GetPlayerData()->m_pPedClothesDesc->SetModel("afro", CLOTHES_MODEL_HEAD);
        CClothes::RebuildPlayer(playerPed, false);
        modelId = MODEL_PLAYER;
    } else {
        modelId = MODEL_WMOST;
    }

    // Como el original: seat -1 es el conductor y 0..m_nMaxPassengers-1 los pasajeros. La comprobacion de que el modelo
    // este cargado va DENTRO del bucle (si no lo esta se pide el modelo y se pasa al siguiente asiento, no se sale), el
    // conductor solo se salta si es el jugador 0 (no si IsPlayer()), el ocupante anterior se borra (destructor virtual con
    // flag 1) despues de crear al nuevo y sin anular el puntero, y el flag de SetPedInCarDirect es true solo en el conductor.
    for (int32 seat = -1; seat < (int32)m_nMaxPassengers; seat++) {
        if (seat == -1 && m_pDriver && m_pDriver == playerPed) {
            continue;
        }
        if (!CStreaming::IsModelLoaded(modelId)) {
            CStreaming::RequestModel(modelId, STREAMING_KEEP_IN_MEMORY);
            continue;
        }
        CPed* const ped = CPopulation::AddPed(PED_TYPE_CIVFEMALE, modelId, GetPosition(), false);
        if (seat == -1) {
            if (m_pDriver) {
                delete m_pDriver;
            }
            CCarEnterExit::SetPedInCarDirect(ped, this, 0, true);
        } else {
            if (m_apPassengers.data()[seat]) {
                delete m_apPassengers.data()[seat];
            }
            const int32 door = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(this, seat);
            CCarEnterExit::SetPedInCarDirect(ped, this, door, false);
        }
    }
}

// 0x6E2E50
bool CVehicle::DoBladeCollision(CVector pos, CMatrix& matrix, int16 rotorType, float radius, float damageMult) {
    static auto& s_TestBladeCol       = StaticRef<CColModel>(0xC1CD38);
    static auto& s_TestBladeColData   = StaticRef<CCollisionData>(0xC1CD68);
    static auto& s_TestBladeColSphere = StaticRef<CColSphere>(0xC1CD98);

    // Set-up collision model for the blade
    {
        CVector bbMin{ pos.x - radius, pos.y - radius, pos.z - radius };
        CVector bbMax{ pos.x + radius, pos.y + radius, pos.z + radius };

        // Como el original: solo los tipos de rotor -3..3 (sin el 0) fijan el grosor del eje; con otros valores no se toca ningun eje
        switch (rotorType) {
        case -1:
        case 1:
            bbMin.x = pos.x - ROTOR_SEMI_THICKNESS;
            bbMax.x = pos.x + ROTOR_SEMI_THICKNESS;
            break;
        case -2:
        case 2:
            bbMin.y = pos.y - ROTOR_SEMI_THICKNESS;
            bbMax.y = pos.y + ROTOR_SEMI_THICKNESS;
            break;
        case -3:
        case 3:
            bbMin.z = pos.z - ROTOR_SEMI_THICKNESS;
            bbMax.z = pos.z + ROTOR_SEMI_THICKNESS;
            break;
        default:
            break;
        }

        s_TestBladeCol.m_boundBox.Set(bbMin, bbMax);
        s_TestBladeCol.m_boundSphere.Set(radius, pos);
        s_TestBladeCol.m_pColData = &s_TestBladeColData;

        s_TestBladeColSphere.Set(radius, pos, SURFACE_DEFAULT);

        s_TestBladeColData.m_pSpheres = &s_TestBladeColSphere;
        s_TestBladeColData.m_nNumSpheres = 1;
    }

    bool collided = false;

    // Como el original: la posicion se transforma con la matriz recibida (no con la del vehiculo; 0x59C890), el indice de sector es
    // floor(x * 0.02 + 60) (no x / 50 + 60) y el rango se recorta a 0..120 antes de avanzar el codigo de escaneo
    const auto center = VehEMatPoint(matrix, pos);
    const auto SectorOf = [](float coord) -> int32 {
        return static_cast<int32>(static_cast<int64>(std::floor(static_cast<double>((coord * 0.02f) + 60.0f))));
    };
    int32 sectorXMin = SectorOf(center.x - radius);
    sectorXMin       = sectorXMin > 0 ? sectorXMin : 0;
    int32 sectorYMin = SectorOf(center.y - radius);
    sectorYMin       = sectorYMin > 0 ? sectorYMin : 0;
    int32 sectorXMax = SectorOf(center.x + radius);
    sectorXMax       = sectorXMax < 120 ? sectorXMax : 120;
    int32 sectorYMax = SectorOf(center.y + radius);
    sectorYMax       = sectorYMax < 120 ? sectorYMax : 120;

    CWorld::AdvanceCurrentScanCode();

    for (int32 y = sectorYMin; y <= sectorYMax; y++) {
        for (int32 x = sectorXMin; x <= sectorXMax; x++) {
            // Como el original: GetSector acota a 0..119 y GetRepeatSector usa x & 15 / y & 15 (siempre >= 0 aqui)
            auto& s  = CWorld::GetSector(x, y);
            auto& rs = CWorld::GetRepeatSector(x, y);
            collided |= BladeColSectorList(s.Buildings, s_TestBladeCol, matrix, rotorType, damageMult);
            collided |= BladeColSectorList(rs.Vehicles, s_TestBladeCol, matrix, rotorType, damageMult);
            collided |= BladeColSectorList(rs.Peds, s_TestBladeCol, matrix, rotorType, 0.0f);
            collided |= BladeColSectorList(rs.Objects, s_TestBladeCol, matrix, rotorType, damageMult);
        }
    }

    // Como el original: tambien se anula m_pSpheres
    s_TestBladeColData.m_pSpheres    = nullptr;
    s_TestBladeColData.m_nNumSpheres = 0;
    s_TestBladeCol.m_pColData        = nullptr;

    return collided;
}

// 0x6E3290
int32 CVehicle::AddVehicleUpgrade(int32 modelId) {
    // Como el original (0x6E3290..0x6E33FC): si SetVehicleUpgradeFlags (0x6D30E0) lo resuelve no toca el modelo; si no, pone el componente como reemplazo (bUsesVehDummy: GetReplacementUpgrade
    // + AddReplacementUpgrade, y con el componente 2 la escala de rueda y los componentes 5, 4 y 7 mas la habilidad todoterreno) o como mejora (GetUpgrade + AddUpgrade), y lo mismo para el
    // modelo enlazado. Al final el primer hueco de m_anUpgrades igual a la mejora sustituida (que se borra del resto) o libre recibe el modelo nuevo.
    // Devuelve la mejora sustituida (-1 si no hay): el EXE la deja en EAX y el llamador de 0x4985DA la guarda en una global (0xA43C78).
    CBaseModelInfo* const upgradeInfo = CModelInfo::ms_modelInfoPtrs[modelId];
    int32 replacedModelIndex = -1;
    if (!SetVehicleUpgradeFlags(modelId, upgradeInfo->CarMod, replacedModelIndex)) {
        const int32 linkedModelIndex = CVehicleModelInfo::ms_linkedUpgrades.FindOtherUpgrade(static_cast<int16>(modelId));
        const int32 carMod = upgradeInfo->CarMod;
        if (upgradeInfo->bUsesVehDummy) {
            replacedModelIndex = GetReplacementUpgrade(carMod);
            AddReplacementUpgrade(modelId, carMod);
            if (carMod == 2) {
                m_fWheelScale = static_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[static_cast<int16>(m_nModelIndex)])->m_fWheelSizeFront;
                AddReplacementUpgrade(modelId, 5);
                AddReplacementUpgrade(modelId, 4);
                AddReplacementUpgrade(modelId, 7);
                if (m_nVehicleSubType == VEHICLE_TYPE_AUTOMOBILE) {
                    if (modelId == static_cast<int32>(static_cast<uint16>(ModelIndices::MI_OFFROAD_WHEEL))) {
                        handlingFlags.bOffroadAbility = true;
                    } else if (!m_pHandlingData->m_bOffroadAbility) {
                        handlingFlags.bOffroadAbility = false;
                    }
                }
            }
        } else {
            replacedModelIndex = GetUpgrade(carMod);
            AddUpgrade(modelId, carMod);
        }

        if (linkedModelIndex != -1) {
            CBaseModelInfo* const linkedInfo = CModelInfo::ms_modelInfoPtrs[linkedModelIndex];
            const int32 linkedCarMod = linkedInfo->CarMod;
            if (linkedInfo->bUsesVehDummy) {
                AddReplacementUpgrade(linkedModelIndex, linkedCarMod);
            } else {
                AddUpgrade(linkedModelIndex, linkedCarMod);
            }
        }
    }

    int32 newValue = modelId;
    for (auto& upgrade : m_anUpgrades) {
        bool store = false;
        if (upgrade == replacedModelIndex) {
            upgrade = -1;
            store   = true;
        } else if (upgrade == -1) {
            store = true;
        }
        if (store) {
            upgrade  = static_cast<int16>(newValue);
            newValue = -1;
        }
    }
    return replacedModelIndex;
}

// 0x6E3400
void CVehicle::SetupUpgradesAfterLoad() {
    for (auto& upgrade : m_anUpgrades) {
        if (upgrade != -1) {
            AddVehicleUpgrade(std::exchange(upgrade, -1));
        }
    }
}

// 0x6E3440
CEntity* CVehicle::GetPlaneWeaponFiringStatus(bool& status, eOrdnanceType& ordnanceType) {
    // Como el original (0x6E3440..0x6E37ED). Siempre pide primero el pad del jugador que conduce (FindPlayerSlotWithVehiclePointer + GetPad) y despues elige por modelo con una tabla de 0x98
    // entradas desde 425 (el indice se compara sin signo: los modelos fuera de 425..576 van al caso por defecto). Los camino que no escriben una de las dos salidas la dejan como estaba
    // (p. ej. el Hunter con L1 escribe solo el bool; con circulo solo el tipo). Devuelve el objetivo en EAX (solo el Hydra con la mira fijada; el resto devuelve nulo).
    // Desplazamientos: +0x4D4 hora de la ultima bengala, +0x9DC y +0x9E0 (avion/helicoptero) hora de inicio de la fijacion y objetivo fijado.
    static_assert(offsetof(CVehicle, m_nGunFiringTime) == 0x4D4);
    static_assert(offsetof(CPlane, field_9DC) == 0x9DC && offsetof(CPlane, field_9E0) == 0x9E0);
    static_assert(WEAPON_FLARE == 58);
    const CPad* const pad = CPad::GetPad(CWorld::FindPlayerSlotWithVehiclePointer(this));
    auto* const lockStart  = reinterpret_cast<uint32*>(reinterpret_cast<uintptr_t>(this) + 0x9DC);
    auto* const lockTarget = reinterpret_cast<CEntity**>(reinterpret_cast<uintptr_t>(this) + 0x9E0);

    // Tabla del EXE (0x6E3804): 0 Hunter y 576, 1 helicopteros ligeros y avion RC, 2 Rustler, 3 Hydra, 4 resto
    enum { KIND_HUNTER = 0, KIND_GUNS = 1, KIND_RUSTLER = 2, KIND_HYDRA = 3, KIND_NONE = 4 };
    const uint32 slot = static_cast<uint32>(static_cast<int32>(static_cast<int16>(m_nModelIndex)) - 425);
    int32 kind = KIND_NONE;
    if (slot <= 0x97) {
        switch (slot + 425) {
        case 425: case 576: kind = KIND_HUNTER; break;
        case 447: case 464: case 487: case 497: case 548: kind = KIND_GUNS; break;
        case 476: kind = KIND_RUSTLER; break;
        case 520: kind = KIND_HYDRA; break;
        default: break;
        }
    }

    switch (kind) {
    case KIND_RUSTLER:
        if (pad->GetCarGunFired() == 2) {
            status = true;
        }
        ordnanceType = 0;
        return nullptr;

    case KIND_HUNTER:
        if (pad->GetCarGunFired() == 2) {
            status = true;
            return nullptr;
        }
        if (pad->CarGunJustDown() == 1) {
            ordnanceType = 1;
            return nullptr;
        }
        break;

    case KIND_GUNS:
        if (pad->GetCarGunFired() == 2) {
            status = true;
        }
        // RC Baron: con el raton en tercera persona y la bandera global, el boton de disparo normal tambien dispara (se vuelve a leer el pad)
        if (static_cast<int16>(m_nModelIndex) == MODEL_RCBARON && StaticRef<uint8>(0xC1CC00) != 0 && StaticRef<uint8>(0xB6EC2E) != 0) {
            if (pad->GetCarGunFired() == 1) {
                status = true;
            }
        }
        ordnanceType = 0;
        return nullptr;

    case KIND_HYDRA: {
        ordnanceType = 0;
        const uint32 now = CTimer::GetTimeInMS();
        if (pad->CarGunJustDown() == 1 && CTimer::GetTimeInMS() > m_nGunFiringTime + 2000u) {
            // Bengala: posicion (matriz + 0x30, o la del placeable) + adelante * -2.5 + arriba * -1.0 en la x87 del EXE (X87Math); la matriz se usa sin comprobar nulo (+0x10 adelante, +0x20 arriba)
            const uintptr_t matrix = reinterpret_cast<uintptr_t>(m_matrix);
            const float* const position = m_matrix ? reinterpret_cast<const float*>(matrix + 0x30) : reinterpret_cast<const float*>(&m_placement.m_vPosn);
            CVector spawn;
            PlaneFiring74FlareCpp(reinterpret_cast<const float*>(matrix + 0x10), reinterpret_cast<const float*>(matrix + 0x20), position, reinterpret_cast<float*>(&spawn));
            CProjectileInfo::AddProjectile(this, WEAPON_FLARE, spawn, 0.0f, reinterpret_cast<const CVector*>(matrix + 0x10), nullptr);
            m_nGunFiringTime = CTimer::GetTimeInMS();
        }

        auto& crossHair = gCrossHair[0];
        if (pad->CarGunJustDown() == 2) {
            // L1: dispara el misil guiado si la mira esta fijada y el objetivo se mantiene mas de 1500 ms; si no, el sin guia
            if (CWeaponEffects::IsLockedOn(0) && *lockStart != 0) {
                CEntity* const target = ScanAndMarkTargetForHeatSeekingMissile(*lockTarget);
                if (target && target == *lockTarget && now - *lockStart > 1500u) {
                    ordnanceType = 2;
                    crossHair.m_color.r = 0xFF;
                    crossHair.m_color.g = 0;
                    crossHair.m_color.b = 0;
                    crossHair.m_fRotation = 1.0f;
                    crossHair.m_nTimeWhenToDeactivate = 0;
                    return target;
                }
            }
            ordnanceType = 1;
            return nullptr;
        }
        if (pad->GetEnterTargeting()) {
            *lockStart  = now;
            *lockTarget = nullptr;
            return nullptr;
        }
        if (pad->GetTarget()) {
            if (*lockStart == 0) {
                *lockStart = now;
            }
            CEntity* const target = ScanAndMarkTargetForHeatSeekingMissile(*lockTarget);
            if (!target || target != *lockTarget) {
                *lockStart = now;
            }
            crossHair.m_nTimeWhenToDeactivate = 0;
            if (now - *lockStart > 1500u) {
                crossHair.m_color.r = 0xFF;
                crossHair.m_color.g = 0;
                crossHair.m_color.b = 0;
                crossHair.m_fRotation = 1.0f;
            } else {
                crossHair.m_color.r = 0xFF;
                crossHair.m_color.g = 0xFF;
                crossHair.m_color.b = 0xFF;
                crossHair.m_fRotation = 0.0f;
            }
            *lockTarget = target;
            return nullptr;
        }
        *lockStart  = 0;
        *lockTarget = nullptr;
        return nullptr;
    }

    default:
        break;
    }

    // Caso por defecto del EXE (0x6E37D3), tambien al que cae el Hunter sin disparo
    status = false;
    ordnanceType = 0;
    return nullptr;
}


// 0x49B010. Como el original: bool en AL; ademas el EXE sale con EAX/ECX/EDX segun el camino (los bancos los comparan en bruto), asi que cada camino
// calcula los tres y termina en ResidueRegsCpp, que es lo ultimo que se ejecuta (ver contrib/audit_register_residue.py, objeto Vehicle). La llamada se
// hace con el tipo de retorno bool (VehicleResidueBoolCpp) para que AL sea el resultado sin instrucciones posteriores. Los colaboradores 0x4C8740/0x4C8750 solo escriben EAX
// (y 0x4C8750 ademas ECX = su segundo argumento) en el EXE, por lo que ECX/EDX de salida son los del EXE original; EDX = infoModelo del vehiculo salvo en la busqueda.
//   - Ruedas (indicador 0x100 y tipo 0x800): bucle sobre GetWheelUpgrade; ECX = indice, o el ultimo indice probado si no se encuentra (o la palabra de indicadores si no hay ruedas).
//   - Lista (resto, salvo tipo 0x4400 sin 0x100): 18 mejoras en infoModelo+0x2D6; ECX = direccion de la entrada, EDX = entrada con signo extendido.
//   - Nitro (tipo 0x4400 sin 0x100): lee tres bytes del vehiculo; ECX = 0x4400 | byte 0x1BE.
// Mismo simbolo que ResidueRegsCpp (X87Math.cpp) con retorno bool: la llamada deja EAX/ECX/EDX y el compilador no anade nada tras ella (llamada directa, no por puntero).
extern "C" bool __fastcall VehicleResidueBoolCpp(uint32 ecxValue, uint32 edxValue, uint32 eaxValue);
#pragma comment(linker, "/alternatename:@VehicleResidueBoolCpp@12=?ResidueRegsCpp@@YIIIII@Z")
#pragma runtime_checks("", off)
__declspec(safebuffers) bool IsValidModForVehicle(uint32 modelId, CVehicle* vehicle) {
    const auto readU8  = [](uint32 address) { return uint32(*reinterpret_cast<const uint8*>(uintptr_t(address))); };
    const auto readU16 = [](uint32 address) { return uint32(*reinterpret_cast<const uint16*>(uintptr_t(address))); };
    const auto readS16 = [](uint32 address) { return uint32(int32(*reinterpret_cast<const int16*>(uintptr_t(address)))); };
    const auto readU32 = [](uint32 address) { return *reinterpret_cast<const uint32*>(uintptr_t(address)); };

    const uint32 veh         = static_cast<uint32>(reinterpret_cast<uintptr_t>(vehicle));
    const uint32 vehicleInfo = readU32(0xA9B0C8u + readS16(veh + 0x22) * 4u); // EDX
    const uint32 modInfo     = readU32(0xA9B0C8u + modelId * 4u);
    const uint32 flags       = readU16(modInfo + 0x12);                       // ECX (movzx)

    uint32 outEcx = flags, outEdx = vehicleInfo, outEax = 0;
    if ((flags & 0x100u) != 0 && (flags & 0x7C00u) == 0x800u) {
        const int32 group = int32(int8(readU8(vehicleInfo + 0x4F)));
        int32 count       = CVehicleModelInfo::GetNumWheelUpgrades(group);
        bool found        = false;
        if (count > 0) {
            for (int32 i = 0;;) {
                const int32 wheelModel = CVehicleModelInfo::GetWheelUpgrade(group, i);
                outEcx                 = uint32(i);
                if (uint32(wheelModel) == modelId) {
                    found = true;
                    break;
                }
                ++i;
                count = CVehicleModelInfo::GetNumWheelUpgrades(group);
                if (!(i < count)) {
                    break;
                }
            }
        }
        outEax = found ? ((modelId & 0xFFFFFF00u) | 1u) : (uint32(count) & 0xFFFFFF00u);
    } else if ((flags & 0x100u) == 0 && (flags & 0x7C00u) == 0x4400u) {
        outEcx = 0x4400u;
        if (readU8(veh + 0x1D3) != 0) {
            outEax = 0;
        } else {
            const uint32 vehicleType = readU8(veh + 0x1BE);
            outEcx                   = 0x4400u | vehicleType;
            outEax                   = (vehicleType != 1 || (readU8(veh + 0x42E) & 0x10u) != 0) ? 1u : 0u;
        }
    } else {
        outEcx = vehicleInfo + 0x2D6u;
        for (uint32 i = 0;;) {
            outEdx = readS16(outEcx);
            if (outEdx == modelId) {
                outEax = 1u; // (indice & ~0xFF) | 1, con indice < 18
                break;
            }
            ++i;
            outEcx += 2;
            if (!(int32(i) < 0x12)) {
                outEax = 0; // (18 & ~0xFF) con AL = 0
                break;
            }
        }
    }
    return VehicleResidueBoolCpp(outEcx, outEdx, outEax);
}
#pragma runtime_checks("", restore)

// 0x6E38F0
bool IsVehiclePointerValid(CVehicle* vehicle) {
    // Como el original (0x6E38F0): indice por division con signo (hacia cero) del desplazamiento respecto al almacenamiento (0xA18 por
    // hueco), sin comprobar que el puntero pertenezca al pool (IsObjectValid si lo hacia); luego lee el vehiculo aunque no sea valido.
    auto* const pool = GetVehiclePool();
    assert(pool);
    const auto idx = pool->GetIndexUnchecked(vehicle);
    if (idx < 0 || idx >= int32(pool->GetSize()) || pool->IsFreeSlotAtIndex(idx)) {
        return false;
    }
    return vehicle->m_nVehicleType == VEHICLE_TYPE_FPLANE || !vehicle->m_pCollisionList.IsEmpty();
}

// 0x6E3950
void CVehicle::ProcessWeapons() {
    // Como el original (0x6E3950). Desplazamientos: +0x594 m_nVehicleSubType, +0x40 banderas fisicas, +0x36 estado (bits 3..7), +0x42C bit 4 bFireGun, +0x513 arma en uso,
    // +0x3BA mision del piloto automatico, +0x41C objetivo del piloto automatico, +0x9DC (avion) y +0xA0C (helicoptero) / +0x9D4 (avion) listas de fogonazos.
    static_assert(offsetof(CVehicle, m_nVehicleSubType) == 0x594 && offsetof(CVehicle, m_nVehicleWeaponInUse) == 0x513);
    static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA && offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_TargetEntity) == 0x41C);
    static_assert(offsetof(CPlane, field_9DC) == 0x9DC && offsetof(CPlane, m_pGunParticles) == 0x9D4 && offsetof(CHeli, m_ppGunflashFx) == 0xA0C);

    CEntity* target = nullptr;

    if (m_nVehicleSubType == VEHICLE_TYPE_PLANE && this == FindPlayerVehicle(-1, false) && !AsPlane()->field_9DC) {
        CWeaponEffects::ClearCrossHairImmediately(0);
    }

    if (m_nPhysicalFlags & PHYSICAL_DESTROYED) {
        return;
    }

    // Como el original: bool y tipo de ordenanza empiezan a cero; solo los escribe GetPlaneWeaponFiringStatus (el bool, un solo byte)
    bool firing = false;
    eOrdnanceType ordnance = 0;
    if (GetStatus() == STATUS_PLAYER || GetStatus() == STATUS_REMOTE_CONTROLLED) {
        target = GetPlaneWeaponFiringStatus(firing, ordnance);
        SelectPlaneWeapon(firing, ordnance);
    } else if (vehicleFlags.bFireGun) {
        firing = true;
        ordnance = 1;
        if (m_nModelIndex == MODEL_HYDRA && m_nVehicleWeaponInUse == CAR_WEAPON_LOCK_ON_ROCKET) {
            ordnance = 2;
        }
    }

    // Como el original: la tabla de saltos solo cubre las armas 1..4
    switch (uint8(m_nVehicleWeaponInUse)) {
    case CAR_WEAPON_HEAVY_GUN:
        if (firing) {
            FirePlaneGuns();
        }
        break;
    case CAR_WEAPON_FREEFALL_BOMB:
        if (ordnance) {
            PossiblyDropFreeFallBombForPlayer(ordnance, true);
        }
        break;
    case CAR_WEAPON_LOCK_ON_ROCKET:
        if (ordnance) {
            if (GetStatus() != STATUS_PLAYER && GetStatus() != STATUS_REMOTE_CONTROLLED) {
                const auto mission = m_autoPilot.m_nCarMission;
                if (mission == MISSION_PLANE_ATTACK_PLAYER || mission == MISSION_PLANE_ATTACK_PLAYER_POLICE || mission == MISSION_PLANE_DOG_FIGHT_PLAYER) {
                    target = FindPlayerVehicle(-1, false);
                } else {
                    target = m_autoPilot.m_TargetEntity;
                }
            }
            if (target) {
                FireHeatSeakingMissile(target, ordnance, true);
            }
        }
        break;
    case CAR_WEAPON_DOUBLE_ROCKET:
        if (ordnance) {
            FireUnguidedMissile(ordnance, true);
        }
        break;
    default:
        break;
    }

    FxSystem_c** gunFx = nullptr;
    if (m_nVehicleSubType == VEHICLE_TYPE_HELI) {
        gunFx = AsHeli()->m_ppGunflashFx;
    } else if (m_nVehicleSubType == VEHICLE_TYPE_PLANE) {
        gunFx = AsPlane()->m_pGunParticles;
    }
    if (!gunFx) {
        return;
    }

    // Como el original: GetPlaneNumGuns una sola vez; el objeto RenderWare se vuelve a leer en cada vuelta y no se comprueba su tipo ni el marco
    const int32 numGuns = GetPlaneNumGuns();
    for (int32 i = 0; i < numGuns; i++) {
        if (FxSystem_c* const fx = gunFx[i]) {
            RwMatrix* matrix = nullptr;
            if (const auto* const rwObject = GetRwObject()) {
                matrix = RwFrameGetMatrix(static_cast<RwFrame*>(rwObject->parent));
            }
            fx->SetMatrix(matrix);
        }
    }
}

// 0x73F400
void CVehicle::DoFixedMachineGuns() {
    if (CCamera::GetActiveCamera().m_nDirectionWasLooking != eLookingDirection::LOOKING_DIRECTION_FORWARD)
        return;

    // Como el original: siempre se pide el mando 0 y, si el conductor es el jugador 2, despues el 1
    auto* driverPad = CPad::GetPad(0);
    if (m_pDriver && m_pDriver->m_nPedType == PED_TYPE_PLAYER2) {
        driverPad = CPad::GetPad(1);
    }
    if (driverPad->GetCarGunFired() && !vehicleFlags.bGunSwitchedOff) {
        FireFixedMachineGuns();
    } else if (CTimer::GetTimeInMS() > m_nGunFiringTime + 1400) {
        m_nAmmoInClip = 20;
    }
}

// 0x73DF00
void CVehicle::FireFixedMachineGuns() {
    plugin::CallMethod<0x73DF00, CVehicle*>(this);
}

// 0x741FD0
void CVehicle::DoDriveByShootings() {
    plugin::CallMethod<0x741FD0, CVehicle*>(this);
}

// NOTSA
bool CVehicle::AreAnyOfPassengersFollowerOfGroup(const CPedGroup& group) {
    return rng::any_of(GetMaxPassengerSeats(), [&](CPed* passenger) {
        return group.GetMembership().IsFollower(passenger);
    });
}

/*!
* @notsa
* @return The index of a passenger, or `std::nullopt` if the given ped isn't a passenger.
*/
auto CVehicle::GetPassengerIndex(const CPed* passenger) const -> std::optional<size_t> {
    const auto passengers = GetPassengers();
    const auto it = rng::find(passengers, passenger);
    if (it == passengers.end()) {
        return std::nullopt;
    }
    return (size_t)rng::distance(passengers.begin(), it);
}

bool CVehicle::IsDriverAPlayer() const {
    return m_pDriver && m_pDriver->IsPlayer();
}
