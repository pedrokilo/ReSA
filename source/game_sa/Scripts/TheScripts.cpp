/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTheScripts
 * Reconstrucción: COMPLETA — 69/69 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 69/69 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TheScripts.h"
#include "UpsideDownCarCheck.h"
#include "Lines.h"
#include "FireManager.h"
#include "PedGroups.h"
#include "Checkpoint.h"
#include "Checkpoints.h"
#include "LoadingScreen.h"
#include "Scripted2dEffects.h"
#include "Shadows.h"
#include "VehicleRecording.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskComplexWander.h"

#include "extensions/File.hpp"
#include "X87Math.h"

static inline auto& ScriptsArray = StaticRef<std::array<CRunningScript, MAX_NUM_SCRIPTS>>(0xA8B430);

// Como el original: la variable estatica local de ClearSpaceForMissionEntity (0x486B00) vive en el EXE:
// 32 CColPoint (0x580 bytes) en 0xA951F8; su guarda de inicializacion es el byte 0xA95778.
static inline auto& ClearSpaceColPoints = StaticRef<std::array<CColPoint, 32>>(0xA951F8);

namespace {
//! Pone a nullptr una referencia sin llamar a CleanUpOldReference (el EXE hace `mov [ref], 0`)
void RawNullEntityRef(notsa::EntityRef<>& ref) {
    *reinterpret_cast<CEntity**>(std::addressof(ref)) = nullptr; // OJO: EntityRef::operator& devuelve el puntero, no la direccion del miembro
}
}

void CTheScripts::InjectHooks() {
#ifndef NOTSA_STANDALONE
    // Has to have these, because there seems to be something going on with the variable init order
    // For now I just changed it to use static addresses, not sure whats going on..
    assert((void*)0xA49960 == (void*)&MainSCMBlock[0]);
    assert((void*)(0xA7A6A0) == (void*)&MissionBlock[0]);
#endif

    RH_ScopedClass(CTheScripts);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Init, 0x468D50);
    RH_ScopedInstall(InitialiseAllConnectLodObjects, 0x470960);
    RH_ScopedInstall(InitialiseConnectLodObjects, 0x470940);
    RH_ScopedInstall(InitialiseSpecialAnimGroupsAttachedToCharModels, 0x474730);
    RH_ScopedInstall(InitialiseSpecialAnimGroup, 0x474710);
    RH_ScopedInstall(ReadObjectNamesFromScript, 0x486720);
    RH_ScopedInstall(UpdateObjectIndices, 0x486780);
    RH_ScopedInstall(ReadMultiScriptFileOffsetsFromScript, 0x4867C0);
    RH_ScopedInstall(AddScriptCheckpoint, 0x4935A0);
    RH_ScopedInstall(AddScriptEffectSystem, 0x492F90);
    RH_ScopedInstall(AddScriptSearchLight, 0x493000);
    RH_ScopedInstall(AddScriptSphere, 0x483B30);
    RH_ScopedInstall(AddToBuildingSwapArray, 0x481140);
    RH_ScopedInstall(AddToInvisibilitySwapArray, 0x481200);
    RH_ScopedInstall(AddToListOfConnectedLodObjects, 0x470980);
    RH_ScopedInstall(AddToListOfSpecialAnimGroupsAttachedToCharModels, 0x474750);
    RH_ScopedInstall(AddToSwitchJumpTable, 0x470390);
    RH_ScopedInstall(AddToVehicleModelsBlockedByScript, 0x46B200);
    RH_ScopedInstall(AddToWaitingForScriptBrainArray, 0x46AB60);
    RH_ScopedInstall(CleanUpThisObject, 0x4866C0);
    RH_ScopedInstall(CleanUpThisPed, 0x486300);
    RH_ScopedInstall(CleanUpThisVehicle, 0x486670);
    RH_ScopedInstall(ClearAllVehicleModelsBlockedByScript, 0x46A840);
    RH_ScopedInstall(ClearAllSuppressedCarModels, 0x46A7C0);
    RH_ScopedInstall(ClearSpaceForMissionEntity, 0x486B00);
    RH_ScopedInstall(DoScriptSetupAfterPoolsHaveLoaded, 0x5D3390);
    RH_ScopedInstall(GetActualScriptThingIndex, 0x4839A0);
    RH_ScopedInstall(GetNewUniqueScriptThingIndex, 0x483720);
    RH_ScopedInstall(GetScriptIndexFromPointer, 0x464D20);
    RH_ScopedInstall(GetUniqueScriptThingIndex, 0x4810C0);
    RH_ScopedInstall(ReinitialiseSwitchStatementData, 0x470370);
    // RH_ScopedInstall(RemoveFromVehicleModelsBlockedByScript, 0x0);
    RH_ScopedInstall(RemoveFromWaitingForScriptBrainArray, 0x46ABC0);
    RH_ScopedInstall(RemoveScriptCheckpoint, 0x4936C0);
    RH_ScopedInstall(RemoveScriptEffectSystem, 0x492FD0);
    RH_ScopedInstall(RemoveScriptSearchLight, 0x493160);
    RH_ScopedInstall(RemoveScriptSphere, 0x483BA0);
    RH_ScopedInstall(RemoveScriptTextureDictionary, 0x465A40);
    RH_ScopedInstall(RemoveThisPed, 0x486240);
    RH_ScopedOverloadedInstall(StartNewScript, "last-idle", 0x464C20, CRunningScript* (*)(uint8*));
    RH_ScopedOverloadedInstall(StartNewScript, "indexed", 0x464C90, CRunningScript* (*)(uint8*, uint16));
    RH_ScopedInstall(UndoBuildingSwaps, 0x481290);
    RH_ScopedInstall(IsPedStopped, 0x486110);
    RH_ScopedInstall(IsPlayerOnAMission, 0x464D50);
    RH_ScopedInstall(IsVehicleStopped, 0x4861F0);
    RH_ScopedInstall(Load, 0x5D4FD0);
    RH_ScopedInstall(Save, 0x5D4C40);
    RH_ScopedInstall(WipeLocalVariableMemoryForMissionScript, 0x464BB0);
    RH_ScopedInstall(HasCarModelBeenSuppressed, 0x46A810);
    RH_ScopedInstall(HasVehicleModelBeenBlockedByScript, 0x46A890);
    RH_ScopedInstall(StartTestScript, 0x464D40);
    RH_ScopedInstall(Process, 0x46A000);
    RH_ScopedInstall(ProcessAllSearchLights, 0x4939F0);
    RH_ScopedInstall(ProcessWaitingForScriptBrainArray, 0x46CF00);
    RH_ScopedInstall(UndoEntityInvisibilitySettings, 0x4812D0);
    RH_ScopedInstall(PrintListSizes, 0x4646D0);
    RH_ScopedOverloadedInstall(DrawDebugSquare, "", 0x486840, void(*)(float,float,float,float));
    RH_ScopedInstall(DrawDebugAngledSquare, 0x486990);
    // RH_ScopedInstall(DrawDebugCube, 0x0);
    // RH_ScopedInstall(DrawDebugAngledCube, 0x0)
    RH_ScopedInstall(DrawScriptSpritesAndRectangles, 0x464980);
    RH_ScopedInstall(ScriptDebugCircle2D, 0x485C20);
    RH_ScopedInstall(DrawScriptSpheres, 0x4810E0);
    RH_ScopedOverloadedInstall(HighlightImportantArea, "", 0x485E00, void(*)(uint32,float,float,float,float,float));
    RH_ScopedInstall(HighlightImportantAngledArea, 0x485EF0);
    RH_ScopedInstall(ScriptDebugLine3D, 0x485DE0);
    // RH_ScopedInstall(RenderTheScriptDebugLines, 0x0);
    RH_ScopedInstall(RenderAllSearchLights, 0x493E30);
    RH_ScopedInstall(ScriptConnectLodsFunction, 0x470A20);
    RH_ScopedInstall(ScriptAttachAnimGroupToCharModel, 0x474800);
    RH_ScopedInstall(UseSwitchJumpTable, 0x4703C0);
    RH_ScopedInstall(AttachSearchlightToSearchlightObject, 0x4934F0);
    RH_ScopedInstall(CheckStreamedScriptVersion, 0x464FF0);
}

namespace {
//! Como el original (0x4690A8..0x46916F y 0x46A0A0..0x46A184): valores iniciales de una linea de texto de script.
//! No escribe los 3 bytes de relleno tras HasRightJustify.
void ResetIntroTextLine(tScriptText& t, float wrapX, float centreSize) {
    t.Scale.x          = std::bit_cast<float>(0x3EF5C28Fu);
    t.Scale.y          = std::bit_cast<float>(0x3F8F5C29u);
    t.Color            = CRGBA{ 225, 225, 225, 255 };
    t.Justify          = false;
    t.IsCentered       = false;
    t.HasBg            = false;
    t.HasBgTextOnly    = false;
    t.WrapX            = wrapX;
    t.CentreSize       = centreSize;
    t.BgColor          = CRGBA{ 128, 128, 128, 128 };
    t.IsProportional   = true;
    t.DropShadowColor  = CRGBA{ 0, 0, 0, 255 };
    t.DropShadow       = 2;
    t.TextEdge         = 0;
    t.IsDrawBeforeFade = false;
    t.HasRightJustify  = false;
    t.FontStyle        = FONT_SUBTITLES;
    t.Pos.x            = 0.0f;
    t.Pos.y            = 0.0f;
    std::memset(t.GXTKey, 0, sizeof(t.GXTKey));
    t.NumberToInsert1  = -1;
    t.NumberToInsert2  = -1;
}

//! Como el original (0x4691C8..0x469204): solo escribe estos campos de un rectangulo de script
void ResetIntroRectangle(tScriptRectangle& r) {
    r.m_nType             = eScriptRectangleType::INACTIVE;
    r.m_bDrawBeforeFade   = false;
    r.m_nTextureId        = -1;
    r.cornerA.x           = 0.0f;
    r.cornerA.y           = 0.0f;
    r.cornerB.x           = 0.0f;
    r.cornerB.y           = 0.0f;
    r.m_nAngle            = 0.0f;
    r.m_nTransparentColor = CRGBA{ 255, 255, 255, 255 };
    r.gxt1[0]             = '\0';
    r.m_nTextboxStyle     = 3;
}
}

