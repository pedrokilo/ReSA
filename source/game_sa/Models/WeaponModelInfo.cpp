/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWeaponModelInfo
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "WeaponModelInfo.h"

namespace CBaseModelInfoBody {
CBaseModelInfo* __fastcall Constructor(CBaseModelInfo* self, void* edx); // BaseModelInfo.cpp: 0x4C4A60
}

// 0x4C5760 (lote 167A, banco Fn167A): constructor de CWeaponModelInfo del EXE (thiscall, sin argumentos, `ret`, 18 bytes: `push esi; mov esi,ecx; call 0x4C4A60; mov [esi],0x85BD78; mov eax,esi; pop esi; ret`; el listado
// lo atribuia a CClumpModelInfo): llama al cuerpo del constructor de CBaseModelInfo (0x4C4A60, con hook) y deja la vtable de CWeaponModelInfo del EXE (0x85BD78); devuelve `this`. Lo pasan a los
// iteradores de vectores del almacen de armas. Funcion propia sin expansion en linea: el banco sustituye el cuerpo base.
__declspec(noinline) static CWeaponModelInfo* __fastcall WeaponModelInfoConstructorBody(CWeaponModelInfo* self, void* /*edx*/) {
    CBaseModelInfoBody::Constructor(self, nullptr);
    *reinterpret_cast<uint32*>(self) = 0x85BD78;
    return self;
}

/**
 * Registra la inicialización y configuración RenderWare de modelos de armas.
 */
void CWeaponModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CWeaponModelInfo, 0x85BD78, 17);
    RH_ScopedCategory("Models");

    RH_ScopedNamedGlobalInstall(WeaponModelInfoConstructorBody, "ConstructorBody", 0x4C5760); // lote 167A (banco Fn167A)
    RH_ScopedVMTInstall(GetModelType, 0x4C5780);
    RH_ScopedVMTInstall(Init, 0x4C98F0);
    RH_ScopedVMTInstall(SetClump, 0x4C9910);
}

// 0x4C5780
ModelInfoType CWeaponModelInfo::GetModelType()
{
    return ModelInfoType::MODEL_INFO_WEAPON;
}

// 0x4C98F0
// Restablece el estado heredado y deja el tipo de arma en WEAPON_UNARMED hasta configurarlo.
void CWeaponModelInfo::Init()
{
    CClumpModelInfo::Init();
    m_weaponInfo = eWeaponType::WEAPON_UNARMED;
}

// 0x4C9910
/**
 * Instala el clump y configura todos sus atomics con el renderer específico de armas.
 *
 * Si existe el frame gunflash, oculta su primer atomic y ajusta el material para que
 * pueda activarse posteriormente como fogonazo sin renderizarse por defecto.
 */
void CWeaponModelInfo::SetClump(RpClump* clump)
{
    CClumpModelInfo::SetClump(clump);
    RpClumpForAllAtomics(clump, CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderWeaponCB);

    auto flashFrame = CClumpModelInfo::GetFrameFromName(clump, "gunflash");
    if (!flashFrame)
        return;

    auto firstAtomic = reinterpret_cast<RpAtomic*>(GetFirstObject(flashFrame));
    if (!firstAtomic)
        return;

    CVehicle::SetComponentAtomicAlpha(firstAtomic, 0);
    RpAtomicSetFlags(firstAtomic, 0);
    auto geometry = RpAtomicGetGeometry(firstAtomic);
    RpMaterialGetSurfaceProperties(RpGeometryGetMaterial(geometry, 0))->ambient = 16.0F;

}