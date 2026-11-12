/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventAcquaintancePedHateBadlyLit
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
#include "EventAcquaintancePedHateBadlyLit.h"

namespace CEventAcquaintancePedBody {
void __fastcall AcquaintancePedDestructorBody(CEventAcquaintancePed* self, void* edx); // EventAcquaintancePed.cpp: 0x4AF890
}

namespace {
// 0x5FF2C0 (lote 178A, sin validar): cuerpo del destructor de CEventAcquaintancePedHateBadlyLit del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86CB10h; jmp 0x4AF890`). Deja la vtable de la clase
// del EXE (0x86CB10) y sigue con el cuerpo de ~CEventAcquaintancePed (0x4AF890, con hook). Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call 0x5FF2C0` del EXE.
__declspec(noinline) void __fastcall HateBadlyLitDestructorBody(CEventAcquaintancePedHateBadlyLit* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86CB10;
    CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);
}

// 0x5FF360 (lote 178A, sin validar): destructor con borrado de CEventAcquaintancePedHateBadlyLit del EXE (ranura 0 de su vtable 0x86CB10; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x5FF2C0; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5FF2C0), libera `this` con CEvent::operator delete
// (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventAcquaintancePedHateBadlyLit* __fastcall HateBadlyLitScalarDeletingDestructor(CEventAcquaintancePedHateBadlyLit* self, void* /*edx*/, uint8 flags) {
    HateBadlyLitDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace


void CEventAcquaintancePedHateBadlyLit::InjectHooks()
{
    RH_ScopedVirtualClass(CEventAcquaintancePedHateBadlyLit, 0x86CB10, 17);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(HateBadlyLitDestructorBody, "DestructorBody", 0x5FF2C0);                           // lote 178A (sin validar)
    RH_ScopedNamedGlobalInstall(HateBadlyLitScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5FF360);       // lote 178A (sin validar)
    RH_ScopedInstall(Constructor, 0x5FF250);
    RH_ScopedVMTInstall(GetEventType, 0x5FF2D0);                    // lote 178A (sin validar): retorno de constante
    RH_ScopedVMTInstall(CloneEditable, 0x5FF2E0);                   // lote 178A (sin validar)
    RH_ScopedVMTInstall(CanBeInterruptedBySameEvent, 0x5FF350);     // lote 178A (sin validar): siempre verdadero
    RH_ScopedVMTInstall(AffectsPed, 0x4AFA90);
}

// 0x5FF2D0 (lote 178A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,3Fh`): EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT (63); no lee el objeto (ranura 1 de la vtable 0x86CB10).
eEventType CEventAcquaintancePedHateBadlyLit::GetEventType() const {
    return EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT;
}

// 0x5FF350 (lote 178A, sin validar; thiscall, `ret`, 3 bytes: `mov al,1`): siempre verdadero (ranura 15 de la vtable 0x86CB10).
bool CEventAcquaintancePedHateBadlyLit::CanBeInterruptedBySameEvent() {
    return true;
}

// 0x5FF2E0 (lote 178A, sin validar; thiscall, sin argumentos, `ret`, 104 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x86CB10).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x28 bytes; sin memoria devuelve nulo; si no, llama al constructor (0x5FF250, con hook; ECX = memoria) con la persona (+0x14), el tiempo
// de inicio (+0x18) y la direccion del punto (+0x1C), y devuelve lo que el constructor deja en EAX.
CEventEditableResponse* CEventAcquaintancePedHateBadlyLit::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventAcquaintancePedHateBadlyLit));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventAcquaintancePedHateBadlyLit*>(memory)->Constructor(m_AcquaintancePed, m_startTimeInMs, m_point);
}

// 0x5FF250
CEventAcquaintancePedHateBadlyLit::CEventAcquaintancePedHateBadlyLit(CPed* ped, int32 startTimeInMs, const CVector& point) : CEventAcquaintancePed(ped)
{
    m_startTimeInMs = startTimeInMs;
    m_point = point;
    if (startTimeInMs == -1) {
        m_startTimeInMs = CTimer::GetTimeInMS();
        m_point = ped->GetPosition();
    }
}

// 0x5FF250
CEventAcquaintancePedHateBadlyLit* CEventAcquaintancePedHateBadlyLit::Constructor(CPed* ped, int32 startTimeInMs, const CVector& point)
{
    this->CEventAcquaintancePedHateBadlyLit::CEventAcquaintancePedHateBadlyLit(ped, startTimeInMs, point);
    return this;
}

// 0x4AFA90
bool CEventAcquaintancePedHateBadlyLit::AffectsPed(CPed* ped)
{
    if (CEventAcquaintancePed::AffectsPed(ped)) {
        CEvent* currentEvent = ped->GetEventHandlerHistory().GetCurrentEvent();
        if (!currentEvent || currentEvent->GetEventType() != EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT
            || currentEvent->GetSourceEntity() != GetSourceEntity())
        {
            return true;
        }
        auto theCurrentEvent = static_cast<CEventAcquaintancePedHateBadlyLit*>(currentEvent);
        // Como el original: diferencia de tiempos con signo (jl) y suma z*z + y*y + x*x; NaN cuenta como lejos (test ah,5 / jnp)
        if (static_cast<int32>(m_startTimeInMs - theCurrentEvent->m_startTimeInMs) >= 2000) {
            const CVector distance = m_point - theCurrentEvent->m_point;
            const float distSq = distance.z * distance.z + distance.y * distance.y + distance.x * distance.x;
            return !(distSq < 1.0f);
        }
    }
    return false;
}

