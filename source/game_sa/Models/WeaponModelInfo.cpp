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

// 0x4C5790 (lote 194A, sin validar; no estaba en el listado: lo llama el destructor con borrado 0x4C62D0 y el iterador de destructores de vectores del almacen de armas): cuerpo del destructor de CWeaponModelInfo
// del EXE (thiscall, sin argumentos de pila, `ret`, 7 bytes: `mov dword [ecx],0x85BB9C`): solo deja la vtable de la clase base y no llama a nada (como 0x4C5590 y 0x4C5750). Funcion propia sin expansion en linea.
__declspec(noinline) static void __fastcall WeaponModelInfoDestructorBody(CWeaponModelInfo* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85BB9C;
}

// 0x4C62D0 (lote 194A, sin validar): destructor con borrado de CWeaponModelInfo del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 27 bytes: `push esi; mov esi,ecx; call 0x4C5790; test byte [esp+8],1; je ..; push esi;
// call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x4C5790, con hook), libera `this` con operator delete (0x8214BD, CRT) solo si el bit 0 del BYTE
// de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
static CWeaponModelInfo* __fastcall WeaponModelInfoScalarDeletingDestructor(CWeaponModelInfo* self, void* /*edx*/, uint8 flags) {
    WeaponModelInfoDestructorBody(self, nullptr);
    if (flags & 1) {
        ::operator delete(self);
    }
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
    RH_ScopedNamedGlobalInstall(WeaponModelInfoDestructorBody, "DestructorBody", 0x4C5790); // lote 194A (sin validar)
    RH_ScopedNamedGlobalInstall(WeaponModelInfoScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4C62D0); // lote 194A (sin validar)
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