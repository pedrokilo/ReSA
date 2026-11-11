/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseClosestFreeScriptedAttractor
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
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

namespace {
// 0x634720 (lote 157A, banco Fn157A): cuerpo del destructor de CTaskComplexUseClosestFreeScriptedAttractor del EXE (con hook (Destructor)). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x634720` del EXE.
__declspec(noinline) void __fastcall CTaskComplexUseClosestFreeScriptedAttractorDestructorBody(CTaskComplexUseClosestFreeScriptedAttractor* self, void* /*edx*/) { self->CTaskComplexUseClosestFreeScriptedAttractor::~CTaskComplexUseClosestFreeScriptedAttractor(); }

// 0x639510 (lote 157A, banco Fn157A): destructor con borrado de CTaskComplexUseClosestFreeScriptedAttractor del EXE (ranura 0 de su vtable 0x86E428; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x634720; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexUseClosestFreeScriptedAttractor* __fastcall CTaskComplexUseClosestFreeScriptedAttractorScalarDeletingDestructor(CTaskComplexUseClosestFreeScriptedAttractor* self, void* /*edx*/, uint8 flags) {
    CTaskComplexUseClosestFreeScriptedAttractorDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskComplexUseClosestFreeScriptedAttractor() { return reinterpret_cast<uintptr_t>(&CTaskComplexUseClosestFreeScriptedAttractorDestructorBody); } // para el banco Fn157A

void CTaskComplexUseClosestFreeScriptedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseClosestFreeScriptedAttractor, 0x86e428, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexUseClosestFreeScriptedAttractorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x639510); // lote 157A (banco Fn157A)

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
