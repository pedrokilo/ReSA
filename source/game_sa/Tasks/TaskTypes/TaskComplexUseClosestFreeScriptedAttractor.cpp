/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseClosestFreeScriptedAttractor
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexUseClosestFreeScriptedAttractor.h"
#include "TaskComplexUseEffect.h"
#include "TaskComplexUseEffectRunning.h"
#include "TaskComplexUseEffectSprinting.h"
#include "Plugins/TwoDEffectPlugin/2dEffect.h"
#include "Scripts/Scripted2dEffects.h"
#include "PedAttractorManager.h"
#include "InterestingEvents.h"

void CTaskComplexUseClosestFreeScriptedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseClosestFreeScriptedAttractor, 0x86e428, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6346F0);
    RH_ScopedInstall(Destructor, 0x634720);

    RH_ScopedGlobalInstall(ComputeClosestFreeScriptedEffect, 0x634740);

    RH_ScopedVMTInstall(Clone, 0x636F70);
    RH_ScopedVMTInstall(GetTaskType, 0x634710);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x634730);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x639530);
    RH_ScopedVMTInstall(ControlSubTask, 0x634890);
}

CTaskComplexUseClosestFreeScriptedAttractor::CTaskComplexUseClosestFreeScriptedAttractor(eMoveState ms) :
    m_MoveState{ms}
{
}

CTaskComplexUseClosestFreeScriptedAttractor::CTaskComplexUseClosestFreeScriptedAttractor(const CTaskComplexUseClosestFreeScriptedAttractor&) :
    CTaskComplexUseClosestFreeScriptedAttractor{}
{
}

// 0x634740
C2dEffect* __stdcall CTaskComplexUseClosestFreeScriptedAttractor::ComputeClosestFreeScriptedEffect(CPed const& ped) {
    C2dEffect* closest{};
    float      closestDistSq = std::bit_cast<float>(0x7F7FFFFFu); // FLT_MAX
    for (size_t i = 0; i < NUM_SCRIPTED_2D_EFFECTS; i++) {
        if (!CScripted2dEffects::ms_activated.data()[i]) {
            continue;
        }
        // Como el original: con lista de usuarios, el modelo del peatón (con signo) o su tipo (marca -2) deben estar en ella
        const auto& userList = CScripted2dEffects::ms_userLists.data()[i];
        if (userList.m_bUseList) {
            bool allowed{};
            for (size_t k = 0; k < 4; k++) {
                if (static_cast<int32>(static_cast<int16>(ped.m_nModelIndex)) == userList.m_UserTypes[k]) {
                    allowed = true;
                    break;
                }
            }
            if (!allowed) {
                for (size_t k = 0; k < 4; k++) {
                    if (userList.m_UserTypes[k] == -2 && userList.m_UserTypesByPedType[k] == static_cast<int32>(ped.m_nPedType)) {
                        allowed = true;
                        break;
                    }
                }
            }
            if (!allowed) {
                continue;
            }
        }
        // Como el original: distancia al cuadrado (dz² + dy²) + dx², estrictamente menor que la mejor (con NaN no entra)
        auto* const effect = &CScripted2dEffects::ms_effects.data()[i];
        const CVector& pedPos = ped.GetPosition();
        const float dx = pedPos.x - effect->m_Pos.x;
        const float dy = pedPos.y - effect->m_Pos.y;
        const float dz = pedPos.z - effect->m_Pos.z;
        const float distSq = (dz * dz + dy * dy) + dx * dx;
        if (distSq < closestDistSq) {
            // Como el original: se construye una matriz local (identidad con escala 1) que no se usa para nada
            if (GetPedAttractorManager()->HasEmptySlot(reinterpret_cast<C2dEffectPedAttractor*>(effect), nullptr)) {
                closestDistSq = distSq;
                closest       = effect;
            }
        }
    }
    return closest;
}

// 0x639530
CTask* CTaskComplexUseClosestFreeScriptedAttractor::CreateFirstSubTask(CPed* ped) {
    C2dEffect* const fx = ComputeClosestFreeScriptedEffect(*ped);
    if (!fx) {
        return nullptr;
    }
    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_3, ped);
    auto* const attractorFx = reinterpret_cast<C2dEffectPedAttractor*>(fx);
    switch (m_MoveState) {
    case PEDMOVE_RUN:
        return new CTaskComplexUseEffectRunning(attractorFx, nullptr);
    case PEDMOVE_SPRINT:
        return new CTaskComplexUseEffectSprinting(attractorFx, nullptr);
    default: // PEDMOVE_WALK y cualquier otro valor
        return new CTaskComplexUseEffect(attractorFx, nullptr);
    }
}
