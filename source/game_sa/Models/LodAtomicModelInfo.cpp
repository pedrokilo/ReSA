/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CLodAtomicModelInfo
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

#include "LodAtomicModelInfo.h"

namespace CBaseModelInfoBody {
CBaseModelInfo* __fastcall Constructor(CBaseModelInfo* self, void* edx); // BaseModelInfo.cpp: 0x4C4A60
}

// 0x4C55E0 (lote 173A, banco Fn173A): cuerpo del constructor de CLodAtomicModelInfo del EXE (thiscall, sin argumentos, `ret`, 28 bytes: `push esi; mov esi,ecx; call 0x4C4A60; xor eax,eax; mov [esi+20h],ax;
// mov [esi+22h],ax; mov [esi],0x85BC70; mov eax,esi; pop esi; ret`; el listado decia CDamageAtomicModelInfo): llama al cuerpo del constructor de CBaseModelInfo (0x4C4A60, con hook), pone a cero los dos contadores de
// 16 bits (+0x20 hijos, +0x22 hijos dibujados) y deja la vtable de CLodAtomicModelInfo del EXE (0x85BC70); devuelve `this`. Funcion propia sin expansion en linea: el banco sustituye el cuerpo base.
__declspec(noinline) static CLodAtomicModelInfo* __fastcall LodAtomicModelInfoConstructorBody(CLodAtomicModelInfo* self, void* /*edx*/) {
    CBaseModelInfoBody::Constructor(self, nullptr);
    *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(self) + 0x20) = 0;
    *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(self) + 0x22) = 0;
    *reinterpret_cast<uint32*>(self) = 0x85BC70;
    return self;
}

/**
 * Registra las operaciones del model info especializado para LODs.
 */
void CLodAtomicModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CLodAtomicModelInfo, 0x85BC70, 16);
    RH_ScopedCategory("Models");

    RH_ScopedNamedGlobalInstall(LodAtomicModelInfoConstructorBody, "ConstructorBody", 0x4C55E0); // lote 173A (banco Fn173A)
    RH_ScopedVMTInstall(AsLodAtomicModelInfoPtr, 0x4C5610);
    RH_ScopedVMTInstall(GetModelType, 0x4C5600);
    RH_ScopedVMTInstall(Init, 0x4C54D0);
}

// Inicializa los contadores usados para coordinar hijos LOD y los ya renderizados.
CLodAtomicModelInfo::CLodAtomicModelInfo() : CAtomicModelInfo()
{
    m_numChildren = 0;
    m_numChildrenRendered = 0;
}

// 0x4C5610
CLodAtomicModelInfo* CLodAtomicModelInfo::AsLodAtomicModelInfoPtr()
{
    return this;
}

// 0x4C5600
ModelInfoType CLodAtomicModelInfo::GetModelType()
{
    return ModelInfoType::MODEL_INFO_LOD;
}

// 0x4C54D0
void CLodAtomicModelInfo::Init()
{
    CAtomicModelInfo::Init();
}