// 0x468D50
void CTheScripts::Init() {
    rng::fill(ScriptSpace, 0u);
    rng::fill(LocalVariablesForCurrentMission, tScriptParam{});

    pActiveScripts = nullptr;
    CRunningScript* nextScript = nullptr;
    for (auto& script : ScriptsArray) {
        script.Init();
        script.m_pPrev = nullptr;
        script.m_pNext = nextScript;

        if (nextScript) {
            nextScript->m_pPrev = &script;
        }
        nextScript = &script;
    }
    pIdleScripts = nextScript;

    MissionCleanUp.Init();
    UpsideDownCars.Init();
    StuckCars.Init();
    ScriptsForBrains.Init();
    ScriptResourceManager.Initialise();
    // Como el original (0x468E12): `mov [entry], 0` y `mov word [entry+4], -1`, sin CleanUpOldReference ni tocar field_6
    for (auto& wfsb : EntitiesWaitingForScriptBrain) {
        RawNullEntityRef(wfsb.m_pEntity);
        wfsb.m_ScriptBrainIndex = -1;
    }

    if (CGame::bMissionPackGame) {
        char scrFile[MAX_PATH]{};

        while (FrontEndMenuManager.CheckMissionPackValidMenu()) {
            CFileMgr::SetDirMyDocuments();
            notsa::format_to_sz(scrFile, "MPACK//MPACK{:d}//SCR.SCM", CGame::bMissionPackGame);

            if (auto f = notsa::File(scrFile, "rb"); f && f.Read(ScriptSpace.data(), MAIN_SCRIPT_SIZE) >= 1) {
                TheText.Load(false);
                break;
            }
        }
        CFileMgr::SetDir(""); // Como el original (0x468E9F): con paquete de misiones se llama dos veces (0x468E9F y 0x468EE9)
    } else {
        CFileMgr::SetDir("data\\script");

        VERIFY(notsa::File("main.scm", "rb").Read(ScriptSpace.data(), MAIN_SCRIPT_SIZE) >= 1);
    }
    CFileMgr::SetDir("");

    StoreVehicleIndex     = -1;
    StoreVehicleWasRandom = true;
    OnAMissionFlag        = false;
    LastMissionPassedTime = -1;
    LastRandomPedId       = -1;

    rng::fill(UsedObjectArray, tUsedObject{});
    NumberOfUsedObjects = 0; // Como el original (0x468F39): antes de leer los nombres (ReadObjectNamesFromScript fija el valor real)
    ReadObjectNamesFromScript();
    UpdateObjectIndices();
    bAlreadyRunningAMissionScript = false;
    bUsingAMultiScriptFile        = true;
    rng::fill(MultiScriptArray, 0u);
    MainScriptSize                             = 0;
    LargestMissionScriptSize                   = 0;
    NumberOfMissionScripts                     = 0;
    NumberOfExclusiveMissionScripts            = 0;
    LargestNumberOfMissionScriptLocalVariables = 0;

    ReadMultiScriptFileOffsetsFromScript();
    if (!CGame::bMissionPackGame) {
        StreamedScripts.ReadStreamedScriptData();
    }

    ForceRandomCarModel                    = -1;
    FailCurrentMission                     = 0;
    ScriptPickupCycleIndex                 = 0;
    bMiniGameInProgress                    = false;
    bDisplayNonMiniGameHelpMessages        = true;
    bPlayerHasMetDebbieHarry               = false;
    RiotIntensity                          = 0;
    bPlayerIsOffTheMap                     = false;
    RadarZoomValue                         = 0;
    RadarShowBlipOnAllLevels               = false;
    HideAllFrontEndMapBlips                = false;
    bDisplayHud                            = true;
    fCameraHeadingWhenPlayerIsAttached     = 0.0f;
    fCameraHeadingStepWhenPlayerIsAttached = 0.0f;
    bEnableCraneRaise                      = true;
    bEnableCraneLower                      = true;
    bEnableCraneRelease                    = true;
    bDrawCrossHair                         = eCrossHairType::NONE;
    gAllowScriptedFixedCameraCollision     = false;
    bAddNextMessageToPreviousBriefs        = true;
    bScriptHasFadedOut                     = false;
    bDrawOddJobTitleBeforeFade             = true;
    bDrawSubtitlesBeforeFade               = true;

    // Como el original (0x469060): m_f1 queda como estaba
    for (auto& sphere : ScriptSphereArray) {
        sphere.m_vCoords   = CVector{};
        sphere.m_bUsed     = false;
        sphere.m_nUniqueId = 1;
        sphere.m_nId       = 0;
        sphere.m_fRadius   = 0.0f;
    }
    for (auto& line : IntroTextLines) {
        ResetIntroTextLine(line, (float)RsGlobal.maximumWidth, (float)RsGlobal.maximumWidth);
    }

    NumberOfIntroTextLinesThisFrame = 0;
    UseTextCommands                 = eUseTextCommandState::DISABLED;
    bUseMessageFormatting           = false;
    MessageCentre                   = 0;
    MessageWidth                    = 0;

    for (auto& rect : IntroRectangles) {
        ResetIntroRectangle(rect);
    }
    NumberOfIntroRectanglesThisFrame = 0;

    RemoveScriptTextureDictionary();

    rng::fill(BuildingSwapArray, tBuildingSwap{});
    rng::fill(InvisibilitySettingArray, nullptr);
    ClearAllSuppressedCarModels();
    ClearAllVehicleModelsBlockedByScript();
    InitialiseAllConnectLodObjects();
    InitialiseSpecialAnimGroupsAttachedToCharModels();
    // Como el original (0x4692B0): el byte de relleno +1 no se escribe
    for (auto& fx : ScriptEffectSystemArray) {
        fx.m_bUsed     = false;
        fx.m_nId       = 1;
        fx.m_pFxSystem = nullptr;
    }

    // Como el original (0x4692D0): bEnableShadow = true, id = 1; no se tocan el relleno (+6/+7) ni TargetSpot/vf64/vf70 (+0x58..+0x7B)
    for (auto& sl : ScriptSearchLightArray) {
        sl.m_bUsed            = false;
        sl.m_bClipIfColliding = false;
        sl.m_bEnableShadow    = true;
        sl.m_nCurrentState    = eScriptSearchLightState::STATE_0;
        sl.m_SomethingFlag    = false;
        sl.m_nId              = 1;
        sl.m_Origin           = CVector{};
        sl.m_Target           = CVector{};
        sl.m_fTargetRadius    = 0.0f;
        sl.m_fBaseRadius      = 0.0f;
        sl.m_PathCoord1       = CVector{};
        sl.m_PathCoord2       = CVector{};
        sl.m_fPathSpeed       = 0.0f;
        RawNullEntityRef(sl.m_AttachedEntity);
        RawNullEntityRef(sl.m_FollowingEntity);
        RawNullEntityRef(sl.m_Tower);
        RawNullEntityRef(sl.m_Housing);
        RawNullEntityRef(sl.m_Bulb);
    }
    NumberOfScriptSearchLights = 0;

    for (auto& cp : ScriptCheckpointArray) {
        cp.m_bUsed      = false;
        cp.m_nId        = 1;
        cp.m_Checkpoint = nullptr;
    }
    NumberOfScriptCheckpoints = 0;

    for (auto& sq : ScriptSequenceTaskArray) {
        sq.m_bUsed = false;
        sq.m_nId   = 1;
    }

    CScripted2dEffects::Init();
    CTaskSequences::Init();
    CPedGroups::Init();
    CInformFriendsEventQueue::Init();
    CInformGroupEventQueue::Init();
    CDecisionMakerTypes::GetInstance();
}

// 0x470960
void CTheScripts::InitialiseAllConnectLodObjects() {
    for (auto& pair : ScriptConnectLodsObjects) {
        pair = tScriptConnectLodsObject(); // InitialiseConnectLodObjects();
    }
}

// 0x470940
void CTheScripts::InitialiseConnectLodObjects(uint16 index) {
    ScriptConnectLodsObjects.data()[index] = tScriptConnectLodsObject(); // Como el original (0x470940): sin bounds-check
}

// 0x474730
void CTheScripts::InitialiseSpecialAnimGroupsAttachedToCharModels() {
    for (auto& group : ScriptAttachedAnimGroups) {
        // Como el original (0x474735..0x474742): solo escribe m_nModelID = -1 e m_IfpName[0] = 0 (el resto del nombre queda igual)
        group.m_nModelID    = -1;
        group.m_IfpName[0]  = '\0';
    }
}

// 0x474710
void CTheScripts::InitialiseSpecialAnimGroup(uint16 index) {
    // Como el original (0x474710): sin bounds-check y solo escribe m_nModelID e m_IfpName[0]
    auto& group = ScriptAttachedAnimGroups.data()[index];
    group.m_nModelID   = -1;
    group.m_IfpName[0] = '\0';
}

// 0x486720
void CTheScripts::ReadObjectNamesFromScript() {
    auto* usedObjs = GetSCMChunk<tSCMUsedObjectsChunk>();

    // Como el original (0x486720): el contador es un word (movzx) y se copian exactamente 24 bytes por nombre
    NumberOfUsedObjects = (uint16)usedObjs->m_NumberOfUsedObjects;
    for (auto i = 0u; i < NumberOfUsedObjects; i++) {
        std::memcpy(UsedObjectArray.data()[i].szModelName, usedObjs->m_UsedObjectNames[i], sizeof(tSCMUsedObjectsChunk::UsedObjectName));
        UsedObjectArray.data()[i].nModelIndex = 0; // To be updated via UpdateObjectIndices.
    }
}

// 0x486780
void CTheScripts::UpdateObjectIndices() {
    // Como el original (0x486780): solo hasta NumberOfUsedObjects (la primera se ignora porque esta vacia),
    // poniendo antes nModelIndex = -1 (GetModelInfo no lo escribe si el nombre no existe).
    for (uint16 i = 1; NumberOfUsedObjects > i; i++) {
        auto& obj = UsedObjectArray.data()[i];
        obj.nModelIndex = -1;
        CModelInfo::GetModelInfo(obj.szModelName, &obj.nModelIndex);
    }
}

// 0x4867C0
void CTheScripts::ReadMultiScriptFileOffsetsFromScript() {
    auto* sfi = GetSCMChunk<tSCMScriptFileInfoChunk>();

    MainScriptSize                             = sfi->m_MainScriptSize;
    LargestMissionScriptSize                   = sfi->m_LargestMissionScriptSize;
    NumberOfExclusiveMissionScripts            = sfi->m_NumberOfExclusiveMissionScripts;
    NumberOfMissionScripts                     = sfi->m_NumberOfMissionScripts;
    LargestNumberOfMissionScriptLocalVariables = sfi->m_LargestNumberOfMissionScriptLocalVars;

    // Como el original (0x4867FF..0x486832): el contador se compara con signo (movsx)
    for (int32 i = 0; i < (int32)(int16)sfi->m_NumberOfMissionScripts; i++) {
        MultiScriptArray.data()[i] = sfi->m_MissionScriptOffsets[i];
    }
}

// signature changed (CVector)
// 0x4935A0
uint32 CTheScripts::AddScriptCheckpoint(CVector at, CVector pointTo, float radius, eCheckpointType type) {
    const auto cp = rng::find_if(ScriptCheckpointArray, [](auto& cp) { return !cp.IsActive(); });
    assert(cp != ScriptCheckpointArray.end()); // In vanilla game does OOB access.

    cp->m_bUsed = true;
    const auto color = [&type]() -> CRGBA {
        switch (type) {
        case eCheckpointType::TUBE:
        case eCheckpointType::ENDTUBE:
        case eCheckpointType::EMPTYTUBE:
            return { 255, 0, 0, 32 };
        case eCheckpointType::TORUS:
        case eCheckpointType::TORUS_NOFADE:
        case eCheckpointType::TORUSROT:
        case eCheckpointType::TORUSTHROUGH:
        case eCheckpointType::TORUS_UPDOWN:
        case eCheckpointType::TORUS_DOWN:
            return { 255, 0, 0, 96 };
        default:
            NOTSA_UNREACHABLE();
        }
    }();
    const auto index = GetNewUniqueScriptThingIndex(std::distance(ScriptCheckpointArray.begin(), cp), SCRIPT_THING_CHECKPOINT);
    cp->m_Checkpoint = CCheckpoints::PlaceMarker(
        index,
        type,
        at,
        pointTo,
        radius,
        color,
        1024,
        0.075f,
        0
    );

    ++NumberOfScriptCheckpoints;
    return index;
}

// 0x492F90
uint32 CTheScripts::AddScriptEffectSystem(FxSystem_c* system) {
    const auto fx = rng::find_if(ScriptEffectSystemArray, [](auto& fx) {
        return !fx.IsActive();
    });
    assert(fx != ScriptEffectSystemArray.end()); // In vanilla game does OOB access.

    fx->m_bUsed = true;
    fx->m_pFxSystem = system;
    return GetNewUniqueScriptThingIndex(std::distance(ScriptEffectSystemArray.begin(), fx), SCRIPT_THING_EFFECT_SYSTEM);
}

// signature changed (CVector)
// 0x493000
uint32 CTheScripts::AddScriptSearchLight(CVector start, CEntity* entity, CVector target, float targetRadius, float baseRadius) {
    // Como el original (0x493000..0x49314E): primer hueco libre (OOB si estan los 8 ocupados) y escritura campo a campo.
    // No toca m_nId (contador de ids), el relleno ni TargetSpot/vf64/vf70; las referencias se limpian en el orden
    // Following, Attached (+ registro de `entity`), Tower, Housing, Bulb.
    uint32 idx = 0;
    while (idx < ScriptSearchLightArray.size() && ScriptSearchLightArray.data()[idx].IsActive()) {
        ++idx;
    }
    assert(idx < ScriptSearchLightArray.size()); // In vanilla game does OOB access.

    auto& sl = ScriptSearchLightArray.data()[idx];
    sl.m_bClipIfColliding = false;
    sl.m_nCurrentState    = eScriptSearchLightState::STATE_0;
    sl.m_SomethingFlag    = true;
    sl.m_bUsed            = true;
    sl.m_bEnableShadow    = true;
    sl.m_Origin           = start;
    sl.m_Target           = target;
    sl.m_fBaseRadius      = baseRadius;
    sl.m_fTargetRadius    = targetRadius;
    sl.m_PathCoord1       = CVector{};
    sl.m_PathCoord2       = CVector{};
    sl.m_fPathSpeed       = 0.0f;
    sl.m_FollowingEntity  = nullptr;
    sl.m_AttachedEntity   = entity;
    sl.m_Tower            = nullptr;
    sl.m_Housing          = nullptr;
    sl.m_Bulb             = nullptr;
    ++NumberOfScriptSearchLights;

    return GetNewUniqueScriptThingIndex(idx, SCRIPT_THING_SEARCH_LIGHT);
}

