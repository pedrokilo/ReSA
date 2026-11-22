#include "StdInc.h"

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"

// Rama vivo-scra (sin validar): despachador original de los comandos de script 200..299 (ranura 2 de s_OriginalCommandHandlerTable, 0x469390). thiscall con el numero de comando en la pila (`ret 4`)
// y AL = 0 (OR_CONTINUE) o 0xFF (OR_INTERRUPT: comando sin caso, destino 0x469DBD). Indice = comando - 214 (0xD6) en la tabla de bytes 0x469E4C (84 entradas, 214..297) y de saltos 0x469DD4; los
// comandos 200..213 y 298..299 no tienen caso. El marco SEH del prologo (manejador 0x837DD9) solo protege los `new` de casos que aqui no estan; no hace falta en C++. Como en
// RunningScriptCommands300To499.cpp, solo llevan caso los comandos que el EXE resuelve aqui y que NO tienen manejador propio en el C++ (docs/ComandosScript.csv, estado original o no_implementado con
// caso en la tabla de saltos); el resto (incluido 268 GIVE_REMOTE_CONTROLLED_CAR_TO_PLAYER, `nop` con caso vacio 0x469DB9) va a ProcessHandledCommand.
namespace {
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
} // namespace

static_assert(offsetof(CRunningScript, m_UsesMissionCleanup) == 0xC6);
static_assert(offsetof(CRunningScript, m_IsDeathArrestCheckEnabled) == 0xD3 && offsetof(CRunningScript, m_DoneDeathArrest) == 0xD4);
static_assert(STATUS_WRECKED == 5);

// 0x469390 (rama vivo-scra, sin validar): ver el comentario de cabecera.
OpcodeResult CRunningScript::ProcessCommands200To299(int32 command) {
    switch (command) {
    case COMMAND_MISSION_HAS_FINISHED: // 216, 0x46945E: sin parametros
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.Process(); // 0x468560
        }
        return OR_CONTINUE;
    case COMMAND_GET_PAD_STATE: { // 226, 0x469715: los dos parametros como WORD (movzx) y el resultado int16 extendido con signo (movsx)
        CollectParameters(2);
        P(0).iParam = static_cast<int32>(GetPadState(P(0).u16Param, static_cast<eButtonId>(P(1).u16Param))); // 0x485B10
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_SET_DEATHARREST_STATE: { // 273, 0x469A7B: `cmp [param], 1; sete`
        CollectParameters(1);
        m_IsDeathArrestCheckEnabled = P(0).iParam == 1;
        return OR_CONTINUE;
    }
    case COMMAND_HAS_DEATHARREST_BEEN_EXECUTED: // 274, 0x469A9B: sin parametros
        UpdateCompareFlag(m_DoneDeathArrest != 0); // 0x4859D0
        return OR_CONTINUE;
    case COMMAND_ADD_AMMO_TO_CHAR: { // 276, 0x469AA8: sin comprobar el puntero
        CollectParameters(3);
        PedOf(P(0).iParam)->GrantAmmo(static_cast<eWeaponType>(P(1).iParam), P(2).uParam); // 0x5DF220
        return OR_CONTINUE;
    }
    case COMMAND_IS_CAR_DEAD: { // 281, 0x469B6D: muerto si no existe o esta destrozado (byte +0x36 & 0xF8 == 0x28); si no, bIsDrowning (bit 6 del byte +0x42B)
        CollectParameters(1);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        if (vehicle && vehicle->GetStatus() != STATUS_WRECKED) {
            UpdateCompareFlag(vehicle->vehicleFlags.bIsDrowning);
            return OR_CONTINUE;
        }
        UpdateCompareFlag(true);
        return OR_CONTINUE;
    }
    default:
        return ProcessHandledCommand(command);
    }
}
