/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CLodTimeModelInfo
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

#include "LodTimeModelInfo.h"

namespace {
// 0x4C56E0 (lote 139A, banco Fn139A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CLodTimeModelInfo del EXE (la cadena de destructores hasta CBaseModelInfo se reduce a
// `mov dword [ecx],0x85BB9C`: deja la vtable de CBaseModelInfo del EXE). Lo llaman el destructor con borrado (0x4C6290) y el iterador de destructores de vectores del EXE (`push 0x4C56E0`).
void __fastcall LodTimeModelInfoDestructorBody(CLodTimeModelInfo* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85BB9C;
}
} // namespace

void CLodTimeModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CLodTimeModelInfo, 0x85BCF0, 16);
    RH_ScopedCategory("Models");
    RH_ScopedNamedGlobalInstall(LodTimeModelInfoDestructorBody, "Destructor", 0x4C56E0);

    // Las direcciones anteriores (0x4C5660/0x4C5670) eran las de CTimeModelInfo; las
    // propias de esta clase están en las entradas [4] y [5] de su vtable 0x85BCF0.
    RH_ScopedVMTInstall(GetModelType, 0x4C56C0);
    RH_ScopedVMTInstall(GetTimeInfo, 0x4C56D0);
}

// 0x4C56C0
ModelInfoType CLodTimeModelInfo::GetModelType()
{
    return ModelInfoType::MODEL_INFO_TIME;
}

// 0x4C56D0
CTimeInfo* CLodTimeModelInfo::GetTimeInfo()
{
    return &m_lodTimeInfo;
}