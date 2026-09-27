/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWeaponModelInfo
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "WeaponModelInfo.h"

/**
 * Registra la inicialización y configuración RenderWare de modelos de armas.
 */
void CWeaponModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CWeaponModelInfo, 0x85BD78, 17);
    RH_ScopedCategory("Models");

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