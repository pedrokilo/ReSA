/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWalkRoundObject
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

// Este archivo se compila completo con /O2 /Oi /arch:IA32 /fp:fast /Zi, sin PCH ni unity (ver source/CMakeLists.txt):
// ComputeRouteRoundObject (0x6551D0) devuelve en ST0 el resultado del x87 del EXE.
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"

#include "TaskComplexWalkRoundObject.h"
#include "PedGeometryAnalyser.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexFollowPointRoute.h"
#include "X87Math.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
}

namespace {
// 0x6550D0 (lote 171A, banco Fn171A; thiscall, sin argumentos de pila, `ret`, 106 bytes con marco SEH que solo desenrolla): cuerpo del destructor de CTaskComplexWalkRoundObject del EXE. Deja la vtable de la clase
// del EXE (0x86F364); si hay objeto (+1Ch) llama a CEntity::CleanUpOldReference (0x571A00, con hook) con la direccion del campo; si hay ruta (+20h) la libera con CPointRoute::operator delete (0x41B7D0, con hook);
// y termina con el cuerpo de ~CTaskComplex (0x61A3D0, con hook). No pone a nulo ningun campo. Funcion propia sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall WalkRoundObjectDestructorBody(CTaskComplexWalkRoundObject* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86F364;
    CEntity::SafeCleanUpRef(self->m_object);
    if (self->m_pointRoute) {
        delete self->m_pointRoute; // CPointRoute::operator delete (0x41B7D0)
    }
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x657200 (lote 182A, sin validar; el listado decia CTaskComplexWalkRoundBuildingAttempt): destructor con borrado de CTaskComplexWalkRoundObject del EXE (ranura 0 de su vtable 0x86F364; thiscall, un argumento de pila, `ret 4`,
// 30 bytes: `push esi; mov esi,ecx; call 0x6550D0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x6550D0, con hook),
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexWalkRoundObject* __fastcall WalkRoundObjectScalarDeletingDestructor(CTaskComplexWalkRoundObject* self, void* /*edx*/, uint8 flags) {
    WalkRoundObjectDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexWalkRoundObject::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundObject, 0x86F364, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(WalkRoundObjectDestructorBody, "DestructorBody", 0x6550D0); // lote 171A (banco Fn171A)
    RH_ScopedVMTInstall(GetTaskType, 0x6550C0);                                             // lote 171A
    RH_ScopedNamedGlobalInstall(WalkRoundObjectScalarDeletingDestructor, "ScalarDeletingDestructor", 0x657200); // lote 182A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x655C10);                                                   // lote 182A (sin validar)
    RH_ScopedInstall(CreateSubTask, 0x655290);                                              // lote 171A
    RH_ScopedInstall(Constructor, 0x655020);
    RH_ScopedInstall(CreateRouteTask, 0x655140);
    RH_ScopedInstall(ComputeRouteRoundObject, 0x6551D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657220);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x657380);
    RH_ScopedVMTInstall(ControlSubTask, 0x6575F0);
}

// 0x655020
CTaskComplexWalkRoundObject::CTaskComplexWalkRoundObject(int32 moveState, const CVector& targetPoint, CEntity* object) : CTaskComplex() {
    m_moveState   = moveState;
    m_targetPoint = targetPoint;
    m_object      = object;
    field_24      = 0;
    field_28      = 0;
    field_2C      = 0;
    field_2D      = 0;

    CEntity::SafeRegisterRef(m_object);

    // Como el original: CPointRoute::operator new (0x41B7C0) y solo se pone a cero el contador;
    // `new CPointRoute()` inicializaba por valor los 0x60 bytes de puntos
    auto* route = static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
    if (route) {
        route->m_NumEntries = 0;
    }
    m_pointRoute = route;
}

CTaskComplexWalkRoundObject::~CTaskComplexWalkRoundObject() {
    CEntity::SafeCleanUpRef(m_object);

    if (m_pointRoute) { // CPool::Delete ya no comprueba nullptr (como el EXE)
        delete m_pointRoute;
    }
    // todo: m_pointRoute = nullptr;
}

CTaskComplexWalkRoundObject* CTaskComplexWalkRoundObject::Constructor(int32 moveState, const CVector& targetPoint, CEntity* object) {
    this->CTaskComplexWalkRoundObject::CTaskComplexWalkRoundObject(moveState, targetPoint, object);
    return this;
}

// 0x655C10 (lote 182A, sin validar; el listado decia CTaskComplexWalkRoundBuildingAttempt; ranura 1 de la vtable 0x86F364; thiscall, sin argumentos, `ret`, 104 bytes con marco SEH que solo desenrolla): copia de la tarea.
// Reserva 0x54 bytes con CTask::operator new (0x61A5A0; nulo -> devuelve nulo sin construir) y llama al constructor (0x655020, con hook) con (estado de movimiento +0xC, &punto destino +0x10, objeto +0x1C); devuelve su resultado.
CTask* CTaskComplexWalkRoundObject::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskComplexWalkRoundObject));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CTaskComplexWalkRoundObject*>(memory)->Constructor(m_moveState, m_targetPoint, m_object);
}