// 0x483B30
uint32 CTheScripts::AddScriptSphere(uint32 id, CVector posn, float radius) {
    const auto sphere = rng::find_if(ScriptSphereArray, [](auto& sphere) {
        return !sphere.IsActive();
    });
    assert(sphere != ScriptSphereArray.end()); // In vanilla game does OOB access.

    const auto idx = std::distance(ScriptSphereArray.begin(), sphere);
    sphere->m_nId     = idx + id;
    sphere->m_vCoords = posn;
    sphere->m_bUsed   = true;
    sphere->m_fRadius = radius;
    // Como el original (0x483B95): GetNewUniqueScriptThingIndex(index, 0 = SCRIPT_THING_SPHERE)
    return GetNewUniqueScriptThingIndex(idx, SCRIPT_THING_SPHERE);
}

// 0x481140
void CTheScripts::AddToBuildingSwapArray(CBuilding* building, int32 oldModelId, int32 newModelId) {
    if (building->GetIplIndex())
        return;

    for (auto& swap : BuildingSwapArray) {
        if (swap.m_pCBuilding == building) {
            if (newModelId == swap.m_nOldModelIndex) {
                swap.Clear();
            } else
                swap.m_nNewModelIndex = newModelId;

            return;
        }
    }

    for (auto& swap : BuildingSwapArray) {
        if (!swap.m_pCBuilding) {
            swap = tBuildingSwap(building, newModelId, oldModelId);
            return;
        }
    }
}

// 0x481200
void CTheScripts::AddToInvisibilitySwapArray(CEntity* entity, bool visible) {
    if (entity->GetIplIndex())
        return;

    const auto is = rng::find(InvisibilitySettingArray, entity);
    if (is != InvisibilitySettingArray.end()) {
        // Already exists.
        if (visible) {
            *is = nullptr;
        }
        return;
    }

    // Como el original (0x481249: `cmp byte [esp+10h], 0; jne ret`): si se pide visible y no estaba, no se anade nada
    if (visible) {
        return;
    }

    const auto free = rng::find(InvisibilitySettingArray, nullptr);
    if (free == InvisibilitySettingArray.end()) {
        return;
    }

    *free = entity;
}

// 0x470980
void CTheScripts::AddToListOfConnectedLodObjects(CObject* obj1, CObject* obj2) {
    const auto idx1 = GetObjectPool()->GetRef(obj1), idx2 = GetObjectPool()->GetRef(obj2);

    // Como el original (0x4709D0..0x470A16): recorre las 10 entradas (sale solo si el par ya existe) y se queda con el ULTIMO hueco (a == -1)
    int32 freeIdx = -1;
    for (int32 i = 0; i < (int32)ScriptConnectLodsObjects.size(); i++) {
        const auto& lod = ScriptConnectLodsObjects.data()[i];
        if (lod.a == idx1 && lod.b == idx2) {
            // Already exists.
            return;
        }
        if (lod.a == -1) {
            freeIdx = i;
        }
    }
    if (freeIdx == -1) {
        return; // El original no hace nada si no hay hueco
    }

    ScriptConnectLodsObjects.data()[freeIdx].a = idx1;
    ScriptConnectLodsObjects.data()[freeIdx].b = idx2;
}

// 0x474750
void CTheScripts::AddToListOfSpecialAnimGroupsAttachedToCharModels(int32 modelId, Const char* ifpName) {
    // Como el original (0x474750..0x4747F5): recorre las 8 entradas (sale solo si ya existe el par modelo+nombre) y se queda con el ULTIMO hueco
    int32 freeIdx = -1;
    for (int32 i = 0; i < (int32)ScriptAttachedAnimGroups.size(); i++) {
        const auto& aag = ScriptAttachedAnimGroups.data()[i];
        if (aag.m_nModelID == modelId) {
            if (!std::strcmp(aag.m_IfpName, ifpName)) {
                // Already exists.
                return;
            }
        } else if (aag.m_nModelID == MODEL_INVALID) {
            freeIdx = i;
        }
    }
    if (freeIdx == -1) {
        return;
    }

    auto& free = ScriptAttachedAnimGroups.data()[freeIdx];
    free.m_nModelID = modelId;
    std::strcpy(free.m_IfpName, ifpName);
}

// 0x470390
void CTheScripts::AddToSwitchJumpTable(int32 switchValue, int32 switchLabelLocalAddress) {
    SwitchJumpTable[NumberOfEntriesInSwitchTable].m_nSwitchValue        = switchValue;
    SwitchJumpTable[NumberOfEntriesInSwitchTable].m_nSwitchLabelAddress = switchLabelLocalAddress;
    NumberOfEntriesInSwitchTable++;
}

// 0x46B200, unused | inlined?
void CTheScripts::AddToVehicleModelsBlockedByScript(eModelID modelIndex) {
    if (notsa::contains(VehicleModelsBlockedByScript, modelIndex)) {
        return;
    }

    const auto free = rng::find(VehicleModelsBlockedByScript, MODEL_INVALID);
    assert(free != VehicleModelsBlockedByScript.end()); // In vanilla game does OOB access.

    *free = modelIndex;
}

// 0x46AB60
void CTheScripts::AddToWaitingForScriptBrainArray(CEntity* entity, int16 specialModelIndex) {
    // Como el original (0x46AB60..0x46ABAF): recorre las 150 entradas (sale solo si la entidad ya esta) y se queda con el ULTIMO hueco
    int32 freeIdx = -1;
    for (int32 i = 0; i < (int32)EntitiesWaitingForScriptBrain.size(); i++) {
        CEntity* const cur = EntitiesWaitingForScriptBrain.data()[i].m_pEntity.Get();
        if (!cur) {
            freeIdx = i;
        } else if (cur == entity) {
            // Already exists.
            return;
        }
    }
    if (freeIdx < 0) {
        return;
    }

    auto& free = EntitiesWaitingForScriptBrain.data()[freeIdx];
    free.m_pEntity = entity; // guarda el puntero y llama a RegisterReference (0x46AB9D)
    free.m_ScriptBrainIndex = specialModelIndex;
}

// 0x4934F0
void CTheScripts::AttachSearchlightToSearchlightObject(int32 searchLightId, CObject* tower, CObject* housing, CObject* bulb, CVector offset) {
    const auto idx = GetActualScriptThingIndex(searchLightId, SCRIPT_THING_SEARCH_LIGHT);
    if (idx < 0) {
        return;
    }
    
    auto& sl = ScriptSearchLightArray[idx];
    sl.m_Origin = offset;
    sl.m_AttachedEntity = nullptr;
    sl.m_Tower = tower;
    sl.m_Housing = housing;
    sl.m_Bulb = bulb;
}

// 0x464FF0
bool CTheScripts::CheckStreamedScriptVersion(RwStream* stream, const char* filename) {
    return true;
}

// 0x4866C0
void CTheScripts::CleanUpThisObject(CObject* obj) {
    if (!obj) {
        return;
    }

    if (obj->IsMissionObject()) {
        obj->m_nObjectType                = OBJECT_TEMPORARY;
        obj->m_nRemovalTime               = CTimer::GetTimeInMS() + 20'000'000;
        obj->m_nRefModelIndex             = -1;
        obj->objectFlags.bChangesVehColor = false;
        CObject::nNoTempObjects++;
    }
}

