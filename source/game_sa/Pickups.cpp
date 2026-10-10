/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPickups
 * Reconstrucción: COMPLETA — 37/37 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 37/37 entradas.
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

#include "Pickups.h"
#include "Garages.h"
#include "tPickupMessage.h"
#include "PostEffects.h"
#include "Radar.h"
#include "Shadows.h"
#include "Coronas.h"
#include "TaskSimpleJetPack.h"

using namespace ModelIndices;

// Como el original (0x4555D7..0x455636, 0x4563BB..0x45641A, 0x456460..0x4564C3): distancia pickup -> punto con x87 a 24 bits,
// restas pickup - punto, suma (dz*dz + dy*dy) + dx*dx y fsqrt. No usa IsPointInSphere (compara cuadrados con <=).
static float PickupDistanceTo(const CPickup& pickup, const CVector& point) {
    const CVector p = pickup.GetPosn();
    const float dx = p.x - point.x, dy = p.y - point.y, dz = p.z - point.z;
    return std::sqrt((dz * dz + dy * dy) + dx * dx);
}

void CPickups::InjectHooks() {
    RH_ScopedClass(CPickups);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x454A70);
    RH_ScopedInstall(ReInit, 0x456E60);
    RH_ScopedInstall(AddToCollectedPickupsArray, 0x455240);
    RH_ScopedOverloadedInstall(CreatePickupCoorsCloseToCoors, "", 0x458A80, void(*)(float, float, float, float&, float&, float&), {});
    RH_ScopedInstall(CreateSomeMoney, 0x458970);
    RH_ScopedInstall(DetonateMinesHitByGunShot, 0x4590C0);
    RH_ScopedInstall(DoCollectableEffects, 0x455E20);
    RH_ScopedInstall(DoMineEffects, 0x4560E0);
    RH_ScopedInstall(DoMoneyEffects, 0x454E80);
    RH_ScopedInstall(DoPickUpEffects, 0x455720);
    RH_ScopedInstall(FindPickUpForThisObject, 0x4551C0);

    // Cannot be hooked at all for now due to ABI fuckery, the return value is 32 bit, but causes the function to assume the calling convention of of T* Function(T*, ...)
    //RH_ScopedInstall(GenerateNewOne, 0x456F20, { .reversed = false });

    RH_ScopedNamedInstall(GenerateNewOne_WeaponType_ExeAbi, "GenerateNewOne_WeaponType", 0x457380);
    RH_ScopedInstall(GetActualPickupIndex, 0x4552A0);
    RH_ScopedNamedInstall(GetNewUniquePickupIndex_ExeAbi, "GetNewUniquePickupIndex", 0x456A30);
    RH_ScopedNamedInstall(GetUniquePickupIndex_ExeAbi, "GetUniquePickupIndex", 0x455280);
    RH_ScopedInstall(GivePlayerGoodiesWithPickUpMI, 0x4564F0);
    RH_ScopedInstall(IsPickUpPickedUp, 0x454B40);
    RH_ScopedInstall(ModelForWeapon, 0x454AC0);
    RH_ScopedInstall(PassTime, 0x455200);
    RH_ScopedInstall(PickedUpHorseShoe, 0x455390);
    RH_ScopedInstall(PickedUpOyster, 0x4552D0);
    RH_ScopedInstall(PictureTaken, 0x456A70);
    RH_ScopedInstall(PlayerCanPickUpThisWeaponTypeAtThisMoment, 0x4554C0);
    RH_ScopedInstall(RemoveMissionPickUps, 0x456DE0);
    RH_ScopedInstall(RemovePickUp, 0x4573D0);
    RH_ScopedInstall(RemovePickUpsInArea, 0x456D30);
    RH_ScopedInstall(RemovePickupObjects, 0x455470);
    RH_ScopedInstall(RemoveUnnecessaryPickups, 0x4563A0);
    RH_ScopedInstall(RenderPickUpText, 0x455000);
    RH_ScopedInstall(TestForPickupsInBubble, 0x456450);
    RH_ScopedInstall(TryToMerge_WeaponType, 0x4555A0);
    RH_ScopedInstall(Update, 0x458DE0);
    RH_ScopedInstall(UpdateMoneyPerDay, 0x455680);
    RH_ScopedInstall(WeaponForModel, 0x454AE0);
    RH_ScopedInstall(Load, 0x5D35A0);
    RH_ScopedInstall(Save, 0x5D3540);

    RH_ScopedGlobalInstall(ModifyStringLabelForControlSetting, 0x454B70);
}

// 0x454A70
void CPickups::Init() {
    ZoneScoped;

    NumMessages = 0;
    for (auto& pickup : aPickUps) {
        pickup.m_nPickupType = PICKUP_NONE;
        pickup.m_nReferenceIndex = 1;
        pickup.m_pObject = nullptr;
    }
    aPickUpsCollected.fill(0);
    CollectedPickUpIndex = 0u;
    DisplayHelpMessage = 10u;
}

// 0x456E60
void CPickups::ReInit() {
    NumMessages = 0; // Como el original: 0x456E65 pone a cero NumMessages antes de recorrer los pickups
    rng::for_each(GetAllActivePickups(), [](auto& pickup) { pickup.Remove(); });
    Init();
}

// 0x455240
void CPickups::AddToCollectedPickupsArray(int32 pickupIndex) {
    aPickUpsCollected[CollectedPickUpIndex++] = GetUniquePickupIndex(pickupIndex).num;
    CollectedPickUpIndex %= std::size(aPickUpsCollected);
}

/*!
 * @addr 0x458A80
 * @brief Created a pickup close to pos (inX, inY, inZ)
 *
 * @param [out] outX, outY, outZ Created pickup's position
 */
