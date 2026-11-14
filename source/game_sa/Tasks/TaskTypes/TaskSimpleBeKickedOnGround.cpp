#include "StdInc.h"

#include "TaskSimpleBeKickedOnGround.h"
#include "AnimBlendAssociation.h"

namespace {
// 0x61FC50 (lote 192A, sin validar): cuerpo del destructor de CTaskSimpleBeKickedOnGround del EXE (thiscall, sin argumentos de pila, `ret`, con marco SEH que solo desenrolla y aqui se omite; ranura 0 de la vtable 0x86D7D8). Deja la
// vtable de la clase del EXE (0x86D7D8); si hay animacion (puntero en +0xC) pone su m_BlendDelta (+0x1C) a -4.0f (0xC0800000) y le pone la funcion de fin por defecto (CAnimBlendAssociation::SetFinishCallback 0x4CEBE0 con
// CDefaultAnimCallback::DefaultAnimCB 0x4D1390 y datos nulos); y sigue con el cuerpo de ~CTask (0x61A3A0, con hook). La clase es una plantilla sin campos: el puntero +0xC se lee por desplazamiento.
__declspec(noinline) void __fastcall BeKickedOnGroundDestructorBody(CTaskSimpleBeKickedOnGround* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86D7D8;
    auto* const anim = *reinterpret_cast<CAnimBlendAssociation**>(reinterpret_cast<uint8*>(self) + 0xC);
    if (anim) {
        static_assert(offsetof(CAnimBlendAssociation, m_BlendDelta) == 0x1C);
        anim->m_BlendDelta = -4.0f;
        anim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    self->CTask::Destructor();
}

// 0x625B80 (lote 192A, sin validar; el listado decia CTaskSimpleThrowProjectile: es la ranura 0 de la vtable 0x86D7D8 de CTaskSimpleBeKickedOnGround; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x61FC50;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo (0x61FC50, aqui BeKickedOnGroundDestructorBody), libera `this` con CTask::operator delete (0x61A5B0,
// con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (clase sin reconstruir: sin vtable exportada).
CTaskSimpleBeKickedOnGround* __fastcall BeKickedOnGroundScalarDeletingDestructor(CTaskSimpleBeKickedOnGround* self, void* /*edx*/, uint8 flags) {
    BeKickedOnGroundDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleBeKickedOnGround::InjectHooks() {
    RH_ScopedClass(CTaskSimpleBeKickedOnGround); // clase aun sin reconstruir: sin vtable exportada, solo hooks de funciones sueltas
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(BeKickedOnGroundDestructorBody, "DestructorBody", 0x61FC50); // lote 192A (sin validar)
    RH_ScopedNamedGlobalInstall(BeKickedOnGroundScalarDeletingDestructor, "ScalarDeletingDestructor", 0x625B80); // lote 192A (sin validar)
    // RH_ScopedVMTInstall(GetTaskType, 0x61FC40);
    // TODO: instalar las funciones conforme se reconstruyan.
}
