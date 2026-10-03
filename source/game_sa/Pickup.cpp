/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPickup
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Pickup.h"
#include "ModelIndices.h"

using namespace ModelIndices;

// Como el original: 0x454C70 solo toca eax y ecx; su llamador del EXE (0x458E42, en CPickups)
// conserva el puntero del pickup en edx a través de la llamada (mov ecx,edx justo después).
static bool __fastcall PickupIsVisible(CPickup* pickup) {
    return pickup->IsVisible();
}

static __declspec(naked) void PickupIsVisibleKeepEdx() {
    __asm {
        push edx
        call PickupIsVisible
        pop  edx
        ret
    }
}

void CPickup::InjectHooks() {
    RH_ScopedClass(CPickup);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ExtractAmmoFromPickup, 0x454BE0);
    RH_ScopedInstall(FindStringForTextIndex, 0x455540);
    RH_ScopedInstall(FindTextIndexForString, 0x455500);
    RH_ScopedInstall(GetPosn, 0x4549A0);
    RH_ScopedInstall(GetRidOfObjects, 0x454CF0);
    RH_ScopedInstall(GiveUsAPickUpObject, 0x4567E0);
    // Envoltorio que conserva edx (ver PickupIsVisibleKeepEdx)
    RH_ScopedNamedGlobalInstall(PickupIsVisibleKeepEdx, "IsVisible", 0x454C70);
    RH_ScopedInstall(PickUpShouldBeInvisible, 0x454D20);
    RH_ScopedInstall(ProcessGunShot, 0x4588B0);
    RH_ScopedInstall(Remove, 0x4556C0);
    RH_ScopedInstall(SetPosn, 0x454960);
    RH_ScopedInstall(Update, 0x457410);
}

// Give player an ammo from weapon pickup
// 0x454BE0
void CPickup::ExtractAmmoFromPickup(CPlayerPed* player) {
    const auto weapon = CPickups::WeaponForModel(m_pObject->m_nModelIndex);
    // Como el original: la información del arma se lee antes de mirar el tipo
    const auto slot = CWeaponInfo::GetWeaponInfo(weapon)->m_nSlot;

    if (m_nPickupType == PICKUP_IN_SHOP)
        return;

    switch (slot) {
    case 3:
    case 4:
    case 5:
        break;
    default:
        return;
    }

    const auto EmptyAmmo = [this] {
        m_nFlags.bEmpty = true;
        m_nAmmo = 0;
    };

    if (m_nAmmo) {
        player->GrantAmmo(weapon, m_nAmmo);
    } else {
        if (m_nFlags.bEmpty) {
            return EmptyAmmo();
        }

        player->GrantAmmo(weapon, AmmoForWeapon_OnStreet[weapon]);
    }

    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_WEAPON);
    EmptyAmmo();
}

// 0x455540
const char* CPickup::FindStringForTextIndex(ePickupPropertyText index) {
    switch (index) {
    case PICKUP_PROPERTY_TEXT_CAN_BUY:
        return "PROP_3"; // Press ~k~~PED_ANSWER_PHONE~ to buy this property.
    case PICKUP_PROPERTY_TEXT_CANT_BUY:
        return "PROP_4"; // You cannot buy this property yet.
    default:
        return "FESZ_CA"; // Cancel (Como el original: cualquier otro índice)
    }
}

// 0x455500
ePickupPropertyText CPickup::FindTextIndexForString(const char* message) {
    if (!message)
        return PICKUP_PROPERTY_TEXT_CANCEL;

    if (!_stricmp("PROP_3", message)) {
        return PICKUP_PROPERTY_TEXT_CAN_BUY;
    } else if (!_stricmp("PROP_4", message)) {
        return PICKUP_PROPERTY_TEXT_CANT_BUY;
    }

    return PICKUP_PROPERTY_TEXT_CANCEL;
}

// Delete pickup's object (CObject)
// 0x454CF0
void CPickup::GetRidOfObjects() {
    if (m_pObject) {
        CWorld::Remove(m_pObject);
        // Como el original: destructor virtual y después se anula el puntero
        delete m_pObject;
        m_pObject = nullptr;
    }
}

