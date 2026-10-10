/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxEmitterBP_c
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxEmitterBP.h"
#include "FxEmitter.h"
#include "FxPrimBP.h"
#include "FxEmitterPrt.h"
#include "FxInfo.h"
#include "FxInfoManager.h"
#include "MovementInfo.h"
#include "RenderInfo.h"
#include "FxPrtMult.h"
#include "Fx.h"
#include "WaterLevel.h"

#include "Particle.h"
#include "FxTools.h"

void FxEmitterBP_c::InjectHooks() {
    RH_ScopedVirtualClass(FxEmitterBP_c, 0x85A788, 7);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4A18D0);
    RH_ScopedInstall(RenderHeatHaze, 0x4A1940);
    RH_ScopedInstall(UpdateParticle, 0x4A21D0);
    RH_ScopedVMTInstall(CreateInstance, 0x4A2B40); // bad
    RH_ScopedVMTInstall(Update, 0x4A2BC0);
    RH_ScopedVMTInstall(Load, 0x5C25F0);
    RH_ScopedVMTInstall(LoadTextures, 0x5C0A30, {.reversed = true});
    RH_ScopedVMTInstall(Render, 0x4A2C40);
    RH_ScopedVMTInstall(FreePrtFromPrim, 0x4A2510);
}

// 0x4A18D0
FxEmitterBP_c::FxEmitterBP_c() : FxPrimBP_c() {
    m_Type = 0;
}


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
            const auto bias = ((random * std::bit_cast<float>(0x38000100u)) * (info.m_SmokeBrightness - -info.m_SmokeBrightness) + -info.m_SmokeBrightness) * byteScale;
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
            RotateVecAboutVec(right, right, at, particle->m_CurrentRotation * std::bit_cast<float>(0x3C8EFA2Eu));
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
                    channels[c] += (float(bytes[0x32])*.0078125f-1.f)*float(info.m_Color2.alpha);
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

// 0x4A1940
void FxEmitterBP_c::RenderHeatHaze(RwCamera* camera, uint32 txdHashKey, float brightness) {
    if (!m_Particles.GetNumItems()) { return; }
    RenderEmitterParticles355(*this, camera, brightness, true);
}

// 0x4A21D0
bool FxEmitterBP_c::UpdateParticle(float deltaTime, FxEmitterPrt_c* emitter) {
    auto* system = emitter->m_System;
    const auto step = float(system->m_nTimeMult) * .001f * deltaTime;
    emitter->m_fCurrentLife += step;
    if (emitter->m_fCurrentLife >= emitter->m_fTotalLife) { return true; }
    emitter->m_Pos.x = step * emitter->m_Velocity.x + emitter->m_Pos.x;
    emitter->m_Pos.y = step * emitter->m_Velocity.y + emitter->m_Pos.y;
    emitter->m_Pos.z = step * emitter->m_Velocity.z + emitter->m_Pos.z;
    MovementInfo_t movement;
    movement.m_Pos = emitter->m_Pos;
    movement.m_Vel = emitter->m_Velocity;
    m_FxInfoManager.ProcessMovementInfo(system->m_fCurrentTime, emitter->m_fCurrentLife / emitter->m_fTotalLife, step, system->m_SystemBP->m_fLength, false, &movement);
    emitter->m_Pos = movement.m_Pos;
    emitter->m_Velocity = movement.m_Vel;
    if (movement.m_bHasFloatInfo || movement.m_bHasUnderwaterInfo) {
        float water = float(system->m_nTimeMult);
        const auto found = CWaterLevel::GetWaterLevel(emitter->m_Pos.x, emitter->m_Pos.y, emitter->m_Pos.z, water, 1, nullptr);
        if (movement.m_bHasFloatInfo && found && emitter->m_Pos.z < water) { emitter->m_Pos.z = water; }
        if (movement.m_bHasUnderwaterInfo && (!found || water < emitter->m_Pos.z)) { return true; }
    }
    const auto* bytes = reinterpret_cast<const uint8*>(emitter);
    const auto random = float(bytes[0x32]), multiplier = float(bytes[0x31]);
    const auto& rotations = movement.m_Rot;
    const auto positive = rotations[0] > 0.f || rotations[1] > 0.f;
    const auto negative = rotations[2] > 0.f || rotations[3] > 0.f;
    constexpr float byteScale = std::bit_cast<float>(0x3B808081u);
    if (!positive || !negative) {
        if (positive) {
            emitter->m_CurrentRotation = (((random * byteScale) * (rotations[1] - rotations[0]) + rotations[0]) * multiplier * step * byteScale) + emitter->m_CurrentRotation;
        } else if (negative) {
            const auto amount = ((random * byteScale) * (rotations[3] - rotations[2]) + rotations[2]) * multiplier;
            emitter->m_CurrentRotation -= amount * step * byteScale;
        }
    } else if (bytes[0x32] < 128) {
        emitter->m_CurrentRotation = (((random * .0078125f) * (rotations[1] - rotations[0]) + rotations[0]) * multiplier * step * byteScale) + emitter->m_CurrentRotation;
    } else {
        const auto amount = (((random - 128.f) * .0078125f) * (rotations[3] - rotations[2]) + rotations[2]) * multiplier;
        emitter->m_CurrentRotation -= amount * step * byteScale;
    }
    return false;

}

