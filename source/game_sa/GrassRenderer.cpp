/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGrassRenderer
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "GrassRenderer.h"
#include "PPTriPlantBuffer.h"

auto& gTriPlantBuf = StaticRef<CPPTriPlantBuffer>(0xC02DE8);
auto& g_GrassCurrentScanCode = StaticRef<uint16>(0x8D1330); // -1

void CGrassRenderer::InjectHooks() {
    RH_ScopedClass(CGrassRenderer);
    RH_ScopedCategory("Plant");

    RH_ScopedInstall(Initialise, 0x5DD6B0);
    RH_ScopedInstall(Shutdown, 0x5DABA0);
    RH_ScopedInstall(AddTriPlant, 0x5DB1D0);
    RH_ScopedInstall(DrawTriPlants, 0x5DAD00);
    RH_ScopedInstall(FlushTriPlantBuffer, 0x5DB250);
    RH_ScopedInstall(GetPlantModelsTab, 0x5DACE0);
    RH_ScopedInstall(SetPlantModelsTab, 0x5DACC0);
    RH_ScopedInstall(SetCloseFarAlphaDist, 0x5DABE0);
    RH_ScopedInstall(SetCurrentScanCode, 0x5DABB0);
    RH_ScopedInstall(SetGlobalCameraPos, 0x5DABC0);
    RH_ScopedInstall(SetGlobalWindBending, 0x5DAC00);

    RH_ScopedInstall(GenPointInTriangle, 0x5DAB00);
}

// 0x5DD6B0
bool CGrassRenderer::Initialise() {
    return true;
}

// 0x5DABA0
void CGrassRenderer::Shutdown() {
    // NOP
}

// 0x5DB1D0
void CGrassRenderer::AddTriPlant(PPTriPlant* srcPlant, uint32 plantModelSet) {
    gTriPlantBuf.ChangeCurrentPlantModelsSet(plantModelSet);

    // Como el original: comparación con signo
    if (gTriPlantBuf.m_CurrentIndex + 1 > (int32)MAX_PLANTS)
        gTriPlantBuf.Flush();

    memcpy(gTriPlantBuf.m_Buffer.data() + gTriPlantBuf.m_CurrentIndex, srcPlant, sizeof(PPTriPlant)); // Como el original: rep movs sin comprobar el índice
    gTriPlantBuf.IncreaseBufferIndex(plantModelSet, 1);
}

// Como el original: distancia sqrt((z*z + y*y) + x*x) de (a - b)
static float GrassDistance(const CVector& a, const CVector& b) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    const float dz = a.z - b.z;
    return std::sqrt((dz * dz + dy * dy) + dx * dx);
}

