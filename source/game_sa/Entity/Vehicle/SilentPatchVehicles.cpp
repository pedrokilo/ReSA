/*
 * Correcciones de vehiculos tomadas de SilentPatch SA (The MIT License, Copyright (c) 2024 Adrian Zdanowicz (Silent); ver docs/THIRD_PARTY_NOTICES.md).
 * Origen: SilentPatchSA/VehicleSA.cpp y SilentPatch/SVF.cpp (adaptado a nuestras clases; sin ini ni registro dinamico de caracteristicas).
 */
#include "StdInc.h"

#include "SilentPatchVehicles.h"
#include "Automobile.h"
#include "Plane.h"
#include "Trailer.h"
#include "extensions/SilentPatchConfig.h"
#include "CustomCarPlateMgr.h"
#include "VehicleModelInfo.h"

namespace SilentPatchVehicles {
namespace {
// SilentPatch (MIT, Silent): constantes de VehicleSA.cpp
constexpr float PHOENIX_FLUTTER_PERIOD = 70.0f;
constexpr float PHOENIX_FLUTTER_AMP    = 0.13f;
constexpr float SWEEPER_BRUSH_SPEED    = 0.3f;
constexpr float TWO_PI_F               = 6.28318530717958647692f;

struct FeatureEntry {
    int32   model;
    Feature feature;
};

// SilentPatch (MIT, Silent): specialVehFeatures de SVF.cpp (vehiculos de serie de SA)
constexpr FeatureEntry kStockFeatures[] = {
    { 432, Feature::RHINO_WHEELS },
    { 511, Feature::EXTRA_AILERONS1 }, // Beagle
    { 513, Feature::EXTRA_AILERONS2 }, // Stuntplane
    { 525, Feature::TOWTRUCK_HOOK },
    { 531, Feature::TRACTOR_HOOK },
    { 539, Feature::VORTEX_EXHAUST },
    { 544, Feature::FIRELA_LADDER },
    { 574, Feature::SWEEPER_BRUSHES },
    { 582, Feature::NEWSVAN_DISH },
    { 591, Feature::DOUBLE_TRAILER }, // artict3
    { 603, Feature::PHOENIX_FLUTTER },
};

struct TypoEntry {
    const char* dataName; // nombre que pide el juego
    const char* nodeName; // nombre (con la errata) que tiene el marco del modelo
};

// SilentPatch (MIT, Silent): HierarchyTypoFix::typosAndFixes de SilentPatchSA.cpp
constexpr TypoEntry kTypos[] = {
    { "boat_moving_hi", "boat_moving" },
    { "elevator_r", "elevator" },
    { "misc_a", "misca" },
    { "misc_b", "miscb" },
    { "taillights", "tailights" },
    { "taillights2", "tailights2" },
    { "transmission_f", "transmision_f" },
    { "transmission_r", "transmision_r" },
    { "wheel_lm_dummy", "wheel_lm" },
};

RwFrame* FrameInOriginalClump(int32 modelId, int32 frameId) {
    auto* const mi = CModelInfo::GetModelInfo(modelId);
    if (!mi) {
        return nullptr;
    }
    auto* const clump = mi->GetRpClump();
    return clump ? CClumpModelInfo::GetFrameFromId(clump, frameId) : nullptr;
}

bool AtomicIsRendered(RwFrame* frame) {
    bool visible = false;
    RwFrameForAllObjects(frame, [](RwObject* object, void* data) -> RwObject* {
        if (rwObjectGetFlags(object) & rpATOMICRENDER) {
            *static_cast<bool*>(data) = true;
            return nullptr;
        }
        return object;
    }, &visible);
    return visible;
}

void TrySpawnAndHideWheel(CAutomobile& car, eCarNodes node, uint32 collisionType) {
    RwFrame* const wheelNode = car.m_aCarNodes[node];
    if (!wheelNode || !AtomicIsRendered(wheelNode)) {
        return;
    }
    car.SpawnFlyingComponent(node, collisionType);
    RwFrameForAllObjects(wheelNode, [](RwObject* object, void*) -> RwObject* {
        rwObjectSetFlags(object, 0);
        return object;
    }, nullptr);
}

void CopyRotation(CPlane& plane, size_t src, size_t dest) {
    auto& nodes = plane.m_aCarNodes;
    if (nodes[src] && nodes[dest]) {
        RwMatrix* const lhs       = RwFrameGetMatrix(nodes[dest]);
        const RwMatrix* const rhs = RwFrameGetMatrix(nodes[src]);
        lhs->at    = rhs->at;
        lhs->up    = rhs->up;
        lhs->right = rhs->right;
        RwMatrixUpdate(lhs);
    }
}

bool IsProcessableStatus(CVehicle& car) {
    const auto status = car.GetStatus();
    return status == STATUS_PLAYER || status == STATUS_PHYSICS || status == STATUS_SIMPLE;
}

void ProcessPhoenixBlower(CAutomobile& car, int32 modelId) {
    if (!car.m_aCarNodes[CAR_MISC_A] || !car.vehicleFlags.bEngineOn) {
        return;
    }
    // La matriz original del marco (los cepillos relativos parten siempre de ella)
    if (RwFrame* const origFrame = FrameInOriginalClump(modelId, CAR_MISC_A)) {
        *RwFrameGetMatrix(car.m_aCarNodes[CAR_MISC_A]) = *RwFrameGetMatrix(origFrame);
    }

    float finalAngle = 0.0f;
    float& special   = car.m_fPropRotate; // m_fSpecialComponentAngle de SilentPatch (+0x958)
    if (std::abs(car.m_GasPedal) > 0.0f) {
        if (special < 1.3f) {
            finalAngle = special = std::min(special + 0.1f * CTimer::GetTimeStep(), 1.3f);
        } else {
            finalAngle = special + (std::sin(float(CTimer::GetTimeInMS() % 10000) / PHOENIX_FLUTTER_PERIOD) * PHOENIX_FLUTTER_AMP);
        }
    } else if (special > 0.0f) {
        finalAngle = special = std::max(special - 0.05f * CTimer::GetTimeStep(), 0.0f);
    }
    car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_A], AXIS_X, finalAngle, false);
}