// 0x4A2B40


FxPrim_c* FxEmitterBP_c::CreateInstance() {
    void* const memory = ::operator new(sizeof(FxEmitter_c));
    return memory ? ::new(memory) FxEmitter_c : nullptr;
}

// 0x4A2BC0


void FxEmitterBP_c::Update(float deltaTime) {
    if (!m_Particles.GetNumItems()) return;
    auto* it = m_Particles.GetHead();
    while (it) {
        if (it->m_System->m_nKillStatus == eFxSystemKillStatus::FX_3) {
            it->m_System->m_nKillStatus = eFxSystemKillStatus::FX_KILLED;
        }
        if (it->m_System->m_nPlayStatus != eFxSystemPlayStatus::T2 && UpdateParticle(deltaTime, it->AsFxEmitterPrt())) {
            auto* const next = m_Particles.GetNext(it);
            m_Particles.RemoveItem(it);
            g_fxMan.ReturnParticle(it->AsFxEmitterPrt());
            it = next;
        } else {
            it = m_Particles.GetNext(it);
        }
    }
}

// 0x5C25F0


bool FxEmitterBP_c::Load(FILESTREAM file, int32 version, FxName32_t* textureNames) {
    FxPrimBP_c::Load(file, version, textureNames);

    char line[256], field[128];
    float value;
    ReadLine(file, line, sizeof(line));
    sscanf(line, "%s %f", field, &value);
    m_FxInfoManager.m_nLodStart = uint16(int64(double(value) * 64.0));
    ReadLine(file, line, sizeof(line));
    sscanf(line, "%s %f", field, &value);
    m_FxInfoManager.m_nLodEnd = uint16(int64(double(value) * 64.0));

    return true;
}

// 0x5C0A30


bool FxEmitterBP_c::LoadTextures(FxName32_t* textureNames, int32 version) {
    assert(textureNames);

    const auto LoadTexture = [&](auto ind) -> RwTexture* {
        char mask[64];
        sprintf(mask, "%sm", textureNames[ind]);

        auto* texture = RwTextureRead(textureNames[ind], mask);
        return texture ? texture : RwTextureRead(textureNames[ind], nullptr);
    };

    const auto LoadTextureIfExists = [&](auto ind) {
        assert(&textureNames[ind]);
        if (strncmp(textureNames[ind], "NULL", 5u) != 0) {
            m_apTextures[ind] = LoadTexture(ind);
        }
    };

    m_apTextures[0] = LoadTexture(0);

    if (version > 101) {
        LoadTextureIfExists(1);
        LoadTextureIfExists(2);
        LoadTextureIfExists(3);
    }

    return true;
}

// 0x4A2C40


void FxEmitterBP_c::Render(RwCamera* camera, uint32 txdHashKey, float brightness, bool doHeatHaze) {
    const auto heatEmitter = IsFxInfoPresent(static_cast<eFxInfoType>(0x4008));
    if (doHeatHaze) {
        if (m_FxInfoManager.m_bHasHeatHazeParticleEmitter) { RenderHeatHaze(camera, txdHashKey, brightness); }
        return;
    }
    if (heatEmitter) {
        if (m_Particles.GetNumItems() > 0) { g_fxMan.m_bHeatHazeEnabled = true; }
        return;
    }
    if (!m_Particles.GetNumItems()) { return; }
    RenderEmitterParticles355(*this, camera, brightness, false);
}

// 0x4A2510

// 0x0
bool FxEmitterBP_c::FreePrtFromPrim(FxSystem_c* system) {
    for (auto* it = m_Particles.GetHead(); it; it = m_Particles.GetNext(it)) {
        if (it->m_System == system) {
            m_Particles.RemoveItem(it);
            g_fxMan.ReturnParticle(it->AsFxEmitterPrt());
            return true;
        }
    }
    return false;
}

// todo: eFxInfo
// 0x4A24D0
bool FxEmitterBP_c::IsFxInfoPresent(eFxInfoType type) const {
    if (m_FxInfoManager.m_nNumInfos <= 0)
        return false;

    for (auto& info : m_FxInfoManager.GetInfos()) {
        if (info->m_nType == type) {
            return true;
        }
    }
    return false;
}