// Creates an object (CObject) for pickup. slotIndex - object to replace; use -1 (or any negative value) to create a new object
// 0x4567E0
void CPickup::GiveUsAPickUpObject(CObject*& obj, int32 slotIndex) {
    // Como el original: el objeto de salida se anula antes de mirar la cinemática
    obj = nullptr;

    if (CCutsceneMgr::HasLoaded()) {
        return;
    }

    const auto mi = CModelInfo::GetModelInfo((uint16)m_nModelIndex);
    if (mi->GetModelType() == MODEL_INFO_WEAPON) {
        CWeaponInfo::GetWeaponInfo(mi->AsWeaponModelInfoPtr()->GetWeaponInfo());
    }

    if (slotIndex < 0) {
        obj = CObject::Create((uint16)m_nModelIndex, false);
    } else {
        CPools::MakeSureSlotInObjectPoolIsEmpty(slotIndex);
        // Como el original: si tras vaciar el hueco ya hay objeto, se configura ese
        if (!obj) {
            obj = new (slotIndex << 8) CObject((uint16)m_nModelIndex, false);
        }
    }
    if (!obj) {
        return;
    }
    obj->m_nObjectType = OBJECT_TYPE_DECORATION;
    obj->GetPosition() = GetPosn();

    if (obj->m_nModelIndex == MI_OYSTER) {
        obj->m_bUnderwater = true;
    }

    // Como el original: -pi/2 exacto del EXE (0xBFC90FDB) y UpdateRW en lugar de SetHeading
    obj->SetOrientation(0.0f, 0.0f, std::bit_cast<float>(0xBFC90FDBu));
    obj->UpdateRwMatrix();
    obj->UpdateRwFrame();
    obj->physicalFlags.bApplyGravity = false;
    obj->physicalFlags.bExplosionProof = true;
    obj->SetUsesCollision(false);
    obj->objectFlags.bIsPickup = true;
    // Como el original: el bit 0x2 del objeto copia bEmpty del pickup
    obj->objectFlags.b0x02 = m_nFlags.bEmpty;
    obj->m_bHasPreRenderEffects = true;
    obj->objectFlags.bDoNotRender = PickUpShouldBeInvisible();
    obj->m_bTunnelTransition = true;
    CEntity::RegisterReference(obj);

    if ((uint16)m_nModelIndex == (uint16)MI_PICKUP_BONUS || (uint16)m_nModelIndex == (uint16)MI_PICKUP_CLOTHES) {
        obj->m_nBonusValue = (uint8)m_nAmmo;
    } else {
        obj->m_nBonusValue = 0;
    }

    switch (m_nPickupType) {
    case PICKUP_IN_SHOP_OUT_OF_STOCK:
        obj->objectFlags.bPickupInShopOutOfStock = true;
        obj->physicalFlags.bRenderScorched = true; // ?
        break;
    case PICKUP_PROPERTY_FORSALE:
        obj->objectFlags.bPickupPropertyForSale = true;
        obj->m_wCostValue = (uint16)((int32)m_nAmmo / 5); // Como el original: división con signo
        break;
    default:
        break;
    }
}

// Is pickup visible (checks if distance between pickup and camera is shorter than 100 units)
// 0x454C70
bool CPickup::IsVisible() {
    return DistanceBetweenPoints2D(GetPosn2D(), TheCamera.GetPosition()) < 100.0f;
}

// 0x454D20
bool CPickup::PickUpShouldBeInvisible() {
    // Como el original: en cinemática siempre es invisible
    if (CCutsceneMgr::IsRunning())
        return true;

    // Como el original: el frenesí solo se ve fuera de misión, sin frenesí en curso y si la versión lo permite
    if ((uint16)m_nModelIndex == (uint16)MI_PICKUP_KILLFRENZY && (CTheScripts::IsPlayerOnAMission() || CDarkel::FrenzyOnGoing() || !CLocalisation::KillFrenzy()))
        return true;

    // Como el original: los pickups de dos jugadores no se ven si existe el jugador 2
    if (m_nPickupType == PICKUP_2P && FindPlayerPed(1))
        return true;

    // Como el original: en la versión alemana el frenesí de dos jugadores se oculta en dos parques
    if ((uint16)m_nModelIndex == (uint16)MI_PICKUP_2P_KILLFRENZY && CLocalisation::GermanGame()) {
        if (DistanceBetweenPoints2D(GetPosn2D(), CVector2D{ +1479.0f, -1658.0f }) < 10.0f) // Pershing Square Park
            return true;
        const CVector diff = GetPosn() - CVector{ 2511.0f, 1208.0f, 0.0f };
        if (std::sqrt(diff.x * diff.x + diff.y * diff.y) < 10.0f)
            return true;
    }

    if (TheCamera.m_bWideScreenOn && m_nPickupType != PICKUP_ASSET_REVENUE && m_nModelIndex != MODEL_FIRE_EX)
        return true;

    // Como el original: en cooperativo se ocultan las armas que no sirven para dos jugadores
    return CGameLogic::IsCoopGameGoingOn() && !CWeapon::CanBeUsedFor2Player(CPickups::WeaponForModel((uint16)m_nModelIndex));
}

