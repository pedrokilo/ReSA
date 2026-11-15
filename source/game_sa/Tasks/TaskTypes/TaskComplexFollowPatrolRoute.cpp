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
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleRunNamedAnim.h"
#include "TaskSimpleNone.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexFollowNodeRoute.h"

static_assert(TASK_COMPLEX_FOLLOW_PATROL_ROUTE == 0x3A3); // 0x670A60: `mov eax,3A3h`

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
    self->m_nMoveState = static_cast<int16>(a);
    *reinterpret_cast<uint32*>(self) = 0x870114;
    self->m_nPatrolMode = static_cast<int16>(b);
    self->m_nFlags &= ~2u;
    auto* const memory   = static_cast<CPatrolRoute*>(CPatrolRoute::operator new(sizeof(CPatrolRoute)));
    CPatrolRoute* newRoute = memory ? memory->Constructor() : nullptr; // el EXE usa el EAX que devuelve el constructor (this), no el de la reserva
    self->m_pRoute         = newRoute;
    newRoute->Assign(route);
    *reinterpret_cast<int32*>(&self->m_fTargetRadius) = c; // dword en bruto (en el EXE es el radio, float)
    self->m_nFlags |= 1u;
    *reinterpret_cast<int32*>(&self->m_fMoveStateRadius) = d;
    self->m_nCurrentNode = 0;
    self->m_nRoutePasses = 0;
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

// 0x670A60 (lote 210A, sin validar; ranura 4 (GetTaskType) de la vtable 0x870114; thiscall, sin argumentos de pila, `ret`, 6 bytes: `mov eax,0x3A3; ret`): TASK_COMPLEX_FOLLOW_PATROL_ROUTE; no lee el objeto. La clase no tiene
// vtable exportada: se engancha como S.
eTaskType __fastcall FollowPatrolRouteGetTaskType(CTaskComplexFollowPatrolRoute* /*self*/, void* /*edx*/) {
    return TASK_COMPLEX_FOLLOW_PATROL_ROUTE;
}

// 0x672BB0 (lote 210A, sin validar; el listado decia CTaskComplexGoToPointAnyMeans): destructor con borrado de CTaskComplexFollowPatrolRoute del EXE (ranura 0 de su vtable 0x870114; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6709F0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). El cuerpo del destructor (0x6709F0: vtable, borrado de la ruta y ~CTaskComplex) aun
// no esta reconstruido y se llama POR DIRECCION. Libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexFollowPatrolRoute* __fastcall FollowPatrolRouteScalarDeletingDestructor(CTaskComplexFollowPatrolRoute* self, void* /*edx*/, uint8 flags) {
    plugin::CallMethod<0x6709F0, CTaskComplexFollowPatrolRoute*>(self);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
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
    RH_ScopedNamedGlobalInstall(FollowPatrolRouteGetTaskType, "GetTaskType", 0x670A60); // lote 210A (sin validar)
    RH_ScopedNamedGlobalInstall(FollowPatrolRouteScalarDeletingDestructor, "ScalarDeletingDestructor", 0x672BB0); // lote 210A (sin validar)
    RH_ScopedInstall(GetNextSubTaskType, 0x670A70); // lote 210A (sin validar)
    RH_ScopedInstall(CreateSubTask, 0x670B00);      // lote 210A (sin validar)
    RH_ScopedNamedInstall(CreateNextSubTaskBody, "CreateNextSubTask", 0x670D80);   // lote 210A (sin validar)
    RH_ScopedNamedInstall(CreateFirstSubTaskBody, "CreateFirstSubTask", 0x670E20); // lote 210A (sin validar)
    // TODO: instalar las funciones conforme se reconstruyan.
}

