/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventDanger
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventDanger.h"

void CEventDanger::InjectHooks() {
    RH_ScopedVirtualClass(CEventDanger, 0x85B848, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B2600);
    RH_ScopedVMTInstall(AffectsPed, 0x4B5470);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B54E0);
    RH_ScopedVMTInstall(GetSourceEntity, 0x4B2700);
}

CEventDanger* CEventDanger::Constructor(CEntity* dangerFrom, float dangerRadius) { this->CEventDanger::CEventDanger(dangerFrom, dangerRadius); return this; }

// 0x4B2600
CEventDanger::CEventDanger(CEntity* dangerFrom, float dangerRadius) : CEventEditableResponse() {
    m_dangerFrom = dangerFrom;
    CEntity::SafeRegisterRef(m_dangerFrom);
    m_dangerRadius = dangerRadius;
}

CEventDanger::~CEventDanger() {
    CEntity::SafeCleanUpRef(m_dangerFrom);
}

// 0x4B5470
bool CEventDanger::AffectsPed(CPed* ped) {
    if (m_dangerFrom && m_dangerFrom != ped->m_pVehicle) {
        CVector2D distance = ped->GetPosition() - m_dangerFrom->GetPosition();
        if (m_dangerRadius * m_dangerRadius >= distance.SquaredMagnitude())
            return ped->IsAlive();
    }
    return false;
}

// 0x4B54E0
bool CEventDanger::AffectsPedGroup(CPedGroup* pedGroup) {
    if (GetSourceEntity() && GetSourceEntity()->GetIsTypePed()) {
        CPed* leader = pedGroup->GetMembership().GetLeader();
        if (leader) {
            CVector2D distance = leader->GetPosition() - m_dangerFrom->GetPosition();
            return distance.SquaredMagnitude() <= m_dangerRadius * m_dangerRadius;
        }
    }
    return false;
}

// 0x4B2700
CEntity* CEventDanger::GetSourceEntity() const {
    if (m_dangerFrom && !m_dangerFrom->GetIsTypePed() && m_dangerFrom->GetIsTypeVehicle()) {
        CVehicle* vehicle = m_dangerFrom->AsVehicle();
        if (vehicle->m_pDriver)
            return vehicle->m_pDriver;
    }
    return m_dangerFrom;
}