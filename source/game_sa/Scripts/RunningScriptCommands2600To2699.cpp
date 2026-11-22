#include "StdInc.h"

#include <bit>
#include <utility>

#include "RunningScript.h"
#include "TheScripts.h"
#include "Pools/Pools.h"
#include "PedGroups.h"
#include "World.h"
#include "PlayerInfo.h"
#include "PlayerPedData.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Vehicle/Automobile.h"
#include "Camera.h"
#include "GameLogic.h"
#include "Cheat.h"
#include "Hud.h"
#include "Messages.h"
#include "Pad.h"
#include "3dMarkers.h"
#include "PedIntelligence.h"
#include "Text/Text.h"
#include "Conversations/Conversations.h"
#include "Tasks/TaskTypes/TaskSimpleSwim.h"
#include "Tasks/TaskTypes/TaskComplexFollowNodeRoute.h"
#include "Tasks/TaskTypes/TaskComplexProstituteSolicit.h"

// F4 de codigo vivo (sin validar): 0x479DA0 (2340 bytes, hasta 0x47A6C3; la tabla de saltos empieza en 0x47A6C4), CRunningScript::ProcessCommands2600To2699, el despachador original de los comandos de script
// 2600..2638 (ranura 26 de la tabla 0x8A6168; la usa ProcessOneCommand cuando el comando no tiene manejador propio). Thiscall con un argumento de pila (`ret 4`; resultado en AL). Prologo SEH (marco con
// manejador 0x8382AE): solo protege el `new` de CTaskComplexFollowNodeRoute (el destructor de desenrollado es operator delete), en C++ no hace falta.
// Salto con una tabla directa de solo 39 entradas (0x47A6C4; `cmp eax,0x26`; indice = comando - 2600): 3 comandos van a 0x47A6A2 y devuelven 0 sin leer parametros (2612 SET_UP_SKIP_TO_BE_FINISHED_BY_SCRIPT,
// 2616 SET_RENDER_PLAYER_WEAPON y 2626 REMOVE_ALL_USER_3D_MARKERS) y los fuera de rango (2639..2699: el ultimo comando del juego es 2638, DO_DEBUG_STUFF; el despachador siguiente, 0x47A762, es el de 2500) van a
// 0x47A6A6 (OR_INTERRUPT, -1). Dos comandos (2633 IS_XBOX_VERSION y 2637 IS_JAPANESE_VERSION) comparten el destino 0x47A68F: UpdateCompareFlag(false). Cada `case` replica el orden de colaboradores del
// desensamblado; los parametros se leen de ScriptParams (0xA43C78). Los indices que vienen del script (jugador, pools) se usan sin comprobar rangos, como el EXE. Las comparaciones x87 con `fcomp` + `test ah,1` /
// `test ah,41h` + `jp` se replican con NaN: un NaN siempre descarta el candidato.
namespace {
// 0x86FCA4 y 0x86FCA8: 3.0 y 2.0 (distancia de frenado y cambio de altura de CTaskComplexFollowNodeRoute); 0x86FCAC: 50000 (tiempo por defecto, ms)
constexpr float EXE_FOLLOW_SLOWDOWN     = std::bit_cast<float>(0x40400000u);
constexpr float EXE_FOLLOW_HEIGHT       = std::bit_cast<float>(0x40000000u);
constexpr int32 EXE_FOLLOW_DEFAULT_TIME = 0xC350;
// 0x447A0000: 1000.0, salud completa que escribe FIX_CAR tras Fix()
constexpr float EXE_FULL_VEHICLE_HEALTH = std::bit_cast<float>(0x447A0000u);

inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x404910 (CPool<CPed>::GetAtRef) y 0x4048E0 (CPool<CVehicle>::GetAtRef): sin comprobar el resultado, como el EXE.
inline CPed* PedOf(int32 handle) {
    return CPools::GetPed(handle);
}
inline CVehicle* VehicleOf(int32 handle) {
    return CPools::GetVehicle(handle);
}
// 0xA44B88 - id * 0x1C: modelo negativo del script = entrada -id de CTheScripts::UsedObjectArray (su nModelIndex); si no, el propio valor.
inline int32 ScriptModelIndex(int32 id) {
    return id >= 0 ? id : CTheScripts::UsedObjectArray[-id].nModelIndex;
}
} // namespace

static_assert(offsetof(CPhysical, m_pAttachedTo) == 0xFC);

