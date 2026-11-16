/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPotentialWalkIntoFire
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventPotentialWalkIntoFire.h"

namespace {
// Cuerpo no virtual de ~CEventPotentialWalkIntoFire del EXE (thiscall, sin argumentos de pila, `ret`): solo deja la vtable de CEvent del EXE (0x85AB38) (la clase no libera referencias). (Lote 119A, banco Fn119A.)
void __fastcall PotentialWalkIntoFireDestructorBody(CEventPotentialWalkIntoFire* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B6840 (lote 192A, sin validar): destructor con borrado de CEventPotentialWalkIntoFire del EXE (ranura 0 de su vtable 0x85B6A0; thiscall, un argumento de pila, `ret 4`, 77 bytes: `call 0x4B1F00; test byte [esp+..],1; je ..;`
// y, con el bit 0 del BYTE de banderas, el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, indice = (this - base) / 0x44 con signo, estado |= 0x80, cursor = min(cursor, indice)); devuelve `this`). El EXE conserva ECX tras la llamada al cuerpo (sin pila extra).
// Como el original: destruye con el cuerpo de la clase (0x4B1F00), libera `this` con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventPotentialWalkIntoFire* __fastcall PotentialWalkIntoFireScalarDeletingDestructor(CEventPotentialWalkIntoFire* self, void* /*edx*/, uint8 flags) {
    PotentialWalkIntoFireDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace


void CEventPotentialWalkIntoFire::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPotentialWalkIntoFire, 0x85B6A0, 17);
    RH_ScopedCategory("Events");
    RH_ScopedVMTInstall(CloneEditable, 0x4B7A40); // lote 201A (sin validar)
    RH_ScopedNamedGlobalInstall(PotentialWalkIntoFireDestructorBody, "Destructor", 0x4B1F00);
    RH_ScopedNamedGlobalInstall(PotentialWalkIntoFireScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B6840); // lote 192A (sin validar)

    RH_ScopedInstall(Constructor, 0x4B1E20);
    RH_ScopedVMTInstall(GetEventType, 0x4B1ED0);     // lote 184A (sin validar): ranura 1
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1EF0); // lote 184A (sin validar): ranura 2
    RH_ScopedVMTInstall(GetLifeTime, 0x4B1EE0);      // lote 184A (sin validar): ranura 3
    RH_ScopedVMTInstall(AffectsPed, 0x4B6890);
}

// 0x4B1ED0 (lote 184A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,2Eh`): EVENT_POTENTIAL_WALK_INTO_FIRE (46); no lee el objeto.
eEventType CEventPotentialWalkIntoFire::GetEventType() const {
    return EVENT_POTENTIAL_WALK_INTO_FIRE;
}

// 0x4B1EF0 (lote 184A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,1Fh`): prioridad 31; no lee el objeto.
int32 CEventPotentialWalkIntoFire::GetEventPriority() const {
    return 31;
}

// 0x4B1EE0 (lote 184A, sin validar; thiscall, `ret`, 3 bytes: `xor eax,eax`): tiempo de vida 0; no lee el objeto.
int32 CEventPotentialWalkIntoFire::GetLifeTime() {
    return 0;
}

// 0x4B1E20
CEventPotentialWalkIntoFire::CEventPotentialWalkIntoFire(const CVector& firePos, float fireSize, eMoveState moveState) :
    m_firePos{firePos},
    m_fireSize{fireSize},
    m_moveState{moveState}
{
    if (fireSize < 1.0f) {
        m_radius = 0.7f;
    } else {
        // Como el original: NaN toma 1.5 (test ah,5 / jp)
        m_radius = fireSize < 2.0f
            ? 1.0f
            : 1.5f;
    }
    m_radius += 0.35f;
}

// 0x4B1E20
CEventPotentialWalkIntoFire* CEventPotentialWalkIntoFire::Constructor(const CVector& firePos, float fireSize, eMoveState moveState) {
    this->CEventPotentialWalkIntoFire::CEventPotentialWalkIntoFire(firePos, fireSize, moveState);
    return this;
}

// 0x4B6890
bool CEventPotentialWalkIntoFire::AffectsPed(CPed* ped) {
    if (ped->IsAlive() && m_moveState != PEDMOVE_STILL) {
        auto goToTask = static_cast<CTaskSimpleGoTo*>(ped->GetTaskManager().GetSimplestActiveTask());
        if (goToTask && CTask::IsGoToTask(goToTask)) {
            CColSphere colSphere;
            colSphere.Set(m_radius, m_firePos, SURFACE_DEFAULT, 0, tColLighting(0xFF));
            CVector intersectPoint2;
            CVector intersectPoint1;
            return colSphere.IntersectEdge(ped->GetPosition(), goToTask->m_vecTargetPoint, intersectPoint1, intersectPoint2);
        }
    }
    return false;
}

// 0x4B7A40 (lote 201A, sin validar; thiscall, sin argumentos, `ret`, ranura 16 de la vtable 0x85B6A0; el listado la atribuia a otra clase). Como el original: CEvent::operator new (0x4B5620, con hook; el EXE lo expande en linea con 0x4B5570 sobre
// el pool 0xB744AC) con CEventPotentialWalkIntoFire; sin memoria devuelve nulo (EAX = 0); si no, llama al constructor del EXE (0x4B1E20, con hook; aqui Constructor) con la direccion de la posicion (+0x14), el tamano del fuego (+0x20, bits del float) y el estado de movimiento (+0x28) y devuelve lo que este deja en EAX (this). El marco SEH solo libera la
// memoria si el constructor lanza y se omite. No modifica el objeto copiado.
CEventPotentialWalkIntoFire* CEventPotentialWalkIntoFire::CloneEditable() const noexcept {
    auto* const object = static_cast<CEventPotentialWalkIntoFire*>(CEvent::operator new(sizeof(CEventPotentialWalkIntoFire)));
    return object ? object->Constructor(m_firePos, m_fireSize, m_moveState) : nullptr;
}
