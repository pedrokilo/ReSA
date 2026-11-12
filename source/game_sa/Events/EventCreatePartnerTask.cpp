#include "StdInc.h"
#include "EventCreatePartnerTask.h"

namespace CEventBody {
void __fastcall EventDestructorBody(CEvent* self, void* edx); // Event.cpp: 0x4AC010
}

namespace {
// 0x5F6200 (lote 177A, sin validar): cuerpo del destructor de CEventCreatePartnerTask del EXE (thiscall, sin argumentos de pila, `ret`, 90 bytes con marco SEH que solo desenrolla). Deja la vtable de la clase del EXE
// (0x86C6D0; la de la DLL no vale para los llamadores del EXE), libera con CEntity::CleanUpOldReference (0x571A00) la referencia no nula del companero (+0x10) y llama al cuerpo de ~CEvent (0x4AC010, con hook propio).
// Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call 0x5F6200` del EXE.
__declspec(noinline) void __fastcall CreatePartnerTaskDestructorBody(CEventCreatePartnerTask* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86C6D0;
    CEntity::SafeCleanUpRef(self->m_partner);
    CEventBody::EventDestructorBody(self, nullptr);
}

// 0x5F6300 (lote 177A, sin validar): destructor con borrado de CEventCreatePartnerTask del EXE (ranura 0 de su vtable 0x86C6D0; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x5F6200;
// test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5F6200), libera `this` con CEvent::operator delete (0x4B5630, con hook)
// solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventCreatePartnerTask* __fastcall CreatePartnerTaskScalarDeletingDestructor(CEventCreatePartnerTask* self, void* /*edx*/, uint8 flags) {
    CreatePartnerTaskDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventCreatePartnerTask::InjectHooks() {
    RH_ScopedVirtualClass(CEventCreatePartnerTask, 0x86C6D0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(CreatePartnerTaskDestructorBody, "DestructorBody", 0x5F6200); // lote 177A (sin validar)
    RH_ScopedNamedGlobalInstall(CreatePartnerTaskScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5F6300); // lote 177A (sin validar)
    RH_ScopedInstall(Constructor, 0x5F6190);                                                                       // lote 177A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x5F6280);                                                                          // lote 177A (sin validar)
}

// 0x5F6190 (lote 177A, sin validar): constructor del EXE (thiscall, cuatro argumentos de pila, `ret 10h`, 108 bytes con marco SEH que solo desenrolla). Llama al constructor de CEvent (0x4ABFC0), guarda el tipo (+0xC), el BYTE
// bajo del tercer argumento (+0x10 es el companero, +0x14 el BYTE) y la distancia como dword (+0x18), deja la vtable de la clase del EXE (0x86C6D0) y registra la referencia del companero (+0x10) con
// CEntity::RegisterReference (0x571B70) si no es nulo. Devuelve `this`.
CEventCreatePartnerTask* CEventCreatePartnerTask::Constructor(int32 type, CPed* partner, bool isLeadSpeaker, float meetDist) { this->CEventCreatePartnerTask::CEventCreatePartnerTask(type, partner, isLeadSpeaker, meetDist); return this; }

// 0x5F6190
CEventCreatePartnerTask::CEventCreatePartnerTask(int32 type, CPed* partner, bool isLeadSpeaker, float meetDist) :
    m_partnerType{type},
    m_partner{partner},
    m_isLeadSpeaker{isLeadSpeaker},
    m_meetDist{meetDist}
{
    CEntity::SafeRegisterRef(m_partner);
}

CEventCreatePartnerTask::~CEventCreatePartnerTask() {
    CEntity::SafeCleanUpRef(m_partner);
}

// 0x5F6280 (lote 177A, sin validar; thiscall, sin argumentos, `ret`, 110 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 4 de la vtable 0x86C6D0).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x1C bytes; sin memoria devuelve nulo; si no, llama al constructor (0x5F6190, con hook; ECX = memoria) con el tipo (+0xC), el companero (+0x10), el BYTE
// +0x14 extendido a cero y la distancia (+0x18, como dword), y devuelve lo que el constructor deja en EAX.
CEvent* CEventCreatePartnerTask::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventCreatePartnerTask));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventCreatePartnerTask*>(memory)->Constructor(m_partnerType, m_partner, m_isLeadSpeaker, m_meetDist);
}