void ProcessSweeper(CAutomobile& car) {
    if (!car.vehicleFlags.bEngineOn) {
        return;
    }
    if (IsProcessableStatus(car)) {
        const float angle = CTimer::GetTimeStep() * SWEEPER_BRUSH_SPEED;
        car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_A], AXIS_Z, angle, false);
        car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_B], AXIS_Z, -angle, false);
    }
}

void ProcessNewsvan(CAutomobile& car) {
    if (IsProcessableStatus(car)) {
        float& orientation = car.m_fDoomVerticalRotation; // m_fGunOrientation de SilentPatch (+0x94C)
        orientation += CTimer::GetTimeStep() * 0.05f;
        if (orientation > TWO_PI_F) {
            orientation -= TWO_PI_F;
        }
        car.SetComponentRotation(car.m_aCarNodes[CAR_MISC_A], AXIS_Z, orientation, true);
    }
}
} // namespace

bool ModelHasFeature(int32 modelId, Feature feature) {
    for (const auto& entry : kStockFeatures) {
        if (entry.model == modelId && entry.feature == feature) {
            return true;
        }
    }
    return false;
}

bool IsHierarchyTypo(const char* dataName, const char* nodeName) {
    for (const auto& typo : kTypos) {
        if (_stricmp(typo.nodeName, nodeName) == 0 && _stricmp(typo.dataName, dataName) == 0) {
            return true;
        }
    }
    return false;
}

void ResetFramesAfterFix(CAutomobile& car) {
    // SilentPatch (MIT, Silent): en lugar de poner la rotacion a identidad (se perdia la orientacion original de piezas como el parachoques del Rumpo
    // o la helice del Bandito), se restaura la matriz del marco original del modelo
    auto* const mi = CModelInfo::GetModelInfo(car.m_nModelIndex);
    auto* const clump = mi ? mi->GetRpClump() : nullptr;
    if (!clump) {
        return;
    }
    for (size_t i = CAR_DOOR_RF; i < CAR_NUM_NODES; i++) {
        RwFrame* const frame = car.m_aCarNodes[i];
        if (!frame) {
            continue;
        }
        if (RwFrame* const origFrame = CClumpModelInfo::GetFrameFromId(clump, int32(i))) {
            *RwFrameGetMatrix(frame) = *RwFrameGetMatrix(origFrame);
            RwMatrixUpdate(RwFrameGetMatrix(frame));
        } else {
            // Igual que el codigo original
            CMatrix mat{ RwFrameGetMatrix(frame), false };
            mat.SetTranslate(mat.GetPosition());
            mat.UpdateRW();
        }
    }
}

