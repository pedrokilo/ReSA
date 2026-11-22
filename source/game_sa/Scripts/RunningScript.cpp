/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRunningScript
 * Reconstrucción: COMPLETA — 43/43 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 43/43 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "DatosExe/DataInicializadaB.h"
#include "X87Math.h"
#include "Tasks/TaskTypes/TaskSimpleFinishBrain.h"
#include "Tasks/ScriptedBrainTaskStore.h"
#include "Tasks/TaskTypes/TaskSimpleRunNamedAnim.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Garages.h"
#include "Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h"
#include "Tasks/TaskSequences.h"
#include "Tasks/TaskTypes/TaskSimpleHoldEntity.h"
#include "Tasks/PedScriptedTaskRecord.h"
#include "ShotInfo.h"
#include "Tasks/TaskTypes/TaskSimpleDuckToggle.h"
#include "Tasks/TaskTypes/TaskComplexUseSwatRope.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h"
#include "Tasks/TaskTypes/TaskComplexClimb.h"
#include "Tasks/TaskTypes/TaskComplexSeekEntityAiming.h"
#include "Tasks/TaskTypes/TaskSimpleJetPack.h"
#include "Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h"
#include "Tasks/TaskTypes/TaskComplexGoPickUpEntity.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h"
#include "Tasks/TaskTypes/TaskSimpleSlideToCoord.h"
#include "Tasks/TaskComplexUseSequence.h"
#include "Tasks/TaskComplexSequence.h"
#include "Ropes.h"
#include "UserDisplay.h"
#include "OnscreenTimer.h"
#include "TagManager.h"
#include "VehicleRecording.h"
#include "WaterLevel.h"
#include "Shopping.h"
#include "Radar.h"
#include "Coronas.h"
#include "SetPieces.h"
#include "RoadBlocks.h"
#include "Population.h"
#include "ScriptResourceManager.h"

#include "RunningScript.h"
#include "TheScripts.h"
#include "CarGenerator.h"
#include "Hud.h"
#include "spdlog/sinks/stdout_color_sinks.h"

static notsa::log_ptr logger;

//static auto logger = NOTSA_MAKE_LOGGER("script");

//! Define it to dump out all commands that don't have a custom handler (that is, they aren't reversed)
//! Makes compilation slow, so don't enable unless necessary!
//#define DUMP_CUSTOM_COMMAND_HANDLERS_TO_FILE

#ifdef DUMP_CUSTOM_COMMAND_HANDLERS_TO_FILE
#include <fstream>
#endif

#include <fstream>
#include <string>
#include <cstdio>
#include "CommandParser/Parser.hpp"
#include "CommandParser/LUTGenerator.hpp"
#include "reversiblehooks/ReversibleHook/ScriptCommand.h"

#include "Commands/Commands.hpp"
#ifdef NOTSA_WITH_CLEO_SCRIPT_COMMANDS // TODO: Add premake/cmake option for this define
#include "Commands/CLEO/Commands.hpp"
#include "Commands/CLEO/Extensions/Commands.hpp"
#endif

// https://library.sannybuilder.com/#/sa

//! Holds all custom command handlers (or null for commands with no custom handler)
static inline std::array<notsa::script::CommandHandlerFunction, (size_t)(COMMAND_HIGHEST_ID_TO_HOOK) + 1> s_CustomCommandHandlerTable{};

std::array<std::array<char, COMMANDS_CHAR_BUFFER_SIZE>, COMMANDS_CHAR_BUFFERS_COUNT> CRunningScript::ScriptArgCharBuffers        = {};
uint8                                                                                CRunningScript::ScriptArgCharNextFreeBuffer = 0;

void CRunningScript::InjectHooks() {
    logger = NOTSA_MAKE_LOGGER("script");

    InjectCustomCommandHooks();

    RH_ScopedClass(CRunningScript);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Init, 0x4648E0);
    RH_ScopedInstall(GetCorrectPedModelIndexForEmergencyServiceType, 0x464F50);

    RH_ScopedInstall(PlayAnimScriptCommand, 0x470150);
    RH_ScopedInstall(LocateCarCommand, 0x487A20);
    RH_ScopedInstall(LocateCharCommand, 0x486D80);
    RH_ScopedInstall(LocateObjectCommand, 0x487D10);
    RH_ScopedInstall(LocateCharCarCommand, 0x487420);
    RH_ScopedInstall(LocateCharCharCommand, 0x4870F0);
    RH_ScopedInstall(LocateCharObjectCommand, 0x487720);
    RH_ScopedInstall(CarInAreaCheckCommand, 0x488EC0);
    RH_ScopedInstall(CharInAreaCheckCommand, 0x488B50);
    RH_ScopedInstall(ObjectInAreaCheckCommand, 0x489150);
    RH_ScopedInstall(CharInAngledAreaCheckCommand, 0x487F60);
    RH_ScopedInstall(FlameInAngledAreaCheckCommand, 0x488780);
    RH_ScopedInstall(ObjectInAngledAreaCheckCommand, 0x4883F0);
    RH_ScopedInstall(CollectParameters, 0x464080, { .stackArguments = 1 });
    RH_ScopedInstall(CollectNextParameterWithoutIncreasingPC, 0x464250, { .stackArguments = 0 });
    RH_ScopedInstall(StoreParameters, 0x464370, { .stackArguments = 1 });
    RH_ScopedInstall(ReadArrayInformation, 0x463CF0, { .stackArguments = 3 });
    RH_ScopedInstall(ReadParametersForNewlyStartedScript, 0x464500, { .stackArguments = 1 });
    RH_ScopedInstall(ReadTextLabelFromScript, 0x463D50, { .stackArguments = 2 });
    RH_ScopedInstall(GetIndexOfGlobalVariable, 0x464700, { .stackArguments = 0 });
    RH_ScopedInstall(GetPadState, 0x485B10);
    RH_ScopedInstall(GetPointerToLocalVariable, 0x463CA0, { .stackArguments = 1 });
    RH_ScopedInstall(GetPointerToLocalArrayElement, 0x463CC0, { .stackArguments = 3 });
    RH_ScopedInstall(GetPointerToScriptVariable, 0x464790, { .stackArguments = 1 });
    RH_ScopedInstall(DoDeathArrestCheck, 0x485A50);
    RH_ScopedInstall(SetCharCoordinates, 0x464DC0);
    RH_ScopedInstall(AddScriptToList, 0x464C00, { .stackArguments = 1 });
    RH_ScopedInstall(RemoveScriptFromList, 0x464BD0, { .stackArguments = 1 });
    RH_ScopedInstall(ShutdownThisScript, 0x465AA0);
    RH_ScopedInstall(IsPedDead, 0x464D70);
    RH_ScopedInstall(ThisIsAValidRandomPed, 0x489490);
    RH_ScopedInstall(ScriptTaskPickUpObject, 0x46AF50);
    RH_ScopedInstall(UpdateCompareFlag, 0x4859D0, { .stackArguments = 1 });
    RH_ScopedInstall(UpdatePC, 0x464DA0, { .stackArguments = 1 });
    RH_ScopedInstall(ProcessOneCommand, 0x469EB0);
    RH_ScopedInstall(ProcessCommands1700To1799, 0x496E00); // lote 238A (sin validar)
    RH_ScopedInstall(Process, 0x469F00);
    RH_ScopedInstall(ProcessCommands1200To1299, 0x48B590, { .stackArguments = 1 });
    RH_ScopedInstall(ProcessCommands1900To1999, 0x46B460, { .stackArguments = 1 });
    RH_ScopedInstall(ProcessCommands800To899, 0x481300, { .stackArguments = 1 }); // lote 239A (sin validar)
    RH_ScopedInstall(ProcessCommands300To399, 0x47C100, { .stackArguments = 1 }); // lote P6B (sin validar)
    RH_ScopedInstall(ProcessCommands400To499, 0x47D210, { .stackArguments = 1 }); // lote P6B (sin validar)
    RH_ScopedOverloadedInstall(GivePedScriptedTask, "OG", 0x465C20, void(CRunningScript::*)(int32, CTask*, int32));
}

//! Register our custom script command handlers
void CRunningScript::InjectCustomCommandHooks() {
    // Uncommenting any call will prevent it from being hooked, so
    // feel free to do so when debugging (Just don't forget to undo the changes!)

    namespace c = ::notsa::script::commands;
    c::basic::RegisterHandlers();
    c::camera::RegisterHandlers();
    c::character::RegisterHandlers();
    c::clock::RegisterHandlers();
    c::comparasion::RegisterHandlers();
    c::game::RegisterHandlers();
    c::generic::RegisterHandlers();
    c::math::RegisterHandlers();
    c::mission::RegisterHandlers();
    c::object::RegisterHandlers();
    c::pad::RegisterHandlers();
    c::ped::RegisterHandlers();
    c::player::RegisterHandlers();
    c::script::RegisterHandlers();
    c::sequence::RegisterHandlers();
    c::text::RegisterHandlers();
    c::unused::RegisterHandlers();
    c::utility::RegisterHandlers();
    c::vehicle::RegisterHandlers();
    c::zone::RegisterHandlers();
    c::stat::RegisterHandlers();
    c::conversation::RegisterHandlers();

#ifdef NOTSA_WITH_CLEO_SCRIPT_COMMANDS
    cleo::audiostream::RegisterHandlers();
    cleo::character::RegisterHandlers();
    cleo::dynamiclibrary::RegisterHandlers();
    cleo::fs::RegisterHandlers();
    cleo::game::RegisterHandlers();
    cleo::generic::RegisterHandlers();
    cleo::memory::RegisterHandlers();
    cleo::pad::RegisterHandlers();
    cleo::script::RegisterHandlers();
    cleo::vehicle::RegisterHandlers();
    cleo::world::RegisterHandlers();

    cleo::extensions::cleoplus::RegisterHandlers();
    cleo::extensions::clipboard::RegisterHandlers();
    cleo::extensions::fs::RegisterHandlers();
    cleo::extensions::imgui::RegisterHandlers();
    cleo::extensions::intoperations::RegisterHandlers();
#endif

#ifdef DUMP_CUSTOM_COMMAND_HANDLERS_TO_FILE
    auto reversed{0}, total{0};
    std::ofstream ofsrev{ "reversed_script_command_handlers.txt" }, ofsnotrev{ "NOT_reversed_script_command_handlers.txt" };
    for (auto&& [idx, handler] : rngv::enumerate(s_CustomCommandHandlerTable)) {
        const auto id = (eScriptCommands)(idx);
        ++total;
        if (handler) ++reversed;
        (handler ? ofsrev : ofsnotrev) << ::notsa::script::GetScriptCommandName(id) << '\n';
    }
    NOTSA_LOG_DEBUG("Comandos de script volcados; búscalos en `<directorio de GTA>/Scripts`");
    NOTSA_LOG_DEBUG("Progreso de ingeniería inversa de comandos de script: {}/{} ({:.2f}% hecho)", reversed, total, 100.0f * ((float)reversed / (float)total));
#endif
}

// 0x4648E0
void CRunningScript::Init() {
    // Como el original (0x4648E2..0x464902): copia exactamente 7 bytes ("noname" + NUL: dword+word+byte) y no toca el octavo
    std::memcpy(m_szName, "noname", 7);
    rng::fill(m_IPStack, nullptr);
    rng::fill(m_LocalVars, tScriptParam{ 0 });
    m_BaseIP                          = nullptr;
    m_pPrev                           = nullptr;
    m_pNext                           = nullptr;
    m_IP                              = nullptr;
    m_StackDepth                      = 0;
    m_WakeTime                        = 0;
    m_IsActive                        = false;
    m_CondResult                      = false;
    m_UsesMissionCleanup              = false;
    m_IsExternal                      = false;
    m_IsTextBlockOverride             = false;
    m_ExternalType                    = -1;
    m_AndOrState                      = 0;
    m_NotFlag                         = false;
    m_DoneDeathArrest                 = false;
    m_SceneSkipIP                     = 0;
    m_ThisMustBeTheOnlyMissionRunning = false;
    m_IsDeathArrestCheckEnabled       = true;
}

/*!
 * Adds script to list
 * @addr 0x464C00
 */
void CRunningScript::AddScriptToList(CRunningScript** queueList) {
    m_pNext = *queueList;
    m_pPrev = nullptr;
    if (*queueList)
        (*queueList)->m_pPrev = this;
    *queueList = this;
}

/*!
 * Removes script from list
 * @addr 0x464BD0
 */
void CRunningScript::RemoveScriptFromList(CRunningScript** queueList) {
    if (m_pPrev)
        m_pPrev->m_pNext = m_pNext;
    else
        *queueList = m_pNext;

    if (m_pNext)
        m_pNext->m_pPrev = m_pPrev;
}

/*!
 * Terminates a script
 * @addr 0x465AA0
 */
void CRunningScript::ShutdownThisScript() {
    m_IsActive = false;
    if (m_IsExternal) {
        const auto index = CTheScripts::StreamedScripts.GetStreamedScriptWithThisStartAddress(m_BaseIP);
        --CTheScripts::StreamedScripts.m_aScripts.data()[index].m_NumberOfUsers;
    }
    const auto reference = m_ThisMustBeTheOnlyMissionRunning
        ? CTheScripts::LocalVariablesForCurrentMission.data()[0].iParam
        : m_LocalVars.data()[0].iParam;
    switch (m_ExternalType) {
    case 0:
    case 2:
    case 3:
    case 5:
        if (auto* ped = GetPedPool()->GetAtRef(reference)) {
            ped->bHasAScriptBrain = false;
            if (m_ExternalType == 5) CScriptedBrainTaskStore::SetTask(ped, new CTaskSimpleFinishBrain);
        }
        break;
    case 1:
    case 4:
        if (auto* object = GetObjectPool()->GetAtRef(reference)) {
            object->m_nObjectFlags = (object->m_nObjectFlags & ~0x200000u) | 0x100000u;
        }
        break;
    }
}

// 0x465C20
void CRunningScript::GivePedScriptedTask(int32 pedHandle, CTask* task, int32 opcode) {
    if (pedHandle == -1) {
        CTaskSequences::AddTaskToActiveSequence(task);
        return;
    }

    CPed* ped = GetPedPool()->GetAtRef(pedHandle);
    assert(ped);
    CPedGroup* pedGroup = CPedGroups::GetPedsGroup(ped);

    CPed* otherPed = nullptr;
    if (m_ExternalType == 5 || m_ExternalType == 2 || !m_ExternalType || m_ExternalType == 3) {
        auto* localVariable = reinterpret_cast<int32*>(GetPointerToLocalVariable(0));
        otherPed = GetPedPool()->GetAtRef(*localVariable);
    }

    if (ped->bHasAScriptBrain && otherPed != ped) {
        delete task;
    } else if (otherPed && m_ExternalType == 5) {
        if (CScriptedBrainTaskStore::SetTask(ped, task)) {
            const int32 slot = CPedScriptedTaskRecord::GetVacantSlot();
            CPedScriptedTaskRecord::ms_scriptedTasks[slot].SetAsAttractorScriptTask(ped, opcode, task);
        }
    } else if (!pedGroup || ped->IsPlayer()) {
        CEventScriptCommand eventScriptCommand(TASK_PRIMARY_PRIMARY, task, false);
        auto* event = static_cast<CEventScriptCommand*>(ped->GetEventGroup().Add(&eventScriptCommand, false));
        if (event) {
            const int32 slot = CPedScriptedTaskRecord::GetVacantSlot();
            CPedScriptedTaskRecord::ms_scriptedTasks[slot].Set(ped, opcode, event);
        }
    } else {
        pedGroup->GetIntelligence().SetScriptCommandTask(ped, *task);
        CTask* scriptedTask = pedGroup->GetIntelligence().GetTaskScriptCommand(ped);
        const int32 slot = CPedScriptedTaskRecord::GetVacantSlot();
        CPedScriptedTaskRecord::ms_scriptedTasks[slot].SetAsGroupTask(ped, opcode, scriptedTask);
        delete task;
    }
}

void CRunningScript::GivePedScriptedTask(CPed* ped, CTask* task, int32 opcode) {
    GivePedScriptedTask(GetPedPool()->GetRef(ped), task, opcode); // Must do it like this, otherwise unhooking of the original `GivePedScriptedTask` will do nothing
}

// 0x470150
void CRunningScript::PlayAnimScriptCommand(int32 commandId) {
    bool canInterrupt = true, offsetPed = false;
    CollectParameters(1);
    const auto pedHandle = ScriptParams[0].iParam;
    char animName[24], blockName[16];
    ReadTextLabelFromScript(animName,24);
    ReadTextLabelFromScript(blockName,16);
    if (commandId == 0x88A) {
        CollectParameters(8);
        canInterrupt = ScriptParams[6].iParam != 0;
        offsetPed = ScriptParams[7].iParam != 0;
    } else if (commandId == 0x605 || commandId == 0x812 || commandId == 0xA1A) {
        if (commandId == 0x812) canInterrupt = false;
        CollectParameters(6);
    }
    const auto duration = ScriptParams[5].iParam;
    uint32 flags = ScriptParams[1].iParam || (duration > 0 && !ScriptParams[4].iParam) ? 0x12 : 0x10;
    if (ScriptParams[2].iParam) flags |= 0x40;
    if (ScriptParams[3].iParam) flags |= 0x80;
    if (!ScriptParams[4].iParam) flags |= 8;
    if (commandId == 0xA1A) flags |= 0x400;
    const bool sequence = CTaskSequences::ms_iActiveSequence >= 0;
    auto* memory = CTask::operator new(sizeof(CTaskSimpleRunNamedAnim));
    CTask* task = memory ? ::new(memory) CTaskSimpleRunNamedAnim(animName,blockName,flags,ScriptParams[0].fParam,uint32(duration > 0 ? duration : -1),!canInterrupt,sequence,offsetPed,false) : nullptr;
    if (commandId == 0xA1A) {
        memory = CTask::operator new(sizeof(CTaskSimpleAffectSecondaryBehaviour));
        task = memory ? ::new(memory) CTaskSimpleAffectSecondaryBehaviour(true,eSecondaryTask(4),task) : nullptr;
    }
    GivePedScriptedTask(pedHandle,task,commandId);
}

// 0x487A20
static void LocateScriptEntity(CRunningScript* script, const CVector& pos, bool threeD, bool object, bool forceFalse) {
    const auto centerX = ScriptParams[1].fParam;
    const auto centerY = ScriptParams[2].fParam;
    const auto radiusX = ScriptParams[threeD ? 4 : 3].fParam;
    const auto radiusY = ScriptParams[threeD ? 5 : 4].fParam;
    const auto lowerX = centerX - radiusX;
    bool inside = pos.x >= lowerX && pos.x <= centerX + radiusX && pos.y >= centerY - radiusY && pos.y <= centerY + radiusY;
    if (threeD) inside = inside && pos.z >= ScriptParams[3].fParam - ScriptParams[6].fParam && pos.z <= ScriptParams[3].fParam + ScriptParams[6].fParam;
    script->UpdateCompareFlag(!forceFalse && inside);
    if (ScriptParams[threeD ? 7 : 5].iParam) script->HighlightImportantArea(CVector2D{lowerX,centerY-radiusY}, CVector2D{centerX+radiusX,centerY+radiusY}, threeD ? ScriptParams[3].fParam : -100.0f);
}

// 0x487A20
void CRunningScript::LocateCarCommand(int32 commandId) {
    const bool threeD = commandId >= 0x1AF && commandId <= 0x1B0;
    CollectParameters(threeD ? 8 : 6);
    auto* vehicle = GetVehiclePool()->GetAtRef(ScriptParams[0].iParam);
    bool forceFalse = false;
    if (commandId == 0x1AE || commandId == 0x1B0) {
        const auto timestep = CTimer::GetTimeStep() < StaticRef<float>(0xB7CB54) ? CTimer::GetTimeStep() : StaticRef<float>(0xB7CB54);
        forceFalse = !(vehicle->m_fMovingSpeed <= timestep * 0.01f);
    }
    LocateScriptEntity(this, vehicle->GetPosition(), threeD, false, forceFalse);
}

// 0x486D80
static CVector ScriptPedLocation(CPed* ped) {
    if (ped->bInVehicle && ped->m_pVehicle) return ped->m_pVehicle->GetPosition();
    return ped->GetPosition();
}

// 0x486D80
void CRunningScript::LocateCharCommand(int32 commandId) {
    const bool threeD = commandId >= 0xFE && commandId <= 0x103;
    CollectParameters(threeD ? 8 : 6);
    auto* ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
    const auto pos = ScriptPedLocation(ped);
    bool forceFalse = false;
    switch (commandId) {
    case 0xEF: case 0xF0: case 0xF1: case 0x101: case 0x102: case 0x103:
        forceFalse = !CTheScripts::IsPedStopped(ped);
    }
    switch (commandId) {
    case 0xEC: case 0xEF: case 0xFE: case 0x101: break;
    case 0xED: case 0xF0: case 0xFF: case 0x102: forceFalse |= ped->bInVehicle; break;
    case 0xEE: case 0xF1: case 0x100: case 0x103: forceFalse |= !ped->bInVehicle; break;
    default: forceFalse = true;
    }
    LocateScriptEntity(this, pos, threeD, false, forceFalse);
}

