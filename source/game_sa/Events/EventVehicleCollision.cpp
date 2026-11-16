/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleCollision
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

#include "EventVehicleCollision.h"

#include "TaskSimpleGoTo.h"
#include "TaskComplexWalkRoundCar.h"
#include "TaskComplexHitPedWithCar.h"

namespace {
// 0x4AC920 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventVehicleCollision que usan los destructores del EXE. Deja la vtable de CEventVehicleCollision del EXE (0x85AC28), libera la referencia de m_vehicle (+0x14) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall VehicleCollisionDestructorBody(CEventVehicleCollision* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AC28;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}

// 0x4B5930 (lote 190A, sin validar): destructor con borrado de CEventVehicleCollision del EXE (ranura 0 de su vtable 0x85AC28; thiscall, un argumento de pila, `ret 4`, 76 bytes: `push esi; push edi; mov edi,ecx; call 0x4AC920; test byte [esp+0Ch],1; je ..;`
// y, con el bit 0 del BYTE de banderas, el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, como 0x4B5630); devuelve `this`). Como el original: destruye con el cuerpo de la clase (0x4AC920), libera `this`
// con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventVehicleCollision* __fastcall VehicleCollisionScalarDeletingDestructor(CEventVehicleCollision* self, void* /*edx*/, uint8 flags) {
    VehicleCollisionDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventVehicleCollision::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleCollision, 0x85AC28, 16);
    RH_ScopedCategory("Events");
    RH_ScopedVMTInstall(GetEventType, 0x4AC8F0); // lote 201A (sin validar): retorno de constante
    RH_ScopedNamedGlobalInstall(VehicleCollisionDestructorBody, "Destructor", 0x4AC920);
    RH_ScopedNamedGlobalInstall(VehicleCollisionScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B5930); // lote 190A (sin validar)

    RH_ScopedInstall(Constructor, 0x4AC840);
    RH_ScopedVMTInstall(Clone, 0x4B6BC0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B2EE0);
    RH_ScopedVMTInstall(GetLifeTime, 0x4AC900);        // lote 202A (sin validar): ranura 3
    RH_ScopedVMTInstall(GetEventPriority, 0x4AC910);   // lote 202A (sin validar): ranura 2
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4AC980);  // lote 202A (sin validar): ranura 11
}

// 0x4AC900 (lote 202A, sin validar; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto (ranura 3 de la vtable 0x85AC28).
int32 CEventVehicleCollision::GetLifeTime() {
    return 0;
}

// 0x4AC910 (lote 202A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,3Ah`): prioridad 58; no lee el objeto (ranura 2).
int32 CEventVehicleCollision::GetEventPriority() const {
    return 58;
}

// 0x4AC980 (lote 202A, sin validar; thiscall, un argumento de pila, `ret 4`, 5 bytes: `mov al,1`): siempre verdadero (solo se define AL); no lee el objeto ni el evento dado (ranura 11).
bool CEventVehicleCollision::TakesPriorityOver(const CEvent& /*refEvent*/) {
    return true;
}

