#include "StdInc.h"

#include "TaskSimpleDuckWhileShotsWhizzing.h"
#include "AnimBlendAssociation.h"
#include "AnimManager.h"

// (lote 187A: las dos funciones siguientes pasan a ser miembros estaticos privados de la clase para poder llamar al envoltorio enganchado CTaskSimpleDuck::Destructor, privado)
// 0x6926F0 (lote 187A, sin validar; el listado decia CTaskSimpleDuck): cuerpo no virtual de ~CTaskSimpleDuckWhileShotsWhizzing del EXE (thiscall, sin argumentos de pila, 11 bytes: `mov dword [ecx],870BECh; jmp 0x692030`). Deja la vtable
// de la clase del EXE (0x870BEC) y sigue con el destructor de CTaskSimpleDuck (0x692030, con hook; el EXE salta, aqui se llama y se vuelve). Funcion propia sin expansion en linea; la llama el destructor con borrado (0x693B80)
// como el `call 0x6926F0` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleDuckWhileShotsWhizzing::DestructorBody(CTaskSimpleDuckWhileShotsWhizzing* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x870BEC;
    self->Destructor(); // correccion hecha al validar: llama al envoltorio enganchado (0x692030), como el `jmp 0x692030` del EXE (el destructor de C++ directo no es enganchable)
}

// 0x693B80 (lote 187A, sin validar; el listado decia CTaskSimpleDuck): destructor con borrado de CTaskSimpleDuckWhileShotsWhizzing del EXE (ranura 0 de su vtable 0x870BEC; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6926F0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x6926F0), libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
__declspec(noinline) CTaskSimpleDuckWhileShotsWhizzing* __fastcall CTaskSimpleDuckWhileShotsWhizzing::ScalarDeletingDestructor(CTaskSimpleDuckWhileShotsWhizzing* self, void* /*edx*/, uint8 flags) {
    DestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

void CTaskSimpleDuckWhileShotsWhizzing::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDuckWhileShotsWhizzing, 0x870BEC, 9); // vtable verificada en gta_sa.exe (GetTaskType en la ranura 4, MakeAbortable en la 6, ProcessPed en la 7)
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(CTaskSimpleDuckWhileShotsWhizzing::DestructorBody, "DestructorBody", 0x6926F0);                     // lote 187A (sin validar)
    RH_ScopedNamedGlobalInstall(CTaskSimpleDuckWhileShotsWhizzing::ScalarDeletingDestructor, "ScalarDeletingDestructor", 0x693B80); // lote 187A (sin validar)
    RH_ScopedInstall(Constructor, 0x692680);                                                                           // lote 187A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x692D60);                                                                              // lote 187A (sin validar): ranura 1
    RH_ScopedVMTInstall(GetTaskType, 0x6926E0);                                                                        // lote 187A (sin validar): ranura 4
    RH_ScopedVMTInstall(MakeAbortable, 0x692700);                                                                      // lote 187A (sin validar): ranura 6
    RH_ScopedVMTInstall(ProcessPed, 0x694640);                                                                         // lote 187A (sin validar): ranura 7
}

// 0x692680 (lote 187A, sin validar; el listado decia CTaskSimpleDuck; thiscall, un argumento de pila (WORD), `ret 4`, 85 bytes): CTask base (0x61A390 en linea en CTaskSimpleDuck), vtable 0x870BC4, +0x18/+0x19 = 0, +0x1A/+0x1B = 1,
// +0x1C/+0x20 = 0, +0x24 = 0 (tipo de control), +0x25 = 0xFF, +0x10/+0x14 = 0, +0x8 = hora actual (0xB7CB84), +0xC = duracion (WORD), +0xE = 0xFFFF (contador -1) y por ultimo la vtable 0x870BEC. Devuelve this.
CTaskSimpleDuckWhileShotsWhizzing::CTaskSimpleDuckWhileShotsWhizzing(uint16 lengthOfDuck) :
    CTaskSimpleDuck{DUCK_STANDALONE, lengthOfDuck, -1}
{
    // Correccion hecha al validar (lote 187A): el EXE deja al final la vtable 0x870BEC (la del EXE), no la de la DLL.
    *reinterpret_cast<uint32*>(this) = 0x870BEC;
}

CTaskSimpleDuckWhileShotsWhizzing* CTaskSimpleDuckWhileShotsWhizzing::Constructor(uint16 lengthOfDuck) {
    this->CTaskSimpleDuckWhileShotsWhizzing::CTaskSimpleDuckWhileShotsWhizzing(lengthOfDuck);
    return this;
}