// 0x487D10
void CRunningScript::LocateObjectCommand(int32 commandId) {
    const bool threeD = commandId == 0x4E6;
    CollectParameters(threeD ? 8 : 6);
    auto* object = GetObjectPool()->GetAtRef(ScriptParams[0].iParam);
    LocateScriptEntity(this, object->GetPosition(), threeD, true, false);
}

// 0x487420
static void LocateScriptPedPair(CRunningScript* script, CPed* ped, const CVector& target, bool threeD, int32 mode) {
    const auto pos = ScriptPedLocation(ped);
    const auto radiusX = ScriptParams[2].fParam;
    const auto radiusY = ScriptParams[3].fParam;
    const auto lowerX = target.x - radiusX;
    bool inside = pos.x >= lowerX && pos.x <= target.x + radiusX && pos.y >= target.y - radiusY && pos.y <= target.y + radiusY;
    if (threeD) inside = inside && pos.z >= target.z - ScriptParams[4].fParam && pos.z <= target.z + ScriptParams[4].fParam;
    if (mode == 1) inside &= !ped->bInVehicle;
    else if (mode == 2) inside &= ped->bInVehicle;
    else if (mode != 0) inside = false;
    script->UpdateCompareFlag(inside);
    if (ScriptParams[threeD ? 5 : 4].iParam) script->HighlightImportantArea(CVector2D{lowerX,target.y-radiusY}, CVector2D{target.x+radiusX,target.y+radiusY}, threeD ? target.z : -100.0f);
}

// 0x487420
void CRunningScript::LocateCharCarCommand(int32 commandId) {
    const bool threeD = commandId >= 0x205 && commandId <= 0x207;
    CollectParameters(threeD ? 6 : 5);
    auto* ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
    const auto target = GetVehiclePool()->GetAtRef(ScriptParams[1].iParam)->GetPosition();
    const auto mode = threeD ? commandId - 0x205 : commandId - 0x202;
    LocateScriptPedPair(this, ped, target, threeD, mode);
}

// 0x4870F0
void CRunningScript::LocateCharCharCommand(int32 commandId) {
    const bool threeD = commandId >= 0x104 && commandId <= 0x106;
    CollectParameters(threeD ? 6 : 5);
    auto* ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
    const auto target = ScriptPedLocation(GetPedPool()->GetAtRef(ScriptParams[1].iParam));
    const auto mode = threeD ? commandId - 0x104 : commandId - 0xF2;
    LocateScriptPedPair(this, ped, target, threeD, mode);
}

// 0x487720
void CRunningScript::LocateCharObjectCommand(int32 commandId) {
    const bool threeD = commandId >= 0x474 && commandId <= 0x476;
    CollectParameters(threeD ? 6 : 5);
    auto* ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
    const auto target = GetObjectPool()->GetAtRef(ScriptParams[1].iParam)->GetPosition();
    const auto mode = threeD ? commandId - 0x474 : commandId - 0x471;
    LocateScriptPedPair(this, ped, target, threeD, mode);
}

// 0x488EC0
static void ScriptAreaTest(CRunningScript* script, const CVector& pos, bool threeD, bool forceFalse) {
    float minX = ScriptParams[1].fParam, minY = ScriptParams[2].fParam;
    float maxX = ScriptParams[threeD ? 4 : 3].fParam, maxY = ScriptParams[threeD ? 5 : 4].fParam;
    float minZ = 0.0f, maxZ = 0.0f;
    if (threeD) {
        minZ = ScriptParams[3].fParam;
        maxZ = ScriptParams[6].fParam;
        if (maxZ < minZ) std::swap(minZ, maxZ);
    }
    if (maxX < minX) std::swap(minX, maxX);
    if (maxY < minY) std::swap(minY, maxY);
    const auto inside = !forceFalse && pos.x >= minX && pos.x <= maxX && pos.y >= minY && pos.y <= maxY && (!threeD || (pos.z >= minZ && pos.z <= maxZ));
    script->UpdateCompareFlag(inside);
    if (ScriptParams[threeD ? 7 : 5].iParam) script->HighlightImportantArea(CVector2D{minX,minY}, CVector2D{maxX,maxY}, threeD ? (maxZ + minZ) * 0.5f : -100.0f);
}

// 0x488EC0
void CRunningScript::CarInAreaCheckCommand(int32 commandId) {
    const bool threeD = commandId == 0xB1 || commandId == 0x1AC;
    CollectParameters(threeD ? 8 : 6);
    auto* vehicle = GetVehiclePool()->GetAtRef(ScriptParams[0].iParam);
    const auto forceFalse = (commandId == 0x1AB || commandId == 0x1AC) && !CTheScripts::IsVehicleStopped(vehicle);
    ScriptAreaTest(this, vehicle->GetPosition(), threeD, forceFalse);
}

// 0x488B50
void CRunningScript::CharInAreaCheckCommand(int32 commandId) {
    const bool threeD = commandId == 0xA4 || (commandId >= 0x1A6 && commandId <= 0x1AA);
    CollectParameters(threeD ? 8 : 6);
    auto* ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
    const auto pos = ScriptPedLocation(ped);
    bool forceFalse = false;
    switch (commandId) {
    case 0x1A3: case 0x1A4: case 0x1A5: case 0x1A8: case 0x1A9: case 0x1AA: forceFalse = !CTheScripts::IsPedStopped(ped);
    }
    switch (commandId) {
    case 0xA3: case 0xA4: case 0x1A3: case 0x1A8: break;
    case 0x1A1: case 0x1A4: case 0x1A6: case 0x1A9: forceFalse |= ped->bInVehicle; break;
    case 0x1A2: case 0x1A5: case 0x1A7: case 0x1AA: forceFalse |= !ped->bInVehicle; break;
    default: forceFalse = true;
    }
    ScriptAreaTest(this, pos, threeD, forceFalse);
}

// 0x489150
void CRunningScript::ObjectInAreaCheckCommand(int32 commandId) {
    const bool threeD = commandId == 0x4EA;
    CollectParameters(threeD ? 8 : 6);
    auto* object = GetObjectPool()->GetAtRef(ScriptParams[0].iParam);
    ScriptAreaTest(this, object->GetPosition(), threeD, false);
}

namespace {
struct ScriptAngledBox {
    CVector2D a, b, c, d, ab, ac;
    float abLength, acLength, minZ, maxZ;
    bool threeD, draw;
};
ScriptAngledBox MakeScriptAngledBox(bool threeD, int offset) {
    ScriptAngledBox box{};
    box.threeD = threeD;
    box.a = CVector2D{ScriptParams[offset].fParam, ScriptParams[offset+1].fParam};
    const int end = offset + (threeD ? 3 : 2);
    box.b = CVector2D{ScriptParams[end].fParam, ScriptParams[end+1].fParam};
    if (threeD) {
        box.minZ = ScriptParams[offset+2].fParam;
        box.maxZ = ScriptParams[end+2].fParam;
        if (box.maxZ < box.minZ) std::swap(box.minZ,box.maxZ);
    }
    const float angledWidth = ScriptParams[offset+(threeD?6:4)].fParam;
    box.draw = ScriptParams[offset+(threeD?7:5)].iParam != 0;
    const float startX=box.a.x, startY=box.a.y, endX=box.b.x, endY=box.b.y;
    auto radianFunction = static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints);
    // Como el original (0x487F60): ángulo entre los puntos + pi/2 normalizado a [0, 2pi] y esquinas con fsin/fcos (X87Math.cpp)
    const float angle = radianFunction(startX,startY,endX,endY);
    float corners[4];
    ScriptAngledCornersCpp(angle,startX,startY,endX,endY,angledWidth,corners);
    const float cornerCX=corners[0],cornerCY=corners[1],cornerDX=corners[2],cornerDY=corners[3];
    box.c=CVector2D{cornerCX,cornerCY};box.d=CVector2D{cornerDX,cornerDY};
    box.ab=box.b-box.a;box.ac=box.c-box.a;
    box.abLength=std::sqrt(box.ab.x*box.ab.x+box.ab.y*box.ab.y);
    box.acLength=std::sqrt(box.ac.x*box.ac.x+box.ac.y*box.ac.y);
    return box;
}
void ScriptNormalizeAngled(CVector2D& v) {
    const float sq=v.x*v.x+v.y*v.y;
    if(sq>0.0f){const float inv=1.0f/std::sqrt(sq);v.x*=inv;v.y*=inv;}else v.x=1.0f;
}
bool ScriptInsideAngled(ScriptAngledBox& box,const CVector& pos,bool flame) {
    ScriptNormalizeAngled(box.ab);
    const float x=pos.x-box.a.x,y=pos.y-box.a.y;
    const float first=x*box.ab.x+y*box.ab.y;
    if(!(first>=0.0f&&first<=box.abLength))return false;
    if(flame)ScriptNormalizeAngled(box.ac);else box.ac.Normalise();
    const float second=x*box.ac.x+y*box.ac.y;
    return second>=0.0f&&second<=box.acLength&&(!box.threeD||(pos.z>=box.minZ&&pos.z<=box.maxZ));
}
void ScriptDrawAngled(CRunningScript* script,const ScriptAngledBox& box) {
    if(box.draw)CTheScripts::HighlightImportantAngledArea(uint32(uintptr_t(script)+uintptr_t(script->m_IP)),box.a.x,box.a.y,box.b.x,box.b.y,box.d.x,box.d.y,box.c.x,box.c.y,box.threeD?(box.minZ+box.maxZ)*0.5f:-100.0f);
}
}
// 0x487F60
void CRunningScript::CharInAngledAreaCheckCommand(int32 commandId) {
    const bool threeD=commandId>=0x5FC&&commandId<=0x601;
    CollectParameters(threeD?9:7);
    auto* ped=GetPedPool()->GetAtRef(ScriptParams[0].iParam);
    bool stopped=true;
    if((commandId>=0x5F9&&commandId<=0x5FB)||(commandId>=0x5FF&&commandId<=0x601))stopped=CTheScripts::IsPedStopped(ped);
    auto box=MakeScriptAngledBox(threeD,1);
    bool inside=stopped&&ScriptInsideAngled(box,ScriptPedLocation(ped),false);
    switch(commandId){
    case 0x5F7:case 0x5FA:case 0x5FD:case 0x600:inside=inside&&!ped->bInVehicle;break;
    case 0x5F8:case 0x5FB:case 0x5FE:case 0x601:inside=inside&&ped->bInVehicle;break;
    case 0x5F6:case 0x5F9:case 0x5FC:case 0x5FF:break;
    default:inside=false;break;
    }
    UpdateCompareFlag(inside);ScriptDrawAngled(this,box);
}
// 0x488780
void CRunningScript::FlameInAngledAreaCheckCommand(int32 commandId) {
    const bool threeD=commandId==0x72E;
    CollectParameters(threeD?8:6);
    auto box=MakeScriptAngledBox(threeD,0);
    bool inside=false;
    for(uint16 i=0;i<100&&!inside;++i){CVector pos;if(CShotInfo::GetFlameThrowerShotPosn(uint8(i),pos))inside=ScriptInsideAngled(box,pos,true);}
    UpdateCompareFlag(inside);ScriptDrawAngled(this,box);
}
// 0x4883F0
void CRunningScript::ObjectInAngledAreaCheckCommand(int32 commandId) {
    const bool threeD=commandId==0x8E4;
    CollectParameters(threeD?9:7);
    auto* object=GetObjectPool()->GetAtRef(ScriptParams[0].iParam);
    auto box=MakeScriptAngledBox(threeD,1);
    const bool inside=ScriptInsideAngled(box,object->GetPosition(),false);
    UpdateCompareFlag(inside);ScriptDrawAngled(this,box);
}

// 0x464D70
bool CRunningScript::IsPedDead(CPed* ped) const {
    ePedState pedState = ped->m_nPedState;
    return pedState == PEDSTATE_DEAD || pedState == PEDSTATE_DIE || pedState == PEDSTATE_DIE_BY_STEALTH;
}

// 0x489490
bool CRunningScript::ThisIsAValidRandomPed(ePedType pedType, bool civilian, bool gang, bool criminal) {
    switch (pedType) {
    case PED_TYPE_CIVMALE:
    case PED_TYPE_CIVFEMALE:
        return civilian;
    case PED_TYPE_GANG1:
    case PED_TYPE_GANG2:
    case PED_TYPE_GANG3:
    case PED_TYPE_GANG4:
    case PED_TYPE_GANG5:
    case PED_TYPE_GANG6:
    case PED_TYPE_GANG7:
    case PED_TYPE_GANG8:
    case PED_TYPE_GANG9:
        return gang;
    case PED_TYPE_CRIMINAL:
    case PED_TYPE_PROSTITUTE:
        return criminal;
    default:
        return false;
    }
}

// 0x485A50
void CRunningScript::DoDeathArrestCheck() {
    if (!m_IsDeathArrestCheckEnabled) {
        return;
    }

    if (!CTheScripts::IsPlayerOnAMission()) {
        return;
    }

    if (const auto& pi = FindPlayerInfo(); !pi.IsRestartingAfterDeath() && !pi.IsRestartingAfterArrest()) {
        return;
    }

    // Como el original (0x485ABA..0x485ADB): la pila se colapsa a 1 (si era > 1), se decrementa (palabra) y la IP pasa a
    // m_IPStack[profundidad] (es decir, la primera direccion de retorno); NO se vacia la pila.
    if (m_StackDepth > 1) {
        m_StackDepth = 1;
    }
    --m_StackDepth;
    m_IP = m_IPStack.data()[m_StackDepth];
    CMessages::ClearSmallMessagesOnly();
    memset(&CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag], 0, sizeof(uint32));

    m_DoneDeathArrest = true;
    m_WakeTime        = 0;
}

// 0x464F50
void CRunningScript::GetCorrectPedModelIndexForEmergencyServiceType(ePedType pedType, uint32* typeSpecificModelId) {
    switch (*typeSpecificModelId) {
    case MODEL_LAPD1:
    case MODEL_SFPD1:
    case MODEL_LVPD1:
    case MODEL_LAPDM1:
        if (pedType == PED_TYPE_COP) {
            *typeSpecificModelId = COP_TYPE_CITYCOP;
        }
        break;
    case MODEL_CSHER:
        if (pedType == PED_TYPE_COP) {
            *typeSpecificModelId = COP_TYPE_CSHER;
        }
        break;
    case MODEL_SWAT:
        if (pedType == PED_TYPE_COP) {
            *typeSpecificModelId = COP_TYPE_SWAT1;
        }
        break;
    case MODEL_FBI:
        if (pedType == PED_TYPE_COP) {
            *typeSpecificModelId = COP_TYPE_FBI;
        }
        break;
    case MODEL_ARMY:
        if (pedType == PED_TYPE_COP) {
            *typeSpecificModelId = COP_TYPE_ARMY;
        }
        break;
    default:
        return;
    }
}

// Returns state of pad button
// 0x485B10
int16 CRunningScript::GetPadState(uint16 playerIndex, eButtonId buttonId) {
    const auto* pad = CPad::GetPad(playerIndex);
    switch (buttonId) {
    case BUTTON_LEFT_STICK_X:    return pad->NewState.LeftStickX;
    case BUTTON_LEFT_STICK_Y:    return pad->NewState.LeftStickY;
    case BUTTON_RIGHT_STICK_X:   return pad->NewState.RightStickX;
    case BUTTON_RIGHT_STICK_Y:   return pad->NewState.RightStickY;
    case BUTTON_LEFT_SHOULDER1:  return pad->NewState.LeftShoulder1;
    case BUTTON_LEFT_SHOULDER2:  return pad->NewState.LeftShoulder2;
    case BUTTON_RIGHT_SHOULDER1: return pad->NewState.RightShoulder1;
    case BUTTON_RIGHT_SHOULDER2: return pad->NewState.RightShoulder2;
    case BUTTON_DPAD_UP:         return pad->NewState.DPadUp;
    case BUTTON_DPAD_DOWN:       return pad->NewState.DPadDown;
    case BUTTON_DPAD_LEFT:       return pad->NewState.DPadLeft;
    case BUTTON_DPAD_RIGHT:      return pad->NewState.DPadRight;
    case BUTTON_START:           return pad->NewState.Start;
    case BUTTON_SELECT:          return pad->NewState.Select;
    case BUTTON_SQUARE:          return pad->NewState.ButtonSquare;
    case BUTTON_TRIANGLE:        return pad->NewState.ButtonTriangle;
    case BUTTON_CROSS:           return pad->NewState.ButtonCross;
    case BUTTON_CIRCLE:          return pad->NewState.ButtonCircle;
    case BUTTON_LEFTSHOCK:       return pad->NewState.ShockButtonL;
    case BUTTON_RIGHTSHOCK:      return pad->NewState.ShockButtonR;
    default:                     return OR_CONTINUE;
    }
}

// 0x46AF50
void CRunningScript::ScriptTaskPickUpObject(int32 commandId) {
    CollectParameters(7);
    const auto pedHandle = ScriptParams[0].iParam;
    auto* object = GetObjectPool()->GetAtRef(ScriptParams[1].iParam);
    const CVector offset{ScriptParams[2].fParam, ScriptParams[3].fParam, ScriptParams[4].fParam};
    const auto bone = uint8(ScriptParams[5].iParam);
    const auto boneFlags = uint8(ScriptParams[6].iParam);
    char anim[24], block[16];
    ReadTextLabelFromScript(anim, sizeof(anim));
    ReadTextLabelFromScript(block, sizeof(block));
    const bool useDefault = !std::strcmp(anim, "NULL") || !std::strcmp(block, "NULL");
    CollectParameters(1);
    const auto flags = eAnimationFlags(ScriptParams[0].iParam == 0 ? 0x18 : 0x10);
    CTask* task = nullptr;
    if (auto* memory = CTask::operator new(sizeof(CTaskSimpleHoldEntity))) {
        task = useDefault
            ? static_cast<CTask*>(::new(memory) CTaskSimpleHoldEntity(object, &offset, bone, boneFlags, AnimationId(0xBF), AssocGroupId(0), false))
            : static_cast<CTask*>(::new(memory) CTaskSimpleHoldEntity(object, &offset, bone, boneFlags, anim, block, flags));
    }
    if (pedHandle == -1) {
        if (commandId == 0x70A) {
            auto* memory = CTask::operator new(sizeof(CTaskSimpleAffectSecondaryBehaviour));
            task = memory ? ::new(memory) CTaskSimpleAffectSecondaryBehaviour(true, eSecondaryTask(4), task) : nullptr;
            CTaskSequences::GetActiveSequence().AddTask(task);
        }
    } else {
        auto* ped = GetPedPool()->GetAtRef(pedHandle);
        ped->GetTaskManager().SetTaskSecondary(task, eSecondaryTask(commandId == 0x70A ? 4 : 0));
        CPedScriptedTaskRecord::ms_scriptedTasks[CPedScriptedTaskRecord::GetVacantSlot()].Set(ped, commandId, task);
    }
}

// 0x464DC0
void CRunningScript::SetCharCoordinates(CPed& ped, CVector posn, bool warpGang, bool offset) {
    // Como el original (0x464DDE..0x464DED): fcomp contra -100.0 y test ah,41h/jp: se llama a FindGroundZForCoord solo si z <= -100.0
    // (C3 o C0); con z NaN (C3=C2=C0=1 -> ah&41h = 41h, paridad par) se salta la llamada y z sigue siendo NaN.
    if (posn.z <= MAP_Z_LOW_LIMIT) {
        posn.z = CWorld::FindGroundZForCoord(posn.x, posn.y);
    }

    CVehicle* vehicle = ped.GetVehicleIfInOne();
    if (vehicle) {
        posn.z += vehicle->GetDistanceFromCentreOfMassToBaseOfModel();
        vehicle->Teleport(posn, false);
        CTheScripts::ClearSpaceForMissionEntity(posn, vehicle);
    } else {
        // Como el original (0x464E80..0x464E93): solo suma la distancia si offset (sumar 0.0f convertiria -0.0 en +0.0).
        if (offset) {
            posn.z += ped.GetDistanceFromCentreOfMassToBaseOfModel();
        }
        CTheScripts::ClearSpaceForMissionEntity(posn, &ped);
        auto* group = CPedGroups::GetPedsGroup(&ped);
        if (group && group->GetMembership().IsLeader(&ped) && warpGang) {
            group->Teleport(posn);
        } else {
            ped.Teleport(posn, false);
        }
    }
}

// 0x463CA0
tScriptParam* CRunningScript::GetPointerToLocalVariable(int32 loc) {
    // Como el original (0x463CA0): aritmetica de punteros de 32 bits sin comprobar rangos ni truncar a 16 bits
    return m_ThisMustBeTheOnlyMissionRunning
        ? &CTheScripts::LocalVariablesForCurrentMission.data()[loc]
        : &m_LocalVars.data()[loc];
}

