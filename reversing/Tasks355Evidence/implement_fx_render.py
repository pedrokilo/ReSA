from pathlib import Path
p=Path('source/game_sa/Fx/FxEmitterBP.cpp');s=p.read_text().replace('#include "MovementInfo.h"','#include "MovementInfo.h"\n#include "RenderInfo.h"\n#include "FxPrtMult.h"\n#include "Fx.h"')
helper='''
namespace {
void RenderEmitterParticles355(FxEmitterBP_c& emitter, RwCamera* camera, float& brightness, bool heat) {
    auto* raster = emitter.m_apTextures[0]->raster;
    if (!emitter.m_Particles.GetNumItems()) { return; }
    if (heat) {
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, reinterpret_cast<void*>(1));
        RwRenderStateSet(rwRENDERSTATESRCBLEND, reinterpret_cast<void*>(5));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, reinterpret_cast<void*>(2));
    } else {
        constexpr RwBlendFunction blends[]{rwBLENDZERO,rwBLENDONE,rwBLENDSRCCOLOR,rwBLENDINVSRCCOLOR,rwBLENDSRCALPHA,rwBLENDINVSRCALPHA,rwBLENDDESTALPHA,rwBLENDINVDESTALPHA,rwBLENDDESTCOLOR,rwBLENDINVDESTCOLOR,rwBLENDSRCALPHASAT};
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, reinterpret_cast<void*>(uintptr_t(emitter.m_bAlphaOn)));
        RwRenderStateSet(rwRENDERSTATESRCBLEND, reinterpret_cast<void*>(uintptr_t(blends[emitter.m_bAlphaOn ? emitter.m_nSrcBlendId : 1])));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, reinterpret_cast<void*>(uintptr_t(blends[emitter.m_bAlphaOn ? emitter.m_nDstBlendId : 0])));
    }
    RenderBegin(raster, nullptr, 1);
    const auto* cameraMatrix = RwFrameGetMatrix(RwCameraGetFrame(camera));
    constexpr float byteScale = std::bit_cast<float>(0x3B808081u);
    const auto cross = [](const CVector& a, const CVector& b) { return CVector{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; };
    const auto normalise = [](CVector& value) { RwV3dNormalize(&value, &value); };
    for (auto* item = emitter.m_Particles.GetHead(); item; item = emitter.m_Particles.GetNext(item)) {
        auto* particle = item->AsFxEmitterPrt();
        const auto* bytes = reinterpret_cast<const uint8*>(particle);
        auto position = particle->m_Pos;
        if (particle->m_bLocalToSystem) {
            auto* matrix = g_fxMan.FxRwMatrixCreate();
            particle->m_System->GetCompositeMatrix(matrix);
            RwV3dTransformPoints(&position, &particle->m_Pos, 1, matrix);
            g_fxMan.FxRwMatrixDestroy(matrix);
        }
        RenderInfo_t info;
        emitter.m_FxInfoManager.ProcessRenderInfo(particle->m_System->m_fCurrentTime, particle->m_fCurrentLife/particle->m_fTotalLife, 0.f, particle->m_System->m_SystemBP->m_fLength, false, &info);
        if (!heat && info.m_SmokeType >= 0) {
            const auto random = float(CGeneral::GetRandomNumber());
            const auto bias = ((info.m_SmokeBrightness - -info.m_SmokeBrightness) * random * std::bit_cast<float>(0x38000100u) + -info.m_SmokeBrightness) * byteScale;
            FxPrtMult_c mult;
            const auto clamp = [](float f) { return f > 1.f ? 1.f : f < 0.f ? 0.f : f; };
            mult.m_Color.red = clamp(info.m_SmokeColor.red * byteScale + bias);
            mult.m_Color.green = clamp(info.m_SmokeColor.green * byteScale + bias);
            mult.m_Color.blue = clamp(info.m_SmokeColor.blue * byteScale + bias);
            mult.m_Color.alpha = info.m_SmokeColor.alpha * byteScale;
            mult.m_fSize = info.m_SmokeSize;
            mult.m_Rot = 1.f;
            mult.m_fLife = info.m_SmokeLife;
            g_fx.m_SmokeHuge->AddParticle(particle->m_Pos, CVector{}, 0.f, mult, -1.f, 1.2f, .6f, false);
        }
        auto* orientation = g_fxMan.FxRwMatrixCreate();
        RwMatrixSetIdentity(orientation);
        CVector at;
        CVector trailDelta{};
        if (!heat && info.m_nTrailScreenMode > 0) {
            if (info.m_nTrailScreenMode == 2) {
                const auto step = CTimer::ms_fTimeStep * .02f;
                const auto previousCamera = *reinterpret_cast<const CVector*>(0xB6FA14);
                const auto previousRelative = particle->m_Pos - particle->m_Velocity * step - previousCamera;
                trailDelta = (particle->m_Pos - CVector{cameraMatrix->pos} - previousRelative) * info.m_fTrailTime;
            } else {
                const auto previous = particle->m_Pos - particle->m_Velocity * info.m_fTrailTime;
                trailDelta = particle->m_Pos - previous;
            }
            auto direction = trailDelta;
            if (direction.x == 0.f && direction.y == 0.f && direction.z == 0.f) { direction.z = 1.f; }
            else { normalise(direction); }
            auto view = particle->m_Pos - CVector{cameraMatrix->pos};
            normalise(view);
            const auto right = cross(direction, view);
            at = cross(direction, right);
            orientation->right = right;
            orientation->up = direction;
            orientation->at = at;
        } else if (info.m_bHasDir) {
            auto direction = info.m_bUseVel ? particle->m_Velocity : info.m_Direction;
            if (!heat && info.m_bUseVel && direction.x == 0.f && direction.y == 0.f && direction.z == 0.f) { direction.z = 1.f; }
            normalise(direction);
            auto view = position - CVector{cameraMatrix->pos};
            normalise(view);
            const auto right = cross(direction, view);
            at = cross(direction, right);
            orientation->right = right;
            orientation->up = direction;
            orientation->at = at;
        } else {
            *orientation = info.m_bIsFlat ? info.m_FlatMatrix : *cameraMatrix;
            at = orientation->at;
        }
        if (bytes[0x36] != 0xFF) { particle->m_CurrentRotation = float(bytes[0x36]) + float(bytes[0x36]); }
        while (particle->m_CurrentRotation < 0.f) { particle->m_CurrentRotation += 360.f; }
        while (particle->m_CurrentRotation >= 360.f) { particle->m_CurrentRotation -= 360.f; }
        CVector right = orientation->right, up = orientation->up;
        if (!(particle->m_CurrentRotation <= 0.f)) {
            RotateVecAboutVec(right, right, at, particle->m_CurrentRotation * std::bit_cast<float>(0x3C8EFA2Du));
            up = cross(at, right);
        }
        g_fxMan.FxRwMatrixDestroy(orientation);
        auto sizeX = (float(bytes[0x32]) * byteScale - .5f) * info.m_fSizeXBias + info.m_fSizeX;
        auto sizeY = (float(bytes[0x33]) * byteScale - .5f) * info.m_fSizeYBias + info.m_fSizeY;
        if (bytes[0x30] != 0xFF) { sizeX = float(bytes[0x30]) * byteScale * sizeX; sizeY = float(bytes[0x30]) * byteScale * sizeY; }
        CRGBA color{0,0,0,info.m_Color1.alpha};
        if (!heat) {
            float channels[]{float(info.m_Color1.red),float(info.m_Color1.green),float(info.m_Color1.blue)};
            const float ranges[]{float(info.m_Color2.red),float(info.m_Color2.green),float(info.m_Color2.blue)};
            for (auto c = 0; c < 3; ++c) {
                if (info.m_nColorType == ERenderColorType::RANGE) { channels[c] = float(bytes[0x32+c])*byteScale*(ranges[c]-channels[c])+channels[c]; }
                else if (info.m_nColorType == ERenderColorType::BRIGHT) {
                    channels[c] += (float(bytes[0x32+c])*.0078125f-1.f)*float(info.m_Color2.alpha);
                    if (channels[c] < 0.f) { channels[c] = 0.f; }
                    if (channels[c] > 255.f) { channels[c] = 255.f; }
                }
                if (bytes[0x2C+c] != 0xFF) { channels[c] = float(bytes[0x2C+c])*byteScale*channels[c]; }
            }
            auto alpha = float(info.m_Color1.alpha);
            if (bytes[0x2F] != 0xFF) { alpha = float(bytes[0x2F])*byteScale*alpha; }
            if (!info.m_bSelfLit) {
                if (bytes[0x35] <= 100) { brightness = float(bytes[0x35]) * .01f; }
                for (auto& channel : channels) { channel *= brightness; }
            }
            color = CRGBA{uint8(int32(channels[0])),uint8(int32(channels[1])),uint8(int32(channels[2])),uint8(int32(alpha))};
        }
        auto* texture = emitter.m_apTextures[0];
        if (info.m_bHasAnimTextures && info.m_nCurrentTexId >= 2 && info.m_nCurrentTexId <= 4) {
            if (auto* other = emitter.m_apTextures[info.m_nCurrentTexId-1]) { texture = other; }
        }
        auto* current = texture->raster;
        if (raster != current) { RenderEnd(); raster = current; RenderBegin(raster, nullptr, 1); }
        auto top = info.m_fSpriteTop, bottom = info.m_fSpriteBottom;
        if (!heat && info.m_nTrailScreenMode > 0) { top = RwV3dLength(&trailDelta); bottom = 0.f; }
        const auto topSize = top * sizeY, bottomSize = bottom * sizeY;
        const auto leftSize = info.m_fSpriteLeft * sizeX, rightSize = info.m_fSpriteRight * sizeX;
        const auto a = right * leftSize + up * topSize + position;
        const auto b = right * rightSize + up * bottomSize + position;
        const auto c = right * rightSize + up * topSize + position;
        const auto d = right * leftSize + up * bottomSize + position;
        RenderAddTri_(a.x,a.y,a.z,b.x,b.y,b.z,c.x,c.y,c.z,0.f,0.f,1.f,1.f,1.f,0.f,color.r,color.g,color.b,color.a,color.r,color.g,color.b,color.a,color.r,color.g,color.b,color.a);
        RenderAddTri_(b.x,b.y,b.z,a.x,a.y,a.z,d.x,d.y,d.z,1.f,1.f,0.f,0.f,0.f,1.f,color.r,color.g,color.b,color.a,color.r,color.g,color.b,color.a,color.r,color.g,color.b,color.a);
    }
    RenderEnd();
}
}
'''
pos=s.index('// 0x4A1940');s=s[:pos]+helper+'\n'+s[pos:]
start=s.index('// 0x4A1940');b=s.index('{',start);end=s.index('\n}',b);s=s[:b+1]+'''\n    if (!m_Particles.GetNumItems()) { return; }
    RenderEmitterParticles355(*this, camera, brightness, true);'''+s[end:]
start=s.index('// 0x4A2C40');b=s.index('{',start);end=s.index('\n}',b);s=s[:b+1]+'''\n    const auto heatEmitter = IsFxInfoPresent(static_cast<eFxInfoType>(0x4008));
    if (doHeatHaze) {
        if (m_FxInfoManager.m_bHasHeatHazeParticleEmitter) { RenderHeatHaze(camera, txdHashKey, brightness); }
        return;
    }
    if (heatEmitter) {
        if (m_Particles.GetNumItems() > 0) { g_fxMan.m_bHeatHazeEnabled = true; }
        return;
    }
    if (!m_Particles.GetNumItems()) { return; }
    RenderEmitterParticles355(*this, camera, brightness, false);'''+s[end:]
p.write_text(s)
