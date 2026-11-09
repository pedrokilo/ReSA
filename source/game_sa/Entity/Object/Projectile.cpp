/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CProjectile
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Projectile.h"

namespace CObjectBody {
void __fastcall Destructor(CObject* self, void* edx); // Object.cpp: 0x59F660
}

namespace {
// 0x5A40E0 (lote 142A, banco Fn142A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CProjectile del EXE (`mov [ecx],0x867030; jmp 0x59F660`). Deja la vtable de CProjectile del EXE
// (0x867030; la de la DLL no vale: las subclases del EXE la sobrescriben) y sigue con el cuerpo de ~CObject (0x59F660, sin hook: aqui el destructor de CObject de la DLL). Sin expansion en linea
// para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall ProjectileDestructorBody(CProjectile* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x867030;
    CObjectBody::Destructor(self, nullptr);
}
} // namespace

void CProjectile::InjectHooks() {
    RH_ScopedClass(CProjectile);
    RH_ScopedCategory("Entity/Object");
    RH_ScopedNamedGlobalInstall(ProjectileDestructorBody, "Destructor", 0x5A40E0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x5A40F0);

    RH_ScopedInstall(Constructor, 0x5A4030);
}

// 0x5A4030
CProjectile::CProjectile(int32 modelId) : CObject() {
    m_bTunnelTransition = true;
    m_fMass = 1.0F;
    m_fTurnMass = 1.0F;
    m_fAirResistance = 0.99999F;
    m_fElasticity = 0.75F;
    m_fBuoyancyConstant = std::bit_cast<float>(0x3A51B718u); // 0x3A51B718 (0.00080000004F) del EXE; 1.0F / 1250.0F da 0x3A51B717
    CObject::SetIsStatic(false);
    physicalFlags.bExplosionProof = true;
    CEntity::SetModelIndex(modelId);
    m_nObjectType = eObjectType::OBJECT_TYPE_DECORATION;
}

// 0x5A40F0 (lote 142A, banco Fn142A) : destructor con borrado de CProjectile del EXE (ranura 0 de su vtable 0x867030; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x5A40E0; test byte [esp+8],1; je ..; push esi; call 0x5A1F20; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5A40E0), libera `this` con
// CObject::operator delete (0x5A1F20, sin hook: pool de objetos 0xB7449C) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CProjectile* CProjectile::ScalarDeletingDestructor(uint8 flags) {
    ProjectileDestructorBody(this, nullptr);
    if (flags & 1) {
        CObject::operator delete(this);
    }
    return this;
}