// Checks if pickup collides with line (origin;target), removes pickup and creates an explosion. Used in previous GTA games for mine pickup
// 0x4588B0
void CPickup::ProcessGunShot(const CVector& start, const CVector& end) {
    if (!m_pObject)
        return;

    if (const auto& posn = m_pObject->GetPosition(); CCollision::TestLineSphere({start, end}, {posn, 4.0f})) {
        CExplosion::AddExplosion(nullptr, nullptr, EXPLOSION_MINE, posn, 0, true, -1.0f, false);
        Remove();
    }
}

// 0x4556C0
void CPickup::Remove() {
    auto pickupRef = tPickupReference(*this);
    CRadar::ClearBlipForEntity(BLIP_PICKUP, pickupRef.num);
    GetRidOfObjects();
    m_nPickupType = PICKUP_NONE;
    m_nFlags.bDisabled = true;
}

// Como el original: fild del producto (exacto), + 2^32 si es negativo, y fmul por 1/1440000 a 24 bits
static float PickupRevenueIncrement(uint32 product) {
    float result;
    const float scale = 1.0f / 1'440'000.0f;
    const float two32 = 4294967296.0f;
    __asm {
        fild  dword ptr product
        mov   eax, product
        test  eax, eax
        jge   positive
        fadd  dword ptr two32
    positive:
        fmul  dword ptr scale
        fstp  dword ptr result
    }
    return result;
}