void CPickups::CreatePickupCoorsCloseToCoors(float inX, float inY, float inZ, float& outX, float& outY, float& outZ) {
    for (int32 attempt = 0; attempt < 32; ++attempt) {
        const int32 randomAngle = CGeneral::GetRandomNumber() & 255;
        const float angleStep = std::bit_cast<float>(0x3CC90FDBu), radius = 1.5f;
        float pickupX, pickupY;
        // Como el original: fsin/fcos permanecen en x87 hasta multiplicar y sumar.
        __asm {
            fild randomAngle
            fmul angleStep
            fld st(0)
            fsin
            fmul radius
            fadd inX
            fstp pickupX
            fcos
            fmul radius
            fadd inY
            fstp pickupY
        }
        bool found;
        const float pickupZ = CWorld::FindGroundZFor3DCoord({pickupX, pickupY, inZ}, &found, nullptr) + 0.5f;
        if (!found) continue;
        const CVector from{inX, inY, inZ + 0.3f};
        const CVector difference{pickupX - from.x, pickupY - from.y, pickupZ - from.z};
        const float length = std::sqrt((difference.z * difference.z + difference.y * difference.y) + difference.x * difference.x);
        const float scale = (length + 0.4f) / length;
        const CVector to{difference.x * scale + from.x, difference.y * scale + from.y, difference.z * scale + from.z};
        // Como el original: cuatro consultas distintas al jugador, en el mismo orden.
        const auto p0 = FindPlayerCoors();
        const auto p1 = FindPlayerCoors();
        const float dy = pickupY - FindPlayerCoors().y;
        const float otherDy = pickupY - FindPlayerCoors().y;
        const float distance = std::sqrt(otherDy * dy + (pickupX - p1.x) * (pickupX - p0.x));
        if (distance <= 2.0f) {
            if (attempt <= 16) continue;
        } else if (attempt <= 16 && TestForPickupsInBubble({pickupX, pickupY, pickupZ}, 1.3f)) {
            continue;
        }
        if (!CWorld::GetIsLineOfSightClear(to, from, true, attempt < 16, false, attempt < 16, false, false, false)) continue;
        if (attempt <= 16 && CWorld::TestSphereAgainstWorld({pickupX, pickupY, pickupZ}, 1.2f, nullptr, false, true, false, false, false, false)) continue;
        outX = pickupX; outY = pickupY; outZ = pickupZ;
        return;
    }
    outX = inX; outY = inY; outZ = inZ + 0.4f;
}

/*!
 * @addr 0x458970
 * @brief Creates wads of money that is worth amount of money at the position coors.
 */
void CPickups::CreateSomeMoney(CVector coors, int32 amount) {
    const auto wads = std::min(amount / 20 + 1, 7);
    const auto perWad = amount / wads;

    for (auto i = 0; i < wads; i++) {
        bool result;
        // Como el original (0x4589B0..0x458A08): el angulo es (rand() & 0xFF) * 0x859BBC (2*pi/256) y el seno/coseno se
        // calcula con fsin/fcos x87 a precision completa; luego * 1.5 + coordenada, cada paso redondeado a float.
        const float angleStep = std::bit_cast<float>(0x3CC90FDBu), radius = 1.5f;
        const int32 randomX = CGeneral::GetRandomNumber() & 0xFF;
        float x = coors.x;
        __asm {
            fild randomX
            fmul angleStep
            fsin
            fmul radius
            fadd x
            fstp x
        }
        coors.x = x;
        const int32 randomY = CGeneral::GetRandomNumber() & 0xFF;
        float y = coors.y;
        __asm {
            fild randomY
            fmul angleStep
            fcos
            fmul radius
            fadd y
            fstp y
        }
        coors.y = y;
        coors.z = CWorld::FindGroundZFor3DCoord(coors, &result, nullptr) + 0.5f;

        if (result) {
            GenerateNewOne(coors, (uint16)MI_MONEY, PICKUP_MONEY, perWad + (CGeneral::GetRandomNumber() & 3));
        }
    }
}

// 0x4590C0
void CPickups::DetonateMinesHitByGunShot(const CVector& shotOrigin, const CVector& shotTarget) {
    for (auto& pickup : aPickUps) {
        if (pickup.m_nPickupType == PICKUP_NAUTICAL_MINE_ARMED) {
            pickup.ProcessGunShot(shotOrigin, shotTarget);
        }
    }
}

// Como el original (0x455E8D..0x455EE5 y 0x4560E0..0x4561A5): distancia (dz*dz + dy*dy) + dx*dx con fsqrt, y el brillo
// t = ((sin(a) + 1) * 0.5 * (radio - d)) * (1/radio) * escala truncado con _ftol2 (el EXE multiplica por la reciproca
// y NO divide); fsin a precision completa, redondeado a float solo por el fadd.
static uint8 PickupGlowByte(CEntity* entity, float distance, uint32 mask, float angleStep, float radius, float inverseRadius, float scale) {
    const int32 ticks = (int32)((CTimer::GetTimeInMS() + (uint32)(uintptr_t)entity) & mask);
    const float one = 1.0f, half = 0.5f;
    float wave;
    __asm {
        fild ticks
        fmul angleStep
        fsin
        fadd one
        fmul half
        fstp wave
    }
    const float value = ((wave * (radius - distance)) * inverseRadius) * scale;
    return (uint8)(int64)value; // _ftol2 y se usa el byte bajo (mov bl,al)
}

