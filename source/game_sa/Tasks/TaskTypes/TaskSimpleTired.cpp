/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleTired
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleTired.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

void CTaskSimpleTired::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleTired, 0x86DED0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x636180); // lote 132A (banco Fn132A)
    RH_ScopedInstall(Constructor, 0x630F20);
    RH_ScopedInstall(Destructor, 0x630F60);
    RH_ScopedInstall(StartAnim, 0x630F70);
}

// 0x630F20
CTaskSimpleTired::CTaskSimpleTired(uint32 tiredDurationMs) :
    CTaskSimple(),
    m_TiredDurationMs{ tiredDurationMs }
{
    m_AnimStartMs = 0;
    m_TiredDurationMs_Copy = 0;
    m_AnimHasStarted = false;
    m_NeedsToStartAnim = false;
    m_TiredAnim = nullptr;
}

// 0x636180 (lote 132A, banco Fn132A): Clone del EXE (thiscall, sin argumentos, `ret`, marco SEH que solo libera la memoria: se omite). Reserva 0x1C bytes con CTask::operator new (0x61A5A0); si devuelve
// nulo, devuelve nulo. Si no, lee la duracion (+8) de esta tarea ANTES de llamar al constructor base de CTaskSimple (0x61A390: +4 = 0, vtable 0x86D4A8) y construye la tarea con la vtable del EXE
// (0x86DED0): +8 = duracion, +0xC = 0, +0x10 = 0, bytes +0x14 y +0x15 = 0 y +0x18 = 0. Devuelve la direccion reservada.
CTask* CTaskSimpleTired::Clone() const {
    auto* const memory = static_cast<uint8*>(CTask::operator new(sizeof(CTaskSimpleTired)));
    if (!memory) {
        return nullptr;
    }
    const uint32 duration = m_TiredDurationMs;
    CTaskSimpleBody::Constructor(memory, nullptr);
    *reinterpret_cast<uint32*>(memory + 0x8) = duration;
    *reinterpret_cast<uint32*>(memory + 0x0) = 0x86DED0;
    *reinterpret_cast<uint32*>(memory + 0xC) = 0;
    *reinterpret_cast<uint32*>(memory + 0x10) = 0;
    *(memory + 0x14) = 0;
    *(memory + 0x15) = 0;
    *reinterpret_cast<uint32*>(memory + 0x18) = 0;
    return reinterpret_cast<CTask*>(memory);
}

// 0x630F70
void CTaskSimpleTired::StartAnim(CPed* ped) {
    ped->SetMoveState(PEDMOVE_STILL);
    ped->SetMoveAnim();
    m_AnimStartMs = CTimer::GetTimeInMS();
    m_TiredDurationMs_Copy = m_TiredDurationMs;
    m_AnimHasStarted = true;
    m_TiredAnim = CAnimManager::BlendAnimation(
        ped->GetRpClump(),
        FindPlayerPed() == ped && CClothes::GetDefaultPlayerMotionGroup() == ANIM_GROUP_FAT ? ANIM_GROUP_FAT_TIRED : ANIM_GROUP_DEFAULT,
        ANIM_ID_IDLE_TIRED,
        4.f
    );
}

// 0x630FF0
bool CTaskSimpleTired::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (m_TiredAnim && m_TiredAnim == RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_IDLE_TIRED)) {
        m_TiredAnim->m_BlendDelta = -4.f;
    }
    return true;
}

// 0x637970
bool CTaskSimpleTired::ProcessPed(CPed* ped) {
    if (!m_TiredAnim) {
        StartAnim(ped);
    }

    if (!m_AnimHasStarted) {
        return false;
    }

    if (m_NeedsToStartAnim) {
        m_AnimStartMs = CTimer::GetTimeInMS();
        m_NeedsToStartAnim = false;
    }

    if (CTimer::GetTimeInMS() < m_AnimStartMs + m_TiredDurationMs_Copy) {
        return false;
    }

    MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    return true;
}