void ResetBouncingPanelsAfterFix(CAutomobile& car) {
    // SilentPatch (MIT, Silent): solo los 3 paneles (no el 4o: la cadera oscilante); la grua y el tractor conservan el primero (el gancho)
    const int32 modelId = car.m_nModelIndex;
    const bool keepFirst = (car.m_aCarNodes[CAR_MISC_B] && ModelHasFeature(modelId, Feature::TOWTRUCK_HOOK))
                        || (car.m_aCarNodes[17] && ModelHasFeature(modelId, Feature::TRACTOR_HOOK));
    for (size_t i = keepFirst ? 1 : 0; i < 3; i++) {
        car.m_panels[i].m_nFrameId = -1;
    }

    // Las ruedas intermedias del Rhino no se quedan en el estado "extra" tras reparar
    if (ModelHasFeature(modelId, Feature::RHINO_WHEELS)) {
        car.m_doors[DOOR_LEFT_REAR].SetExtraWheelPositions(1.0f, 1.0f, 1.0f, 1.0f);
        car.m_doors[DOOR_RIGHT_REAR].SetExtraWheelPositions(1.0f, 1.0f, 1.0f, 1.0f);
        for (const auto node : { CAR_WHEEL_RM, CAR_WHEEL_LM }) {
            if (car.m_aCarNodes[node]) {
                if (auto* const object = GetFirstObject(car.m_aCarNodes[node])) {
                    RpAtomicSetFlags(reinterpret_cast<RpAtomic*>(object), 0);
                }
            }
        }
    }
}

void ResetBouncingPanelsAfterFix(CPlane& plane) {
    // SilentPatch (MIT, Silent): el Vortex conserva su primer panel
    const size_t first = ModelHasFeature(plane.m_nModelIndex, Feature::VORTEX_EXHAUST) ? 1 : 0;
    for (size_t i = first; i < 3; i++) {
        plane.m_panels[i].m_nFrameId = -1;
    }
}

float EngineComponentStep(const CAutomobile& car, float timeStep) {
    // SilentPatch (MIT, Silent): ms_engineCompSpeed = bEngineOn ? timeStep : 0
    return car.vehicleFlags.bEngineOn ? timeStep : 0.0f;
}

void AfterAutomobilePreRender(CAutomobile& car) {
    const int32 modelId = car.m_nModelIndex;
    if (ModelHasFeature(modelId, Feature::PHOENIX_FLUTTER)) {
        ProcessPhoenixBlower(car, modelId);
    }
    if (ModelHasFeature(modelId, Feature::SWEEPER_BRUSHES)) {
        ProcessSweeper(car);
    }
    if (ModelHasFeature(modelId, Feature::NEWSVAN_DISH)) {
        ProcessNewsvan(car);
    }
}

void AfterPlanePreRender(CPlane& plane) {
    const int32 modelId = plane.m_nModelIndex;
    if (ModelHasFeature(modelId, Feature::EXTRA_AILERONS1)) {
        CopyRotation(plane, 18, 21);
    }
    if (ModelHasFeature(modelId, Feature::EXTRA_AILERONS2)) {
        CopyRotation(plane, 19, 23);
        CopyRotation(plane, 20, 24);
    }
}

void SpawnAndHideDestroyedWheels(CAutomobile& car, uint32 collisionType) {
    auto& dm = car.m_damageManager;
    if (dm.GetWheelStatus(CAR_WHEEL_FRONT_LEFT) == WHEEL_STATUS_MISSING) {
        TrySpawnAndHideWheel(car, CAR_WHEEL_LF, collisionType);
    }
    if (dm.GetWheelStatus(CAR_WHEEL_FRONT_RIGHT) == WHEEL_STATUS_MISSING) {
        TrySpawnAndHideWheel(car, CAR_WHEEL_RF, collisionType);
    }
    // En las ruedas traseras tambien se separa la rueda intermedia (si existe)
    if (dm.GetWheelStatus(CAR_WHEEL_REAR_LEFT) == WHEEL_STATUS_MISSING) {
        TrySpawnAndHideWheel(car, CAR_WHEEL_LM, collisionType);
        TrySpawnAndHideWheel(car, CAR_WHEEL_LB, collisionType);
    }
    if (dm.GetWheelStatus(CAR_WHEEL_REAR_RIGHT) == WHEEL_STATUS_MISSING) {
        TrySpawnAndHideWheel(car, CAR_WHEEL_RM, collisionType);
        TrySpawnAndHideWheel(car, CAR_WHEEL_RB, collisionType);
    }
}

