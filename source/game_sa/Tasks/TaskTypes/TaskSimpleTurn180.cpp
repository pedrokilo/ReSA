/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleTurn180
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleTurn180.h"

namespace {
// 0x631D00 (lote 157A, banco Fn157A): cuerpo del destructor de CTaskSimpleTurn180 del EXE (con hook (Destructor)). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x631D00` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleTurn180DestructorBody(CTaskSimpleTurn180* self, void* /*edx*/) { self->CTaskSimpleTurn180::~CTaskSimpleTurn180(); }

// 0x637FC0 (lote 157A, banco Fn157A): destructor con borrado de CTaskSimpleTurn180 del EXE (ranura 0 de su vtable 0x86E01C; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x631D00; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleTurn180* __fastcall CTaskSimpleTurn180ScalarDeletingDestructor(CTaskSimpleTurn180* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleTurn180DestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskSimpleTurn180() { return reinterpret_cast<uintptr_t>(&CTaskSimpleTurn180DestructorBody); } // para el banco Fn157A

void CTaskSimpleTurn180::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleTurn180, 0x86e01c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleTurn180ScalarDeletingDestructor, "ScalarDeletingDestructor", 0x637FC0); // lote 157A (banco Fn157A)

    RH_ScopedInstall(Constructor, 0x631CC0);
    RH_ScopedInstall(Destructor, 0x631D00);

    RH_ScopedVMTInstall(Clone, 0x636570);
    RH_ScopedVMTInstall(GetTaskType, 0x631CE0);
    RH_ScopedVMTInstall(MakeAbortable, 0x631CF0);
    RH_ScopedVMTInstall(ProcessPed, 0x639990);
}

// NOTSA
// 0x636570 - Como el original: sin inicialización por valor (delegar en `CTaskSimpleTurn180{}` ponía a cero el objeto
// entero, relleno incluido); solo se escriben el byte +8 y el puntero +0xC
CTaskSimpleTurn180::CTaskSimpleTurn180(const CTaskSimpleTurn180& o) {
}

// 0x631D00
CTaskSimpleTurn180::~CTaskSimpleTurn180() {
    if (m_anim) {
        m_anim->SetDefaultFinishCallback();
    }
}

void CTaskSimpleTurn180::FinishAnimTurn180CB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleTurn180*>(data);
    self->m_animHasFinished = true;
    self->m_anim = nullptr;
}

// 0x639990
bool CTaskSimpleTurn180::ProcessPed(CPed* ped) {
    if (m_animHasFinished) {
        ped->RestoreHeadingRate();
        return true;
    }

    if (!m_anim) {
        ped->m_fHeadingChangeRate = 0.f;
        m_anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_TURN_180, 4.f);
        m_anim->SetFinishCallback(FinishAnimTurn180CB, this);
    }

    return false;
}
