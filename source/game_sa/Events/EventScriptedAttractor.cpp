/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventScriptedAttractor
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
#include "EventScriptedAttractor.h"

namespace CEventAttractorBody {
void __fastcall AttractorDestructorBody(CEventAttractor* self, void* edx); // EventAttractor.cpp: 0x4AF400
}

namespace {
// 0x5FEF70 (lote 188A, sin validar): cuerpo del destructor de CEventScriptedAttractor del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86C9F0h; jmp 0x4AF400`). Deja la vtable de la clase
// del EXE (0x86C9F0) y sigue con el cuerpo de ~CEventAttractor (0x4AF400, con hook). No es un salto puro: escribe la vtable antes. Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call 0x5FEF70` del EXE.
__declspec(noinline) void __fastcall ScriptedAttractorDestructorBody(CEventScriptedAttractor* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86C9F0;
    CEventAttractorBody::AttractorDestructorBody(self, nullptr);
}

// 0x5FEFF0 (lote 188A, sin validar): destructor con borrado de CEventScriptedAttractor del EXE (ranura 0 de su vtable 0x86C9F0; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x5FEF70; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5FEF70), libera `this` con CEvent::operator delete
// (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventScriptedAttractor* __fastcall ScriptedAttractorScalarDeletingDestructor(CEventScriptedAttractor* self, void* /*edx*/, uint8 flags) {
    ScriptedAttractorDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace


void CEventScriptedAttractor::InjectHooks()
{
    RH_ScopedVirtualClass(CEventScriptedAttractor, 0x86C9F0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(ScriptedAttractorDestructorBody, "DestructorBody", 0x5FEF70);                           // lote 188A (sin validar)
    RH_ScopedNamedGlobalInstall(ScriptedAttractorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5FEFF0);       // lote 188A (sin validar)
    RH_ScopedInstall(Constructor, 0x5FEF40);
    RH_ScopedVMTInstall(GetEventType, 0x5FEF80);    // lote 188A (sin validar): retorno de constante
    RH_ScopedVMTInstall(CloneEditable, 0x5FEF90);   // lote 188A (sin validar)
}

// 0x5FEF80 (lote 188A, sin validar; thiscall, `ret`, 5 bytes: `mov eax,30h`): EVENT_SCRIPTED_ATTRACTOR (48); no lee el objeto (ranura 1 de la vtable 0x86C9F0).
eEventType CEventScriptedAttractor::GetEventType() const {
    return EVENT_SCRIPTED_ATTRACTOR;
}

// 0x5FEF90 (lote 188A, sin validar; thiscall, sin argumentos, `ret`, 93 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x86C9F0).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x20 bytes; sin memoria devuelve nulo; si no, llama al constructor de CEventAttractor (0x4AF350, con hook) con el efecto (+0x14), la entidad (+0x18)
// y false, deja la vtable de la clase del EXE (0x86C9F0) y devuelve la memoria. Llama al envoltorio enganchado del constructor (0x4AF350), no al constructor de C++ (que dejaria la vtable de la DLL).
CEventEditableResponse* CEventScriptedAttractor::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventScriptedAttractor));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEventAttractor*>(memory)->Constructor(m_2dEffect, m_entity, false);
    *reinterpret_cast<uint32*>(memory) = 0x86C9F0;
    return static_cast<CEventScriptedAttractor*>(memory);
}

// 0x5FEF40
CEventScriptedAttractor::CEventScriptedAttractor(C2dEffectPedAttractor* the2dEffect, CEntity* entity) :
    CEventAttractor(the2dEffect, entity, false)
{
}

// 0x5FEF40
// Como el original: dos argumentos (ret 8); con el tercero la DLL desequilibraba la pila de los llamadores del EXE
CEventScriptedAttractor* CEventScriptedAttractor::Constructor(C2dEffectPedAttractor* the2dEffect, CEntity* entity)
{
    this->CEventScriptedAttractor::CEventScriptedAttractor(the2dEffect, entity);
    return this;
}

