/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWander
 * Reconstrucción: COMPLETA — 21/21 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 21/21 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWander.h"
#include "X87Math.h"

#include "TaskSimpleCarDriveTimed.h"
#include "TaskSimpleScratchHead.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleRunAnim.h"

#include "TaskComplexObserveTrafficLightsAndAchieveHeading.h"
#include "TaskComplexCrossRoadLookAndAchieveHeading.h"
#include "TaskComplexWanderProstitute.h"
#include "TaskComplexWanderStandard.h"
#include "TaskComplexWanderCriminal.h"
#include "TaskComplexWanderMedic.h"
#include "TaskComplexWanderCop.h"
#include "TaskComplexLeaveCar.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

// Espacio con nombre (no anonimo) para que CTaskComplexWanderMedic (0x658850) y CTaskComplexWanderStandard (0x48E600) (lote 142A) llamen a este mismo cuerpo sin duplicarlo.
namespace CTaskComplexWanderBody {
// 0x669D20 (lote 115A, banco Fn115A): cuerpo no virtual de ~CTaskComplexWander del EXE (thiscall, sin argumentos de pila, `ret`)
// Deja la vtable de CTaskComplexWander del EXE (0x86FE84) y salta a CTaskComplex::Destructor (0x61A3D0). El EXE lo envuelve en un marco SEH que solo desenrolla.
void __fastcall Destructor(CTaskComplexWander* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86FE84;
    CTaskComplexBody::Destructor(self, nullptr);
}
} // namespace CTaskComplexWanderBody

static CTaskComplexWander* __fastcall WanderScalarDeletingDestructor(CTaskComplexWander* self, void* edx, uint8 flags); // 0x66F4A0 (mas abajo)

void CTaskComplexWander::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWander, 0x86FE84, 15);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexWanderBody::Destructor, "Destructor", 0x669D20);
    RH_ScopedVMTInstall(GetTaskType, 0x460CD0); // lote 184A (sin validar): ranura 4; la comparten las vtables de todas las tareas de paseo del EXE

    RH_ScopedInstall(Constructor, 0x66F450);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x674140);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6740E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x674C30);
    RH_ScopedVMTInstall(UpdateDir, 0x669DA0);
    RH_ScopedVMTInstall(UpdatePathNodes, 0x669ED0);
    RH_ScopedInstall(CreateSubTask, 0x671CB0);
    RH_ScopedInstall(ComputeTargetPos, 0x669F60);
    RH_ScopedInstall(ComputeTargetHeading, 0x66F530);
    RH_ScopedInstall(ValidNodes, 0x669F30);
    RH_ScopedInstall(ScanForBlockedNodes, 0x674560);
    RH_ScopedOverloadedInstall(ScanForBlockedNode, "", 0x671EF0, bool(CTaskComplexWander::*)(CPed*, const CNodeAddress&));
    RH_ScopedOverloadedInstall(ScanForBlockedNode, "1", 0x66F4C0, bool(CTaskComplexWander::*)(const CVector&, CEntity*));
    RH_ScopedInstall(GetWanderTaskByPedType, 0x673D00);
    RH_ScopedInstall(GetWanderTaskByPedTypeId, 0x673EF0); // lote 194A (sin validar)
    RH_ScopedInstall(GetDistSqOfClosestPathNodeToPed, 0x669FF0); // lote 194A (sin validar)
    RH_ScopedNamedGlobalInstall(WanderScalarDeletingDestructor, "ScalarDeletingDestructor", 0x66F4A0); // lote 194A (sin validar)
    RH_ScopedInstall(RequestDir, 0x669D30);
    RH_ScopedInstall(RequestNodesFrom, 0x669D50);
}

// 0x460CD0 (lote 184A, sin validar; el listado decia CTaskComplexWanderCop; thiscall, `ret`, 6 bytes: `mov eax,390h`): TASK_COMPLEX_WANDER (912); no lee el objeto. Ranura 4 de la vtable 0x86FE84 de CTaskComplexWander; las
// vtables de CTaskComplexWanderCop (0x859C80), Standard, Medic, Criminal, etc. comparten esta misma direccion.
eTaskType CTaskComplexWander::GetTaskType() const {
    return Type;
}