// 0x486300
void CTheScripts::CleanUpThisPed(CPed* ped) {
    // Como el original (0x486328..0x486330: `cmp byte [esi+484h], 2; jne ret`): solo actua sobre peds creados por la mision;
    // el resto sale sin tocar el contador.
    if (!ped || !ped->IsCreatedByMission()) {
        return;
    }

    ped->SetCharCreatedBy(ePedCreatedBy::PED_GAME);

    // Como el original (0x48664E): desde aqui todos los caminos (tambien bKeepTasksAfterCleanUp) decrementan el contador
    notsa::ScopeGuard _([]() {
        --CPopulation::ms_nTotalMissionPeds;
    });

    if (ped->bKeepTasksAfterCleanUp) {
        return;
    }

    const auto RemoveFromGroup = [ped] {
        if (auto* group = CPedGroups::GetPedsGroup(ped)) {
            if (auto& member = group->GetMembership(); member.IsFollower(ped)) {
                member.RemoveMember(ped);
            }
        }
    };

    // Como el original (0x48635B..0x486375): bInVehicle, m_pVehicle != null y el ped es el conductor
    if (auto* veh = ped->GetVehicleIfInOne(); veh && veh->m_pDriver == ped) {
        const auto SetMissionUnlessCrashing = [veh](eCarMission mission) {
            auto& mis = veh->m_autoPilot.m_nCarMission;
            if (mis != MISSION_PLANE_CRASH_AND_BURN && mis != MISSION_HELI_CRASH_AND_BURN) {
                mis = mission;
            }
        };

        // Como el original (0x486383/0x4863E7): se decide por m_nVehicleSubType (+0x594), no por el tipo de entidad
        switch (veh->m_nVehicleSubType) {
        case eVehicleType::VEHICLE_TYPE_HELI: {
            SetMissionUnlessCrashing(MISSION_HELI_FLYTOCOORS);

            veh->m_autoPilot.m_vecDestinationCoors = CVector{ 10'000.0f, -10'000.0f, 1'000.0f };
            veh->AsHeli()->m_fMinAltitude          = 1000.0f;
            veh->AsHeli()->m_fMaxAltitude          = 1000.0f;
            break;
        }
        case eVehicleType::VEHICLE_TYPE_PLANE: {
            SetMissionUnlessCrashing(MISSION_PLANE_FLYTOCOORS);

            veh->m_autoPilot.m_vecDestinationCoors = CVector{ 10'000.0f, 10'000.0f, 1'000.0f };
            veh->AsPlane()->m_minAltitude          = 1000.0f;
            veh->AsPlane()->m_maxAltitude          = 1000.0f;
            break;
        }
        default:
            // Como el original (0x486448): m_nVehicleType (+0x590) automovil (0) o moto (9)
            if (veh->IsAutomobile() || veh->IsBike()) {
                CCarCtrl::JoinCarWithRoadSystem(veh);
                SetMissionUnlessCrashing(MISSION_CRUISE);
            }
        }

        // Quick return: The captain goes down with the ship.
        ped->bStayInSamePlace = false;
        RemoveFromGroup();
        return;
    }
    ped->bStayInSamePlace = false;
    RemoveFromGroup();

    // Como el original (0x4864E3..0x48650B): comprueba el evento EVENT_SCRIPT_COMMAND (con tarea no nula) y la tarea primaria
    const auto CheckTaskExists = [ped](eTaskType type) {
        if (auto* event = ped->GetEventGroup().GetEventOfType(EVENT_SCRIPT_COMMAND)) {
            if (auto* task = static_cast<CEventScriptCommand*>(event)->m_task) {
                if (task->GetTaskType() == type) {
                    return true;
                }
            }
        }

        if (auto* task = ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY); task && task->GetTaskType() == type) {
            return true;
        }

        return false;
    };

    if (ped->IsInVehicle()) {
        if (CheckTaskExists(TASK_COMPLEX_SEQUENCE)) {
            return;
        }

        // Get them out of the car then make them wander.
        // Como el original (0x486522..0x48658F): orden de reserva secuencia, salir del coche, wander; luego AddTask x2
        auto* sequence = new CTaskComplexSequence();
        auto* leaveCar = new CTaskComplexLeaveAnyCar(0, true, false);
        auto* wander   = CTaskComplexWander::GetWanderTaskByPedType(ped);
        sequence->AddTask(leaveCar);
        sequence->AddTask(wander);

        CEventScriptCommand event{ TASK_PRIMARY_PRIMARY, sequence, false };
        ped->GetEventGroup().Add(&event, false);
    } else {
        if (CheckTaskExists(TASK_COMPLEX_WANDER)) {
            return;
        }

        // Make them wander.
        CEventScriptCommand event{ TASK_PRIMARY_PRIMARY, CTaskComplexWander::GetWanderTaskByPedType(ped), false };
        ped->GetEventGroup().Add(&event, false);
    }
}

// 0x486670
void CTheScripts::CleanUpThisVehicle(CVehicle* vehicle) {
    // Como el original (0x486679: `cmp byte [esi+4A4h], 2; jne ret`): solo actua sobre vehiculos creados por la mision
    if (!vehicle || !vehicle->IsCreatedBy(eVehicleCreatedBy::MISSION_VEHICLE)) {
        return;
    }

    vehicle->physicalFlags.bDontApplySpeed        = false;
    vehicle->physicalFlags.bDisableCollisionForce = false;
    vehicle->vehicleFlags.bIsLocked               = false;

    CCarCtrl::RemoveFromInterestingVehicleList(vehicle);
    CVehicleRecording::StopPlaybackRecordedCar(vehicle);
    vehicle->SetVehicleCreatedBy(eVehicleCreatedBy::RANDOM_VEHICLE);
}

// 0x46A840
void CTheScripts::ClearAllVehicleModelsBlockedByScript() {
    rng::fill(VehicleModelsBlockedByScript, MODEL_INVALID);
}

// 0x46A7C0
void CTheScripts::ClearAllSuppressedCarModels() {
    rng::fill(SuppressedVehicleModels, MODEL_INVALID);
}

// 0x486B00
void CTheScripts::ClearSpaceForMissionEntity(const CVector& pos, CEntity* ourEntity) {
    StaticRef<uint8>(0xA95778) |= 1; // Como el original (0x486B0C): guarda de inicializacion de la variable estatica local
    std::array<CEntity*, 16> colEntities{};
    int16                    numColliding{};

    CWorld::FindObjectsKindaColliding(
        pos,
        ourEntity->GetModelInfo()->GetColModel()->GetBoundRadius(),
        false,
        &numColliding,
        (int16)colEntities.max_size(),
        colEntities.data(),
        false,
        true,
        true,
        false,
        false
    );

    if (numColliding <= 0) {
        return;
    }

    // Como el original (0x486B63..0x486B9C): primero se descartan los peds que estan en un vehiculo (solo por la bandera bInVehicle)
    for (uint16 i = 0; i < numColliding; i++) {
        auto* entity = colEntities[i];
        if (entity != ourEntity && entity->GetIsTypePed() && entity->AsPed()->bInVehicle) {
            colEntities[i] = nullptr;
        }
    }

    for (int32 j = 0; j < numColliding; j++) {
        CEntity* const entity = colEntities[j];
        if (entity == ourEntity || !entity) {
            continue;
        }

        // Como el original (0x486BC4..0x486C71): las lineas de suspension (CColData::m_nNumLines, con signo) se desactivan
        // solo durante ProcessColModels de cada entidad y se restauran justo despues.
        auto* ourColModel = ourEntity->GetColModel(); // una sola llamada (0x486BC6); el EXE reutiliza el resultado
        auto* ourColData = ourColModel->m_pColData;
        if (!ourColData) {
            continue;
        }
        const uint8 savedNumLines = ourColData->m_nNumLines;
        if ((int8)savedNumLines > 0) {
            ourColData->m_nNumLines = 0;
        }

        // Como el original: primero la matriz de la otra entidad y luego la nuestra (AllocateMatrix + UpdateMatrix si faltan)
        auto& entityMat = entity->GetMatrix();
        auto& ourMat    = ourEntity->GetMatrix();
        const auto numCollisions = CCollision::ProcessColModels(
            ourMat,
            *ourColModel,
            entityMat,
            *entity->GetColModel(),
            ClearSpaceColPoints,
            nullptr,
            nullptr,
            false
        );

        if ((int8)savedNumLines > 0) {
            ourColData->m_nNumLines = savedNumLines;
        }

        if (numCollisions <= 0) {
            continue;
        }

        if (entity->GetIsTypeVehicle()) {
            auto* vehicle = entity->AsVehicle();
            if (vehicle->vehicleFlags.bIsLocked || !vehicle->CanBeDeleted()) {
                continue;
            }

            if (auto* driver = vehicle->m_pDriver) {
                CPopulation::RemovePed(driver);
                CEntity::SafeCleanUpRef(vehicle->m_pDriver);
                vehicle->m_pDriver = nullptr;
            }

            // Need to use raw pointer here instead of a reference - the m_aPassengers array is modifed in RemovePassenger() and we would crash in RemovePed afterwards
            for (uint32 k = 0; k < vehicle->m_nMaxPassengers; k++) {
                if (auto* passenger = vehicle->m_apPassengers[k]) {
                    vehicle->RemovePassenger(passenger);
                    CPopulation::RemovePed(passenger);
                }
            }

            CCarCtrl::RemoveFromInterestingVehicleList(vehicle);
            CWorld::Remove(vehicle);
            delete vehicle;
        } else if (entity->GetIsTypePed()) {
            if (!entity->AsPed()->IsPlayer() && entity->AsPed()->CanBeDeleted()) {
                CPopulation::RemovePed(entity->AsPed());
            }
        }
    }
}

// 0x5D3390
void CTheScripts::DoScriptSetupAfterPoolsHaveLoaded() {
    for (const auto& lod : ScriptConnectLodsObjects) {
        if (lod.a != -1 && lod.b != -1) {
            ScriptConnectLodsFunction(lod.a, lod.b);
        }
    }
}

// 0x4839A0
int32 CTheScripts::GetActualScriptThingIndex(int32 ref, eScriptThingType type) {
    if (ref == -1) {
        return -1;
    }

    // Como el original (0x4839B4..0x4839BC): indice = word bajo, id = word alto, ambos sin signo; el id guardado
    // (int16 en varias estructuras) se compara tambien como word sin signo (movzx).
    const uint32 idx = (uint16)ref, id = (uint32)ref >> 16;

    switch (type) {
    case SCRIPT_THING_SPHERE:
        if (const auto& s = ScriptSphereArray[idx]; (uint16)s.m_nUniqueId == id && s.IsActive()) {
            return idx;
        }
        break;
    case SCRIPT_THING_EFFECT_SYSTEM:
        if (const auto& fx = ScriptEffectSystemArray[idx]; (uint16)fx.m_nId == id && fx.IsActive()) {
            return idx;
        }
        break;
    case SCRIPT_THING_SEARCH_LIGHT:
        if (const auto& sl = ScriptSearchLightArray[idx]; (uint16)sl.m_nId == id && sl.IsActive()) {
            return idx;
        }
        break;
    case SCRIPT_THING_CHECKPOINT:
        if (const auto& cp = ScriptCheckpointArray[idx]; (uint16)cp.m_nId == id && cp.IsActive()) {
            return idx;
        }
        break;
    case SCRIPT_THING_SEQUENCE_TASK:
        if (const auto& sqt = ScriptSequenceTaskArray[idx]; (uint16)sqt.m_nId == id && sqt.IsActive()) {
            return idx;
        }
        break;
    case SCRIPT_THING_FIRE:
        // Como el original (0x483A66..0x483A9E): id y bit "creado por script" sin exigir que el fuego este activo
        if (const auto& f = gFireManager.m_aFires[idx]; (uint16)f.GetId() == id && f.IsScript()) {
            return idx;
        }
        break;
    case SCRIPT_THING_2D_EFFECT:
        if (CScripted2dEffects::ScriptReferenceIndex[idx] == id && CScripted2dEffects::ms_activated[idx]) {
            return idx;
        }
        break;
    case SCRIPT_THING_DECISION_MAKER:
        CDecisionMakerTypes::GetInstance();
        if (CDecisionMakerTypes::ScriptReferenceIndex[idx] == id) {
            CDecisionMakerTypes::GetInstance();
            if (CDecisionMakerTypes::m_IsActive[idx]) {
                return idx;
            }
        }
        break;
    case SCRIPT_THING_PED_GROUP:
        if (CPedGroups::ScriptReferenceIndex[idx] == id && CPedGroups::ms_activeGroups[idx]) {
            return idx;
        }
        break;
    default:
        break;
    }
    return -1;
}

// 0x483720
int32 CTheScripts::GetNewUniqueScriptThingIndex(int32 index, eScriptThingType type) {
    // Como el original (p. ej. 0x483741..0x48375F): el id es un word sin signo; si ya vale >= 0xFFFE vuelve a 1, si no se incrementa.
    // El resultado es (id << 16) | index.
    const auto NewUniqueId = [index](auto& id) -> int32 {
        const uint16 cur = (uint16)id;
        id = (std::remove_reference_t<decltype(id)>)(cur >= 0xFFFEu ? 1u : cur + 1u);
        return (int32)(((uint32)(uint16)id << 16) | (uint32)index);
    };

    switch (type) {
    case eScriptThingType::SCRIPT_THING_SPHERE:
        return NewUniqueId(ScriptSphereArray[index].m_nUniqueId);
    case eScriptThingType::SCRIPT_THING_EFFECT_SYSTEM:
        return NewUniqueId(ScriptEffectSystemArray[index].m_nId);
    case eScriptThingType::SCRIPT_THING_SEARCH_LIGHT:
        return NewUniqueId(ScriptSearchLightArray[index].m_nId);
    case eScriptThingType::SCRIPT_THING_CHECKPOINT:
        return NewUniqueId(ScriptCheckpointArray[index].m_nId);
    case eScriptThingType::SCRIPT_THING_SEQUENCE_TASK: {
        const auto ret = NewUniqueId(ScriptSequenceTaskArray[index].m_nId);
        ScriptSequenceTaskArray[index].m_bUsed = true;
        return ret;
    }
    case eScriptThingType::SCRIPT_THING_FIRE:
        return NewUniqueId(gFireManager.m_aFires[index].GetId());
    case eScriptThingType::SCRIPT_THING_2D_EFFECT:
        return NewUniqueId(CScripted2dEffects::ScriptReferenceIndex[index]);
    case eScriptThingType::SCRIPT_THING_DECISION_MAKER:
        CDecisionMakerTypes::GetInstance();
        return NewUniqueId(CDecisionMakerTypes::ScriptReferenceIndex[index]);
    case eScriptThingType::SCRIPT_THING_PED_GROUP:
        return NewUniqueId(CPedGroups::ScriptReferenceIndex[index]);
    default:
        break;
    }

    return -1;
}

// 0x464D20
int32 CTheScripts::GetScriptIndexFromPointer(CRunningScript* thread) {
    // Como el original (0x464D20): (puntero - base) / 0xE0 sin comprobar rango
    return (int32)(thread - ScriptsArray.data());
}

/*!
 * @param type always SCRIPT_THING_PED_GROUP
 * @addr 0x4810C0
 */
int32 CTheScripts::GetUniqueScriptThingIndex(int32 playerGroup, eScriptThingType type) {
    // Como el original (0x4810C0): cualquier tipo distinto de 8 devuelve -1
    switch (type) {
    case SCRIPT_THING_PED_GROUP:
        return (int32)((uint32)playerGroup | ((uint32)CPedGroups::ScriptReferenceIndex[playerGroup] << 16));
    default:
        return -1;
    }
}

// 0x470370
void CTheScripts::ReinitialiseSwitchStatementData() {
    NumberOfEntriesStillToReadForSwitch = 0;
    ValueToCheckInSwitchStatement       = 0;
    SwitchDefaultExists                 = false;
    SwitchDefaultAddress                = 0;
    NumberOfEntriesInSwitchTable        = 0;
}

// unused
// 0x?
void CTheScripts::RemoveFromVehicleModelsBlockedByScript(int32 modelIndex) {
    for (auto& model : VehicleModelsBlockedByScript) {
        if (model == modelIndex) {
            model = MODEL_INVALID;
        }
    }
}

// 0x46ABC0
void CTheScripts::RemoveFromWaitingForScriptBrainArray(CEntity* entity, int16 modelIndex) {
    for (auto& bwe : EntitiesWaitingForScriptBrain) {
        if (bwe.m_pEntity.Get() != entity || bwe.m_ScriptBrainIndex != modelIndex) {
            continue;
        }

        bwe.m_pEntity = nullptr; // CleanUpOldReference (0x46ABE8) si habia entidad y luego `mov [entry], 0`
        bwe.m_ScriptBrainIndex = -1;
    }
}

// 0x4936C0
void CTheScripts::RemoveScriptCheckpoint(int32 scriptIndex) {
    const auto i = GetActualScriptThingIndex(scriptIndex, eScriptThingType::SCRIPT_THING_CHECKPOINT);
    if (i == -1) {
        return;
    }

    auto& scp = ScriptCheckpointArray[i];
    scp.m_bUsed = false; // Como el original (0x4936DC): el id (m_nId) NO se toca, es el contador de ids
    if (const auto* cp = scp.m_Checkpoint) {
        CCheckpoints::DeleteCP(cp->m_ID, cp->m_Type.get_underlying());
    }
    --NumberOfScriptCheckpoints;
    scp.m_Checkpoint = nullptr;
}

// 0x492FD0
void CTheScripts::RemoveScriptEffectSystem(int32 scriptIndex) {
    const auto i = GetActualScriptThingIndex(scriptIndex, eScriptThingType::SCRIPT_THING_EFFECT_SYSTEM);
    if (i == -1) {
        return;
    }

    auto& sef = ScriptEffectSystemArray[i];
    sef.m_bUsed = false;
    sef.m_pFxSystem = nullptr;
}

// 0x493160
void CTheScripts::RemoveScriptSearchLight(int32 scriptIndex) {
    const auto i = GetActualScriptThingIndex(scriptIndex, eScriptThingType::SCRIPT_THING_SEARCH_LIGHT);
    if (i == -1) {
        return;
    }

    // Como el original (0x493172..0x493276): campo a campo; m_bEnableShadow queda en true, el flag/estado a 0, y no se tocan
    // m_nId (contador de ids), TargetSpot, vf64 ni vf70. Las referencias se limpian en el orden Following, Attached, Tower, Housing, Bulb.
    auto& sl = ScriptSearchLightArray[i];
    sl.m_bUsed            = false;
    sl.m_bClipIfColliding = false;
    sl.m_bEnableShadow    = true;
    sl.m_nCurrentState    = eScriptSearchLightState::STATE_0;
    sl.m_SomethingFlag    = false;
    sl.m_Origin           = CVector{};
    sl.m_Target           = CVector{};
    sl.m_fTargetRadius    = 0.0f;
    sl.m_fBaseRadius      = 0.0f;
    sl.m_PathCoord1       = CVector{};
    sl.m_PathCoord2       = CVector{};
    sl.m_fPathSpeed       = 0.0f;
    sl.m_FollowingEntity  = nullptr;
    sl.m_AttachedEntity   = nullptr;
    sl.m_Tower            = nullptr;
    sl.m_Housing          = nullptr;
    sl.m_Bulb             = nullptr;
    --NumberOfScriptSearchLights;
}

// 0x483BA0
void CTheScripts::RemoveScriptSphere(int32 scriptIndex) {
    const auto i = GetActualScriptThingIndex(scriptIndex, eScriptThingType::SCRIPT_THING_SPHERE);
    if (i == -1) {
        return;
    }

    auto& ss = ScriptSphereArray[i];
    ss.m_bUsed = false;
    ss.m_nId   = 0;
}

// 0x465A40
void CTheScripts::RemoveScriptTextureDictionary() {
    rng::for_each(ScriptSprites, &CSprite2d::Delete);
    if (const auto slot = CTxdStore::FindTxdSlot("script"); slot != -1) {
        // Como el original (0x465A71..0x465A8F): solo se quita si el TxD existe en el pool y tiene diccionario RW (`cmp [txd], 0`)
        if (const auto* txd = CTxdStore::ms_pTxdPool->GetAt(slot); txd && txd->m_pRwDictionary) {
            CTxdStore::RemoveTxd(slot);
        }
    }
}

// 0x486240
void CTheScripts::RemoveThisPed(CPed* ped) {
    if (!ped) {
        return;
    }

    const auto isMissionChar = ped->GetCreatedBy() == ePedCreatedBy::PED_MISSION;

    if (auto* veh = ped->GetVehicleIfInOne()) {
        if (veh->m_pDriver == ped) {
            veh->RemoveDriver(false);

            // Como el original (0x486284..0x48628D): el vehiculo queda abandonado (m_nStatus = STATUS_ABANDONED)
            veh->SetStatus(STATUS_ABANDONED);

            if (veh->m_nDoorLock == eCarLock::CARLOCK_COP_CAR) {
                veh->m_nDoorLock = eCarLock::CARLOCK_UNLOCKED;
            }

            if (ped->IsCop() && veh->IsLawEnforcementVehicle()) {
                veh->ChangeLawEnforcerState(false);
            }
        } else {
            veh->RemovePassenger(ped);
        }
    }

    CWorld::RemoveReferencesToDeletedObject(ped);
    delete ped;

    if (isMissionChar) {
        --CPopulation::ms_nTotalMissionPeds;
    }
}

// 0x464C20
CRunningScript* CTheScripts::StartNewScript(uint8* startIP) {
    CRunningScript* script = pIdleScripts;

    script->RemoveScriptFromList(&pIdleScripts);
    script->Init();
    script->SetCurrentIp(startIP);
    script->AddScriptToList(&pActiveScripts);
    script->SetActive(true);

    return script;
}

// 0x464C90
CRunningScript* CTheScripts::StartNewScript(uint8* startIP, uint16 index) {
    if (!pIdleScripts) {
        return nullptr;
    }

    auto* script = pIdleScripts;
    while (script && script != &ScriptsArray[index]) {
        script = script->m_pNext;
    }

    if (!script) {
        return nullptr;
    }

    script->RemoveScriptFromList(&pIdleScripts);
    script->Init();
    if (startIP) { // NOTSA/BUGFIX: `IP` is initialized to null already, so no need to set it again (As it causes an assert)
        script->SetCurrentIp(startIP);
    }
    script->AddScriptToList(&pActiveScripts);
    script->SetActive(true);

    return script;
}

// 0x481290
void CTheScripts::UndoBuildingSwaps() {
    for (auto& swap : BuildingSwapArray) {
        if (swap.m_pCBuilding) {
            swap.m_pCBuilding->ReplaceWithNewModel(swap.m_nOldModelIndex);
            swap.Clear();
        }
    }
}

// 0x486110
bool CTheScripts::IsPedStopped(CPed* ped) {
    if (ped->IsInVehicle()) {
        // Como el original (0x48612A..0x486149): `fcompp` + `test ah,1`: verdadero salvo si timeStep * 0.01f < velocidad (o NaN)
        return CTimer::GetTimeStep() * std::bit_cast<float>(0x3C23D70Au) >= ped->m_pVehicle->m_fMovingSpeed;
    }
    // Como el original (0x48614D..0x4861A0): primero los cuatro RpAnimBlendClumpGetAssociation del jugador, en este orden
    if (ped->IsPlayer()) {
        for (const uint32 animId : { (uint32)ANIM_ID_RUN_STOP, (uint32)ANIM_ID_RUN_STOPR, (uint32)ANIM_ID_JUMP_LAUNCH, (uint32)ANIM_ID_JUMP_GLIDE }) {
            if (RpAnimBlendClumpGetAssociation(ped->GetRpClump(), animId)) {
                return false;
            }
        }
    }
    // Como el original (0x4861A0): CPed::IsPedStandingInPlace en linea (`cmp [esi+534h], 4; jge`, con signo)
    if ((int32)ped->m_nMoveState >= 4) {
        return false;
    }
    if (ped->bIsLanding || ped->bIsInTheAir || !ped->bIsStanding) {
        return false;
    }
    // Como el original: dos fcomp con 0.0 (cualquier NaN da falso)
    return ped->m_vecAnimMovingShiftLocal.x == 0.0f && ped->m_vecAnimMovingShiftLocal.y == 0.0f;
}

// 0x464D50
bool CTheScripts::IsPlayerOnAMission() {
    return OnAMissionFlag && notsa::ReadAs<uint32_t>(&ScriptSpace[OnAMissionFlag]) == 1;
}

// 0x4861F0
bool CTheScripts::IsVehicleStopped(CVehicle* veh) {
    // Como el original (0x4861F6..0x486237): minimo de timeStep y oldTimeStep (`fcomp` + `test ah,5` + jp: con NaN o >= elige el viejo),
    // multiplicado por 0.01f y comparado con `fcompp`: verdadero si velocidad <= minimo * 0.01f
    const float ts  = CTimer::GetTimeStep();
    const float old = CTimer::ms_fOldTimeStep;
    const float sel = (ts < old) ? ts : old;
    return std::bit_cast<float>(0x3C23D70Au) * sel >= veh->m_fMovingSpeed;
}

// 0x5D4FD0
void CTheScripts::Load() {
    Init();

    const auto totalSize = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>();
    auto       p         = ScriptSpace.data();

    // Como el original (0x5D4FF3..0x5D5045): con total > 0xC800 (comparacion con signo) se cargan
    // (total - 0xC801) / 0xC800 + 1 trozos de 0xC800 bytes y luego el resto; con un total multiplo de 0xC800 no hay trozo final de tamano 0.
    uint32 remainder = totalSize;
    if ((int32)totalSize > (int32)MAX_SAVED_GVAR_PART_SIZE) {
        auto nParts = (totalSize - (MAX_SAVED_GVAR_PART_SIZE + 1)) / MAX_SAVED_GVAR_PART_SIZE + 1;
        remainder   = totalSize - nParts * MAX_SAVED_GVAR_PART_SIZE;
        for (; nParts > 0; nParts--, p += MAX_SAVED_GVAR_PART_SIZE) {
            CGenericGameStorage::LoadDataFromWorkBuffer(p, MAX_SAVED_GVAR_PART_SIZE);
        }
    }

    // Load remainder
    CGenericGameStorage::LoadDataFromWorkBuffer(p, remainder);

    for (auto& sfb : ScriptsForBrains.m_aScriptForBrains) {
        CGenericGameStorage::LoadDataFromWorkBuffer(sfb);
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(OnAMissionFlag);
    CGenericGameStorage::LoadDataFromWorkBuffer(LastMissionPassedTime);

    for (auto& bswap : BuildingSwapArray) {
        const auto type    = CGenericGameStorage::LoadDataFromWorkBuffer<ScriptSavedObjectType>();
        const auto poolRef = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>() - 1;
        CGenericGameStorage::LoadDataFromWorkBuffer(bswap.m_nNewModelIndex);
        CGenericGameStorage::LoadDataFromWorkBuffer(bswap.m_nOldModelIndex);

        bswap.m_pCBuilding = nullptr;
        switch (type) {
        case ScriptSavedObjectType::NONE:
        case ScriptSavedObjectType::INVISIBLE:
            break;
        case ScriptSavedObjectType::BUILDING:
            bswap.m_pCBuilding = GetBuildingPool()->GetAt(poolRef);
            break;
        default:
            NOTSA_UNREACHABLE();
        }

        if (auto* b = bswap.m_pCBuilding) {
            CWorld::Remove(b);
            b->ReplaceWithNewModel(bswap.m_nNewModelIndex);
            CWorld::Add(b);
        }
    }

    for (auto& is : InvisibilitySettingArray) {
        const auto type    = CGenericGameStorage::LoadDataFromWorkBuffer<ScriptSavedObjectType>();
        const auto poolRef = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>() - 1;

        switch (type) {
        case ScriptSavedObjectType::NONE:
            is = nullptr;
            break;
        case ScriptSavedObjectType::INVISIBLE:
            // Not saved by the game, but the logic is still there
            if (is) {
                is->SetUsesCollision(false);
                is->SetIsVisible(false);
            }
            break;
        case ScriptSavedObjectType::BUILDING:
            is = nullptr;
            if (auto* obj = GetBuildingPool()->GetAt(poolRef)) {
                is                   = obj;
                is->SetUsesCollision(false);
                is->SetIsVisible(false);
            }
            break;
        case ScriptSavedObjectType::OBJECT:
            is = nullptr;
            if (auto* obj = GetObjectPool()->GetAt(poolRef)) {
                is                   = obj;
                is->SetUsesCollision(false);
                is->SetIsVisible(false);
            }
            break;
        case ScriptSavedObjectType::DUMMY:
            is = nullptr;
            if (auto* obj = GetDummyPool()->GetAt(poolRef)) {
                is                   = obj;
                is->SetUsesCollision(false);
                is->SetIsVisible(false);
            }
            break;
        default:
            NOTSA_UNREACHABLE();
        }
    }

    for (auto& veh : VehicleModelsBlockedByScript) {
        CGenericGameStorage::LoadDataFromWorkBuffer(veh);
    }

    for (auto& lod : ScriptConnectLodsObjects) {
        CGenericGameStorage::LoadDataFromWorkBuffer(lod);
    }

    for (auto& ag : ScriptAttachedAnimGroups) {
        CGenericGameStorage::LoadDataFromWorkBuffer(ag);

        if (ag.m_nModelID != MODEL_INVALID) {
            ScriptAttachAnimGroupToCharModel(ag.m_nModelID, ag.m_IfpName);
        }
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(bUsingAMultiScriptFile);
    CGenericGameStorage::LoadDataFromWorkBuffer(bPlayerHasMetDebbieHarry);

    {
        // Ignored
        CGenericGameStorage::LoadDataFromWorkBuffer<uint32>(); // MainScriptSize
        CGenericGameStorage::LoadDataFromWorkBuffer<uint32>(); // LargestMissionScriptSize
        CGenericGameStorage::LoadDataFromWorkBuffer<uint16>(); // NumberOfMissionScripts
        CGenericGameStorage::LoadDataFromWorkBuffer<uint16>(); // NumberOfExclusiveMissionScripts
        CGenericGameStorage::LoadDataFromWorkBuffer<uint32>(); // LargestNumberOfMissionScriptLocalVariables
    }

    // Unused
    // auto j = 0u;
    // for (auto* s = pActiveScripts; s; s->m_pNext)
    //     j++;

    auto numScripts = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>();
    for (auto i = 0u; i < numScripts; i++) {
        auto* script = StartNewScript(nullptr, CGenericGameStorage::LoadDataFromWorkBuffer<uint16>());
        {
            const auto prev = script->m_pPrev, next = script->m_pNext;
            CGenericGameStorage::LoadDataFromWorkBuffer(*script);
            script->m_pPrev = prev;
            script->m_pNext = next;
        }
        script->SetCurrentIp(&ScriptSpace[CGenericGameStorage::LoadDataFromWorkBuffer<uint32>()]);

        for (auto& stk : script->m_IPStack) {
            if (const auto ip = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>()) {
                stk = &ScriptSpace[ip];
            } else {
                stk = nullptr;
            }
        }
    }
}

// 0x5D4C40
void CTheScripts::Save() {
    const auto totalSize = GetSCMChunk<tSCMGlobalVarChunk>()->m_NextChunkOffset;
    auto       p         = ScriptSpace.data();
    CGenericGameStorage::SaveDataToWorkBuffer(totalSize);

    // Como el original (0x5D4C69..0x5D4CB7): igual que Load; sin trozo final de tamano 0 cuando el total es multiplo de 0xC800
    uint32 remainder = totalSize;
    if ((int32)totalSize > (int32)MAX_SAVED_GVAR_PART_SIZE) {
        auto nParts = (totalSize - (MAX_SAVED_GVAR_PART_SIZE + 1)) / MAX_SAVED_GVAR_PART_SIZE + 1;
        remainder   = totalSize - nParts * MAX_SAVED_GVAR_PART_SIZE;
        for (; nParts > 0; nParts--, p += MAX_SAVED_GVAR_PART_SIZE) {
            CGenericGameStorage::SaveDataToWorkBuffer(p, MAX_SAVED_GVAR_PART_SIZE);
        }
    }

    // Save remainder
    CGenericGameStorage::SaveDataToWorkBuffer(p, remainder);


    for (auto& sfb : ScriptsForBrains.m_aScriptForBrains) {
        CGenericGameStorage::SaveDataToWorkBuffer(sfb);
    }

    CGenericGameStorage::SaveDataToWorkBuffer(OnAMissionFlag);
    CGenericGameStorage::SaveDataToWorkBuffer(LastMissionPassedTime);

    for (auto& bswap : BuildingSwapArray) {
        if (auto* b = bswap.m_pCBuilding) {
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::BUILDING);

            // Add 1 to the index because 0 is considered invalid and index can be 0.
            CGenericGameStorage::SaveDataToWorkBuffer(GetBuildingPool()->GetIndex(b) + 1);
        } else {
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::NONE);
            CGenericGameStorage::SaveDataToWorkBuffer(0);
        }

        CGenericGameStorage::SaveDataToWorkBuffer(bswap.m_nNewModelIndex);
        CGenericGameStorage::SaveDataToWorkBuffer(bswap.m_nOldModelIndex);
    }

    for (auto& is : InvisibilitySettingArray) {
        if (!is) {
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::NONE);
            CGenericGameStorage::SaveDataToWorkBuffer(0);
            continue;
        }

        switch (is->GetType()) {
        case ENTITY_TYPE_NOTHING:
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::NONE);
            CGenericGameStorage::SaveDataToWorkBuffer(0);
            break;
        case ENTITY_TYPE_BUILDING:
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::BUILDING);
            CGenericGameStorage::SaveDataToWorkBuffer(GetBuildingPool()->GetIndex(is->AsBuilding()) + 1);
            break;
        case ENTITY_TYPE_OBJECT:
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::OBJECT);
            CGenericGameStorage::SaveDataToWorkBuffer(GetObjectPool()->GetIndex(is->AsObject()) + 1);
            break;
        case ENTITY_TYPE_DUMMY:
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::DUMMY);
            CGenericGameStorage::SaveDataToWorkBuffer(GetDummyPool()->GetIndex(is->AsDummy()) + 1);
            break;
        default:
            NOTSA_UNREACHABLE();
        }
    }

    for (auto& veh : VehicleModelsBlockedByScript) {
        CGenericGameStorage::SaveDataToWorkBuffer(veh);
    }

    for (auto& lod : ScriptConnectLodsObjects) {
        CGenericGameStorage::SaveDataToWorkBuffer(lod);
    }

    for (auto& ag : ScriptAttachedAnimGroups) {
        CGenericGameStorage::SaveDataToWorkBuffer(ag);
    }

    CGenericGameStorage::SaveDataToWorkBuffer(bUsingAMultiScriptFile);
    CGenericGameStorage::SaveDataToWorkBuffer(bPlayerHasMetDebbieHarry); // ?

    CGenericGameStorage::SaveDataToWorkBuffer(MainScriptSize);
    CGenericGameStorage::SaveDataToWorkBuffer(LargestMissionScriptSize);
    CGenericGameStorage::SaveDataToWorkBuffer(NumberOfMissionScripts);
    CGenericGameStorage::SaveDataToWorkBuffer(NumberOfExclusiveMissionScripts);
    CGenericGameStorage::SaveDataToWorkBuffer(LargestNumberOfMissionScriptLocalVariables);

    auto numNonExternalScripts = 0u;
    auto* lastScript           = pActiveScripts;
    for (auto* s = pActiveScripts; s; s = s->m_pNext) {
        lastScript = s;
        if (!s->m_IsExternal && s->m_ExternalType == -1) {
            numNonExternalScripts++;
        }
    }
    CGenericGameStorage::SaveDataToWorkBuffer(numNonExternalScripts);

    for (auto* s = lastScript; s; s = s->m_pPrev) {
        if (s->m_IsExternal || s->m_ExternalType != -1) {
            continue;
        }

        CGenericGameStorage::SaveDataToWorkBuffer((int16)GetScriptIndexFromPointer(s));
        CGenericGameStorage::SaveDataToWorkBuffer(*s);
        CGenericGameStorage::SaveDataToWorkBuffer((uint32)(s->m_IP - ScriptSpace.data()));

        for (auto& stk : s->m_IPStack) {
            CGenericGameStorage::SaveDataToWorkBuffer((uint32)(stk ? stk - ScriptSpace.data() : 0));
        }
    }
}

