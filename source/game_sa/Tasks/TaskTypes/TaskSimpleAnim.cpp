/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleAnim
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleAnim.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
// 0x61A6C0 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskSimpleAnim del EXE (thiscall, un dword en la pila, `ret 4`, devuelve this). Base CTaskSimple (0x61A390), el bit 2 del byte +0xC toma el bit 0 del argumento (los bits 0..2 se borran y el resto se conserva), vtable 0x86D504 y +8 = 0.
CTaskSimpleAnim* __fastcall SimpleAnimConstructorBody(CTaskSimpleAnim* self, void* /*edx*/, uint32 holdLastFrame) {
    CTaskSimpleBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    bytes[0xC] = static_cast<uint8>(((holdLastFrame & 1) << 2) | (bytes[0xC] & 0xF8));
    *reinterpret_cast<uint32*>(bytes) = 0x86D504;
    *reinterpret_cast<uint32*>(bytes + 8) = 0;
    return self;
}
} // namespace

namespace {
// 0x61A6F0 (lotes 150A y 169A, bancos Fn150A y Fn169A): cuerpo del destructor de CTaskSimpleAnim del EXE (thiscall, sin argumentos de pila, `ret`, 159 bytes con marco SEH que solo desenrolla; vtable 0x86D504).
// Como el original: deja la vtable de la clase del EXE (0x86D504); si hay animacion (+8) le pone el callback de fin por defecto (CAnimBlendAssociation::SetFinishCallback 0x4CEBE0, con hook, con el DefaultAnimCB
// del EXE 0x4D1390 y dato nulo), activa su bit 0x4 (palabra +0x2E), y si la tarea no mantiene el ultimo fotograma (bit 2 del byte +0xC) y el peso (+0x18) es mayor que cero y la velocidad de mezcla (+0x1C) mayor
// o igual que cero (comparaciones ordenadas: con NaN no entra) deja la velocidad en -4.0 (0xC0800000); despues pone la animacion a nulo y termina con CTask::Destructor (0x61A3A0, con hook). El destructor
// generado por el compilador dejaria la vtable de la DLL. Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x61A6F0` del EXE. El marco SEH
// se omite.
__declspec(noinline) void __fastcall CTaskSimpleAnimDestructorBody(CTaskSimpleAnim* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86D504;
    if (self->m_pAnim) {
        self->m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        self->m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        if (!self->m_bHoldLastFrame && self->m_pAnim->m_BlendAmount > 0.0f && self->m_pAnim->m_BlendDelta >= 0.0f) {
            self->m_pAnim->m_BlendDelta = -4.0f;
        }
        self->m_pAnim = nullptr;
    }
    self->CTask::Destructor();
}

// 0x61BA80 (lote 150A, banco Fn150A): destructor con borrado de CTaskSimpleAnim del EXE (ranura 0 de su vtable 0x86D504; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x61A6F0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleAnim* __fastcall CTaskSimpleAnimScalarDeletingDestructor(CTaskSimpleAnim* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleAnimDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote150ABody_CTaskSimpleAnim() { return reinterpret_cast<uintptr_t>(&CTaskSimpleAnimDestructorBody); } // para el banco Fn150A

// 0x61A6F0 (lote 151A): la misma funcion propia, visible para otras clases (CTaskSimpleLookAbout::DestructorBody la llama como el `jmp 0x61A6F0` del EXE)
void __fastcall CTaskSimpleAnim::DestructorBodyExe(CTaskSimpleAnim* self, void* edx) { CTaskSimpleAnimDestructorBody(self, edx); }

void CTaskSimpleAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleAnim, 0x86D504, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleAnimScalarDeletingDestructor, "ScalarDeletingDestructor", 0x61BA80); // lote 150A (banco Fn150A)
    RH_ScopedNamedGlobalInstall(CTaskSimpleAnimDestructorBody, "DestructorBody", 0x61A6F0);                     // lote 169A (banco Fn169A)
    RH_ScopedNamedGlobalInstall(SimpleAnimConstructorBody, "Constructor", 0x61A6C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x61A790);
}

// 0x61A6C0
CTaskSimpleAnim::CTaskSimpleAnim(bool bHoldLastFrame) : CTaskSimple() {
    m_bIsFinished = false;
    m_bDontInterrupt = false;
    m_bHoldLastFrame = bHoldLastFrame;
    m_pAnim = nullptr;
}

// 0x61A6F0
CTaskSimpleAnim::~CTaskSimpleAnim() {
    if (!m_pAnim)
        return;

    m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
    if (!m_bHoldLastFrame &&
        m_pAnim->m_BlendAmount > 0.0f &&
        m_pAnim->m_BlendDelta >= 0.0f
    ) {
        m_pAnim->m_BlendDelta = -4.0f;
    }
    m_pAnim = nullptr;
}

// 0x61A790
bool CTaskSimpleAnim::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    auto fBlend = -4.0F;
    if (priority == eAbortPriority::ABORT_PRIORITY_IMMEDIATE) {
        fBlend = -1000.0F;
        m_bHoldLastFrame = false;
    } else if (m_bDontInterrupt) {
        if (event && event->GetEventType() != eEventType::EVENT_SCRIPT_COMMAND)
            return false;
    }

    // Como el original: la comprobación del comando de script (tarea TASK_SIMPLE_NAMED_ANIM) se hace en todos los
    // casos que llegan aquí, también con prioridad IMMEDIATE y con m_bDontInterrupt (0x61A7DB), no solo en el último else
    bool bSkipBlend = false;
    if (event && event->GetEventType() == eEventType::EVENT_SCRIPT_COMMAND) {
        const auto* scriptCommand = static_cast<const CEventScriptCommand*>(event);
        if (scriptCommand->m_task && scriptCommand->m_task->GetTaskType() == TASK_SIMPLE_NAMED_ANIM) {
            if (m_pAnim)
                m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;

            bSkipBlend = true;
        }
    }

    if (!bSkipBlend) {
        if (m_pAnim) {
            m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
            if (!m_bHoldLastFrame) {
                if (m_pAnim->m_Flags & ANIMATION_IS_PARTIAL)
                    m_pAnim->m_BlendDelta = fBlend;
                else
                    CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, -fBlend);
            }
        }
    }

    if (priority == ABORT_PRIORITY_IMMEDIATE || priority == ABORT_PRIORITY_URGENT || m_bHoldLastFrame) {
        if (m_pAnim) {
            m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_bIsFinished = true;
            m_pAnim = nullptr;
        }
        return true;
    }

    return false;
}

// data is CTaskSimpleAnim
// 0x61A8A0
void CTaskSimpleAnim::FinishRunAnimCB(CAnimBlendAssociation* blendAssoc, void* data) {
    //auto* const self = notsa::cast<CTaskSimpleAnim>(static_cast<CTask*>(data));
    //
    //self->m_bIsFinished = true;
    //self->m_pAnim = nullptr;

    static_cast<CTaskSimpleAnim*>(data)->m_bIsFinished = true;
    static_cast<CTaskSimpleAnim*>(data)->m_pAnim = nullptr;
}