// 0x670A70 (lote 210A, sin validar; thiscall, sin argumentos de pila, `ret`, 121 bytes; lo llaman 0x670D80 y 0x670E20): devuelve el tipo de la siguiente subtarea. Con la ruta vacia (contador +0 de la ruta, comparado solo con
// cero) TASK_FINISHED (0x516). Con n = contador (con signo) y k = nodo actual (+0x12, con signo): k + 1 < n -> TASK_SIMPLE_GO_TO_POINT (0x384); k + 1 == n -> TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL (0x387); k + 1 > n y k != n ->
// TASK_NONE (0xC8). Con k == n (se pasa del ultimo nodo): el contador de pasadas (+0xE, 16 bits) se incrementa SIEMPRE, antes de mirar el modo (+0xC, con signo; > 3 sin signo -> TASK_NONE): modo 0 -> TASK_FINISHED; modo 1 ->
// TASK_FINISHED si el contador no es 1 (comparacion de 16 bits), si no sigue como el modo 2; modo 2 -> CPatrolRoute::Reverse (0x66D550, con hook) y sigue como el modo 3; modo 3 -> nodo actual = 0 y se vuelve a calcular (salto
// recursivo a 0x670A70 con el mismo this).
eTaskType CTaskComplexFollowPatrolRoute::GetNextSubTaskType() {
    const int32 nodeCount = static_cast<int32>(m_pRoute->m_nNumNodes);
    if (nodeCount == 0) {
        return TASK_FINISHED;
    }
    const int32 current = m_nCurrentNode;
    const int32 next    = current + 1;
    if (next < nodeCount) {
        return TASK_SIMPLE_GO_TO_POINT;
    }
    if (next == nodeCount) {
        return TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL;
    }
    if (current != nodeCount) {
        return TASK_NONE;
    }

    const int32 mode = m_nPatrolMode;
    m_nRoutePasses = static_cast<int16>(m_nRoutePasses + 1);
    if (static_cast<uint32>(mode) > 3u) {
        return TASK_NONE;
    }
    switch (mode) {
    case 0:
        return TASK_FINISHED;
    case 1:
        if (m_nRoutePasses != 1) {
            return TASK_FINISHED;
        }
        [[fallthrough]];
    case 2:
        m_pRoute->Reverse();
        [[fallthrough]];
    default: // 3
        m_nCurrentNode = 0;
        return GetNextSubTaskType();
    }
}

// 0x670B00 (lote 210A, sin validar; thiscall, un argumento de pila, `ret 4`, 631 bytes con marco SEH que solo libera la memoria si el constructor lanzara: se omite): crea la subtarea. Cada rama reserva con CTask::operator new
// (0x61A5A0, con hook) y, sin memoria, devuelve nulo sin construir. Tipos (comparacion con signo): cualquier otro (TASK_FINISHED incluido) -> nulo.
//  - TASK_SIMPLE_GO_TO_POINT (0x384): 0x30 bytes, CTaskSimpleGoToPoint (0x667CD0, con hook) con (estado de movimiento +0x10 con signo, punto k de la ruta (+0x144 + 12 k, k = +0x12 con signo, sin comprobar), radio +0x14, false, false).
//  - TASK_SIMPLE_NAMED_ANIM (0x191): 0x64 bytes, CTaskSimpleRunNamedAnim (0x61A990, con hook) con (nombre del nodo k (+4 + 0x28 k), segundo nombre (+0x18 mas), 0x58, 4.0f, -1, false, false, false, false).
//  - TASK_NONE (0xC8): 8 bytes, CTaskSimpleNone (constructor de CTaskSimple 0x61A390 y vtable 0x859CFC).
//  - TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL (0x387): radio de estado de movimiento = +0x18, salvo 0.0f con el modo (+0xC) 2 o 3, o con el modo 1 y el contador de pasadas (+0xE) a cero (se decide ANTES de reservar);
//    0x28 bytes, CTaskComplexGoToPointAndStandStill (0x668120, con hook) con (estado, punto k, radio +0x14, ese radio, false, false).
//  - TASK_COMPLEX_FOLLOW_NODE_ROUTE (0x38A): ruta vacia -> nulo sin reservar. Si el nodo actual (con signo) es igual al contador, se decrementa (16 bits) y se escribe en +0x12 (aunque despues no haya memoria). Copia el punto
//    k a una variable local, reserva 0x60 bytes y construye CTaskComplexFollowNodeRoute (0x66EA30, con hook) con (estado, punto, 0.5f (0x86FCA0), 3.0f (0x86FCA4), 2.0f (0x86FCA8), false, -1, true).
CTask* CTaskComplexFollowPatrolRoute::CreateSubTask(int32 taskType) {
    switch (taskType) {
    case TASK_SIMPLE_GO_TO_POINT:
        return new CTaskSimpleGoToPoint(static_cast<eMoveState>(m_nMoveState), m_pRoute->m_aPoints[m_nCurrentNode], m_fTargetRadius, false, false);
    case TASK_SIMPLE_NAMED_ANIM: {
        const CPatrolRoute::Node& node = m_pRoute->m_aNodes[m_nCurrentNode];
        return new CTaskSimpleRunNamedAnim(node.m_szName, node.m_szName2, 0x58, 4.0f, static_cast<uint32>(-1), false, false, false, false);
    }
    case TASK_NONE:
        return new CTaskSimpleNone();
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        float moveStateRadius = m_fMoveStateRadius;
        switch (m_nPatrolMode) {
        case 1:
            if (m_nRoutePasses == 0) {
                moveStateRadius = 0.0f;
            }
            break;
        case 2:
        case 3:
            moveStateRadius = 0.0f;
            break;
        default:
            break;
        }
        return new CTaskComplexGoToPointAndStandStill(static_cast<eMoveState>(m_nMoveState), m_pRoute->m_aPoints[m_nCurrentNode], m_fTargetRadius, moveStateRadius, false, false);
    }
    case TASK_COMPLEX_FOLLOW_NODE_ROUTE: {
        if (m_pRoute->m_nNumNodes == 0) {
            return nullptr;
        }
        if (static_cast<int32>(m_nCurrentNode) == static_cast<int32>(m_pRoute->m_nNumNodes)) {
            m_nCurrentNode = static_cast<int16>(m_nCurrentNode - 1);
        }
        const CVector point = m_pRoute->m_aPoints[m_nCurrentNode];
        return new CTaskComplexFollowNodeRoute(static_cast<eMoveState>(m_nMoveState), point, 0.5f, 3.0f, 2.0f, false, -1, true);
    }
    default:
        return nullptr;
    }
}