CEventVehicleCollision::CEventVehicleCollision(int16 pieceType, float damageIntensity, CVehicle* vehicle, const CVector& collisionImpactVelocity, const CVector& collisionPosition, int8 moveState, int16 evadeType)
{
    m_pieceType               = pieceType;
    m_evadeType               = evadeType;
    m_fDamageIntensity        = damageIntensity;
    m_vehicle                 = vehicle;
    m_impactNormal = collisionImpactVelocity;
    m_impactPos       = collisionPosition;
    m_moveState               = moveState;
    m_DirectionToWalkRoundCar                  = 0;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventVehicleCollision::~CEventVehicleCollision()
{
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4AC840
CEventVehicleCollision* CEventVehicleCollision::Constructor(int16 pieceType, float damageIntensity, CVehicle* vehicle, const CVector& collisionImpactVelocity, const CVector& collisionPosition, int8 moveState, int16 evadeType)
{
    this->CEventVehicleCollision::CEventVehicleCollision(pieceType, damageIntensity, vehicle, collisionImpactVelocity, collisionPosition, moveState, evadeType);
    return this;
}

// 0x4B2EE0
bool CEventVehicleCollision::AffectsPed(CPed* ped)
{
    if (!ped->IsAlive())
        return false;

    if (m_evadeType)
        return true;

    if (!m_vehicle)
        return false;

    if (ped->bInVehicle
        || m_vehicle->IsBoat()
        || -DotProduct(m_impactNormal, ped->GetForward()) < 0.35f)
    {
        return false;
    }

    CTask* pSimplestActiveTask = ped->GetTaskManager().GetSimplestActiveTask();
    if (pSimplestActiveTask && CTask::IsGoToTask(pSimplestActiveTask)) {
        auto* pGoToTask = static_cast<CTaskSimpleGoTo*>(pSimplestActiveTask);
        int32 hitSide = CPedGeometryAnalyser::ComputeEntityHitSide(*ped, *m_vehicle);
        if (hitSide == CPedGeometryAnalyser::ComputeEntityHitSide(pGoToTask->m_vecTargetPoint, *m_vehicle)) {
            if (!m_vehicle->m_pTowingVehicle && !m_vehicle->m_pVehicleBeingTowed)
                return false;

            CVector boundingBoxPlanes[4];
            float planes_D[4];
            CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(ped->GetPosition().z, *m_vehicle, &boundingBoxPlanes, planes_D);
            int32 targetPointInPlanes = 0, pedInPlanes = 0;
            for (int32 i = 0; i < 4; i++) {
                CVector& plane = boundingBoxPlanes[i];
                if (DotProduct(pGoToTask->m_vecTargetPoint, plane) + planes_D[i] < 0.0f)
                    ++targetPointInPlanes;
                if (DotProduct(ped->GetPosition(), plane) + planes_D[i] < 0.0f)
                    ++pedInPlanes;
            }
            if (targetPointInPlanes != 4 && pedInPlanes != 4)
                return false;
        }
    }

    CTask* pActiveTask = ped->GetTaskManager().GetActiveTask();
    if (pActiveTask) {
        if (pActiveTask->GetTaskType() == TASK_COMPLEX_WALK_ROUND_CAR) {
            auto pTaskWalkRoundCar = reinterpret_cast<CTaskComplexWalkRoundCar*>(pActiveTask);
            CVehicle* walkRoundVehicle = pTaskWalkRoundCar->m_Veh;
            if (walkRoundVehicle == m_vehicle)
                return false;
            if ((m_vehicle->m_pVehicleBeingTowed && m_vehicle->m_pVehicleBeingTowed == walkRoundVehicle) ||
                (m_vehicle->m_pTowingVehicle && m_vehicle->m_pTowingVehicle == walkRoundVehicle))
            {
                // Como el original: pasa los bits 2-5 del byte 0xD con signo ((int8)(b << 2) >> 4) y devuelve verdadero.
                const auto dir = static_cast<int8>(reinterpret_cast<const uint8*>(pTaskWalkRoundCar)[0xD] << 2) >> 4;
                pTaskWalkRoundCar->SetNewVehicle(m_vehicle, static_cast<uint8>(dir));
            }
        }
        else {
            auto pTaskHitPedWithCar = reinterpret_cast<CTaskComplexHitPedWithCar*>(pActiveTask);
            if (pActiveTask->GetTaskType() == TASK_COMPLEX_HIT_PED_WITH_CAR && pTaskHitPedWithCar->m_Veh == m_vehicle)
                return false;
        }
    }

    return true;
}

// 0x4AC8F0 (lote 201A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,1`): EVENT_VEHICLE_COLLISION (1); no lee el objeto (ranura 1 de la vtable 0x85AC28).
eEventType CEventVehicleCollision::GetEventType() const {
    return EVENT_VEHICLE_COLLISION;
}