// 0x5DAD00
void CGrassRenderer::DrawTriPlants(PPTriPlant* triPlants, int32 numTriPlants, RpAtomic** plantModelsTab) {
    constexpr float RAND_RECIPROCAL = 3.0518509e-05f; // 0x858C7C (1/32767)

    for (int32 i = 0; i < numTriPlants; i++) { // Como el original: contador con signo
        const auto& plant = triPlants[i];

        const auto camDist = GrassDistance(plant.Center, m_vecCameraPos);
        // Como el original: la calidad se consulta en cada planta y cualquier valor >= 2 usa la distancia completa
        const float farDist   = g_fx.GetFxQuality() >= FX_QUALITY_HIGH ? m_farDist : m_farDist * 0.5f;
        const float farDist20 = farDist + 20.0f;
        if (camDist > farDist20) {
            continue;
        }

        const auto atomic = plantModelsTab[plant.model_id];
        auto frame = RpAtomicGetFrame(atomic);

        // Como el original: srand con la semilla truncada (_ftol2) y rand() en vez de un generador propio
        srand(static_cast<uint32>(static_cast<int64>(plant.seed))); // La semilla está en [0, 1): siempre 0

        RwRGBA newColorIntensity;
        if (camDist < farDist) {
            newColorIntensity.alpha = plant.color.a;
        } else {
            // Como el original: ((lejos + 20 - dist) * a) * 0.05, floor en double y recorte a [0, 255] tras truncar
            float alpha = (farDist20 - camDist) * (float)plant.color.a;
            alpha = alpha * 0.05f;
            int32 ialpha = (int32)std::floor((double)alpha);
            ialpha = std::min(ialpha, 255);
            ialpha = std::max(ialpha, 0);
            newColorIntensity.alpha = (uint8)ialpha;
        }

        // 0x5DAE61
        const auto mult = (uint16)(int32)std::min((float)plant.intensity, 255.0f);
        newColorIntensity.red   = (uint8)(((int32)plant.color.r * mult) >> 8);
        newColorIntensity.green = (uint8)(((int32)plant.color.g * mult) >> 8);
        newColorIntensity.blue  = (uint8)(((int32)plant.color.b * mult) >> 8);

        // 0x5DAC40
        RenderGrassTexture = plant.texture;
        RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), CPPTriPlantBuffer::SetGrassMaterialCB, &newColorIntensity);

        for (auto j = 0; j < plant.num_plants; j++) {
            const float randX = (float)CGeneral::GetRandomNumber() * RAND_RECIPROCAL;
            const float randY = (float)CGeneral::GetRandomNumber() * RAND_RECIPROCAL;
            CVector posn;
            GenPointInTriangle(posn, plant.V1, plant.V2, plant.V3, randX, randY);

            if (GrassDistance(posn, m_vecCameraPos) < m_closeDist - 2.0f) {
                // Como el original: se consumen los tres números aleatorios aunque no se dibuje
                CGeneral::GetRandomNumber();
                CGeneral::GetRandomNumber();
                CGeneral::GetRandomNumber();
                continue;
            }

            RwFrameTranslate(frame, &posn, rwCOMBINEREPLACE);

            const float scaleVarXY = plant.scale_var_xy;
            const float xy = (float)CGeneral::GetRandomNumber() * RAND_RECIPROCAL * scaleVarXY + plant.scale.x;
            RwFrameGetMatrix(frame)->right.x *= xy;
            RwFrameGetMatrix(frame)->up.y *= xy;

            const float scaleVarZ = plant.scale_var_z;
            RwFrameGetMatrix(frame)->at.z *= (float)CGeneral::GetRandomNumber() * RAND_RECIPROCAL * scaleVarZ + plant.scale.y;

            const float windBendVar = plant.wind_bend_var;
            const float xy1 = ((float)CGeneral::GetRandomNumber() * RAND_RECIPROCAL * windBendVar + 1.0f) * (m_windBending * plant.wind_bend_scale);
            RwFrameGetMatrix(frame)->at.x = xy1;
            RwFrameGetMatrix(frame)->at.y = xy1;

            RwMatrixUpdate(RwFrameGetMatrix(frame));
            RpAtomicRender(atomic);
        }
    }
}

// 0x5DB250
void CGrassRenderer::FlushTriPlantBuffer() {
    gTriPlantBuf.Flush();
}

// 0x5DACE0
RpAtomic** CGrassRenderer::GetPlantModelsTab(uint32 type) {
    return gTriPlantBuf.GetPlantModelsTab(type);
}

// 0x5DACC0
bool CGrassRenderer::SetPlantModelsTab(uint32 type, RpAtomic** atomics) {
    return gTriPlantBuf.SetPlantModelsTab(type, atomics); // Como el original: devuelve si el índice era válido (al)
}

// 0x5DABE0
void CGrassRenderer::SetCloseFarAlphaDist(float fCloseDist, float fFarDist) {
    m_closeDist = fCloseDist;
    m_farDist   = fFarDist;
}

// 0x5DABB0
void CGrassRenderer::SetCurrentScanCode(uint16 scanCode) {
    g_GrassCurrentScanCode = scanCode;
}

// 0x5DABC0
void CGrassRenderer::SetGlobalCameraPos(const CVector& cameraPosn) {
    m_vecCameraPos = cameraPosn;
}

// 0x5DAC00
void CGrassRenderer::SetGlobalWindBending(float bending) {
    m_windBending = bending;
}

// Interpolates inside the triangle (v1,v2,v3) with specific step values
// Original name unknown
// 0x5DAB00 (inlined)
CVector& CGrassRenderer::GenPointInTriangle(CVector& outPosn, const CVector& v1, const CVector& v2, const CVector& v3, float stepA, float stepB) {
    if (stepA + stepB > 1.0f) {
        stepA = 1.0f - stepA;
        stepB = 1.0f - stepB;
    }
    outPosn = (1.0f - stepA - stepB) * v1 + stepA * v2 + stepB * v3;
    return outPosn;
}
