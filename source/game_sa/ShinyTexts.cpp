/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CShinyTexts
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

#include "ShinyTexts.h"
#include "Shadows.h"

namespace {
// Como el original: los contadores del búfer temporal (0xC4B950/0xC4B954) se leen y escriben como dwords con signo.
auto& ShinyTempVerticesStored = StaticRef<int32>(0xC4B950);
auto& ShinyTempIndicesStored  = StaticRef<int32>(0xC4B954);
} // namespace

void CShinyTexts::InjectHooks() {
    RH_ScopedClass(CShinyTexts);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x7221B0);
    RH_ScopedInstall(RenderOutGeometryBuffer, 0x7221C0);
    RH_ScopedInstall(Render, 0x724890);
    RH_ScopedInstall(RegisterOne, 0x724B60);
}

// 0x7221B0
void CShinyTexts::Init() {
    NumShinyTexts = 0;
}

// 0x7221C0
void CShinyTexts::RenderOutGeometryBuffer() {
    if (ShinyTempIndicesStored) {
        LittleTest();
        if (RwIm3DTransform(TempBufferVertices.m_3d, ShinyTempVerticesStored, nullptr, rwIM3D_VERTEXUV))
        {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, ShinyTempIndicesStored);
            RwIm3DEnd();
        }
        ShinyTempVerticesStored = 0;
        ShinyTempIndicesStored = 0;
    }
}

// 0x724890
void CShinyTexts::Render() {
    ZoneScoped;

    if (NumShinyTexts == 0)
        return;

    RwRenderStateSet(rwRENDERSTATECULLMODE,     RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,     RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,    RWRSTATE(rwBLENDONE));

    ShinyTempIndicesStored  = 0;
    ShinyTempVerticesStored = 0;

    RwTexture* texture{};
    // Como el original: contador con signo releído en cada vuelta.
    for (int32 t = 0; t < static_cast<int32>(NumShinyTexts); t++) {
        CRegisteredShinyText& text = aShinyTexts.data()[t];
        if (ShinyTempIndicesStored > TOTAL_TEMP_BUFFER_INDICES - 64 ||
            ShinyTempVerticesStored > TOTAL_TEMP_BUFFER_3DVERTICES - 64
        )
        {
            RenderOutGeometryBuffer();
        }

        // Como el original: solo se dibuja si el byte vale exactamente 1.
        if (*reinterpret_cast<const uint8*>(&text.m_bAlwaysTrue) != 1)
            continue;

        if (texture != gpHandManTex) {
            RenderOutGeometryBuffer();
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpHandManTex)));
            texture = gpHandManTex;
        }

        const int32 vertexBase = ShinyTempVerticesStored;

        // Como el original: alfa 0xFF (el color se compone como 0xFFFFFF00 | r, desplazado).
        const uint32       color  = 0xFF000000u | (static_cast<uint32>(text.m_colorR) << 16) | (static_cast<uint32>(text.m_colorG) << 8) | text.m_colorB;
        const CVector*     posn[] = { &text.m_vecCornerAA, &text.m_vecCornerAB, &text.m_vecCornerBA, &text.m_vecCornerBB };
        const RwTexCoords* uvs[]  = { &text.m_texCoorsAA, &text.m_texCoorsAB, &text.m_texCoorsBA, &text.m_texCoorsBB };
        for (int32 i = 0; i < 4; i++) {
            auto* vertex = &TempBufferVertices.m_3d[vertexBase + i];

            vertex->color = color;
            RxObjSpace3DVertexSetPos(vertex, posn[i]);
            RxObjSpace3DVertexSetU(vertex, uvs[i]->u);
            RxObjSpace3DVertexSetV(vertex, uvs[i]->v);
        }

        // Relative to the 0th vertex of the current text.
        constexpr RxVertexIndex indices[] = { 0, 1, 2, 2, 1, 3 };
        const int32 indexBase = ShinyTempIndicesStored;
        for (int32 i = 0; i < 6; i++) {
            aTempBufferIndices[indexBase + i] = static_cast<RxVertexIndex>(vertexBase + indices[i]);
        }

        ShinyTempIndicesStored  = indexBase + 6;
        ShinyTempVerticesStored = vertexBase + 4;
    }

    RenderOutGeometryBuffer();
    NumShinyTexts = 0;

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
}

// Must be called each frame to re-draw
// They are drawn once, then deleted
// 0x724B60
void CShinyTexts::RegisterOne(CVector cornerAA, CVector cornerAB, CVector cornerBA, CVector cornerBB,
                              float u1, float v1,
                              float u2, float v2,
                              float u3, float v3,
                              float u4, float v4,
                              bool alwaysTrue, uint8 red, uint8 green, uint8 blue, float maxDistance
) {
    // Como el original: comparación con signo.
    if (static_cast<int32>(NumShinyTexts) >= static_cast<int32>(MAX_SHINYTEXTS))
        return;

    CRegisteredShinyText& text = aShinyTexts.data()[NumShinyTexts];

    // Como el original: distancia con el orden de suma del EXE ((z*z + y*y) + x*x), guardada en la entrada libre
    // antes de comparar (aunque luego se descarte).
    const CVector& camPos = TheCamera.GetPosition();
    const float dx = cornerAA.x - camPos.x, dy = cornerAA.y - camPos.y, dz = cornerAA.z - camPos.z;
    const float distToCam = std::sqrt(dz * dz + dy * dy + dx * dx);
    text.m_fDistanceToCamera = distToCam;

    // Como el original: se descarta si la distancia es mayor que el máximo o NaN.
    if (!(distToCam <= maxDistance))
        return;

    text.m_vecCornerAA       = cornerAA;
    text.m_vecCornerAB       = cornerAB;
    text.m_vecCornerBA       = cornerBA;
    text.m_vecCornerBB       = cornerBB;
    text.m_texCoorsAA        = {u1, v1};
    text.m_texCoorsAB        = {u2, v2};
    text.m_texCoorsBA        = {u3, v3};
    text.m_texCoorsBB        = {u4, v4};
    *reinterpret_cast<uint8*>(&text.m_bAlwaysTrue) = *reinterpret_cast<const uint8*>(&alwaysTrue); // Como el original: copia el byte tal cual
    text.m_colorR            = red;
    text.m_colorG            = green;
    text.m_colorB            = blue;

    const float halfMaxDist = maxDistance * 0.5f;
    if (halfMaxDist < distToCam) {
        // Como el original: 1 - (d - mitad) / mitad, y cada componente (float)c * mult truncado con _ftol2.
        const float mult = 1.0f - (distToCam - halfMaxDist) / halfMaxDist;
        text.m_colorR = static_cast<uint8>(static_cast<int64>(static_cast<float>(text.m_colorR) * mult));
        text.m_colorG = static_cast<uint8>(static_cast<int64>(static_cast<float>(text.m_colorG) * mult));
        text.m_colorB = static_cast<uint8>(static_cast<int64>(static_cast<float>(text.m_colorB) * mult));
    }

    NumShinyTexts++;
}
