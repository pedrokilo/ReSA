/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRegisteredMotionBlurStreak
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

#include "RegisteredMotionBlurStreak.h"

#include "MotionBlurStreaks.h"

void CRegisteredMotionBlurStreak::InjectHooks()
{
    RH_ScopedClass(CRegisteredMotionBlurStreak);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Update, 0x721EE0);
    RH_ScopedInstall(Render, 0x721F70);
}

void CRegisteredMotionBlurStreak::Update()
{
    bool bAlive = false;
    for (int32 i = 2; i >= 1; --i) {
        m_avecLeftPoints[i] = m_avecLeftPoints[i - 1];
        m_avecRightPoints[i] = m_avecRightPoints[i - 1];
        m_acRenderHistory[i] = m_acRenderHistory[i - 1];
        if (bAlive || m_acRenderHistory[i])
            bAlive = true;
    }

    m_acRenderHistory[0] = false;
    if (!bAlive)
        m_nId = 0;
}

void CRegisteredMotionBlurStreak::Render()
{
    for (int32 iInd = 0; iInd < 2; ++iInd) {
        const auto iAlpha = 255 - (iInd * 85);
        // Como el original: exige el historial del tramo actual (iInd), no siempre el [0].
        if (!m_acRenderHistory[iInd] || !m_acRenderHistory[iInd + 1])
            continue;

        // Como el original: (iAlpha / 3) entero por el alfa, a float, por 1/255 (0x859A3C) y truncado con _ftol2.
        const auto ucAlpha1 = static_cast<RwUInt8>(static_cast<int64>(static_cast<float>(iAlpha / 3 * m_color.a) * (1.0f / 255.0f)));
        const auto ucAlpha2 = static_cast<RwUInt8>(static_cast<int64>(static_cast<float>((iAlpha - 85) / 3 * m_color.a) * (1.0f / 255.0f)));

        auto rwColor1 = CRGBA(m_color.r, m_color.g, m_color.b, ucAlpha1).ToRwRGBA();
        auto rwColor2 = CRGBA(m_color.r, m_color.g, m_color.b, ucAlpha2).ToRwRGBA();

        RxObjSpace3DVertexSetPreLitColor(&CMotionBlurStreaks::aStreakVertices[0], &rwColor1);
        RxObjSpace3DVertexSetPreLitColor(&CMotionBlurStreaks::aStreakVertices[1], &rwColor1);
        RxObjSpace3DVertexSetPreLitColor(&CMotionBlurStreaks::aStreakVertices[2], &rwColor2);
        RxObjSpace3DVertexSetPreLitColor(&CMotionBlurStreaks::aStreakVertices[3], &rwColor2);

        RxObjSpace3DVertexSetPos(&CMotionBlurStreaks::aStreakVertices[0], &m_avecLeftPoints[iInd]);
        RxObjSpace3DVertexSetPos(&CMotionBlurStreaks::aStreakVertices[1], &m_avecRightPoints[iInd]);
        RxObjSpace3DVertexSetPos(&CMotionBlurStreaks::aStreakVertices[2], &m_avecLeftPoints[iInd + 1]);
        RxObjSpace3DVertexSetPos(&CMotionBlurStreaks::aStreakVertices[3], &m_avecRightPoints[iInd + 1]);

        LittleTest();
        if (RwIm3DTransform(CMotionBlurStreaks::aStreakVertices, CMotionBlurStreaks::NUM_STREAK_VERTICES, nullptr, rwMATRIXTYPENORMAL)) {
            RwIm3DRenderIndexedPrimitive(RwPrimitiveType::rwPRIMTYPETRILIST, CMotionBlurStreaks::aIndices, CMotionBlurStreaks::NUM_STREAK_INDICES);
            RwIm3DEnd();
        }
    }
}