// 0x455E20
void CPickups::DoCollectableEffects(CEntity* entity) {
    const CVector entityPos = entity->GetPosition(); // Como el original: copia local que se pasa a las llamadas
    const CVector& camera = TheCamera.GetPosition();
    const float dx = camera.x - entityPos.x, dy = camera.y - entityPos.y, dz = camera.z - entityPos.z;
    const float d = std::sqrt((dz * dz + dy * dy) + dx * dx);

    if (d < 14.0f) {
        // shade of gray
        const uint8 t = PickupGlowByte(entity, d, 0x7FFu, std::bit_cast<float>(0x3B48F5C3u), 14.0f, std::bit_cast<float>(0x3D924925u), 255.0f);

        CShadows::StoreStaticShadow(
            (uint32)entity,
            SHADOW_ADDITIVE,
            gpShadowExplosionTex,
            entityPos,
            2.0f,
            0.0f,
            0.0f,
            -2.0f,
            0,
            t,
            t,
            t,
            4.0f,
            1.0f,
            40.0f,
            false,
            0.0f
        );
        CCoronas::RegisterCorona(
            (uint32)entity,
            nullptr,
            t,
            t,
            t,
            255,
            entityPos,
            0.6f,
            40.0f,
            CORONATYPE_TORUS,
            FLARETYPE_NONE,
            eCoronaReflType::CORREFL_NONE,
            eCoronaLOSCheck::LOSCHECK_OFF,
            eCoronaTrail::TRAIL_OFF,
            0.0f,
            false,
            1.5f,
            0,
            15.0f,
            false,
            false
        );
    }

    entity->GetMatrix().SetRotateZOnly(static_cast<float>(CTimer::GetTimeInMS() % 4096) * std::bit_cast<float>(0x3AC851ECu));
}

// 0x4560E0
void CPickups::DoMineEffects(CEntity* entity) {
    const CVector entityPos = entity->GetPosition(); // Como el original: copia local que se pasa a las llamadas
    const CVector& camera = TheCamera.GetPosition();
    const float dx = camera.x - entityPos.x, dy = camera.y - entityPos.y, dz = camera.z - entityPos.z;
    const float d = std::sqrt((dz * dz + dy * dy) + dx * dx);

    if (d < 20.0f) {
        // shade of red
        const uint8 t = PickupGlowByte(entity, d, 0x1FFu, std::bit_cast<float>(0x3C48F5C3u), 20.0f, std::bit_cast<float>(0x3D4CCCCDu), 64.0f);

        CShadows::StoreStaticShadow(
            (uint32)entity,
            SHADOW_ADDITIVE,
            gpShadowExplosionTex,
            entityPos,
            2.0f,
            0.0f,
            0.0f,
            -2.0f,
            0,
            t,
            0,
            0,
            4.0f,
            1.0f,
            40.0f,
            false,
            0.0f
        );
        CCoronas::RegisterCorona(
            (uint32)entity,
            nullptr,
            t,
            0,
            0,
            255,
            entityPos,
            0.6f,
            60.0f, // 0x4561FD: push 42700000h (60.0f), no 40.0f
            CORONATYPE_TORUS,
            FLARETYPE_NONE,
            eCoronaReflType::CORREFL_NONE,
            eCoronaLOSCheck::LOSCHECK_OFF,
            eCoronaTrail::TRAIL_OFF,
            0.0f,
            false,
            1.5f,
            0,
            15.0f,
            false,
            false
        );
    }

    entity->GetMatrix().SetRotateZOnly(static_cast<float>(CTimer::GetTimeInMS() % 1024) * std::bit_cast<float>(0x3BC851ECu));
}

// 0x454E80
void CPickups::DoMoneyEffects(CEntity* entity) {
    entity->GetMatrix().SetRotateZOnly(static_cast<float>(CTimer::GetTimeInMS() % 2048) * 0.0030566407f);
}