/*!
 * @addr 0x463CC0
 * @brief Returns pointer to a local script variable.
 *
 * @param arrayBaseOffset          The offset of the array
 * @param index                    Index of the variable inside the array
 * @param arrayEntriesSizeAsParams Size of 1 variable in the array (In terms of `tScriptParam`'s - So for a regular `int` (or float, etc) variable this will be `1`, for long strings it's `4` and for short one's it's `2`)
 */
tScriptParam* CRunningScript::GetPointerToLocalArrayElement(int32 arrayBaseOffset, uint16 index, uint8 arrayEntriesSizeAsParams) {
    // Como el original (0x463CC0): base + index * size en 32 bits (index y size sin signo), sin truncar a 16 bits
    return GetPointerToLocalVariable(arrayBaseOffset + int32(index) * int32(arrayEntriesSizeAsParams));
}

/*!
 * Returns pointer to script variable of any type.
 * @addr 0x464790
 */
tScriptParam* CRunningScript::GetPointerToScriptVariable(eScriptVariableType) {
    uint8  arrElemSize;
    uint16 arrVarOffset;
    int32  arrElemIdx;

    int8 type = CTheScripts::Read1ByteFromScript(m_IP);
    switch (type) {
    case SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE:
    case SCRIPT_PARAM_GLOBAL_SHORT_STRING_VARIABLE:
    case SCRIPT_PARAM_GLOBAL_LONG_STRING_VARIABLE:
    {
        uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
        return reinterpret_cast<tScriptParam*>(&CTheScripts::ScriptSpace[index]);
    }
    case SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE:
    case SCRIPT_PARAM_LOCAL_SHORT_STRING_VARIABLE:
    case SCRIPT_PARAM_LOCAL_LONG_STRING_VARIABLE:
    {
        uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
        return GetPointerToLocalVariable(index);
    }

    case SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY:
    case SCRIPT_PARAM_GLOBAL_SHORT_STRING_ARRAY:
    case SCRIPT_PARAM_GLOBAL_LONG_STRING_ARRAY:
        ReadArrayInformation(true, &arrVarOffset, &arrElemIdx);
        if (type == SCRIPT_PARAM_GLOBAL_LONG_STRING_ARRAY)
            return reinterpret_cast<tScriptParam*>(&CTheScripts::ScriptSpace[LONG_STRING_SIZE * arrElemIdx + arrVarOffset]);
        else if (type == SCRIPT_PARAM_GLOBAL_SHORT_STRING_ARRAY)
            return reinterpret_cast<tScriptParam*>(&CTheScripts::ScriptSpace[SHORT_STRING_SIZE * arrElemIdx + arrVarOffset]);
        else // SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY
            return reinterpret_cast<tScriptParam*>(&CTheScripts::ScriptSpace[4 * arrElemIdx + arrVarOffset]);

    case SCRIPT_PARAM_LOCAL_NUMBER_ARRAY:
    case SCRIPT_PARAM_LOCAL_SHORT_STRING_ARRAY:
    case SCRIPT_PARAM_LOCAL_LONG_STRING_ARRAY:
        ReadArrayInformation(true, &arrVarOffset, &arrElemIdx);
        if (type == SCRIPT_PARAM_LOCAL_LONG_STRING_ARRAY)
            arrElemSize = 4;
        else if (type == SCRIPT_PARAM_LOCAL_SHORT_STRING_ARRAY)
            arrElemSize = 2;
        else // SCRIPT_PARAM_LOCAL_NUMBER_ARRAY
            arrElemSize = 1;
        return GetPointerToLocalArrayElement(arrVarOffset, arrElemIdx, arrElemSize);

    default:
        return nullptr; // Como el original (0x4648A7: `xor eax,eax`)
    }
}

/*!
 * Returns offset of global variable
 * @addr 0x464700
 */
uint16 CRunningScript::GetIndexOfGlobalVariable() {
    switch (const auto t = GetAtIPAs<uint8>()) {
    case SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE:
        return GetAtIPAs<uint16>();
    case SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY: {
        uint16 base;
        int32  idx;
        ReadArrayInformation(true, &base, &idx);
        return base + sizeof(tScriptParam) * idx;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x464080
void CRunningScript::CollectParameters(int16 count) {
    uint16 arrVarOffset;
    int32  arrElemIdx;

    for (auto i = 0; i < count; i++) {
        switch (CTheScripts::Read1ByteFromScript(m_IP)) {
        case SCRIPT_PARAM_STATIC_INT_32BITS:
            ScriptParams[i].iParam = CTheScripts::Read4BytesFromScript(m_IP);
            break;
        case SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE:
        {
            uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
            ScriptParams[i].iParam = *reinterpret_cast<int32*>(&CTheScripts::ScriptSpace[index]);
            break;
        }
        case SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE:
        {
            uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
            ScriptParams[i] = *GetPointerToLocalVariable(index);
            break;
        }
        case SCRIPT_PARAM_STATIC_INT_8BITS:
            ScriptParams[i].iParam = CTheScripts::Read1ByteFromScript(m_IP);
            break;
        case SCRIPT_PARAM_STATIC_INT_16BITS:
            ScriptParams[i].iParam = CTheScripts::Read2BytesFromScript(m_IP);
            break;
        case SCRIPT_PARAM_STATIC_FLOAT:
            ScriptParams[i].fParam = CTheScripts::ReadFloatFromScript(m_IP);
            break;
        case SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY:
            ReadArrayInformation(true, &arrVarOffset, &arrElemIdx);
            ScriptParams[i].iParam = *reinterpret_cast<int32*>(&CTheScripts::ScriptSpace[arrVarOffset + 4 * arrElemIdx]);
            break;
        case SCRIPT_PARAM_LOCAL_NUMBER_ARRAY:
            ReadArrayInformation(true, &arrVarOffset, &arrElemIdx);
            ScriptParams[i] = *GetPointerToLocalArrayElement(arrVarOffset, arrElemIdx, 1);
            break;
        }
    }
}

/*!
 * Collects parameter and returns it.
 * @addr 0x464250
 */
int32 CRunningScript::CollectNextParameterWithoutIncreasingPC() {
    uint16 arrVarOffset;
    int32  arrElemIdx;
    uint8* ip = m_IP;
    int32  result = -1;

    switch (CTheScripts::Read1ByteFromScript(m_IP)) {
    case SCRIPT_PARAM_STATIC_INT_32BITS:
    case SCRIPT_PARAM_STATIC_FLOAT:
        result = CTheScripts::Read4BytesFromScript(m_IP);
        break;
    case SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE:
    {
        uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
        result = *reinterpret_cast<int32*>(&CTheScripts::ScriptSpace[index]);
        break;
    }
    case SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE:
    {
        uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
        result = GetPointerToLocalVariable(index)->iParam;
        break;
    }
    case SCRIPT_PARAM_STATIC_INT_8BITS:
        result = CTheScripts::Read1ByteFromScript(m_IP);
        break;
    case SCRIPT_PARAM_STATIC_INT_16BITS:
        result = CTheScripts::Read2BytesFromScript(m_IP);
        break;
    case SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY:
        ReadArrayInformation(false, &arrVarOffset, &arrElemIdx);
        result = *reinterpret_cast<int32*>(&CTheScripts::ScriptSpace[arrVarOffset + 4 * arrElemIdx]);
        break;
    case SCRIPT_PARAM_LOCAL_NUMBER_ARRAY:
        ReadArrayInformation(false, &arrVarOffset, &arrElemIdx);
        result = GetPointerToLocalArrayElement(arrVarOffset, arrElemIdx, 1)->iParam;
        break;
    }

    m_IP = ip;
    return result;
}

/*!
 * @addr 0x464370
 */
void CRunningScript::StoreParameters(int16 count) {
    uint16 arrVarOffset;
    int32  arrElemIdx;

    for (auto i = 0; i < count; i++) {
        switch (CTheScripts::Read1ByteFromScript(m_IP)) {
        case SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE:
        {
            uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
            *reinterpret_cast<int32*>(&CTheScripts::ScriptSpace[index]) = ScriptParams[i].iParam;
            break;
        }
        case SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE:
        {
            uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
            *GetPointerToLocalVariable(index) = ScriptParams[i];
            break;
        }
        case SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY:
            ReadArrayInformation(true, &arrVarOffset, &arrElemIdx);
            *reinterpret_cast<int32*>(&CTheScripts::ScriptSpace[arrVarOffset + 4 * arrElemIdx]) = ScriptParams[i].iParam;
            break;
        case SCRIPT_PARAM_LOCAL_NUMBER_ARRAY:
            ReadArrayInformation(true, &arrVarOffset, &arrElemIdx);
            *GetPointerToLocalArrayElement(arrVarOffset, arrElemIdx, 1) = ScriptParams[i];
            break;
        }
    }
}

// Reads array var base offset and element index from index variable.
// 0x463CF0
void CRunningScript::ReadArrayInformation(int32 updateIP, uint16* outArrayBase, int32* outArrayIndex) {
    const auto op = GetAtIPAs<scm::ArrayAccess>(updateIP);
    *outArrayIndex = op.IdxVarIsGlobal
        ? *reinterpret_cast<int32*>(&CTheScripts::ScriptSpace.data()[op.IdxVarLoc])
        : GetPointerToLocalVariable(op.IdxVarLoc)->iParam;
    *outArrayBase = op.ArrayBase;
}

// Collects parameters and puts them to local variables of new script
// 0x464500
void CRunningScript::ReadParametersForNewlyStartedScript(CRunningScript* newScript) {
    uint16 arrVarOffset;
    int32  arrElemIdx;
    int8   type = CTheScripts::Read1ByteFromScript(m_IP);

    for (int i = 0; type != SCRIPT_PARAM_END_OF_ARGUMENTS; type = CTheScripts::Read1ByteFromScript(m_IP), i++) {
        switch (type) {
        case SCRIPT_PARAM_STATIC_INT_32BITS:
            newScript->m_LocalVars[i].iParam = CTheScripts::Read4BytesFromScript(m_IP);
            break;
        case SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE:
        {
            uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
            newScript->m_LocalVars[i].iParam = *reinterpret_cast<int32*>(&CTheScripts::ScriptSpace[index]);
            break;
        }
        case SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE:
        {
            uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
            newScript->m_LocalVars[i] = *GetPointerToLocalVariable(index);
            break;
        }
        case SCRIPT_PARAM_STATIC_INT_8BITS:
            newScript->m_LocalVars[i].iParam = CTheScripts::Read1ByteFromScript(m_IP);
            break;
        case SCRIPT_PARAM_STATIC_INT_16BITS:
            newScript->m_LocalVars[i].iParam = CTheScripts::Read2BytesFromScript(m_IP);
            break;
        case SCRIPT_PARAM_STATIC_FLOAT:
            newScript->m_LocalVars[i].fParam = CTheScripts::ReadFloatFromScript(m_IP);
            break;
        case SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY:
            ReadArrayInformation(true, &arrVarOffset, &arrElemIdx);
            newScript->m_LocalVars[i].iParam = *reinterpret_cast<int32*>(&CTheScripts::ScriptSpace[arrVarOffset + 4 * arrElemIdx]);
            break;
        case SCRIPT_PARAM_LOCAL_NUMBER_ARRAY:
            ReadArrayInformation(true, &arrVarOffset, &arrElemIdx);
            newScript->m_LocalVars[i] = *GetPointerToLocalArrayElement(arrVarOffset, arrElemIdx, 1);
            break;
        default:
            break;
        }
    }
}

// Collects string parameter
// 0x463D50
void CRunningScript::ReadTextLabelFromScript(char* buffer, uint8 nBufferLength) {
    // Como el original (0x463D50): TODAS las copias desde variables/arrays usan strncpy del CRT (0x821F40): copian hasta
    // N caracteres, paran en NUL y rellenan con ceros hasta N, sin terminar la cadena ni validar el tamano del destino
    // (la DLL usaba strncpy_s, que rellena con 0xFE en depuracion y falla con cadenas de 8/16 caracteres).
    uint16 arrVarOffset;
    int32  arrElemIdx;
#pragma warning(push)
#pragma warning(disable : 4996)
    const auto copy = [&](const void* src, size_t n) { std::strncpy(buffer, static_cast<const char*>(src), n); };
#pragma warning(pop)
    // 0x463E20 / 0x463F34 / 0x463FB8: el recuento largo es min(nBufferLength, 16) sin signo
    const size_t longCount = std::min<uint8>(nBufferLength, LONG_STRING_SIZE);

    int8 type = CTheScripts::Read1ByteFromScript(m_IP);
    switch (type) {
    case SCRIPT_PARAM_STATIC_SHORT_STRING:
        for (auto i = 0; i < SHORT_STRING_SIZE; i++)
            buffer[i] = CTheScripts::Read1ByteFromScript(m_IP);
        break;

    case SCRIPT_PARAM_GLOBAL_SHORT_STRING_VARIABLE:
    {
        uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
        copy(&CTheScripts::ScriptSpace[index], SHORT_STRING_SIZE);
        break;
    }

    case SCRIPT_PARAM_LOCAL_SHORT_STRING_VARIABLE:
    {
        uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
        copy(GetPointerToLocalVariable(index), SHORT_STRING_SIZE);
        break;
    }

    case SCRIPT_PARAM_GLOBAL_SHORT_STRING_ARRAY:
    case SCRIPT_PARAM_GLOBAL_LONG_STRING_ARRAY:
        ReadArrayInformation(true, &arrVarOffset, &arrElemIdx);
        if (type == SCRIPT_PARAM_GLOBAL_SHORT_STRING_ARRAY)
            copy(&CTheScripts::ScriptSpace[SHORT_STRING_SIZE * arrElemIdx + arrVarOffset], SHORT_STRING_SIZE);
        else
            copy(&CTheScripts::ScriptSpace[LONG_STRING_SIZE * arrElemIdx + arrVarOffset], longCount);
        break;

    case SCRIPT_PARAM_LOCAL_SHORT_STRING_ARRAY:
    case SCRIPT_PARAM_LOCAL_LONG_STRING_ARRAY:
        ReadArrayInformation(true, &arrVarOffset, &arrElemIdx);
        if (type == SCRIPT_PARAM_LOCAL_SHORT_STRING_ARRAY)
            copy(GetPointerToLocalArrayElement(arrVarOffset, arrElemIdx, 2), SHORT_STRING_SIZE);
        else
            copy(GetPointerToLocalArrayElement(arrVarOffset, arrElemIdx, 4), longCount);
        break;

    case SCRIPT_PARAM_STATIC_PASCAL_STRING:
    {
        int16 nStringLen = CTheScripts::Read1ByteFromScript(m_IP); // sign extension. max size = 127, not 255
        for (auto i = 0; i < nStringLen; i++)
            buffer[uint8(i)] = CTheScripts::Read1ByteFromScript(m_IP);

        // Como el original (0x463EFD: `cmp al,cl / jae`): la comparacion es SIN signo sobre el byte bajo de la longitud
        // (con una longitud negativa no se rellena nada).
        if (uint8(nStringLen) < nBufferLength)
            memset(&buffer[uint8(nStringLen)], 0, uint8(nBufferLength - uint8(nStringLen)));
        break;
    }

    case SCRIPT_PARAM_STATIC_LONG_STRING:
        // Como el original (0x463F30): copia en bruto min(nBufferLength, 16) bytes (sin parar en NUL ni terminar) y
        // avanza siempre 16 bytes.
        for (size_t i = 0; i < longCount; ++i)
            buffer[i] = static_cast<char>(m_IP[i]);
        m_IP += LONG_STRING_SIZE;
        break;

    case SCRIPT_PARAM_GLOBAL_LONG_STRING_VARIABLE:
    {
        uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
        copy(&CTheScripts::ScriptSpace[index], longCount);
        break;
    }

    case SCRIPT_PARAM_LOCAL_LONG_STRING_VARIABLE:
    {
        uint16 index = CTheScripts::Read2BytesFromScript(m_IP);
        copy(GetPointerToLocalVariable(index), longCount);
        break;
    }

    default:
        break;
    }
}

// Updates comparement flag, used in conditional commands
// 0x4859D0
void CRunningScript::UpdateCompareFlag(bool state) {
    if (m_NotFlag)
        state = !state;

    if (m_AndOrState == ANDOR_NONE) {
        m_CondResult = state;
        return;
    }

    if (m_AndOrState >= ANDS_1 && m_AndOrState <= ANDS_8) {
        m_CondResult &= state;
        if (m_AndOrState == ANDS_1)
            m_AndOrState = ANDOR_NONE;
        else
            m_AndOrState--;

        return;
    }

    if (m_AndOrState >= ORS_1 && m_AndOrState <= ORS_8) {
        m_CondResult |= state;
        if (m_AndOrState == ORS_1)
            m_AndOrState = ANDOR_NONE;
        else
            m_AndOrState--;

        return;
    }
}

// Sets instruction pointer, used in GOTO-like commands
// 0x464DA0
void CRunningScript::UpdatePC(int32 newIP) {
    m_IP = newIP >= 0
        ? &CTheScripts::ScriptSpace[newIP]
        : m_BaseIP + std::abs(newIP);
}

// 0x469EB0, inlined
OpcodeResult CRunningScript::ProcessOneCommand() {
    ++CTheScripts::CommandsExecuted;

    const auto op = GetAtIPAs<scm::Instruction>();

    // Check if IP is valid pre-return
    notsa::ScopeGuard guardIP{[this]() {
        const auto next{ GetAtIPAs<scm::Instruction>(false) };
        VERIFY(next.Command <= COMMAND_HIGHEST_VANILLA_ID);
    }};

#ifdef NOTSA_SCRIPT_TRACING
    // snprintf is faster (in debug at least) - Gotta stick to it for now
    char msg[4096];
    sprintf_s(msg, "[%s][IP: 0x%X + 0x%X]: %s [0x%X]", m_szName, LOG_PTR(m_pBaseIP), LOG_PTR(m_IP - m_pBaseIP), notsa::script::GetScriptCommandName((eScriptCommands)op.Command).data(), (size_t)op.Command);
    SPDLOG_LOGGER_TRACE(logger, msg);
    //SPDLOG_LOGGER_TRACE(logger, "[{}][IP: {:#x} + {:#x}]: {} [{:#x}]", BaseFilename, LOG_PTR(m_pBaseIP), LOG_PTR(m_IP - m_pBaseIP), notsa::script::GetScriptCommandName((eScriptCommands)op.command), (size_t)op.command);
#endif
    
    m_NotFlag = op.NotFlag;

    // Herramienta de bisección: GTASA_CMD_BYPASS=<archivo> (líneas "N" o "A-B" en decimal) obliga a usar el manejador original
    // del EXE para esos comandos, sin tocar el resto. Sin la variable no cambia nada.
    static const std::vector<bool> s_Bypass = []() {
        std::vector<bool> v((size_t)COMMAND_HIGHEST_ID_TO_HOOK + 1, false);
        char* env = nullptr; size_t len = 0;
        if (_dupenv_s(&env, &len, "GTASA_CMD_BYPASS") == 0 && env) {
            if (std::ifstream in{ env }) {
                for (std::string line; std::getline(in, line);) {
                    int a = 0, b = 0;
                    if (std::sscanf(line.c_str(), "%d-%d", &a, &b) == 2) {
                    } else if (std::sscanf(line.c_str(), "%d", &a) == 1) { b = a; } else { continue; }
                    for (int i = std::max(a, 0); i <= b && i < (int)v.size(); ++i) v[(size_t)i] = true;
                }
            }
            free(env);
        }
        return v;
    }();
    const bool bypass = (size_t)op.Command < s_Bypass.size() && s_Bypass[(size_t)op.Command];
    if (const auto handler = bypass ? notsa::script::CommandHandlerFunction{} : CustomCommandHandlerOf((eScriptCommands)(op.Command))) {
        return std::invoke(handler, this);
    } else {
        return std::invoke(s_OriginalCommandHandlerTable[(size_t)op.Command / 100], this, (eScriptCommands)(op.Command));
    }
}

// 0x469F00
OpcodeResult CRunningScript::Process() {
    if (m_SceneSkipIP && CCutsceneMgr::IsCutsceneSkipButtonBeingPressed()) {
        CHud::m_BigMessage[1][0] = 0;
        UpdatePC(std::exchange(m_SceneSkipIP, 0));
        m_WakeTime = 0;
    }

    if (m_UsesMissionCleanup) {
        DoDeathArrestCheck();
    }

    if (m_ThisMustBeTheOnlyMissionRunning && CTheScripts::FailCurrentMission == 1) {
        if (m_StackDepth > 0) {
            ResetIP();
        }
    }

    CTheScripts::ReinitialiseSwitchStatementData();

    if (CTimer::GetTimeInMS() >= (uint32)m_WakeTime) {
        while (ProcessOneCommand() == OR_CONTINUE)
            ; // Process commands
    }

    return OR_CONTINUE;
}

void CRunningScript::HighlightImportantArea(CVector2D from, CVector2D to, float z) {
    CTheScripts::HighlightImportantArea(reinterpret_cast<int32>(this) + reinterpret_cast<int32>(m_IP), from.x, from.y, to.x, to.y, z);
}

void CRunningScript::HighlightImportantArea(CRect area, float z) {
    HighlightImportantArea(area.GetTopLeft(), area.GetBottomRight(), z);
}

void CRunningScript::HighlightImportantArea(CVector from, CVector to) {
    HighlightImportantArea(CVector2D{ from }, CVector2D{ to }, (from.z + to.z) / 2.f);
}

void CRunningScript::HighlightImportantAngledArea(uint32 id, CVector2D a, CVector2D b, CVector2D c, CVector2D d) {
    NOTSA_UNREACHABLE(); // Fuck this, we dont need it!
}

notsa::script::CommandHandlerFunction& CRunningScript::CustomCommandHandlerOf(eScriptCommands command) {
    return s_CustomCommandHandlerTable[(size_t)(command)];
}

void CRunningScript::ResetIP() {
    // Como el original (0x469F77..0x469F9A, solo con profundidad >= 1): la pila se vacia a 0 y la IP pasa a la primera
    // direccion de retorno (m_IPStack[0]). La version anterior usaba m_IPStack[profundidad] y llegaba hasta [1].
    assert(m_StackDepth > 0); // Original bug...
    m_StackDepth = 0;
    m_IP         = m_IPStack[0];
}

// Lote 237A: desplazamientos del EXE que usan ProcessCommands1200To1299 / 1900To1999 (comprobados contra el desensamblado de 0x48B590 y 0x46B460).
static_assert(offsetof(CVehicle, m_pHandlingData) == 0x384);
static_assert(offsetof(tHandlingData, m_nModelFlags) == 0xCC);
static_assert(offsetof(CVehicle, m_vecMoveSpeed) == 0x44);
static_assert(offsetof(CVehicle, vehicleFlags) == 0x428);
static_assert(offsetof(CVehicle, m_nVehicleType) == 0x590);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCruiseSpeed) == 0x3D0);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_ucHeliTargetDist) == 0x3D8);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_ucHeliSpeedMult) == 0x3D9);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_TargetEntity) == 0x41C);
static_assert(offsetof(CHeli, m_fMinAltitude) == 0x9B0);
static_assert(offsetof(CPedIntelligence, m_TaskMgr) == 0x4);
static_assert(offsetof(CTaskComplexUseSequence, m_nSequenceIndex) == 0xC);
static_assert(offsetof(CTaskComplexUseSequence, m_nCurrentTaskIndex) == 0x10);
static_assert(offsetof(CTaskComplexUseSequence, m_nEndTaskIndex) == 0x14);
static_assert(offsetof(CVehicleModelInfo, m_nPlateType) == 0x31);
static_assert(offsetof(CVehicle, m_pTowingVehicle) == 0x4C4);
static_assert(offsetof(CVehicle, m_pVehicleBeingTowed) == 0x4C8);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nStraightLineDistance) == 0x3DD);
static_assert(offsetof(CAutomobile, m_fHeliRotorSpeed) == 0x84C);
static_assert(offsetof(CPed, m_aWeapons) == 0x5A0);
static_assert(offsetof(CPed, m_fArmour) == 0x548);
static_assert(offsetof(CPed, m_pVehicle) == 0x58C);
static_assert(offsetof(CPed, m_acquaintance) == 0x4E0);
static_assert(offsetof(CPhysical, m_nNumEntitiesCollided) == 0xB9);
static_assert(offsetof(CPhysical, m_pEntityIgnoredCollision) == 0x128);
static_assert(offsetof(CPlayerInfo, m_PlayerData) == 0x4);
static_assert(offsetof(CPlayerPedData, m_pPedClothesDesc) == 0x4);
static_assert(offsetof(CPlayerPedData, m_nPlayerGroup) == 0x38);
static_assert(offsetof(CPlayerInfo, m_nBestCarTwoWheelsTimeMs) == 0x118);
static_assert(offsetof(CPlayerInfo, m_fBestBikeStoppieDistM) == 0x12C);
static_assert(offsetof(CPlayerInfo, m_bCanDoDriveBy) == 0x153);
static_assert(sizeof(CPlayerInfo) == 0x190);
static_assert(offsetof(CPedClothesDesc, m_anTextureKeys) == 0x28);
static_assert(offsetof(CZone, m_ZoneInfoIndex) == 0x1C);
static_assert(sizeof(CZoneInfo) == 0x11);
static_assert(offsetof(CPedGroup, m_groupIntelligence) == 0x30);
static_assert(sizeof(CPedGroup) == 0x2D4);
static_assert(offsetof(CPad, bDisablePlayerEnterCar) == 0x119);
static_assert(offsetof(CRunningScript, m_IsTextBlockOverride) == 0xC8);
static_assert(offsetof(CRunningScript, m_UsesMissionCleanup) == 0xC6);
static_assert(offsetof(tBeatInfo, IsBeatInfoPresent) == 0xA0);
static_assert(offsetof(tMessage, Text) == 0x0);
static_assert(offsetof(tBeatInfo, BeatNumber) == 0xA8);
static_assert(offsetof(CWeaponInfo, m_nModelId1) == 0xC);
static_assert(offsetof(CWeaponInfo, m_nSlot) == 0x14);
static_assert(offsetof(CAnimBlock, IsLoaded) == 0x10);
static_assert(sizeof(CTaskSimpleDuckToggle) == 0xC);
static_assert(sizeof(CTaskComplexUseSwatRope) == 0x1C);
static_assert(sizeof(CTaskSimpleStandStill) == 0x20);
static_assert(sizeof(CTaskComplexSeekEntityStandard) == 0x4C);
static_assert(sizeof(CTaskComplexClimb) == 0x14);
static_assert(sizeof(CTaskComplexUseSequence) == 0x1C);
static_assert(sizeof(CTaskComplexSeekEntityAiming) == 0x18);
static_assert(sizeof(CTaskComplexKillPedOnFoot) == 0x38);
static_assert(sizeof(CTaskSimpleJetPack) == 0x70);
static_assert(sizeof(CTaskSimpleSetCharDecisionMaker) == 0xC);
static_assert(sizeof(CTaskComplexGoPickUpEntity) == 0x34);
static_assert(sizeof(CTaskComplexSequence) == 0x40);
static_assert(sizeof(CTaskComplexGoToPointAndStandStill) == 0x28);
static_assert(sizeof(CTaskSimpleSlideToCoord) == 0x80);

