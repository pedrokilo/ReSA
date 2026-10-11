/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBrightLights
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
#include "X87Math.h"

#include "BrightLights.h"

namespace {
// Como el original: los contadores del búfer temporal (0xC4B950/0xC4B954) se leen y escriben como dwords con signo.
auto& BrightTempVerticesStored = StaticRef<int32>(0xC4B950);
auto& BrightTempIndicesStored  = StaticRef<int32>(0xC4B954);

// Como el original: fsin/fcos del x87 (precisión interna completa) y producto redondeado por la FPU.



// Como el original: float a byte con _ftol2 (truncado a 64 bits, se queda el byte bajo).
uint8 BrightFtolByte(float f) {
    return static_cast<uint8>(static_cast<int64>(f));
}
} // namespace

void CBrightLights::InjectHooks() {
    RH_ScopedClass(CBrightLights);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x722140);
    RH_ScopedInstall(RenderOutGeometryBuffer, 0x722150);
    RH_ScopedInstall(Render, 0x7241C0);
    RH_ScopedInstall(RegisterOne, 0x724770);
}

// 0x722140
void CBrightLights::Init() {
    NumBrightLights = 0;
}

// 0x722150
void CBrightLights::RenderOutGeometryBuffer() {
    if (BrightTempIndicesStored) {
        LittleTest();
        if (RwIm3DTransform(TempBufferVertices.m_3d, BrightTempVerticesStored, nullptr, rwIM3D_VERTEXUV))
        {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, BrightTempIndicesStored);
            RwIm3DEnd();
        }
        BrightTempVerticesStored = 0;
        BrightTempIndicesStored = 0;
    }
}

// 0x7241C0
void CBrightLights::Render() {
    ZoneScoped;

    if (NumBrightLights == 0)
        return;

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLNONE));

    BrightTempIndicesStored = 0;
    BrightTempVerticesStored = 0;

    // Como el original: contador y comparaciones con signo; el contador se relee en cada vuelta.
    for (int32 i = 0; i < static_cast<int32>(NumBrightLights); i++) {
        // Maybe render vertex buffer if running low on free items...
        if (BrightTempIndicesStored > TOTAL_TEMP_BUFFER_INDICES - 40 ||
            BrightTempVerticesStored > TOTAL_TEMP_BUFFER_3DVERTICES - 40
        ) {
            CBrightLights::RenderOutGeometryBuffer();
        }
        aBrightLights[i].Render();
    }

    RenderOutGeometryBuffer();

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));

    NumBrightLights = 0;
}

// 0x724770
void CBrightLights::RegisterOne(CVector posn, CVector top, CVector right, CVector at, eBrightLightColor color, uint8 arg5, uint8 arg6, uint8 arg7) {
    if (color == eBrightLightColor::BRIGHTLIGHT_NONE)
        return;

    // Como el original: comparación con signo.
    if (static_cast<int32>(NumBrightLights) >= static_cast<int32>(MAX_NUM_BRIGHTLIGHTS))
        return;

    auto& light = aBrightLights[NumBrightLights];

    // Como el original: distancia con el orden de suma del EXE ((z*z + y*y) + x*x), dividida por el multiplicador
    // de LOD y guardada en la entrada libre antes de comparar (aunque luego se descarte).
    const CVector& camPos = TheCamera.GetPosition();
    const float dx = posn.x - camPos.x, dy = posn.y - camPos.y, dz = posn.z - camPos.z;
    const float distToCam = std::sqrt(dz * dz + dy * dy + dx * dx) / TheCamera.m_fLODDistMultiplier;
    light.m_fDistanceToCamera = distToCam;

    // Como el original: se descarta si la distancia es mayor que 60 o NaN.
    if (!(distToCam <= 60.0f))
        return;

    light.m_vecPosition       = posn;
    light.m_vecRight          = top;
    light.m_vecTop            = right;
    light.m_vecAt             = at;
    light.m_nColor            = color;
    light.field_35            = arg5;
    light.field_36            = arg6;
    light.field_37            = arg7;

    NumBrightLights++;
}