// 0x669D30 (sin simbolo en el EXE): si la direccion pedida (dword contra el byte actual) es distinta, la guarda y marca m_bNewDir
void CTaskComplexWander::RequestDir(uint32 dir) {
    if ((uint32)m_nDir != dir) {
        m_nDir = (uint8)dir;
        m_bNewDir = true;
    }
}

// 0x669D50 (sin simbolo en el EXE): si los nodos (anterior y siguiente, las cuatro palabras) de `from` difieren de los propios, copia los dos nodos y la direccion y marca m_bNewNodes
void CTaskComplexWander::RequestNodesFrom(const CTaskComplexWander* from) {
    if (m_LastNode != from->m_LastNode || m_NextNode != from->m_NextNode) {
        m_LastNode = from->m_LastNode;
        m_NextNode = from->m_NextNode;
        m_nDir     = from->m_nDir;
        m_bNewNodes = true;
    }
}
CTaskComplexWander* CTaskComplexWander::Constructor(eMoveState moveState, uint8 dir, bool bWanderSensibly, float fTargetRadius) { this->CTaskComplexWander::CTaskComplexWander(moveState, dir, bWanderSensibly, fTargetRadius); return this; }

// 0x66F450
CTaskComplexWander::CTaskComplexWander(eMoveState moveState, uint8 dir, bool bWanderSensibly, float fTargetRadius) : CTaskComplex() {
    m_nMoveState = moveState;
    m_nDir = dir;
    m_fTargetRadius = fTargetRadius;
    m_LastNode.m_wAreaId = (uint16)-1; // Como el original: mov word ptr [esi+18h],0FFFFh (m_wNodeId sin escribir)
    m_NextNode.m_wAreaId = (uint16)-1; // Como el original: mov word ptr [esi+1Ch],0FFFFh
    m_nFlags = (uint8)((m_nFlags & 0xF0) | ((uint8)bWanderSensibly & 1)); // Como el original: borra los bits 0-3 y pone el 0
    m_nLastUpdateDirFrameCount = 0;
}

// 0x674140
CTask* CTaskComplexWander::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_SCRATCH_HEAD: {
        m_nDir++;
        UpdatePathNodes(ped, m_nDir, m_LastNode, m_NextNode, m_nDir);

        if (ValidNodes()) {
            return CreateSubTask(ped, TASK_SIMPLE_GO_TO_POINT);
        }

        return new CTaskSimpleScratchHead();
    }
    case TASK_COMPLEX_CROSS_ROAD_LOOK_AND_ACHIEVE_HEADING:
    case TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING:
        return CreateSubTask(ped, TASK_SIMPLE_GO_TO_POINT);

    case TASK_SIMPLE_CAR_DRIVE_TIMED:
        return CreateFirstSubTask(ped);

    case TASK_COMPLEX_LEAVE_CAR:
        if (ped->bInVehicle) {
            return new CTaskSimpleCarDriveTimed(ped->m_pVehicle, 2000);
        }
        return CreateFirstSubTask(ped);

    case TASK_COMPLEX_SEQUENCE:
    case TASK_SIMPLE_GO_TO_POINT: {
        if (m_bAllNodesBlocked) {
            m_bAllNodesBlocked = false;
            return new CTaskSimpleScratchHead();
        }

        UpdateDir(ped);
        const auto previousLastNode = m_LastNode;
        UpdatePathNodes(ped, m_nDir, m_LastNode, m_NextNode, m_nDir);

        if (m_NextNode == previousLastNode) {
            auto* sequence = new CTaskComplexSequence();
            sequence->AddTask(new CTaskSimpleStandStill(500, false, false, 8.0f));
            sequence->AddTask(new CTaskSimpleRunAnim(ped->m_nAnimGroup, ANIM_ID_ROADCROSS, 4.0f, false));
            sequence->AddTask(new CTaskSimpleScratchHead());
            CVector outTargetPos;
            ComputeTargetPos(ped, outTargetPos, m_NextNode);
            sequence->AddTask(new CTaskSimpleGoToPoint(m_nMoveState, outTargetPos, m_fTargetRadius, false, false));
            return sequence;
        }

        if (!ValidNodes()) {
            return new CTaskSimpleScratchHead();
        }

        if (m_bWanderSensibly && ThePaths.TestForPedTrafficLight(m_LastNode, m_NextNode)) {
            return CreateSubTask(ped, TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING);
        }
        if (m_bWanderSensibly && ThePaths.TestCrossesRoad(m_LastNode, m_NextNode)) {
            return new CTaskComplexCrossRoadLookAndAchieveHeading(2000, ComputeTargetHeading(ped));
        }

        return CreateSubTask(ped, TASK_SIMPLE_GO_TO_POINT);
    }
    case TASK_COMPLEX_IN_AIR_AND_LAND:
    default:
        return nullptr;
    }
}

