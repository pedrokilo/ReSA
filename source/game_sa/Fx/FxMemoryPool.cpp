/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxMemoryPool_c
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxMemoryPool.h"

/**
 * Registra el allocator lineal usado por el subsistema FX.
 */
void FxMemoryPool_c::InjectHooks() {
    RH_ScopedClass(FxMemoryPool_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Init, 0x4A9C30);
    RH_ScopedInstall(Exit, 0x4A9C80);
    RH_ScopedInstall(Reset, 0x4A9C90);
    RH_ScopedInstall(GetMem, 0x4A9CA0);
    RH_ScopedInstall(Optimise, 0x4A9CD0);
}

// 0x4A9C30
// Reserva inicialmente un megabyte y comienza a entregar memoria desde el offset cero.
void FxMemoryPool_c::Init() {
    m_nSize     = 1024 * 1024; // 1 megabyte
    m_nPosition = 0;
    m_pData     = static_cast<uint8*>(CMemoryMgr::Malloc(m_nSize));

    if (m_pData)
        memset(m_pData, 0, m_nSize);
}

// 0x4A9C80
void FxMemoryPool_c::Exit() {
    CMemoryMgr::Free(this);
}

// 0x4A9C90
// Reutiliza todo el bloque sin liberar memoria ni ejecutar destructores individuales.
void FxMemoryPool_c::Reset() {
    m_nPosition = 0;
}

// 0x4A9CA0
/**
 * Avanza m_nPosition hasta la alineación solicitada y devuelve el siguiente rango.
 * No mantiene free-list: la memoria individual no se devuelve hasta Reset/Exit.
 */
void* FxMemoryPool_c::GetMem(int32 size, int32 align) {
    if (m_nPosition % align > 0)
        m_nPosition = align + m_nPosition - m_nPosition % align;

    auto result = m_pData + m_nPosition;
    m_nPosition = size + m_nPosition;
    return result;
}

// 0x4A9CD0
// Reduce el bloque al número de bytes efectivamente consumidos durante la carga.
void FxMemoryPool_c::Optimise() {
    m_pData = static_cast<uint8*>(CMemoryMgr::Realloc(m_pData, m_nPosition));
    m_nSize = m_nPosition;
}
