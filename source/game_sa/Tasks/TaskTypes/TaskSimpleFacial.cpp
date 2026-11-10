/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleFacial
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

#include "TaskSimpleFacial.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
// 0x690C70 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskSimpleFacial del EXE (thiscall, dos dwords en la pila, `ret 8`, devuelve this). Base CTaskSimple (0x61A390), vtable 0x870AB8, +8 y +0xC a cero, bytes +0x10 y +0x11 a cero, +0x14 = primer argumento y +0x18 = segundo.
CTaskSimpleFacial* __fastcall FacialConstructorBody(CTaskSimpleFacial* self, void* /*edx*/, uint32 first, uint32 second) {
    CTaskSimpleBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes) = 0x870AB8;
    *reinterpret_cast<uint32*>(bytes + 8) = 0;
    *reinterpret_cast<uint32*>(bytes + 0xC) = 0;
    bytes[0x10] = 0;
    bytes[0x11] = 0;
    *reinterpret_cast<uint32*>(bytes + 0x18) = second;
    *reinterpret_cast<uint32*>(bytes + 0x14) = first;
    return self;
}

// 0x690CB0 (definida mas abajo; lote 151A, banco Fn151A)
__declspec(noinline) void __fastcall SimpleFacialDestructorBody(CTaskSimpleFacial* self, void* edx);
} // namespace

void CTaskSimpleFacial::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleFacial, 0x870ab8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(ScalarDeletingDestructor, 0x692E30);
    RH_ScopedNamedGlobalInstall(FacialConstructorBody, "Constructor", 0x690C70);
    RH_ScopedNamedGlobalInstall(SimpleFacialDestructorBody, "Destructor", 0x690CB0); // lote 151A (banco Fn151A): el cuerpo ya existia (lote 128A) sin hook propio

    RH_ScopedGlobalInstall(GetAnimId, 0x690CC0);
    RH_ScopedVMTInstall(Clone, 0x692820);
    RH_ScopedVMTInstall(GetTaskType, 0x690CA0);
    RH_ScopedVMTInstall(MakeAbortable, 0x692E50);
    RH_ScopedVMTInstall(ProcessPed, 0x692E80);
}

// 0x690C70
CTaskSimpleFacial::CTaskSimpleFacial(eFacialExpression type, int32 durMs) :
    m_Duration{ durMs },
    m_Type{ type }
{
}

// See Android, looks like **map**
// 0x690CC0
AnimationId CTaskSimpleFacial::GetAnimId(eFacialExpression expression) {
    switch (expression) {
    case eFacialExpression::SURPRISEPANIC: return ANIM_ID_FACSURP;
    case eFacialExpression::SURPRISEMILD: return ANIM_ID_FACSURPM;

    case eFacialExpression::CURIOSITY:
    case eFacialExpression::HAPPINESS: return ANIM_ID_FACURIOS;

    case eFacialExpression::ANGER:
    case eFacialExpression::SADNESS: return ANIM_ID_FACANGER_0;

    case eFacialExpression::TALKING: return ANIM_ID_FACTALK;
    case eFacialExpression::CHEWING: return ANIM_ID_FACGUM;

    default: return ANIM_ID_UNDEFINED;
    }
}

// 0x692E50
bool CTaskSimpleFacial::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), GetAnimId(m_Type))) {
        anim->m_BlendDelta = -4.0f;
    }
    return true;
}

// 0x692E80
bool CTaskSimpleFacial::ProcessPed(CPed* ped) {
    const auto animId = CTaskSimpleFacial::GetAnimId(m_Type);
    const auto anim  = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), animId);

    if (RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_FACTALK)) {
        if (animId == ANIM_ID_FACTALK) {
            // Como el original: (int)((rand() & 0xFFFF) * (1/32768) * 100) < 40
            if (CGeneral::GetRandomIntInRange(0, 100) < 40) {
                anim->SetSpeed(CGeneral::GetRandomNumberInRange(0.5f, 3.0f));
            }
            return false;
        }
        if (anim) {
            anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, true);
            anim->SetBlendDelta(-4.0f);
        }
        return true;
    }

    if (!m_Timer.IsStarted()) {
        if (!anim) {
            CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, animId, 4.0f);
            // Como el original: el temporizador se arranca sin comprobar que la duración sea >= 0
            m_Timer.m_nInterval  = m_Duration;
            m_Timer.m_nStartTime = CTimer::GetTimeInMS();
            m_Timer.m_bStarted   = true;
            return false;
        }
        return true;
    }

    if (!anim) {
        return true;
    }

    if (!m_Timer.IsOutOfTime()) {
        return false;
    }

    anim->m_BlendDelta = -4.0f;

    return true;
}

namespace {
// 0x690CB0 (lote 128A, banco Fn128A; con hook propio desde el lote 151A, banco Fn151A): cuerpo del destructor de CTaskSimpleFacial del EXE (`mov dword [ecx],0x870AB8; jmp CTask::Destructor (0x61A3A0)`, thiscall sin
// argumentos). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama ScalarDeletingDestructor como el `call 0x690CB0` del EXE.
__declspec(noinline) void __fastcall SimpleFacialDestructorBody(CTaskSimpleFacial* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x870AB8;
    self->CTask::Destructor();
}
} // namespace

uintptr_t Lote128ASimpleFacialDestructorBody() { return reinterpret_cast<uintptr_t>(&SimpleFacialDestructorBody); } // para el banco Fn128A

// 0x692E30 (lote 128A, banco Fn128A) : destructor con borrado de CTaskSimpleFacial del EXE (ranura 0 de su vtable 0x870AB8; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x690CB0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x690CB0), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleFacial* CTaskSimpleFacial::ScalarDeletingDestructor(uint8 flags) {
    SimpleFacialDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
