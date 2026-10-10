/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRunningScript
 * Reconstrucción: COMPLETA — 39/39 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 39/39 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "Tasks/TaskTypes/TaskSimpleFinishBrain.h"
#include "Tasks/ScriptedBrainTaskStore.h"
#include "Tasks/TaskTypes/TaskSimpleRunNamedAnim.h"
#include "Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h"
#include "Tasks/TaskSequences.h"
#include "Tasks/TaskTypes/TaskSimpleHoldEntity.h"
#include "Tasks/PedScriptedTaskRecord.h"
#include "ShotInfo.h"

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
    RH_ScopedInstall(Process, 0x469F00);
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
    NOTSA_LOG_DEBUG("Script cmds dumped! Find them in `<GTA Directory>/Scripts`!");
    NOTSA_LOG_DEBUG("Script cmds reverse progress: {}/{} ({:.2f}% done)", reversed, total, 100.0f * ((float)reversed / (float)total));
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
    const float halfPi=std::bit_cast<float>(0x3FC90FDBu),twoPi=std::bit_cast<float>(0x40C90FDBu),zero=0.0f;
    auto radianFunction = static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints);
    float cornerCX,cornerCY,cornerDX,cornerDY,minusCosWidth;
    __asm {
        push endY
        push endX
        push startY
        push startX
        call radianFunction
        add esp,16
        fadd halfPi
        fcom zero
        fnstsw ax
        test ah,5
        jp nonnegative
    negative:
        fadd twoPi
        fcom zero
        fnstsw ax
        test ah,5
        jnp negative
    nonnegative:
        fcom twoPi
        fnstsw ax
        test ah,41h
        jne normalized
    tooLarge:
        fsub twoPi
        fcom twoPi
        fnstsw ax
        test ah,41h
        je tooLarge
    normalized:
        fld st(0)
        fsin
        fmul angledWidth
        fld endX
        fadd st(0),st(1)
        fstp cornerDX
        fxch st(1)
        fcos
        fmul angledWidth
        fchs
        fst minusCosWidth
        fadd endY
        fstp cornerDY
        fadd startX
        fstp cornerCX
        fld minusCosWidth
        fadd startY
        fstp cornerCY
    }
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

    if (const auto handler = CustomCommandHandlerOf((eScriptCommands)(op.Command))) {
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
