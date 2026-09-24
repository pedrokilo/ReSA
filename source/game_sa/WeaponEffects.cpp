/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWeaponEffects
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "WeaponEffects.h"

void CWeaponEffects::InjectHooks() {
    RH_ScopedClass(CWeaponEffects);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x742AB0);
    RH_ScopedInstall(Shutdown, 0x742B80);
    RH_ScopedInstall(IsLockedOn, 0x742BD0);
    RH_ScopedInstall(MarkTarget, 0x742BF0);
    RH_ScopedInstall(ClearCrossHair, 0x742C60);
    RH_ScopedInstall(ClearCrossHairs, 0x742C80);
    RH_ScopedInstall(ClearCrossHairImmediately, 0x742CA0);
    RH_ScopedInstall(ClearCrossHairsImmediately, 0x742CC0);
    RH_ScopedInstall(Render, 0x742CF0);
}

// 0x742AB0
void CWeaponEffects::Init() {
    for (auto& crossHair : gCrossHair) {
        crossHair.m_vecPosn = CVector();
        crossHair.m_bActive = false;
        crossHair.m_color = CRGBA(255, 0, 0, 127);
        crossHair.m_fSize = 1.0f;
        crossHair.field_1C = 0;
        crossHair.m_nTimeWhenToDeactivate = 0;
        crossHair.m_bClearImmediately = false;
        crossHair.m_fRotation = 0.0f;
    }

    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    gpCrossHairTex          = RwTextureRead("target256",  "target256m");
    gpCrossHairTexFlight[0] = RwTextureRead("lockon",     "lockonA");
    gpCrossHairTexFlight[1] = RwTextureRead("lockonFire", "lockonFireA");
    CTxdStore::PopCurrentTxd();
}

// 0x742B80
void CWeaponEffects::Shutdown() {
    for (auto& crossHair : gpCrossHairTexFlight) {
        RwTextureDestroy(crossHair);
        crossHair = nullptr;
    }

    RwTextureDestroy(gpCrossHairTex);
    gpCrossHairTex = nullptr;
}

// 0x742BD0
bool CWeaponEffects::IsLockedOn(CrossHairId id) {
    return gCrossHair[id].m_fRotation;
}

// 0x742BF0
void CWeaponEffects::MarkTarget(CrossHairId id, CVector posn, uint8 red, uint8 green, uint8 blue, uint8 alpha, float size, bool bClearImmediately) {
    auto& crossHair = gCrossHair[id];
    crossHair.m_vecPosn               = posn;
    crossHair.m_color.r               = red;
    crossHair.m_color.g               = green;
    crossHair.m_color.b               = blue;
    crossHair.m_color.a               = alpha;
    crossHair.m_bActive               = true;
    crossHair.m_fSize                 = size;
    crossHair.m_nTimeWhenToDeactivate = -1;
    crossHair.m_bClearImmediately     = bClearImmediately;
}

// 0x742C60
void CWeaponEffects::ClearCrossHair(CrossHairId id) {
    gCrossHair[id].m_nTimeWhenToDeactivate = static_cast<int32>(CTimer::GetTimeInMS() + 400);
}

// 0x742C80
void CWeaponEffects::ClearCrossHairs() {
    for (auto& crossHair : gCrossHair) {
        crossHair.m_bActive = false;
    }
}

// 0x742CA0
void CWeaponEffects::ClearCrossHairImmediately(CrossHairId id) {
    gCrossHair[id].m_nTimeWhenToDeactivate = static_cast<int32>(CTimer::GetTimeInMS() - 100);
    gCrossHair[id].m_bActive = false;
}

// 0x742CC0
void CWeaponEffects::ClearCrossHairsImmediately() {
    for (auto i = 0u; i < gCrossHair.size(); i++) {
        ClearCrossHair(i);
    }
}

