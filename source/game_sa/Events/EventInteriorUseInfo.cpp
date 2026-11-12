/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventInteriorUseInfo
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventInteriorUseInfo.h"


namespace CEventBody {
void __fastcall EventDestructorBody(CEvent* self, void* edx); // Event.cpp: 0x4AC010
}

namespace {
// 0x674ED0 (lote 184A, sin validar; el listado decia CTaskComplexWanderCop): cuerpo no virtual de ~CEventInteriorUseInfo del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],8701A8h; jmp 0x4AC010`).
// Deja la vtable de la clase del EXE (0x8701A8) y sigue con el cuerpo de ~CEvent (0x4AC010, con hook). La clase no libera referencias. Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call 0x674ED0` del EXE.
__declspec(noinline) void __fastcall InteriorUseInfoDestructorBody(CEventInteriorUseInfo* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x8701A8;
    CEventBody::EventDestructorBody(self, nullptr);
}

// 0x674F90 (lote 184A, sin validar): destructor con borrado de CEventInteriorUseInfo del EXE (ranura 0 de su vtable 0x8701A8; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x674ED0;
// test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x674ED0), libera `this` con CEvent::operator delete (0x4B5630, con hook)
// solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventInteriorUseInfo* __fastcall InteriorUseInfoScalarDeletingDestructor(CEventInteriorUseInfo* self, void* /*edx*/, uint8 flags) {
    InteriorUseInfoDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventInteriorUseInfo::InjectHooks() {
    RH_ScopedVirtualClass(CEventInteriorUseInfo, 0x8701A8, 16);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(InteriorUseInfoDestructorBody, "DestructorBody", 0x674ED0); // lote 184A (sin validar)
    RH_ScopedNamedGlobalInstall(InteriorUseInfoScalarDeletingDestructor, "ScalarDeletingDestructor", 0x674F90); // lote 184A (sin validar)
    RH_ScopedInstall(Constructor, 0x674EA0); // lote 184A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x674F00);    // lote 184A (sin validar): ranura 4
    RH_ScopedVMTInstall(IsValid, 0x4B1640); // lote 153A (banco Fn153A): ranura 14 de la vtable 0x8701A8
}

CEventInteriorUseInfo::CEventInteriorUseInfo(InteriorInfo_t* interiorInfo, Interior_c* interior, uint32 animTime, int8 loopAction) : CEvent() {
    m_InteriorInfo = interiorInfo;
    m_nLoopAction = loopAction;
    m_Interior = interior;
    m_ActionAnimTime = animTime;
}

// 0x4B1640 (lote 153A, banco Fn153A; thiscall, un puntero en la pila, `ret 4`, 54 bytes): con ped no nulo devuelve CPed::IsAlive (0x5E0170, con hook) con ECX = ped; sin ped, como CEvent::IsValid en linea:
// verdadero si el byte +8 (m_bValid) no es cero; si no, compara m_nTimeActive (+4) con GetLifeTime() (ranura 3, llamada virtual) y es verdadero si no lo supera (jle, con signo).
bool CEventInteriorUseInfo::IsValid(CPed* ped) {
    if (ped)
        return ped->IsAlive();
    return CEvent::IsValid(ped);
}

// 0x674EA0 (lote 184A, sin validar; el listado decia CTaskComplexWanderCop): constructor del EXE (thiscall, cuatro argumentos de pila, `ret 10h`, 48 bytes). Llama al constructor de CEvent (0x4ABFC0), guarda la informacion del
// interior (+0xC), el BYTE bajo del cuarto argumento (+0x18), deja la vtable de la clase del EXE (0x8701A8), el interior (+0x10) y el tiempo de animacion (+0x14); devuelve `this`.
CEventInteriorUseInfo* CEventInteriorUseInfo::Constructor(InteriorInfo_t* interiorInfo, Interior_c* interior, uint32 animTime, int8 loopAction) {
    this->CEventInteriorUseInfo::CEventInteriorUseInfo(interiorInfo, interior, animTime, loopAction);
    return this;
}

// 0x674F00 (lote 184A, sin validar; thiscall, sin argumentos, `ret`, 122 bytes con marco SEH que solo desenrolla la reserva; ranura 4 de la vtable 0x8701A8). Como el original: CEvent::operator new (0x4B5620, con hook) con 0x1C bytes;
// sin memoria devuelve nulo; si no, construye (el EXE lleva el constructor expandido: CEvent 0x4ABFC0, campos +0xC, +0x10, +0x14 y BYTE +0x18 copiados de este evento) y devuelve la memoria.
CEvent* CEventInteriorUseInfo::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventInteriorUseInfo));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventInteriorUseInfo*>(memory)->Constructor(m_InteriorInfo, m_Interior, m_ActionAnimTime, m_nLoopAction);
}

bool CEventInteriorUseInfo::AffectsPed(CPed* ped) {
    return true;
}