static_assert(offsetof(CPed, m_pAttachedTo) == 0xFC);

static_assert(offsetof(CVehicle, m_fHealth) == 0x4C0);
static_assert(offsetof(CTaskSimpleSwim, m_fAnimSpeed) == 0x10);
static_assert(offsetof(CPlayerInfo, m_pPed) == 0 && sizeof(CPlayerInfo) == 0x190);
static_assert(offsetof(CPlayerInfo, m_PlayerData) + offsetof(CPlayerPedData, m_nModelIndexOfLastBuildingShot) == 0xA0);
static_assert(offsetof(CCamera, m_nCarZoom) == 0xB4);
static_assert(CHEAT_PROSTITUTES_PAY_YOU == 0x5A);

// 0x479DA0
OpcodeResult CRunningScript::ProcessCommands2600To2699(int32 command) {
    switch (command) {
    // 0x479DDA
    case COMMAND_SET_SWIM_SPEED: {
        CollectParameters(2);
        CPed* const ped   = PedOf(P(0).iParam);
        const float speed = P(1).fParam;
        if (ped) {
            if (auto* const swim = ped->GetIntelligence()->GetTaskSwim()) { // el EXE llama dos veces a GetTaskSwim (0x601070)
                swim->m_fAnimSpeed = speed;
            }
        }
        return OR_CONTINUE;
    }
    // 0x479E34
    case COMMAND_IS_PLAYER_CLIMBING: {
        CollectParameters(1);
        CPlayerPed* const player = CWorld::Players[P(0).iParam].m_pPed;
        UpdateCompareFlag(player->GetIntelligence()->GetTaskClimb() != nullptr); // 0x601180
        return OR_CONTINUE;
    }
    // 0x479E75
    case COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED: {
        char label[8];
        ReadTextLabelFromScript(label, 8);
        const GxtChar* const text = TheText.Get(label); // 0x6A0050
        bool                 same = false;
        if (CHud::HelpMessageDisplayed()) { // 0x588B50
            GxtChar buffer[400];
            CMessages::StringCopy(buffer, text, 0x190);                 // 0x69DB70
            CMessages::InsertPlayerControlKeysInString(buffer);         // 0x69E160
            const uint32 length = CMessages::GetStringLength(buffer);   // 0x69DB50
            same                = CMessages::StringCompare(buffer, CHud::m_pHelpMessageToPrint, static_cast<uint16>(length)); // 0x69DBD0 con 0xBAA480
        }
        UpdateCompareFlag(same);
        return OR_CONTINUE;
    }
    // 0x479EF0: byte 0xBA6793 (FrontEndMenuManager.m_bWidescreenOn)
    case COMMAND_IS_WIDESCREEN_ON_IN_OPTIONS: {
        UpdateCompareFlag(StaticRef<uint8>(0xBA6793) != 0);
        return OR_CONTINUE;
    }
    // 0x479F07
    case COMMAND_DRAW_SUBTITLES_BEFORE_FADE: {
        CollectParameters(1);
        StaticRef<uint8>(0xA44488) = P(0).u8Param; // CTheScripts::bDrawSubtitlesBeforeFade (solo el byte bajo)
        return OR_CONTINUE;
    }
    // 0x479F21
    case COMMAND_DRAW_ODDJOB_TITLE_BEFORE_FADE: {
        CollectParameters(1);
        StaticRef<uint8>(0xA44489) = P(0).u8Param; // CTheScripts::bDrawOddJobTitleBeforeFade (solo el byte bajo)
        return OR_CONTINUE;
    }
    // 0x479F3D
    case COMMAND_TASK_FOLLOW_PATH_NODES_TO_COORD_WITH_RADIUS: {
        CollectParameters(7);
        const int32  pedHandle = P(0).iParam;
        const CVector target{ P(1).fParam, P(2).fParam, P(3).fParam };
        const int32  moveState = P(4).iParam;
        int32        time      = P(5).iParam;
        const float  radius    = P(6).fParam;
        if (time == -1) {
            time = EXE_FOLLOW_DEFAULT_TIME; // [0x86FCAC]
        } else if (time == -2) {
            time = -1;
        }
        auto* const task = new CTaskComplexFollowNodeRoute(static_cast<eMoveState>(moveState), target, radius, EXE_FOLLOW_SLOWDOWN, EXE_FOLLOW_HEIGHT, true, time, true); // 0x66EA30
        GivePedScriptedTask(pedHandle, task, command); // 0x465C20
        return OR_CONTINUE;
    }
    // 0x479FFD
    case COMMAND_SET_PHOTO_CAMERA_EFFECT: {
        CollectParameters(1);
        CTheScripts::bDrawCrossHair = P(0).iParam != 0 ? eCrossHairType::FIXED_DRAW_1STPERSON_WEAPON : eCrossHairType::NONE; // neg + sbb + and 2
        return OR_CONTINUE;
    }
    // 0x47A020
    case COMMAND_FIX_CAR: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        veh->Fix(); // vtable +0xC8
        veh->m_fHealth = EXE_FULL_VEHICLE_HEALTH;
        return OR_CONTINUE;
    }
    // 0x47A057
    case COMMAND_SET_PLAYER_GROUP_TO_FOLLOW_NEVER: {
        CollectParameters(2);
        const int32 playerId = P(0).iParam;
        const bool  enable   = P(1).iParam != 0;
        FindPlayerPed(playerId)->ForceGroupToNeverFollow(enable); // 0x56E210 y 0x60C800
        return OR_CONTINUE;
    }
    // 0x47A089
    case COMMAND_IS_CHAR_ATTACHED_TO_ANY_CAR: {
        CollectParameters(1);
        CPed* const ped      = PedOf(P(0).iParam);
        auto* const attached = ped->m_pAttachedTo; // +0xFC; el EXE no comprueba el ped
        UpdateCompareFlag(attached && attached->GetIsTypeVehicle());
        return OR_CONTINUE;
    }
    // 0x47A0D4
    case COMMAND_STORE_CAR_CHAR_IS_ATTACHED_TO_NO_SAVE: {
        CollectParameters(1);
        CPed* const ped      = PedOf(P(0).iParam);
        auto* const attached = ped->m_pAttachedTo;
        int32       handle   = -1;
        if (attached && attached->GetIsTypeVehicle()) {
            handle = static_cast<int32>(GetVehiclePool()->GetRef(static_cast<CVehicle*>(attached))); // 0x424160
        }
        P(0).iParam = handle;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47A129: SET_UP_SKIP_FOR_VEHICLE_FINISHED_BY_SCRIPT
    case COMMAND_SET_UP_SKIP_FOR_VEHICLE_FINISHED_BY_SCRIPT: {
        CollectParameters(5);
        const CVector coord{ P(0).fParam, P(1).fParam, P(2).fParam };
        const float   angle = P(3).fParam;
        CVehicle* const veh = VehicleOf(P(4).iParam);
        CGameLogic::SetUpSkip(coord, angle, false, veh, true); // 0x4423C0
        return OR_CONTINUE;
    }
    // 0x47A195
    case COMMAND_IS_SKIP_WAITING_FOR_SCRIPT_TO_FADE_IN: {
        UpdateCompareFlag(CGameLogic::IsSkipWaitingForScriptToFadeIn()); // 0x4416C0
        return OR_CONTINUE;
    }
    // 0x47A1B1
    case COMMAND_FORCE_ALL_VEHICLE_LIGHTS_OFF: {
        CollectParameters(1);
        StaticRef<uint8>(0xC1CC18) = P(0).u8Param; // CVehicle::ms_forceVehicleLightsOff (solo el byte bajo)
        return OR_CONTINUE;
    }
    // 0x47A1CB
    case COMMAND_GET_PLAYER_IN_CAR_CAMERA_MODE: {
        P(0).uParam = TheCamera.m_nCarZoom; // [0xB6F0DC]
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47A1E7
    case COMMAND_IS_LAST_BUILDING_MODEL_SHOT_BY_PLAYER: {
        CollectParameters(2);
        CPlayerInfo& info  = CWorld::Players[P(0).iParam];
        const int32  model = ScriptModelIndex(P(1).iParam);
        UpdateCompareFlag(static_cast<int32>(info.m_PlayerData.m_nModelIndexOfLastBuildingShot) == model); // Players[i] + 0xA0
        return OR_CONTINUE;
    }
    // 0x47A23B
    case COMMAND_CLEAR_LAST_BUILDING_MODEL_SHOT_BY_PLAYER: {
        CollectParameters(1);
        CWorld::Players[P(0).iParam].m_PlayerData.m_nModelIndexOfLastBuildingShot = static_cast<uint32>(-1);
        return OR_CONTINUE;
    }
    // 0x47A261
    case COMMAND_SET_UP_CONVERSATION_END_NODE_WITH_SCRIPTED_SPEECH: {
        char label[8];
        ReadTextLabelFromScript(label, 8);
        CollectParameters(1);
        CConversations::SetUpConversationNode(label, nullptr, nullptr, -P(0).iParam, 0, 0); // 0x43A870
        return OR_CONTINUE;
    }
    // 0x47A29C
    case COMMAND_ACTIVATE_PIMP_CHEAT: {
        CollectParameters(1);
        if (P(0).iParam != 0) {
            CCheat::ApplyCheat(CHEAT_PROSTITUTES_PAY_YOU); // 0x438370 con 0x5A
        } else {
            CCheat::Disable(CHEAT_PROSTITUTES_PAY_YOU);    // byte 0x96918A = 0
        }
        return OR_CONTINUE;
    }
    // 0x47A2CD
    case COMMAND_GET_RANDOM_CHAR_IN_AREA_OFFSET_NO_SAVE: {
        CollectParameters(6);
        const float cx = P(0).fParam;
        const float cy = P(1).fParam;
        const float cz = P(2).fParam;
        const float rx = P(3).fParam;
        const float ry = P(4).fParam;
        const float rz = P(5).fParam;
        // caja = centro -/+ radio (x87 a 24 bits: operaciones en float)
        const float minX = cx - rx;
        const float minY = cy - ry;
        const float minZ = cz - rz;
        const float maxX = rx + cx;
        const float maxY = ry + cy;
        const float maxZ = rz + cz;
        int32       result = -1;
        auto* const pool   = GetPedPool();
        // el EXE recorre la reserva desde el ultimo hueco al primero y se detiene en el primer candidato
        for (int32 i = static_cast<int32>(pool->GetSize()); i != 0;) {
            --i;
            CPed* const ped = pool->GetAt(i); // hueco libre (bit 7 de la bandera) o nulo: se salta
            if (!ped) {
                continue;
            }
            if (ped->GetCreatedBy() != PED_GAME) { // +0x484 == 1
                continue;
            }
            if (ped->m_bRemoveFromWorld) { // bit 11 de +0x1C
                continue;
            }
            if (ped->bFadeOut) { // bit 3 de +0x470
                continue;
            }
            if (IsPedDead(ped)) { // 0x464D70
                continue;
            }
            if (ped->bInVehicle) { // bit 8 de +0x46C
                continue;
            }
            if (CPedGroups::GetPedsGroup(ped)) { // 0x5F7E80
                continue;
            }
            const CVector pos = ped->GetPosition(); // matriz + 0x30 si la hay; si no, +4
            // fcomp + test ah,1 (descarta x < min y NaN) y fcomp + test ah,41h + jp (descarta x > max y NaN)
            if (!(pos.x >= minX) || !(pos.x <= maxX)) {
                continue;
            }
            if (!(pos.y >= minY) || !(pos.y <= maxY)) {
                continue;
            }
            if (!(pos.z >= minZ) || !(pos.z <= maxZ)) {
                continue;
            }
            result = static_cast<int32>(pool->GetRef(ped)); // 0x4442D0
            break;
        }
        P(0).iParam = result;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47A498
    case COMMAND_SET_SCRIPT_COOP_GAME: {
        CollectParameters(1);
        CGameLogic::bScriptCoopGameGoingOn = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x47A4B8
    case COMMAND_CREATE_USER_3D_MARKER: {
        CollectParameters(4);
        const float x      = P(0).fParam;
        const float y      = P(1).fParam;
        const float z      = P(2).fParam;
        const auto  colour = static_cast<eHudColours>(P(3).iParam);
        P(0).iParam        = C3dMarkers::User3dMarkerSet(x, y, z, colour); // 0x720FD0
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47A50E
    case COMMAND_REMOVE_USER_3D_MARKER: {
        CollectParameters(1);
        C3dMarkers::User3dMarkerDelete(P(0).iParam); // 0x721090
        return OR_CONTINUE;
    }
    // 0x47A52D
    case COMMAND_GET_RID_OF_PLAYER_PROSTITUTE: {
        CTaskComplexProstituteSolicit::GetRidOfPlayerProstitute(); // 0x661D30
        return OR_CONTINUE;
    }
    // 0x47A539
    case COMMAND_DISPLAY_NON_MINIGAME_HELP_MESSAGES: {
        CollectParameters(1);
        CTheScripts::bDisplayNonMiniGameHelpMessages = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x47A558
    case COMMAND_SET_RAILTRACK_RESISTANCE_MULT: {
        CollectParameters(1);
        const float mult = P(0).fParam;
        // fcom con el double 0.0 (0x859EF8) + test ah,41h + jne: con mult <= 0 (o NaN) copia el valor por defecto sin multiplicar
        if (static_cast<double>(mult) > 0.0) {
            CVehicle::ms_fRailTrackResistance = CVehicle::ms_fRailTrackResistanceDefault * mult; // [0x8D34B0] * mult -> [0x8D34AC]
        } else {
            CVehicle::ms_fRailTrackResistance = CVehicle::ms_fRailTrackResistanceDefault;
        }
        return OR_CONTINUE;
    }
    // 0x47A5A0
    case COMMAND_SWITCH_OBJECT_BRAINS: {
        CollectParameters(2);
        const auto id = static_cast<int8>(P(0).u8Param);
        if (P(1).iParam != 0) {
            CTheScripts::ScriptsForBrains.SwitchAllObjectBrainsWithThisID(id, true); // 0x46A900 con ECX = 0xA90CF0
        } else {
            CTheScripts::ScriptsForBrains.SwitchAllObjectBrainsWithThisID(id, false);
        }
        return OR_CONTINUE;
    }
    // 0x47A5E4
    case COMMAND_FINISH_SETTING_UP_CONVERSATION_NO_SUBTITLES: {
        CConversations::DoneSettingUpConversation(true); // 0x43ADB0
        return OR_CONTINUE;
    }
    // 0x47A5F5
    case COMMAND_ALLOW_PAUSE_IN_WIDESCREEN: {
        CollectParameters(1);
        StaticRef<bool>(0xBA677C) = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x47A615
    case COMMAND_GET_PC_MOUSE_MOVEMENT: {
        static_cast<void>(CPad::GetPad(0)); // 0x53FB70 (el EXE descarta el resultado)
        P(0).fParam = CPad::NewMouseControllerState.m_AmountMoved.x; // [0xB73424]
        static_cast<void>(CPad::GetPad(0));
        P(1).fParam = CPad::NewMouseControllerState.m_AmountMoved.y; // [0xB73428]
        StoreParameters(2);
        return OR_CONTINUE;
    }
    // 0x47A649: byte 0xBA6818 (FrontEndMenuManager.m_ControlMethod != 0)
    case COMMAND_IS_PC_USING_JOYPAD: {
        UpdateCompareFlag(StaticRef<uint8>(0xBA6818) != 0);
        return OR_CONTINUE;
    }
    // 0x47A66C: byte 0xBA6745 (CMenuManager::bInvertMouseY)
    case COMMAND_IS_MOUSE_USING_VERTICAL_INVERSION: {
        UpdateCompareFlag(StaticRef<uint8>(0xBA6745) != 0);
        return OR_CONTINUE;
    }
    // 0x47A68F: IS_XBOX_VERSION y IS_JAPANESE_VERSION (siempre falso en la version de PC)
    case COMMAND_IS_XBOX_VERSION:
    case COMMAND_IS_JAPANESE_VERSION: {
        UpdateCompareFlag(false);
        return OR_CONTINUE;
    }
    // 0x47A69C: DO_DEBUG_STUFF
    case COMMAND_DO_DEBUG_STUFF: {
        CTheScripts::UnknownDebugStuff++; // [0xA95190]
        return OR_CONTINUE;
    }
    // 0x47A6A2 (sin accion, sin leer parametros): SET_UP_SKIP_TO_BE_FINISHED_BY_SCRIPT, SET_RENDER_PLAYER_WEAPON y REMOVE_ALLUSER_3D_MARKERS
    case COMMAND_SET_UP_SKIP_TO_BE_FINISHED_BY_SCRIPT:
    case COMMAND_SET_RENDER_PLAYER_WEAPON:
    case COMMAND_REMOVE_ALLUSER_3D_MARKERS:
        return OR_CONTINUE;
    // 0x47A6A6: fuera de 2600..2638
    default:
        return OR_INTERRUPT;
    }
}