// 0x742CF0
void CWeaponEffects::Render() {
    ZoneScoped;

    if (TheCamera.m_bWideScreenOn) {
        return;
    }
    auto& pulse = StaticRef<std::array<float, 2>>(0xC8A8A0);
    auto& expanding = StaticRef<std::array<bool, 2>>(0x8D6144);
    auto& flightX = StaticRef<float>(0xC8A89C);
    auto& flightY = StaticRef<float>(0xC8A898);
    auto& previousTarget = StaticRef<CEntity*>(0xC8A894);
    auto& targetSince = StaticRef<uint32>(0xC8A890);
    const auto now = CTimer::GetTimeInMS();
    const float spin = float(now & 1023) * 0.006135923322290182f;
    const auto state = [](RwRenderState key, uint32 value) {
        RwRenderStateSet(key, RWRSTATE(value));
    };
    const auto begin = [&] {
        state(rwRENDERSTATEZWRITEENABLE, false);
        state(rwRENDERSTATEZTESTENABLE, false);
        state(rwRENDERSTATEVERTEXALPHAENABLE, true);
    };
    const auto blend = [&] {
        state(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
        state(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    };
    const auto end = [&] {
        state(rwRENDERSTATEZWRITEENABLE, true);
        state(rwRENDERSTATEZTESTENABLE, true);
    };
    const auto minimumHeight = [](float& w, float& h) {
        const auto factor = 20.f / h;
        if (factor > 1.f) {
            w *= factor;
            h *= factor;
        }
    };
    for (size_t i = 0; i < gCrossHair.size(); ++i) {
        auto& ch = gCrossHair[i];
        auto* player = CWorld::Players[i].m_pPed;
        if (ch.m_nTimeWhenToDeactivate && uint32(ch.m_nTimeWhenToDeactivate) < now) {
            ch.m_bActive = false;
            ch.m_nTimeWhenToDeactivate = 0;
            pulse[i] = 0.f;
        }
        if (ch.m_nTimeWhenToDeactivate != -1) {
            pulse[i] = 0.f;
        }
        if (!player) {
            ch.m_bActive = false;
        }
        if (!ch.m_bActive) {
            continue;
        }
        RwV3d screen{};
        float w{}, h{};
        if (ch.m_bClearImmediately) {
            if (CSprite::CalcScreenCoors(ch.m_vecPosn, &screen, &w, &h, true, true)) {
                minimumHeight(w, h);
                begin();
                blend();
                RwRenderStateSet(rwRENDERSTATETEXTURERASTER, gpCrossHairTexFlight[0]->raster);
                const float sx = std::min(w, 28.f), sy = std::min(h, 20.f);
                for (const float scale : {0.95f, 1.05f}) {
                    CSprite::RenderOneXLUSprite_Rotate_Aspect(screen, {sx * scale * 1.8f, sy * scale * 1.8f}, 0, 0, 0, 255, 0.01f, 0.f, 255);
                }
                CSprite::RenderOneXLUSprite_Rotate_Aspect(screen, {sx * 1.8f, sy * 1.8f}, ch.m_color.r, ch.m_color.g, ch.m_color.b, 255, 0.01f, 0.f, ch.m_color.a);
                RwRenderStateSet(rwRENDERSTATETEXTURERASTER, gpCrossHairTexFlight[1]->raster);
                ch.field_20 = std::max(ch.field_20 - 20.f * spin, 0.f);
                ch.m_fRotation = ch.field_20 == 0.f ? 1.f : 0.f;
                flightX = std::sin(spin) * ch.field_20;
                flightY = std::cos(spin) * ch.field_20;
                screen.x -= flightX;
                screen.y -= flightY;
                for (const float scale : {0.95f, 1.05f}) {
                    CSprite::RenderOneXLUSprite_Rotate_Aspect(screen, {sx * scale * 0.8f, sy * scale * 0.8f}, 0, 0, 0, 255, 0.01f, spin, 255);
                }
                CSprite::RenderOneXLUSprite_Rotate_Aspect(screen, {sx * 0.8f, sy * 0.8f}, ch.m_color.r, ch.m_color.g, ch.m_color.b, ch.m_color.a, 0.01f, spin, 255);
            }
            end();
            continue;
        }
        begin();
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
        CSprite::CalcScreenCoors(ch.m_vecPosn, &screen, &w, &h, false, false);
        if (screen.x > 0.f && screen.x < float(RsGlobal.maximumWidth) && screen.y > 0.f && screen.y < float(RsGlobal.maximumHeight)) {
            ch.m_fSize = std::max(std::min(ch.m_fSize, 1.2f), 0.3f);
            float inner = 5.f * ch.m_fSize, outer = 25.f * ch.m_fSize;
            screen.z = std::max(screen.z, 2.5f);
            const auto weaponRadius = player->GetWeaponRadiusOnScreen();
            if (weaponRadius > 0.f) {
                inner = weaponRadius * StaticRef<float>(0x8D6148);
                outer = ch.m_fSize * 20.f + inner;
            }
            state(rwRENDERSTATESRCBLEND, rwBLENDZERO);
            state(rwRENDERSTATEDESTBLEND, rwBLENDZERO);
            for (int triangle = 0; triangle < 3; ++triangle) {
                const auto point = [&](float degrees, float radius) {
                    const auto angle = degrees * 0.01745329238474369f + ch.field_1C;
                    return CVector2D{screen.x - std::sin(angle) * radius, screen.y - std::cos(angle) * radius};
                };
                const float degrees = float(triangle) * 120.f;
                auto a = point(degrees, inner + pulse[i]);
                auto b = point(degrees + 15.f, outer + pulse[i]);
                auto c = point(degrees - 15.f, outer + pulse[i]);
                CSprite::RenderOneXLUSprite_Triangle(a, b, c, screen.z, 0, 0, 0, 255, 1.f, ch.m_color.a);
                const auto center = (a + b + c) * 0.3333333432674408f;
                a = center + (a - center) * 0.75f;
                b = center + (b - center) * 0.75f;
                c = center + (c - center) * 0.75f;
                blend();
                state(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
                CSprite::RenderOneXLUSprite_Triangle(a, b, c, screen.z, ch.m_color.r, ch.m_color.g, ch.m_color.b, 255, 1.f, ch.m_color.a);
            }
            const auto expires = uint32(ch.m_nTimeWhenToDeactivate);
            if (expires && expires != uint32(-1)) {
                ch.field_1C += 0.75f;
                pulse[i] += 2.f;
                ch.m_fSize *= 0.9f;
            } else {
                ch.field_1C += 0.05f;
            }
            if (ch.field_1C > 6.2831854820251465f) {
                ch.field_1C = 0.f;
            }
            if (!expires) {
                if (expanding[i]) {
                    pulse[i] += ch.m_fSize * 2.f;
                    if (pulse[i] > ch.m_fSize * 20.f) {
                        expanding[i] = false;
                    }
                } else {
                    pulse[i] -= 2.f;
                    if (pulse[i] < 0.f) {
                        expanding[i] = true;
                    }
                }
            }
        } else if (CGameLogic::IsCoopGameGoingOn()) {
            const auto& camera = TheCamera.m_mCameraMatrix;
            const auto angleTo = [&](CVector origin) {
                const auto d = ch.m_vecPosn - origin;
                return std::atan2(DotProduct(d, camera.GetUp()), DotProduct(d, camera.GetRight()));
            };
            const auto cameraAngle = angleTo(camera.GetPosition());
            const auto playerAngle = angleTo(player->GetPosition());
            const float s = std::sin(playerAngle), c = std::cos(playerAngle);
            const CVector2D base{(1.f - std::cos(cameraAngle)) * float(RsGlobal.maximumWidth) * 0.5f, (1.f - std::sin(cameraAngle)) * float(RsGlobal.maximumHeight) * 0.5f};
            const float radius = StaticRef<float>(0x8D6140);
            const CVector2D left{(c + s * 0.5f) * radius, (s - c * 0.5f) * radius};
            const CVector2D right{(c - s * 0.5f) * radius, (s + c * 0.5f) * radius};
            CSprite::RenderOneXLUSprite_Triangle(base, base + left, base + right, 2.5f, 0, 0, 0, 255, 1.f, 255);
            const auto insetBase = base + CVector2D{float(double(c * radius) * 0.1), s * radius * 0.1f};
            CSprite::RenderOneXLUSprite_Triangle(insetBase, insetBase + left * 0.8f, insetBase + right * 0.8f, 2.5f, ch.m_color.r, ch.m_color.g, ch.m_color.b, 255, 1.f, 255);
        }
        state(rwRENDERSTATEVERTEXALPHAENABLE, false);
        end();
    }
    auto& cam = TheCamera.GetActiveCam();
    if (cam.m_nMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) {
        begin();
        blend();
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, gpCrossHairTex->raster);
        CSprite::RenderOneXLUSprite_Rotate_Aspect({(cam.m_fX_Targetting + 1.f) * float(RsGlobal.maximumWidth) * 0.5f, (cam.m_fY_Targetting + 1.f) * float(RsGlobal.maximumHeight) * 0.5f, 100.f}, {10.f, 10.f}, 255, 128, 0, 128, 0.01f, spin, 255);
        auto* first = CWorld::Players[0].m_pPed;
        auto* shooter = CWorld::Players[1].m_pPed;
        if (first->m_pVehicle && first->m_pVehicle->m_pDriver != first) {
            shooter = first;
        }
        CEntity* target{};
        if (shooter) {
            const auto* info = CWeaponInfo::GetWeaponInfo(shooter->GetActiveWeapon().m_Type, shooter->GetWeaponSkill());
            target = CWeapon::FindNearestTargetEntityWithScreenCoors(cam.m_fX_Targetting, cam.m_fY_Targetting, 2.f * info->m_fWeaponRange, shooter->GetPosition());
        }
        if (target != previousTarget) {
            previousTarget = target;
            targetSince = now;
        }
        if (target) {
            RwV3d screen{};
            float w{}, h{};
            if (CSprite::CalcScreenCoors(target->GetPosition(), &screen, &w, &h, true, true)) {
                minimumHeight(w, h);
                const auto elapsed = int32(now - targetSince);
                const auto scale = std::max(3.f - float(elapsed) * 0.001953125f, 1.f);
                const auto red = uint8(std::min(elapsed / 4 + 70, 255));
                CSprite::RenderOneXLUSprite_Rotate_Aspect(screen, {w * scale, h * scale}, red, 0, 0, red, 0.01f, spin, 255);
            }
        }
        end();
    }
    for (int i = 0; i < 2; ++i) {
        // The original delegates each player's auxiliary crosshair to this method.
        reinterpret_cast<void(__thiscall*)(void*, int)>(0x56EF90)(&CWorld::Players[i].m_nCrosshairActivated, i);
    }
}