// 0x6740E0


CTask* CTaskComplexWander::CreateFirstSubTask(CPed* ped) {
    if (ped->bInVehicle) {
        return CreateSubTask(ped, TASK_COMPLEX_LEAVE_CAR);
    }

    UpdateDir(ped);
    UpdatePathNodes(ped, m_nDir, m_LastNode, m_NextNode, m_nDir);
    return CreateSubTask(ped, TASK_SIMPLE_GO_TO_POINT);
}

// 0x674C30


CTask* CTaskComplexWander::ControlSubTask(CPed* ped) {
    // Como el original: el tipo de la subtarea se vuelve a pedir en cada comprobacion (llamada virtual)
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_LEAVE_CAR || m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_DRIVE_TIMED) {
        return m_pSubTask;
    }

    // Como el original: guarda m_pSubTask antes de comprobar los flags y lo devuelve aunque ScanForStuff lo cambie
    CTask* subTask = m_pSubTask;
    if (m_bNewDir && subTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
        m_bNewDir = false;
        m_NextNode.ResetAreaId();
        m_LastNode.ResetAreaId();
        return CreateFirstSubTask(ped);
    } else {
        if (m_bNewNodes && m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
            m_bNewNodes = false;
            CVector outTargetPos;
            ComputeTargetPos(ped, outTargetPos, m_NextNode);
            static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->UpdatePoint(outTargetPos, 0.5f, false);
            subTask = m_pSubTask; // Como el original: aquí sí vuelve a leer la subtarea
        } else {
            ScanForBlockedNodes(ped);
            ScanForStuff(ped);
        }

        if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
            static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->m_moveState = m_nMoveState;
            if (m_nMoveState <= PEDMOVE_WALK) {
                ped->Say(CTX_GLOBAL_CHAT, 0, 0.1f); // Como el original: probabilidad 0.1
            }
        }

        if (ped->GetIntelligence()->m_AnotherStaticCounter > 30) {
            if (m_pSubTask->MakeAbortable(ped)) {
                return CreateSubTask(ped, TASK_SIMPLE_SCRATCH_HEAD);
            }
        }
    }
    return subTask;
}

// 0x669DA0


void CTaskComplexWander::UpdateDir(CPed* ped) {
    uint8 newDir = m_nDir;
    if (m_NextNode.IsAreaValid()) {
        const CPathNode* pathNodes = ThePaths.m_pPathNodes[m_NextNode.m_wAreaId];
        if (pathNodes) {
            const CPathNode* pathNode = &pathNodes[m_NextNode.m_wNodeId];
            if (pathNode) {
                if (pathNode->m_nNumLinks >= 3u && CTimer::GetFrameCounter() != m_nLastUpdateDirFrameCount && m_bWanderSensibly) {
                    m_nLastUpdateDirFrameCount = CTimer::GetFrameCounter();
                    uint8 remainder = (3 * CTimer::GetFrameCounter() + ped->m_nRandomSeed) % 100;
                    if (remainder <= 90u) {
                        if (remainder > 80u) {
                            newDir = (newDir + 10) % 8;
                        }
                    } else {
                        newDir = (m_nDir + 6) % 8;
                    }
                }
            }
        }
    }

    if (newDir != m_nDir) {
        // Como el original: trabaja sobre copias locales de los nodos (los miembros no cambian) y outDir
        // reutiliza la ranura del argumento ped en la pila, donde 0x669DDE guarda m_NextNode al leer el nodo
        // (solo se llega aquí por esa rama): si UpdatePathNodes no lo escribe vale el byte bajo de m_wAreaId
        CNodeAddress originNode = m_LastNode, targetNode = m_NextNode;
        uint8 outDir = (uint8)m_NextNode.m_wAreaId;
        UpdatePathNodes(ped, newDir, originNode, targetNode, outDir);

        uint8 currentDir = m_nDir;
        uint8 theDir = outDir;
        if (outDir <= currentDir) {
            theDir = m_nDir;
        }

        if (theDir == currentDir) {
            currentDir = outDir;
        }

        if (((theDir - currentDir + 8) % 8) <= 3u) {
            m_nDir = newDir;
        }
    }
}

