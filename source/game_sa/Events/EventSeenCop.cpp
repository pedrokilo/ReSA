/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventSeenCop
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventSeenCop.h"

namespace CEventAcquaintancePedBody {
void __fastcall AcquaintancePedDestructorBody(CEventAcquaintancePed* self, void* edx); // EventAcquaintancePed.cpp: 0x4AF890
}

namespace {
// 0x5FF3A0 (lote 196A, sin validar): cuerpo del destructor de CEventSeenCop del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86CB58h; jmp 0x4AF890`). Deja la vtable de la clase del EXE (0x86CB58)
// y sigue con el cuerpo de ~CEventAcquaintancePed (0x4AF890, con hook). Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call 0x5FF3A0` del EXE (0x5FF423) y la alcanza por salto
// el desenrollado SEH de 0x83EA23.
__declspec(noinline) void __fastcall SeenCopDestructorBody(CEventSeenCop* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86CB58;
    CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);
}

// 0x5FF420 (lote 196A, sin validar): destructor con borrado de CEventSeenCop del EXE (ranura 0 de su vtable 0x86CB58; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x5FF3A0;
// test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5FF3A0), libera `this` con CEvent::operator delete
// (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventSeenCop* __fastcall SeenCopScalarDeletingDestructor(CEventSeenCop* self, void* /*edx*/, uint8 flags) {
    SeenCopDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventSeenCop::InjectHooks()
{
    RH_ScopedVirtualClass(CEventSeenCop, 0x86CB58, 17);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(SeenCopDestructorBody, "DestructorBody", 0x5FF3A0);                       // lote 196A (sin validar)
    RH_ScopedNamedGlobalInstall(SeenCopScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5FF420);   // lote 196A (sin validar)
    RH_ScopedInstall(Constructor1, 0x5FF380);
    RH_ScopedVMTInstall(GetEventPriority, 0x5FF880);                                                       // lote 197A (sin validar): retorno de constante
    RH_ScopedVMTInstall(GetEventType, 0x5FF3B0);                                                           // lote 196A (sin validar): retorno de constante
    RH_ScopedVMTInstall(CloneEditable, 0x5FF3C0);                                                          // lote 196A (sin validar)
}

// 0x5FF3B0 (lote 196A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,48h`): EVENT_SEEN_COP (72); no lee el objeto (ranura 1 de la vtable 0x86CB58).
eEventType CEventSeenCop::GetEventType() const {
    return EVENT_SEEN_COP;
}

// 0x5FF3C0 (lote 196A, sin validar; thiscall, sin argumentos, `ret`, 88 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x86CB58).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x18 bytes; sin memoria devuelve nulo; si no, llama al constructor de CEventAcquaintancePed (0x4AF820, con hook; ECX = memoria) con la persona
// (+0x14), pone la vtable de CEventSeenCop (0x86CB58) y devuelve la memoria. Se llama al cuerpo enganchado del constructor (CEventAcquaintancePed::Constructor) y se escribe la vtable de la clase del EXE (la de la DLL no vale para los llamadores del EXE), como el resto de los CloneEditable.
CEventSeenCop* CEventSeenCop::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventSeenCop));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEventAcquaintancePed*>(memory)->Constructor(m_AcquaintancePed);
    *reinterpret_cast<uint32*>(memory) = 0x86CB58;
    return static_cast<CEventSeenCop*>(memory);
}

// 0x5FF380
CEventSeenCop* CEventSeenCop::Constructor1(CPed* cop)
{
    this->CEventSeenCop::CEventSeenCop(cop);
    return this;
}

// 0x5FF880 (lote 197A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,15h`): prioridad 21; no lee el objeto (ranura 2 de la vtable 0x86CB58).
int32 CEventSeenCop::GetEventPriority() const {
    return 21;
}
