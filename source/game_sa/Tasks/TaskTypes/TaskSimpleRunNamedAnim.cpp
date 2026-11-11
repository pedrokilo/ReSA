/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleRunNamedAnim
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

#include "TaskSimpleRunNamedAnim.h"
#include "AnimManager.h"
#include "AnimBlendAssociation.h"
#include "EventGroup.h"

namespace {
// 0x61BF10 (lote 150A, banco Fn150A): cuerpo del destructor de CTaskSimpleRunNamedAnim del EXE (con hook propio (Destructor); vtable 0x86D54C). Funcion propia sin expansion en linea para que el banco pueda sustituirla;
// la llama el destructor con borrado como el `call 0x61BF10` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleRunNamedAnimDestructorBody(CTaskSimpleRunNamedAnim* self, void* /*edx*/) { self->CTaskSimpleRunNamedAnim::~CTaskSimpleRunNamedAnim(); }

// 0x61BEF0 (lote 150A, banco Fn150A): destructor con borrado de CTaskSimpleRunNamedAnim del EXE (ranura 0 de su vtable 0x86D54C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x61BF10; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleRunNamedAnim* __fastcall CTaskSimpleRunNamedAnimScalarDeletingDestructor(CTaskSimpleRunNamedAnim* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleRunNamedAnimDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

// 0x667460 (lote 160A, banco Fn160A; thiscall, `ret`, 37 bytes; el listado decia CTaskComplexProstituteSolicit; sin llamadores ni referencias en el EXE; vecina de las comparaciones de nombre de animacion
// 0x6673B0 y 0x667440 y del constructor 0x6674B0): compara el primer byte de la cadena de ECX con el de la cadena vacia del EXE (0x858B54, un solo byte, `repe cmpsb` con ECX = 1) y devuelve AL = 1 si son
// iguales (cadena vacia) y 0 si no. La clase propietaria no se pudo identificar: se deja como funcion global enganchada de forma nombrada.
__declspec(noinline) bool __fastcall AnimNameIsEmpty(const char* name, void* /*edx*/) {
    return *name == 0;
}
} // namespace

uintptr_t Lote150ABody_CTaskSimpleRunNamedAnim() { return reinterpret_cast<uintptr_t>(&CTaskSimpleRunNamedAnimDestructorBody); } // para el banco Fn150A

void CTaskSimpleRunNamedAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleRunNamedAnim, 0x86D54C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(AnimNameIsEmpty, "AnimNameIsEmpty", 0x667460); // lote 160A (banco Fn160A)
    RH_ScopedNamedGlobalInstall(CTaskSimpleRunNamedAnimScalarDeletingDestructor, "ScalarDeletingDestructor", 0x61BEF0); // lote 150A (banco Fn150A)

    RH_ScopedOverloadedInstall(Constructor, "Default", 0x6674B0, CTaskSimpleRunNamedAnim*(CTaskSimpleRunNamedAnim::*)());
    RH_ScopedOverloadedInstall(Constructor, "Anim", 0x61A990, CTaskSimpleRunNamedAnim*(CTaskSimpleRunNamedAnim::*)(char const*, char const*, int32, float, int32, bool, bool, bool, bool));
    RH_ScopedInstall(Destructor, 0x61BF10);

    RH_ScopedVMTInstall(Clone, 0x61B770);
    RH_ScopedVMTInstall(GetTaskType, 0x61AA90);
    RH_ScopedVMTInstall(ProcessPed, 0x61BF20);
    RH_ScopedInstall(OffsetPedPosition, 0x61AB00);
}

// 0x6674B0
CTaskSimpleRunNamedAnim::CTaskSimpleRunNamedAnim() :
    CTaskSimpleAnim{ false }
{
    // Rest done in header
}

// 0x61A990
CTaskSimpleRunNamedAnim::CTaskSimpleRunNamedAnim(
    const char* animName,
    const char* animGroupName,
    uint32 animFlags,
    float blendDelta,
    uint32 endTime,
    bool bDontInterrupt,
    bool bRunInSequence,
    bool bOffsetPed,
    bool bHoldLastFrame
) :
    CTaskSimpleAnim(bHoldLastFrame),
    m_pAnimHierarchy{},
    m_Time{ endTime },
    m_vecOffsetAtEnd{},
    m_animFlags{ animFlags }
{
    m_bDontInterrupt = bDontInterrupt;
    m_bRunInSequence = bRunInSequence;
    m_bOffsetAtEnd = bOffsetPed;
    m_bOffsetAvailable = false;
    m_fBlendDelta = blendDelta;
    // Como el original: copia byte a byte hasta el nulo, sin relleno ni comprobación de tamaño
    strcpy(m_animName, animName);
    strcpy(m_animGroupName, animGroupName);
    if (const auto block = CAnimManager::GetAnimationBlock(m_animGroupName)) {
        m_pAnimHierarchy = CAnimManager::GetAnimation(m_animName, block);
    }
}