// 0x6550C0 (lote 171A, banco Fn171A; thiscall, `ret`, 6 bytes: `mov eax,1FEh`): TASK_COMPLEX_WALK_ROUND_OBJECT (510); no lee el objeto (ranura 4 de la vtable 0x86F364).
eTaskType CTaskComplexWalkRoundObject::GetTaskType() const {
    static_assert(Type == 0x1FE);
    return Type;
}

// 0x655290 (lote 171A, banco Fn171A; thiscall, tipo y ped en la pila, `ret 8`, 161 bytes con marco SEH que solo desenrolla): subtarea por tipo. TASK_SIMPLE_STAND_STILL (0xCB): CTask::operator new(0x20)
// (0x61A5A0) y, si hay memoria, CTaskSimpleStandStill (0x62F310, con hook) con (500, false, false, 8.0f); sin memoria devuelve nulo. TASK_COMPLEX_FOLLOW_POINT_ROUTE (0x389): CreateRouteTask (0x655140, con hook)
// con el ped y devuelve su resultado. TASK_FINISHED (0x516) y cualquier otro tipo: nulo.
CTask* CTaskComplexWalkRoundObject::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_STAND_STILL: {
        void* const memory = CTask::operator new(sizeof(CTaskSimpleStandStill));
        if (!memory) {
            return nullptr;
        }
        return static_cast<CTaskSimpleStandStill*>(memory)->Constructor(500, false, false, 8.0f); // el EXE devuelve el EAX del constructor
    }
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE:
        return CreateRouteTask(ped);
    default: // TASK_FINISHED y cualquier otro tipo
        return nullptr;
    }
}

