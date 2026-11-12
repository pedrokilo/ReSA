/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToBoatSteeringWheel
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

// Este archivo se compila completo con /O2 /Oi /arch:IA32 /fp:fast /Zi, sin PCH ni unity (ver source/CMakeLists.txt):
// ComputeTargetPos (0x649160) suma con la x87 del EXE (sin el aplanado de denormales del SSE del juego).
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"

#include "TaskComplexGoToBoatSteeringWheel.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexGoToPointAndStandStill.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

void CTaskComplexGoToBoatSteeringWheel::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToBoatSteeringWheel, 0x86f018, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x649090);
    RH_ScopedInstall(Destructor, 0x649100);
    RH_ScopedInstall(ComputeTargetPos, 0x649160); // lote 171A (banco Fn171A)

    RH_ScopedVMTInstall(Clone, 0x64A350);
    RH_ScopedVMTInstall(GetTaskType, 0x6490F0); // lote 171A (banco Fn171A): retorno de constante
    RH_ScopedVMTInstall(CreateNextSubTask, 0x64E350);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x64E390);
    RH_ScopedVMTInstall(ControlSubTask, 0x64E3B0);
}

// 0x649090
CTaskComplexGoToBoatSteeringWheel::CTaskComplexGoToBoatSteeringWheel(CVehicle* vehicle) :
    m_Vehicle{vehicle}
{
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x649100
CTaskComplexGoToBoatSteeringWheel::~CTaskComplexGoToBoatSteeringWheel() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x64E350
CTask* CTaskComplexGoToBoatSteeringWheel::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
        return CreateSubTask(TASK_FINISHED);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        m_bAchievedPoint = true;
        return CreateSubTask(TASK_FINISHED);
    default:
        return nullptr; // Como el original
    }
}

// 0x649160 (lote 171A, banco Fn171A; thiscall, sin argumentos de pila, `ret`, 172 bytes): punto de la rueda del timon en el mundo. Toma el model info del vehiculo (indice de modelo con signo), su dummy 0
// (DUMMY_LIGHT_FRONT_MAIN) si es un barco o el 4 (DUMMY_SEAT_FRONT) si no, lo copia, lo multiplica por la matriz 3x3 del vehiculo con Multiply3x3 (0x59C790, con hook; recibe el puntero de la matriz tal cual,
// aunque sea nulo) y le suma la posicion del vehiculo (la de la matriz si la tiene, si no la de su placement) componente a componente con redondeo a float tras cada suma.
static_assert(offsetof(CVehicleModelInfo, m_nVehicleType) == 0x3C && offsetof(CVehicleModelInfo, m_pVehicleStruct) == 0x5C); // los desplazamientos que lee 0x649160

void CTaskComplexGoToBoatSteeringWheel::ComputeTargetPos() {
    const auto* const mi = static_cast<const CVehicleModelInfo*>(CModelInfo::GetModelInfo(static_cast<int16>(m_Vehicle->m_nModelIndex))); // sin la comprobacion virtual de AsVehicleModelInfoPtr, como el EXE
    const CVector d = mi->m_pVehicleStruct->m_avDummyPos[mi->m_nVehicleType == VEHICLE_TYPE_BOAT ? DUMMY_LIGHT_FRONT_MAIN : DUMMY_SEAT_FRONT];
    CVector r;
    Multiply3x3Exe(&r, m_Vehicle->m_matrix, &d);
    const CVector& pos = m_Vehicle->GetPosition();
    m_TargetPos = CVector{ r.x + pos.x, r.y + pos.y, r.z + pos.z };
}

// 0x6490F0 (lote 171A, banco Fn171A; thiscall, `ret`, 6 bytes: `mov eax,33Eh`): TASK_COMPLEX_GO_TO_BOAT_STEERING_WHEEL (830); no lee el objeto (ranura 4 de la vtable 0x86F018).
eTaskType CTaskComplexGoToBoatSteeringWheel::GetTaskType() const {
    static_assert(Type == 0x33E);
    return Type;
}

// 0x64CDA0
CTask* CTaskComplexGoToBoatSteeringWheel::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill(-1, false, false, 8.0f);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        ComputeTargetPos();
        return new CTaskComplexGoToPointAndStandStill(PEDMOVE_WALK, m_TargetPos, 0.5f, 2.0f, false, false);
    default: // Como el original: TASK_FINISHED y cualquier otro tipo
        return nullptr;
    }
}

// 0x64E390
CTask* CTaskComplexGoToBoatSteeringWheel::CreateFirstSubTask(CPed* ped) {
    m_FirstSubTaskStartTime = CTimer::GetTimeInMS();
    return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
}

// 0x64E3B0
CTask* CTaskComplexGoToBoatSteeringWheel::ControlSubTask(CPed* ped) {
    if (ped->IsPlayer()) {
        if (m_Vehicle) {
            if (CCarEnterExit::IsPlayerToQuitCarEnter(ped, m_Vehicle, m_FirstSubTaskStartTime, m_pSubTask)) {
                return CreateSubTask(TASK_SIMPLE_STAND_STILL);
            }
        }
    }

    // Possibly adjust goto point (if it has changed significantly)
    if (const auto subSubTask = m_pSubTask->GetSubTask()) {
        if (subSubTask->GetTaskType() != TASK_SIMPLE_STAND_STILL) {
            const auto subTaskGoTo = static_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask);
            ComputeTargetPos();
            // Como el original: (dz*dz + dy*dy) + dx*dx frente a 0.2*0.2 (0x863244) y GoToPoint sin mas condiciones
            const CVector delta = subTaskGoTo->m_vecTargetPoint - m_TargetPos;
            if (delta.z * delta.z + delta.y * delta.y + delta.x * delta.x > std::bit_cast<float>(0x3D23D70Bu)) {
                subTaskGoTo->GoToPoint(m_TargetPos, 0.5f, 2.f);
            }
        }
    }

    return m_pSubTask;
}
