/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFollowPatrolRoute
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

#include "TaskComplexFollowPatrolRoute.h"
#include "PatrolRoute.h"
#include "Entity/Ped/Ped.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
}

namespace CTaskComplexBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x674930 (lote 175A, banco Fn175A; el listado lo atribuia a CTaskComplexWanderProstitute): constructor de CTaskComplexFollowPatrolRoute del EXE (thiscall, CINCO argumentos de pila, `ret 14h`, 182 bytes, marco SEH que
// solo desenrolla). Argumentos: a (16 bits bajos), ruta origen, b (16 bits bajos), c y d (32 bits). Como el original, en este orden: base CTaskComplex (0x61A3B0, con hook); +0x10 = a; vtable de la clase del EXE (0x870114);
// +0xC = b; +0x20 con el bit 1 a cero (el resto no se toca); reserva una ruta de patrulla con CPatrolRoute::operator new (0x41B810, con hook; el EXE le pasa 0x1A4) y, SOLO si hay memoria, la construye con 0x66D440; guarda
// el EAX de esa llamada (el constructor devuelve this; nulo si no hubo memoria) en +0x1C y llama a 0x66D4B0 con ella (SIN comprobar nulo: con el pool lleno el EXE se cae, como esta funcion) para copiar la ruta origen; +0x14 = c, +0x18 = d, el bit 0 de +0x20 a uno
// (releido) y +0x12 y +0xE a cero. Devuelve `this`. El constructor de la clase no existe aun: el del EXE deja la vtable del EXE.
__declspec(noinline) CTaskComplexFollowPatrolRoute* __fastcall CTaskComplexFollowPatrolRouteConstructorExe(CTaskComplexFollowPatrolRoute* self, void* /*edx*/, int32 a, const CPatrolRoute* route, int32 b, uint32 c, uint32 d) {
    CTaskComplexBody::Constructor(self, nullptr);
    self->m_nParam10 = static_cast<int16>(a);
    *reinterpret_cast<uint32*>(self) = 0x870114;
    self->m_nParamC = static_cast<int16>(b);
    self->m_nFlags &= ~2u;
    auto* const memory   = static_cast<CPatrolRoute*>(CPatrolRoute::operator new(sizeof(CPatrolRoute)));
    CPatrolRoute* newRoute = memory ? memory->Constructor() : nullptr; // el EXE usa el EAX que devuelve el constructor (this), no el de la reserva
    self->m_pRoute         = newRoute;
    newRoute->Assign(route);
    self->m_nParam14 = c;
    self->m_nFlags |= 1u;
    self->m_nParam18 = d;
    self->m_nParam12 = 0;
    self->m_nParamE  = 0;
    return self;
}
} // namespace

namespace {
// Lote 209A (sin validar): la clase sigue sin campos declarados, asi que se leen en bruto los que muestran los desensamblados (ver el Clone de abajo): +0x08 subtarea (CTaskComplex::m_pSubTask), +0x1C CPatrolRoute*, +0x20 banderas (bit 0 y bit 1) y
// +0x24 un CVector (copia de la posicion del peaton), +0x12 un word.
template<typename T>
T& FieldAt(void* object, uint32 offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(object) + offset);
}

// 0x674BB0 (lote 195A, sin validar; el listado lo atribuia a CTaskComplexInvestigateDisturbance): Clone de CTaskComplexFollowPatrolRoute del EXE (ranura 1 de su vtable 0x870114; thiscall, sin argumentos de
// pila, `ret`, 114 bytes con marco SEH que solo libera la memoria si el constructor lanzara: omitido). La clase aun no esta reconstruida (sin campos): se leen en crudo los campos del EXE. Como el original:
// reserva 0x30 bytes con CTask::operator new (0x61A5A0, con hook); sin memoria devuelve nulo SIN leer nada; si no, llama al constructor del EXE 0x674930 (thiscall, `ret 14h`, aun sin reconstruir; llama a
// los constructores de CPatrolRoute 0x66D440 y de copia de ruta 0x66D4B0) con, en este orden: (int16)[+0x10] (movsx), [+0x1C] (puntero a la ruta), (int16)[+0xC] (movsx), [+0x14] y [+0x18]; devuelve su EAX
// (el objeto nuevo). Se engancha como S (la clase aun no tiene una declaracion virtual que nombre la ranura).
CTask* __fastcall CTaskComplexFollowPatrolRouteCloneExe(const CTaskComplexFollowPatrolRoute* self, void* /*edx*/) {
    void* const memory = CTask::operator new(0x30);
    if (!memory) {
        return nullptr;
    }
    const uint8* const source = reinterpret_cast<const uint8*>(self);
    const int32  field10 = *reinterpret_cast<const int16*>(source + 0x10);
    void* const  route   = *reinterpret_cast<void* const*>(source + 0x1C);
    const int32  field0C = *reinterpret_cast<const int16*>(source + 0x0C);
    const uint32 field14 = *reinterpret_cast<const uint32*>(source + 0x14);
    const uint32 field18 = *reinterpret_cast<const uint32*>(source + 0x18);
    return plugin::CallMethodAndReturn<CTask*, 0x674930, CTask*, int32, void*, int32, uint32, uint32>(static_cast<CTask*>(memory), field10, route, field0C, field14, field18);
}

