/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxPrimBP_c
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxPrimBP.h"
#include "FxTools.h"

/**
 * Registra la carga común y recuperación de matriz de los blueprints de primitivas FX.
 */
void FxPrimBP_c::InjectHooks() {
    RH_ScopedClass(FxPrimBP_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(GetRWMatrix, 0x4A9DC0);
    RH_ScopedNamedInstall(Load_Reversed, "Load", 0x5C2010);
}

// 0x4A9CF0
FxPrimBP_c::FxPrimBP_c() {
    m_apTextures.fill(nullptr);
}

// 0x4A9D20
// Libera todas las texturas adquiridas por la primitiva y anula sus punteros.
FxPrimBP_c::~FxPrimBP_c() {
    // FIX_BUGS: They forgot destroy third texture (texture 3 destroyed twice)
    for (auto& texture : m_apTextures) {
        RwTextureDestroy(texture);
        texture = nullptr;
    }
}

// 0x4A9DC0
// Expande la matriz compacta si existe; la ausencia de datos representa identidad.
void FxPrimBP_c::GetRWMatrix(RwMatrix& outMatrix) {
    if (m_pMatrixBuffered) {
        m_pMatrixBuffered->CopyToRwMatrix(outMatrix);
    } else {
        RwMatrixSetIdentity(&outMatrix);
    }
}

// 0x5C2010
/**
 * Lee la sección base de una primitiva: matriz, nombres de texturas, alpha/blend
 * y bloques FxInfo. La matriz identidad se representa con nullptr para ahorrar memoria.
 */
bool FxPrimBP_c::Load(FILESTREAM file, int32 version, FxName32_t* textureNames) {
    char line[256], field[128];

    ReadField<void>(file, "FX_PRIM_BASE_DATA:");
    ReadFieldImpl(file, field, "NAME:");
    
    CVector mat[4];
    ReadLine(file, line, sizeof(line));
    VERIFY(sscanf(
        line,
        "%s %f %f %f %f %f %f %f %f %f %f %f %f",
        field,
        &mat[0].x, &mat[0].y, &mat[0].z, // right
        &mat[1].x, &mat[1].y, &mat[1].z, // up
        &mat[2].x, &mat[2].y, &mat[2].z, // at
        &mat[3].x, &mat[3].y, &mat[3].z  // pos
    ) == 13);

    // if it's identity matrix, don't allocate it
    if (mat[0] == CVector{1.0f, 0.0f, 0.0f} &&
        mat[1] == CVector{0.0f, 1.0f, 0.0f} &&
        mat[2] == CVector{0.0f, 0.0f, 1.0f} &&
        mat[3] == CVector{0.0f, 0.0f, 0.0f}
    ) {
        m_pMatrixBuffered = nullptr;
    } else {
        m_pMatrixBuffered = static_cast<FxBufferedMatrix*>(g_fxMan.GetMemPool().GetMem(sizeof(FxBufferedMatrix), alignof(int16)));
        assert(m_pMatrixBuffered);

        m_pMatrixBuffered->CopyFromVectors(mat[0], mat[1], mat[2], mat[3]);
    }

    assert(textureNames);
    ReadFieldImpl(file, textureNames[0], "TEXTURE:");

    if (version > 101) {
        ReadFieldImpl(file, textureNames[1], "TEXTURE2:");
        ReadFieldImpl(file, textureNames[2], "TEXTURE3:");
        ReadFieldImpl(file, textureNames[3], "TEXTURE4:");
    }

    const auto alpha = ReadField<int32>(file, "ALPHAON:");
    // The file stores an integer; the original copies its low byte verbatim.
    reinterpret_cast<uint8&>(m_bAlphaOn) = uint8(alpha);
    m_nSrcBlendId = ReadField<uint8>(file, "SRCBLENDID:");
    m_nDstBlendId = ReadField<uint8>(file, "DSTBLENDID:");

    m_FxInfoManager.Load(file, version);
    return true;
}