// 0x670D80 (lote 210A, sin validar; thiscall, un argumento de pila (el ped, sin usar), `ret 4`, 146 bytes; ranura 8 de la vtable 0x870114). Como el original (el tipo de la subtarea +8 se pide con una llamada virtual en cada
// comparacion): si NO es TASK_COMPLEX_FOLLOW_NODE_ROUTE: si es TASK_NONE -> CreateSubTask(TASK_FINISHED); si no es TASK_SIMPLE_NAMED_ANIM y el primer byte del nombre del nodo actual (+4 + 0x28 k) es distinto de cero (comparacion
// de UN byte con la cadena vacia 0x858B54) -> CreateSubTask(TASK_SIMPLE_NAMED_ANIM); en otro caso el nodo actual avanza (+0x12, 16 bits). Despues (y directamente con TASK_COMPLEX_FOLLOW_NODE_ROUTE, sin avanzar)
// CreateSubTask(GetNextSubTaskType()).
CTask* CTaskComplexFollowPatrolRoute::CreateNextSubTaskBody(CPed* /*ped*/) {
    if (m_pSubTask->GetTaskType() != TASK_COMPLEX_FOLLOW_NODE_ROUTE) {
        if (m_pSubTask->GetTaskType() == TASK_NONE) {
            return CreateSubTask(TASK_FINISHED);
        }
        if (m_pSubTask->GetTaskType() != TASK_SIMPLE_NAMED_ANIM) {
            if (m_pRoute->m_aNodes[m_nCurrentNode].m_szName[0] != '\0') {
                return CreateSubTask(TASK_SIMPLE_NAMED_ANIM);
            }
        }
        m_nCurrentNode = static_cast<int16>(m_nCurrentNode + 1);
    }
    return CreateSubTask(GetNextSubTaskType());
}