// 0x464BB0
void CTheScripts::WipeLocalVariableMemoryForMissionScript() {
    rng::fill(LocalVariablesForCurrentMission, tScriptParam{});
}

// 0x46A810
bool CTheScripts::HasCarModelBeenSuppressed(eModelID carModelId) {
    return notsa::contains(SuppressedVehicleModels, carModelId);
}

// 0x46A890
bool CTheScripts::HasVehicleModelBeenBlockedByScript(eModelID carModelId) {
    return notsa::contains(VehicleModelsBlockedByScript, carModelId);
}

// 0x464D40
void CTheScripts::StartTestScript() {
    ZoneScoped;

    StartNewScript(MainSCMBlock.data());
}

// 0x46A000
void CTheScripts::Process() {
    ZoneScoped;

    if (CReplay::Mode == MODE_PLAYBACK) {
        return;
    }

    CommandsExecuted = 0;
    ScriptsUpdated   = 0;

    // Como el original (0x46A010..0x46A036): CTimer::ms_fTimeStepNonClipped (0xB7CB58) * 0.02f * 1000.0f truncado con _ftol2
    const auto timeStepMS = (int32)(CTimer::GetTimeStepNonClipped() * std::bit_cast<float>(0x3CA3D70Au) * 1000.0f);

    UpsideDownCars.UpdateTimers();
    StuckCars.Process();
    MissionCleanUp.CheckIfCollisionHasLoadedForMissionObjects();
    DrawScriptSpheres();
    ProcessAllSearchLights();
    ProcessWaitingForScriptBrainArray();

    if (FailCurrentMission) {
        --FailCurrentMission;
    }

    if (UseTextCommands != eUseTextCommandState::DISABLED) {
        // Como el original (0x46A0A0..0x46A184): wrapX = 182.0, centreSize = 640.0 (no el ancho de pantalla de Init)
        for (auto& line : IntroTextLines) {
            ResetIntroTextLine(line, std::bit_cast<float>(0x43360000u), std::bit_cast<float>(0x44200000u));
        }
        NumberOfIntroTextLinesThisFrame = 0;

        // Como el original (0x46A196..0x46A1A8): solo se limpian m_nType y m_bDrawBeforeFade de cada rectangulo
        for (auto& rect : IntroRectangles) {
            rect.m_nType           = eScriptRectangleType::INACTIVE;
            rect.m_bDrawBeforeFade = false;
        }
        NumberOfIntroRectanglesThisFrame = 0;

        if (UseTextCommands == eUseTextCommandState::DISABLE_NEXT_FRAME) {
            UseTextCommands = eUseTextCommandState::DISABLED;
        }
    }

    LocalVariablesForCurrentMission[SCRIPT_VAR_TIMERA].iParam += timeStepMS;
    LocalVariablesForCurrentMission[SCRIPT_VAR_TIMERB].iParam += timeStepMS;

    CLoadingScreen::NewChunkLoaded();

    // Como el EXE (0x46A1F0..0x46A22C): `next` se guarda antes de ejecutar el script y el bucle termina si el siguiente
    // ya no esta activo (m_IsActive, +0xC4). Sin esa comprobacion, un script que termina a otro por nombre
    // (TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME) hacia que se ejecutara el script terminado y, siguiendo su m_pNext,
    // toda la lista de scripts inactivos.
    for (CRunningScript* it = pActiveScripts; it;) {
        ++ScriptsUpdated;
        CRunningScript* const next = it->m_pNext;

        it->m_LocalVars[SCRIPT_VAR_TIMERA].iParam += timeStepMS;
        it->m_LocalVars[SCRIPT_VAR_TIMERB].iParam += timeStepMS;
        it->Process();

        it = (next && next->m_IsActive) ? next : nullptr;
    }

    CLoadingScreen::NewChunkLoaded();

    // Como el original (0x46A233..0x46A279): el pool de peds se recorre de la ultima ranura a la primera
    auto* const pedPool = GetPedPool();
    for (auto i = pedPool->GetSize(); i-- > 0;) {
        if (auto* ped = pedPool->GetAt(i); ped && ped->IsCreatedByMission()) {
            ped->GetIntelligence()->RecordEventForScript(0, 0);
        }
    }
}

