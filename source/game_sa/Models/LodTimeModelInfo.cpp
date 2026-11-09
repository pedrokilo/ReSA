/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CLodTimeModelInfo
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

#include "LodTimeModelInfo.h"

namespace {
// 0x4C56E0 (lote 139A, banco Fn139A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CLodTimeModelInfo del EXE (la cadena de destructores hasta CBaseModelInfo se reduce a
// `mov dword [ecx],0x85BB9C`: deja la vtable de CBaseModelInfo del EXE). Lo llaman el destructor con borrado (0x4C6290) y el iterador de destructores de vectores del EXE (`push 0x4C56E0`).
void __fastcall LodTimeModelInfoDestructorBody(CLodTimeModelInfo* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85BB9C;
}

// 0x4C5690 (lote 142A, banco Fn142A; thiscall, sin argumentos de pila, `ret`): constructor de CLodTimeModelInfo del EXE (lo llama el iterador de constructores de vectores, 0x4C5E30: 0x28 bytes por
// elemento, sin leer el resultado; el listado lo atribuia a CTimeModelInfo). En el EXE llama al constructor de CBaseModelInfo (0x4C4A60: m_nRefCount (+8) = 0 y m_nTxdIndex (+0xA) = -1; sin hook), pone
// a cero m_numChildren (+0x20) y m_numChildrenRendered (+0x22), deja la vtable de CLodTimeModelInfo del EXE (0x85BCF0; la de la DLL no vale: las subclases del EXE la sobrescriben) y pone
// m_lodTimeInfo.m_nOtherTimeModel (+0x26) a -1 (m_nTimeOn/m_nTimeOff, +0x24 y +0x25, no se tocan). Devuelve this en EAX. Aqui el cuerpo de CBaseModelInfo va en linea: las mismas escrituras.
CLodTimeModelInfo* __fastcall LodTimeModelInfoConstructorBody(CLodTimeModelInfo* self, void* /*edx*/) {
    self->m_nRefCount = 0;
    self->m_nTxdIndex = -1;
    self->m_numChildren = 0;
    self->m_numChildrenRendered = 0;
    *reinterpret_cast<uint32*>(self) = 0x85BCF0;
    self->m_lodTimeInfo.m_nOtherTimeModel = -1;
    return self;
}
} // namespace

void CLodTimeModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CLodTimeModelInfo, 0x85BCF0, 16);
    RH_ScopedCategory("Models");
    RH_ScopedNamedGlobalInstall(LodTimeModelInfoDestructorBody, "Destructor", 0x4C56E0);
    RH_ScopedNamedGlobalInstall(LodTimeModelInfoConstructorBody, "Constructor", 0x4C5690);

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