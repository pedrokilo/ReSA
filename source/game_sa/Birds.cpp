/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBirds
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "X87Math.h"

#include "Birds.h"
#include "Clouds.h"

/*
These spawn in a formation like this:
    4           5
             1
          2
       6           3
*/
std::array<float, 6> CBirds::faCreationCoorsX = { 0.0f, -1.0f, 2.0f, -3.0f, 1.0f, -2.0f }; // 0x8D5250
std::array<float, 6> CBirds::faCreationCoorsY = { 0.0f, -1.0f, -2.0f, 1.0f, 1.0f, -2.0f }; // 0x8D5268
std::array<float, 6> CBirds::faCreationCoorsZ = { 0.0f, 0.5f, 1.0f, 0.7f, 2.0f, 1.2f };    // 0x8D5280

std::array<float, 8> CBirds::faRenderCoorsU = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.5f }; // 0x8D52B8
std::array<float, 8> CBirds::faRenderCoorsV = { 0.5f, 0.5f, 0.75f, 0.75f, 1.0f, 1.0f, 1.0f, 0.5f }; // 0x8D5298
std::array<float, 8> CBirds::faRenderPosY = { 0.25f, -0.25f, 0.25f, -0.25f, 0.0f, -0.25f, -0.35f, -0.35f }; // 0x8D52D8;

std::array<uint32, 30> CBirds::auRenderIndices = { 0, 3, 1, 0, 2, 3, 2, 4, 5, 2, 5, 3, 0, 1, 3, 0, 3, 2, 2, 5, 4, 2, 3, 5, 0, 6, 7, 0, 7, 6 }; // 0x8D52F8

void CBirds::InjectHooks() {
    RH_ScopedClass(CBirds);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x711EC0);
    RH_ScopedInstall(Shutdown, 0x712300);
    RH_ScopedInstall(HandleGunShot, 0x712E40);
    RH_ScopedInstall(Update, 0x712330);
    RH_ScopedInstall(CreateNumberOfBirds, 0x711EF0);
    RH_ScopedInstall(Render, 0x712810);
}

// 0x711EC0
void CBirds::Init() {
    ZoneScoped;

    for (auto& bird : aBirds) {
        bird.m_bCreated = false;
    }
    uiNumberOfBirds = 0;
    bHasBirdBeenShot = false;
}

inline float GetMaxDistance(eBirdsBiome biome) {
    switch (biome) {
    case eBirdsBiome::BIOME_WATER:  return 45.0F;
    case eBirdsBiome::BIOME_DESERT: return 80.0F;
    case eBirdsBiome::BIOME_NORMAL:
    default:                        return 40.0F;
    }
};

namespace {
// Como el original: fsin/fcos/fpatan del x87 con precisión interna completa; el producto/suma siguiente lo redondea la FPU.








// -sin(angle) / div, con el seno sin redondear


// Shared C++ helpers replace the three compound trigonometric bodies.
// Spawn position uses sine times distance plus camera X.
// Its second component uses cosine times distance plus camera Y.
// Both expressions keep the full intermediate x87 value.
// Wing motion uses the same sine, multiply, and add sequence.
// The negative sine factor changes sign before division.
// Moving the sign change after division would change rounding behavior.
// The helper therefore keeps those operations separate in its C++ source.
// Double expressions allow the compiler to retain ST0 intermediates.
// The actual object must still prove that no temporary was introduced.
// Intrinsic sine and cosine must emit the original x87 instructions.
// Library calls are rejected for these helpers.
// The calling convention remains cdecl with float arguments.
// The float result remains in ST0.
// The helper preserves integer flags and caller registers.
// Consumer instructions and relocations are compared separately.
// Only the authorized helper call destinations may change.
// Update exercises both spawn helpers in the game bank.
// Render exercises the wing helpers and negative division.
// CreateNumberOfBirds remains a companion regression.
// Diagnostic source-line constants are part of the object comparison.
// Keeping this explanation here preserves those existing constants.
// No byte or relocation differences are ignored.
// atan2(x, y) con fpatan
__declspec(naked) float __cdecl BirdsAtan2(float x, float y) {
    __asm {
        fld    dword ptr [esp + 4]
        fld    dword ptr [esp + 8]
        fpatan
        ret
    }
}

// add + atan2(x, y), con el arcotangente sin redondear
__declspec(naked) float __cdecl BirdsAddAtan2(float add, float x, float y) {
    __asm {
        fld    dword ptr [esp + 4]
        fld    dword ptr [esp + 8]
        fld    dword ptr [esp + 12]
        fpatan
        faddp  st(1), st
        ret
    }
}

// Giro de la velocidad objetivo (0x7126C5): x' = cos·x + sin·y, y' = cos·y - sin·x
__declspec(naked) void __cdecl BirdsRotate(float angle, float* px, float* py) {
    __asm {
        mov   eax, dword ptr [esp + 8]
        mov   edx, dword ptr [esp + 12]
        fld   dword ptr [esp + 4]
        fld   dword ptr [eax]
        fld   dword ptr [edx]
        fld   st(2)
        fsin
        fxch  st(3)
        fcos
        fld   st(0)
        fmul  st, st(3)
        fld   st(4)
        fmul  st, st(3)
        faddp st(1), st
        fstp  dword ptr [eax]
        fmul  st, st(1)
        fxch  st(3)
        fmul  st, st(2)
        fsubp st(3), st
        fxch  st(2)
        fstp  dword ptr [edx]
        fstp  st(1)
        fstp  st(0)
        ret
    }
}

const float BIRDS_WING_K   = std::bit_cast<float>(0x40C8F5C3u); // 0x872828: 6,28 (no 2pi)
const float BIRDS_ANGLE_K  = std::bit_cast<float>(0x3CC8F5C3u); // 0x859C44: 0,02453125
const float BIRDS_CURVES_K = std::bit_cast<float>(0x3B03126Fu); // 0x858F44: 0,002
} // namespace