// 0x669ED0

// 0x0
void CTaskComplexWander::UpdatePathNodes(const CPed* ped, uint8 dir, CNodeAddress& originNode, CNodeAddress& targetNode, uint8& outDir) {
    originNode = targetNode;
    targetNode.m_wAreaId = (uint16)-1;
    const CVector& pos = ped->GetPosition();
    ThePaths.FindNextNodeWandering(PATH_TYPE_PED, pos, &originNode, &targetNode, dir, &outDir);
}

// 0x671CB0
CTask* CTaskComplexWander::CreateSubTask(CPed* ped, eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_DRIVE_TIMED: {
        return new CTaskSimpleCarDriveTimed(ped->m_pVehicle, 2000);
    }
    case TASK_SIMPLE_GO_TO_POINT: {
        CVector outTargetPos;
        ComputeTargetPos(ped, outTargetPos, m_NextNode);
        return new CTaskSimpleGoToPoint(m_nMoveState, outTargetPos, m_fTargetRadius, false, false);
    }
    case TASK_FINISHED: {
        return nullptr;
    }
    case TASK_COMPLEX_LEAVE_CAR: {
        return new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0, true, false);
    }
    case TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING: {
        return new CTaskComplexObserveTrafficLightsAndAchieveHeading(2000, ComputeTargetHeading(ped));
    }
    case TASK_COMPLEX_CROSS_ROAD_LOOK_AND_ACHIEVE_HEADING: {
        return new CTaskComplexCrossRoadLookAndAchieveHeading(2000, ComputeTargetHeading(ped));
    }
    case TASK_SIMPLE_SCRATCH_HEAD: {
        return new CTaskSimpleScratchHead();
    }
    default: {
        return nullptr;
    }
    }
}

// 0x66F530
float CTaskComplexWander::ComputeTargetHeading(CPed* ped) {
    auto position = ThePaths.TakeWidthIntoAccountForWandering(m_NextNode, ped->m_nRandomSeed);
    position -= ped->GetPosition();
    float radianAngle = CGeneral::GetRadianAngleBetweenPoints(position.x, position.y, 0.0f, 0.0f);
    return CGeneral::LimitRadianAngle(radianAngle);
}

// 0x669F60
void CTaskComplexWander::ComputeTargetPos(const CPed* ped, CVector& outTargetPos, const CNodeAddress& targetNodeAddress) {
    outTargetPos = ThePaths.TakeWidthIntoAccountForWandering(targetNodeAddress, ped->m_nRandomSeed);
    outTargetPos.z += 1.0f;
}

// 0x669F30
bool CTaskComplexWander::ValidNodes() const {
    // Como el original: solo comprueba m_wAreaId != 0xFFFF (no m_wNodeId)
    return m_NextNode.IsAreaValid() && m_LastNode.IsAreaValid() && m_NextNode != m_LastNode;
}

// 0x674560
void CTaskComplexWander::ScanForBlockedNodes(CPed* ped) {
    // Como el original: solo comprueba m_wAreaId
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT && m_NextNode.IsAreaValid()) {
        if (ScanForBlockedNode(ped, m_NextNode)) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);

            // Como el original: UpdatePathNodes trabaja sobre copias locales (los miembros no cambian),
            // outDir empieza con m_nDir y el nodo resultante se compara con m_LastNode
            CNodeAddress originNode = m_LastNode, targetNode = m_NextNode;
            uint8 outDir = m_nDir;
            UpdatePathNodes(ped, m_nDir, originNode, targetNode, outDir);
            if (ScanForBlockedNode(ped, targetNode) || targetNode == m_LastNode) {
                m_bAllNodesBlocked = true;
            }
        }
    }
}

// 0x671EF0
bool CTaskComplexWander::ScanForBlockedNode(CPed* ped, const CNodeAddress& targetNodeAddress) {
    auto outVec = ThePaths.TakeWidthIntoAccountForWandering(targetNodeAddress, ped->m_nRandomSeed);
    outVec.z += 1.0f; // Como el original: suma 1.0 a z (no afecta al resultado 2D)
    // Como el original: fcompp + test ah,5/jnp -> NaN sigue adelante
    if (!(DistanceBetweenPointsSquared2D(outVec, ped->GetPosition()) > sq(3.0f))) {
        CPed* closestPed = ped->GetIntelligence()->GetPedScanner().GetClosestPedInRange();
        if (ScanForBlockedNode(outVec, closestPed)) {
            return true;
        }

        CVehicle* closestVehicle = ped->GetIntelligence()->GetVehicleScanner().GetClosestVehicleInRange();
        if (ScanForBlockedNode(outVec, closestVehicle)) {
            return true;
        }
    }
    return false;
}

