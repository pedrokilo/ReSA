/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoPickUpEntity
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGoPickUpEntity.h"

#include "AnimManager.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimplePickUpEntity.h"
#include "TaskManager.h"
#include "ColModel.h"
#include "Timer.h"
#include "ModelInfo.h"
#include "Entity/Physical.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "Streaming.h"
#include "Pad.h"
#include "General.h"
#include "Entity/Ped/PlayerPed.h"
#include "X87Math.h"

static_assert(TASK_COMPLEX_GO_PICKUP_ENTITY == 0x136); // GetTaskType del EXE (0x691A40)

void CTaskComplexGoPickUpEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoPickUpEntity, 0x870B98, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6919C0);
    RH_ScopedInstall(Destructor, 0x691A50);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x691AE0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x693610);
    RH_ScopedVMTInstall(ControlSubTask, 0x691D50);
}

// 0x6919C0
CTaskComplexGoPickUpEntity::CTaskComplexGoPickUpEntity(CEntity* entity, int32 animGroupId) : CTaskComplex() {
    m_pEntity = entity;
    m_vecPosition = CVector{ -999.0f, 0.0f, 0.0f };
    m_vecPickupPosition = CVector{ -999.0f, 0.0f, 0.0f };
    m_nAnimGroupId = animGroupId;
    if (m_pEntity) {
        m_pEntity->RegisterReference(&m_pEntity);
    }
    m_nTimePassedSinceLastSubTaskCreatedInMs = 0;
    m_bAnimBlockReferenced = false;
}

// 0x691A50
CTaskComplexGoPickUpEntity::~CTaskComplexGoPickUpEntity() {
    if (m_pEntity) {
        m_pEntity->CleanUpOldReference(&m_pEntity);
    }
    if (m_bAnimBlockReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(static_cast<AssocGroupId>(m_nAnimGroupId)));
        m_bAnimBlockReferenced = false;
    }
}

// 0x692C80: CTask::operator new (0x61A5A0) devuelve nulo si el pool esta agotado y el EXE lo comprueba antes de construir.
CTask* CTaskComplexGoPickUpEntity::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskComplexGoPickUpEntity));
    return memory ? ::new (memory) CTaskComplexGoPickUpEntity(m_pEntity, m_nAnimGroupId) : nullptr;
}

// Redondeo a 24 bits de mantisa (a la mas cercana, empate al par) de un double con exponente sin limite, como la x87 con la palabra de control del juego (PC = 24 bits).
// Solo por enteros: no usa float (SSE con FTZ vaciaria un resultado desnormal). Las sumas de dos floats en double y este redondeo equivalen a la suma de 24 bits (el doble redondeo es inocuo).
static double X87Round24(double value) {
    if (!(std::fabs(value) < HUGE_VAL) || value == 0.0) {
        return value; // NaN, infinito o cero
    }
    uint64 bits = std::bit_cast<uint64>(value);
    const uint64 low = bits & 0x1FFFFFFFull; // 29 bits que sobran de los 52 de la mantisa
    bits &= ~0x1FFFFFFFull;
    if (low > 0x10000000ull || (low == 0x10000000ull && (bits & (1ull << 29)))) {
        bits += 1ull << 29; // el acarreo llega al exponente
    }
    return std::bit_cast<double>(bits);
}

