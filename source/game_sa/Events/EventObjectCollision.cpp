/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventObjectCollision
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
