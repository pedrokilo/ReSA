/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskGoToVehicleAndLean
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskGoToVehicleAndLean.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"
#include "TaskLeanOnVehicle.h"
#include "TaskComplexGangLeader.h"
#include "X87Math.h"

namespace {
// 0x660EE0 (lote 154A, banco Fn154A): cuerpo del destructor de CTaskGoToVehicleAndLean del EXE (con hook (Destructor); vtable 0x86FAC8). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor
// con borrado como el `call 0x660EE0` del EXE.
__declspec(noinline) void __fastcall CTaskGoToVehicleAndLeanDestructorBody(CTaskGoToVehicleAndLean* self, void* /*edx*/) { self->CTaskGoToVehicleAndLean::~CTaskGoToVehicleAndLean(); }

// 0x6644E0 (lote 154A, banco Fn154A): destructor con borrado de CTaskGoToVehicleAndLean del EXE (ranura 0 de su vtable 0x86FAC8; thiscall, un argumento de pila, `ret 4`, 30 bytes, igual que 0x6644C0 con el cuerpo 0x660EE0).
// Libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskGoToVehicleAndLean* __fastcall CTaskGoToVehicleAndLeanScalarDeletingDestructor(CTaskGoToVehicleAndLean* self, void* /*edx*/, uint8 flags) {
    CTaskGoToVehicleAndLeanDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote154ABody_CTaskGoToVehicleAndLean() { return reinterpret_cast<uintptr_t>(&CTaskGoToVehicleAndLeanDestructorBody); } // para el banco Fn154A

void CTaskGoToVehicleAndLean::InjectHooks() {
    RH_ScopedVirtualClass(CTaskGoToVehicleAndLean, 0x86FAC8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskGoToVehicleAndLeanScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6644E0); // lote 154A (banco Fn154A)

    RH_ScopedInstall(Constructor, 0x660E60);
    RH_ScopedInstall(Destructor, 0x660EE0);
    RH_ScopedInstall(DoTidyUp, 0x660F60);
    RH_ScopedInstall(CalcTargetPos, 0x664770);    // lote 177A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x660ED0);   // lote 177A (sin validar): retorno de constante
    RH_ScopedVMTInstall(Clone, 0x6621B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x664500);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x664590);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x664D40);
    RH_ScopedVMTInstall(ControlSubTask, 0x664E60);
}

// 0x660ED0 (lote 177A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,4C0h`): TASK_COMPLEX_GOTO_VEHICLE_AND_LEAN (1216); no lee el objeto (ranura 4 de la vtable 0x86FAC8).
eTaskType CTaskGoToVehicleAndLean::GetTaskType() const {
    return Type;
}

// 0x660E60
CTaskGoToVehicleAndLean::CTaskGoToVehicleAndLean(CVehicle* vehicle, int32 leanAnimDurationInMs) {
    m_Vehicle = vehicle;
    m_LeanAnimDurationInMs = leanAnimDurationInMs;
    m_LeanOnVehicle = false;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x660EE0
CTaskGoToVehicleAndLean::~CTaskGoToVehicleAndLean() {
    if (m_LeanOnVehicle) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_LeanOnVehicle = false;
    }
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x664500
bool CTaskGoToVehicleAndLean::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    const bool aborted = m_pSubTask->MakeAbortable(ped, priority, event);
    if (priority != ABORT_PRIORITY_IMMEDIATE && !aborted) return false;
    DoTidyUp(ped);
    return true;
}

// 0x664590
CTask* CTaskGoToVehicleAndLean::CreateNextSubTask(CPed* ped) {
    if (!m_Vehicle) { if (m_pSubTask) m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr); return nullptr; }
    if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x387)) {
        CVector dir = m_Vehicle->GetMatrix().GetRight();
        if (!field_15) dir *= -1.f;
        const auto angle = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.f, 0.f));
        return new CTaskSimpleAchieveHeading(angle, .5f, .2f);
    }
    if (m_pSubTask->GetTaskType() != static_cast<eTaskType>(0x386) && m_pSubTask->GetTaskType() != static_cast<eTaskType>(0xCB)) { DoTidyUp(ped); return nullptr; }
    return m_LeanOnVehicle ? static_cast<CTask*>(new CTaskLeanOnVehicle(m_Vehicle, m_LeanAnimDurationInMs, 0)) : new CTaskSimpleStandStill(500, false, false, 8.f);
}