RwFrame* GetTowBarFrame(const CAutomobile& car) {
    RwFrame* towBar = car.m_aCarNodes[CAR_MISC_A];
    if (!towBar) {
        towBar = car.m_aCarNodes[CAR_MISC_B];
    }
    return towBar;
}

bool GetDoubleTrailerTowBarPos(const CTrailer& trailer, CVector& outPos) {
    if (!ModelHasFeature(trailer.m_nModelIndex, Feature::DOUBLE_TRAILER)) {
        return false;
    }
    RwFrame* const hook = trailer.m_aCarNodes[CAR_MISC_B];
    if (!hook) {
        return false;
    }
    outPos = *RwMatrixGetPos(RwFrameGetLTM(hook));
    return true;
}

// ---- Rotores y helices ----
bool RotorBlurEnabled() {
    static const bool enabled = SilentPatchOption("RotorBlur", true);
    return enabled;
}

void ApplyRotorBlur(CAutomobile& vehicle, float rotorSpeed, double speedDivisor) {
    // SilentPatch (MIT, Silent): VehicleSA.cpp CHeli::RenderRotors / CPlane::RenderRotors
    auto& nodes = vehicle.m_aCarNodes;
    // SilentPatch (MIT, Silent): SilentPatchSA.ini excluye por defecto el Maverick (487) y el Skimmer (460) del desenfoque (sus texturas estan corruptas): comportamiento del EXE
    const bool displayRotors   = vehicle.m_nModelIndex != 487 && vehicle.m_nModelIndex != 460;
    const bool hasMovingRotor  = nodes[13] != nullptr && displayRotors;
    const bool hasMovingRotor2 = nodes[15] != nullptr && displayRotors;

    double rotorsSpeed = 0.0;
    if (rotorSpeed > 0.0f) {
        rotorsSpeed = std::min(1.7 * (1.0 / speedDivisor) * double(rotorSpeed), 1.5);
    }
    const double movingSpeed = std::max(rotorsSpeed - 0.4, 0.0);

    const int32 staticAlpha = static_cast<int32>(std::min((1.5 - rotorsSpeed) * 255.0, 255.0));
    const int32 movingAlpha = static_cast<int32>(std::min(movingSpeed * 175.0, 175.0));

    const auto setAlpha = [&](size_t node, int32 alpha) {
        if (nodes[node]) {
            if (auto* const atomic = reinterpret_cast<RpAtomic*>(GetCurrentAtomicObject(nodes[node]))) {
                CVehicle::SetComponentAtomicAlpha(atomic, alpha);
            }
        }
    };
    setAlpha(12, hasMovingRotor ? staticAlpha : 255);
    setAlpha(14, hasMovingRotor2 ? staticAlpha : 255);
    setAlpha(13, hasMovingRotor ? movingAlpha : 0);
    setAlpha(15, hasMovingRotor2 ? movingAlpha : 0);
}

void RenderRotorAtomic(RpAtomic* atomic) {
    int32 savedAlphaRef = 0, savedVertexAlpha = 0;
    RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTIONREF, &savedAlphaRef);
    RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &savedVertexAlpha);
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    AtomicDefaultRenderCallBack(atomic);
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(savedAlphaRef));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(savedVertexAlpha));
}

void RenderPropOrDefaultAtomic(RpAtomic* atomic) {
    if (RotorBlurEnabled() && strstr(GetFrameNodeName(RpAtomicGetFrame(atomic)), "_prop") != nullptr) {
        RenderRotorAtomic(atomic);
    } else {
        AtomicDefaultRenderCallBack(atomic);
    }
}

