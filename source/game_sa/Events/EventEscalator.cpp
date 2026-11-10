/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventEscalator
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventEscalator.h"


namespace CEventBody {
void __fastcall EventDestructorBody(CEvent* self, void* edx); // Event.cpp: 0x4AC010
}

namespace {
// 0x5FF860 (lote 151A, banco Fn151A): cuerpo del destructor de CEventEscalator del EXE (sin hook; es `jmp 0x4AC010`, el cuerpo de ~CEvent, con hook; no escribe la vtable de la clase). Funcion propia sin expansion en linea
// para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x5FF860` del EXE.
__declspec(noinline) void __fastcall EscalatorDestructorBody(CEventEscalator* self, void* /*edx*/) {
    CEventBody::EventDestructorBody(self, nullptr);
}

// 0x602BE0 (lote 151A, banco Fn151A; el listado decia CInterestingEvents): destructor con borrado de CEventEscalator del EXE (ranura 0 de su vtable 0x86CD00; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x5FF860; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo (0x5FF860), libera `this` con
// CEvent::operator delete (0x4B5630, sin hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CEventEscalator* __fastcall EscalatorScalarDeletingDestructor(CEventEscalator* self, void* /*edx*/, uint8 flags) {
    EscalatorDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote151ABody_CEventEscalator() { return reinterpret_cast<uintptr_t>(&EscalatorDestructorBody); } // para el banco Fn151A
uintptr_t Lote151ABody_CEventOperatorDelete() { return reinterpret_cast<uintptr_t>(static_cast<void(*)(void*)>(&CEvent::operator delete)); } // 0x4B5630, para el banco Fn151A

void CEventEscalator::InjectHooks() {
    RH_ScopedVirtualClass(CEventEscalator, 0x86CD00, 16);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(EscalatorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x602BE0); // lote 151A (banco Fn151A)
    RH_ScopedVMTInstall(Clone, 0x602B90); // lote 151A (banco Fn151A)
    RH_ScopedVMTInstall(AffectsPed, 0x4B2580);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B25E0);
}

// 0x602B90 (lote 151A, banco Fn151A; el listado decia CInterestingEvents): thiscall, sin argumentos, `ret`, 80 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); no lee `this`.
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0xC bytes; sin memoria devuelve nulo; si no, CEvent::CEvent (0x4ABFC0, con hook; ECX = memoria), deja la vtable de la clase del EXE (0x86CD00;
// la de la DLL no vale para los llamadores del EXE) y devuelve la memoria.
CEventEscalator* CEventEscalator::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventEscalator));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEvent*>(memory)->Constructor();
    *reinterpret_cast<uint32*>(memory) = 0x86CD00;
    return static_cast<CEventEscalator*>(memory);
}

// 0x4B25E0 (lote 129A, banco Fn129A; el listado de pendientes lo atribuia a CEventEscalator y lo es: ranura 11 de la vtable 0x86CD00): thiscall, `ret 4`, 32 bytes. Como el original, pide
// primero el tipo del otro evento (ranura 1 de refEvent) y despues el propio (ranura 1 de this), y devuelve `propio != otro` (AL; el resto de EAX queda sin definir).
bool CEventEscalator::TakesPriorityOver(const CEvent& refEvent) {
    const eEventType other = refEvent.GetEventType();
    return GetEventType() != other;
}

// 0x4B2580
bool CEventEscalator::AffectsPed(CPed* ped) {
    if (ped->IsAlive() && !ped->IsPlayer() && ped->m_pContactEntity) {
        // Como el original: el modelo de la entidad se extiende con signo (movsx) y los MI_ sin signo (movzx)
        const int32 modelId = (int16)ped->m_pContactEntity->m_nModelIndex;
        if (modelId == (int32)(eModelID)ModelIndices::MI_ESCALATORSTEP || modelId == (int32)(eModelID)ModelIndices::MI_ESCALATORSTEP8)
            return true;
    }
    return false;
}