// 0x664D40
CTask* CTaskGoToVehicleAndLean::CreateFirstSubTask(CPed* ped) {
    if (!m_Vehicle || m_Vehicle->vehicleFlags.bHasGangLeaningOn) { DoTidyUp(ped); return nullptr; }
    m_Vehicle->vehicleFlags.bHasGangLeaningOn = true;
    m_TargetPos = CalcTargetPos(ped);
    const CVector delta = m_TargetPos - ped->GetPosition();
    if ((delta.z*delta.z + delta.y*delta.y) + delta.x*delta.x < 1.f) return nullptr;
    return new CTaskComplexGoToPointAndStandStill(static_cast<eMoveState>(4), m_TargetPos, .05f, 2.f, false, true);
}

// 0x664E60
CTask* CTaskGoToVehicleAndLean::ControlSubTask(CPed* ped) {
    if (!m_Vehicle) { if (m_pSubTask) m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr); return nullptr; }
    if (ped->GetIntelligence()->m_AnotherStaticCounter > 30) { DoTidyUp(ped); return nullptr; }
    const auto speed = m_Vehicle->GetSpeed(ped->GetPosition() - m_Vehicle->GetPosition());
    if ((m_Vehicle->m_pDriver || std::sqrt((speed.z*speed.z + speed.y*speed.y) + speed.x*speed.x) > .01f) && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) { DoTidyUp(ped); return nullptr; }
    const auto delta = CalcTargetPos(ped) - m_TargetPos;
    if ((delta.x*delta.x + delta.y*delta.y) + delta.z*delta.z > std::bit_cast<float>(uint32{0x3C23D70B}) && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) { DoTidyUp(ped); return nullptr; }
    if (!m_LeanOnVehicle) {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            const auto index = CAnimManager::GetAnimationBlockIndex("gangs");
            if (CAnimManager::GetAnimBlocks()[index].IsLoaded != 1) CStreaming::RequestModel(IFPToModelId(index), STREAMING_KEEP_IN_MEMORY);
            else { CAnimManager::AddAnimBlockRef(index); m_LeanOnVehicle = true; }
        }
    } else if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_LeanOnVehicle = false;
    }
    return m_pSubTask;
}

// 0x664770 (lote 177A, sin validar; thiscall, dos argumentos de pila (puntero de retorno oculto y el peaton), `ret 8`, 295 bytes; devuelve el puntero de retorno en EAX).
// Como el original: lee la caja de colision del modelo del vehiculo (minimo +0, maximo +0xC); delta = posicion del peaton - posicion del vehiculo (cada una de la matriz si existe, o de m_placement; las restas, los productos y la suma van en la x87 sin guardar parciales: ayudantes de X87Math.cpp); el vector derecha es
// la primera fila de la matriz o, sin matriz, (cos(rumbo), sin(rumbo), 0) con fcos/fsin sin redondear; producto escalar (rz * dz + ry * dy) + rx * dx comparado con 0.0f (ordenado: NaN da falso). Si es mayor que cero guarda
// field_15 = 1 y el punto local es (maximo.x + 0.5f, 0, 0); si no, field_15 = 0 y (minimo.x - 0.5f, 0, 0). El resultado es CMatrix * CVector (0x59C890) con la matriz del vehiculo SIN comprobar si es nula (como el EXE).
CVector CTaskGoToVehicleAndLean::CalcTargetPos(CPed* ped) {
    CVehicle* const vehicle = m_Vehicle;
    const CBoundingBox& box = CModelInfo::GetModelInfo(vehicle->m_nModelIndex)->GetColModel()->m_boundBox;
    const float minX = box.m_vecMin.x;
    const float maxX = box.m_vecMax.x;
    const CVector& pedPos = ped->GetPosition();
    const CVector& vehPos = vehicle->GetPosition();

    bool ahead;
    if (vehicle->m_matrix) {
        ahead = Hp177ALeanSideAheadMatrixCpp(&vehicle->m_matrix->GetRight().x, &pedPos.x, &vehPos.x);
    } else {
        ahead = Hp177ALeanSideAheadHeadingCpp(vehicle->m_placement.m_fHeading, &pedPos.x, &vehPos.x);
    }
    field_15 = ahead;

    const float localX = ahead ? maxX + 0.5f : minX - 0.5f;
    const CVector local{localX, 0.0f, 0.0f};
    CVector result;
    Hp65MatrixTimesPointCpp(reinterpret_cast<const float*>(vehicle->m_matrix), &local.x, &result.x);
    return result;
}

// 0x660F60
void CTaskGoToVehicleAndLean::DoTidyUp(CPed* ped) {
    if (m_Vehicle)
        m_Vehicle->vehicleFlags.bHasGangLeaningOn = false;

    if (m_pSubTask)
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
}
