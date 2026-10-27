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
 * Validación registrada: 9/9 entradas.
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
#include "X87Math.h"

static void __fastcall RenderPlayerCrosshair_Hook(uint32* crosshair, void* edx, int32 playerIndex);

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
    RH_ScopedNamedGlobalInstall(RenderPlayerCrosshair_Hook, "RenderPlayerCrosshair", 0x56EF90);
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
    // Como el original: primero lockonFire, después lockon y por último target256
    RwTextureDestroy(gpCrossHairTexFlight[1]);
    gpCrossHairTexFlight[1] = nullptr;
    RwTextureDestroy(gpCrossHairTexFlight[0]);
    gpCrossHairTexFlight[0] = nullptr;
    RwTextureDestroy(gpCrossHairTex);
    gpCrossHairTex = nullptr;
}

// 0x742BD0
bool CWeaponEffects::IsLockedOn(CrossHairId id) {
    // Como el original: _ftol2 del float y se devuelve el byte bajo (0.5f no cuenta como fijado)
    return static_cast<uint8>(static_cast<int64>(gCrossHair[id].m_fRotation)) != 0;
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
    // Como el original: igual que ClearCrossHairImmediately para cada mira (hora - 100 y desactivada)
    for (auto& crossHair : gCrossHair) {
        crossHair.m_nTimeWhenToDeactivate = static_cast<int32>(CTimer::GetTimeInMS() - 100);
        crossHair.m_bActive = false;
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
        // Como el original: se compara el byte con 1
        if (*reinterpret_cast<const uint8*>(&ch.m_bClearImmediately) == 1) {
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
                flightX = TrigSimpleSinMulCpp(spin, ch.field_20);
                flightY = TrigSimpleCosMulCpp(spin, ch.field_20);
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
        // Como el original: los bordes cuentan como dentro y un NaN también pasa
        if (!(double(RsGlobal.maximumWidth) < double(screen.x)) && !(screen.x < 0.f) && !(double(RsGlobal.maximumHeight) < double(screen.y)) && !(screen.y < 0.f)) {
            // Como el original: min (0x404330) y max (0x420800) devuelven el segundo valor con NaN
            const float clampedSize = ch.m_fSize < 1.2f ? ch.m_fSize : 1.2f;
            ch.m_fSize = clampedSize > 0.3f ? clampedSize : 0.3f;
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
                    return CVector2D{screen.x - TrigSimpleSinMulCpp(angle, radius), screen.y - TrigSimpleCosMulCpp(angle, radius)};
                };
                const float degrees = float(triangle) * 120.f;
                auto a = point(degrees, inner + pulse[i]);
                auto b = point(degrees + 15.f, outer + pulse[i]);
                auto c = point(degrees - 15.f, outer + pulse[i]);
                CSprite::RenderOneXLUSprite_Triangle(a, b, c, screen.z, 0, 0, 0, 255, 1.f, ch.m_color.a);
                // Como el original: (c + b) + a
                const CVector2D center{ (c.x + b.x + a.x) * 0.3333333432674408f, (c.y + b.y + a.y) * 0.3333333432674408f };
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
            // Como el original: productos escalares sumados como (x + z) + y y ángulo sin redondear
            const auto angleTo = [&](const CVector& origin, float& outSin, float& outCos, float& outOneMinusCos) {
                const float dx = ch.m_vecPosn.x - origin.x, dy = ch.m_vecPosn.y - origin.y, dz = ch.m_vecPosn.z - origin.z;
                const auto& up = camera.GetUp();
                const auto& right = camera.GetRight();
                const float u = up.x * dx + up.z * dz + up.y * dy;
                const float r = right.x * dx + right.z * dz + right.y * dy;
                WeaponFxAngleSinCosCpp(u, r, &outSin, &outCos, &outOneMinusCos);
            };
            float camSin, camCos, camOneMinusCos;
            angleTo(camera.GetPosition(), camSin, camCos, camOneMinusCos);
            float s, c, unusedOneMinusCos;
            angleTo(player->GetPosition(), s, c, unusedOneMinusCos);
            CVector2D base{ camOneMinusCos * float(RsGlobal.maximumWidth) * 0.5f, (1.f - camSin) * float(RsGlobal.maximumHeight) * 0.5f };
            const float radius = StaticRef<float>(0x8D6140);
            const float t1 = s * 0.5f + c;
            const float t2 = c * 0.5f + s;
            const float rs = radius * s;
            const float rc = c * radius;
            // Como el original: los vértices laterales se forman como (r*s + y) - r*c*0.5 y (r*c + x) - r*s*0.5
            const CVector2D v2{ t1 * radius + base.x, (rs + base.y) - rc * 0.5f };
            const CVector2D v3{ (rc + base.x) - rs * 0.5f, t2 * radius + base.y };
            CSprite::RenderOneXLUSprite_Triangle(base, v2, v3, 2.5f, 0, 0, 0, 255, 1.f, 255);
            // Como el original: desplazamiento en X con la constante double 0.1 (0x86EEE0) y en Y con 0.1f
            const CVector2D insetBase{ WeaponFxMulDoubleAddCpp(rc, 0.1, base.x), 0.1f * rs + base.y };
            const CVector2D iv2{ t1 * radius * 0.8f + insetBase.x, (rs - 0.5f * rc) * 0.8f + insetBase.y };
            const CVector2D iv3{ (rc - 0.5f * rs) * 0.8f + insetBase.x, t2 * radius * 0.8f + insetBase.y };
            CSprite::RenderOneXLUSprite_Triangle(insetBase, iv2, iv3, 2.5f, ch.m_color.r, ch.m_color.g, ch.m_color.b, 255, 1.f, 255);
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
                const auto scale = std::max(float(3.0 - double(elapsed) * 0.001953125), 1.f); // Como el original: fild exacto y un solo redondeo
                const auto red = uint8(std::min(elapsed / 4 + 70, 255));
                CSprite::RenderOneXLUSprite_Rotate_Aspect(screen, {w * scale, h * scale}, red, 0, 0, red, 0.01f, spin, 255);
            }
        }
        end();
    }
    for (int i = 0; i < 2; ++i) {
        RenderPlayerCrosshair(CWorld::Players[i], i); // la mira auxiliar de cada jugador (0x56EF90)
    }
}