// 0x6575F0
CTask* CTaskComplexWalkRoundObject::ControlSubTask(CPed* ped) {
    // Como el original: se aborta si no hay objeto, si venció el plazo de la ruta (comparación sin signo), si el contador del peatón
    // supera 30 (0x86C938, con signo), o si el objeto se ha movido o girado; con la subtarea en reposo no se evalúa nada más
    bool abort = false;
    if (!m_object) {
        abort = true;
    } else {
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_FOLLOW_POINT_ROUTE && field_2C) {
            if (field_2D) {
                field_24 = (int32)CTimer::GetTimeInMS();
                field_2D = 0;
            }
            if (!(CTimer::GetTimeInMS() < (uint32)field_24 + (uint32)field_28)) {
                abort = true;
            }
        }
        if (!abort) {
            if (ped->GetIntelligence()->m_AnotherStaticCounter > 30) {
                abort = true;
            } else if (m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
                return m_pSubTask;
            } else {
                if (!m_object->m_matrix) {
                    m_object->AllocateMatrix();                              // 0x54F560
                    m_object->m_placement.UpdateMatrix(m_object->m_matrix);  // 0x54F1B0
                }
                const CMatrix& mat = *m_object->m_matrix;
                const CVector& pos = m_object->GetPosition();
                if (WalkRoundMovedCpp(&m_objectStartPos.x, &pos.x)) {                                       // |inicio - posición|² > 0.0625
                    abort = true;
                } else if (WalkRoundDotBelowCpp(&mat.GetForward().x, &m_objectStartForward.x)) {            // frente actual . inicial < 0.9
                    abort = true;
                } else if (WalkRoundDotBelowCpp(&m_objectStartRight.x, &mat.GetRight().x)) {                // derecha inicial . actual < 0.9
                    abort = true;
                } else {
                    return m_pSubTask;
                }
            }
        }
    }

    // Como el original (0x657718): si la subtarea se puede abortar, se crea la de reposo (CreateSubTask 0x655290, TASK_SIMPLE_STAND_STILL)
    if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return m_pSubTask;
    }
    return new CTaskSimpleStandStill(500, false, false, 8.0f);
}

// 0x657220
CTask* CTaskComplexWalkRoundObject::CreateNextSubTask(CPed* ped) {
    if (!m_object) {
        return nullptr;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: // 0x387
        return CreateRouteTask(ped);
    case TASK_SIMPLE_ACHIEVE_HEADING: // 0x386
        break;
    default: // incluye TASK_SIMPLE_STAND_STILL (0xCB) y TASK_COMPLEX_FOLLOW_POINT_ROUTE (0x389)
        return nullptr;
    }

    if (m_pointRoute->m_NumEntries != 0) {
        return CreateRouteTask(ped);
    }

    // Como el original: punto más cercano de la superficie del objeto al peatón; si cae del mismo lado que el destino se añaden a la ruta
    // ese punto y el más cercano al destino; si no, se camina hasta el punto
    CVector point;
    CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_object, point);
    const int32 sideOfPoint  = CPedGeometryAnalyser::ComputeEntityHitSide(point, *m_object);
    const int32 sideOfTarget = CPedGeometryAnalyser::ComputeEntityHitSide(m_targetPoint, *m_object);
    if (sideOfPoint == sideOfTarget) {
        m_pointRoute->AddUnlessFull(point);
        CPedGeometryAnalyser::ComputeClosestSurfacePoint(m_targetPoint, *m_object, point);
        m_pointRoute->AddUnlessFull(point);
        return CreateRouteTask(ped);
    }
    return new CTaskComplexGoToPointAndStandStill(PEDMOVE_WALK, point, 0.5f, 2.0f, false, false); // 0x86FC84 y 0x86FC88
}