// 0x691AE0
// Como el original: sin entidad no hay subtarea; el tipo de la subtarea actual decide la siguiente (la subtarea no se comprueba, el EXE la desreferencia).
// Constantes de mezcla del EXE: 0x870A80 (0.6f), 0x870A84 (0x3E888889), 0x870A88 (0.2f) y 0x866CAC (0.55f).
CTask* CTaskComplexGoPickUpEntity::CreateNextSubTask(CPed* ped) {
    if (!m_pEntity) {
        return nullptr;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PICKUP_ENTITY: { // 0x134 -> 0x691CC3
        m_nTimePassedSinceLastSubTaskCreatedInMs = CTimer::GetTimeInMS();
        if (ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
            return nullptr;
        }
        // CTask::operator new (0x61A5A0) devuelve nulo con el pool agotado; el EXE pasa ese nulo a SetTaskSecondary sin comprobarlo.
        void* const memory = CTask::operator new(sizeof(CTaskSimpleHoldEntity));
        CTask* const hold = memory
            ? ::new (memory) CTaskSimpleHoldEntity(m_pEntity, &m_vecPosition, 6, 1, static_cast<AnimationId>(0x13E), static_cast<AssocGroupId>(0x51), false)
            : nullptr;
        ped->GetTaskManager().SetTaskSecondary(hold, TASK_SECONDARY_PARTIAL_ANIM);
        return nullptr;
    }
    case TASK_SIMPLE_ACHIEVE_HEADING: { // 0x386 -> 0x691BCC
        float blend = std::bit_cast<float>(0x3F19999Au); // [0x870A80]
        if (m_nAnimGroupId == 0x51) {
            const CVector& entityPos = m_pEntity->GetPosition();
            const float    colMinZ   = m_pEntity->GetColModel()->m_boundBox.m_vecMin.z; // 0x535300; fld [colmodel+8]
            const float    pedZ      = ped->GetPosition().z;
            // fadd [pos+8] en la x87 a 24 bits con exponente extendido: la suma no se guarda como float (un resultado desnormal o que desborda el float se conserva, y SSE con FTZ lo vaciaria).
            const double top = X87Round24(double(colMinZ) + double(entityPos.z));
            if (top > double(pedZ)) {
                m_nAnimGroupId = 0x53;
                blend = std::bit_cast<float>(0x3E4CCCCDu); // [0x870A88]
            } else if (double(pedZ) < X87Round24(double(std::bit_cast<float>(0x3F0CCCCDu)) + top)) { // 0x866CAC (0.55f) + top
                m_nAnimGroupId = 0x52;
                blend = std::bit_cast<float>(0x3E888889u); // [0x870A84]
            }
        }
        void* const memory = CTask::operator new(sizeof(CTaskSimplePickUpEntity));
        if (!memory) {
            return nullptr; // El EXE escribe la posicion en la direccion 0x40 (nulo + 0x40) y se cae; aqui se devuelve nulo.
        }
        auto* const pickUp = ::new (memory) CTaskSimplePickUpEntity(m_pEntity, &m_vecPosition, 6, 1, static_cast<AnimationId>(0x13D), static_cast<AssocGroupId>(m_nAnimGroupId), blend);
        std::memcpy(&pickUp->m_vecPickuposn, &m_vecPickupPosition, sizeof(CVector)); // tres dwords (ecx/edx/esi)
        return pickUp;
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x387 -> 0x691B2E
        const CVector& pedPos    = ped->GetPosition();
        const CVector& entityPos = m_pEntity->GetPosition();
        const float    dy        = AutoPreSubCpp(entityPos.y, pedPos.y); // fld; fsub; fstp dword en la x87 (conserva los desnormales)
        const float    dx        = AutoPreSubCpp(entityPos.x, pedPos.x);
        void* const memory = CTask::operator new(sizeof(CTaskSimpleAchieveHeading));
        if (!memory) {
            return nullptr;
        }
        // fpatan(-dx, dy) a precision completa, guardado como float
        return ::new (memory) CTaskSimpleAchieveHeading(TrigAtan2Cpp(-dx, dy), 1.0f, std::bit_cast<float>(0x3A83126Fu));
    }
    default:
        return nullptr;
    }
}