// 0x66BA90 (lote 209A, sin validar; el listado decia CTaskComplexTurnToFaceEntityOrCoord; ranura 6 de la vtable 0x870114, MakeAbortable; thiscall con (ped, prioridad, evento), `ret 0Ch`, 104 bytes): si la prioridad es 0 pone a cero el primer
// dword de la ruta (+0x1C; es el contador de nodos; sin comprobar nulo); copia los tres dwords de la posicion del peaton (matriz +0x14 -> +0x30, o +4 sin matriz) a +0x24; llama a MakeAbortable de la subtarea (+8, ranura 6; sin comprobar nulo) con
// los mismos tres argumentos; guarda el resultado en el bit 1 de +0x20 (`flags ^= ((resultado << 1) ^ flags) & 2`) y devuelve ese bit.
bool __fastcall FollowPatrolRouteMakeAbortable(CTaskComplex* self, void* /*edx*/, CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == 0) {
        FieldAt<CPatrolRoute*>(self, 0x1C)->m_nNumNodes = 0;
    }
    const uint32* const position = reinterpret_cast<const uint32*>(&ped->GetPosition());
    uint32* const       saved    = &FieldAt<uint32>(self, 0x24);
    saved[0] = position[0];
    saved[1] = position[1];
    saved[2] = position[2];
    const bool aborted = FieldAt<CTask*>(self, 0x08)->MakeAbortable(ped, priority, event);
    uint32&    flags   = FieldAt<uint32>(self, 0x20);
    flags ^= ((static_cast<uint32>(aborted) << 1) ^ flags) & 2;
    return (flags >> 1) & 1;
}

// 0x66BB00 (lote 209A, sin validar; el listado decia CTaskComplexTurnToFaceEntityOrCoord; ranura 10 de la vtable 0x870114, ControlSubTask; thiscall con (ped), `ret 4`, 23 bytes): si alguno de los bits 0 o 1 de +0x20 esta puesto, pone a cero el word
// +0x12 y salta (`jmp [vtable + 0x24]`, ranura 9) a CreateFirstSubTask con el mismo peaton, devolviendo su resultado; si no, devuelve la subtarea actual (+8).
CTask* __fastcall FollowPatrolRouteControlSubTask(CTaskComplex* self, void* /*edx*/, CPed* ped) {
    if (FieldAt<uint8>(self, 0x20) & 3) {
        FieldAt<uint16>(self, 0x12) = 0;
        return self->CreateFirstSubTask(ped);
    }
    return FieldAt<CTask*>(self, 0x08);
}

// 0x6709F0 (lote 209A, sin validar; el listado decia CTaskComplexTurnToFaceEntityOrCoord; cuerpo del destructor de CTaskComplexFollowPatrolRoute, lo llama 0x672BB3; thiscall, sin argumentos, `ret`, 110 bytes con marco SEH que solo desenrolla: omitido): deja
// la vtable 0x870114; si la ruta (+0x1C) no es nula, ejecuta el destructor de sus ocho entradas (`eh vector destructor iterator` de 0x28 bytes desde +4 con 0x46A380, un `ret`: CPatrolRoute::Destructor, 0x6677A0) y la libera con
// CPatrolRoute::operator delete (0x41B820, con hook; no pone a nulo el puntero); termina con el cuerpo de CTaskComplex (0x61A3D0, con hook).
void __fastcall FollowPatrolRouteDestructorBody(CTaskComplex* self, void* /*edx*/) {
    FieldAt<uint32>(self, 0x00) = 0x870114;
    if (CPatrolRoute* const route = FieldAt<CPatrolRoute*>(self, 0x1C)) {
        route->Destructor();
        CPatrolRoute::operator delete(route);
    }
    CTaskComplexBody::Destructor(self, nullptr);
}
} // namespace

void CTaskComplexFollowPatrolRoute::InjectHooks() {
    RH_ScopedClass(CTaskComplexFollowPatrolRoute);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(CTaskComplexFollowPatrolRouteConstructorExe, "ConstructorExe", 0x674930); // lote 175A (banco Fn175A)
    RH_ScopedNamedGlobalInstall(CTaskComplexFollowPatrolRouteCloneExe, "Clone", 0x674BB0); // lote 195A (sin validar)
    RH_ScopedNamedGlobalInstall(FollowPatrolRouteMakeAbortable, "MakeAbortable", 0x66BA90);        // lote 209A (sin validar)
    RH_ScopedNamedGlobalInstall(FollowPatrolRouteControlSubTask, "ControlSubTask", 0x66BB00);      // lote 209A (sin validar)
    RH_ScopedNamedGlobalInstall(FollowPatrolRouteDestructorBody, "DestructorBody", 0x6709F0);      // lote 209A (sin validar)
    // RH_ScopedVMTInstall(GetTaskType, 0x670A60);
    // TODO: instalar las funciones conforme se reconstruyan.
}