// 0x657380
CTask* CTaskComplexWalkRoundObject::CreateFirstSubTask(CPed* ped) {
    if (!m_object) {
        return nullptr;
    }

    m_pointRoute->m_NumEntries = 0;
    const float routeDist = static_cast<float>(ComputeRouteRoundObject(ped)); // 0x6551D0: devuelve un float en ST0
    if (m_pointRoute->m_NumEntries == 0) {
        return nullptr;
    }

    field_24 = (int32)CTimer::GetTimeInMS();
    field_28 = m_moveState == PEDMOVE_WALK ? 8000 : 4000;
    field_2C = 1;

    // Como el original: posición, frente y derecha iniciales del objeto (se crea la matriz si falta)
    m_objectStartPos = m_object->GetPosition();
    if (!m_object->m_matrix) {
        m_object->AllocateMatrix();                              // 0x54F560
        m_object->m_placement.UpdateMatrix(m_object->m_matrix);  // 0x54F1B0
    }
    m_objectStartForward = m_object->m_matrix->GetForward();
    if (!m_object->m_matrix) {
        m_object->AllocateMatrix();
        m_object->m_placement.UpdateMatrix(m_object->m_matrix);
    }
    m_objectStartRight = m_object->m_matrix->GetRight();

    // Como el original: dirección del peatón al primer punto de la ruta (floats guardados), normalizada con su módulo (0x59C970), y su
    // producto escalar con el frente del peatón (sin comprobar su matriz) en el orden (z + y) + x, sin redondear
    const CVector& pedPos = ped->GetPosition();
    const CVector& p0     = m_pointRoute->m_Entries.data()[0];
    CVector        dir{ p0.x - pedPos.x, p0.y - pedPos.y, p0.z - pedPos.z };
    const float    mag    = dir.NormaliseAndMag();
    const CMatrix* pedMat = ped->m_matrix;
    const bool     facing = WalkRoundDotPositiveCpp(&dir.x, &pedMat->GetForward().x);
    const float    maxDist = m_moveState == PEDMOVE_WALK ? 2.0f : (m_moveState == PEDMOVE_RUN ? 4.0f : 6.0f); // 0x858CA0 / 0x858B90 / 0x858B44
    if (mag > maxDist && routeDist > maxDist && facing) {
        return CreateRouteTask(ped);
    }

    // Como el original: se gira hacia el primer punto (ángulo entre (x0 - px, y0 - py) y el origen, limitado)
    const float x = p0.x - pedPos.x;
    const float y = p0.y - pedPos.y;
    const float heading = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(x, y, 0.f, 0.f));
    return new CTaskSimpleAchieveHeading(heading, 1.0f, 0.2f);
}

// 0x655140 (ret 4: el EXE no usa el peaton). Sin ruta (contador a cero) no crea nada; si no, reserva con CTask::operator new (0x61A5A0) y, con el pool agotado (nulo),
// no construye y devuelve nulo (`test eax, eax; je`); el constructor 0x671510 recibe (movimiento, ruta, 0 = ONE_WAY, 0.5f [0x86FC98], 0.0f, true, false, false).
// La ruta se lee sin comprobar el nulo, como el EXE.
CTask* CTaskComplexWalkRoundObject::CreateRouteTask(CPed* ped) {
    if (m_pointRoute->m_NumEntries == 0) {
        return nullptr;
    }
    void* const memory = CTask::operator new(sizeof(CTaskComplexFollowPointRoute));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskComplexFollowPointRoute(static_cast<eMoveState>(m_moveState), *m_pointRoute, CTaskComplexFollowPointRoute::Mode::ONE_WAY, 0.5f, 0.0f, true, false, false);
}

// 0x6551D0
#pragma float_control(push)
#pragma float_control(precise, on)
double CTaskComplexWalkRoundObject::ComputeRouteRoundObject(CPed* ped) {
    // Como el original: el margen del analizador de geometria (0x8D22B0) vale 0.7 (0x3F333333) solo durante el calculo de la ruta
    // (se copia como bits enteros: un float con NaN senalizante se convertiria en NaN silencioso)
    auto&        margin      = StaticRef<uint32>(0x8D22B0);
    const uint32 savedMargin = margin;
    margin = std::bit_cast<uint32>(0.7f);
    CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(*ped, *m_object, m_targetPoint, *m_pointRoute, 0);
    margin = savedMargin;

    // Planos del objeto a la altura del peaton y lado que toca el peaton; el resultado es la distancia con signo al plano (suma en el orden z, y, x)
    CVector planes[4];
    float   planesDot[4];
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(ped->GetPosition().z, *m_object, &planes, planesDot);
    const auto side = CPedGeometryAnalyser::ComputeEntityHitSide(ped->GetPosition(), *m_object);
    const auto& pos   = ped->GetPosition();
    const auto& plane = planes[side];
    return ((static_cast<double>(plane.z) * pos.z + static_cast<double>(plane.y) * pos.y) + static_cast<double>(plane.x) * pos.x) + planesDot[side];
}
#pragma float_control(pop)
