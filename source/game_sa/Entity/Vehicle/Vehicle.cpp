/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CVehicle
 * Reconstrucción: COMPLETA — 142/142 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 142/142 entradas.
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
    // RH_ScopedInstall(AddReplacementUpgrade, 0x6D3830);
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
    // RH_ScopedInstall(FlyingControl, 0x6D85F0);
    RH_ScopedInstall(BladeColSectorList<CPtrListSingleLink<CEntity*>>, 0x6DAF00);
    RH_ScopedInstall(SetComponentRotation, 0x6DBA30);
    RH_ScopedInstall(SetTransmissionRotation, 0x6DBBB0);
    RH_ScopedInstall(DoBoatSplashes, 0x6DD130);
    // RH_ScopedInstall(DoSunGlare, 0x6DD6F0);
    RH_ScopedInstall(AddWaterSplashParticles, 0x6DDF60);
    RH_ScopedInstall(AddExhaustParticles, 0x6DE240);
    // RH_ScopedInstall(AddSingleWheelParticles, 0x6DE880);
    RH_ScopedInstall(GetSpecialColModel, 0x6DF3D0);
    // RH_ScopedInstall(RemoveVehicleUpgrade, 0x6DF930);
    // RH_ScopedInstall(AddUpgrade, 0x6DFA20);
    // RH_ScopedInstall(UpdateTrailerLink, 0x6DFC50);
    // RH_ScopedInstall(UpdateTractorLink, 0x6E0050);
    // RH_ScopedInstall(ScanAndMarkTargetForHeatSeekingMissile, 0x6E0400);
    // RH_ScopedInstall(FireHeatSeakingMissile, 0x6E05C0);
    // RH_ScopedInstall(PossiblyDropFreeFallBombForPlayer, 0x6E07E0);
    // RH_ScopedInstall(ProcessSirenAndHorn, 0x6E0950);
    RH_ScopedInstall(DoHeadLightEffect, 0x6E0A50);
    RH_ScopedInstall(DoHeadLightReflectionSingle, 0x6E1440);
    RH_ScopedInstall(DoHeadLightReflectionTwin, 0x6E1600);
    RH_ScopedInstall(DoHeadLightReflection, 0x6E1720);
    RH_ScopedInstall(DoTailLightEffect, 0x6E1780);
    // RH_ScopedInstall(DoVehicleLights, 0x6E1A60);
    RH_ScopedInstall(FillVehicleWithPeds, 0x6E2900);
    RH_ScopedInstall(DoBladeCollision, 0x6E2E50);
    // RH_ScopedInstall(AddVehicleUpgrade, 0x6E3290);
    RH_ScopedInstall(SetupUpgradesAfterLoad, 0x6E3400);
    // RH_ScopedInstall(GetPlaneWeaponFiringStatus, 0x6E3440);
    // RH_ScopedInstall(ProcessWeapons, 0x6E3950);
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
    // RH_ScopedGlobalInstall(RemoveUpgradeCB, 0x6D3300);
    // RH_ScopedGlobalInstall(FindUpgradeCB, 0x6D3370);
    RH_ScopedGlobalOverloadedInstall(RemoveObjectsCB, "Object", 0x6D33B0, RwObject*(*)(RwObject*, void*));
    RH_ScopedGlobalOverloadedInstall(RemoveObjectsCB, "Frame", 0x6D3420, RwFrame*(*)(RwFrame*, void*));
    RH_ScopedGlobalInstall(CopyObjectsCB, 0x6D3450);
    // RH_ScopedGlobalInstall(FindReplacementUpgradeCB, 0x6D3490);
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
RpAtomic* RemoveUpgradeCB(RpAtomic* atomic, void* data) {
    return ((RpAtomic * (__cdecl*)(RpAtomic*, void*))0x6D3300)(atomic, data);
}

