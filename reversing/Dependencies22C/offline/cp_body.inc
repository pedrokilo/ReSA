// 0x4A41E0
void FxEmitter_c::CreateParticles(float currentTime, float deltaTime) {
    EmissionInfo_t emission;
    m_PrimBP->m_FxInfoManager.ProcessEmissionInfo(currentTime, deltaTime, m_System->m_SystemBP->m_fLength, m_System->m_UseConstTime, &emission);

    // 0x4A421C..0x4A4331: visibilidad por distancia, acumulacion de la intensidad (se guarda siempre) y comprobaciones de viento, lluvia y m_fEmissionIntensity >= 1.
    if (!FxEmitterAccumulateCpp(&m_fEmissionIntensity, &m_System->m_fCameraDistance, m_System->m_nRateMult, m_PrimBP->m_FxInfoManager.m_nLodStart, m_PrimBP->m_FxInfoManager.m_nLodEnd, emission.m_fCount, &emission.m_fMinWind, &emission.m_fMaxWind, &emission.m_fMinRain, &emission.m_fMaxRain, &CWeather::Wind, &CWeather::Rain)) {
        return;
    }

    auto* const mat  = g_fxMan.FxRwMatrixCreate();
    auto* const mat1 = g_fxMan.FxRwMatrixCreate();
    RwMatrixUpdate(&m_System->m_LocalMatrix);
    if (m_System->m_ParentMatrix) {
        RwMatrixMultiply(mat1, &m_System->m_LocalMatrix, m_System->m_ParentMatrix);
    } else {
        *mat1 = m_System->m_LocalMatrix;
    }

    auto* const mat2 = g_fxMan.FxRwMatrixCreate();
    m_PrimBP->GetRWMatrix(*mat2);
    RwMatrixMultiply(mat, mat2, mat1);
    g_fxMan.FxRwMatrixDestroy(mat2);

    // 0x4A43B0..0x4A4416: el EXE vuelve a convertir la intensidad (_ftol2) tras cada particula. CreateParticle (0x4A2580) sigue siendo la del EXE: la reconstruccion de
    // FxEmitter_c::CreateParticle de este archivo esta incompleta (todo) y no se llama desde aqui hasta que se valide.
    for (int32 step = 0; step < FxEmitterTruncateCpp(&m_fEmissionIntensity); ++step) {
        FxPrtMult_c prtMult;
        const float timeSince = FxEmitterTimeSinceCpp(step, &m_fEmissionIntensity, deltaTime);
        plugin::CallMethodAndReturn<FxEmitterPrt_c*, 0x4A2580, FxEmitter_c*, const EmissionInfo_t*, RwMatrix*, const CVector*, float, const FxPrtMult_c*, float, bool>(
            this, &emission, mat, nullptr, timeSince, &prtMult, 1.2f, m_System->m_createLocal);
    }
    FxEmitterTakeWholeCpp(&m_fEmissionIntensity);

    g_fxMan.FxRwMatrixDestroy(mat1);
    g_fxMan.FxRwMatrixDestroy(mat);
}