// 0x61BF20
bool CTaskSimpleRunNamedAnim::ProcessPed(CPed* ped) {
    if (m_bOffsetAvailable) {
        OffsetPedPosition(ped);
    }

    if (!m_bIsFinished) {
        // Como el original: sin jerarquía (o sin secuencias) no hay nada que reproducir y la tarea termina
        if (!m_pAnimHierarchy || !m_pAnimHierarchy->m_pSequences) {
            return true;
        }
        // Como el original: con m_bDontInterrupt se purgan los eventos no válidos (salvo animación fija sin bucle)
        if (m_bDontInterrupt) {
            if (!(m_animFlags & ANIMATION_IS_FINISH_AUTO_REMOVE) || (m_animFlags & ANIMATION_IS_LOOPED)) {
                ped->GetEventGroup().RemoveInvalidEvents(true);
                ped->GetEventGroup().Reorganise();
            }
        }
        if (m_Timer.m_bStarted && m_Timer.IsOutOfTime()) {
            if (m_animFlags & ANIMATION_IS_FINISH_AUTO_REMOVE) {
                MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
            } else {
                if (m_pAnim) {
                    m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
                }
                m_bIsFinished = true;
                m_pAnim       = nullptr;
                return true;
            }
        }
        if (!m_pAnim) {
            // Como el original: si ya hay una asociación de esta jerarquía con datos de callback, la tarea termina
            if (const auto* const assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), true, m_pAnimHierarchy)) {
                if (assoc->m_pCallbackData) {
                    return true;
                }
            }
            StartAnim(ped);
        }
        return m_bIsFinished;
    }

    // Como el original: con el desplazamiento final pendiente de aplicar no se hace nada más
    if (m_bOffsetAtEnd) {
        return true;
    }
    const auto assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), true, m_pAnimHierarchy);
    CVector bonePos{};
    ped->GetBonePosition(&bonePos, BONE_SPINE1, false);
    if (m_fBlendDelta > 100.0f && assoc) {
        // Como el original: si el peatón está a más de 1 del hueso se fuerza el fundido de salida de la asociación
        const CVector& pos = ped->GetPosition();
        const float dx = pos.x - bonePos.x;
        const float dy = pos.y - bonePos.y;
        const float dz = pos.z - bonePos.z;
        const float dist = std::sqrt((dz * dz + dy * dy) + dx * dx);
        if (dist > 1.0f) {
            assoc->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
            assoc->m_BlendDelta = m_fBlendDelta * -1.0f;
        }
    }
    return true;
}

// 0x61BB10
void CTaskSimpleRunNamedAnim::StartAnim(CPed* ped) {
    // Como el original: el temporizador solo arranca con un tiempo final con signo no negativo
    if (static_cast<int32>(m_Time) >= 0) {
        m_Timer.m_nStartTime = CTimer::m_snTimeInMilliseconds;
        m_Timer.m_nInterval  = static_cast<int32>(m_Time);
        m_Timer.m_bStarted   = true;
    }
    if (!(m_animFlags & ANIMATION_IS_FINISH_AUTO_REMOVE)) {
        m_animFlags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
    }
    m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_pAnimHierarchy, static_cast<int32>(m_animFlags), m_fBlendDelta);
    if (m_bOffsetAtEnd) {
        m_pAnim->SetFinishCallback(FinishRunAnimMovePedCB, this);
    } else if (!(m_animFlags & ANIMATION_IS_FINISH_AUTO_REMOVE) && !m_bRunInSequence) {
        m_pAnim->SetDeleteCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
    } else {
        m_pAnim->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
    }
    m_nAnimId = static_cast<int16>(m_pAnim->GetAnimId());
}

// 0x61AAA0
void CTaskSimpleRunNamedAnim::FinishRunAnimMovePedCB(CAnimBlendAssociation* blendAssoc, void* data) {
    auto* const task = static_cast<CTaskSimpleRunNamedAnim*>(data);
    // Como el original: se guarda en la tarea el desplazamiento de la raíz del primer nodo
    blendAssoc->GetNode(0)->GetCurrentTranslation(task->m_vecOffsetAtEnd, 0.0f);
    task->m_bOffsetAvailable = true;
    blendAssoc->m_Flags |= ANIMATION_IGNORE_ROOT_TRANSLATION;
    if (!(blendAssoc->m_Flags & ANIMATION_IS_FINISH_AUTO_REMOVE) && blendAssoc->m_nCallbackType != ANIM_BLEND_CALLBACK_DELETE) {
        blendAssoc->SetDeleteCallback(CTaskSimpleAnim::FinishRunAnimCB, task);
        return;
    }
    task->m_bIsFinished = true;
    task->m_pAnim       = nullptr;
}

// 0x61AB00
void CTaskSimpleRunNamedAnim::OffsetPedPosition(CPed* ped) {
    ped->UpdateRpHAnim();
    ped->m_bDontUpdateHierarchy = true;
    auto& pos = ped->GetPosition();
    // Como el original: Multiply3x3 (0x59C790) con el orden de sumas del EXE
    const CMatrix& mat = *ped->m_matrix;
    const CVector& v = m_vecOffsetAtEnd;
    const CVector off{
        mat.GetUp().x * v.z + mat.GetForward().x * v.y + mat.GetRight().x * v.x,
        mat.GetUp().y * v.z + mat.GetRight().y * v.x + mat.GetForward().y * v.y,
        mat.GetUp().z * v.z + mat.GetRight().z * v.x + mat.GetForward().z * v.y,
    };
    pos.x = off.x + pos.x;
    pos.y = off.y + pos.y;
    pos.z = off.z + pos.z;
    m_bOffsetAvailable = false;
}