// 0x455720
void CPickups::DoPickUpEffects(CEntity* entity) {
    auto* const object = static_cast<CObject*>(entity);
    auto* const pickup = FindPickUpForThisObject(object);
    auto& flags = *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(object) + 0x140);
    const int32 model = entity->GetModelId();
    if (model == MI_PICKUP_CAMERA) {
        if (TheCamera.GetActiveCam().m_nMode == MODE_CAMERA) {
            flags &= ~0x02000000u;
        } else {
            flags |= 0x02000000u;
            if (*reinterpret_cast<uint8*>(0xB70153) < 5 || CPostEffects::IsVisionFXActive()) {
                const int32 red = 100 - static_cast<int32>((float(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000000u)) * -50.0f);
                const int32 ticks = CTimer::GetTimeInMS() & 8191;
                const float angleStep = std::bit_cast<float>(0x3A48F5C3u), bias = 1.7f, multiplier = 3.7f;
                float coronaSize;
                __asm {
                    fild ticks
                    fmul angleStep
                    fsin
                    fadd bias
                    fmul multiplier
                    fstp coronaSize
                }
                const uint8 green = static_cast<uint8>(static_cast<int32>(float(red) * 0.7f));
                // Como el original: el identificador es una direccion de la pila, no la del objeto.
                uint8 coronaId;
                CCoronas::RegisterCorona(reinterpret_cast<uint32>(&coronaId), nullptr, uint8(red), green, green, 255,
                    entity->GetPosition(), coronaSize, 100.0f, static_cast<eCoronaType>(1), FLARETYPE_NONE,
                    eCoronaReflType::CORREFL_NONE, eCoronaLOSCheck::LOSCHECK_OFF, eCoronaTrail::TRAIL_OFF,
                    0.0f, false, 1.5f, false, 15.0f, false, false);
            }
        }
    } else {
        flags = (flags & ~0x02000000u) | (pickup->PickUpShouldBeInvisible() ? 0x02000000u : 0);
    }
    if (flags & 0x02000000u) return;
    // Como el original: algunos modelos conservan los 16 bits bajos del argumento como color.
    // Sus objetos normales no solicitan texto y por ello no acceden a esa entrada.
    int16 colorIndex = static_cast<int16>(reinterpret_cast<uintptr_t>(entity));
    if (model == MI_PICKUP_ADRENALINE || model == MI_PICKUP_HEALTH || model == MI_PICKUP_BONUS ||
        model == MI_PICKUP_PROPERTY_FORSALE || model == MI_PICKUP_CLOTHES) {
        colorIndex = 47;
    } else if (model == MI_PICKUP_BODYARMOUR) {
        colorIndex = 48;
    } else if (model == MI_PICKUP_REVENUE) {
        colorIndex = 53;
    } else if (model != MI_PICKUP_BRIBE && model != MI_PICKUP_INFO && model != MI_PICKUP_KILLFRENZY &&
               model != MI_PICKUP_PROPERTY && model != MI_PICKUP_SAVEGAME) {
        colorIndex = static_cast<int16>(WeaponForModel(model));
    }
    const uint8 textKind = *reinterpret_cast<uint8*>(reinterpret_cast<uint8*>(object) + 0x13D);
    const uint16 price = *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(object) + 0x13E);
    if ((flags & 12) || textKind || price) {
        const auto& camera = TheCamera.GetPosition();
        const auto& position = entity->GetPosition();
        const float dy = camera.y - position.y, dx = camera.x - position.x;
        const float distance = std::sqrt(dy * dy + dx * dx);
        if (distance < 14.0f && NumMessages < 16) {
            const CVector textPosition{position.x, position.y, position.z + 0.7f};
            CVector screen; float width, height;
            if (CSprite::CalcScreenCoors(textPosition, &screen, &width, &height, true, true)) {
                auto& message = aMessages[NumMessages];
                message.pos.x = screen.x;
                message.pos.y = screen.y;
                message.pos.z = std::bit_cast<float>(static_cast<uint32>(WeaponForModel(model)));
                message.width = width; message.height = height;
                const auto* color = reinterpret_cast<const uint8*>(0x8A5FB0 + colorIndex * 8);
                message.color.r = color[0]; message.color.g = color[1]; message.color.b = color[2];
                const float alphaFactor = std::bit_cast<float>(0x3D924925u), one = 1.0f, alphaScale = 255.0f;
                int64 alphaValue; uint16 alphaOldControl, alphaTruncateControl;
                __asm {
                    fnstcw alphaOldControl
                    mov ax, alphaOldControl
                    or ax, 0C00h
                    mov alphaTruncateControl, ax
                    fld distance
                    fmul alphaFactor
                    fsubr one
                    fmul alphaScale
                    fldcw alphaTruncateControl
                    fistp alphaValue
                    fldcw alphaOldControl
                }
                message.color.a = static_cast<uint8>(alphaValue);
                message.flags = (message.flags & ~1u) | ((flags & 8) ? 1u : 0u);
                message.field_19 = textKind;
                message.price = uint32(price) * 5;
                if (model == MI_PICKUP_PROPERTY_FORSALE || model == MI_PICKUP_PROPERTY) {
                    message.text = const_cast<GxtChar*>(TheText.Get(CPickup::FindStringForTextIndex(static_cast<ePickupPropertyText>(FindPickUpForThisObject(object)->m_nFlags.nPropertyTextIndex))));
                    message.flags = (message.flags & ~2u) | (model == MI_PICKUP_PROPERTY_FORSALE ? 2u : 0u);
                } else {
                    message.text = nullptr;
                    message.flags &= ~2u;
                }
                ++NumMessages;
            }
        }
    }
    const auto& box = CModelInfo::GetModelInfo(model)->GetColModel()->m_boundBox;
    const float y = box.m_vecMax.y - box.m_vecMin.y, z = box.m_vecMax.z - box.m_vecMin.z;
    const float yz = z < y ? y : z;
    float dimension = box.m_vecMax.x - box.m_vecMin.x;
    if (dimension <= yz) dimension = z < y ? y : z;
    float scale = 1.2f / dimension;
    if (scale < 1.0f) scale = 1.0f;
    scale = (scale - 1.0f) * 0.6f + 1.0f;
    if (model == 362) scale = 1.2f;
    const int32 ticks = CTimer::GetTimeInMS() & 2047;
    const float angleStep = std::bit_cast<float>(0x3B4851ECu);
    float cosineScaled, sineScaled;
    __asm {
        fild ticks
        fmul angleStep
        fld st(0)
        fcos
        fmul scale
        fstp cosineScaled
        fsin
        fmul scale
        fstp sineScaled
    }
    auto& matrix = entity->GetMatrix();
    matrix.GetRight() = CVector{cosineScaled, sineScaled, 0.0f};
    matrix.GetForward() = CVector{-sineScaled, cosineScaled, 0.0f};
    matrix.GetUp() = CVector{0.0f, 0.0f, scale};
}

// 0x4551C0
CPickup* CPickups::FindPickUpForThisObject(CObject* object) {
    for (auto& pickup : GetAllActivePickups()) {
        if (pickup.m_pObject == object) {
            return &pickup;
        }
    }
    return aPickUps.data();
}

// returns pickup handle
// g
tPickupReference CPickups::GenerateNewOne(CVector coors, uint32 modelId, ePickupType pickupType, uint32 ammo, uint32 moneyPerDay, bool isEmpty, char* message) {
    auto retVal = plugin::CallAndReturn<int32, 0x456F20, CVector, uint32, ePickupType, uint32, uint32, bool, char*>(coors, modelId, pickupType, ammo, moneyPerDay, isEmpty, message);
    return tPickupReference(retVal);
}

/*!
 *
 * @param coors Position of new pickup
 * @param weaponType Weapon type
 * @param pickupType Pickup type
 * @param ammo
 * @param isEmpty
 * @param message
 * @return Pickup handle
 * @addr 0x457380
 */
tPickupReference CPickups::GenerateNewOne_WeaponType(CVector coors, eWeaponType weaponType, ePickupType pickupType, uint32 ammo, bool isEmpty, char* message) {
    return GenerateNewOne(coors, CWeaponInfo::GetWeaponInfo(weaponType)->m_nModelId1, pickupType, ammo, 0u, isEmpty, message);
}

/*!
 * @param pickupIndex Index of pickup
 * @return -1 if this index is not actual
 * @addr 0x4552A0
 */
int32 CPickups::GetActualPickupIndex(tPickupReference pickupRef) {
    if (pickupRef.num == -1)
        return -1;

    // Como el original (0x4552AB..0x4552BF): el indice es la mitad baja SIN signo (and eax,0FFFFh) y no se comprueba el rango.
    const uint32 index = (uint32)pickupRef.num & 0xFFFFu;
    if (pickupRef.refIndex != aPickUps.data()[index].m_nReferenceIndex)
        return -1;

    return (int32)index;
}

