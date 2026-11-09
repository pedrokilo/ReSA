/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTimeModelInfo
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

#include "ModelInfo.h"

namespace {
// 0x4C5680 (lote 142A, banco Fn142A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CTimeModelInfo del EXE (la cadena de destructores hasta CBaseModelInfo se reduce a
// `mov dword [ecx],0x85BB9C`: deja la vtable de CBaseModelInfo del EXE). Lo llaman el destructor con borrado (0x4C6270) y el iterador de destructores de vectores del EXE (`push 0x4C5680` en 0x4C58A0
// y 0x4C5DD0, con elementos de 0x24 bytes = `CTimeModelInfo`).
void __fastcall TimeModelInfoDestructorBody(CTimeModelInfo* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85BB9C;
}
} // namespace

/**
 * Registra las operaciones específicas de los modelos con visibilidad por horario.
 */
// 0x4C5640 (lote 140A, banco Fn140A): constructor de CTimeModelInfo del EXE (thiscall, sin argumentos, `ret`, 24 bytes: `push esi; mov esi,ecx; call 0x4C4A60; mov dword [esi],0x85BCB0; mov word [esi+22h],0FFFFh;
// mov eax,esi; pop esi; ret`). 0x4C4A60 (sin hook) es el constructor de CBaseModelInfo expandido: vtable 0x85BB9C, +8 = 0 (contador de referencias, word) y +0xA = 0xFFFF (indice del TXD, word). Aqui se escribe
// en linea (el resultado final es el mismo: la vtable de la base se sobrescribe enseguida con la de la clase); +0x22 (CTimeInfo::m_nOtherTimeModel) = 0xFFFF; devuelve this. Lo pasan a los iteradores
// de vectores del almacen de modelos con horario (CStore de 169 elementos de 0x24 bytes).
__declspec(noinline) static CTimeModelInfo* __fastcall TimeModelInfoConstructorBody(CTimeModelInfo* self, void* /*edx*/) {
    auto* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint16*>(bytes + 0x8) = 0;
    *reinterpret_cast<uint16*>(bytes + 0xA) = 0xFFFF;
    *reinterpret_cast<uint32*>(bytes) = 0x85BCB0;
    *reinterpret_cast<uint16*>(bytes + 0x22) = 0xFFFF;
    return self;
}

void CTimeModelInfo::InjectHooks() {
    RH_ScopedVirtualClass(CTimeModelInfo, 0x85BCB0, 16);
    RH_ScopedCategory("Models");
    RH_ScopedNamedGlobalInstall(TimeModelInfoDestructorBody, "Destructor", 0x4C5680);

    RH_ScopedNamedGlobalInstall(TimeModelInfoConstructorBody, "ConstructorBody", 0x4C5640); // lote 140A (banco Fn140A)

    RH_ScopedVMTInstall(GetModelType, 0x4C5660);
    RH_ScopedVMTInstall(GetTimeInfo, 0x4C5670);
}

// 0x4C5660
ModelInfoType CTimeModelInfo::GetModelType() {
    return ModelInfoType::MODEL_INFO_TIME;
}

// 0x4C5670
// Expone los metadatos horarios embebidos para que CBaseModelInfo pueda tratar pares día/noche.
CTimeInfo* CTimeModelInfo::GetTimeInfo() {
    return &m_timeInfo;
}

