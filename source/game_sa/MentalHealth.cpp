/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMentalState
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "MentalHealth.h"

#include "Events/EventHealthLow.h"
#include "Events/EventHealthReallyLow.h"
#include "Events/EventHighAngerAtPlayer.h"
#include "Events/EventLowAngerAtPlayer.h"

// Como el original: operacion local; 0x421050 recibe CPedIntelligence*, no este this.
void CMentalState::IncrementAnger(int32 anger) {
    if (!m_AngerTimer.m_bStarted) return;
    const uint32 now = CTimer::GetTimeInMS();
    if (m_AngerTimer.m_bStopped) {
        m_AngerTimer.m_nStartTime = now;
        m_AngerTimer.m_bStopped = false;
    }
    const uint32 expires = m_AngerTimer.m_nStartTime + static_cast<uint32>(m_AngerTimer.m_nInterval);
    if (now < expires) return;
    m_AngerTimer.m_nStartTime = now;
    m_AngerTimer.m_nInterval = 3000;
    m_AngerTimer.m_bStarted = true;
    m_AngerAtPlayer = static_cast<uint8>(m_AngerAtPlayer + static_cast<uint8>(anger));
}

void CMentalState::InjectHooks() {
    RH_ScopedClass(CMentalState);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Process, 0x6008A0);
}

// Como el original: float -> int64 truncado (_ftol2, 0x821B40) y solo se guarda el byte bajo.
static uint8 HealthByte(float value) {
    return static_cast<uint8>(static_cast<int64>(value));
}

// 0x6008A0
void CMentalState::Process(CPed* ped) {
    // Como el original: la salud del peaton se lee siempre; la del vehiculo solo si va en uno y tiene vehiculo
    m_pedHealth = HealthByte(ped->m_fHealth);
    if (ped->bInVehicle && ped->m_pVehicle) {
        m_vehicleHealth = HealthByte(ped->m_pVehicle->m_fHealth);
    }

    // Como el original: el evento se construye en la pila, se anade al grupo del peaton con Add(&evento, false) y se destruye
    const auto AddEvent = [&](auto event) {
        ped->GetIntelligence()->GetEventGroup().Add(&event, false);
    };

    if (!ped->bInVehicle) {
        if (m_oldPedHealth >= 50) {
            if (m_pedHealth < 10) {
                AddEvent(CEventHealthReallyLow{});
            } else if (m_pedHealth < 50) {
                AddEvent(CEventHealthLow{});
            }
        } else if (m_oldPedHealth >= 10 && m_pedHealth < 10) {
            AddEvent(CEventHealthReallyLow{});
        }
    } else {
        // Como el original: se compara con 600 y 300 un byte (0..255): ninguna de estas ramas llega a cumplirse, pero se conservan
        const int32 oldVehicle = m_oldVehicleHealth;
        const int32 vehicle    = m_vehicleHealth;
        if (oldVehicle >= 600) {
            if (vehicle < 300) {
                AddEvent(CEventHealthReallyLow{});
            } else if (vehicle < 600) {
                AddEvent(CEventHealthLow{});
            }
        } else if (oldVehicle >= 300 && vehicle < 300) {
            AddEvent(CEventHealthReallyLow{});
        }
    }

    if (m_LastAngerAtPlayer <= 3) {
        if (m_AngerAtPlayer > 6) {
            AddEvent(CEventHighAngerAtPlayer{});
        } else if (m_AngerAtPlayer > 3) {
            AddEvent(CEventLowAngerAtPlayer{});
        }
    } else if (m_LastAngerAtPlayer <= 6 && m_AngerAtPlayer > 6) {
        AddEvent(CEventHighAngerAtPlayer{});
    }

    m_LastAngerAtPlayer = m_AngerAtPlayer;
    m_oldPedHealth      = m_pedHealth;
    m_oldVehicleHealth  = m_vehicleHealth;
}