int32 CPickups::GenerateNewOne_WeaponType_ExeAbi(CVector coors, eWeaponType weaponType, ePickupType pickupType, uint32 ammo, bool isEmpty, char* message) {
    return GenerateNewOne_WeaponType(coors, weaponType, pickupType, ammo, isEmpty, message).num;
}

int32 CPickups::GetNewUniquePickupIndex_ExeAbi(int32 pickupIndex) {
    return GetNewUniquePickupIndex(pickupIndex).num;
}

int32 CPickups::GetUniquePickupIndex_ExeAbi(int32 pickupIndex) {
    return GetUniquePickupIndex(pickupIndex).num;
}

// 0x456A30
tPickupReference CPickups::GetNewUniquePickupIndex(int32 pickupIndex) {
    // Como el original: 0x456A44 "cmp cx,0FFFEh; jae" sobre el valor sin signo -> 0xFFFE y 0xFFFF vuelven a 1.
    auto& refIdx = aPickUps.data()[pickupIndex].m_nReferenceIndex;
    refIdx = ((uint16)refIdx >= 0xFFFEu) ? (int16)1 : (int16)(refIdx + 1);

    return GetUniquePickupIndex(pickupIndex);
}

// returns pickup handle
// 0x455280
tPickupReference CPickups::GetUniquePickupIndex(int32 pickupIndex) {
    // Como el original (0x455286): (referencia << 16) | indice, sin comprobar el rango
    return tPickupReference((int16)pickupIndex, aPickUps.data()[pickupIndex].m_nReferenceIndex);
}

// returns TRUE if player got goodies
// 0x4564F0
bool CPickups::GivePlayerGoodiesWithPickUpMI(uint16 modelId, int32 playerId) {
    // Como el original (0x456510..0x456535): playerId > 0 -> Players[playerId]; si no, el jugador en foco (byte 0xB7CD74).
    // Los accesos a la maxima salud/armadura usan el playerId SIN corregir (imul eax,eax,190h en 0x456514/0x456596/0x4565F3).
    auto* ped = CWorld::Players[playerId > 0 ? playerId : (int32)CWorld::PlayerInFocus].m_pPed;
    const auto& rawInfo = (&CWorld::Players[0])[playerId];

    if (modelId == MI_PICKUP_ADRENALINE) {
        ped->GetPlayerData()->m_bAdrenaline = true;
        ped->GetPlayerData()->m_nAdrenalineEndTime = CTimer::GetTimeInMS() + 20'000;
        ped->ResetSprintEnergy();
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_ADRENALINE);
        return true;
    }

    if (modelId == MI_PICKUP_BODYARMOUR) {
        ped->m_fArmour = (float)rawInfo.m_nMaxArmour;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_BODY_ARMOUR);
        return true;
    }

    if (modelId == MI_PICKUP_INFO) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == MI_PICKUP_HEALTH) {
        auto maxHealth = rawInfo.m_nMaxHealth;
        // Como el original: 0x45660A _ftol2 (truncado a 64 bits) y se pasa el dword bajo.
        CStats::UpdateStatsAddToHealth((uint32)(int64)((float)maxHealth - ped->m_fHealth));
        ped->m_fHealth = (float)maxHealth;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_HEALTH);
        return true;
    }

    if (modelId == MI_PICKUP_BONUS) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == MI_PICKUP_BRIBE) {
        // Como el original (0x456661..0x456698): si wanted-1 < 0 -> 0; si no, vuelve a consultar el nivel (segunda llamada).
        int32 wantedLevel = (int32)FindPlayerPed()->GetWantedLevel() - 1;
        if (wantedLevel < 0)
            wantedLevel = 0;
        else
            wantedLevel = (int32)FindPlayerPed()->GetWantedLevel() - 1;
        FindPlayerPed(0)->SetWantedLevel((eWantedLevel)wantedLevel);
        CStats::IncrementStat(STAT_NUMBER_OF_POLICE_BRIBES, 1.0f);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == MI_PICKUP_KILLFRENZY) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == MODEL_JETPACK) {
        auto* task = new CTaskSimpleJetPack(nullptr, 10.0f, 0, nullptr);
        CEventScriptCommand event(3, task, 0);
        ped->GetEventGroup().Add(&event, false);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == MI_OYSTER) {
        PickedUpOyster();
        return true;
    }

    if (modelId == MI_HORSESHOE) {
        PickedUpHorseShoe();
        return true;
    }

    return false;
}

/*!
 * @brief Check if pickup was picked up, and then mark it as not picked up.
 * @addr 0x454B40
 */
bool CPickups::IsPickUpPickedUp(tPickupReference pickupRef) {
    if (const auto it = rng::find(aPickUpsCollected, pickupRef.num); it != aPickUpsCollected.end()) {
        *it = 0; // Reset
        return true;
    }
    return false;
}

/*!
 * @addr 0x454AC0
 * @returns The `nModelId1` of the given weapon type.
 */
int32 CPickups::ModelForWeapon(eWeaponType weaponType) {
    return CWeaponInfo::GetWeaponInfo(weaponType)->m_nModelId1;
}

/*!
 * @brief Update each pickup's (except if of type NONE or ASSET_REVENUE) `nRegenerationTime` field.
 * @addr 0x455200
 */
void CPickups::PassTime(uint32 time) {
    for (auto& pickup : aPickUps) {
        switch (pickup.m_nPickupType) {
        case PICKUP_NONE:
        case PICKUP_ASSET_REVENUE:
            continue;
        }

        if (pickup.m_nRegenerationTime <= time) {
            pickup.m_nRegenerationTime = 0;
        } else {
            pickup.m_nRegenerationTime -= time;
        }
    }
}

