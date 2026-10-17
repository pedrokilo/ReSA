// 0x6FFBB0
float CPointLights::GenerateLightsAffectingObject(const CVector* point, float* totalLighting, CEntity* entity) {
    float result = 1.0f;
    if (std::bit_cast<int32>(NumLights) > 0) {
        uint32 counter = 0;
        int32 index = 0;
        do {
            float args[6];
            if (PointLightsGenerateOneCpp(&aLights[index], &point->x, totalLighting, entity, &result, args)) {
                AddAnExtraDirectionalLight(Scene.m_pRpWorld, args[0], args[1], args[2], args[3], args[4], args[5]);
            }
            ++counter;
            index = static_cast<int16>(counter);
        } while (index < std::bit_cast<int32>(NumLights));
    }
    return result;
}

// 0x6FFE70
float CPointLights::GetLightMultiplier(const CVector* point) {
    return PointLightsLightMultiplierFltCpp(&point->x, aLights, std::bit_cast<int32>(NumLights));
}

// 0x6FFFF0
bool CPointLights::ProcessVerticalLineUsingCache(CVector point, float* outZ) {
    const int32 hit = PointLightsCacheFindCpp(&aCachedMapReads[0].x, &point.x);
    if (hit >= 0) {
        std::memcpy(outZ, &aCachedMapReadResults[hit], sizeof(float));
        return true;
    }
    CColPoint colPoint;
    CEntity* entity;
    if (!CWorld::ProcessVerticalLine(point, PointLightsVerticalEndZCpp(point.z), colPoint, entity, true, false, false, false, true, false, nullptr)) {
        return false;
    }
    const int32 slot = NextCachedValue;
    PointLightsFloatCopyCpp(&colPoint.m_vecPoint.z, &aCachedMapReadResults[slot]);
    std::memcpy(&aCachedMapReads[slot], &point, sizeof(point));
    NextCachedValue = (slot + 1) % MAX_POINT_LIGHTS;
    PointLightsFloatCopyCpp(&colPoint.m_vecPoint.z, outZ);
    return true;
}