// 0x692D60 (lote 187A, sin validar; el listado decia CTaskSimpleDuck; thiscall, sin argumentos, `ret`, 99 bytes con marco SEH que solo desenrolla la reserva; ranura 1 de la vtable 0x870BEC). Como el original: reserva 0x28 bytes
// con CTask::operator new (0x61A5A0, pool de tareas); sin memoria devuelve nulo; si no, llama al constructor (0x692680, con hook; ECX = memoria) con la duracion (+0xC, WORD extendido a cero) y devuelve lo que deja en EAX.
CTask* CTaskSimpleDuckWhileShotsWhizzing::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskSimpleDuckWhileShotsWhizzing));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CTaskSimpleDuckWhileShotsWhizzing*>(memory)->Constructor(m_LengthOfDuck);
}

// 0x6926E0 (lote 187A, sin validar; el listado decia CTaskSimpleDuck; thiscall, `ret`, 6 bytes: `mov eax,137h`): TASK_SIMPLE_DUCK_WHILE_SHOTS_WHIZZING (311); no lee el objeto (ranura 4 de la vtable 0x870BEC).
eTaskType CTaskSimpleDuckWhileShotsWhizzing::GetTaskType() const {
    return Type;
}

// 0x692700 (lote 187A, sin validar; el listado decia CTaskSimpleDuck; thiscall, tres argumentos de pila (ped, prioridad, evento), `ret 0Ch`, 167 bytes; ranura 6 de la vtable 0x870BEC). Como el original: con prioridad 2
// (ABORT_PRIORITY_IMMEDIATE) delega en CTaskSimpleDuck::MakeAbortable (0x692100, llamada directa) con los mismos argumentos. Si no, con evento no nulo cuyo tipo (ranura 1) es 0x31 (EVENT_SHOT_FIRED_WHIZZED_BY): sin animacion
// de agacharse (+0x10) devuelve falso; si no, pone la devolucion de llamada por defecto (CAnimBlendAssociation::SetFinishCallback 0x4CEBE0 con 0x4D1390 y datos nulos) y despues anula +0x10, hace lo mismo con la animacion
// de movimiento (+0x14) si no es nula, quita el bit 26 (bIsDucking) de la palabra +0x46C del ped y devuelve verdadero. Cualquier otro caso delega en CTaskSimpleDuck::MakeAbortable con los mismos argumentos.
bool CTaskSimpleDuckWhileShotsWhizzing::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        return CTaskSimpleDuck::MakeAbortable(ped, priority, event);
    }
    if (event && event->GetEventType() == EVENT_SHOT_FIRED_WHIZZED_BY) {
        if (!m_DuckAnim) {
            return false;
        }
        m_DuckAnim->SetDefaultFinishCallback();
        m_DuckAnim = nullptr;
        if (m_MoveAnim) {
            m_MoveAnim->SetDefaultFinishCallback();
            m_MoveAnim = nullptr;
        }
        ped->bIsDucking = false;
        return true;
    }
    return CTaskSimpleDuck::MakeAbortable(ped, priority, event);
}

// 0x694640 (lote 187A, sin validar; el listado decia CTaskSimpleDuck; thiscall, un argumento de pila (ped), `ret 4`, 68 bytes; ranura 7 de la vtable 0x870BEC). Como el original: sin animacion de agacharse (+0x10): la pide con
// CAnimManager::BlendAnimation (0x4D4610) sobre el clump del ped (+0x18), grupo 0, animacion 0x8D (ANIM_ID_DUCK_COWER) y mezcla 4.0f, la guarda en +0x10 y le pone como devolucion de llamada de fin
// CTaskSimpleDuck::DeleteDuckAnimCB (0x692550) con esta tarea como datos (CAnimBlendAssociation::SetFinishCallback 0x4CEBC0; no comprueba que la animacion exista, como el EXE). Despues devuelve lo que devuelve
// CTaskSimpleDuck::ProcessPed (0x694390, llamada directa) con el mismo ped.
bool CTaskSimpleDuckWhileShotsWhizzing::ProcessPed(CPed* ped) {
    if (!m_DuckAnim) {
        m_DuckAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_DUCK_COWER, 4.0f);
        m_DuckAnim->SetDeleteCallback(CTaskSimpleDuck::DeleteDuckAnimCB, this); // correccion hecha al validar: el EXE llama a 0x4CEBC0 (SetDeleteCallback), no a 0x4CEBE0 (SetFinishCallback)
    }
    return CTaskSimpleDuck::ProcessPed(ped);
}