// 0x711EF0
void CBirds::CreateNumberOfBirds(CVector vecStartPos, CVector vecTargetPos, int32 iBirdCount, eBirdsBiome biome, bool bCheckObstacles) {
    const float fMaxDistance = GetMaxDistance(biome);
    for (auto i = 0; i < iBirdCount; ++i) {
        auto iFreeBirdIndex = 0;
        while (iFreeBirdIndex < (int32)std::size(aBirds)) {
            if (!aBirds[iFreeBirdIndex].m_bCreated)
                break;
            ++iFreeBirdIndex;
        }
        if (iFreeBirdIndex >= (int32)std::size(aBirds))
            continue; // Como el original: sigue con la siguiente iteración

        auto& bird = aBirds[iFreeBirdIndex];
        bird.m_vecPosn = vecStartPos;

        auto vecBirdDirection = vecTargetPos - bird.m_vecPosn;
        vecBirdDirection.Normalise();

        // Como el original: el punto de comprobación sustituye al destino para las siguientes iteraciones
        vecTargetPos = bird.m_vecPosn + (vecBirdDirection * fMaxDistance * 2.4F);
        if (bCheckObstacles && !CWorld::GetIsLineOfSightClear(bird.m_vecPosn, vecTargetPos, true, false, false, false, false, false, false))
            continue;

        assert(i < (int32)std::size(faCreationCoorsX));

        const auto iSpeedRandFactor = static_cast<int32>(CGeneral::GetRandomNumber() % 31);
        bird.m_fAngle = BirdsAtan2(vecBirdDirection.x, vecBirdDirection.y);
        bird.m_fMaxBirdDistance = fMaxDistance;
        bird.m_bCreated = true;
        bird.m_eBirdMode = eBirdMode::BIRD_DRAW_UPDATE;
        bird.m_nUpdateAfterMS = 0;
        bird.m_bMustDoCurves = false;
        bird.m_nWingStillness = 1000 - 12 * iSpeedRandFactor; // Como el original: se escribe para todos los biomas
        bird.m_vecPosn.x += faCreationCoorsX[i];
        bird.m_vecPosn.y += faCreationCoorsY[i];
        bird.m_vecPosn.z += faCreationCoorsZ[i];

        // Como el original: velocidad = r · 0,02 + base (multiplicación, no división entre 50)
        const auto SetVelocity = [&](float base) {
            const float fSpeedMult = (float)iSpeedRandFactor * 0.02f + base;
            bird.m_vecTargetVelocity = vecBirdDirection * fSpeedMult;
            bird.m_vecCurrentVelocity = bird.m_vecTargetVelocity;
        };

        switch (biome) {
        case eBirdsBiome::BIOME_WATER:
            SetVelocity(4.0f);                                                  // [4.0 : 4.6]
            bird.m_BodyColor.Set((CGeneral::GetRandomNumber() & 0x3F) + 0x50);  // [80 : 143]
            bird.m_WingsColor.Set((CGeneral::GetRandomNumber() & 0x3F) - 0x4C); // [180 : 243]
            bird.m_fSize = CGeneral::GetRandomNumberInRange(0.8F, 1.2F);
            break;
        case eBirdsBiome::BIOME_DESERT:
            bird.m_BodyColor.Set(30, 15, 10);
            bird.m_WingsColor.Set(80, 15, 10);
            SetVelocity(3.0f);                                     // [3.0 : 3.6]
            bird.m_nWingStillness = 12 * (125 - iSpeedRandFactor); // [1140 : 1500]
            bird.m_fSize = CGeneral::GetRandomNumberInRange(2.0F, 2.5F);
            if (CGeneral::GetRandomNumber() & 4)
                bird.m_nWingStillness = 1'000'000; // Not moving it's wings

            if (CGeneral::GetRandomNumber() & 8)
                bird.m_bMustDoCurves = true;

            break;
        case eBirdsBiome::BIOME_NORMAL:
            SetVelocity(5.0f);                                                  // [5.0 : 5.6]
            bird.m_BodyColor.Set((CGeneral::GetRandomNumber() & 0x7F) + 0x80);  // Como el original: rand & 0x7F + 0x80
            bird.m_WingsColor.Set((CGeneral::GetRandomNumber() & 0x7F) + 0x80);
            bird.m_nWingStillness = 500 - 6 * iSpeedRandFactor;                 // [320 : 500]
            bird.m_fSize = CGeneral::GetRandomNumberInRange(0.5F, 0.6F);
            break;
        default:
            break; // Como el original: otros biomas no fijan velocidad, colores ni tamaño
        }

        ++uiNumberOfBirds;
    }
}