// 0x66F4C0
bool CTaskComplexWander::ScanForBlockedNode(const CVector& position, CEntity* entity) {
    if (!entity)
        return false;

    float fRadius = entity->GetModelInfo()->GetColModel()->GetBoundRadius() + 1.0f;
    if (DistanceBetweenPointsSquared2D(entity->GetPosition(), position) < sq(fRadius))
        return true;

    return false;
}

// 0x673D00
CTaskComplexWander* CTaskComplexWander::GetWanderTaskByPedType(CPed* ped) {
    // Como el original: la dirección aleatoria se pide después de reservar la tarea (0x407180 o su copia
    // en línea rand() * (1/32768) * 8, truncada), no con la interpolación 1/32767 de GetRandomNumberInRange<int>
    switch (ped->m_nPedType) {
    case PED_TYPE_COP:        return new CTaskComplexWanderCop(PEDMOVE_WALK, (uint8)CGeneral::GetRandomIntInRange(0, 8));
    case PED_TYPE_MEDIC:      return new CTaskComplexWanderMedic(PEDMOVE_WALK, (uint8)CGeneral::GetRandomIntInRange(0, 8));
    case PED_TYPE_CRIMINAL:   return new CTaskComplexWanderCriminal(PEDMOVE_WALK, (uint8)CGeneral::GetRandomIntInRange(0, 8));
    case PED_TYPE_PROSTITUTE: return new CTaskComplexWanderProstitute(PEDMOVE_WALK, (uint8)CGeneral::GetRandomIntInRange(0, 8));
    default:                  return new CTaskComplexWanderStandard(PEDMOVE_WALK, (uint8)CGeneral::GetRandomIntInRange(0, 8));
    }
}

// 0x669FF0 (lote 194A, sin validar; OG name unknown; thiscall, peaton en la pila, `ret 4`, 265 bytes; la llama el EXE en 0x662467): distancia al cuadrado del peaton al nodo mas cercano de los dos guardados.
// Como el original (no es la version con rangos que habia): para cada nodo, SOLO se comprueba el area (+0x1C/+0x1E el siguiente, +0x18/+0x1A el anterior; area 0xFFFF o area sin cargar -> 999999.0f = 0x86FEC0,
// sin mirar el id del nodo) y la distancia es (dz*dz + dy*dy) + dx*dx de (posicion del nodo - posicion del peaton) (CPathNode::GetPosition 0x420A10; x87 a 24 bits = floats). Primero se calcula el siguiente
// (guardado como float) y despues el anterior; devuelve el anterior si es ESTRICTAMENTE menor (comparacion ordenada: empate o NaN devuelven el siguiente).
float CTaskComplexWander::GetDistSqOfClosestPathNodeToPed(CPed* ped) {
    static_assert(sizeof(CPathNode) == 0x1C);
    // Posicion del nodo (CPathNode::GetPosition, 0x420A10) o nulo si el area no es valida o no esta cargada
    const auto NodePos = [](const CNodeAddress& node, CVector& out) -> const float* {
        if (node.m_wAreaId != uint16(-1) && ThePaths.m_pPathNodes[node.m_wAreaId]) {
            out = ThePaths.m_pPathNodes[node.m_wAreaId][node.m_wNodeId].GetPosition();
            return &out.x;
        }
        return nullptr;
    };
    CVector      nextPos, lastPos;
    const float* const next = NodePos(m_NextNode, nextPos);
    const float* const last = NodePos(m_LastNode, lastPos);
    // Las distancias, el guardado de la del siguiente como float y la comparacion con la del anterior van en la x87 (24 bits, exponente ampliado): X87Math.cpp
    return Pf194ClosestDistSqCpp(next, last, &ped->GetPosition().x);
}

