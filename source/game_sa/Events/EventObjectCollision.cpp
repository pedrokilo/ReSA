/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventObjectCollision
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventObjectCollision.h"


namespace {
// 0x4ACDD0 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventObjectCollision que usan los destructores del EXE. Deja la vtable de CEventObjectCollision del EXE (0x85ACA8), libera la referencia de m_object (+0x14) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall ObjectCollisionDestructorBody(CEventObjectCollision* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85ACA8;
    CEntity::SafeCleanUpRef(self->m_object);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventObjectCollision::InjectHooks()
{
    RH_ScopedVirtualClass(CEventObjectCollision, 0x85ACA8, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(ObjectCollisionDestructorBody, "Destructor", 0x4ACDD0);

    RH_ScopedInstall(Constructor, 0x4ACCF0);
    RH_ScopedVMTInstall(AffectsPed, 0x4ACE30);
    RH_ScopedVMTInstall(GetEventType, 0x4ACDA0);        // lote 154A (banco Fn154A)
    RH_ScopedVMTInstall(GetEventPriority, 0x4ACDC0);    // lote 154A
    RH_ScopedVMTInstall(GetLifeTime, 0x4ACDB0);         // lote 154A
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4ACEF0);   // lote 154A
    RH_ScopedInstall(IsHeadOnCollision, 0x4ACE80);      // lote 154A
}

// 0x4ACCF0
CEventObjectCollision::CEventObjectCollision(int16 pieceType, float damageIntensity, CObject* object, const CVector& collisionImpactVelocity, const CVector& collisionPos, int16 moveState)
{
    m_pieceType = pieceType;
    m_moveState = moveState;
    m_damageIntensity = damageIntensity;
    m_object = object;
    m_impactNormal = collisionImpactVelocity;
    m_impactPos = collisionPos;
    CEntity::SafeRegisterRef(m_object);
}

CEventObjectCollision::~CEventObjectCollision()
{
    CEntity::SafeCleanUpRef(m_object);
}

// 0x4ACCF0
CEventObjectCollision* CEventObjectCollision::Constructor(int16 pieceType, float damageIntensity, CObject* object, const CVector& collisionImpactVelocity, const CVector& collisionPos, int16 moveState)
{
    this->CEventObjectCollision::CEventObjectCollision(pieceType, damageIntensity, object, collisionImpactVelocity, collisionPos, moveState);
    return this;
}

// 0x4ACDA0 (lote 154A, banco Fn154A; `mov eax,5; ret`): EVENT_OBJECT_COLLISION
eEventType CEventObjectCollision::GetEventType() const {
    return EVENT_OBJECT_COLLISION;
}

// 0x4ACEF0 (`mov al,1; ret 4`)
bool CEventObjectCollision::TakesPriorityOver(const CEvent& refEvent) {
    return true;
}

// 0x4ACDC0 (`mov eax,0x39; ret`)
int32 CEventObjectCollision::GetEventPriority() const {
    return 57;
}

// 0x4ACDB0 (`xor eax,eax; ret`)
int32 CEventObjectCollision::GetLifeTime() {
    return 0;
}

// 0x4ACE80 (lote 154A, banco Fn154A; thiscall(ped), `ret 4`): mismo cuerpo que CEventBuildingCollision::IsHeadOnCollision (0x4AD1E0) con el umbral 0.9397f (0x85AAE4): normaliza (0x59C910) la normal del impacto sin
// componente z y la compara, con el orden de sumas del EXE, con el vector hacia delante (m_matrix + 0x10) del ped. Lo llama ComputeObjectCollisionResponse y descarta el resultado.
bool CEventObjectCollision::IsHeadOnCollision(CPed* ped) {
    CVector velocity = m_impactNormal;
    velocity.z = 0.0f;
    velocity.Normalise();
    const CVector& fwd = ped->GetForward();
    return -((velocity.y * fwd.y + velocity.z * fwd.z) + velocity.x * fwd.x) > 0.9397f; // Como el original: orden de sumas del EXE
}

// 0x4ACE30
bool CEventObjectCollision::AffectsPed(CPed* ped)
{
    if (!ped->m_pAttachedTo) {
        if (m_object && !m_object->physicalFlags.bDisableMoveForce) {
            if (!ped->IsPlayer() && ped->IsAlive()) 
                return true;
        }
    }
    return false;
}