// NOTSA, inlined (0x724265 - 0x7246AC)
void tBrightLight::Render() const {
    // Como el original: rand() & 0xFF por 0.2 se calcula siempre, antes de mirar el color.
    const float rnd = static_cast<float>(CGeneral::GetRandomNumber() & 0xFF) * 0.2f;

    // Como el original: componentes del color por la primera tabla de saltos (los colores no válidos no tocan nada).
    uint8 red{}, green{}, blue{};
    switch (m_nColor) {
    case BRIGHTLIGHT_GREEN_BIG:
    case BRIGHTLIGHT_GREEN_SMALL:
        red   = BrightFtolByte(rnd);
        green = 255;
        blue  = red;
        break;
    case BRIGHTLIGHT_YELLOW_BIG:
    case BRIGHTLIGHT_YELLOW_SMALL:
        red   = 255;
        green = 128;
        blue  = BrightFtolByte(rnd);
        break;
    case BRIGHTLIGHT_RED_BIG:
    case BRIGHTLIGHT_RED_SMALL:
        red   = 255;
        green = BrightFtolByte(rnd);
        blue  = green;
        break;
    default:
        break;
    }

    // Como el original: alfa 255 por debajo de 45; si no (o NaN), (1 - (d - 45) * (1/15)) * 255 truncado.
    uint8 alpha = 255;
    if (!(m_fDistanceToCamera < 45.0f)) {
        alpha = BrightFtolByte((1.0f - (m_fDistanceToCamera - 45.0f) * std::bit_cast<float>(0x3D888889u)) * 255.0f); // 0x863E0C
    }

    const uint32 vertexBase = static_cast<uint32>(BrightTempVerticesStored);
    switch (m_nColor) {
    case BRIGHTLIGHT_GREEN_BIG:
    case BRIGHTLIGHT_YELLOW_BIG:
    case BRIGHTLIGHT_RED_BIG: {
        // Como el original: el alfa se reduce a la mitad con (float)alfa * 0.5 truncado.
        const uint8  bigAlpha = BrightFtolByte(static_cast<float>(alpha) * 0.5f);
        const uint32 color    = (static_cast<uint32>(bigAlpha) << 24) | (static_cast<uint32>(red) << 16) | (static_cast<uint32>(green) << 8) | blue;

        static constexpr float TrafficLightsRight[] = { 0.162f, 0.162f, 0.0f, -0.162f, -0.162f, 0.0f };  // 0x8D5DA4 (por m_vecRight)
        static constexpr float TrafficLightsTop[]   = { -0.09f, 0.09f, 0.162f, 0.09f, -0.09f, -0.162f }; // 0x8D5D8C (por m_vecTop)

        for (unsigned i = 0; i < 6; i++) {
            auto* vertex  = &TempBufferVertices.m_3d[vertexBase + i];
            vertex->color = color;
            // Como el original: (R * k1 + T * k2) + P.
            vertex->objVertex.x = (m_vecRight.x * TrafficLightsRight[i] + m_vecTop.x * TrafficLightsTop[i]) + m_vecPosition.x;
            vertex->objVertex.y = (m_vecRight.y * TrafficLightsRight[i] + m_vecTop.y * TrafficLightsTop[i]) + m_vecPosition.y;
            vertex->objVertex.z = (m_vecRight.z * TrafficLightsRight[i] + m_vecTop.z * TrafficLightsTop[i]) + m_vecPosition.z;
        }

        // 0x8D5DBC
        static constexpr RxVertexIndex TrafficLightIndices[] = { 0, 1, 5, 1, 2, 3, 1, 3, 4, 1, 4, 5 };
        const int32 indexBase = BrightTempIndicesStored;
        for (unsigned i = 0; i < std::size(TrafficLightIndices); i++) {
            aTempBufferIndices[indexBase + i] = static_cast<RxVertexIndex>(TrafficLightIndices[i] + vertexBase);
        }

        BrightTempIndicesStored  = indexBase + 12;
        BrightTempVerticesStored = static_cast<int32>(vertexBase) + 6;
        break;
    }
    case BRIGHTLIGHT_GREEN_SMALL:
    case BRIGHTLIGHT_YELLOW_SMALL:
    case BRIGHTLIGHT_RED_SMALL: {
        const uint32 color = (static_cast<uint32>(alpha) << 24) | (static_cast<uint32>(red) << 16) | (static_cast<uint32>(green) << 8) | blue;
        for (unsigned i = 0; i < 8; i++) {
            auto* vertex  = &TempBufferVertices.m_3d[vertexBase + i];
            vertex->color = color;

            // Como el original: ángulo (float)i * (2pi/8) (0x859AB0) y fsin/fcos del x87 por 0.1 (0x8D5E48).
            const float angle = static_cast<float>(static_cast<int32>(i)) * std::bit_cast<float>(0x3F490FDBu);
            const float s     = TrigSimpleSinMulCpp(angle, 0.1f);
            const float c     = TrigSimpleCosMulCpp(angle, 0.1f);
            // Como el original: (R * c + T * s) + P.
            vertex->objVertex.x = (c * m_vecRight.x + s * m_vecTop.x) + m_vecPosition.x;
            vertex->objVertex.y = (c * m_vecRight.y + s * m_vecTop.y) + m_vecPosition.y;
            vertex->objVertex.z = (c * m_vecRight.z + s * m_vecTop.z) + m_vecPosition.z;
        }

        const int32 indexBase = BrightTempIndicesStored;
        for (unsigned i = 0; i < 6; i++) {
            aTempBufferIndices[indexBase + i * 3 + 0] = static_cast<RxVertexIndex>(vertexBase);
            aTempBufferIndices[indexBase + i * 3 + 1] = static_cast<RxVertexIndex>(vertexBase + i + 1);
            aTempBufferIndices[indexBase + i * 3 + 2] = static_cast<RxVertexIndex>(vertexBase + i + 2);
        }

        BrightTempIndicesStored  = indexBase + 18;
        BrightTempVerticesStored = static_cast<int32>(vertexBase) + 8;
        break;
    }
    default: // Como el original: los colores no válidos no generan geometría.
        break;
    }
}