namespace {
//! Campo por desplazamiento de un objeto del EXE (solo donde el miembro nombrado no esta en las cabeceras)
template<typename T>
T& RawField(void* object, size_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(object) + offset);
}
} // namespace

// 0x48B590 (lote 237A, sin validar; el listado decia CRunningScript, 5816 bytes: es CRunningScript::ProcessCommands1200To1299, el manejador original de los comandos de script 1200..1299; ocupa la ranura 12 de
// s_OriginalCommandHandlerTable (0x8A6198) y lo llama ProcessOneCommand con la ranura command / 100 cuando el comando no tiene manejador propio). Thiscall con un argumento de pila (`ret 4`). Prologo SEH (marco con
// manejador 0x8384BA): solo protege los tres `new` de tareas (estados 0, 1 y 2; el destructor de desenrollado es operator delete), sin objetos con destructor: en C++ no hace falta. Devuelve OR_INTERRUPT (-1) para
// los comandos sin caso (el EXE sale por 0x48CC3D) y OR_CONTINUE (0) en el resto, incluidos los siete comandos que el EXE deja vacios sin leer parametros (1212, 1214, 1215, 1223, 1269, 1287, 1291).
// Locales del EXE: dos palabras de area de nodo inicializadas a 0xFFFF (1209, la unica que las usa).
// Cada caso replica el orden de colaboradores del desensamblado (CollectParameters / GetPointerToScriptVariable / lecturas de etiqueta / reservas de tarea) y las comparaciones x87 con -100.0 (jp tras test ah,41h:
// la rama se toma con z <= -100.0, y con NaN no).
OpcodeResult CRunningScript::ProcessCommands1200To1299(int32 command) {
    auto& sp = ScriptParams;

    constexpr float MIN_GROUND_Z = -100.0f; // 0x859014

    switch (command) {
    // 1200..1207: comparaciones de entero con variable (global = tipo 2, local = tipo 1); en las variantes "constante > variable" el EXE lee primero el parametro y luego la variable
    case 1200: { // IS_INT_VAR_GREATER_THAN_CONSTANT
        auto* var = GetPointerToScriptVariable(VAR_GLOBAL);
        CollectParameters(1);
        UpdateCompareFlag(var->iParam > sp[0].iParam);
        return OR_CONTINUE;
    }
    case 1201: { // IS_INT_LVAR_GREATER_THAN_CONSTANT
        auto* var = GetPointerToScriptVariable(VAR_LOCAL);
        CollectParameters(1);
        UpdateCompareFlag(var->iParam > sp[0].iParam);
        return OR_CONTINUE;
    }
    case 1202: { // IS_CONSTANT_GREATER_THAN_INT_VAR
        CollectParameters(1);
        auto* var = GetPointerToScriptVariable(VAR_GLOBAL);
        UpdateCompareFlag(sp[0].iParam > var->iParam);
        return OR_CONTINUE;
    }
    case 1203: { // IS_CONSTANT_GREATER_THAN_INT_LVAR
        CollectParameters(1);
        auto* var = GetPointerToScriptVariable(VAR_LOCAL);
        UpdateCompareFlag(sp[0].iParam > var->iParam);
        return OR_CONTINUE;
    }
    case 1204: { // IS_INT_VAR_GREATER_OR_EQUAL_TO_CONSTANT
        auto* var = GetPointerToScriptVariable(VAR_GLOBAL);
        CollectParameters(1);
        UpdateCompareFlag(var->iParam >= sp[0].iParam);
        return OR_CONTINUE;
    }
    case 1205: { // IS_INT_LVAR_GREATER_OR_EQUAL_TO_CONSTANT
        auto* var = GetPointerToScriptVariable(VAR_LOCAL);
        CollectParameters(1);
        UpdateCompareFlag(var->iParam >= sp[0].iParam);
        return OR_CONTINUE;
    }
    case 1206: { // IS_CONSTANT_GREATER_OR_EQUAL_TO_INT_VAR
        CollectParameters(1);
        auto* var = GetPointerToScriptVariable(VAR_GLOBAL);
        UpdateCompareFlag(sp[0].iParam >= var->iParam);
        return OR_CONTINUE;
    }
    case 1207: { // IS_CONSTANT_GREATER_OR_EQUAL_TO_INT_LVAR
        CollectParameters(1);
        auto* var = GetPointerToScriptVariable(VAR_LOCAL);
        UpdateCompareFlag(sp[0].iParam >= var->iParam);
        return OR_CONTINUE;
    }

    case 1208: { // GET_CHAR_WEAPON_IN_SLOT: el EXE direcciona ped + 0x584 + ranura * 0x1C (= m_aWeapons[ranura - 1]); lee tipo (+0) y municion total (+0xC)
        CollectParameters(2);
        auto*        ped    = GetPedPool()->GetAtRef(sp[0].iParam);
        const auto&  weapon = *(ped->m_aWeapons.data() + (sp[1].iParam - 1));
        const auto   type   = weapon.m_Type;
        sp[0].iParam        = type;
        sp[1].iParam        = weapon.m_TotalAmmo;
        sp[2].iParam        = CPickups::ModelForWeapon(type);
        StoreParameters(3);
        return OR_CONTINUE;
    }

    case 1209: { // GET_CLOSEST_STRAIGHT_ROAD
        CollectParameters(5);
        CNodeAddress nodeA{ 0xFFFF, 0 }, nodeB{ 0xFFFF, 0 };
        float        heading{};
        ThePaths.FindNodePairClosestToCoors(CVector{ sp[0].fParam, sp[1].fParam, sp[2].fParam }, 0, &nodeA, &nodeB, &heading, sp[3].fParam, sp[4].fParam, true, true, 0);
        bool found = false;
        if (nodeA.m_wAreaId != 0xFFFF) {
            const CVector first = ThePaths.FindNodeCoorsForScript(nodeA, nullptr);
            sp[0].fParam        = first.x;
            sp[1].fParam        = first.y;
            sp[2].fParam        = first.z;
            bool foundSecond{};
            const CVector second = ThePaths.FindNodeCoorsForScript(nodeB, &foundSecond);
            if (foundSecond) {
                sp[5].fParam = second.z;
                sp[3].fParam = second.x;
                sp[4].fParam = second.y;
                found        = true;
            }
        }
        if (!found) {
            sp[0].iParam = sp[1].iParam = sp[2].iParam = sp[3].iParam = sp[4].iParam = sp[5].iParam = 0;
            heading                                                                                    = 0.0f;
        }
        sp[6].fParam = heading;
        StoreParameters(7);
        UpdateCompareFlag(found);
        return OR_CONTINUE;
    }

    case 1210: { // SET_CAR_FORWARD_SPEED: velocidad = adelante * (parametro * 0.016666668 [0x859044]); un helicoptero (flag de modelo 0x2000000) con +0x590 == 0 recibe la velocidad del rotor 0.22
        CollectParameters(2);
        auto* veh = GetVehiclePool()->GetAtRef(sp[0].iParam);
        const float scaledSpeed = sp[1].fParam * 0.016666668f;
        veh->m_vecMoveSpeed = veh->GetMatrix().GetForward() * scaledSpeed;
        if ((veh->m_pHandlingData->m_nModelFlags & VEHICLE_HANDLING_MODEL_IS_HELI) && veh->m_nVehicleType == VEHICLE_TYPE_AUTOMOBILE) {
            static_cast<CAutomobile*>(veh)->m_fHeliRotorSpeed = 0.22f; // 0x3E6147AE
        }
        return OR_CONTINUE;
    }

    case 1211: { // SET_AREA_VISIBLE
        CollectParameters(1);
        CGame::currArea = (eAreaCodes)sp[0].iParam;
        CStreaming::RemoveBuildingsNotInArea((eAreaCodes)sp[0].iParam);
        return OR_CONTINUE;
    }

    case 1213: { // MARK_CAR_AS_CONVOY_CAR
        CollectParameters(2);
        auto* veh                     = GetVehiclePool()->GetAtRef(sp[0].iParam);
        veh->vehicleFlags.bPartOfConvoy = sp[1].iParam != 0;
        return OR_CONTINUE;
    }

    case 1216: { // CREATE_SCRIPT_ROADBLOCK
        CollectParameters(7);
        CRoadBlocks::RegisterScriptRoadBlock(CVector{ sp[0].fParam, sp[1].fParam, sp[2].fParam }, CVector{ sp[3].fParam, sp[4].fParam, sp[5].fParam }, sp[6].iParam != 0);
        return OR_CONTINUE;
    }
    case 1217: // CLEAR_ALL_SCRIPT_ROADBLOCKS
        CRoadBlocks::ClearScriptRoadBlocks();
        return OR_CONTINUE;

    case 1220: { // GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS: Multiply3x3(matriz del ped, desplazamiento) + posicion (matriz + 0x30, o +4 si no hay matriz)
        CollectParameters(4);
        auto*   ped    = GetPedPool()->GetAtRef(sp[0].iParam);
        const CVector input{ sp[1].fParam, sp[2].fParam, sp[3].fParam };
        CVector       offset;
        Multiply3x3Exe(&offset, ped->m_matrix, &input);
        offset += ped->GetPosition();
        sp[0].fParam = offset.x;
        sp[1].fParam = offset.y;
        sp[2].fParam = offset.z;
        StoreParameters(3);
        return OR_CONTINUE;
    }

    case 1221: { // HAS_CHAR_BEEN_PHOTOGRAPHED: consume el bit (bHasBeenPhotographed) si estaba puesto
        CollectParameters(1);
        auto* ped    = GetPedPool()->GetAtRef(sp[0].iParam);
        bool  result = false;
        if (ped && ped->bHasBeenPhotographed) {
            ped->bHasBeenPhotographed = false;
            result                    = true;
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }

    case 1224: { // IS_CHAR_IN_FLYING_VEHICLE: apariencia 5 (helicoptero) o 3 (avion); GetVehicleAppearance se llama otra vez si la primera no es 5
        CollectParameters(1);
        auto* ped    = GetPedPool()->GetAtRef(sp[0].iParam);
        bool  result = false;
        if (ped->bInVehicle) {
            if (ped->m_pVehicle->GetVehicleAppearance() == 5 || ped->m_pVehicle->GetVehicleAppearance() == 3) {
                result = true;
            }
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }

    case 1230: { // ADD_SHORT_RANGE_SPRITE_BLIP_FOR_COORD
        CollectParameters(4);
        float z = sp[2].fParam;
        if (z <= MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(sp[0].fParam, sp[1].fParam);
        }
        (void)CRadar::GetActualBlipArrayIndex(CollectNextParameterWithoutIncreasingPC()); // el EXE descarta el resultado
        const auto blip = CRadar::SetShortRangeCoordBlip(BLIP_COORD, CVector{ sp[0].fParam, sp[1].fParam, z }, (eBlipColour)5, (eBlipDisplay)3, m_szName);
        CRadar::SetBlipSprite(blip, (eRadarSprite)sp[3].iParam);
        sp[0].iParam = blip;
        StoreParameters(1);
        return OR_CONTINUE;
    }

    case 1232: { // SET_HELI_ORIENTATION: angulo = (param + 90.0) * 0.017453292 normalizado a [0, 2*pi]
        CollectParameters(2);
        auto*       veh   = GetVehiclePool()->GetAtRef(sp[0].iParam);
        float       angle = (sp[1].fParam + 90.0f) * 0.017453292f; // 0x85991C, 0x8595EC
        constexpr float TWO_PI = 6.2831855f;                      // 0x858CBC
        if (angle < 0.0f) {
            do {
                angle += TWO_PI;
            } while (angle < 0.0f);
        }
        if (angle > TWO_PI) {
            do {
                angle -= TWO_PI;
            } while (angle > TWO_PI);
        }
        static_cast<CAutomobile*>(veh)->SetHeliOrientation(angle);
        return OR_CONTINUE;
    }
    case 1233: { // CLEAR_HELI_ORIENTATION
        CollectParameters(1);
        static_cast<CAutomobile*>(GetVehiclePool()->GetAtRef(sp[0].iParam))->ClearHeliOrientation();
        return OR_CONTINUE;
    }
    case 1234: { // PLANE_GOTO_COORDS
        CollectParameters(6);
        auto* veh = GetVehiclePool()->GetAtRef(sp[0].iParam);
        static_cast<CAutomobile*>(veh)->TellPlaneToGoToCoors(sp[1].fParam, sp[2].fParam, sp[3].fParam, sp[4].fParam, sp[5].fParam);
        return OR_CONTINUE;
    }

    case 1235: { // GET_NTH_CLOSEST_CAR_NODE
        CollectParameters(4);
        float z = sp[2].fParam;
        if (z <= MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(sp[0].fParam, sp[1].fParam);
        }
        const CNodeAddress addr = ThePaths.FindNthNodeClosestToCoors(CVector{ sp[0].fParam, sp[1].fParam, z }, 0, std::bit_cast<float>(0x497423FEu), false, true, sp[3].iParam - 1, false, false, nullptr);
        bool       found{};
        const auto pos = ThePaths.FindNodeCoorsForScript(addr, &found);
        if (found) {
            sp[0].fParam = pos.x;
            sp[1].fParam = pos.y;
            sp[2].fParam = pos.z;
        } else {
            sp[0].iParam = 0;
            sp[1].iParam = 0;
            sp[2].iParam = 0;
        }
        StoreParameters(3);
        UpdateCompareFlag(found);
        return OR_CONTINUE;
    }

    case 1237: { // DRAW_WEAPONSHOP_CORONA: el identificador de la corona es (this + m_IP) como en HighlightImportantArea
        CollectParameters(9);
        float z = sp[2].fParam;
        if (z <= MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(sp[0].fParam, sp[1].fParam);
        }
        const CVector pos{ sp[0].fParam, sp[1].fParam, z };
        CCoronas::RegisterCorona(
            uint32(reinterpret_cast<uintptr_t>(this) + reinterpret_cast<uintptr_t>(m_IP)),
            nullptr,
            sp[6].u8Param, sp[7].u8Param, sp[8].u8Param, 255,
            pos, sp[3].fParam, 150.0f,
            (eCoronaType)sp[4].u8Param, (eCoronaFlareType)sp[5].u8Param, (eCoronaReflType)1, (eCoronaLOSCheck)0, (eCoronaTrail)0,
            0.0f, false,
            0.2f, false,
            15.0f, false, false
        );
        return OR_CONTINUE;
    }

    case 1238: // SET_ENABLE_RC_DETONATE_ON_CONTACT
        CollectParameters(1);
        CVehicle::bDisableRemoteDetonationOnContact = sp[0].iParam == 0;
        return OR_CONTINUE;

    case 1239: { // FREEZE_CHAR_POSITION (bit 0x2000 de las banderas fisicas)
        CollectParameters(2);
        auto* ped                           = GetPedPool()->GetAtRef(sp[0].iParam);
        ped->physicalFlags.bDontApplySpeed = sp[1].iParam != 0;
        return OR_CONTINUE;
    }
    case 1240: { // SET_CHAR_DROWNS_IN_WATER (bit 0x100000 de las banderas del ped, segunda palabra)
        CollectParameters(2);
        auto* ped              = GetPedPool()->GetAtRef(sp[0].iParam);
        ped->bDrownsInWater    = sp[1].iParam != 0;
        return OR_CONTINUE;
    }
    case 1241: { // SET_OBJECT_RECORDS_COLLISIONS (bit 0x10000000 de las banderas fisicas)
        CollectParameters(2);
        auto* object                               = GetObjectPool()->GetAtRef(sp[0].iParam);
        object->physicalFlags.bCanBeCollidedWith = sp[1].iParam != 0;
        return OR_CONTINUE;
    }
    case 1242: { // HAS_OBJECT_COLLIDED_WITH_ANYTHING
        CollectParameters(1);
        auto* object = GetObjectPool()->GetAtRef(sp[0].iParam);
        UpdateCompareFlag(object->m_nNumEntitiesCollided != 0);
        return OR_CONTINUE;
    }
    case 1243: // REMOVE_RC_BUGGY
        CWorld::Players[CWorld::PlayerInFocus].BlowUpRCBuggy(false);
        return OR_CONTINUE;

    case 1245: { // GET_CHAR_ARMOUR (float -> entero truncado)
        CollectParameters(1);
        auto* ped    = GetPedPool()->GetAtRef(sp[0].iParam);
        sp[0].iParam = (int32)ped->m_fArmour;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case 1247: { // SET_HELI_STABILISER (bit 0x10 del byte +0x42B = bHeliMinimumTilt)
        CollectParameters(2);
        auto* veh                           = GetVehiclePool()->GetAtRef(sp[0].iParam);
        veh->vehicleFlags.bHeliMinimumTilt = sp[1].iParam != 0;
        return OR_CONTINUE;
    }
    case 1248: { // SET_CAR_STRAIGHT_LINE_DISTANCE
        CollectParameters(2);
        auto* veh                                  = GetVehiclePool()->GetAtRef(sp[0].iParam);
        veh->m_autoPilot.m_nStraightLineDistance = sp[1].u8Param;
        return OR_CONTINUE;
    }
    case 1249: { // POP_CAR_BOOT
        CollectParameters(1);
        static_cast<CAutomobile*>(GetVehiclePool()->GetAtRef(sp[0].iParam))->PopBoot();
        return OR_CONTINUE;
    }
    case 1250: // SHUT_PLAYER_UP: el EXE solo lee los dos parametros
        CollectParameters(2);
        return OR_CONTINUE;
    case 1251: // SET_PLAYER_MOOD
        CollectParameters(3);
        CAEPedSpeechAudioEntity::SetCJMood((eCJMood)sp[1].u16Param, sp[2].uParam, -1, -1, -1);
        return OR_CONTINUE;
    case 1252: { // REQUEST_COLLISION: el EXE pasa un CVector (x, y, 0) y el area actual
        CollectParameters(2);
        const CVector pos{ sp[0].fParam, sp[1].fParam, 0.0f };
        CColStore::RequestCollision(pos, (eAreaCodes)(int32)CGame::currArea);
        return OR_CONTINUE;
    }
    case 1253: // LOCATE_OBJECT_2D
    case 1254: // LOCATE_OBJECT_3D
        LocateObjectCommand(command);
        return OR_CONTINUE;
    case 1255: { // IS_OBJECT_IN_WATER (bit 8 de las banderas fisicas)
        CollectParameters(1);
        auto* object = GetObjectPool()->GetAtRef(sp[0].iParam);
        UpdateCompareFlag(object && object->physicalFlags.bSubmergedInWater);
        return OR_CONTINUE;
    }
    case 1257: // IS_OBJECT_IN_AREA_2D
    case 1258: // IS_OBJECT_IN_AREA_3D
        ObjectInAreaCheckCommand(command);
        return OR_CONTINUE;
    case 1259: { // TASK_TOGGLE_DUCK
        CollectParameters(2);
        CTask* task = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskSimpleDuckToggle))) {
            task = ::new (memory) CTaskSimpleDuckToggle((CTaskSimpleDuckToggle::eMode)sp[1].iParam);
        }
        GivePedScriptedTask(sp[0].iParam, task, command);
        return OR_CONTINUE;
    }

    case 1261: { // REQUEST_ANIMATION
        char name[16]{};
        ReadTextLabelFromScript(name, 16);
        const auto block = CAnimManager::GetAnimationBlockIndex(name);
        CStreaming::RequestModel((eModelID)(block + 0x63E7), STREAMING_MISSION_REQUIRED);
        CTheScripts::ScriptResourceManager.AddToResourceManager(block, RESOURCE_TYPE_ANIMATION, this);
        return OR_CONTINUE;
    }
    case 1262: { // HAS_ANIMATION_LOADED
        char name[16]{};
        ReadTextLabelFromScript(name, 16);
        UpdateCompareFlag(CAnimManager::GetAnimationBlock(name)->IsLoaded == 1);
        return OR_CONTINUE;
    }
    case 1263: { // REMOVE_ANIMATION
        char name[16]{};
        ReadTextLabelFromScript(name, 16);
        const auto block = CAnimManager::GetAnimationBlockIndex(name);
        if (CTheScripts::ScriptResourceManager.RemoveFromResourceManager(block, RESOURCE_TYPE_ANIMATION, this)) {
            CStreaming::SetMissionDoesntRequireAnim(block);
        }
        return OR_CONTINUE;
    }
    case 1264: { // IS_CHAR_WAITING_FOR_WORLD_COLLISION
        CollectParameters(1);
        auto* ped = GetPedPool()->GetAtRef(sp[0].iParam);
        UpdateCompareFlag(ped->m_bIsStaticWaitingForCollision);
        return OR_CONTINUE;
    }
    case 1265: { // IS_CAR_WAITING_FOR_WORLD_COLLISION
        CollectParameters(1);
        auto* veh = GetVehiclePool()->GetAtRef(sp[0].iParam);
        UpdateCompareFlag(veh->m_bIsStaticWaitingForCollision);
        return OR_CONTINUE;
    }

    case 1268: { // ATTACH_CHAR_TO_OBJECT: primero se resuelve el objeto y luego el ped
        CollectParameters(8);
        const float rotationLimit = sp[6].fParam * 0.017453292f; // 0x8595EC
        auto*       object        = GetObjectPool()->GetAtRef(sp[1].iParam);
        auto*       ped           = GetPedPool()->GetAtRef(sp[0].iParam);
        ped->AttachPedToEntity(object, CVector{ sp[2].fParam, sp[3].fParam, sp[4].fParam }, sp[5].u16Param, rotationLimit, (eWeaponType)sp[7].iParam);
        return OR_CONTINUE;
    }

    case 1271: { // DISPLAY_NTH_ONSCREEN_COUNTER_WITH_STRING: el indice de la variable se lee ANTES de los parametros; CText::Get se llama y su resultado se descarta
        const auto varId = GetIndexOfGlobalVariable();
        CollectParameters(2);
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        (void)TheText.Get(label); // el resultado se descarta (el EXE llama a CText::Get y no usa el texto traducido)
        CUserDisplay::OnscnTimer.AddCounter(varId, (eOnscreenCounter)sp[0].u16Param, label, (uint16)(sp[1].iParam - 1));
        return OR_CONTINUE;
    }

    case 1272: { // ADD_SET_PIECE
        CollectParameters(13);
        CSetPieces::AddOne(
            (eSetPieceType)sp[0].u8Param,
            CVector2D{ sp[1].fParam, sp[2].fParam },
            CVector2D{ sp[3].fParam, sp[4].fParam },
            CVector2D{ sp[5].fParam, sp[6].fParam },
            CVector2D{ sp[7].fParam, sp[8].fParam },
            CVector2D{ sp[9].fParam, sp[10].fParam },
            CVector2D{ sp[11].fParam, sp[12].fParam }
        );
        return OR_CONTINUE;
    }

    case 1273: // SET_EXTRA_COLOURS
        CollectParameters(2);
        CTimeCycle::StartExtraColour(sp[0].iParam - 1, sp[1].iParam != 0);
        return OR_CONTINUE;
    case 1274: // CLEAR_EXTRA_COLOURS
        CollectParameters(1);
        CTimeCycle::StopExtraColour(sp[0].iParam != 0);
        return OR_CONTINUE;

    case 1276: { // GET_WHEELIE_STATS: devuelve los seis mejores registros del jugador y los pone a cero
        CollectParameters(1);
        auto& info   = CWorld::Players[sp[0].iParam];
        sp[0].uParam = info.m_nBestCarTwoWheelsTimeMs;
        sp[1].fParam = info.m_fBestCarTwoWheelsDistM;
        sp[2].uParam = info.m_nBestBikeWheelieTimeMs;
        sp[3].fParam = info.m_fBestBikeWheelieDistM;
        sp[4].uParam = info.m_nBestBikeStoppieTimeMs;
        sp[5].fParam = info.m_fBestBikeStoppieDistM;
        StoreParameters(6);
        info.m_nBestCarTwoWheelsTimeMs = 0;
        info.m_fBestCarTwoWheelsDistM  = 0.0f;
        info.m_nBestBikeWheelieTimeMs  = 0;
        info.m_fBestBikeWheelieDistM   = 0.0f;
        info.m_nBestBikeStoppieTimeMs  = 0;
        info.m_fBestBikeStoppieDistM   = 0.0f;
        return OR_CONTINUE;
    }

    case 1278: { // BURST_CAR_TYRE: en motos (+0x590 == 9) el segundo parametro 2 pasa a 0 y 3 a 1 (se escribe de vuelta en el parametro); la llamada es la virtual de la ranura 0xB0
        CollectParameters(2);
        auto* veh = GetVehiclePool()->GetAtRef(sp[0].iParam);
        if (veh->m_nVehicleType == VEHICLE_TYPE_BIKE) {
            int32 tyre = sp[1].iParam;
            if (tyre == 2) {
                tyre            = 0;
                sp[1].iParam    = tyre;
            } else if (tyre == 3) {
                tyre            = 1;
                sp[1].iParam    = tyre;
            }
            veh->BurstTyre((uint8)tyre, true);
        } else {
            veh->BurstTyre(sp[1].u8Param, true);
        }
        return OR_CONTINUE;
    }

    case 1280: { // IS_PLAYER_WEARING: la clave se lee ANTES de la etiqueta (+0x28 = m_anTextureKeys)
        CollectParameters(2);
        const auto key = CWorld::Players[sp[0].iParam].m_PlayerData.m_pPedClothesDesc->m_anTextureKeys[sp[1].iParam];
        char       label[16]{};
        ReadTextLabelFromScript(label, 16);
        UpdateCompareFlag(key == CKeyGen::GetUppercaseKey(label));
        return OR_CONTINUE;
    }
    case 1281: { // SET_PLAYER_CAN_DO_DRIVE_BY
        CollectParameters(2);
        CWorld::Players[sp[0].iParam].m_bCanDoDriveBy = sp[1].iParam != 0;
        return OR_CONTINUE;
    }

    case 1283: { // CREATE_SWAT_ROPE
        CollectParameters(5);
        const CVector ropePos{ sp[2].fParam, sp[3].fParam, sp[4].fParam };
        const auto    pedType    = sp[0].iParam;
        const auto    modelIndex = sp[1].iParam;
        const auto    ropeId     = CRopes::CreateRopeForSwatPed(ropePos);
        auto*         ped        = CPopulation::AddPed((ePedType)pedType, (eModelID)modelIndex, ropePos, true);
        {
            CTask* task = nullptr;
            if (auto* memory = CTask::operator new(sizeof(CTaskComplexUseSwatRope))) {
                task = ::new (memory) CTaskComplexUseSwatRope((uint32)ropeId);
            }
            ped->GetIntelligence()->m_TaskMgr.SetTask(task, TASK_PRIMARY_PRIMARY, false);
            CPedScriptedTaskRecord::ms_scriptedTasks[CPedScriptedTaskRecord::GetVacantSlot()].Set(ped, command, task);
        }
        ped->SetCharCreatedBy(PED_MISSION);
        ped->bAllowMedicsToReviveMe = false; // and 0x7FFFFFFF sobre la primera palabra de banderas
        {
            CTask* task = nullptr;
            if (auto* memory = CTask::operator new(sizeof(CTaskSimpleStandStill))) {
                task = ::new (memory) CTaskSimpleStandStill(999999, true, false, 8.0f);
            }
            ped->GetIntelligence()->m_TaskMgr.SetTask(task, TASK_PRIMARY_DEFAULT, false);
        }
        // El EXE pasa aqui un CVector local que ESTE caso nunca escribe (pila sin inicializar, +0x6C): no hay valor fiel que reproducir.
        CVector uninitializedPos;
        CTheScripts::ClearSpaceForMissionEntity(uninitializedPos, ped);
        if (m_UsesMissionCleanup) {
            ped->m_bIsStaticWaitingForCollision = true;
        }
        CPopulation::ms_nTotalMissionPeds++;
        sp[0].iParam = GetPedPool()->GetRef(ped);
        StoreParameters(1);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(sp[0].iParam, MISSION_CLEANUP_ENTITY_TYPE_PED);
        }
        return OR_CONTINUE;
    }

    case 1286: // SET_CAR_MODEL_COMPONENTS (0x8A6458 / 0x8A6459)
        CollectParameters(3);
        CVehicleModelInfo::ms_compsToUse[0] = sp[1].i8Param;
        CVehicleModelInfo::ms_compsToUse[1] = sp[2].i8Param;
        return OR_CONTINUE;
    case 1288: { // CLOSE_ALL_CAR_DOORS
        CollectParameters(1);
        static_cast<CAutomobile*>(GetVehiclePool()->GetAtRef(sp[0].iParam))->CloseAllDoors();
        return OR_CONTINUE;
    }

    case 1289: { // GET_DISTANCE_BETWEEN_COORDS_2D: sqrt(dy * dy + dx * dx) con dx = x1 - x2, dy = y1 - y2
        CollectParameters(4);
        const float dx = sp[0].fParam - sp[2].fParam;
        const float dy = sp[1].fParam - sp[3].fParam;
        sp[0].fParam   = std::sqrt(dy * dy + dx * dx);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case 1290: { // GET_DISTANCE_BETWEEN_COORDS_3D: CVector::Magnitude de (x1 - x2, y1 - y2, z1 - z2)
        CollectParameters(6);
        const CVector delta{ sp[0].fParam - sp[3].fParam, sp[1].fParam - sp[4].fParam, sp[2].fParam - sp[5].fParam };
        sp[0].fParam = delta.Magnitude();
        StoreParameters(1);
        return OR_CONTINUE;
    }

    case 1294: { // SORT_OUT_OBJECT_COLLISION_WITH_CAR: se resuelve primero el vehiculo (parametro 1) y luego el objeto (parametro 0); escritura directa (+0x128 = m_pEntityIgnoredCollision)
        CollectParameters(2);
        auto* veh                          = GetVehiclePool()->GetAtRef(sp[1].iParam);
        auto* object                       = GetObjectPool()->GetAtRef(sp[0].iParam);
        object->m_pEntityIgnoredCollision = veh;
        return OR_CONTINUE;
    }
    case 1295: // GET_MAX_WANTED_LEVEL
        sp[0].iParam = (int32)CWanted::MaximumWantedLevel;
        StoreParameters(1);
        return OR_CONTINUE;

    case 1298: { // PRINT_HELP_FOREVER
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        const auto* text = TheText.Get(label);
        if (CTheScripts::bDisplayNonMiniGameHelpMessages || m_IsTextBlockOverride || !CTheScripts::bMiniGameInProgress) {
            CHud::SetHelpMessage(text, false, true, false);
            if (CTheScripts::bAddNextMessageToPreviousBriefs) {
                CMessages::AddToPreviousBriefArray(text, -1, -1, -1, -1, -1, -1, nullptr);
            }
        }
        CTheScripts::bAddNextMessageToPreviousBriefs = true;
        return OR_CONTINUE;
    }
    case 1299: { // PRINT_HELP_FOREVER_WITH_NUMBER
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        const auto* text = TheText.Get(label);
        CollectParameters(1);
        if (CTheScripts::bDisplayNonMiniGameHelpMessages || m_IsTextBlockOverride || !CTheScripts::bMiniGameInProgress) {
            CHud::SetHelpMessageWithNumber(text, sp[0].iParam, false, true);
        }
        return OR_CONTINUE;
    }

    case 1212: // SET_CUTSCENE_ANIM_TO_LOOP: el EXE no hace nada (ni lee parametros)
    case 1214: // RESET_HAVOC_CAUSED_BY_PLAYER
    case 1215: // GET_HAVOC_CAUSED_BY_PLAYER
    case 1223: // SWITCH_SECURITY_CAMERA
    case 1269: // SET_CHAR_AS_PLAYER_FRIEND
    case 1287: // SWITCH_LIFT_CAMERA
    case 1291: // POP_CAR_BOOT_USING_PHYSICS
        return OR_CONTINUE;

    default:
        return OR_INTERRUPT;
    }
}