// 0x693610 (0x693610..0x6939CB, `ret 4`): orienta el objeto hacia el peaton, calcula el punto de recogida junto a su caja de colision y devuelve
// CTaskComplexGoToPointAndStandStill(4, punto, 0.2f, 0.0f, false, true). Sin entidad devuelve nulo (antes no escribe nada). Los productos escalares siguen el orden
// de sumas del EXE ((dz*z + dy*y) + dx*x); la matriz se pide por separado en cada uso (cada vez puede reservarla, como el codigo en linea del EXE).
CTask* CTaskComplexGoPickUpEntity::CreateFirstSubTask(CPed* ped) {
    CEntity* const entity = m_pEntity;
    if (!entity) {
        return nullptr;
    }
    m_nTimePassedSinceLastSubTaskCreatedInMs = CTimer::m_snTimeInMilliseconds;

    // Como el original: vector del peaton al objeto (peaton - objeto), posicion de la matriz si existe (+0x30) o del placement (+4); la aritmetica va en X87Math.cpp (x87 a 24 bits, sin FTZ)
    float delta[3]; // dx, dy, dz
    GoPickUpDeltaCpp(&ped->GetPosition().x, &entity->GetPosition().x, delta);
    const float dx = delta[0];
    const float dy = delta[1];
    const float dz = delta[2];

    // Como el original: movsx del indice del modelo (ms_modelInfoPtrs 0xA9B0C8) -> CColModel (+0x14) -> caja (min +0, max +0xC)
    const CBoundingBox& box = CModelInfo::ms_modelInfoPtrs[(int16)entity->m_nModelIndex]->GetColModel()->GetBoundingBox();
    m_vecPosition.x = -0.2f;                                   // 0x870A74
    GoPickUpNegMinusCpp(box.m_vecMin.y, 0.2f, &m_vecPosition.y); // -min.y - 0.2f (0x870A78)
    m_vecPosition.z = -box.m_vecMin.z;

    // Como el original: se orienta el objeto por su lado mas largo (2 * maxY < maxX, luego 2 * maxX < maxY, si no por la matriz)
    if (GoPickUpDoubleLessCpp(box.m_vecMax.y, box.m_vecMax.x)) {
        entity->SetHeading(TrigAtan2Cpp(-dx, dy));
    } else if (GoPickUpDoubleLessCpp(box.m_vecMax.x, box.m_vecMax.y)) {
        entity->SetHeading(Atan2MinusCpp(-dx, dy, std::bit_cast<float>(0x3FC90FDBu))); // 0x858FE4, fpatan menos pi/2 con un solo redondeo
    } else if (entity->GetMatrix().GetUp().z < 0.9f) {                                  // 0x858C20; con NaN no entra
        entity->SetHeading(entity->GetHeading()); // GetHeading guarda el fpatan como float, como `fstp` del EXE
    }

    // Como el original: los objetos se detienen y quedan estaticos
    if (entity->GetIsTypeObject()) {
        entity->AsPhysical()->m_vecMoveSpeed = CVector{ 0.0f, 0.0f, 0.0f };
        entity->AsPhysical()->m_vecTurnSpeed = CVector{ 0.0f, 0.0f, 0.0f };
        entity->SetIsStatic(true);
    }
    entity->UpdateRwMatrix();

    // Como el original: la matriz se pide antes de cada producto escalar; derecha = (dz*z + dy*y) + dx*x guardada como float, frente igual sin guardar (X87Math.cpp)
    const CMatrix& rightMat = entity->GetMatrix();
    const CMatrix& forwardMat = entity->GetMatrix();
    switch (GoPickUpRegionCpp(dz, dy, dx, &rightMat.GetRight().x, &forwardMat.GetForward().x)) {
    case 0: // derecha > |frente|
        m_vecPickupPosition.y = 0.0f;
        m_vecPickupPosition.x = 0.4f + box.m_vecMax.x; // 0x870A7C
        break;
    case 1: // -|frente| > derecha
        m_vecPickupPosition.y = 0.0f;
        m_vecPickupPosition.x = box.m_vecMin.x - 0.4f;
        break;
    case 2: // frente > 0.0f (0x858B50)
        m_vecPickupPosition.x = 0.0f;
        m_vecPickupPosition.y = 0.4f + box.m_vecMax.y;
        break;
    default:
        m_vecPickupPosition.x = 0.0f;
        m_vecPickupPosition.y = box.m_vecMin.y - 0.4f;
        break;
    }
    m_vecPickupPosition.z = box.m_vecMin.z + 1.0f; // 0x858624

    // Como el original: punto de recogida en el mundo (0x59C890) y CTask::operator new (0x61A5A0) con comprobacion de nulo
    const CVector target = entity->GetMatrix() * m_vecPickupPosition;
    void* const memory = CTask::operator new(sizeof(CTaskComplexGoToPointAndStandStill));
    return memory ? ::new (memory) CTaskComplexGoToPointAndStandStill(PEDMOVE_WALK, target, 0.2f, 0.0f, false, true) : nullptr;
}

