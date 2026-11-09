/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventGunShotWhizzedBy
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
#include "EventGunShotWhizzedBy.h"

namespace CEventGunShotBody {
void __fastcall Destructor(CEventGunShot* self, void* edx); // EventGunShot.cpp: 0x4AC720
}

namespace {
// 0x4B2070 (lote 139A, banco Fn139A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventGunShotWhizzedBy del EXE (`mov [ecx],0x85B730; jmp 0x4AC720`). Deja la vtable de
// CEventGunShotWhizzedBy del EXE (0x85B730) y sigue con el cuerpo de ~CEventGunShot (0x4AC720, con hook).
void __fastcall GunShotWhizzedByDestructorBody(CEventGunShotWhizzedBy* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B730;
    CEventGunShotBody::Destructor(self, nullptr);
}
} // namespace

void CEventGunShotWhizzedBy::InjectHooks()
{
    RH_ScopedVirtualClass(CEventGunShotWhizzedBy, 0x85B730, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(GunShotWhizzedByDestructorBody, "Destructor", 0x4B2070);

    RH_ScopedInstall(Constructor, 0x4B1FD0);
    RH_ScopedVMTInstall(GetEventType, 0x4B2030);                // lote 139A (banco Fn139A): retorno de constante (49)
    RH_ScopedVMTInstall(GetLifeTime, 0x4B2040);                 // lote 139A: 0
    RH_ScopedVMTInstall(GetEventPriority, 0x4B2060);            // lote 139A: 36
    RH_ScopedVMTInstall(CanBeInterruptedBySameEvent, 0x4B2050); // lote 139A: siempre true (`mov al,1; ret`)
    RH_ScopedVMTInstall(AffectsPed, 0x4B5120);
}

// 0x4B1FD0
CEventGunShotWhizzedBy::CEventGunShotWhizzedBy(CEntity* entity, const CVector& startPoint, const CVector& endPoint, bool bHasNoSound) :
    CEventGunShot(entity, startPoint, endPoint, bHasNoSound)
{
}

// 0x4B1FD0
CEventGunShotWhizzedBy* CEventGunShotWhizzedBy::Constructor(CEntity* entity, const CVector& startPoint, const CVector& endPoint, bool bHasNoSound)
{
    this->CEventGunShotWhizzedBy::CEventGunShotWhizzedBy(entity, startPoint, endPoint, bHasNoSound);
    return this;
}

// 0x4B5120
bool CEventGunShotWhizzedBy::AffectsPed(CPed* ped)
{
    if (!ped->IsPlayer() && ped->IsAlive()) {
        CVector direction = m_endPoint - m_startPoint;
        direction.Normalise();
        CVector distance = ped->GetPosition() - m_startPoint;
        // Como el original: producto escalar (z + y + x), no la magnitud del producto por componentes
        const float dot = DotProduct(distance, direction);
        if (dot > 0.0f) {
            CVector2D point = (direction * dot) + m_startPoint;
            return 2.0f * 2.0f > (ped->GetPosition() - point).SquaredMagnitude();
        }
    }
    return false;
}