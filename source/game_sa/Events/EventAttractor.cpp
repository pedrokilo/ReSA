/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventAttractor
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventAttractor.h"

#include "Ragdoll/IKChainManager.h"

namespace {
// 0x4AF400 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventAttractor que usan los destructores del EXE. Deja la vtable de CEventAttractor del EXE (0x85B1B8), libera la referencia de m_entity (+0x18) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall AttractorDestructorBody(CEventAttractor* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B1B8;
    CEntity::SafeCleanUpRef(self->m_entity);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventAttractor::InjectHooks()
{
    RH_ScopedVirtualClass(CEventAttractor, 0x85B1B8, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(AttractorDestructorBody, "Destructor", 0x4AF400);

    RH_ScopedInstall(Constructor, 0x4AF350);
    RH_ScopedVMTInstall(AffectsPed, 0x4AF4B0);
    RH_ScopedVMTInstall(CloneEditable, 0x4B7440);
    RH_ScopedInstall(IsEffectActive, 0x4AF460);
    RH_ScopedVMTInstall(GetEventType, 0x4AF3D0);     // lote 187A (sin validar): ranura 1
    RH_ScopedVMTInstall(GetEventPriority, 0x4AF3F0); // lote 187A (sin validar): ranura 2
    RH_ScopedVMTInstall(GetLifeTime, 0x4AF3E0);      // lote 187A (sin validar): ranura 3
}

// 0x4AF3D0 (lote 187A, sin validar; thiscall, sin argumentos, `ret`, 6 bytes: `mov eax,1Bh`): EVENT_ATTRACTOR (27); no lee el objeto (ranura 1 de la vtable 0x85B1B8).
eEventType CEventAttractor::GetEventType() const {
    return EVENT_ATTRACTOR;
}

// 0x4AF3F0 (lote 187A, sin validar; thiscall, `ret`, 3 bytes: `xor eax,eax`): prioridad 0 (ranura 2). El EXE comparte esta direccion con CEventScriptedAttractor (vtable 0x86C9F0, ranura 2): el valor devuelto es el mismo.
int32 CEventAttractor::GetEventPriority() const {
    return 0;
}

// 0x4AF3E0 (lote 187A, sin validar; thiscall, `ret`, 3 bytes: `xor eax,eax`): tiempo de vida 0 (ranura 3). El EXE comparte esta direccion con CEventScriptedAttractor (vtable 0x86C9F0, ranura 3): el valor devuelto es el mismo.
int32 CEventAttractor::GetLifeTime() {
    return 0;
}

CEventAttractor::CEventAttractor(C2dEffectPedAttractor* effect, CEntity* entity, bool bAvoidLookingAtAttractor, eTaskType taskType) :
    CEventEditableResponse{taskType}
{
    m_2dEffect = effect;
    m_entity   = entity;
    m_bAvoidLookingAtAttractor = bAvoidLookingAtAttractor;
    CEntity::SafeRegisterRef(m_entity);
}

CEventAttractor::~CEventAttractor() {
    CEntity::SafeCleanUpRef(m_entity);
}

// 0x4AF4B0
bool CEventAttractor::AffectsPed(CPed* ped) {
    if (   ped->IsAlive()
        && (GetEventType() != EVENT_ATTRACTOR || m_entity)
        && (GetEventType() != EVENT_ATTRACTOR || IsEffectActive(m_entity, m_2dEffect))
        && !ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_BE_IN_COUPLE))
    {
        if (ped->m_nPedType != PED_TYPE_COP
            || GetEventType() != EVENT_ATTRACTOR
            || FindPlayerWanted()->GetWantedLevel() == eWantedLevel::WANTED_CLEAN
            && m_2dEffect->m_nAttractorType == PED_ATTRACTOR_TRIGGER_SCRIPT
            && CPopulation::PedMICanBeCreatedAtThisAttractor(ped->GetModelId(), m_2dEffect->m_szScriptName))
        {
            // Como el original: la comprobacion del refugio con lluvia se hace siempre, haya tarea activa o no
            CTask* activeTask = ped->GetTaskManager().GetActiveTask();
            if (activeTask && !m_bAvoidLookingAtAttractor && activeTask->GetTaskType() == TASK_COMPLEX_USE_EFFECT)
                return false;
            if (m_2dEffect->m_nAttractorType == PED_ATTRACTOR_SHELTER && !CWeather::IsRainy())
                return false;
            if (m_bAvoidLookingAtAttractor)
                return true;
            if (CGeneral::GetRandomIntInRange(0, 100) >= m_2dEffect->m_nPedExistingProbability)
                return false;
            if (CGeneral::GetRandomIntInRange(0, 100) >= m_2dEffect->field_36)
                return true;
            if (!g_ikChainMan.IsLooking(ped)) {
                uint32 time = CGeneral::GetRandomIntInRange(2000, 4000); // Como el original: 0x407180
                // Como el original: MultiplyMatrixWithVector (0x59C890) con su orden de sumas
                const CMatrix& m = m_entity->GetMatrix();
                const CVector& v = m_2dEffect->m_Pos;
                CVector point{
                    ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
                    ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
                    ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
                };
                g_ikChainMan.LookAt("CEventAttractor", ped, 0, time, BONE_UNKNOWN, &point, false, 0.25f, 500, 3, false);
            }
        }
    }
    return false;
}

// 0x4B7440
CEventEditableResponse* CEventAttractor::CloneEditable() const noexcept
{
    return new CEventAttractor(m_2dEffect, m_entity, m_bAvoidLookingAtAttractor);
}

// 0x4AF460
bool CEventAttractor::IsEffectActive(CEntity* entity, const C2dEffectPedAttractor* effect) {
    auto mi = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    for (int32 i = 0; i < mi->m_n2dfxCount; i++) {
        if (effect == notsa::dyn_cast_if_present<C2dEffectPedAttractor>(mi->Get2dEffect(i))) {
            return true;
        }
    }
    return false;
}