// Updates the pickup. Returns TRUE if pickup was removed/disabled
// 0x457410
bool CPickup::Update(CPlayerPed* player, CVehicle* vehicle, int32 playerId) {
    auto& pt = m_nPickupType;
    bool isRemoved = false;

    if (m_pObject) {
        m_pObject->SetPosn(GetPosn());
    }

    if (pt == PICKUP_ASSET_REVENUE) {
        const auto now = CTimer::GetTimeInMS();
        const auto elapsed = now - m_nRegenerationTime;
        m_nRegenerationTime = now;

        // Como el original: distancia 3D con el orden (dz² + dy²) + dx²
        const auto posn = GetPosn();
        const auto coors = FindPlayerCoors();
        const float dx = coors.x - posn.x, dy = coors.y - posn.y, dz = coors.z - posn.z;
        if (std::sqrt(dz * dz + dy * dy + dx * dx) > 10.0f) {
            // Como el original: producto entero (dinero por día * tiempo) en 32 bits, sin signo, por 1/1440000
            m_fRevenueValue += PickupRevenueIncrement((uint32)m_nMoneyPerDay * elapsed);
        }
        // Como el original: mínimo con el ammo con signo; un valor NaN se queda con el ammo
        const float ammoF = (float)(int32)m_nAmmo;
        m_fRevenueValue = m_fRevenueValue < ammoF ? m_fRevenueValue : ammoF;

        if (m_pObject) {
            // Como el original: (valor * 0.2) truncado cuando el valor no es menor que 10
            m_pObject->m_wCostValue = m_fRevenueValue < 10.0f ? 0 : (uint16)(int64)(m_fRevenueValue * 0.2f);
        }
    }

    if (m_nFlags.bDisabled) {
        if (CTimer::GetTimeInMS() > m_nRegenerationTime) {
            // Como el original: distancia 2D al cuadrado con FindPlayerCoors llamado cuatro veces
            const auto posn = GetPosn();
            const auto c1 = FindPlayerCoors();
            const auto c2 = FindPlayerCoors();
            const auto c3 = FindPlayerCoors();
            const float ty = c3.y - posn.y;
            const auto c4 = FindPlayerCoors();
            const float distSq = (c4.y - posn.y) * ty + (c2.x - posn.x) * (c1.x - posn.x);
            if (distSq > 100.0f || (pt == PICKUP_IN_SHOP && distSq > 2.4f)) {
                GiveUsAPickUpObject(m_pObject);

                if (m_pObject) {
                    CWorld::Add(m_pObject);
                    m_nFlags.bDisabled = false;
                }
            }
        }

        return false;
    }

    if (!m_pObject) {
        GiveUsAPickUpObject(m_pObject);
        if (m_pObject) {
            CWorld::Add(m_pObject);
        }
    }

    if (!m_pObject) {
        return false;
    }

    // Como el original: el agua se busca 5 m por encima y se escribe directamente en la matriz
    const auto PlaceOnWater = [this](bool withOffset) {
        auto& m = *m_pObject->m_matrix;
        float level;
        if (CWaterLevel::GetWaterLevel(m.GetPosition().x, m.GetPosition().y, m.GetPosition().z + 5.0f, level, true, nullptr)) {
            if (withOffset) {
                m.GetPosition().z = level + 0.6f;
            } else {
                m.GetPosition().z = level;
            }
        }
        m_pObject->UpdateRwMatrix();
        m_pObject->UpdateRwFrame();
    };

    // Como el original: recorre el pool de vehículos del último hueco al primero
    const auto AnyVehicleTouching = [this](float radius) {
        bool touching = false;
        auto* pool = GetVehiclePool();
        for (auto i = (int32)pool->GetSize(); i-- > 0;) {
            if (auto* veh = pool->GetAt(i)) {
                if (veh->IsSphereTouchingVehicle(m_pObject->GetPosition(), radius)) {
                    touching = true;
                }
            }
        }
        return touching;
    };

    switch (pt) {
    case PICKUP_MINE_INACTIVE:
        if (vehicle && !vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition(), 2.0f)) {
            pt = PICKUP_MINE_ARMED;
            m_nRegenerationTime = CTimer::GetTimeInMS() + 10'000;
        }
        break;

    case PICKUP_NAUTICAL_MINE_INACTIVE: {
        PlaceOnWater(true);
        if (!AnyVehicleTouching(2.0f)) {
            pt = PICKUP_NAUTICAL_MINE_ARMED;
            m_nRegenerationTime = CTimer::GetTimeInMS() + 10'000;
        }
        break;
    }
    case PICKUP_NAUTICAL_MINE_ARMED:
        PlaceOnWater(true);
        [[fallthrough]];

    case PICKUP_MINE_ARMED: {
        bool explode = CTimer::GetTimeInMS() > m_nRegenerationTime;
        if (AnyVehicleTouching(1.5f)) {
            explode = true;
        }

        if (explode) {
            CExplosion::AddExplosion(nullptr, nullptr, EXPLOSION_MINE, m_pObject->GetPosition(), 0u, 1u, -1.0f, false);
            Remove();
        }
        break;
    }
    case PICKUP_FLOATINGPACKAGE: {
        // Como el original: la gravedad del paquete es paso * 0.01
        m_pObject->GetMoveSpeed().z -= CTimer::GetTimeStep() * 0.01f;
        m_pObject->SetPosn(m_pObject->GetPosition() + m_pObject->GetMoveSpeed() * CTimer::GetTimeStep());
        m_pObject->UpdateRwMatrix();
        m_pObject->UpdateRwFrame();

        auto& m = *m_pObject->m_matrix;
        float level;
        if (CWaterLevel::GetWaterLevel(m.GetPosition().x, m.GetPosition().y, m.GetPosition().z + 5.0f, level, true, nullptr) && level >= m.GetPosition().z) {
            pt = PICKUP_FLOATINGPACKAGE_FLOATING;
        }
        break;
    }
    case PICKUP_FLOATINGPACKAGE_FLOATING:
        PlaceOnWater(false);

        if (vehicle && vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition(), 2.0f)) {
            Remove();
            isRemoved = true;
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_DRUGS);
        }
        break;
    default: {
        const auto mi = m_pObject->m_nModelIndex;

        // Como el original: |dz| < 2 y distancia 2D al cuadrado < 1.8 (sin raíz)
        const auto IsCloseEnough = [&] {
            const auto& pp = player->GetPosition();
            const auto& op = m_pObject->GetPosition();
            if (!(std::fabs(pp.z - op.z) < 2.0f)) {
                return false;
            }
            const float dy = pp.y - op.y;
            const float dx = pp.x - op.x;
            return dy * dy + dx * dx < 1.8f;
        };

        bool isPicked = false;
        if (mi == MI_PICKUP_BRIBE) {
            // Como el original: dentro de un vehículo solo cuenta el contacto con la esfera
            if (vehicle) {
                isPicked = vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition(), 2.0f);
            } else {
                isPicked = IsCloseEnough();
            }
        } else if (mi == MI_PICKUP_CAMERA) {
            isPicked = false;
        } else {
            // these can not be picked on missions
            const bool nonMissionPickup = mi == MI_PICKUP_SAVEGAME || mi == MI_PICKUP_2P_KILLFRENZY || mi == MI_PICKUP_2P_COOP;
            if ((!nonMissionPickup || player->CanPlayerStartMission()) && !vehicle && player->IsAlive()) {
                isPicked = IsCloseEnough();
            }
        }

        if (m_pObject->objectFlags.bDoNotRender) {
            isPicked = false;
        }

        if (player->GetPadFromPlayer() && player->GetPadFromPlayer()->ArePlayerControlsDisabled() && m_pObject->m_nModelIndex == MODEL_JETPACK) {
            break;
        }
        if (!isPicked) {
            break;
        }

        // Comprobaciones de armas. Devuelve false para terminar sin recoger.
        const auto CheckWeapon = [&]() -> bool {
            switch (pt) {
            case PICKUP_COLLECTABLE1:
            case PICKUP_MONEY:
            case PICKUP_MINE_INACTIVE:
            case PICKUP_MINE_ARMED:
            case PICKUP_NAUTICAL_MINE_INACTIVE:
            case PICKUP_FLOATINGPACKAGE:
            case PICKUP_FLOATINGPACKAGE_FLOATING:
            case PICKUP_ASSET_REVENUE:
            case PICKUP_PROPERTY_LOCKED:
            case PICKUP_PROPERTY_FORSALE:
            case PICKUP_MONEY_DOESNTDISAPPEAR:
                return true;
            default:
                break;
            }

            const auto weapon = CPickups::WeaponForModel(m_pObject->m_nModelIndex);
            if ((int32)weapon < 0 || (int32)weapon >= 47) {
                return true;
            }
            if (CDarkel::FrenzyOnGoing()) {
                m_nFlags.bHelpMessageDisplayed = false;
                return false;
            }
            if (weapon == WEAPON_UNARMED) {
                return true;
            }

            const auto slot = CWeaponInfo::GetWeaponInfo(weapon)->m_nSlot;
            auto& pedWeapon = player->GetWeaponInSlot((size_t)slot);
            if (pedWeapon.m_Type == weapon) {
                return true;
            }
            if (!CStreaming::GetInfo(m_pObject->m_nModelIndex).IsLoaded()) {
                return false;
            }

            const auto IsSlotExchangeable = [](int32 s) { // 0x454920
                return s == 3 || s == 4 || s == 5;
            };
            const auto lastLOSTime = *reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(player->GetPlayerData()) + 0x6C);

            if (pedWeapon.m_Type != WEAPON_UNARMED && (pedWeapon.m_TotalAmmo != 0 || IsSlotExchangeable(slot))) {
                CPickups::PlayerOnWeaponPickup = 6;

                if (IsSlotExchangeable(slot)) {
                    switch (pt) {
                    case PICKUP_ONCE_TIMEOUT:
                    case PICKUP_ONCE:
                    case PICKUP_ON_STREET:
                    case PICKUP_ONCE_FOR_MISSION:
                        ExtractAmmoFromPickup(player);
                        pedWeapon.Reload();
                        break;
                    default:
                        break;
                    }
                }

                if (!CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment(weapon)) {
                    return false;
                }

                if (!CPad::GetPad(0)->GetTarget() && CPickups::DisplayHelpMessage && !m_nFlags.bHelpMessageDisplayed) {
                    switch (CPad::GetPad(0)->Mode) {
                    case 0:
                    case 1:
                        CHud::SetHelpMessage(TheText.Get("PU_CF1"), false, false, false);
                        break;
                    case 2:
                        CHud::SetHelpMessage(TheText.Get("PU_CF3"), false, false, false);
                        break;
                    case 3:
                        CHud::SetHelpMessage(TheText.Get("PU_CF4"), false, false, false);
                        break;
                    default:
                        break;
                    }

                    if (CPickups::DisplayHelpMessage) {
                        CPickups::DisplayHelpMessage--;
                    }
                    m_nFlags.bHelpMessageDisplayed = true;
                }

                if (!CollectPickupBuffer || CTimer::GetTimeInMS() - lastLOSTime < 1500) {
                    return false;
                }
            } else if (CTimer::GetTimeInMS() - lastLOSTime < 1500 || !CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment(weapon)) {
                CPickups::PlayerOnWeaponPickup = 6;
                return false;
            }
            return true;
        };
        if (!CheckWeapon()) {
            break;
        }

        // Como el original: condiciones para poder recoger
        if (mi == MI_PICKUP_BODYARMOUR && (float)CWorld::Players[playerId].m_nMaxArmour - 0.2f < player->m_fArmour)
            break;
        if (mi == MI_PICKUP_HEALTH && (float)CWorld::Players[playerId].m_nMaxHealth - 0.2f < player->m_fHealth)
            break;
        if (mi == MI_PICKUP_BRIBE) {
            // Como el original: el nivel de búsqueda es siempre el del jugador 0
            const auto* wanted = FindPlayerPed(0)->GetPlayerData()->m_pWanted;
            if (!*reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(wanted) + 0x2C))
                break;
        }
        if (mi == MI_PICKUP_KILLFRENZY && (CTheScripts::IsPlayerOnAMission() || CDarkel::FrenzyOnGoing() || !CLocalisation::KillFrenzy()))
            break;
        if (m_pObject->m_nModelIndex == MODEL_JETPACK && player->GetIntelligence()->GetTaskJetPack())
            break;
        if (pt == PICKUP_ASSET_REVENUE && m_fRevenueValue < 10.0f)
            break;
        if (pt == PICKUP_2P && CTheScripts::IsPlayerOnAMission())
            break;

        if (mi != MI_PICKUP_PROPERTY && mi != MI_PICKUP_PROPERTY_FORSALE && pt != PICKUP_2P) {
            CPad::GetPad(0)->StartShake(120, 100u, 0);
        }

        const auto weapon = CPickups::WeaponForModel(m_pObject->m_nModelIndex);
        const auto GiveWeapon = [&] {
            if (m_nAmmo) {
                player->GiveDelayedWeapon(weapon, m_nAmmo);
            } else {
                player->GiveDelayedWeapon(weapon, m_nFlags.bEmpty ? 0 : AmmoForWeapon_OnStreet[weapon]);
            }

            if (auto& chosen = player->GetPlayerData()->m_nChosenWeapon; (int32)(int8)chosen == (int32)player->GetWeaponSlot(WEAPON_UNARMED)) {
                player->GetPlayerData()->m_nChosenWeapon = (uint8)player->GetWeaponSlot(weapon);
            }
        };
        switch (pt) {
        case PICKUP_ON_STREET:
        case PICKUP_ON_STREET_SLOW: {
            uint32 regenTime = 0;
            bool useCamTime = false;
            if (!CPickups::GivePlayerGoodiesWithPickUpMI(mi, playerId)) {
                if (!player->DoesPlayerWantNewWeapon(weapon, false)) {
                    break;
                }

                if (weapon != WEAPON_UNARMED) {
                    GiveWeapon();
                    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_WEAPON);
                } else if (mi == MI_PICKUP_CAMERA && vehicle) {
                    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
                    CPickups::bPickUpcamActivated = true;
                    CPickups::pPlayerVehicle = FindPlayerVehicle();
                    // Como el original: se copian las coordenadas (no un puntero)
                    *CPickups::StaticCamCoors = m_pObject->GetPosition();
                    CPickups::StaticCamStartTime = CTimer::GetTimeInMS();
                    regenTime = CPickups::StaticCamStartTime;
                    useCamTime = true;
                }
            }
            if (!useCamTime) {
                regenTime = CTimer::GetTimeInMS();
            }
            switch (pt) {
            case PICKUP_ON_STREET:
                m_nRegenerationTime = regenTime + 30'000;
                break;
            case PICKUP_ON_STREET_SLOW:
                // Como el original: 300000 (soborno) o 360000 ms
                m_nRegenerationTime = regenTime + ((mi == MI_PICKUP_BRIBE) ? 300'000 : 360'000);
                break;
            default:
                break;
            }
            isRemoved = true;
            GetRidOfObjects();
            m_nFlags.bDisabled = true;
            break;
        }
        case PICKUP_ONCE:
        case PICKUP_ONCE_TIMEOUT:
        case PICKUP_ONCE_TIMEOUT_SLOW:
        case PICKUP_ONCE_FOR_MISSION:
            if (!CPickups::GivePlayerGoodiesWithPickUpMI(mi, playerId)) {
                if (!player->DoesPlayerWantNewWeapon(weapon, false)) {
                    ExtractAmmoFromPickup(player);
                    break;
                }

                if (weapon != WEAPON_UNARMED) {
                    GiveWeapon();
                }

                if (mi != MI_PICKUP_SAVEGAME) {
                    CHud::SetHelpMessage(nullptr, true, false, false);
                    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_WEAPON);
                }
            }
            isRemoved = true;
            Remove();
            break;
        case PICKUP_MONEY:
        case PICKUP_MONEY_DOESNTDISAPPEAR:
            // Como el original: siempre el jugador 0; primero se quita el pickup y después suenan audio y voz
            CWorld::Players[0].m_nMoney += m_nAmmo;
            isRemoved = true;
            Remove();
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_MONEY);
            player->Say(CTX_GLOBAL_PICKUP_CASH);
            break;
        case PICKUP_ASSET_REVENUE: {
            // Como el original: (float)dinero + valor, truncado con _ftol2
            auto& money = CWorld::Players[playerId].m_nMoney;
            money = (int32)(int64)((float)money + m_fRevenueValue);
            m_fRevenueValue = 0.0f;
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_MONEY);
            break;
        }
        case PICKUP_PROPERTY_LOCKED:
            if (!m_nFlags.bHelpMessageDisplayed) {
                m_nFlags.bHelpMessageDisplayed = true;
                auto text = TheText.Get(FindStringForTextIndex((ePickupPropertyText)m_nFlags.nPropertyTextIndex));
                CHud::SetHelpMessage(text, false, false, false);
            }
            break;
        case PICKUP_PROPERTY_FORSALE: {
            char labelKey[16];
            strcpy(labelKey, FindStringForTextIndex((ePickupPropertyText)m_nFlags.nPropertyTextIndex));
            ModifyStringLabelForControlSetting(labelKey);
            CMessages::InsertNumberInString(TheText.Get(labelKey), m_nAmmo, 0, 0, 0, 0, 0, gGxtString);

            if (!CHud::HelpMessageDisplayed()) {
                CHud::SetHelpMessage(gGxtString, false, false, false);
            }
            if (CollectPickupBuffer) {
                if (CTheScripts::IsPlayerOnAMission()) {
                    CHud::SetHelpMessage(TheText.Get("PROP_2"), true, false, false);
                } else {
                    // Como el original: con dinero suficiente se compra la propiedad y se quita el pickup
                    auto& money = CWorld::Players[CWorld::PlayerInFocus].m_nMoney;
                    if (money >= (int32)m_nAmmo) {
                        money -= (int32)m_nAmmo;
                        CHud::SetHelpMessage(nullptr, true, false, false);
                        isRemoved = true;
                        Remove();
                    } else {
                        CHud::SetHelpMessage(TheText.Get("PROP_1"), true, false, false);
                    }
                }
            }
            break;
        }
        case PICKUP_2P:
            if (CPad::GetPad(1)->GetTouchedTimeDelta() >= 500) {
                // Como el original: el mensaje se marca como mostrado
                if (!m_nFlags.bHelpMessageDisplayed) {
                    m_nFlags.bHelpMessageDisplayed = true;
                    CHud::SetHelpMessage(TheText.Get("PU_2PLN"), true, false, true);
                }
            } else {
                CHud::m_Wants_To_Draw_Hud = true;
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_MONEY);
                isRemoved = true;
                Remove();
                CHud::SetHelpMessage(nullptr, true, false, false);
                CGameLogic::vec2PlayerStartLocation = FindPlayerCoors();
                CGameLogic::f2PlayerStartHeading = FindPlayerPed()->GetHeading();
            }
            break;
        default:
            break;
        }
        break;
    }
    }

    // Como el original: el pickup caducado se quita sin marcarlo como recogido
    if (!m_nFlags.bDisabled) {
        switch (pt) {
        case PICKUP_ONCE_TIMEOUT:
        case PICKUP_ONCE_TIMEOUT_SLOW:
        case PICKUP_MONEY:
            if (CTimer::GetTimeInMS() > m_nRegenerationTime) {
                Remove();
            }
            break;
        default:
            break;
        }
    }
    return isRemoved;
}
