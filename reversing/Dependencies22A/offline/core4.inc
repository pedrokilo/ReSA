// 0x6FFBB0
float CPointLights::GenerateLightsAffectingObject(const CVector* point, float* totalLighting, CEntity* entity) {
    // Como el original (0x6FFBB0..0x6FFE68): acumulador de 1.0f en la pila, bucle con contador de 16 bits (MOVSX BX) que relee NumLights en cada vuelta y, por cada luz,
    // el tramo x87 de X87Math (PointLightsGenerateOneCpp); sólo si la luz aporta una dirección se llama a AddAnExtraDirectionalLight.
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
    // El EXE no tiene llamadores. Devuelve F + S (producto de las luces ANTILIGHT y suma de las demás) en ST0 sin redondear a float: ver PointLightsLightMultiplierDblCpp.
    return PointLightsLightMultiplierFltCpp(&point->x, aLights, std::bit_cast<int32>(NumLights));
}

// 0x6FFFF0
bool CPointLights::ProcessVerticalLineUsingCache(CVector point, float* outZ) {
    // Como el original: busca el punto en las 32 entradas de la caché (comparación x87 ordenada) y devuelve el z guardado tal cual (copia cruda); si no está, traza una línea
    // vertical de 20 unidades hacia abajo (sólo edificios y dummies) y guarda el z del impacto (FLD/FSTP: un NaN señalizante se silencia) con una escritura circular.
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

// Llamada a CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect con los argumentos que calcula PointLightsFogSpriteCpp (0x70076A..0x700817 y 0x700AAC..0x700B6B).
static void FogEmitSprite(const CPointLight& light, const float& alpha, const RwV3d& screen, float w, float h, int32 tab, uint32 timeValue, bool pointLight) {
    PointLightsFogSprite sprite;
    PointLightsFogSpriteCpp(&light, &alpha, &screen.x, w, h, &StaticRef<float>(0x8D5068), tab, static_cast<int32>(timeValue), pointLight, &sprite);
    CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect(sprite.x, sprite.y, sprite.z, sprite.w, sprite.h, static_cast<uint8>(sprite.red), static_cast<uint8>(sprite.green),
        static_cast<uint8>(sprite.blue), static_cast<int16>(sprite.intensity), sprite.recipNearZ, sprite.angle, 255);
}

// Luz direccional (0x7003AB..0x700851): rejilla de 4 unidades alrededor del segmento de 12 unidades que sale de la luz.
static void FogDirectionalLight(const CPointLight& light, const float& intensity) {
    int32 bounds[4];
    if (!PointLightsFogDirBoundsCpp(&light, bounds)) {
        return;
    }
    for (int32 gx = bounds[0]; gx <= bounds[1]; gx += 4) {
        for (int32 gy = bounds[2]; gy <= bounds[3]; gy += 4) {
            float origin[3], endZ;
            int32 tab;
            if (!PointLightsFogDirCellPreCpp(&light, gx, gy, origin, &endZ, &tab)) {
                continue;
            }
            CColPoint colPoint;
            CEntity* entity;
            if (!CWorld::ProcessVerticalLine(*reinterpret_cast<const CVector*>(origin), endZ, colPoint, entity, true, false, false, false, true, false, nullptr)) {
                continue;
            }
            float posn[3], alpha;
            if (!PointLightsFogDirCellPostCpp(&light, origin, &colPoint.m_vecPoint.z, &intensity, posn, &alpha)) {
                continue;
            }
            RwV3d screen;
            float w, h;
            if (!CSprite::CalcScreenCoors(*reinterpret_cast<const RwV3d*>(posn), &screen, &w, &h, true, true)) {
                continue;
            }
            FogEmitSprite(light, alpha, screen, w, h, tab, CTimer::GetTimeInMS() & 0x1FFFu, false);
        }
    }
}

// Luces de punto y de sólo niebla (0x700856..0x700BA9): suelo de la luz con la caché y rejilla de 2 unidades en un cuadrado de lado 2·size.
static void FogPointLight(const CPointLight& light, const float& intensity, const float& size) {
    float groundZ;
    if (!CPointLights::ProcessVerticalLineUsingCache(light.m_vecPosn, &groundZ)) {
        return;
    }
    int32 bounds[4];
    if (!PointLightsFogPointBoundsCpp(&light, &size, bounds)) {
        return;
    }
    for (int32 gx = bounds[0]; gx <= bounds[1]; gx += 2) {
        for (int32 gy = bounds[2]; gy <= bounds[3]; gy += 2) {
            float posn[3], alpha;
            int32 tab;
            if (!PointLightsFogPointCellPreCpp(&light, gx, gy, &size, &TheCamera.GetPosition().x, &intensity, &groundZ, posn, &alpha, &tab)) {
                continue;
            }
            RwV3d screen;
            float w, h;
            if (!CSprite::CalcScreenCoors(*reinterpret_cast<const RwV3d*>(posn), &screen, &w, &h, true, true)) {
                continue;
            }
            FogEmitSprite(light, alpha, screen, w, h, tab, (static_cast<uint32>(tab) * 0x8FCu + CTimer::GetTimeInMS()) & 0x7FFFu, true);
        }
    }
}

// 0x7002D0
void CPointLights::RenderFogEffect() {
    ZoneScoped;

    // Como el original (0x7002D0..0x700BCD): fuera de cinemáticas, estados de render fijos, buffer de sprites y una pasada por las luces con niebla (tipos de niebla 1 y 2);
    // termina vaciando el buffer (0x70CF20, salto final en el EXE).
    if (CCutsceneMgr::IsRunning()) {
        return;
    }
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(RwTextureGetRaster(gpCloudTex)));
    CSprite::InitSpriteBuffer();

    if (std::bit_cast<int32>(NumLights) > 0) {
        int32 index = 0;
        do {
            const CPointLight& light = aLights[index];
            float size, intensity;
            bool visible = true;
            if (light.m_nFogType == 1) {
                size = 9.0f;
                visible = PointLightsFogIntensityCpp(&CWeather::Foggyness, &intensity);
            } else if (light.m_nFogType == 2) {
                intensity = 0.4f;
                size = 3.0f;
            } else {
                visible = false;
            }
            if (visible) {
                if (light.m_nType == PLTYPE_DIRECTIONAL) {
                    FogDirectionalLight(light, intensity);
                } else if (light.m_nType == PLTYPE_POINTLIGHT || light.m_nType == PLTYPE_ONLYFOGEFFECT_ALWAYS || light.m_nType == PLTYPE_ONLYFOGEFFECT) {
                    FogPointLight(light, intensity, size);
                }
            }
            ++index;
        } while (index < std::bit_cast<int32>(NumLights));
    }
    CSprite::FlushSpriteBuffer();
}
