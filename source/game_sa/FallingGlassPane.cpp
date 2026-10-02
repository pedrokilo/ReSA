/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CFallingGlassPane
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

#include "FallingGlassPane.h"
#include "Glass.h"

void CFallingGlassPane::InjectHooks() {
    RH_ScopedClass(CFallingGlassPane);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Update, 0x71AA10);
    RH_ScopedInstall(Render, 0x71B100);
}

// 0x71AA10
void CFallingGlassPane::Update() {
    if (CTimer::GetTimeInMS() < m_nCreatedTime)
        return;

    auto& pos = m_Matrix.GetPosition();

    // Como el original: (paso * velocidad) * 0.35f, y las caídas multiplican por 0.01f / 0.02f en vez de dividir.
    if (m_f6F) {
        pos.x += CTimer::GetTimeStep() * m_Velocity.x * 0.35f;
        pos.y += CTimer::GetTimeStep() * m_Velocity.y * 0.35f;
        pos.z += CTimer::GetTimeStep() * m_Velocity.z * 0.35f;
        m_Velocity.z -= CTimer::GetTimeStep() * 0.01f;
    } else {
        pos.x += CTimer::GetTimeStep() * m_Velocity.x;
        pos.y += CTimer::GetTimeStep() * m_Velocity.y;
        pos.z += CTimer::GetTimeStep() * m_Velocity.z;
        m_Velocity.z -= CTimer::GetTimeStep() * 0.02f;
    }

    m_Matrix.GetRight()   += CrossProduct(m_RandomNumbers, m_Matrix.GetRight());
    m_Matrix.GetForward() += CrossProduct(m_RandomNumbers, m_Matrix.GetForward());
    m_Matrix.GetUp()      += CrossProduct(m_RandomNumbers, m_Matrix.GetUp());

    if (pos.z < m_fGroundZ) {
        m_bExist = false;
        // Como el original: los cristales también nacen a la altura del suelo, no en la posición del panel.
        const CVector groundPos{ pos.x, pos.y, m_fGroundZ };
        AudioEngine.ReportGlassCollisionEvent(AE_GLASS_HIT_GROUND, groundPos);
        if (!m_f6F) {
            RwRGBA color{ 255, 255, 255, 32 };
            for (auto i = 0; i < 4; i++) {
                // Como el original: rand() * (1/32767) * 0.04f + 0.02f en línea.
                g_fx.AddGlass(groundPos, color, (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL * 0.04f + 0.02f, 1);
            }
        }
    }
}

RwRGBA CFallingGlassPane::CalculateHiLightPolyColor() {
    const auto alpha = CGlass::CalcAlphaWithNormal(Normalized(m_Matrix.GetForward()));
    // Como el original: diferencia de tiempo con signo limitada a [0, 500], (delta * 0.002f) * alfa truncado
    // con _ftol2 y, en paneles m_f6F, mínimo de 64 aplicado al byte ya truncado.
    const auto delta = std::clamp((int32)(CTimer::GetTimeInMS() - m_nCreatedTime), 0, 500);
    auto color = (uint8)(int64)((float)delta * 0.002f * (float)alpha);
    if (m_f6F && color < 64) {
        color = 64;
    }
    return { color, color, color, color };
}

RwRGBA CFallingGlassPane::CalculateShatterPolyColor(float camDist) {
    // Como el original: multiplica por 0.1f en vez de dividir entre 10.
    if (camDist > 30.f) {
        const auto alpha = (uint8)(int64)((1.0f - (camDist - 30.f) * 0.1f) * 140.f);
        return { alpha, alpha, alpha, alpha };
    }
    return { 140u, 140u, 140u, 140u};
}

// 0x71B100
void CFallingGlassPane::Render() {
    constexpr RwImVertexIndex indices[]{ 0u, 1u, 2u, 0u, 2u, 1u };

    // Como el original: la distancia a la cámara se calcula al principio, sumando z² + y² + x².
    const auto camDelta = TheCamera.GetPosition() - m_Matrix.GetPosition();
    const float camDist = std::sqrt(camDelta.z * camDelta.z + camDelta.y * camDelta.y + camDelta.x * camDelta.x);

    // Render HiLightPoly's
    CVector vertices[3]{};
    {
        const auto color = CalculateHiLightPolyColor();

        // Como el original: vacía el lote de brillos si no caben los 3 vértices y 6 índices nuevos.
        if (CGlass::HiLightPolyIndicesIdx >= 2041 || (int32)CGlass::H1iLightPolyVerticesIdx >= 1020) {
            CGlass::RenderHiLightPolys();
        }

        constexpr RwTexCoords uv[3] = {
            { 0.5f, 0.5f },
            { 0.5f, 0.6f },
            { 0.6f, 0.6f },
        };
        for (auto i = 0u; i < 3u; i++) {
            auto& vertex = TempBufferVertices.m_3d[CGlass::H1iLightPolyVerticesIdx + i];

            const auto pos2DOS = CGlass::PanePolyPositions[m_nPieceIndex][i] - CGlass::PanePolyCenterPositions[m_nPieceIndex];
            vertices[i] = m_Matrix.TransformPoint({ pos2DOS.x, 0.f, pos2DOS.y });
            RxObjSpace3DVertexSetPos(&vertex, &vertices[i]);

            RxObjSpace3DVertexSetU(&vertex, uv[i].u);
            RxObjSpace3DVertexSetV(&vertex, uv[i].v);

            RxObjSpace3DVertexSetPreLitColor(&vertex, &color);
        }

        // Store indices
        uint32 i = CGlass::HiLightPolyIndicesIdx;
        for (const auto idx : indices) {
            aTempBufferIndices[i++] = (RwImVertexIndex)(CGlass::H1iLightPolyVerticesIdx + idx);
        }

        CGlass::H1iLightPolyVerticesIdx += 3;
        CGlass::HiLightPolyIndicesIdx += 6;
    }

    if (m_bRenderShatter) {
        // Como el original: vacía el lote de fragmentos si no caben los nuevos.
        if (CGlass::ShatteredIndicesBaseIdx >= 3065 || CGlass::ShatteredVerticesBaseIdx >= 1532) {
            CGlass::RenderShatteredPolys();
        }

        const auto color = CalculateShatterPolyColor(camDist);
        for (auto i = 0u; i < 3u; i++) {
            auto vertex = &TempBufferVertices.m_3d[CGlass::ShatteredVerticesBaseIdx + i];

            RxObjSpace3DVertexSetPos(vertex, &vertices[i]);
            RxObjSpace3DVertexSetPreLitColor(vertex, &color);

            const auto uv = CGlass::PanePolyPositions[m_nPieceIndex][i] * m_fSize * 4.f;
            RxObjSpace3DVertexSetU(vertex, uv.x);
            RxObjSpace3DVertexSetV(vertex, uv.y);
        }

        // Como el original: los índices van en ShatteredIndicesBaseIdx, valen base - 1024 + idx y ambos
        // contadores avanzan.
        uint32 i = CGlass::ShatteredIndicesBaseIdx;
        for (const auto idx : indices) {
            aTempBufferIndices[i++] = (RwImVertexIndex)(CGlass::ShatteredVerticesBaseIdx - 1024 + idx);
        }

        CGlass::ShatteredIndicesBaseIdx += 6;
        CGlass::ShatteredVerticesBaseIdx += 3;
    }
}