// 0x56EF90: el EXE llama con this = &Players[i].m_nCrosshairActivated; ret 4, sin valor de retorno que lea ningun llamador.
static void __fastcall RenderPlayerCrosshair_Hook(uint32* crosshair, void* /*edx*/, int32 playerIndex) {
    CWeaponEffects::RenderPlayerCrosshair(*reinterpret_cast<CPlayerInfo*>(reinterpret_cast<uint8*>(crosshair) - offsetof(CPlayerInfo, m_nCrosshairActivated)), playerIndex);
}

// 0x56EF90
// Dibuja la estela de la mira del jugador (cinco muestras de su posicion en pantalla, con 12 sprites cada una): guarda la posicion y la hora actuales en tres historiales
// globales de 5 entradas por jugador (X 0xB9B948, Y 0xB9B920, hora 0xB9B8F8) y los pinta de la mas vieja a la mas nueva con el color del jugador reducido a la mitad por capa.
void CWeaponEffects::RenderPlayerCrosshair(CPlayerInfo& info, int32 playerIndex) {
    if (!*reinterpret_cast<const uint8*>(&info.m_nCrosshairActivated)) {
        return;
    }
    const auto state = [](RwRenderState key, uint32 value) {
        RwRenderStateSet(key, RWRSTATE(value));
    };
    state(rwRENDERSTATEZWRITEENABLE, 0);
    state(rwRENDERSTATEZTESTENABLE, 0);
    state(rwRENDERSTATEVERTEXALPHAENABLE, 1);
    state(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    state(rwRENDERSTATEDESTBLEND, rwBLENDINVDESTALPHA);
    // Como el original: textura 0xC3E000[indice en 0x8CDF1C] (gpCoronaTexture), sin comprobar el indice
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, reinterpret_cast<RwTexture**>(0xC3E000)[StaticRef<int32>(0x8CDF1C)]->raster);

    int32 r, g, b;
    if (playerIndex != 0) {
        r = 0x32; g = 0; b = 0xFF;
    } else {
        r = 0; g = 0xFF; b = 0x32;
    }
    auto* const historyX = reinterpret_cast<uint32*>(0xB9B948);
    auto* const historyY = reinterpret_cast<uint32*>(0xB9B920);
    auto* const historyTime = reinterpret_cast<uint32*>(0xB9B8F8);
    const int32 base = playerIndex * 5;
    // Como el original: la hora de cada entrada se copia de una direccion ABSOLUTA (Y del jugador 0 + 0xC, retrocediendo 4 por vuelta), no de la del jugador
    const auto* const timeSource = reinterpret_cast<const uint32*>(0xB9B92C);
    for (int32 k = 0; k < 4; ++k) {
        const int32 to = base + 4 - k;
        historyX[to] = historyX[to - 1];
        historyY[to] = historyY[to - 1];
        historyTime[to] = timeSource[-k];
    }
    memcpy(&historyX[base], &info.m_vecCrosshairTarget.x, 4);
    memcpy(&historyY[base], &info.m_vecCrosshairTarget.y, 4);
    historyTime[base] = CTimer::m_snTimeInMilliseconds;

    for (int32 layer = 0; layer < 5; ++layer) {
        // Como el original: la hora es siempre la de la primera entrada del jugador
        const float phase = Cross74PhaseCpp(historyTime[base]);
        for (int32 j = 0; j < 3; ++j) {
            const float radius = float(j) * 10.0f + 20.0f;
            for (int32 i = 0; i < 4; ++i) {
                uint32 out[2];
                Cross74TrailPointCpp(historyX[base + layer], historyY[base + layer], RsGlobal.maximumWidth, RsGlobal.maximumHeight, phase, radius, i, out);
                CSprite::RenderOneXLUSprite_Rotate_Aspect({std::bit_cast<float>(out[0]), std::bit_cast<float>(out[1]), 100.0f}, {15.0f, 15.0f}, uint8(r), uint8(g), uint8(b), 255, 0.01f, 0.0f, 255);
            }
        }
        r >>= 1;
        g >>= 1;
        b >>= 1;
    }
    state(rwRENDERSTATEZWRITEENABLE, 1);
    state(rwRENDERSTATEZTESTENABLE, 1);
}