// 0x455390
void CPickups::PickedUpHorseShoe() {
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_COLLECTABLE1);

    CStats::IncrementStat(STAT_HORSESHOES_COLLECTED);
    CStats::IncrementStat(STAT_LUCK, 1000.f / CStats::GetStatValue(STAT_TOTAL_HORSESHOES)); // TODO: Is this some inlined stuff? (The division part)

    CWorld::Players[0].m_nMoney += 100; // Como el original: 0xB7CE50 (Players[0]) absoluto

    const auto collected = CStats::GetStatValue(STAT_HORSESHOES_COLLECTED);
    const auto total = CStats::GetStatValue(STAT_TOTAL_HORSESHOES);
    if (collected == total) {
        CGarages::TriggerMessage("HO_ALL");
        CWorld::Players[0].m_nMoney += 100'000; // Como el original: 0xB7CE50 (Players[0]) absoluto
    } else {
        // Como el original (0x45535C..0x455379): las estadisticas se leen DE NUEVO (total y luego recogidas) para los argumentos
        const auto totalArg = static_cast<int16>(static_cast<int32>(CStats::GetStatValue(STAT_TOTAL_HORSESHOES)));
        const auto collectedArg = static_cast<int16>(static_cast<int32>(CStats::GetStatValue(STAT_HORSESHOES_COLLECTED)));
        CGarages::TriggerMessage("HO_ONE", collectedArg, 5000u, totalArg);
    }
}

// 0x4552D0
void CPickups::PickedUpOyster() {
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_COLLECTABLE1);

    CStats::IncrementStat(STAT_OYSTERS_COLLECTED);
    CWorld::Players[0].m_nMoney += 100; // Como el original: 0xB7CE50 (Players[0]) absoluto

    const auto collected = CStats::GetStatValue(STAT_OYSTERS_COLLECTED);
    const auto total = CStats::GetStatValue(STAT_TOTAL_OYSTERS);
    if (collected == total) {
        CGarages::TriggerMessage("OY_ALL");
        CWorld::Players[0].m_nMoney += 100'000; // Como el original: 0xB7CE50 (Players[0]) absoluto
    } else {
        // Como el original (0x45535C..0x455379): las estadisticas se leen DE NUEVO (total y luego recogidas) para los argumentos
        const auto totalArg = static_cast<int16>(static_cast<int32>(CStats::GetStatValue(STAT_TOTAL_OYSTERS)));
        const auto collectedArg = static_cast<int16>(static_cast<int32>(CStats::GetStatValue(STAT_OYSTERS_COLLECTED)));
        CGarages::TriggerMessage("OY_ONE", collectedArg, 5000u, totalArg);
    }
}

// 0x456A70
void CPickups::PictureTaken() {
    int32 capturedPickup = -1;                                       // edi
    float lastFoundDist = std::bit_cast<float>(0x497423FEu);          // 0x456A79 (999999.875f)

    for (int32 i = 0; i < (int32)MAX_NUM_PICKUPS; ++i) {
        const auto& pickup = aPickUps.data()[i];
        if (pickup.m_nPickupType != PICKUP_SNAPSHOT)
            continue;

        const CVector pupPos = pickup.GetPosn();
        // Como el original (0x456AED..0x456B17): restas pickup - camara, suma (dz*dz + dy*dy) + dx*dx y fsqrt.
        const float dist = PickupDistanceTo(pickup, TheCamera.GetPosition());

        // 0x456B26: (90 / fov) * 20 > dist
        if (!(90.0f / TheCamera.FindCamFOV() * 20.0f > dist))
            continue;

        // Como el original (0x456B90..0x456BBD): la visibilidad se evalua ANTES de comparar con la distancia mas cercana, con la
        // matriz 0xB6FA74 y, si el espejo esta activo (byte 0xB6F998), con la matriz 0xB6FABC. No usa la sobrecarga de dos argumentos.
        bool visible = TheCamera.IsSphereVisible(pupPos, 0.2f, reinterpret_cast<RwMatrix*>(0xB6FA74));
        if (!visible && *reinterpret_cast<uint8*>(0xB6F998))
            visible = TheCamera.IsSphereVisible(pupPos, 0.2f, reinterpret_cast<RwMatrix*>(0xB6FABC));
        if (!visible)
            continue;

        // 0x456BBF: fcomp + test ah,5 + jp -> solo si dist < lastFoundDist
        if (dist < lastFoundDist) {
            capturedPickup = i;
            lastFoundDist = dist;
        }
    }

    if (capturedPickup < 0)
        return;

    aPickUps.data()[capturedPickup].Remove();

    // Como el original (0x456C48..0x456C68): +100 (add edi,64h), no 100000.
    CWorld::Players[0].m_nMoney += 100;

    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_COLLECTABLE1);
    CStats::IncrementStat(STAT_SNAPSHOTS_TAKEN, 1.0f);

    const auto taken = CStats::GetStatValue(STAT_SNAPSHOTS_TAKEN);
    const auto total = CStats::GetStatValue(STAT_TOTAL_SNAPSHOTS);
    if (taken == total) {
        CGarages::TriggerMessage("SN_ALL");
        FindPlayerInfo().m_nMoney += 100'000; // 0x456CBB: Players[PlayerInFocus]
    } else {
        // Como el original (0x456CE6..0x456D0F): las estadisticas se leen DE NUEVO (total y luego tomadas) para los argumentos
        const auto totalArg = static_cast<int16>(static_cast<int32>(CStats::GetStatValue(STAT_TOTAL_SNAPSHOTS)));
        const auto takenArg = static_cast<int16>(static_cast<int32>(CStats::GetStatValue(STAT_SNAPSHOTS_TAKEN)));
        CGarages::TriggerMessage("SN_ONE", takenArg, 5000u, totalArg);
    }
}

// 0x4554C0
bool CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment(eWeaponType weaponType) {
    if (!CWeaponInfo::GetWeaponInfo(weaponType)->flags.bAimWithArm) {
        if (FindPlayerPed()->GetIntelligence()->GetTaskJetPack()) {
            return false;
        }
    }
    return true;
}

// 0x456DE0
void CPickups::RemoveMissionPickUps() {
    for (auto&& [i, pickup] : rngv::enumerate(aPickUps)) {
        switch (pickup.m_nPickupType) {
        case PICKUP_ONCE_FOR_MISSION: {
            CRadar::ClearBlipForEntity(BLIP_PICKUP, GetUniquePickupIndex(i).num);
            pickup.GetRidOfObjects();

            pickup.m_nFlags.bDisabled = true;
            pickup.m_nPickupType = PICKUP_NONE;
            break;
        }
        }
    }
}