namespace {
//! Como el original (0x493A33..0x493C86, igual en los cuatro estados): acerca el objetivo del foco a `goal`.
//! Devuelve true si todavia estaba lejos (distancia > velocidad, o NaN) y lo ha movido; false si ha llegado.
//! Distancia = sqrt((dz*dz + dy*dy) + dx*dx), como el orden de las sumas del EXE.
bool StepSearchLightTarget(tScriptSearchlight& sl, const CVector& goal) {
    const float dx = goal.x - sl.m_Target.x;
    const float dy = goal.y - sl.m_Target.y;
    const float dz = goal.z - sl.m_Target.z;
    const float len = sqrtf((dz * dz + dy * dy) + dx * dx);
    if (len <= sl.m_fPathSpeed) {
        return false;
    }

    CVector dir{ dx, dy, dz };
    dir.Normalise(); // 0x59C910
    const float speed = sl.m_fPathSpeed;
    const float mx = dir.x * speed;
    const float my = dir.y * speed;
    const float mz = speed * dir.z;
    sl.m_Target.x = mx + sl.m_Target.x;
    sl.m_Target.y = my + sl.m_Target.y;
    sl.m_Target.z = mz + sl.m_Target.z;
    return true;
}
}

// 0x4939F0
void CTheScripts::ProcessAllSearchLights() {
    ZoneScoped;

    for (auto& sl : ScriptSearchLightArray) {
        if (!sl.IsActive() || !sl.m_SomethingFlag) {
            continue;
        }

        // Como el original (0x493A1A..0x493C86): salto por estado; los estados 0 y > 4 no mueven el objetivo
        switch (sl.m_nCurrentState) {
        case eScriptSearchLightState::STATE_1: {
            if (!StepSearchLightTarget(sl, sl.m_PathCoord1)) {
                sl.m_Target        = sl.m_PathCoord1;
                sl.m_SomethingFlag = true;
                sl.m_nCurrentState = eScriptSearchLightState::STATE_2;
            }
            break;
        }
        case eScriptSearchLightState::STATE_2: {
            if (!StepSearchLightTarget(sl, sl.m_PathCoord2)) {
                sl.m_Target        = sl.m_PathCoord2;
                sl.m_SomethingFlag = true;
                sl.m_nCurrentState = eScriptSearchLightState::STATE_1;
            }
            break;
        }
        case eScriptSearchLightState::STATE_3: {
            if (!StepSearchLightTarget(sl, sl.m_FollowingEntity->GetPosition())) {
                sl.m_Target = sl.m_FollowingEntity->GetPosition();
                /* flag/estado no se alteran */
            }
            break;
        }
        case eScriptSearchLightState::STATE_4: {
            if (!StepSearchLightTarget(sl, sl.m_PathCoord1)) {
                sl.m_Target        = sl.m_PathCoord1;
                sl.m_nCurrentState = eScriptSearchLightState::STATE_0;
                sl.m_SomethingFlag = true;
                sl.m_PathCoord1    = CVector{};
                sl.m_fPathSpeed    = 0.0f;
            }
            break;
        }
        default:
            break;
        }

        CEntity* const bulb = sl.m_Bulb.Get();
        if (!bulb) {
            continue;
        }

        // Como el original (0x493C86..0x493E01): direccion bombilla -> objetivo normalizada (0x59C910),
        // rumbo = atan2(-x, y) y cabeceo = atan2(z, sqrt(x*x + y*y)) con fpatan; la matriz del EXE se usa sin comprobar null.
        const CVector bulbPos = bulb->GetPosition();
        CVector       dir{ sl.m_Target.x - bulbPos.x, sl.m_Target.y - bulbPos.y, sl.m_Target.z - bulbPos.z };
        dir.Normalise();

        const CVector prevPos = bulb->m_matrix->GetPosition();
        const float   heading = TrigAtan2Cpp(-dir.x, dir.y); // X87Math.cpp: fpatan(ST1 = num, ST0 = den) con un solo redondeo a float
        const float   pitch   = TrigAtan2Cpp(dir.z, sqrtf(dir.x * dir.x + dir.y * dir.y));

        const auto Transform = [&](CEntity* entity) {
            auto& mat = *entity->m_matrix;
            mat.SetRotateX(pitch); // 0x59B060
            mat.RotateZ(heading);  // 0x59B390
            mat.GetPosition().x = prevPos.x + mat.GetPosition().x;
            mat.GetPosition().y = prevPos.y + mat.GetPosition().y;
            mat.GetPosition().z = prevPos.z + mat.GetPosition().z;
            entity->UpdateRwMatrix();
            entity->UpdateRwFrame();
        };

        Transform(bulb);
        Transform(sl.m_Housing.Get());
    }
}

