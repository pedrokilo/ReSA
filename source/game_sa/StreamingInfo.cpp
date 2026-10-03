/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CStreamingInfo
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

void CStreamingInfo::InjectHooks() {
    RH_ScopedClass(CStreamingInfo);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x407460);
    RH_ScopedInstall(AddToList, 0x407480);
    RH_ScopedNamedInstall(GetCdPosn_Hook, "GetCdPosn", 0x407570); // Como el original: devuelve el valor en EAX
    RH_ScopedInstall(SetCdPosnAndSize, 0x4075E0);
    RH_ScopedInstall(GetCdPosnAndSize, 0x4075A0);
    RH_ScopedInstall(InList, 0x407560);
    RH_ScopedInstall(RemoveFromList, 0x4074E0);
}

// 0x407460
void CStreamingInfo::Init() {
    // Como el original: no toca m_Flags ni el relleno
    m_NextIndex     = -1;
    m_PrevIndex     = -1;
    m_NextIndexOnCd = -1;
    m_ImgID         = 0;
    m_CDOffset      = 0;
    m_CDSize        = 0;
    m_LoadState     = LOADSTATE_NOT_LOADED;
}

// 0x407570
CdStreamPos CStreamingInfo::GetCdPosn() const {
    // Como el original: suma de 32 bits del handle del archivo y el desplazamiento
    return std::bit_cast<CdStreamPos>((uint32)CStreaming::ms_files[m_ImgID].StreamHandle + m_CDOffset);
}

// 0x407570 - envoltorio del hook: una función miembro de MSVC devuelve las estructuras por puntero oculto,
// pero el EXE espera el valor en EAX sin argumentos (ret sin pila)
uint32 CStreamingInfo::GetCdPosn_Hook() const {
    return std::bit_cast<uint32>(GetCdPosn());
}

// 0x4075E0
void CStreamingInfo::SetCdPosnAndSize(uint32 offset, size_t size) {
    m_CDOffset = offset;
    m_CDSize   = size;
}

// 0x4075A0
bool CStreamingInfo::GetCdPosnAndSize(CdStreamPos& pos, size_t& size) {
    if (HasCdPosnAndSize()) {
        pos  = GetCdPosn();
        size = m_CDSize;
        return true;
    }
    return false;
}

// 0x407560
bool CStreamingInfo::InList() const {
    // Como el original: solo mira el siguiente
    return m_NextIndex != -1;
}

// 0x407480
void CStreamingInfo::AddToList(CStreamingInfo* after) {
    m_NextIndex = after->m_NextIndex;
    m_PrevIndex = static_cast<int16>(after - ms_pArrayBase);

    after->m_NextIndex = static_cast<int16>(this - ms_pArrayBase);
    ms_pArrayBase[m_NextIndex].m_PrevIndex = after->m_NextIndex;
}

// 0x4074E0
void CStreamingInfo::RemoveFromList() {
    ms_pArrayBase[m_NextIndex].m_PrevIndex = m_PrevIndex;
    ms_pArrayBase[m_PrevIndex].m_NextIndex = m_NextIndex;
    m_NextIndex = -1;
    m_PrevIndex = -1;
}
