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
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxPrimBP.h"
#include "FxTools.h"

namespace {
// 0x4A9CF0 (lote 166A, banco Fn166A; thiscall, sin argumentos de pila, 35 bytes; el listado decia FxMemoryPool_c; lo llama el constructor de FxEmitterBP_c, 0x4A18D3): cuerpo del constructor de FxPrimBP_c del EXE. Como
// el original: vtable 0x85A9C4 (la del EXE), lista de particulas (+0x20, tres dwords a cero: 0x4A8DD0, sin hook, `xor ecx,ecx` y tres `mov`) y las cuatro texturas (+0xC, +0x10, +0x14, +0x18) a cero; devuelve this.
// No construye el FxInfoManager_c (+0x2C): lo hace el constructor de la clase derivada.
__declspec(noinline) void* __fastcall FxPrimBPConstructorBody(FxPrimBP_c* self, void* /*edx*/) {
    auto* const raw = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(raw + 0x00) = 0x85A9C4;
    *reinterpret_cast<uint32*>(raw + 0x20) = 0;
    *reinterpret_cast<uint32*>(raw + 0x24) = 0;
    *reinterpret_cast<uint32*>(raw + 0x28) = 0;
    *reinterpret_cast<uint32*>(raw + 0x0C) = 0;
    *reinterpret_cast<uint32*>(raw + 0x10) = 0;
    *reinterpret_cast<uint32*>(raw + 0x14) = 0;
    *reinterpret_cast<uint32*>(raw + 0x18) = 0;
    return self;
}

// 0x4A9D20 (lote 166A, banco Fn166A; thiscall, sin argumentos de pila, 159 bytes con marco SEH que solo desenrolla; lo alcanza el destructor de FxEmitterBP_c con `jmp`, 0x4A18F6): cuerpo del destructor de
// FxPrimBP_c del EXE. Como el original: deja la vtable 0x85A9C4 y destruye las texturas de +0xC, +0x10 y +0x14 (cada una solo si no es nula, con RwTextureDestroy 0x7F3820 y a cero justo despues); repite la
// comprobacion de +0x14 (ya nula: no hace nada) y NO toca la cuarta textura (+0x18; el bug del original que corrige `~FxPrimBP_c`). Los destructores de +0x2C (FxInfoManager_c, 0x4A4950) y +0x20 (lista,
// 0x4A8DE0) son un `ret` vacio: no hacen nada. No devuelve nada util.
__declspec(noinline) void __fastcall FxPrimBPDestructorBody(FxPrimBP_c* self, void* /*edx*/) {
    auto* const raw = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(raw + 0x00) = 0x85A9C4;
    for (const uint32 offset : { 0xCu, 0x10u, 0x14u, 0x14u }) {
        if (RwTexture* const texture = *reinterpret_cast<RwTexture**>(raw + offset)) {
            RwTextureDestroy(texture);
            *reinterpret_cast<RwTexture**>(raw + offset) = nullptr;
        }
    }
}
} // namespace

/**
 * Registra la carga común y recuperación de matriz de los blueprints de primitivas FX.
 */
void FxPrimBP_c::InjectHooks() {
    RH_ScopedClass(FxPrimBP_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(GetRWMatrix, 0x4A9DC0);
    RH_ScopedNamedGlobalInstall(FxPrimBPConstructorBody, "ConstructorBody", 0x4A9CF0); // lote 166A (banco Fn166A)
    RH_ScopedNamedGlobalInstall(FxPrimBPDestructorBody, "DestructorBody", 0x4A9D20);   // lote 166A
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
    auto* const out = reinterpret_cast<float*>(&outMatrix);
    if (m_pMatrixBuffered) {
        // Como el original: cada componente se multiplica por el float 1/32767 (0x858C7C) en lugar de
        // dividirse entre 32767; no se tocan los flags ni los rellenos de la RwMatrix.
        constexpr float INV_32767 = std::bit_cast<float>(0x38000100u);
        const auto* const src = reinterpret_cast<const int16*>(m_pMatrixBuffered);
        for (auto row = 0; row < 4; row++) {
            for (auto col = 0; col < 3; col++) {
                out[row * 4 + col] = (float)src[row * 3 + col] * INV_32767;
            }
        }
    } else {
        // Como el original: identidad escrita a mano (sin RwMatrixSetIdentity, que también cambiaría los flags).
        for (auto row = 0; row < 4; row++) {
            for (auto col = 0; col < 3; col++) {
                out[row * 4 + col] = row == col ? 1.0f : 0.0f;
            }
        }
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