// ---- Matriculas ----
namespace {
// SilentPatch (MIT, Silent): la unica pieza de estado de las matriculas por vehiculo. El EXE no tiene sitio para el diseno: se guarda en el byte de relleno anterior a m_pCustomCarPlate
int8& PlateDesignOf(CVehicle& vehicle) {
    return *(reinterpret_cast<int8*>(&vehicle.m_pCustomCarPlate) - 1);
}

// Generador local (xorshift32) para no consumir la secuencia aleatoria del juego en la eleccion del diseno (SilentPatch usa su propio rand31)
int32 PlateRandom(int32 from, int32 to) {
    static uint32 state = 0x9E3779B9u ^ static_cast<uint32>(GetTickCount());
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return from + static_cast<int32>(state % static_cast<uint32>(to - from));
}

struct PlateMaterialBackup {
    RpMaterial* material;
    RwTexture*  texture;
};
std::vector<PlateMaterialBackup> g_originalPlateMaterials;
} // namespace

RpMaterial* PlateMaterialPlaceholder() {
    static uint8 dummy{};
    return reinterpret_cast<RpMaterial*>(&dummy);
}

bool CreatePlate(CVehicle& vehicle, CVehicleModelInfo* modelInfo) {
    char plateText[CCustomCarPlateMgr::MAX_TEXT_LENGTH + 1]{};
    if (const char* const overrideText = modelInfo->GetCustomCarPlateText()) {
        strncpy_s(plateText, overrideText, CCustomCarPlateMgr::MAX_TEXT_LENGTH);
    } else {
        CCustomCarPlateMgr::GeneratePlateText(plateText, CCustomCarPlateMgr::MAX_TEXT_LENGTH);
    }
    plateText[CCustomCarPlateMgr::MAX_TEXT_LENGTH] = '\0';

    int8 design;
    if (modelInfo->m_nPlateType != CARPLATE_DEFAULT) {
        design = static_cast<int8>(modelInfo->m_nPlateType);
    } else if (vehicle.IsLawEnforcementVehicle()) {
        design = CCustomCarPlateMgr::GetMapRegionPlateDesign();
    } else {
        design = PlateRandom(0, 20) == 0 ? static_cast<int8>(PlateRandom(0, 3)) : CCustomCarPlateMgr::GetMapRegionPlateDesign();
    }
    PlateDesignOf(vehicle) = design;
    // Mismo diseno para la textura de la matricula y la pieza "carpback" (SilentPatch usaba el tipo del modelo, a veces -1 = region)
    vehicle.m_pCustomCarPlate = CCustomCarPlateMgr::CreatePlateTexture(plateText, static_cast<uint8>(design));

    modelInfo->SetCustomCarPlateText(nullptr);
    modelInfo->m_nPlateType = CARPLATE_DEFAULT;
    return true;
}

void BeforeRenderPlate(CVehicle& vehicle) {
    if (!vehicle.m_pCustomCarPlate || !vehicle.GetRwObject()) {
        return;
    }
    const int8 design = PlateDesignOf(vehicle);
    RpClumpForAllAtomics(reinterpret_cast<RpClump*>(vehicle.GetRwObject()), [](RpAtomic* atomic, void* data) -> RpAtomic* {
        auto& vehicle = *static_cast<CVehicle*>(data);
        RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), [](RpMaterial* material, void* data) -> RpMaterial* {
            auto& vehicle = *static_cast<CVehicle*>(data);
            if (RwTexture* const texture = RpMaterialGetTexture(material)) {
                const char* const name = RwTextureGetName(texture);
                const bool isPlate = _stricmp(name, "carplate") == 0;
                const bool isBack  = !isPlate && _stricmp(name, "carpback") == 0;
                if (isPlate || isBack) {
                    RwTextureAddRef(texture); // se devuelve en AfterRenderPlate (evita que RpMaterialSetTexture lo destruya)
                    g_originalPlateMaterials.push_back({ material, texture });
                    if (isPlate) {
                        RpMaterialSetTexture(material, vehicle.m_pCustomCarPlate);
                    } else {
                        CCustomCarPlateMgr::SetupMaterialPlatebackTexture(material, static_cast<uint8>(PlateDesignOf(vehicle)));
                    }
                }
            }
            return material;
        }, &vehicle);
        return atomic;
    }, &vehicle);
    (void)design;
}

void AfterRenderPlate(CVehicle&) {
    for (const auto& backup : g_originalPlateMaterials) {
        RpMaterialSetTexture(backup.material, backup.texture);
        RwTextureDestroy(backup.texture);
    }
    g_originalPlateMaterials.clear();
}

} // namespace SilentPatchVehicles