// 0x712300
void CBirds::Shutdown() {
    for (auto& bird : aBirds) {
        if (bird.m_bCreated) {
            bird.m_bCreated = false;
        }
    }
    uiNumberOfBirds = 0;
}

// 0x712330
void CBirds::Update() {
    ZoneScoped;

    const CVector camPosn = TheCamera.GetPosition();

    // Como el original: contador con signo y horas 6..21
    if (!CGame::currArea
        && (int32)uiNumberOfBirds < (int32)std::size(aBirds)
        && CClock::ms_nGameClockHours < 22 && CClock::ms_nGameClockHours > 5
        && (CTimer::GetFrameCounter() & 0x1FF) == std::size(aBirds)
    ) {
        // Como el original: 0x407180 en [1, 7 - n)
        int32 iNumBirdsToCreate = CGeneral::GetRandomIntInRange(1, 7 - (int32)uiNumberOfBirds);

        eBirdsBiome biome = eBirdsBiome::BIOME_WATER;
        if (!(TheCamera.m_fDistanceToWater < 30.0F)) { // Como el original: >= 30 (o NaN) es tierra
            if (CWeather::WeatherRegion == eWeatherRegion::WEATHER_REGION_DESERT) {
                biome = eBirdsBiome::BIOME_DESERT;
                // Como el original: como mucho 3 pájaros en el desierto
                const int32 maxDesertBirds = 3 - (int32)uiNumberOfBirds;
                if (!(iNumBirdsToCreate < maxDesertBirds)) {
                    iNumBirdsToCreate = maxDesertBirds;
                }
            }
            else {
                biome = eBirdsBiome::BIOME_NORMAL;
            }
        }

        if (iNumBirdsToCreate > 0) {
            CGeneral::GetRandomNumber(); // Called 2 times for some reason
            CGeneral::GetRandomNumber();

            float fSpawnDistance = GetMaxDistance(biome);
            float fFlightHeight = [&biome]() {
                switch (biome) {
                case eBirdsBiome::BIOME_WATER:  return CGeneral::GetRandomNumberInRange(4.0F, 13.0F);
                case eBirdsBiome::BIOME_DESERT: return CGeneral::GetRandomNumberInRange(15.0F,25.0F);
                case eBirdsBiome::BIOME_NORMAL:
                default:                        return CGeneral::GetRandomNumberInRange(2.0F, 10.0F);
                }
            }();

            const float fBirdSpawnZ = fFlightHeight + camPosn.z;
            if (fBirdSpawnZ > 5.0F) {
                float fSpawnAngleCamRelative;

                if (CGeneral::GetRandomNumber() & 1)
                    fSpawnAngleCamRelative = (float)(CGeneral::GetRandomNumber() & 0xFF) * BIRDS_ANGLE_K; // [0 : 255] mapped to [0 : 2π]
                else {
                    auto vecForward = TheCamera.m_mCameraMatrix.GetForward();
                    vecForward.z = 0.0F;
                    if (vecForward.x == 0.0F)
                        vecForward.x = 0.01F;

                    vecForward.Normalise();
                    // Como el original: (rand & 0xFF) - 128 con signo, · 1/256, más atan2 sin redondear
                    const int32 iRand = (int32)(CGeneral::GetRandomNumber() & 0xFF) - 0x80;
                    fSpawnAngleCamRelative = BirdsAddAtan2((float)iRand * (1.0f / 256.0f), vecForward.x, vecForward.y);
                }

                const CVector vecBirdSpawnPos{
                    TrigCompoundSinMulAdd(fSpawnAngleCamRelative, fSpawnDistance, camPosn.x),
                    TrigCompoundCosMulAdd(fSpawnAngleCamRelative, fSpawnDistance, camPosn.y),
                    fBirdSpawnZ
                };

                float fWaterLevel;
                if (!CWaterLevel::GetWaterLevelNoWaves(vecBirdSpawnPos, &fWaterLevel, nullptr, nullptr) || fWaterLevel + 4.0F < fBirdSpawnZ) {
                    auto vecForward = TheCamera.m_mCameraMatrix.GetForward();
                    vecForward.z = 0.0F;
                    vecForward.Normalise();

                    auto vecFlightTargetPos = camPosn + (vecForward * 8.0F);
                    vecFlightTargetPos.z = fBirdSpawnZ;

                    CreateNumberOfBirds(vecBirdSpawnPos, vecFlightTargetPos, iNumBirdsToCreate, biome, true);
                }
            }
        }
    }

    // Como el original: m_bCreated == 1 y comparación de cuadrados (maxD² < dx² + dy²), sin raíz
    auto& checkedBird = aBirds[CTimer::GetFrameCounter() % std::size(aBirds)];
    if (reinterpret_cast<const uint8&>(checkedBird.m_bCreated) == 1) {
        const float fDeltaY = checkedBird.m_vecPosn.y - camPosn.y;
        const float fDeltaX = checkedBird.m_vecPosn.x - camPosn.x;
        const float fMaxDist = checkedBird.m_fMaxBirdDistance;
        if (fMaxDist * fMaxDist < fDeltaX * fDeltaX + fDeltaY * fDeltaY) {
            checkedBird.m_bCreated = false;
            uiNumberOfBirds--;
        }
    }

    const uint32 uiTimeInMS = CTimer::GetTimeInMS();
    for (auto& bird : aBirds) {
        if (reinterpret_cast<const uint8&>(bird.m_bCreated) != 1)
            continue;

        // Como el original: se actualiza solo si el tiempo es estrictamente mayor que m_nUpdateAfterMS
        if (bird.m_eBirdMode != eBirdMode::BIRD_DRAW_UPDATE || uiTimeInMS <= bird.m_nUpdateAfterMS)
            continue;

        if (bird.m_bMustDoCurves) {
            BirdsRotate(CTimer::GetTimeStep() * BIRDS_CURVES_K, &bird.m_vecTargetVelocity.x, &bird.m_vecTargetVelocity.y);
        }

        // Como el original: paso en segundos = ms_fTimeStep · 0,02
        const float fTimeStep = CTimer::GetTimeStep() * 0.02f;
        float fProgress = fTimeStep * 0.5f;
        if (1.0f < fProgress) {
            fProgress = 1.0f;
        }
        const CVector vecTarget = bird.m_vecTargetVelocity;
        bird.m_vecCurrentVelocity = Lerp(bird.m_vecCurrentVelocity, vecTarget, fProgress);
        bird.m_vecPosn += (fTimeStep * bird.m_vecCurrentVelocity);
        bird.m_fAngle = BirdsAtan2(vecTarget.x, vecTarget.y);
    }
}