// 0x691D50
CTask* CTaskComplexGoPickUpEntity::ControlSubTask(CPed* ped) {
    // Como el original: si el grupo de animaciones no esta referenciado se pide el bloque (indice = (bloque - 0xB5D4A0) >> 5, aritmetico) y,
    // si ya esta cargado (byte IsLoaded distinto de 0), se anade la referencia; si no, se solicita al streaming (IFP + 0x63E7, flags 8).
    if (m_nAnimGroupId && !reinterpret_cast<const uint8&>(m_bAnimBlockReferenced)) {
        CAnimBlock* block = StaticRef<CAnimBlendAssocGroup*>(0xB4EA34)[m_nAnimGroupId].m_AnimBlock; // grupos de 0x14 bytes, sin comprobar el indice
        if (!block) {
            block = CAnimManager::GetAnimationBlock(CAnimManager::GetAnimBlockName(static_cast<AssocGroupId>(m_nAnimGroupId)));
        }
        const int32 blockIndex = static_cast<int32>(reinterpret_cast<uintptr_t>(block) - 0xB5D4A0u) >> 5;
        if (reinterpret_cast<const uint8&>(block->IsLoaded)) {
            CAnimManager::AddAnimBlockRef(blockIndex);
            m_bAnimBlockReferenced = true;
        } else {
            CStreaming::RequestModel(IFPToModelId(blockIndex), STREAMING_KEEP_IN_MEMORY);
        }
    }

    CTask* const subTask = m_pSubTask;
    if (!subTask) {
        return m_pSubTask;
    }

    const eTaskType subType = subTask->GetTaskType();
    if (subType == TASK_SIMPLE_PICKUP_ENTITY) { // 0x134
        const auto* const pickUp = static_cast<CTaskSimplePickUpEntity*>(m_pSubTask);
        if (!pickUp->m_pAnimBlendAssociation && !reinterpret_cast<const uint8&>(pickUp->m_bEntityDropped)) {
            if (CTimer::m_snTimeInMilliseconds > m_nTimePassedSinceLastSubTaskCreatedInMs + 8000u) { // 0x8D2FFC
                m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
                return nullptr;
            }
        }
        return m_pSubTask;
    }
    if (subType != TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) { // 0x387
        return m_pSubTask;
    }

    if (CTimer::m_snTimeInMilliseconds > m_nTimePassedSinceLastSubTaskCreatedInMs + 5000u) { // 0x8D2FF8
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
        return nullptr;
    }

    if (!ped->GetPlayerData()) {
        return m_pSubTask;
    }
    const auto* const goTo = static_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask);
    CPad* const pad = static_cast<CPlayerPed*>(ped)->GetPadFromPlayer(); // 0x609560
    if (!pad) {
        return m_pSubTask;
    }

    // Como el original: primero 0x53FD30 (arriba/abajo) y despues 0x53FC90 (izquierda/derecha), ambos convertidos a float exacto
    const float walkUpDown    = static_cast<float>(static_cast<int32>(pad->GetPedWalkUpDown()));
    const float walkLeftRight = static_cast<float>(static_cast<int32>(pad->GetPedWalkLeftRight()));
    const float stickLength   = std::sqrt(walkUpDown * walkUpDown + walkLeftRight * walkLeftRight) * 0.0078125f; // 0x858B88 = 1/128

    // Rumbo hacia el destino: atan2(-(destino.x - ped.x), destino.y - ped.y) (fpatan, un solo redondeo a float)
    const CVector& pos = ped->GetPosition();
    const float targetHeading = PickUpEntityHeadingCpp(&goTo->m_vecTargetPoint.x, &pos.x); // las restas y el fpatan en x87 (X87Math.cpp)

    // Rumbo del stick relativo a la camara (0xB6F178), llevado al intervalo [rumbo - pi, rumbo + pi]
    float stickHeading = CGeneral::GetRadianAngleBetweenPoints(0.0f, 0.0f, -walkLeftRight, walkUpDown) - StaticRef<float>(0xB6F178);
    if (targetHeading + 3.14159274f < stickHeading) {          // 0x858CB8 = pi
        stickHeading -= 6.28318548f;                           // 0x858CBC = 2 pi
    } else if (targetHeading - 3.14159274f > stickHeading) {
        stickHeading += 6.28318548f;
    }

    // Como el original: 0x540770 (salto) y, si no, 0x5407F0 (correr) abortan la subtarea
    bool shouldAbort = pad->JumpJustDown() || pad->SprintJustDown();
    if (!shouldAbort) {
        if (!(stickLength > 0.75f)) { // 0x858F34
            return m_pSubTask;
        }
        shouldAbort = (stickHeading - targetHeading) > 0.785398185f; // 0x859AB0 = pi / 4
    }
    if (!shouldAbort) {
        return m_pSubTask;
    }
    m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
    return nullptr;
}