// 0x4573D0
void CPickups::RemovePickUp(tPickupReference pickupRef) {
    if (const auto i = GetActualPickupIndex(pickupRef); i != -1) {
        aPickUps[i].Remove();
    }
}

// 0x456D30
void CPickups::RemovePickUpsInArea(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    CBoundingBox bb{ { minX, minY, minZ }, { maxX, maxY, maxZ } }; // They didn't use a bounding box, but it's nicer to do so.

    for (auto& pickup : GetAllActivePickups()) {
        if (bb.IsPointWithin(pickup.GetPosn())) {
            pickup.Remove();
        }
    }
}

// 0x455470
void CPickups::RemovePickupObjects() {
    for (auto& pickup : GetAllActivePickups()) {
        if (pickup.m_pObject) {
            pickup.GetRidOfObjects();
            pickup.m_nFlags.bVisible = false;
        }
    }
}

// remove pickups with types PICKUP_ONCE_TIMEOUT and PICKUP_MONEY in area
// 0x4563A0
void CPickups::RemoveUnnecessaryPickups(const CVector& posn, float radius) {
    for (auto& pickup : aPickUps) {
        switch (pickup.m_nPickupType) {
        case PICKUP_ONCE_TIMEOUT:
        case PICKUP_MONEY: {
            // Como el original: 0x456420 "dist < radius" con la formula x87 del EXE (no IsPointInSphere).
            if (PickupDistanceTo(pickup, posn) < radius) {
                pickup.Remove();
            }
            break;
        }
        }
    }
}

// 0x455000
void CPickups::RenderPickUpText() {
    // Como el original: el texto se arma en los buffers globales gString (0xB71670) y gGxtString (0xC1B100) y,
    // si el mensaje no lleva numeros (flags & 2 == 0), se imprime directamente message.text (0x4550B5..0x455058).
    for (int32 i = 0; i < (int32)NumMessages; ++i) {
        const auto& message = aMessages.data()[i];
        const GxtChar* text;
        if (message.price != 0u) {
            std::sprintf(gString, "$%d", (int32)message.price); // 0x455041 (sprintf)
            AsciiToGxtChar(gString, gGxtString);               // 0x455050
            text = gGxtString;
        } else {
            text = message.text;
            if (!text)
                continue;
            if (message.flags & 2) {
                CMessages::InsertNumberInString(text, 0, 0, 0, 0, 0, 0, gGxtString); // 0x4550D3
                text = gGxtString;
            }
        }

        CFont::SetProportional(true);
        CFont::SetBackground(false, false);

        // Como el original (0x455073..0x4550FE): SCREEN_STRETCH_X(1) es una division ENTERA (imul 66666667h, sar 8) convertida
        // a float, y width/height se multiplican por 0x858F10 (= 0x3D088889, no una division entre 30). La seleccion es
        // "S < v ? S : v" (fcomp + test ah,5 + jp): con NaN devuelve v, a diferencia de std::min.
        const float stretch = (float)((int32)RsGlobal.maximumWidth / 640);
        const float k = std::bit_cast<float>(0x3D088889u);
        const float hv = message.height * k;
        const float scaleY = (stretch < hv) ? stretch : hv;
        const float wv = message.width * k;
        const float scaleX = (stretch < wv) ? stretch : wv;
        CFont::SetScale(scaleX, scaleY);
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetCentreSize((float)RsGlobal.maximumWidth);
        CFont::SetColor(message.color);
        CFont::SetFontStyle(eFontStyle::FONT_PRICEDOWN);
        CFont::PrintString(message.pos.x, message.pos.y, text);
    }
    NumMessages = 0;
}

// check for pickups in area
// 0x456450
bool CPickups::TestForPickupsInBubble(const CVector posn, float radius) {
    // NOTE: (Possible bug) - They dont check if the pickup is active (eg: type != NONE)
    // Como el original (0x4564C9): "dist < radius" con la formula x87 del EXE.
    return rng::any_of(aPickUps, [&](const CPickup& p) {
        return PickupDistanceTo(p, posn) < radius;
    });
}

// search for pickup in area (radius = 5.5 units) with this weapon model and pickup type and add ammo to this pickup; returns TRUE if merged
// 0x4555A0
bool CPickups::TryToMerge_WeaponType(CVector posn, eWeaponType weaponType, ePickupType pickupType, uint32 ammo, bool arg4) {
    const auto mi = CWeaponInfo::GetWeaponInfo(weaponType)->m_nModelId1;

    for (auto& pickup : aPickUps) {
        // Como el original: 0x4555C2 compara el tipo; 0x4555C7 hace movzx del modelo (uint16, sin signo) frente a modelId1 (dword);
        // 0x455638 compara la distancia con 5.5 (0x859B4C) con "dist < 5.5" (fcomp + test ah,5 + jnp).
        if (pickup.m_nPickupType == pickupType && (uint32)(uint16)pickup.m_nModelIndex == (uint32)mi && PickupDistanceTo(pickup, posn) < 5.5f) {
            pickup.m_nAmmo += ammo;

            return true;
        }
    }
    return false;
}