// 0x712810
void CBirds::Render() {
    ZoneScoped;

    if (uiNumberOfBirds == 0)
        return;

    // Como el original: los contadores del búfer temporal son de 32 bits en el EXE
    auto& indicesStored  = StaticRef<uint32>(0xC4B954);
    auto& verticesStored = StaticRef<uint32>(0xC4B950);

    uint32 uiWingMoveTimeOffset = 0;
    indicesStored = 0;
    verticesStored = 0;

    for (auto& bird : aBirds) {
        if (!bird.m_bCreated) {
            uiWingMoveTimeOffset += 100;
            continue;
        }

        CMatrix matBirdTransform;
        matBirdTransform.SetTranslate(bird.m_vecPosn);

        if (bird.m_eBirdMode == eBirdMode::BIRD_DRAW_NOUPDATE || bird.m_eBirdMode == eBirdMode::BIRD_DRAW_UPDATE) {
            // Como el original: (t % ws) · (6,28 / ws), fsin completo · 0,1
            auto& vecPos = matBirdTransform.GetPosition();
            const uint32 uiStillness = bird.m_nWingStillness;
            const uint32 uiTime = CTimer::GetTimeInMS() + uiWingMoveTimeOffset;
            vecPos.z = TrigCompoundSinMulAdd((float)(uiTime % uiStillness) * (BIRDS_WING_K / (float)uiStillness), 0.1F, vecPos.z);
        }

        CVector vecScreenPos;
        float fScreenWidth;
        float fScreenHeight;
        if (CSprite::CalcScreenCoors(matBirdTransform.GetPosition(), &vecScreenPos, &fScreenWidth, &fScreenHeight, false, true)) {
            const auto fCosSize = TrigSimpleCosMulCpp(bird.m_fAngle, bird.m_fSize);
            const auto fSinSize = TrigSimpleSinMulCpp(bird.m_fAngle, bird.m_fSize);

            matBirdTransform.GetRight().Set(fCosSize, -fSinSize, 0.0F);
            matBirdTransform.GetForward().Set(fSinSize, fCosSize, 0.0F);
            matBirdTransform.GetUp().Set(0.0F, 0.0F, bird.m_fSize);

            if (bird.m_eBirdMode == eBirdMode::BIRD_DRAW_NOUPDATE || bird.m_eBirdMode == eBirdMode::BIRD_DRAW_UPDATE) {
                for (auto index = 0u; index < std::size(auRenderIndices); ++index) {
                    auto uiVertInd = static_cast<RxVertexIndex>(verticesStored + auRenderIndices[index]);
                    aTempBufferIndices[indicesStored + index] = uiVertInd;
                    aTempBufferIndices[indicesStored + index + std::size(auRenderIndices)] = uiVertInd + 8;
                }

                // Como el original: ángulo = r · (6,28 / ws); el primer seno se guarda redondeado y el segundo
                // se divide sin redondear
                const uint32 uiStillness = bird.m_nWingStillness;
                const uint32 uiTime = CTimer::GetTimeInMS() + uiWingMoveTimeOffset;
                const float fWingK = BIRDS_WING_K / (float)uiStillness;
                const float fSin1 = TrigSimpleSinMulCpp((float)((uiStillness / 6 + uiTime) % uiStillness) * fWingK, -1.0f);

                const auto fSizeUsed = (1.0F > bird.m_fSize) ? 1.0F : bird.m_fSize; // Como el original: NaN usa el tamaño
                const auto fSin1Factor = fSin1 / fSizeUsed;
                const auto fSin2Factor = TrigCompoundNegSinDiv((float)(uiTime % uiStillness) * fWingK, fSizeUsed);

                const auto fOffX1 = TrigSimpleCosMulCpp(fSin1Factor, 0.5F);
                const auto fOffZ1 = TrigSimpleSinMulCpp(fSin1Factor, 0.5F);
                const auto fOffX2 = TrigSimpleCosMulCpp(fSin2Factor, 0.5F);
                const auto fOffZ2 = TrigSimpleSinMulCpp(fSin2Factor, 0.5F);

                RwUInt8 cAlpha = 255;
                const auto& vecCameraPos = TheCamera.GetPosition();
                auto fCameraDist = DistanceBetweenPoints(vecCameraPos, bird.m_vecPosn);
                auto fAlphaCutoffDist = bird.m_fMaxBirdDistance * 0.7F;
                if (fCameraDist > fAlphaCutoffDist) {
                    // Como el original: 1 - (d - 0,7·max) / (0,3·max) y truncado de _ftol2
                    auto fTransparency = 1.0F - (fCameraDist - fAlphaCutoffDist) / (bird.m_fMaxBirdDistance * 0.3F);
                    if (0.0f > fTransparency) {
                        fTransparency = 0.0f;
                    }
                    cAlpha = static_cast<RwUInt8>(static_cast<int64>(fTransparency * 255.0F));
                }

                for (auto uiVertInd = 0u; uiVertInd < std::size(faRenderPosY); ++uiVertInd) {
                    auto [point, color] = [&] {
                        switch (uiVertInd) {
                        case 0:
                        case 1:
                        case 7:
                            return std::make_pair(
                                CVector{ 0.0f, faRenderPosY[uiVertInd], 0.0f },
                                bird.m_BodyColor
                            );
                        case 2:
                        case 3:
                            return std::make_pair(
                                CVector{ fOffX1, faRenderPosY[uiVertInd], fOffZ1 },
                                bird.m_BodyColor
                            );
                        case 4:
                        case 5:
                            return std::make_pair(
                                CVector{ fOffX1 + fOffX2, faRenderPosY[uiVertInd], fOffZ1 + fOffZ2 },
                                bird.m_WingsColor
                            );
                        case 6:
                            return std::make_pair(
                                CVector{ 0.1f, faRenderPosY[uiVertInd], 0.0f },
                                bird.m_BodyColor
                            );
                        }
                        NOTSA_UNREACHABLE("CBirds::Render::lambda suppress warning");
                        return std::make_pair(CVector(), CBirdColor());
                    }();
                    auto vecWorldPos = matBirdTransform.TransformPoint(point);

                    auto iBufferInd = verticesStored + uiVertInd;
                    auto vert1 = &TempBufferVertices.m_3d[iBufferInd];
                    RwRGBA rwColor = CRGBA(color.cRed, color.cGreen, color.cBlue, cAlpha).ToRwRGBA();
                    RxObjSpace3DVertexSetPreLitColor(vert1, &rwColor);
                    RxObjSpace3DVertexSetPos(vert1, &vecWorldPos);
                    RxObjSpace3DVertexSetU(vert1, faRenderCoorsU[uiVertInd]);
                    RxObjSpace3DVertexSetV(vert1, faRenderCoorsV[uiVertInd]);

                    // Mirror on the other side with slightly changed colors
                    point.x = -point.x;
                    vecWorldPos = matBirdTransform.TransformPoint(point);
                    color.Scale(0.8F);
                    rwColor = CRGBA(color.cRed, color.cGreen, color.cBlue, cAlpha).ToRwRGBA();

                    auto vert2 = &TempBufferVertices.m_3d[iBufferInd + 8];
                    RxObjSpace3DVertexSetPreLitColor(vert2, &rwColor);
                    RxObjSpace3DVertexSetPos(vert2, &vecWorldPos);
                    RxObjSpace3DVertexSetU(vert2, faRenderCoorsU[uiVertInd]);
                    RxObjSpace3DVertexSetV(vert2, faRenderCoorsV[uiVertInd]);
                }

                indicesStored += 60;
                verticesStored += 16;
            }
        }
        uiWingMoveTimeOffset += 100;
    }

    if (indicesStored) { // Como el original: comprueba el contador de índices
        RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
        RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,    RWRSTATE(rwTEXTUREADDRESSCLAMP));
        RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(gpCloudMaskTex->raster));
        CBrightLights::RenderOutGeometryBuffer();
    }
}

// 0x712E40
void CBirds::HandleGunShot(const CVector* pointA, const CVector* pointB) {
    for (auto& bird : aBirds) {
        if (!bird.m_bCreated)
            continue;

        CColLine line{ *pointA, *pointB };
        CColSphere sphere{ bird.m_vecPosn, 0.5f };

        if (CCollision::TestLineSphere(line, sphere)) {
            vecBirdShotAt = bird.m_vecPosn;
            bHasBirdBeenShot = true;
            bird.m_bCreated = false;
            uiNumberOfBirds--;
        }
    }
}