// 0x46CF00
void CTheScripts::ProcessWaitingForScriptBrainArray() {
    ZoneScoped;

    if (!FindPlayerPed())
        return;

    // Como el original (0x46CF24): el centro del mundo del jugador (FindPlayerCentreOfWorld 0x56E250) se calcula una sola vez antes del bucle
    const CVector playerCentre = FindPlayerCentreOfWorld(CWorld::PlayerInFocus);

    for (auto& e : EntitiesWaitingForScriptBrain) {
        if (!e.m_pEntity) {
            continue;
        }

        switch (const auto t = ScriptsForBrains.m_aScriptForBrains[e.m_ScriptBrainIndex].m_TypeOfBrain) {
        case 0: // TODO: enum
        case 3: // for peds?
        {
            auto*      ped = e.m_pEntity->AsPed();
            const auto idx = ScriptsForBrains.m_aScriptForBrains[ped->m_StreamedScriptBrainToLoad].m_StreamedScriptIndex;

            if (CStreaming::IsModelLoaded(SCMToModelId(idx))) {
                ScriptsForBrains.StartNewStreamedScriptBrain(
                    static_cast<uint8>(ped->m_StreamedScriptBrainToLoad), // cast?
                    ped,
                    false
                );
            } else {
                CStreaming::RequestModel(SCMToModelId(idx), STREAMING_MISSION_REQUIRED);
            }
            break;
        }
        case 1:
        case 4: // for objects?
        {
            auto* obj = e.m_pEntity->AsObject();

            switch (obj->objectFlags.b0x100000_0x200000) {
            case 1:
                if (!ScriptsForBrains.IsObjectWithinBrainActivationRange(obj, playerCentre))
                    break;

                [[fallthrough]];
            case 2:
                ScriptsForBrains.StartOrRequestNewStreamedScriptBrain(
                    static_cast<uint8>(obj->m_nStreamedScriptBrainToLoad), // cast?
                    obj,
                    t,
                    false
                );
                break;
            default:
                break;
            }
            break;
        }
        default:
            break;
        }
    }
}

// 0x4812D0
void CTheScripts::UndoEntityInvisibilitySettings() {
    for (auto& is : InvisibilitySettingArray) {
        if (!is) {
            continue;
        }

        is->SetIsVisible(true);
        is->SetUsesCollision(true);

        is = nullptr; // Remove from the array.
    }
}

// 0x4703C0
void CTheScripts::UseSwitchJumpTable(int32& switchLabelAddress) {
    switchLabelAddress = 0x0;

    const auto CheckEntryAndJump = [&](tScriptSwitchCase* swtch) {
        if (swtch && ValueToCheckInSwitchStatement != swtch->m_nSwitchValue) {
            return false;
        }

        switchLabelAddress = swtch ? swtch->m_nSwitchLabelAddress : SwitchDefaultAddress;
        ReinitialiseSwitchStatementData();
        return true;
    };

    // Como el original (0x4703CC..0x4703DD): los punteros son bytes (bl = NumberOfEntriesInSwitchTable - 1, con wrap a 8 bits)
    uint8 ptr1 = 0;
    uint8 ptr2 = (uint8)((uint8)NumberOfEntriesInSwitchTable - 1);
    while ((int32)ptr2 - (int32)ptr1 > 1) {
        const uint8 idx = (uint8)(((int32)ptr1 + (int32)ptr2) / 2);
        if (CheckEntryAndJump(&SwitchJumpTable.data()[idx])) {
            return;
        }

        if (ValueToCheckInSwitchStatement <= SwitchJumpTable.data()[idx].m_nSwitchValue) {
            ptr2 = idx;
        } else {
            ptr1 = idx;
        }
    }

    if (CheckEntryAndJump(&SwitchJumpTable.data()[ptr2]) || CheckEntryAndJump(&SwitchJumpTable.data()[ptr1])) {
        return;
    }

    CheckEntryAndJump(nullptr); // Jump to the default case
}

// 0x4646D0
void CTheScripts::PrintListSizes() {
    auto active{ 0u }, idle{ 0u };

    for (const auto* s = pActiveScripts; s; s = s->m_pNext) active++;
    for (const auto* s = pIdleScripts; s; s = s->m_pNext) idle++;

    NOTSA_LOG_DEBUG("Scripts Active: {}, Idle: {}", active, idle);
}

uint32 DbgLineColour = 0x0000FFFF; // r = 0, g = 0, b = 255, a = 255

namespace {
//! Como el original (p. ej. 0x48684B..0x48687D): ProcessVerticalLine desde (x, y, -1000) con distancia 1000 sobre edificios y dummies
void DebugProbeVerticalLine(float x, float y) {
    CColPoint colPoint;
    CEntity*  colEntity;
    const CVector origin{ x, y, -1000.0f };
    CWorld::ProcessVerticalLine(origin, 1000.0f, colPoint, colEntity, true, false, false, false, true, false, nullptr);
}
}

// Usage:
//  const auto pos = FindPlayerPed()->GetPosition() + CVector{ 0.0f, 0.f, 1.0f };
//  CTheScripts::DrawDebugSquare(pos.x + 5.f, pos.y, pos.x, pos.y + 5.f);
// 0x486840
void CTheScripts::DrawDebugSquare(float x1, float y1, float x2, float y2) {
    // Como el original: cuatro sondas verticales y nada mas (las alturas calculadas no se usan y ScriptDebugLine3D del EXE es un `ret`)
    DebugProbeVerticalLine(x1, y1);
    DebugProbeVerticalLine(x2, y1);
    DebugProbeVerticalLine(x2, y2);
    DebugProbeVerticalLine(x1, y2);
}

// 0x486990
void CTheScripts::DrawDebugAngledSquare(float infX, float infY, float supX, float supY, float rotSupX, float rotSupY, float rotInfX, float rotInfY) {
    // Como el original: cuatro sondas verticales (inf, sup, rotSup, rotInf)
    DebugProbeVerticalLine(infX, infY);
    DebugProbeVerticalLine(supX, supY);
    DebugProbeVerticalLine(rotSupX, rotSupY);
    DebugProbeVerticalLine(rotInfX, rotInfY);
}

// (float infX, float infY, float infZ, float supX, float supY, float supZ)
void CTheScripts::DrawDebugCube(const CVector& inf, const CVector& sup) {
    ScriptDebugLine3D({ inf.x, inf.y, inf.z }, { sup.x, inf.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, inf.y, inf.z }, { sup.x, sup.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, sup.y, inf.z }, { inf.x, sup.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x, sup.y, inf.z }, { inf.x, inf.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x, inf.y, sup.z }, { sup.x, inf.y, sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, inf.y, sup.z }, { sup.x, sup.y, sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, sup.y, sup.z }, { inf.x, sup.y, sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x, sup.y, sup.z }, { inf.x, inf.y, sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x, inf.y, sup.z }, { inf.x, inf.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, inf.y, sup.z }, { sup.x, inf.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, sup.y, sup.z }, { sup.x, sup.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x, sup.y, sup.z }, { inf.x, sup.y, inf.z }, DbgLineColour, DbgLineColour);
}

// (float infX, float infY, float infZ, float supX, float supY, float supZ, float rotSup, float rotSup, float rotInf, float rotInf)
void CTheScripts::DrawDebugAngledCube(const CVector& inf, const CVector& sup, const CVector2D& rotSup, const CVector2D& rotInf) {
    ScriptDebugLine3D({ inf.x,    inf.y,    inf.z    }, { sup.x,    inf.y,    inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x,    inf.y,    inf.z    }, { rotSup.x, rotSup.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotSup.x, rotSup.y, inf.z    }, { rotInf.x, rotInf.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotInf.x, rotInf.y, inf.z    }, { inf.x,    inf.y,    inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x,    inf.y,    sup.z    }, { sup.x,    inf.y,    sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x,    inf.y,    sup.z    }, { rotSup.x, rotSup.y, sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotSup.x, rotSup.y, rotInf.x }, { rotInf.y, sup.y,    sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotInf.x, rotInf.y, sup.z    }, { inf.x,    inf.y,    sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x,    inf.y,    sup.z    }, { inf.x,    inf.y,    inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x,    inf.y,    sup.z    }, { sup.x,    inf.y,    inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotSup.x, rotSup.y, sup.z    }, { rotSup.x, rotSup.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotInf.x, rotInf.y, sup.z    }, { rotInf.x, rotInf.y, inf.z }, DbgLineColour, DbgLineColour);
}

namespace {
//! Como el original: copia directa de las esquinas (los rectangulos de script pueden estar invertidos; CRect(min, max) lo comprueba con assert)
CRect ScriptRawRect(const CVector2D& a, const CVector2D& b) {
    CRect r;
    r.left = a.x; r.bottom = a.y; r.right = b.x; r.top = b.y;
    return r;
}
}