// 0x6D3370
RpAtomic* FindUpgradeCB(RpAtomic* atomic, void* data) {
    return ((RpAtomic * (__cdecl*)(RpAtomic*, void*))0x6D3370)(atomic, data);
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
    return ((RwObject * (__cdecl*)(RwObject*, void*))0x6D3490)(object, data);
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
    // NOTE: 0x6D38D5 - 0x6D390B => `SetupUpgradeAtomicRendering`

    ((void(__thiscall*)(CVehicle*, int32, int32))0x6D3830)(this, modelIndex, nodeId);
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

// 0x6D85F0
void CVehicle::FlyingControl(eFlightModel flightModel, float leftRightSkid, float steeringUpDown, float steeringLeftRight, float accelerationBreakStatus) {
    return ((void(__thiscall*)(CVehicle*, eFlightModel, float, float, float, float))0x6D85F0)(this, flightModel, leftRightSkid, steeringUpDown, steeringLeftRight, accelerationBreakStatus);

    /*
    * Code below should be correct. I just didn't finish it, because I can't be bothered to figure out what
    * 0x6D8BFD is.. maybe later.
    */
    if (!m_pFlyingHandlingData || CTimer::GetTimeStep() <= 0.f) {
        return;
    }

    const auto padOfPlayerDriver = (GetStatus() == STATUS_PLAYER && m_pDriver && m_pDriver->IsPlayer()) ? m_pDriver->AsPlayer()->GetPadFromPlayer() : nullptr;
    const auto windDragForce     = IsMissionVehicle() ? CVector{} : -CWeather::WindDir * m_pFlyingHandlingData->m_fWindMult;
    const auto velocityWithDrag = m_vecMoveSpeed + windDragForce; // Plus because `windDragForce` is opposing to our velocity already

    switch (flightModel) {
    case FLIGHT_MODEL_UNK:
    case FLIGHT_MODEL_UNK2: {
        NOTSA_UNREACHABLE("The function doesn't seem to be called with this model from anywhere, but it is present in the code.. So I wont bother");
        return;
    }
    case FLIGHT_MODEL_BARON:
    case FLIGHT_MODEL_PLANE:
    case FLIGHT_MODEL_BOAT: {
        if (leftRightSkid == -9999.9902f) { // What a weird fucking value lol
            leftRightSkid = 0.f;
            if (padOfPlayerDriver) {
                leftRightSkid = (float)padOfPlayerDriver->GetSteeringLeftRight() / 128.f;
            }
        }
        if (steeringUpDown == -9999.9902f) {
            steeringUpDown = 0.f;
            if (padOfPlayerDriver) {
                steeringUpDown = (float)padOfPlayerDriver->GetSteeringUpDown() / 128.f;
                const auto gunUpDown = padOfPlayerDriver->GetCarGunUpDown();
                if (std::abs((float)gunUpDown) > 1.f) {
                    steeringUpDown = -gunUpDown / 128.f;
                }
            }
        }
        break;
    }
    }
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
    ((void(__thiscall*)(CVehicle*, int32))0x6DF930)(this, upgradeModelIndex);
}

// 0x6DFA20
void CVehicle::AddUpgrade(int32 modelIndex, int32 upgradeIndex) {
    ((void(__thiscall*)(CVehicle*, int32, int32))0x6DFA20)(this, modelIndex, upgradeIndex);
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
    return ((CEntity * (__thiscall*)(CVehicle*, CEntity*))0x6E0400)(this, entity);
}

// 0x6E05C0
void CVehicle::FireHeatSeakingMissile(CEntity* targetEntity, eOrdnanceType type, bool arg2) {
    ((void(__thiscall*)(CVehicle*, CEntity*, eOrdnanceType, bool))0x6E05C0)(this, targetEntity, type, arg2);
}

// 0x6E07E0
void CVehicle::PossiblyDropFreeFallBombForPlayer(eOrdnanceType type, bool arg1) {
    ((void(__thiscall*)(CVehicle*, eOrdnanceType, bool))0x6E07E0)(this, type, arg1);
}

// 0x6E0950
void CVehicle::ProcessSirenAndHorn(bool arg0) {
    ((void(__thiscall*)(CVehicle*, bool))0x6E0950)(this, arg0);
}

// 0x6E0A50
bool CVehicle::DoHeadLightEffect(int32 lightId, CMatrix& vehicleMatrix, uint8 isRight, uint8 disabledOrAlarm) {
    // Como el original: la posicion del dummy es el elemento 2 * lightId del modelo; con lightId == 1 y un vector exactamente nulo (comparacion ordenada: un NaN no cuenta como cero) no se dibuja nada
    const auto& dummy = GetVehicleModelInfo()->GetModelDummyPosition(static_cast<eVehicleDummy>(2 * lightId));
    if (lightId == 1 && dummy.x == 0.0f && dummy.y == 0.0f && dummy.z == 0.0f) {
        return false;
    }

    // Como el original: toda la geometria es aritmetica x87 (punto + frente * 0.05, CMatrix * CVector de 0x59C890, direccion a la camara con 0x59C970 y producto escalar con el frente de la matriz)
    float offset[3], magnitude, dot;
    VehHeadTailHeadGeometryCpp(&dummy.x, m_matrix ? &m_matrix->GetForward().x : nullptr, m_placement.m_fHeading, isRight == 0, reinterpret_cast<const float*>(&vehicleMatrix),
                               &TheCamera.GetPosition().x, offset, &magnitude, &dot);

    // Como el original: devuelve "disabledOrAlarm == 0" (salvo la salida del dummy nulo, que devuelve false); la geometria se calcula siempre antes de mirar el argumento
    if (disabledOrAlarm != 0) {
        return false;
    }
    if (!(dot > 0.0f)) {
        return true;
    }
    // En primera persona el jugador no ve las coronas de su propio vehiculo
    if (CCamera::GetActiveCamera().m_nMode == MODE_1STPERSON && this == FindPlayerVehicle(-1, false)) {
        return true;
    }

    // Como el original: un tren que no es el tranvia (subtipo 6, modelo != 0x1C1) usa otro limite, otros tamanos y otro alfa
    const float sqrtDot = std::sqrt(dot);
    const bool  isTrain = m_nVehicleSubType == VEHICLE_TYPE_TRAIN && m_nModelIndex != MODEL_TRAM;
    const bool  halogen = (static_cast<uint32>(m_pHandlingData->m_nHandlingFlags) & VEHICLE_HANDLING_HALOGEN_LIGHTS) != 0;
    float       size, range;
    uint32      red, blue;
    VehHeadTailHeadCoronaCpp(magnitude, sqrtDot, TheCamera.m_fLODDistMultiplier, isTrain, halogen, &size, &range, &red, &blue);
    const CVector position{ offset[0], offset[1], offset[2] };
    // Como el original: el id de la corona es la direccion del vehiculo + lado + 2 * indice (+ 4 la linea de luz)
    const auto id = static_cast<uint32>(reinterpret_cast<uintptr_t>(this)) + isRight + static_cast<uint32>(lightId) * 2;

    if (sqrtDot > (isTrain ? 0.85f : 0.9f) && magnitude < 40.0f) {
        CCoronas::RegisterCorona(
            id + 4, this,
            static_cast<uint8>(halogen ? 0x96 : 0xA0), static_cast<uint8>(halogen ? 0x96 : 0xA0), static_cast<uint8>(halogen ? 0xC3 : 0x8C), 0xFF,
            position, isTrain ? 0.3f : 0.075f, range,
            CORONATYPE_HEADLIGHTLINE, FLARETYPE_NONE, CORREFL_NONE, LOSCHECK_OFF, TRAIL_OFF,
            sqrtDot, false, 0.3f, false, 15.0f, false, false
        );
    }
    CCoronas::RegisterCorona(
        id, this,
        static_cast<uint8>(red), static_cast<uint8>(red), static_cast<uint8>(blue), 0x80,
        position, size, range,
        CORONATYPE_HEADLIGHT, FLARETYPE_NONE, CORREFL_SIMPLE, LOSCHECK_OFF, TRAIL_OFF,
        sqrtDot, false, 0.5f, false, 15.0f, false, false
    );
    return true;
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
    ((void(__thiscall*)(CVehicle*, CMatrix&, uint32))0x6E1A60)(this, matrix, flags);
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
void CVehicle::AddVehicleUpgrade(int32 modelId) {
    ((void(__thiscall*)(CVehicle*, int32))0x6E3290)(this, modelId);
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
void CVehicle::GetPlaneWeaponFiringStatus(bool& status, eOrdnanceType& ordnanceType) {
    ((void(__thiscall*)(CVehicle*, bool&, eOrdnanceType&))0x6E3440)(this, status, ordnanceType);
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
    ((void(__thiscall*)(CVehicle*))0x6E3950)(this);
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