// 0x458DE0
void CPickups::Update() {
    ZoneScoped;

    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    auto start = 620 * (CTimer::GetFrameCounter() % 32) / 32;
    auto end   = 620 * (CTimer::GetFrameCounter() % 32 + 1) / 32;
    for (auto i = start; i < end; i++) {
        auto& pickup = aPickUps[i];
        if (pickup.m_nPickupType == PICKUP_NONE)
            continue;

        if (pickup.m_nFlags.bVisible = pickup.IsVisible()) {
            if (!pickup.m_nFlags.bDisabled && !pickup.m_pObject) {
                pickup.GiveUsAPickUpObject(pickup.m_pObject);

                if (auto& obj = pickup.m_pObject; obj) {
                    CWorld::Add(obj);
                }
            }
        } else {
            pickup.GetRidOfObjects();
        }
    }

    const auto pad = CPad::GetPad();
    if (pad->CollectPickupJustDown()) {
        CollectPickupBuffer = 6;
    } else {
        // Como el original: 0x458ECB..0x458EDB resta 1 y fija el minimo en 0 (tambien para valores negativos)
        CollectPickupBuffer = std::max(CollectPickupBuffer - 1, 0);
    }

    if (PlayerOnWeaponPickup) {
        PlayerOnWeaponPickup = std::max(PlayerOnWeaponPickup - 1, 0); // 0x458EE1..0x458EF5
    }

    if (pad->GetTarget()) {
        CollectPickupBuffer = 0;
    }

    // Como el original (0x458F1A..0x458F9B): ambos indicadores consultan las tareas del jugador 1 (Players[0].m_pPed, no
    // FindPlayerPed); el del jugador 2 solo se evalua si existe Players[1].m_pPed.
    const auto isPlayer1Busy = [] {
        auto* intelligence = CWorld::Players[0].m_pPed->GetIntelligence();
        return intelligence->FindTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER) || intelligence->FindTaskByType(TASK_COMPLEX_USE_MOBILE_PHONE);
    };
    const bool p1NotBusy = !isPlayer1Busy();
    bool p2Allowed = false;
    if (CWorld::Players[1].m_pPed) {
        p2Allowed = !isPlayer1Busy();
    }

    start = 620 * (CTimer::GetFrameCounter() % 6) / 6;
    end   = 620 * (CTimer::GetFrameCounter() % 6 + 1) / 6;
    for (auto i = start; i < end; i++) {
        auto& pickup = aPickUps[i];

        if (pickup.m_nPickupType == PICKUP_NONE || !pickup.m_nFlags.bVisible)
            continue;

        // Como el original (0x459003..0x45909F): si Update del jugador 1 devuelve false se cae (0x459069) a la rama del
        // jugador 2, que vuelve a comprobar tipo y visibilidad (Update pudo cambiarlos). "p2Allowed" implica jugador 1
        // libre, asi que con dos jugadores se llama tambien a Update del jugador 2: no es un "else".
        if (p1NotBusy) {
            if (pickup.Update(FindPlayerPed(), FindPlayerVehicle(), CWorld::PlayerInFocus)) {
                AddToCollectedPickupsArray(i);
                continue;
            }
        }
        if (p2Allowed && pickup.m_nPickupType != PICKUP_NONE && pickup.m_nFlags.bVisible) {
            if (pickup.Update(FindPlayerPed(1), FindPlayerVehicle(1), PED_TYPE_PLAYER2)) {
                AddToCollectedPickupsArray(i);
            }
        }
    }
}

// 0x455680
void CPickups::UpdateMoneyPerDay(tPickupReference pickupRef, uint16 money) {
    if (auto idx = GetActualPickupIndex(pickupRef); idx != -1) {
        aPickUps[idx].m_nMoneyPerDay = money;
    }
}

// 0x454AE0
eWeaponType CPickups::WeaponForModel(int32 modelId) {
    if (modelId == MI_PICKUP_BODYARMOUR) {
        return WEAPON_ARMOUR;
    }
    if (modelId == MI_PICKUP_HEALTH) {
        return WEAPON_LAST_WEAPON;
    }
    if (modelId == MI_PICKUP_ADRENALINE) {
        return WEAPON_ARMOUR;
    }

    switch (modelId) {
    case MODEL_JETPACK:
        return WEAPON_LAST_WEAPON;

    case MODEL_INVALID:
        return WEAPON_UNARMED;
    }

    if (auto mi = CModelInfo::GetModelInfo(modelId); mi->GetModelType() == MODEL_INFO_WEAPON) {
        return mi->AsWeaponModelInfoPtr()->GetWeaponInfo();
    }

    return WEAPON_UNARMED;
}

// 0x5D35A0
bool CPickups::Load() {
    for (auto& pickup : aPickUps) {
        CGenericGameStorage::LoadDataFromWorkBuffer(pickup);
        if (pickup.m_nPickupType != PICKUP_NONE && pickup.m_pObject) {
            pickup.m_pObject = nullptr;
            pickup.m_nFlags.bVisible = false;
        }
    }
    NumMessages = 0u;
    CGenericGameStorage::LoadDataFromWorkBuffer(CPickups::CollectedPickUpIndex);
    CGenericGameStorage::LoadDataFromWorkBuffer(CPickups::DisplayHelpMessage);

    for (auto& collected : aPickUpsCollected) {
        CGenericGameStorage::LoadDataFromWorkBuffer(collected);
    }
    return true; // Como el original: 0x5D361A mov al,1
}
// 0x5D3540
bool CPickups::Save() {
    for (auto& pickup : aPickUps) {
        CGenericGameStorage::SaveDataToWorkBuffer(pickup);
    }
    CGenericGameStorage::SaveDataToWorkBuffer(CPickups::CollectedPickUpIndex);
    CGenericGameStorage::SaveDataToWorkBuffer(CPickups::DisplayHelpMessage);

    for (auto& collected : aPickUpsCollected) {
        CGenericGameStorage::SaveDataToWorkBuffer(collected);
    }
    return true; // Como el original: 0x5D3594 mov al,1
}

// 0x454B70
void ModifyStringLabelForControlSetting(char* stringLabel) {
    const auto len = strlen(stringLabel);

    // Como el original: 0x454B8B "cmp esi,2; jle" -> con longitud 2 o menos no hace nada
    if (len <= 2 || stringLabel[len - 2] != '_')
        return;

    switch (CPad::GetPad(0)->Mode) {
    case 0:
    case 1:
        stringLabel[len - 1] = 'L';
        break;
    case 2:
        stringLabel[len - 1] = 'T';
        break;
    case 3:
        stringLabel[len - 1] = 'C';
        break;
    }
}