// 0x464980
void CTheScripts::DrawScriptSpritesAndRectangles(bool drawBeforeFade) {
    for (const auto& rt : IntroRectangles) {
        if (rt.m_bDrawBeforeFade != drawBeforeFade) {
            continue;
        }
        switch (rt.m_nType) {
        case eScriptRectangleType::INACTIVE:
            break;
        case eScriptRectangleType::TITLE_AND_MESSAGE:
            FrontEndMenuManager.DrawWindowedText(
                rt.cornerA.x, rt.cornerA.y,
                rt.cornerB.x, // ?
                rt.gxt1,
                rt.gxt2,
                rt.m_Alignment
            );
            break;
        case eScriptRectangleType::TEXT:
            FrontEndMenuManager.DrawWindow(
                ScriptRawRect(rt.cornerA, rt.cornerB),
                rt.gxt1,
                0,
                CRGBA{ 0, 0, 0, 190 },
                rt.m_nTextboxStyle, // ?
                true
            );
            break;
        case eScriptRectangleType::MONOCOLOR:
            CSprite2d::DrawRect(
                ScriptRawRect(rt.cornerA, rt.cornerB),
                rt.m_nTransparentColor
            );
            break;
        case eScriptRectangleType::TEXTURED:
            ScriptSprites[rt.m_nTextureId].Draw(
                ScriptRawRect(rt.cornerA, rt.cornerB),
                rt.m_nTransparentColor
            );
            break;
        case eScriptRectangleType::MONOCOLOR_ANGLED: {
            // Como el original (0x464A9A..0x464B7F): mid = (B + A) * 0.5 y vAM = mid - A; cos(fcos) queda en ST0 sin redondear a float
            // y sin(fsin) se guarda como float; los cuatro productos se redondean a 24 bits.
            const float midX = (rt.cornerB.x + rt.cornerA.x) * 0.5f;
            const float midY = (rt.cornerB.y + rt.cornerA.y) * 0.5f;
            const float vAMx = midX - rt.cornerA.x;
            const float vAMy = midY - rt.cornerA.y;
            const float angle = rt.m_nAngle;
            float terms[4]; // {cos * vAMy, sin * vAMx, cos * vAMx, sin * vAMy}
            TheScriptsAngledRectTermsCpp(angle, vAMx, vAMy, terms); // X87Math.cpp
            const float prodCosB = terms[0];
            const float prodSinA = terms[1];
            const float prodCosA = terms[2];
            const float prodSinB = terms[3];

            ScriptSprites[rt.m_nTextureId].Draw(
                (-prodCosA + prodSinB) + midX,
                (-prodSinA - prodCosB) + midY,
                (prodSinB + prodCosA) + midX,
                (prodSinA - prodCosB) + midY,
                (-prodCosA - prodSinB) + midX,
                (prodCosB + -prodSinA) + midY,
                (prodCosA - prodSinB) + midX,
                (prodSinA + prodCosB) + midY,
                rt.m_nTransparentColor
            );
            break;
        }
        default:
            NOTSA_UNREACHABLE("Unknown script-rect type ({})", (int32)(rt.m_nType));
        }
    }
}

// Usage:
//   CTheScripts::ScriptDebugCircle2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 100, 50.f, 50.f, HudColour.GetRGB(HUD_COLOUR_RED).ToInt());
// 0x485C20
void CTheScripts::ScriptDebugCircle2D(float x, float y, float width, float height, uint32 color) {
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

    // Como el original (0x485C7F..0x485DA1): parte resaltada = (CTimer::m_snTimeInMilliseconds >> 6) & 0xF; el color llega como
    // r<<24|g<<16|b<<8|a y los vertices lo llevan como a<<24|r<<16|g<<8|b; el segmento resaltado usa 3/4 de cada canal.
    // z, rhw y u/v de los vertices quedan sin inicializar, como en el EXE.
    const int32  part = (int16)((CTimer::GetTimeInMS() >> 6) & 0xF);
    const int32  cr   = (color >> 24) & 0xFF;
    const int32  cg   = (color >> 16) & 0xFF;
    const int32  cb   = (color >> 8) & 0xFF;
    const int32  ca   = color & 0xFF;
    const uint32 argb = ((uint32)ca << 24) | ((uint32)cr << 16) | ((uint32)cg << 8) | (uint32)cb;
    const uint32 dim  = ((uint32)(ca * 3 / 4) << 24) | ((uint32)(cr * 3 / 4) << 16) | ((uint32)(cg * 3 / 4) << 8) | (uint32)(cb * 3 / 4);
    const float  step = std::bit_cast<float>(0x3EC90FDBu); // 0x859F50: pi / 8

    RwIm2DVertex vertices[2];
    for (int32 i = 0; i < 16; i++) {
        const int32 idx0 = i;
        const int32 idx1 = i + 1;
        // t = i * (pi / 8) con fild + fmul (un redondeo a 24 bits); x = fsin(t) * width + x e y = fcos(t) * height + y con fsin/fcos sin redondear
        const float t0  = IntTimesFloatCpp(idx0, step);
        const float t1  = IntTimesFloatCpp(idx1, step);
        const float v0x = TrigCompoundSinMulAdd(t0, width, x);
        const float v0y = TrigCompoundCosMulAdd(t0, height, y);
        const float v1x = TrigCompoundSinMulAdd(t1, width, x);
        const float v1y = TrigCompoundCosMulAdd(t1, height, y);
        vertices[0].x = v0x;
        vertices[0].y = v0y;
        vertices[1].x = v1x;
        vertices[1].y = v1y;
        vertices[0].emissiveColor = (part == i) ? dim : argb;
        vertices[1].emissiveColor = vertices[0].emissiveColor;
        RwIm2DRenderLine(vertices, 2, 0, 1);
    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
}

// 0x4810E0
void CTheScripts::DrawScriptSpheres() {
    ZoneScoped;

    for (auto& ss : ScriptSphereArray) {
        if (!ss.m_bUsed) {
            continue;
        }

        C3dMarkers::PlaceMarkerSet(
            ss.m_nId,
            e3dMarkerType::MARKER3D_CYLINDER,
            ss.m_vCoords,
            ss.m_fRadius,
            255,
            0,
            0,
            228,
            2048,
            0.1f,
            0
        );
    }
}

// 0x485E00
void CTheScripts::HighlightImportantArea(uint32 id, float x1, float y1, float x2, float y2, float z) {
    CVector2D inf, sup;
    // Como el original (0x485E07..0x485E55): `fcomp` + `test ah,41h` + jne -> con x1 <= x2 (o NaN) inf = x1 y sup = x2
    if (!(x1 > x2)) {
        inf.x = x1;
        sup.x = x2;
    } else {
        inf.x = x2;
        sup.x = x1;
    }

    if (!(y1 > y2)) {
        inf.y = y1;
        sup.y = y2;
    } else {
        inf.y = y2;
        sup.y = y1;
    }

    CVector center;
    center.x = (sup.x + inf.x) * 0.5f;
    center.y = (sup.y + inf.y) * 0.5f;
    // Como el original (0x485E99): a la altura del suelo se le suma 2.0 (0x858CA0)
    center.z = (z <= MAP_Z_LOW_LIMIT) ? CWorld::FindGroundZForCoord(center.x, center.y) + 2.0f : z;
    CShadows::RenderIndicatorShadow(id, SHADOW_ADDITIVE, nullptr, center, sup.x - center.x, 0.0f, 0.0f, center.y - sup.y, 0);
}

// 0x485EF0
void CTheScripts::HighlightImportantAngledArea(uint32 id, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float z) {
    CVector2D inf, sup;
    float x, y;

    x = (x1 + x2) / 2;
    y = (y1 + y2) / 2;
    sup.x = inf.x = x;
    sup.y = inf.y = y;

    x = (x2 + x3) / 2;
    y = (y2 + y3) / 2;
    inf.x = std::min(inf.x, x);
    sup.x = std::max(sup.x, x);
    inf.y = std::min(inf.y, y);
    sup.y = std::max(sup.y, y);

    x = (x3 + x4) / 2;
    y = (y3 + y4) / 2;
    inf.x = std::min(inf.x, x);
    sup.x = std::max(sup.x, x);
    inf.y = std::min(inf.y, y);
    sup.y = std::max(sup.y, y);

    x = (x4 + x1) / 2;
    y = (y4 + y1) / 2;
    inf.x = std::min(inf.x, x);
    sup.x = std::max(sup.x, x);
    inf.y = std::min(inf.y, y);
    sup.y = std::max(sup.y, y);

    CVector center;
    center.x = (inf.x + sup.x) / 2;
    center.y = (inf.y + sup.y) / 2;
    center.z = (z <= MAP_Z_LOW_LIMIT) ? CWorld::FindGroundZForCoord(center.x, center.y) : z;
    CShadows::RenderIndicatorShadow(id, SHADOW_ADDITIVE, nullptr, center, sup.x - center.x, 0.0f, 0.0f, center.y - sup.y, 0);
}

static uint16 NumScriptDebugLines;
static tStoredLine aStoredLines[MAX_NUM_STORED_LINES];

// 0x485DE0
void CTheScripts::ScriptDebugLine3D(const CVector& start, const CVector& end, uint32 color1, uint32 color2) {
    // Como el original: el cuerpo del EXE es un `ret` (las lineas de depuracion no se almacenan en la version final)
}

// 0x0 NOP
void CTheScripts::RenderTheScriptDebugLines() {
#ifndef FINAL
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    for (auto& line : std::span{ aStoredLines, NumScriptDebugLines }) {
        CLines::RenderLineWithClipping(line.vecInf, line.vecSup, line.color1, line.color2);
    }
    NumScriptDebugLines = 0;
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
#endif
}

namespace {
//! Como el original (Multiply3x3 0x59C790): matriz * vector con el orden de sumas del EXE
CVector ExeMultiply3x3MV(const CMatrix& m, const CVector& v) {
    return {
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y
    };
}
}

// 0x493E30
void CTheScripts::RenderAllSearchLights() {
    ZoneScoped;

    for (const auto&& [i, light] : rngv::enumerate(ScriptSearchLightArray)) {
        if (!light.IsActive()) {
            continue;
        }

        CVector origin = light.m_Origin;
        if (auto e = notsa::coalesce(light.m_AttachedEntity.Get(), light.m_Bulb.Get())) {
            // Como el original (0x493EE2..0x493F7B): matriz de la entidad (la crea si falta), origen transformado + posicion de la matriz
            const auto& mat         = e->GetMatrix();
            const CVector rotated   = ExeMultiply3x3MV(mat, light.m_Origin);
            const CVector& matPos   = mat.GetPosition();
            origin = CVector{ rotated.x + matPos.x, rotated.y + matPos.y, rotated.z + matPos.z };
        }

        // Como el original (0x493E37: ebx = 0x216, incrementado por foco): el primer argumento es 0x216 + indice, no el indice
        CHeli::SearchLightCone(
            0x216 + (int32)i,
            origin,
            light.m_Target,
            light.m_fTargetRadius,
            1.0f,
            light.m_bClipIfColliding,
            light.m_bEnableShadow,
            light.m_TargetSpot,
            light.vf64,
            light.vf70,
            true,
            light.m_fBaseRadius,
            0.0f,
            0.0f,
            1.0f
        );
    }
}

// 0x474800
bool CTheScripts::ScriptAttachAnimGroupToCharModel(int32 modelId, const char* ifpName) {
    auto* mi = CModelInfo::GetModelInfo(modelId);
    // Como el original (0x474806..0x474824): primero GetAnimFileIndex (vtable +0x38) y despues GetAnimationBlockIndex
    const auto curIndex   = mi->GetAnimFileIndex();
    const auto blockIndex = CAnimManager::GetAnimationBlockIndex(ifpName);
    if (curIndex == blockIndex) {
        // Already attached?
        return false;
    }
    mi->SetAnimFile(ifpName);
    mi->ConvertAnimFileIndex();
    return true;
}

// 0x470A20
void CTheScripts::ScriptConnectLodsFunction(int32 lodRef1, int32 lodRef2) {
    auto obj1 = GetObjectPool()->GetAtRef(lodRef1), obj2 = GetObjectPool()->GetAtRef(lodRef2);

    obj1->SetLod(obj2);
    obj2->AddLodChildren();
    CWorld::Remove(obj2);
    obj2->SetupBigBuilding();
    CWorld::Add(obj2);
}
