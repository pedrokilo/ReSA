#include "StdInc.h"

#include "TaskSimpleBeHitWhileMoving.h"
#include "AnimBlendAssociation.h"

namespace {
// Callback por defecto de animaciones del EXE (CDefaultAnimCallback::DefaultAnimCB, 0x4D1390: `ret`). Como el original, el destructor pasa la direccion del EXE a SetFinishCallback.
using AnimFinishCallback = void (*)(CAnimBlendAssociation*, void*);
const AnimFinishCallback kExeDefaultAnimCB = reinterpret_cast<AnimFinishCallback>(0x4D1390);

// 0x61FE70 (lote 193A, sin validar): cuerpo del destructor de CTaskSimpleBeHitWhileMoving del EXE (thiscall, sin argumentos de pila, `ret`, con marco SEH que solo desenrolla y aqui se omite; ranura 0 de la vtable 0x86D7FC).
// Deja la vtable de la clase del EXE (0x86D7FC); si hay animacion (puntero en +0x14) pone su m_BlendDelta (+0x1C) a -4.0f (0xC0800000) y le pone la funcion de fin por defecto del EXE (CAnimBlendAssociation::SetFinishCallback
// 0x4CEBE0 con 0x4D1390 y datos nulos); y sigue con el cuerpo de ~CTask (0x61A3A0, con hook). La clase es una plantilla sin campos: el puntero +0x14 se lee por desplazamiento.
__declspec(noinline) void __fastcall BeHitWhileMovingDestructorBody(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86D7FC;
    auto* const anim = *reinterpret_cast<CAnimBlendAssociation**>(reinterpret_cast<uint8*>(self) + 0x14);
    if (anim) {
        static_assert(offsetof(CAnimBlendAssociation, m_BlendDelta) == 0x1C);
        anim->m_BlendDelta = -4.0f;
        anim->SetFinishCallback(kExeDefaultAnimCB, nullptr);
    }
    self->CTask::Destructor();
}

// 0x625BA0 (lote 193A, sin validar; el listado decia CTaskSimpleThrowProjectile: es la ranura 0 de la vtable 0x86D7FC de CTaskSimpleBeHitWhileMoving; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x61FE70; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo (0x61FE70, aqui BeHitWhileMovingDestructorBody), libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (clase sin reconstruir: sin vtable exportada).
CTaskSimpleBeHitWhileMoving* __fastcall BeHitWhileMovingScalarDeletingDestructor(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/, uint8 flags) {
    BeHitWhileMovingDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleBeHitWhileMoving::InjectHooks() {
    RH_ScopedClass(CTaskSimpleBeHitWhileMoving); // clase aun sin reconstruir: sin vtable exportada, solo hooks de funciones sueltas
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(BeHitWhileMovingDestructorBody, "DestructorBody", 0x61FE70); // lote 193A (sin validar)
    RH_ScopedNamedGlobalInstall(BeHitWhileMovingScalarDeletingDestructor, "ScalarDeletingDestructor", 0x625BA0); // lote 193A (sin validar)
    // RH_ScopedVMTInstall(GetTaskType, 0x61FE60);
    // TODO: instalar las funciones conforme se reconstruyan.
}