// 0x670E20 (lote 210A, sin validar; thiscall, ped en la pila, `ret 4`, 968 bytes; ranura 9). Como el original: apaga el bit 0 de las banderas (+0x20). Ruta vacia (contador 0) -> CreateSubTask(TASK_NONE) sin mas (un contador
// negativo, con signo, solo se salta los bucles). Busca el nodo siguiente al segmento de la ruta mas cercano al ped (posicion = matriz si hay, si no la posicion simple):
//  1. Por cada segmento i (de B = punto i a A = punto (i+1) % n): d = A - B (x, y, z), longitud al cuadrado = (z*z + x*x) + y*y (guardada como float), d normalizado con CVector::Normalise (0x59C910); r = ped - B;
//     t = (r.z*d.z + r.x*d.x) + r.y*d.y. Solo cuenta si t > 0 y sqrt(longitud al cuadrado) > t (comparaciones ordenadas: NaN descarta). El pie Q = B + d*t (x, y, z en ese orden, d*t primero y luego + B) y la distancia al
//     cuadrado D = (ez*ez + ey*ey) + ex*ex con e = ped - Q. Se queda con el segmento si D < mejor*mejor (EL MEJOR SE MULTIPLICA POR SI MISMO aunque se guarda la distancia al cuadrado: comportamiento del EXE; el primer
//     mejor es FLT_MAX): mejor = D y candidato = (i+1) % n.
//  2. Si ninguno valio (candidato -1): se prueba cada punto i (en el EXE desenrollado de cuatro en cuatro, mismo orden): D = (dz*dz + dy*dy) + dx*dx con d = ped - punto; mismo criterio D < mejor*mejor con mejor inicial FLT_MAX.
//  3. El nodo actual (+0x12, 16 bits) = candidato (puede quedar -1). Si el bit 1 de las banderas estaba puesto: se apaga y CreateSubTask(TASK_COMPLEX_FOLLOW_NODE_ROUTE); si no, se apaga y
//     CreateSubTask(GetNextSubTaskType()).
CTask* CTaskComplexFollowPatrolRoute::CreateFirstSubTaskBody(CPed* ped) {
    m_nFlags &= ~1u;
    const int32 nodeCount = static_cast<int32>(m_pRoute->m_nNumNodes);
    if (nodeCount == 0) {
        return CreateSubTask(TASK_NONE);
    }

    const CVector& pedPos = ped->GetPosition();
    int32 closest = -1;
    float best    = FLT_MAX; // 0x7F7FFFFF

    for (int32 i = 0; i < nodeCount; ++i) {
        const int32    next = (i + 1) % nodeCount;
        const CVector& to   = m_pRoute->m_aPoints[next];
        const CVector& from = m_pRoute->m_aPoints[i % nodeCount];

        CVector dir;
        dir.x = to.x - from.x;
        dir.y = to.y - from.y;
        dir.z = to.z - from.z;
        const float lengthSq = (dir.z * dir.z + dir.x * dir.x) + dir.y * dir.y;
        dir.Normalise();

        const float rx = pedPos.x - from.x;
        const float ry = pedPos.y - from.y;
        const float rz = pedPos.z - from.z;
        const float t  = (rz * dir.z + rx * dir.x) + ry * dir.y;
        if (!(t > 0.0f)) {
            continue;
        }
        if (!(std::sqrt(lengthSq) > t)) {
            continue;
        }

        const float dxt    = dir.x * t;
        const float dyt    = dir.y * t;
        const float dzt    = dir.z * t;
        const float qx     = dxt + from.x;
        const float qy     = dyt + from.y;
        const float qz     = dzt + from.z;
        const float ex     = pedPos.x - qx;
        const float ey     = pedPos.y - qy;
        const float ez     = pedPos.z - qz;
        const float distSq = (ez * ez + ey * ey) + ex * ex;
        if (distSq < best * best) {
            best    = distSq;
            closest = next;
        }
    }

    if (closest == -1) {
        float bestPoint = FLT_MAX;
        for (int32 i = 0; i < nodeCount; ++i) {
            const CVector& point  = m_pRoute->m_aPoints[i];
            const float    dx     = pedPos.x - point.x;
            const float    dy     = pedPos.y - point.y;
            const float    dz     = pedPos.z - point.z;
            const float    distSq = (dz * dz + dy * dy) + dx * dx;
            if (distSq < bestPoint * bestPoint) {
                bestPoint = distSq;
                closest   = i;
            }
        }
    }

    const uint32 flags = m_nFlags;
    m_nCurrentNode     = static_cast<int16>(closest);
    if (flags & 2u) {
        m_nFlags = flags & ~2u;
        return CreateSubTask(TASK_COMPLEX_FOLLOW_NODE_ROUTE);
    }
    m_nFlags = flags & ~2u;
    return CreateSubTask(GetNextSubTaskType());
}