// 0x46B460 (lote 237A, sin validar; el listado decia CScriptsForBrains, 6028 bytes: es CRunningScript::ProcessCommands1900To1999, el manejador original de los comandos de script 1900..1999; ranura 19 de
// s_OriginalCommandHandlerTable (0x8A6168 + 19 * 4 = 0x46B460)). Thiscall con un argumento de pila (`ret 4`). Prologo SEH como el de ProcessCommands1200To1299: solo protege los `new` de tareas (y los CMatrix locales
// de los comandos 1988 y 1990: sus destructores se ejecutan en C++ al salir del bloque). Salta con una tabla directa de 100 entradas (0x46CBEC): 30 comandos van a 0x46CBCB (devuelven 0 sin hacer nada y SIN leer
// parametros) y el 1977 y los fuera de rango a 0x46CBCF (OR_INTERRUPT, -1).
// Cada caso replica el orden de colaboradores del desensamblado. Los indices que vienen del script (zonas, armas, claves de ropa) se calculan con aritmetica de punteros como el EXE, sin la comprobacion de rango
// de std::array (en depuracion abortaria donde el EXE solo lee memoria).
OpcodeResult CRunningScript::ProcessCommands1900To1999(int32 command) {
    auto& sp = ScriptParams;

    // 0x46BDCB: cola compartida de GRAB_ENTITY_ON_WINCH y GRAB_ENTITY_ON_ROPE_FOR_OBJECT; los tres parametros se ponen antes a -1 y solo se rellena el que corresponde al tipo de la entidad
    const auto StoreGrabbedEntity = [&](CEntity* entity) {
        if (entity) {
            switch (entity->GetType()) {
            case ENTITY_TYPE_VEHICLE:
                sp[0].iParam = GetVehiclePool()->GetRef(static_cast<CVehicle*>(entity));
                break;
            case ENTITY_TYPE_PED:
                sp[1].iParam = GetPedPool()->GetRef(static_cast<CPed*>(entity));
                break;
            case ENTITY_TYPE_OBJECT:
                sp[2].iParam = GetObjectPool()->GetRef(static_cast<CObject*>(entity));
                break;
            default:
                break;
            }
        }
        StoreParameters(3);
    };

    switch (command) {
    case 1900: { // SET_ZONE_GANG_STRENGTH
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        CollectParameters(2);
        const auto zone = CTheZones::FindZoneByLabel(label, (eZoneType)2);
        if (zone >= 0) {
            const auto zoneInfo = (CTheZones::NavigationZoneArray.data() + zone)->m_ZoneInfoIndex;
            (CTheZones::ZoneInfoArray.data() + zoneInfo)->GangStrength[sp[0].iParam] = sp[1].u8Param;
        }
        return OR_CONTINUE;
    }
    case 1901: { // GET_ZONE_GANG_STRENGTH: el EXE NO comprueba que la zona exista (indice -1 lee memoria anterior a la tabla)
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        CollectParameters(1);
        const auto zone     = CTheZones::FindZoneByLabel(label, (eZoneType)2);
        const auto zoneInfo = (CTheZones::NavigationZoneArray.data() + zone)->m_ZoneInfoIndex;
        sp[0].iParam        = (CTheZones::ZoneInfoArray.data() + zoneInfo)->GangStrength[sp[0].iParam];
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case 1903: // IS_MESSAGE_BEING_DISPLAYED: primer mensaje breve con texto
        UpdateCompareFlag(CMessages::BriefMessages[0].Text != nullptr);
        return OR_CONTINUE;
    case 1904: { // SET_CHAR_IS_TARGET_PRIORITY (bit 0x20000000 de la segunda palabra de banderas del ped)
        CollectParameters(2);
        auto* ped                         = GetPedPool()->GetAtRef(sp[0].iParam);
        ped->bThisPedIsATargetPriority    = sp[1].iParam != 0;
        return OR_CONTINUE;
    }
    case 1905: { // CUSTOM_PLATE_DESIGN_FOR_NEXT_CAR: solo si el modelo existe, es de vehiculo (tipo 6) y tiene objeto RenderWare (+0x24)
        CollectParameters(2);
        auto* modelInfo = *(CModelInfo::ms_modelInfoPtrs + sp[0].iParam);
        if (modelInfo && (int32)modelInfo->GetModelType() == 6 && RawField<void*>(modelInfo, 0x24)) { // +0x24 = m_pRwObject
            static_cast<CVehicleModelInfo*>(modelInfo)->m_nPlateType = sp[1].u8Param;
        }
        return OR_CONTINUE;
    }
    case 1906: { // TASK_GOTO_CAR
        CollectParameters(4);
        const auto pedHandle    = sp[0].iParam;
        const auto targetRadius = sp[3].fParam;
        auto*      veh          = GetVehiclePool()->GetAtRef(sp[1].iParam);
        int32      seekInterval = sp[2].iParam;
        if (seekInterval < 0) {
            seekInterval = 50000;
        }
        CTask* task = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskComplexSeekEntityStandard))) {
            task = ::new (memory) CTaskComplexSeekEntityStandard(veh, seekInterval, 1000, targetRadius, 2.0f, 2.0f, true, true); // 0x859E30 / 0x859E34 = 2.0
        }
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    case 1910: { // REQUEST_IPL
        char name[24]{};
        ReadTextLabelFromScript(name, 18);
        CIplStore::RequestIplAndIgnore(CIplStore::FindIplSlot(name));
        return OR_CONTINUE;
    }
    case 1911: { // REMOVE_IPL
        char name[24]{};
        ReadTextLabelFromScript(name, 18);
        CIplStore::RemoveIplAndIgnore(CIplStore::FindIplSlot(name));
        return OR_CONTINUE;
    }
    case 1912: { // REMOVE_IPL_DISCREETLY
        char name[24]{};
        ReadTextLabelFromScript(name, 18);
        CIplStore::RemoveIplWhenFarAway(CIplStore::FindIplSlot(name));
        return OR_CONTINUE;
    }
    case 1914: { // SET_CHAR_RELATIONSHIP
        CollectParameters(3);
        auto* ped = GetPedPool()->GetAtRef(sp[0].iParam);
        ped->m_acquaintance.SetAsAcquaintance((AcquaintanceId)sp[1].iParam, CPedType::GetPedFlag((ePedType)sp[2].iParam));
        return OR_CONTINUE;
    }
    case 1915: { // CLEAR_CHAR_RELATIONSHIP
        CollectParameters(3);
        auto* ped = GetPedPool()->GetAtRef(sp[0].iParam);
        ped->m_acquaintance.ClearAsAcquaintance((AcquaintanceId)sp[1].iParam, CPedType::GetPedFlag((ePedType)sp[2].iParam));
        return OR_CONTINUE;
    }
    case 1916: { // CLEAR_ALL_CHAR_RELATIONSHIPS
        CollectParameters(2);
        auto* ped = GetPedPool()->GetAtRef(sp[0].iParam);
        ped->m_acquaintance.ClearAcquaintances((AcquaintanceId)sp[1].iParam);
        return OR_CONTINUE;
    }
    case 1917: { // GET_CAR_PITCH: grados normalizados a [0, 360]
        CollectParameters(1);
        auto*       veh   = GetVehiclePool()->GetAtRef(sp[0].iParam);
        float       pitch = static_cast<CAutomobile*>(veh)->GetCarPitch() * std::bit_cast<float>(0x42652EE0u); // 0x859878 = 57.2957763671875 (el literal 57.29578f es 0x42652EE1: un ulp mas)
        constexpr float FULL_TURN = 360.0f;                                            // 0x859E2C
        if (pitch < 0.0f) {
            pitch += FULL_TURN;
        }
        if (pitch > FULL_TURN) {
            pitch -= FULL_TURN;
        }
        sp[0].fParam = pitch;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case 1918: // GET_AREA_VISIBLE
        sp[0].iParam = CGame::currArea;
        StoreParameters(1);
        return OR_CONTINUE;
    case 1920: { // HELI_KEEP_ENTITY_IN_VIEW (autopiloto: mision +0x3BA, velocidad de crucero +0x3D0, distancia al objetivo +0x3D8, objetivo +0x41C; CHeli::m_fMinAltitude +0x9B0): el objetivo es el vehiculo (parametro 2, si >= 0) o, si no, el ped (parametro 1, si >= 0); RegisterReference se llama aunque sea nulo, como el EXE
        CollectParameters(5);
        auto*    heli   = GetVehiclePool()->GetAtRef(sp[0].iParam);
        CEntity* target = nullptr;
        if (sp[1].iParam >= 0) {
            target = GetPedPool()->GetAtRef(sp[1].iParam);
        }
        if (sp[2].iParam >= 0) {
            target = GetVehiclePool()->GetAtRef(sp[2].iParam);
        }
        const uint32 distance = sp[3].uParam; // float del parametro 3
        const uint32 altitude = sp[4].uParam; // float del parametro 4 (dword sin convertir)
        auto& mission = heli->m_autoPilot.m_nCarMission;
        if ((uint8)mission != 0x39 && (uint8)mission != 0x3A) {
            mission = (eCarMission)0x33;
        }
        auto& targetRef = RawField<CEntity*>(&heli->m_autoPilot.m_TargetEntity, 0); // CEntity** para las referencias (el campo es un CVehicle*)
        if (targetRef) {
            targetRef->CleanUpOldReference(&targetRef);
        }
        targetRef = target;
        target->RegisterReference(&targetRef);
        static_cast<CHeli*>(heli)->m_fMinAltitude = std::bit_cast<float>(altitude);
        heli->m_autoPilot.m_nCruiseSpeed          = 100;
        heli->m_autoPilot.m_ucHeliTargetDist      = (uint8)(int32)std::bit_cast<float>(distance);
        return OR_CONTINUE;
    }
    case 1921: // GET_WEAPONTYPE_MODEL
        CollectParameters(1);
        sp[0].iParam = CWeaponInfo::GetWeaponInfo((eWeaponType)sp[0].iParam, eWeaponSkill::STD)->m_nModelId1;
        StoreParameters(1);
        return OR_CONTINUE;
    case 1922: // GET_WEAPONTYPE_SLOT
        CollectParameters(1);
        sp[0].iParam = CWeaponInfo::GetWeaponInfo((eWeaponType)sp[0].iParam, eWeaponSkill::STD)->m_nSlot;
        StoreParameters(1);
        return OR_CONTINUE;
    case 1923: // GET_SHOPPING_EXTRA_INFO
        CollectParameters(2);
        sp[0].iParam = CShopping::GetExtraInfo(sp[0].uParam, sp[1].iParam);
        StoreParameters(1);
        return OR_CONTINUE;
    case 1924: // GIVE_PLAYER_CLOTHES
        CollectParameters(4);
        CWorld::Players[sp[0].iParam].m_PlayerData.m_pPedClothesDesc->SetTextureAndModel(sp[1].uParam, sp[2].uParam, (eClothesTexturePart)sp[3].iParam);
        return OR_CONTINUE;
    case 1926: { // GET_NUMBER_OF_FIRES_IN_AREA: cada eje se ordena (intercambio solo si el primero es estrictamente mayor)
        CollectParameters(6);
        float x1 = sp[0].fParam, y1 = sp[1].fParam, z1 = sp[2].fParam;
        float x2 = sp[3].fParam, y2 = sp[4].fParam, z2 = sp[5].fParam;
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        if (y1 > y2) {
            std::swap(y1, y2);
        }
        if (z1 > z2) {
            std::swap(z1, z2);
        }
        sp[0].iParam = gFireManager.GetNumFiresInArea(x1, y1, z1, x2, y2, z2);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case 1928: { // ATTACH_WINCH_TO_HELI
        CollectParameters(2);
        GetVehiclePool()->GetAtRef(sp[0].iParam)->InitWinch(sp[1].iParam);
        return OR_CONTINUE;
    }
    case 1929: { // RELEASE_ENTITY_FROM_WINCH
        CollectParameters(1);
        GetVehiclePool()->GetAtRef(sp[0].iParam)->ReleasePickedUpEntityWithWinch();
        return OR_CONTINUE;
    }
    case 1930: { // GET_TRAIN_CARRIAGE
        CollectParameters(2);
        auto* train    = static_cast<CTrain*>(GetVehiclePool()->GetAtRef(sp[0].iParam));
        auto* carriage = CTrain::FindCarriage(train, sp[1].u8Param);
        sp[0].iParam   = carriage ? GetVehiclePool()->GetRef(carriage) : -1;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case 1931: { // GRAB_ENTITY_ON_WINCH
        CollectParameters(1);
        auto* veh = GetVehiclePool()->GetAtRef(sp[0].iParam);
        sp[0].iParam = sp[1].iParam = sp[2].iParam = -1;
        StoreGrabbedEntity(veh->QueryPickedUpEntityWithWinch());
        return OR_CONTINUE;
    }
    case 1932: { // GET_NAME_OF_ITEM: copia 8 caracteres de la etiqueta a la variable global
        CollectParameters(1);
        const auto* nameTag = CShopping::GetNameTag(sp[0].uParam);
        std::strncpy(reinterpret_cast<char*>(GetPointerToScriptVariable(VAR_GLOBAL)), nameTag, 8);
        return OR_CONTINUE;
    }
    case 1935: { // TASK_CLIMB: el byte +0x10 de la tarea se escribe tambien con la reserva fallida (bug del EXE)
        CollectParameters(2);
        const bool flag = sp[1].iParam != 0;
        CTask*     task = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskComplexClimb))) {
            task = ::new (memory) CTaskComplexClimb();
        }
        RawField<uint8>(task, 0x10) = flag;
        GivePedScriptedTask(sp[0].iParam, task, command);
        return OR_CONTINUE;
    }
    case 1936: // BUY_ITEM
        CollectParameters(1);
        CShopping::Buy(sp[0].uParam, 0);
        return OR_CONTINUE;
    case 1938: { // CLEAR_CHAR_TASKS_IMMEDIATELY
        CollectParameters(1);
        GetPedPool()->GetAtRef(sp[0].iParam)->GetIntelligence()->FlushImmediately(true);
        return OR_CONTINUE;
    }
    case 1939: // STORE_CLOTHES_STATE
        CShopping::StoreClothesState();
        return OR_CONTINUE;
    case 1940: // RESTORE_CLOTHES_STATE
        CShopping::RestoreClothesState();
        return OR_CONTINUE;
    case 1942: { // GET_ROPE_HEIGHT_FOR_OBJECT
        CollectParameters(1);
        sp[0].fParam = GetObjectPool()->GetAtRef(sp[0].iParam)->GetRopeHeight();
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case 1943: { // SET_ROPE_HEIGHT_FOR_OBJECT
        CollectParameters(2);
        GetObjectPool()->GetAtRef(sp[0].iParam)->SetRopeHeight(sp[1].fParam);
        return OR_CONTINUE;
    }
    case 1944: { // GRAB_ENTITY_ON_ROPE_FOR_OBJECT
        CollectParameters(1);
        auto* object = GetObjectPool()->GetAtRef(sp[0].iParam);
        sp[0].iParam = sp[1].iParam = sp[2].iParam = -1;
        StoreGrabbedEntity(object->GetObjectCarriedWithRope());
        return OR_CONTINUE;
    }
    case 1945: { // RELEASE_ENTITY_FROM_ROPE_FOR_OBJECT
        CollectParameters(1);
        GetObjectPool()->GetAtRef(sp[0].iParam)->ReleaseObjectCarriedWithRope();
        return OR_CONTINUE;
    }
    case 1949: // PLAYER_ENTERED_DOCK_CRANE
        CRopes::PlayerControlsCrane      = (eControlledCrane)1;
        CWaterLevel::m_bWaterFogScript   = false;
        return OR_CONTINUE;
    case 1950: // PLAYER_ENTERED_BUILDINGSITE_CRANE
        CRopes::PlayerControlsCrane      = (eControlledCrane)2;
        CWaterLevel::m_bWaterFogScript   = false;
        return OR_CONTINUE;
    case 1951: // PLAYER_LEFT_CRANE
        CRopes::PlayerControlsCrane      = (eControlledCrane)0;
        CWaterLevel::m_bWaterFogScript   = true;
        return OR_CONTINUE;
    case 1952: { // PERFORM_SEQUENCE_TASK_FROM_PROGRESS: los campos +0x14 y +0x10 se escriben tambien con la reserva fallida
        CollectParameters(4);
        const auto sequence = CTheScripts::GetActualScriptThingIndex(sp[1].iParam, SCRIPT_THING_SEQUENCE_TASK);
        if (sequence < 0 || sequence >= 64) {
            return OR_CONTINUE;
        }
        CTask* task = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskComplexUseSequence))) {
            task = ::new (memory) CTaskComplexUseSequence(sequence);
        }
        // +0x14 = m_nEndTaskIndex, +0x10 = m_nCurrentTaskIndex (se escriben aunque la reserva haya fallado, como el EXE)
        static_cast<CTaskComplexUseSequence*>(task)->m_nEndTaskIndex     = sp[3].iParam;
        static_cast<CTaskComplexUseSequence*>(task)->m_nCurrentTaskIndex = sp[2].iParam;
        GivePedScriptedTask(sp[0].iParam, task, command);
        return OR_CONTINUE;
    }
    case 1953: // SET_NEXT_DESIRED_MOVE_STATE
        CollectParameters(1);
        DatosExe::Data::v8D237C = sp[0].iParam;
        return OR_CONTINUE;
    case 1955: { // TASK_GOTO_CHAR_AIMING
        CollectParameters(4);
        const auto pedHandle = sp[0].iParam;
        auto*      target    = GetPedPool()->GetAtRef(sp[1].iParam);
        CTask*     task      = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskComplexSeekEntityAiming))) {
            task = ::new (memory) CTaskComplexSeekEntityAiming(target, sp[2].fParam, sp[3].fParam);
        }
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    case 1956: { // GET_SEQUENCE_PROGRESS_RECURSIVE: progreso de la tarea primaria (+0x10) y, si su subtarea es una secuencia (tipo 0x113), el de esa (+0x10)
        CollectParameters(1);
        auto* ped      = GetPedPool()->GetAtRef(sp[0].iParam);
        int32 progress = -1;
        int32 subProgress = -1;
        if (CPedScriptedTaskRecord::GetStatus(ped, 0x618) != (eScriptedTaskStatus)0) {
            auto* task = ped->GetIntelligence()->m_TaskMgr.GetTaskPrimary(TASK_PRIMARY_PRIMARY);
            progress   = RawField<int32>(task, 0x10);
            auto* sub  = task->GetSubTask();
            if (sub && (int32)sub->GetTaskType() == 0x113) {
                subProgress = RawField<int32>(sub, 0x10);
            }
        }
        sp[0].iParam = progress;
        sp[1].iParam = subProgress;
        StoreParameters(2);
        return OR_CONTINUE;
    }
    case 1957: { // TASK_KILL_CHAR_ON_FOOT_TIMED
        CollectParameters(3);
        const auto pedHandle = sp[0].iParam;
        const auto time      = sp[2].iParam;
        auto*      target    = GetPedPool()->GetAtRef(sp[1].iParam);
        CTask*     task      = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskComplexKillPedOnFoot))) {
            task = ::new (memory) CTaskComplexKillPedOnFoot(target, time, 0, 0, 0, 1);
        }
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    case 1958: { // GET_NEAREST_TAG_POSITION: sin marca, (-4000, -4000, -4000)
        CollectParameters(3);
        const CVector from{ sp[0].fParam, sp[1].fParam, sp[2].fParam };
        if (auto* tag = CTagManager::GetNearestTag(from)) {
            const CVector pos = tag->GetPosition();
            sp[0].fParam    = pos.x;
            sp[1].fParam    = pos.y;
            sp[2].fParam    = pos.z;
        } else {
            sp[0].uParam = sp[1].uParam = sp[2].uParam = 0xC57A0000u; // -4000.0f
        }
        StoreParameters(3);
        return OR_CONTINUE;
    }
    case 1959: { // TASK_JETPACK
        CollectParameters(1);
        const auto pedHandle = sp[0].iParam;
        CTask*     task      = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskSimpleJetPack))) {
            task = ::new (memory) CTaskSimpleJetPack(nullptr, 10.0f, 0, nullptr);
        }
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    case 1960: // SET_AREA51_SAM_SITE
        CollectParameters(1);
        CObject::bArea51SamSiteDisabled = sp[0].iParam == 0;
        return OR_CONTINUE;
    case 1961: { // IS_CHAR_IN_ANY_SEARCHLIGHT: el parametro 0 se pone a -1 antes de la consulta y se guarda siempre
        CollectParameters(1);
        auto* ped    = GetPedPool()->GetAtRef(sp[0].iParam);
        bool  result = false;
        int32 index{};
        sp[0].iParam = -1;
        if (CTheScripts::IsEntityWithinAnySearchLight(ped, &index)) {
            sp[0].iParam = index;
            result       = true;
        }
        StoreParameters(1);
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    case 1963: { // IS_TRAILER_ATTACHED_TO_CAB: sin remolque el EXE desreferencia la cabina sin comprobarla
        CollectParameters(2);
        auto* trailer = sp[0].iParam == -1 ? nullptr : GetVehiclePool()->GetAtRef(sp[0].iParam);
        auto* cab     = sp[1].iParam == -1 ? nullptr : GetVehiclePool()->GetAtRef(sp[1].iParam);
        bool  result  = false;
        if (trailer) {
            if (cab) {
                result = trailer->m_pTowingVehicle == cab;
            } else {
                result = trailer->m_pTowingVehicle != nullptr;
            }
        } else {
            result = cab->m_pVehicleBeingTowed != nullptr;
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    case 1964: { // DETACH_TRAILER_FROM_CAB (virtual de la ranura 0xF8 = BreakTowLink)
        CollectParameters(2);
        auto* trailer = sp[0].iParam == -1 ? nullptr : GetVehiclePool()->GetAtRef(sp[0].iParam);
        auto* cab     = sp[1].iParam == -1 ? nullptr : GetVehiclePool()->GetAtRef(sp[1].iParam);
        if (trailer) {
            trailer->BreakTowLink();
        } else if (auto* towed = cab->m_pVehicleBeingTowed) {
            towed->BreakTowLink();
        }
        return OR_CONTINUE;
    }
    case 1967: { // GET_PLAYER_GROUP
        CollectParameters(1);
        const auto group = CWorld::Players[sp[0].iParam].m_pPed->GetPlayerData()->m_nPlayerGroup;
        sp[0].iParam     = CTheScripts::GetUniqueScriptThingIndex(group, SCRIPT_THING_PED_GROUP);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case 1968: { // GET_LOADED_SHOP: copia 8 caracteres de la tienda cargada, los pasa a mayusculas y los copia a la variable global
        char name[16]{};
        std::strncpy(name, StaticRef<char[24]>(0xA9A7D8), 8); // CShopping::ms_shopLoaded (privado)
        MakeUpperCase(name);
        std::strncpy(reinterpret_cast<char*>(GetPointerToScriptVariable(VAR_GLOBAL)), name, 8);
        return OR_CONTINUE;
    }
    case 1969: { // GET_BEAT_PROXIMITY: ventana de pulsos de la pista; con 0 se elige entre las ranuras 9 y 10 la de mayor valor absoluto
        CollectParameters(1);
        const auto abs32 = [](int32 v) { const int32 mask = v >> 31; return (v ^ mask) - mask; };
        auto*      info  = AudioEngine.GetBeatInfo();
        auto*      beat  = reinterpret_cast<const int32*>(info->BeatWindow); // pares (a, b) de 8 bytes
        if (sp[0].iParam != 0) {
            const int32 slot = sp[0].iParam < 0 ? sp[0].iParam + 10 : sp[0].iParam + 9;
            if (info->IsBeatInfoPresent) {
                sp[0].iParam = beat[slot * 2];
                sp[1].iParam = beat[slot * 2 + 1];
                sp[2].iParam = info->BeatNumber;
                StoreParameters(3);
                return OR_CONTINUE;
            }
        } else if (info->IsBeatInfoPresent) {
            const int32 nextA = beat[10 * 2];
            const int32 prevA = beat[9 * 2];
            if (nextA && info->BeatNumber && abs32(nextA) > abs32(prevA)) {
                sp[0].iParam = prevA;
                sp[1].iParam = beat[9 * 2 + 1];
                sp[2].iParam = info->BeatNumber - 1;
            } else {
                sp[0].iParam = nextA;
                sp[1].iParam = beat[10 * 2 + 1];
                sp[2].iParam = info->BeatNumber;
            }
            StoreParameters(3);
            return OR_CONTINUE;
        }
        sp[0].iParam = sp[1].iParam = sp[2].iParam = -1;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    case 1971: { // SET_GROUP_DEFAULT_TASK_ALLOCATOR (el grupo empieza en 0xC09950 = ms_groups + 0x30, m_groupIntelligence)
        CollectParameters(2);
        const auto group = CTheScripts::GetActualScriptThingIndex(sp[0].iParam, SCRIPT_THING_PED_GROUP);
        if (group >= 0 && group < 8) {
            CPedGroups::ms_groups[group].m_groupIntelligence.SetDefaultTaskAllocatorType((ePedGroupDefaultTaskAllocatorType)sp[1].iParam);
        }
        return OR_CONTINUE;
    }
    case 1972: { // SET_PLAYER_GROUP_RECRUITMENT (bit 8 de la palabra +0x34 de los datos del jugador)
        CollectParameters(2);
        auto& flags = RawField<uint32>(CWorld::Players[sp[0].iParam].m_pPed->GetPlayerData(), 0x34);
        if (sp[1].iParam != 0) {
            flags &= 0xFFFFFEFFu;
        } else {
            flags |= 0x100u;
        }
        return OR_CONTINUE;
    }
    case 1979: { // ACTIVATE_HELI_SPEED_CHEAT
        CollectParameters(2);
        GetVehiclePool()->GetAtRef(sp[0].iParam)->m_autoPilot.m_ucHeliSpeedMult = sp[1].u8Param;
        return OR_CONTINUE;
    }
    case 1980: { // TASK_SET_CHAR_DECISION_MAKER
        CollectParameters(2);
        const auto pedHandle = sp[0].iParam;
        int32      decisionMaker = -1;
        if (sp[1].iParam != -1) {
            decisionMaker = CTheScripts::GetActualScriptThingIndex(sp[1].iParam, SCRIPT_THING_DECISION_MAKER);
        }
        if (pedHandle != -1) {
            GetPedPool()->GetAtRef(pedHandle)->GetIntelligence()->SetPedDecisionMakerType(decisionMaker);
            return OR_CONTINUE;
        }
        CTask* task = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskSimpleSetCharDecisionMaker))) {
            task = ::new (memory) CTaskSimpleSetCharDecisionMaker((uint32)decisionMaker);
        }
        GivePedScriptedTask(-1, task, command);
        return OR_CONTINUE;
    }
    case 1981: { // DELETE_MISSION_TRAIN
        CollectParameters(1);
        if (auto* train = static_cast<CTrain*>(GetVehiclePool()->GetAtRef(sp[0].iParam))) {
            CTrain::RemoveOneMissionTrain(train);
        }
        return OR_CONTINUE;
    }
    case 1982: { // MARK_MISSION_TRAIN_AS_NO_LONGER_NEEDED
        CollectParameters(1);
        if (auto* train = static_cast<CTrain*>(GetVehiclePool()->GetAtRef(sp[0].iParam))) {
            CTrain::ReleaseOneMissionTrain(train);
        }
        return OR_CONTINUE;
    }
    case 1983: // SET_BLIP_ALWAYS_DISPLAY_ON_ZOOMED_RADAR (el segundo parametro se pasa como byte)
        CollectParameters(2);
        CRadar::SetBlipAlwaysDisplayInZoom(sp[0].iParam, std::bit_cast<bool>(sp[1].u8Param)); // el EXE pasa el byte tal cual y el destino solo usa su bit 0 (`shl al,4; xor/and 0x10`): con `!= 0` un valor par distinto de cero activaria el bit
        return OR_CONTINUE;
    case 1984: // REQUEST_CAR_RECORDING
        CollectParameters(1);
        CVehicleRecording::RequestRecordingFile(sp[0].iParam);
        return OR_CONTINUE;
    case 1985: // HAS_CAR_RECORDING_BEEN_LOADED
        CollectParameters(1);
        UpdateCompareFlag(CVehicleRecording::HasRecordingFileBeenLoaded(sp[0].iParam));
        return OR_CONTINUE;
    case 1987: { // GET_OBJECT_QUATERNION: x, y, z, w en los parametros 0..3
        CollectParameters(1);
        auto*       object = GetObjectPool()->GetAtRef(sp[0].iParam);
        CQuaternion quat;
        quat.Set(*object->GetModellingMatrix());
        sp[0].fParam = quat.x;
        sp[1].fParam = quat.y;
        sp[2].fParam = quat.z;
        sp[3].fParam = quat.w;
        StoreParameters(4);
        return OR_CONTINUE;
    }
    case 1989: { // GET_VEHICLE_QUATERNION
        CollectParameters(1);
        auto*       veh = GetVehiclePool()->GetAtRef(sp[0].iParam);
        CQuaternion quat;
        quat.Set(*veh->GetModellingMatrix());
        sp[0].fParam = quat.x;
        sp[1].fParam = quat.y;
        sp[2].fParam = quat.z;
        sp[3].fParam = quat.w;
        StoreParameters(4);
        return OR_CONTINUE;
    }
    case 1988: // SET_OBJECT_QUATERNION
    case 1990: { // SET_VEHICLE_QUATERNION: matriz de rotacion RenderWare (sin inicializar salvo lo que escribe Get) -> CMatrix enlazado -> SetMatrix; luego se restaura la posicion
        CollectParameters(5);
        CPhysical* entity = command == 1988
            ? static_cast<CPhysical*>(GetObjectPool()->GetAtRef(sp[0].iParam))
            : static_cast<CPhysical*>(GetVehiclePool()->GetAtRef(sp[0].iParam));
        CQuaternion quat{ sp[1].fParam, sp[2].fParam, sp[3].fParam, sp[4].fParam };
        RwMatrix    rotation;
        quat.Get(&rotation);
        const CVector position = entity->GetPosition();
        {
            CMatrix matrix(&rotation, false);
            entity->SetMatrix(matrix);
        }
        entity->SetPosn(position);
        return OR_CONTINUE;
    }
    case 1991: { // SET_MISSION_TRAIN_COORDINATES
        CollectParameters(4);
        CTrain::SetNewTrainPosition(static_cast<CTrain*>(GetVehiclePool()->GetAtRef(sp[0].iParam)), CVector{ sp[1].fParam, sp[2].fParam, sp[3].fParam });
        return OR_CONTINUE;
    }
    case 1993: { // TASK_COMPLEX_PICKUP_OBJECT (animGroup 0x51)
        CollectParameters(2);
        auto*  object = GetObjectPool()->GetAtRef(sp[1].iParam);
        CTask* task   = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskComplexGoPickUpEntity))) {
            task = ::new (memory) CTaskComplexGoPickUpEntity(object, 0x51);
        }
        GivePedScriptedTask(sp[0].iParam, task, command);
        return OR_CONTINUE;
    }
    case 1995: { // LISTEN_TO_PLAYER_GROUP_COMMANDS (bit 0x20 de la tercera palabra de banderas del ped)
        CollectParameters(2);
        auto* ped                                  = GetPedPool()->GetAtRef(sp[0].iParam);
        ped->bDoesntListenToPlayerGroupCommands = sp[1].iParam == 0;
        return OR_CONTINUE;
    }
    case 1996: // SET_PLAYER_ENTER_CAR_BUTTON
        CollectParameters(2);
        CPad::GetPad(sp[0].iParam)->bDisablePlayerEnterCar = sp[1].iParam == 0;
        return OR_CONTINUE;
    case 1997: { // TASK_CHAR_SLIDE_TO_COORD: secuencia (ir al punto + deslizarse); el rumbo se pasa a radianes y un tiempo de mezcla negativo pasa a 0.1
        CollectParameters(6);
        const float   heading = sp[4].fParam * 0.017453292f; // 0x8595EC
        float         blend   = sp[5].fParam;
        if (blend < 0.0f) {
            blend = 0.1f; // 0x3DCCCCCD
        }
        const CVector target{ sp[1].fParam, sp[2].fParam, sp[3].fParam };
        const auto    pedHandle = sp[0].iParam;
        CTaskComplexSequence* sequence = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskComplexSequence))) {
            sequence = ::new (memory) CTaskComplexSequence();
        }
        CTask* goTo = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskComplexGoToPointAndStandStill))) {
            goTo = ::new (memory) CTaskComplexGoToPointAndStandStill((eMoveState)4, target, 0.5f, 2.0f, false, false); // 0x86FC84 / 0x86FC88
        }
        sequence->AddTask(goTo);
        CTask* slide = nullptr;
        if (auto* memory = CTask::operator new(sizeof(CTaskSimpleSlideToCoord))) {
            slide = ::new (memory) CTaskSimpleSlideToCoord(target, heading, blend);
        }
        sequence->AddTask(slide);
        GivePedScriptedTask(pedHandle, sequence, command);
        return OR_CONTINUE;
    }

    case 1902: // sin accion en el EXE (0x46CBCB): ni leen parametros
    case 1907:
    case 1908:
    case 1909:
    case 1913:
    case 1919:
    case 1925:
    case 1927:
    case 1933:
    case 1934:
    case 1937:
    case 1941:
    case 1946:
    case 1947:
    case 1948:
    case 1954:
    case 1962:
    case 1965:
    case 1966:
    case 1970:
    case 1973:
    case 1974:
    case 1975:
    case 1976:
    case 1978:
    case 1986:
    case 1992:
    case 1994:
    case 1998:
    case 1999:
        return OR_CONTINUE;

    default: // 1977 (TASK_KILL_CHAR_ON_FOOT_PATROL) y fuera de 1900..1999: 0x46CBCF
        return OR_INTERRUPT;
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Lote 239A (sin validar; el listado decia CTheScripts: es CRunningScript::ProcessCommands800To899, el despachador del EXE para los
// comandos 801..899). Sin banco: la validacion par a par la hace el coordinador.
// ---------------------------------------------------------------------------------------------------------------------------------
namespace {
// Cajas de colision de ROTATE_OBJECT (0x481DB2..0x4822E9) y SLIDE_OBJECT (0x48252B..0x482A1D): transforma cuatro esquinas de la caja del
// modelo con la matriz (A = min, B = (max.x, min.y, max.z), C = (max.x, max.y, min.z), D = (min.x, max.y, max.z)), calcula el minimo y el
// maximo de X e Y de las cuatro con las cadenas de comparacion del EXE (`(b < a) ? b : a` para el minimo y `(b > a) ? b : a` para el maximo,
// que con NaN conservan el primer operando) y llama a CWorld::FindObjectsIntersectingAngledCollisionBox (0x568FF0) con maxCount = 2, sin
// array de salida y solo vehiculos y peds. Devuelve el contador (int16, el EXE lo compara con signo).
int16 ScriptObjectAngledBoxCount(CObject* obj, const CMatrix& matrix, const CVector& point) {
    CColModel* const col = CModelInfo::ms_modelInfoPtrs[static_cast<int16>(obj->m_nModelIndex)]->GetColModel();
    const CVector    bbMin = col->m_boundBox.m_vecMin;
    const CVector    bbMax = col->m_boundBox.m_vecMax;

    const CVector vA = matrix * CVector{ bbMin.x, bbMin.y, bbMin.z };
    const CVector vB = matrix * CVector{ bbMax.x, bbMin.y, bbMax.z };
    const CVector vC = matrix * CVector{ bbMax.x, bbMax.y, bbMin.z };
    const CVector vD = matrix * CVector{ bbMin.x, bbMax.y, bbMax.z };

    const auto minOf = [](float a, float b) { return (b < a) ? b : a; };
    const auto maxOf = [](float a, float b) { return (b > a) ? b : a; };

    const float minX = minOf(minOf(minOf(vA.x, vB.x), vC.x), vD.x);
    const float maxX = maxOf(maxOf(maxOf(vA.x, vB.x), vC.x), vD.x);
    const float minY = minOf(minOf(minOf(vA.y, vB.y), vC.y), vD.y);
    const float maxY = maxOf(maxOf(maxOf(vA.y, vB.y), vC.y), vD.y);

    int16 count{};
    CWorld::FindObjectsIntersectingAngledCollisionBox(col->m_boundBox, matrix, point, minX, minY, maxX, maxY, &count, 2, nullptr, false, true, true, false, false);
    return count;
}

// Un paso de SLIDE_OBJECT en un eje (0x4823D8..0x48251E): d = actual - destino; con d >= 0 (ordenado) resta la velocidad si d > velocidad y si no
// llega al destino; con d < 0 (o NaN) suma la velocidad si -d > velocidad y si no llega al destino.
float ScriptSlideStep(float current, float target, float speed) {
    const float d = current - target;
    if (d >= 0.0f) {
        if (d > speed) {
            return current - speed;
        }
        return target;
    }
    if (-d > speed) {
        return speed + current;
    }
    return target;
}
} // namespace

// 0x481300 (lote 239A, sin validar): despachador de los comandos 801..899 (indice = comando - 801, tabla de bytes 0x4836B8 y de saltos 0x483600).
// thiscall con un argumento de pila (`ret 4`), AL = 0 (OR_CONTINUE) o 0xFF (OR_INTERRUPT: comando que no maneja). Los comandos 809, 812, 894 y 899
// (y solo esos) devuelven 0 sin leer parametros; los demas sin caso devuelven -1. Cada caso recoge sus parametros con CollectParameters y los
// lee de ScriptParams.
OpcodeResult CRunningScript::ProcessCommands800To899(int32 commandId) {
    switch (commandId) {
    case COMMAND_EXPLODE_CHAR_HEAD: { // 0x48135B
        CollectParameters(1);
        CPed* const ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
        CPedDamageResponseCalculator calculator{ nullptr, 1000.0f, WEAPON_SNIPERRIFLE, PED_PIECE_HEAD, false };           // 0x4AD3F0
        CEventDamage                  event{ nullptr, CTimer::GetTimeInMS(), WEAPON_SNIPERRIFLE, PED_PIECE_HEAD, 0, false, static_cast<bool>(ped->bInVehicle) }; // 0x4AD830
        event.ComputeDamageResponseIfAffectsPed(ped, calculator, true); // AffectsPed (0x4B35A0) + ComputeDamageResponse (0x4B5AC0), o marca la respuesta como calculada
        ped->GetEventGroup().Add(&event, false); // 0x4AB420
        return OR_CONTINUE;
    }
    case COMMAND_ANCHOR_BOAT: { // 0x481435
        CollectParameters(2);
        auto* const boat = static_cast<CBoat*>(GetVehiclePool()->GetAtRef(ScriptParams[0].iParam));
        boat->m_nBoatFlags.bLockedToXY = ScriptParams[1].iParam != 0; // byte +0x5AC, bit 2
        return OR_CONTINUE;
    }
    case COMMAND_START_CAR_FIRE: { // 0x481479
        CollectParameters(1);
        CVehicle* const vehicle = GetVehiclePool()->GetAtRef(ScriptParams[0].iParam);
        const CVector   pos     = vehicle->GetPosition();
        ScriptParams[0].iParam  = gFireManager.StartScriptFire(pos, vehicle, 0.8f, 1, 0, 1); // 0x53A270
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_START_CHAR_FIRE: { // 0x4814E4
        CollectParameters(1);
        CPed* const   ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
        const CVector pos = ped->GetPosition();
        ScriptParams[0].iParam = gFireManager.StartScriptFire(pos, ped, 0.8f, 1, 0, 1); // 0x53A270
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_GET_RANDOM_CAR_OF_TYPE_IN_AREA: { // 0x48150A
        CollectParameters(5);
        int32 result = -1;
        auto* const pool = GetVehiclePool();
        int32 i = pool->GetSize();
        if (i != 0) {
            do {
                i--;
                if (result != -1) {
                    break;
                }
                CVehicle* const vehicle = pool->GetAt(i);
                if (!vehicle) {
                    continue;
                }
                if (vehicle->GetVehicleAppearance() != VEHICLE_APPEARANCE_AUTOMOBILE && vehicle->GetVehicleAppearance() != VEHICLE_APPEARANCE_BIKE) { // 0x6D1080 (la segunda llamada solo si la primera no da 1)
                    continue;
                }
                if (vehicle->vehicleFlags.bIsLawEnforcer) { // byte +0x428, bit 0
                    continue;
                }
                const int32 model = static_cast<int16>(vehicle->m_nModelIndex);
                if (model != ScriptParams[4].iParam && ScriptParams[4].iParam >= 0) {
                    continue;
                }
                if (!vehicle->CanBeDeleted()) { // 0x6D1180
                    continue;
                }
                if (!vehicle->IsWithinArea(ScriptParams[0].fParam, ScriptParams[1].fParam, ScriptParams[2].fParam, ScriptParams[3].fParam)) { // 0x54F200
                    continue;
                }
                result = pool->GetRef(vehicle);
                vehicle->SetVehicleCreatedBy(MISSION_VEHICLE);
                if (m_UsesMissionCleanup) {
                    CTheScripts::MissionCleanUp.AddEntityToList(result, MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);
                }
            } while (i != 0);
        }
        ScriptParams[0].iParam = result;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAMERA_ZOOM: { // 0x481619
        CollectParameters(1);
        const int16 mode = static_cast<int16>(TheCamera.GetActiveCam().m_nMode);
        if (mode == MODE_FOLLOWPED) {
            TheCamera.SetZoomValueFollowPedScript(static_cast<int16>(ScriptParams[0].u16Param)); // 0x50C160
        } else if (mode == MODE_CAM_ON_A_STRING) {
            TheCamera.SetZoomValueCamStringScript(static_cast<int16>(ScriptParams[0].u16Param)); // 0x50C1B0
        }
        return OR_CONTINUE;
    }
    case COMMAND_CREATE_PICKUP_WITH_AMMO: { // 0x481678
        CollectParameters(6);
        int32 model = ScriptParams[0].iParam;
        if (model < 0) {
            model = CTheScripts::UsedObjectArray.data()[-model].nModelIndex;
        }
        float       z = ScriptParams[5].fParam;
        const float x = ScriptParams[3].fParam;
        const float y = ScriptParams[4].fParam;
        if (z <= -100.0f) {
            z = CWorld::FindGroundZForCoord(x, y) + 0.5f; // 0x569660
        }
        static_cast<void>(CPickups::GetActualPickupIndex(tPickupReference{ CollectNextParameterWithoutIncreasingPC() })); // 0x464250 + 0x4552A0 (el resultado no se usa)
        ScriptParams[0].iParam = CPickups::GenerateNewOne_ExeAbi(CVector{ x, y, z }, static_cast<uint32>(model), static_cast<ePickupType>(ScriptParams[1].u8Param), ScriptParams[2].uParam, 0, 0, nullptr); // 0x456F20
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_SET_PLAYER_NEVER_GETS_TIRED: { // 0x48175F
        CollectParameters(2);
        CWorld::Players[ScriptParams[0].iParam].m_bDoesNotGetTired = ScriptParams[1].iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_SET_PLAYER_FAST_RELOAD: { // 0x48178E
        CollectParameters(2);
        CWorld::Players[ScriptParams[0].iParam].m_bFastReload = ScriptParams[1].iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_SET_CHAR_BLEEDING: { // 0x4817BD
        CollectParameters(2);
        CPed* const ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
        ped->bPedIsBleeding = ScriptParams[1].iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_SET_FREE_RESPRAYS: { // 0x481808
        CollectParameters(1);
        CGarages::RespraysAreFree = ScriptParams[0].iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_SET_CHAR_VISIBLE: { // 0x481824
        CollectParameters(2);
        CPed* const ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
        if (ped == FindPlayerPed(-1)) {
            gPlayerPedVisible = ScriptParams[1].iParam != 0;
        }
        ped->SetIsVisible(ScriptParams[1].iParam != 0);
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_VISIBLE: { // 0x481880
        CollectParameters(2);
        CVehicle* const vehicle = GetVehiclePool()->GetAtRef(ScriptParams[0].iParam);
        vehicle->SetIsVisible(ScriptParams[1].iParam != 0);
        return OR_CONTINUE;
    }
    case COMMAND_IS_AREA_OCCUPIED: { // 0x4818BB
        CollectParameters(11);
        float x1 = ScriptParams[0].fParam, x2 = ScriptParams[3].fParam;
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        float y1 = ScriptParams[1].fParam, y2 = ScriptParams[4].fParam;
        if (y1 > y2) {
            std::swap(y1, y2);
        }
        float z1 = ScriptParams[2].fParam, z2 = ScriptParams[5].fParam;
        if (z1 > z2) {
            std::swap(z1, z2);
        }
        const CVector cornerMin{ x1, y1, z1 };
        const CVector cornerMax{ x2, y2, z2 };
        int16         count{};
        CWorld::FindObjectsIntersectingCube(
            cornerMin, cornerMax, &count, 2, nullptr,
            ScriptParams[6].iParam != 0, ScriptParams[7].iParam != 0, ScriptParams[8].iParam != 0, ScriptParams[9].iParam != 0, ScriptParams[10].iParam != 0
        ); // 0x568DD0
        UpdateCompareFlag(count > 0);
        return OR_CONTINUE;
    }
    case COMMAND_DISPLAY_TEXT: { // 0x481A08
        CollectParameters(2);
        char label[8];
        ReadTextLabelFromScript(label, 8);
        static_cast<void>(TheText.Get(label)); // 0x6A0050 (el resultado no se usa)
        auto& line = CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame];
        line.Pos.x = ScriptParams[0].fParam;
        line.Pos.y = ScriptParams[1].fParam;
        strncpy(line.GXTKey, label, 8); // 0x821F40
        CTheScripts::NumberOfIntroTextLinesThisFrame++;
        line.NumberToInsert1 = -1;
        line.NumberToInsert2 = -1;
        return OR_CONTINUE;
    }
    case COMMAND_SET_TEXT_SCALE: { // 0x481AAE
        CollectParameters(2);
        auto& line   = CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame];
        line.Scale.x = ScriptParams[0].fParam;
        line.Scale.y = ScriptParams[1].fParam;
        return OR_CONTINUE;
    }
    case COMMAND_SET_TEXT_COLOUR: { // 0x481ADE
        CollectParameters(4);
        const CRGBA color{ ScriptParams[0].u8Param, ScriptParams[1].u8Param, ScriptParams[2].u8Param, ScriptParams[3].u8Param }; // 0x7170C0
        auto&       line = CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame];
        line.Color       = color;
        return OR_CONTINUE;
    }
    case COMMAND_SET_TEXT_JUSTIFY: { // 0x481B47
        CollectParameters(1);
        CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame].Justify = ScriptParams[0].iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_SET_TEXT_CENTRE: { // 0x481B84
        CollectParameters(1);
        CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame].IsCentered = ScriptParams[0].iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_SET_TEXT_WRAPX: { // 0x481BC1
        CollectParameters(1);
        CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame].WrapX = ScriptParams[0].fParam;
        return OR_CONTINUE;
    }
    case COMMAND_SET_TEXT_CENTRE_SIZE: { // 0x481BE5
        CollectParameters(1);
        CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame].CentreSize = ScriptParams[0].fParam;
        return OR_CONTINUE;
    }
    case COMMAND_SET_TEXT_BACKGROUND: { // 0x481C08
        CollectParameters(1);
        CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame].HasBg = ScriptParams[0].iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_SET_TEXT_PROPORTIONAL: { // 0x481C45
        CollectParameters(1);
        CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame].IsProportional = ScriptParams[0].iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_SET_TEXT_FONT: { // 0x481C82
        CollectParameters(1);
        CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame].FontStyle.get_underlying_ref() = ScriptParams[0].iParam; // el EXE guarda los 32 bits (0x481C9A: mov [edx+0xA91410],eax); un static_cast<eFontStyle> los truncaba a 8
        return OR_CONTINUE;
    }
    case COMMAND_ROTATE_OBJECT: { // 0x481CA5
        CollectParameters(4);
        CObject* const obj = GetObjectPool()->GetAtRef(ScriptParams[0].iParam);

        float heading = obj->GetHeading() * 57.2957763671875f; // 0x441DB0, grados
        if (heading < 0.0f) {
            heading += 360.0f;
        }
        const float target = ScriptParams[1].fParam;
        const float speed  = ScriptParams[2].fParam;
        bool        result = false;

        if (heading == target) {
            UpdateCompareFlag(true);
            return OR_CONTINUE;
        }

        float distCcw = target - heading;
        float distCw  = heading - target;
        if (distCcw < 0.0f) {
            distCcw += 360.0f;
        }
        if (distCw < 0.0f) {
            distCw += 360.0f;
        }
        float newAngle;
        if (distCcw < distCw) {
            newAngle = (distCcw < speed) ? target : speed + heading;
        } else {
            newAngle = (distCw < speed) ? target : heading - speed;
        }

        if (ScriptParams[3].iParam != 0) {
            const CVector objPos = obj->GetPosition();
            CMatrix       matrix;
            matrix.SetRotateZ(newAngle * 0.01745329238474369f); // 0x59B0E0
            matrix.GetPosition().x = matrix.GetPosition().x + objPos.x;
            matrix.GetPosition().y = matrix.GetPosition().y + objPos.y;
            matrix.GetPosition().z = matrix.GetPosition().z + objPos.z;
            if (ScriptObjectAngledBoxCount(obj, matrix, objPos) > 0) {
                UpdateCompareFlag(true);
                return OR_CONTINUE;
            }
        }

        obj->SetHeading(newAngle * 0.01745329238474369f); // 0x43E0C0
        obj->UpdateRwMatrix();                            // 0x446F90
        obj->UpdateRwFrame();                             // 0x532B00
        if (newAngle == target) {
            result = true;
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    case COMMAND_SLIDE_OBJECT: { // 0x482339
        CollectParameters(8);
        CObject* const obj = GetObjectPool()->GetAtRef(ScriptParams[0].iParam);
        const CVector  cur = obj->GetPosition();
        const float    tx = ScriptParams[1].fParam, ty = ScriptParams[2].fParam, tz = ScriptParams[3].fParam;

        if (cur.x == tx && cur.y == ty && cur.z == tz) {
            UpdateCompareFlag(true);
            return OR_CONTINUE;
        }

        const float speedX = ScriptParams[4].fParam;
        const float speedY = ScriptParams[5].fParam;
        const float speedZ = ScriptParams[6].fParam;
        // El EXE calcula las tres diferencias antes de los pasos (dx -> pila x87, dy -> [esp+38h], dz -> [esp+50h]); cada paso usa la suya
        const CVector newPos{ ScriptSlideStep(cur.x, tx, speedX), ScriptSlideStep(cur.y, ty, speedY), ScriptSlideStep(cur.z, tz, speedZ) };

        if (ScriptParams[7].iParam != 0) {
            CMatrix matrix;
            matrix = *obj->m_matrix; // 0x59BBC0
            matrix.GetPosition() = newPos;
            if (ScriptObjectAngledBoxCount(obj, matrix, newPos) > 0) {
                UpdateCompareFlag(true);
                return OR_CONTINUE;
            }
        }

        obj->Teleport(newPos, false); // ranura 14 de la vtable (+0x38)
        bool result = false;
        if (newPos.x == tx && newPos.y == ty && newPos.z == tz) {
            result = true;
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    case COMMAND_REMOVE_CHAR_ELEGANTLY: { // 0x482A96
        CollectParameters(1);
        CPed* const ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
        if (ped && ped->GetCreatedBy() == PED_MISSION) {
            if (ped->bInVehicle && ped->m_pVehicle) {
                CTheScripts::RemoveThisPed(ped); // 0x486240
            } else {
                ped->SetCharCreatedBy(PED_GAME); // 0x5E47E0
                if (CPedGroup* const group = CPedGroups::GetPedsGroup(ped)) { // 0x5F7E80
                    CPedGroupMembership& membership = group->GetMembership();
                    if (membership.IsFollower(ped)) { // 0x5F69E0
                        membership.RemoveMember(ped); // 0x5FB210
                    }
                }
                CPopulation::ms_nTotalMissionPeds--;
                ped->bFadeOut = true;
                CWorld::RemoveReferencesToDeletedObject(ped); // 0x565510
            }
        }
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(ScriptParams[0].iParam, MISSION_CLEANUP_ENTITY_TYPE_PED);
        }
        return OR_CONTINUE;
    }
    case COMMAND_SET_CHAR_STAY_IN_SAME_PLACE: { // 0x482B4D
        CollectParameters(2);
        CPed* const ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
        ped->SetStayInSamePlace(ScriptParams[1].iParam != 0); // 0x481090
        return OR_CONTINUE;
    }
    case COMMAND_IS_EXPLOSION_IN_AREA: { // 0x482B89
        CollectParameters(7);
        float x1 = ScriptParams[1].fParam, x2 = ScriptParams[4].fParam;
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        float y1 = ScriptParams[2].fParam, y2 = ScriptParams[5].fParam;
        if (y1 > y2) {
            std::swap(y1, y2);
        }
        float z1 = ScriptParams[3].fParam, z2 = ScriptParams[6].fParam;
        if (z1 > z2) {
            std::swap(z1, z2);
        }
        bool result = false;
        if (CExplosion::TestForExplosionInArea(static_cast<eExplosionType>(ScriptParams[0].iParam), x1, x2, y1, y2, z1, z2)) { // 0x736950
            result = true;
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    case COMMAND_PLACE_OBJECT_RELATIVE_TO_CAR: { // 0x482C67
        CollectParameters(5);
        CObject* const  obj     = GetObjectPool()->GetAtRef(ScriptParams[0].iParam);
        CVehicle* const vehicle = GetVehiclePool()->GetAtRef(ScriptParams[1].iParam);
        const CVector   offset{ ScriptParams[2].fParam, ScriptParams[3].fParam, ScriptParams[4].fParam };
        CPhysical::PlacePhysicalRelativeToOtherPhysical(vehicle, obj, offset); // 0x546DB0
        return OR_CONTINUE;
    }
    case COMMAND_MAKE_OBJECT_TARGETTABLE: { // 0x482CDB
        CollectParameters(2);
        CObject* const obj    = GetObjectPool()->GetAtRef(ScriptParams[0].iParam);
        CPed* const    player = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
        if (ScriptParams[1].iParam != 0) {
            obj->SetObjectTargettable(true); // 0x59F300
        } else {
            obj->SetObjectTargettable(false);
            CEntity** const target = &player->m_pTargetedObject; // +0x71C
            if (obj == *target) {
                if (*target) {
                    (*target)->CleanUpOldReference(target); // 0x571A00
                }
                *target = nullptr;
            }
        }
        return OR_CONTINUE;
    }
    case COMMAND_ADD_ARMOUR_TO_CHAR: { // 0x482D52
        CollectParameters(2);
        CPed* const ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
        // fild + fadd: la suma se redondea una sola vez a 24 bits (palabra de control del EXE)
        ped->m_fArmour = static_cast<float>(static_cast<double>(ScriptParams[1].iParam) + static_cast<double>(ped->m_fArmour));
        if (ped->IsPlayer()) { // 0x5DF8F0
            const int32 slot      = CWorld::FindPlayerSlotWithPedPointer(ped); // 0x563FA0
            const float maxArmour = static_cast<float>(CWorld::Players[slot].m_nMaxArmour);
            ped->m_fArmour        = (maxArmour > ped->m_fArmour) ? ped->m_fArmour : maxArmour;
        } else {
            ped->m_fArmour = (ped->m_fArmour < 100.0f) ? ped->m_fArmour : 100.0f;
        }
        ped->m_fArmour = (ped->m_fArmour > 0.0f) ? ped->m_fArmour : 0.0f;
        return OR_CONTINUE;
    }
    case COMMAND_OPEN_GARAGE: { // 0x482E1D
        char label[8];
        ReadTextLabelFromScript(label, 8);
        const int16 garage = CGarages::GetGarageNumberByName(label); // 0x447680
        if (garage >= 0) {
            CGarages::aGarages[garage].OpenThisGarage(); // 0x447D50
        }
        return OR_CONTINUE;
    }
    case COMMAND_CLOSE_GARAGE: { // 0x482E60
        char label[8];
        ReadTextLabelFromScript(label, 8);
        const int16 garage = CGarages::GetGarageNumberByName(label); // 0x447680
        if (garage >= 0) {
            CGarages::aGarages[garage].CloseThisGarage(); // 0x447D70
        }
        return OR_CONTINUE;
    }
    case COMMAND_WARP_CHAR_FROM_CAR_TO_COORD: { // 0x482EA3
        CollectParameters(4);
        CPed* const ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
        const float x   = ScriptParams[1].fParam;
        const float y   = ScriptParams[2].fParam;
        float       z   = ScriptParams[3].fParam;
        if (z <= -100.0f) {
            z = CWorld::FindGroundZForCoord(x, y); // 0x569660
        }
        ped->GetIntelligence()->FlushImmediately(true); // 0x601640
        const float offsetZ = ped->GetDistanceFromCentreOfMassToBaseOfModel(); // 0x536BE0
        z                   = offsetZ + z;
        const CVector pos{ x, y, z };
        ped->Teleport(pos, false); // ranura 14 de la vtable (+0x38)
        CTheScripts::ClearSpaceForMissionEntity(pos, ped); // 0x486B00
        return OR_CONTINUE;
    }
    case COMMAND_SET_VISIBILITY_OF_CLOSEST_OBJECT_OF_TYPE: { // 0x482F84
        CollectParameters(6);
        const float x = ScriptParams[0].fParam;
        const float y = ScriptParams[1].fParam;
        float       z = ScriptParams[2].fParam;
        if (z <= -100.0f) {
            z = CWorld::FindGroundZForCoord(x, y); // 0x569660
        }
        const float radius   = ScriptParams[3].fParam;
        float       bestDist = radius + radius;
        int32       model    = ScriptParams[4].iParam;
        if (model < 0) {
            model = CTheScripts::UsedObjectArray.data()[-model].nModelIndex;
        }
        CEntity* found[16];
        int16    count{};
        const CVector pos{ x, y, z };
        CWorld::FindObjectsOfTypeInRange(static_cast<uint32>(model), pos, radius, true, &count, 16, found, true, false, false, true, true); // 0x564C70
        if (count == 0) {
            const CVector pos2{ x, y, z };
            CWorld::FindLodOfTypeInRange(static_cast<uint32>(model), pos2, radius, true, &count, 16, found); // 0x564ED0
        }
        CEntity* closest = nullptr;
        const int32 foundCount = count; // movsx
        for (uint16 i = 0; i < foundCount; i++) {
            CEntity* const entity = found[i];
            const CVector& ePos   = entity->GetPosition();
            const float    dz     = ePos.z - z;
            const float    dy     = ePos.y - y;
            const float    dx     = ePos.x - x;
            const float    dist   = std::sqrt((dx * dx + dy * dy) + dz * dz);
            if (dist < bestDist) {
                bestDist = dist;
                closest  = entity;
            }
        }
        if (closest) {
            if (ScriptParams[5].iParam != 0) {
                const uint8 iplIndex = closest->GetIplIndex();
                closest->m_nFlags |= 0x81u; // bIsVisible | bUsesCollision
                if (iplIndex == 0) {
                    uint16 slot      = 0;
                    bool   foundSlot = false;
                    do {
                        if (foundSlot) {
                            break;
                        }
                        if (CTheScripts::InvisibilitySettingArray.data()[slot] == closest) {
                            foundSlot = true;
                        } else {
                            slot++;
                        }
                    } while (slot < 0x14);
                    if (foundSlot) {
                        CTheScripts::InvisibilitySettingArray.data()[slot] = nullptr;
                    }
                }
            } else {
                closest->m_nFlags &= 0xFFFFFF7Eu;
                CTheScripts::AddToInvisibilitySwapArray(closest, false); // 0x481200
            }
        }
        return OR_CONTINUE;
    }
    case COMMAND_HAS_CHAR_SPOTTED_CHAR: { // 0x4831A2
        CollectParameters(2);
        CPed* const ped    = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
        CPed* const target = GetPedPool()->GetAtRef(ScriptParams[1].iParam);
        bool        result = false;
        if (ped->OurPedCanSeeThisEntity(target, true)) { // 0x5E1660
            result = true;
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    case COMMAND_HAS_OBJECT_BEEN_DAMAGED: { // 0x4831F8
        CollectParameters(1);
        CObject* const obj    = GetObjectPool()->GetAtRef(ScriptParams[0].iParam);
        bool           result = false;
        if ((obj->m_bRenderDamaged || !obj->m_bIsVisible)) { // CEntity::IsInvisibleOrRenderDamaged (bandera de dano o invisible)
            result = true;
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    case COMMAND_WARP_CHAR_INTO_CAR: { // 0x483239
        CollectParameters(2);
        CPed* const     ped     = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
        CVehicle* const vehicle = GetVehiclePool()->GetAtRef(ScriptParams[1].iParam);
        ped->GetIntelligence()->FlushImmediately(false); // 0x601640
        CTaskSimpleCarSetPedInAsDriver task{ vehicle, nullptr }; // 0x6470E0
        task.m_bWarpingInToCar = true;
        task.ProcessPed(ped); // 0x64B950; el destructor (0x647170) sale al terminar el caso
        return OR_CONTINUE;
    }
    case COMMAND_PRINT_WITH_2_NUMBERS_BIG: { // 0x4832BB
        char label[8];
        ReadTextLabelFromScript(label, 8);
        const GxtChar* const text = TheText.Get(label); // 0x6A0050
        CollectParameters(4);
        CMessages::AddBigMessageWithNumber(text, ScriptParams[2].uParam, static_cast<eMessageStyle>(ScriptParams[3].iParam - 1), ScriptParams[0].iParam, ScriptParams[1].iParam, -1, -1, -1, -1); // 0x69E5F0
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAMERA_BEHIND_PLAYER: // 0x48331B
        TheCamera.SetCameraDirectlyBehindForFollowPed_CamOnAString(); // 0x50BD40
        return OR_CONTINUE;
    case COMMAND_CREATE_RANDOM_CHAR: { // 0x48332A
        CollectParameters(3);
        eModelID model{};
        int32    pedType = 6;
        uint16   tries   = 0;
        while (pedType != 5 && tries < 5) {
            model = CPopulation::ChooseCivilianOccupation(false, false, ANIM_GROUP_NONE, MODEL_INVALID, ePedStats::NONE, false, false, false, nullptr); // 0x612F90
            CBaseModelInfo* const info = CModelInfo::ms_modelInfoPtrs[model];
            if (info->GetRwObjectNoChecks()) {
                pedType = static_cast<CPedModelInfo*>(info)->m_nPedType;
            }
            tries++;
            if (pedType == 4) {
                break;
            }
        }
        if (!CModelInfo::ms_modelInfoPtrs[model]->GetRwObjectNoChecks()) {
            pedType = static_cast<CPedModelInfo*>(CModelInfo::ms_modelInfoPtrs[7])->m_nPedType;
            model   = static_cast<eModelID>(7);
        }
        void* const  pedMemory = CPed::operator new(0x79C); // 0x5E4720
        CPed* const  ped       = pedMemory ? ::new (pedMemory) CCivilianPed(static_cast<ePedType>(pedType), static_cast<uint32>(model)) : nullptr;
        void* const  taskMemory = CTask::operator new(0x20); // 0x61A5A0
        CTask* const task       = taskMemory ? ::new (taskMemory) CTaskSimpleStandStill(999999, true, false, 8.0f) : nullptr; // 0x62F310
        ped->GetTaskManager().SetTask(task, TASK_PRIMARY_DEFAULT, false); // CTaskManager::SetTask
        ped->SetCharCreatedBy(PED_MISSION); // 0x5E47E0
        ped->bAllowMedicsToReviveMe = false; // bit 31 de las banderas (+0x46C)

        const float px = ScriptParams[0].fParam;
        const float py = ScriptParams[1].fParam;
        float       pz = ScriptParams[2].fParam;
        if (pz <= -100.0f) {
            pz = CWorld::FindGroundZForCoord(px, py); // 0x569660
        }
        pz = pz + 1.0f;
        const CVector pos{ px, py, pz };
        ped->SetPosn(pos);           // 0x4241C0
        ped->SetOrientation(0.0f, 0.0f, 0.0f); // 0x439A80
        CTheScripts::ClearSpaceForMissionEntity(pos, ped); // 0x486B00
        if (m_UsesMissionCleanup) {
            ped->m_nFlags |= 0x40000u; // bIsStaticWaitingForCollision
        }
        CWorld::Add(ped); // 0x563220
        CPopulation::ms_nTotalMissionPeds = CPopulation::ms_nTotalMissionPeds + 1;
        ScriptParams[0].iParam = GetPedPool()->GetRef(ped); // 0x4442D0
        StoreParameters(1);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(ScriptParams[0].iParam, MISSION_CLEANUP_ENTITY_TYPE_PED); // 0x4637E0
        }
        return OR_CONTINUE;
    }
    case COMMAND_SET_OBJECT_VELOCITY: { // 0x48352B
        CollectParameters(4);
        CObject* const obj = GetObjectPool()->GetAtRef(ScriptParams[0].iParam);
        const float    vz  = ScriptParams[3].fParam * 0.02f;
        const float    vy  = ScriptParams[2].fParam * 0.02f;
        const float    vx  = ScriptParams[1].fParam * 0.02f;
        obj->SetVelocity(CVector{ vx, vy, vz }); // 0x441130
        return OR_CONTINUE;
    }
    case COMMAND_SET_OBJECT_COLLISION: { // 0x483580
        CollectParameters(2);
        CObject* const obj = GetObjectPool()->GetAtRef(ScriptParams[0].iParam);
        obj->m_bUsesCollision = ScriptParams[1].iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_HAS_RESPRAY_HAPPENED:  // 0x329
    case COMMAND_SET_CAR_RAM_CAR:       // 0x32C
    case COMMAND_IS_SNIPER_BULLET_IN_AREA: // 0x37E
    case COMMAND_IS_ICECREAM_JINGLE_ON: // 0x383
        return OR_CONTINUE; // 0x4835B1: no leen parametros
    default:
        return OR_INTERRUPT; // 0x4835CC
    }
}