// 0x66F4A0 (lote 194A, sin validar): destructor con borrado de CTaskComplexWander del EXE (ranura 0 de su vtable 0x86FE84; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x669D20;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x669D20, con hook), libera `this` con CTask::operator
// delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
static CTaskComplexWander* __fastcall WanderScalarDeletingDestructor(CTaskComplexWander* self, void* /*edx*/, uint8 flags) {
    CTaskComplexWanderBody::Destructor(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

// 0x673EF0 (lote 194A, sin validar; sin llamadores ni referencias en el EXE: codigo muerto; static, un argumento de pila (el tipo de peaton, NO el peaton), `ret`, 448 bytes con marco SEH que solo libera la
// memoria si un constructor lanzara): copia exacta de GetWanderTaskByPedType (0x673D00) que recibe el tipo en vez del peaton. Tabla de saltos 0x6740B0 / indices 0x6740C4 sobre (tipo - 6) sin signo (> 16 -> por
// defecto): 6 = PED_TYPE_COP -> CTaskComplexWanderCop (0x50 bytes, constructor 0x460C80 con (4, dir)); 18 = PED_TYPE_MEDIC -> CTaskComplexWanderMedic (0x28, 0x658770 con (4, dir, true)); 20 = PED_TYPE_CRIMINAL ->
// CTaskComplexWanderCriminal (0x38, 0x48E610); 22 = PED_TYPE_PROSTITUTE -> CTaskComplexWanderProstitute (0x3C, 0x672690); cualquier otro -> CTaskComplexWanderStandard (0x38, 0x48E4F0). Todas reservan con
// CTask::operator new (0x61A5A0; nulo -> devuelve nulo), piden la direccion DESPUES de reservar y llaman al constructor con PEDMOVE_WALK (4). Las cuatro primeras piden la direccion con 0x407180(0, 8); la de
// por defecto usa en linea (rand() del CRT del EXE, 0x821B1E, & 0xFFFF) * (1/32768) * 8.0f truncado con _ftol2 (0x821B40).
CTaskComplexWander* CTaskComplexWander::GetWanderTaskByPedTypeId(int32 pedType) {
    switch (uint32(pedType) - uint32(PED_TYPE_COP)) {
    case uint32(PED_TYPE_COP) - uint32(PED_TYPE_COP): {
        void* const memory = CTask::operator new(sizeof(CTaskComplexWanderCop));
        if (!memory) {
            return nullptr;
        }
        const uint8 dir = uint8(CGeneral::GetRandomIntInRange(0, 8));
        return static_cast<CTaskComplexWanderCop*>(memory)->Constructor(PEDMOVE_WALK, dir);
    }
    case uint32(PED_TYPE_MEDIC) - uint32(PED_TYPE_COP): {
        void* const memory = CTask::operator new(sizeof(CTaskComplexWanderMedic));
        if (!memory) {
            return nullptr;
        }
        const uint8 dir = uint8(CGeneral::GetRandomIntInRange(0, 8));
        return static_cast<CTaskComplexWanderMedic*>(memory)->Constructor(PEDMOVE_WALK, dir, true);
    }
    case uint32(PED_TYPE_CRIMINAL) - uint32(PED_TYPE_COP): {
        void* const memory = CTask::operator new(sizeof(CTaskComplexWanderCriminal));
        if (!memory) {
            return nullptr;
        }
        const uint8 dir = uint8(CGeneral::GetRandomIntInRange(0, 8));
        return static_cast<CTaskComplexWanderCriminal*>(memory)->Constructor(PEDMOVE_WALK, dir, true);
    }
    case uint32(PED_TYPE_PROSTITUTE) - uint32(PED_TYPE_COP): {
        void* const memory = CTask::operator new(sizeof(CTaskComplexWanderProstitute));
        if (!memory) {
            return nullptr;
        }
        const uint8 dir = uint8(CGeneral::GetRandomIntInRange(0, 8));
        return static_cast<CTaskComplexWanderProstitute*>(memory)->Constructor(PEDMOVE_WALK, dir, true);
    }
    default: {
        void* const memory = CTask::operator new(sizeof(CTaskComplexWanderStandard));
        if (!memory) {
            return nullptr;
        }
        const int32 random = reinterpret_cast<int(__cdecl*)()>(0x821B1E)() & 0xFFFF;
        const uint8 dir = uint8(int32(float(random) * 3.0517578125e-05f * 8.0f));
        return static_cast<CTaskComplexWanderStandard*>(memory)->Constructor(PEDMOVE_WALK, dir, true);
    }
    }
}
