/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCullZones
 * Reconstrucción: COMPLETA — 21/21 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 21/21 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "CullZones.h"
#include "X87Math.h"

void CCullZones::InjectHooks() {
    RH_ScopedClass(CCullZones);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x72D6B0);
    RH_ScopedInstall(AddCullZone, 0x72DF70);
    RH_ScopedInstall(AddTunnelAttributeZone, 0x72DB50);
    RH_ScopedInstall(AddMirrorAttributeZone, 0x72DC10);
    RH_ScopedInstall(InRoomForAudio, 0x72DD70);
    RH_ScopedInstall(FewerCars, 0x72DD80);
    RH_ScopedInstall(CamNoRain, 0x72DDB0);
    RH_ScopedInstall(PlayerNoRain, 0x72DDC0);
    RH_ScopedInstall(FewerPeds, 0x72DD90);
    RH_ScopedInstall(CamCloseInForPlayer, 0x72DD20);
    RH_ScopedInstall(CamStairsForPlayer, 0x72DD30);
    RH_ScopedInstall(Cam1stPersonForPlayer, 0x72DD40);
    RH_ScopedInstall(NoPolice, 0x72DD50);
    RH_ScopedInstall(PoliceAbandonCars, 0x72DD60);
    RH_ScopedInstall(DoExtraAirResistanceForPlayer, 0x72DDD0);
    RH_ScopedInstall(DoINeedToLoadCollision, 0x72DDA0);
    RH_ScopedInstall(FindTunnelAttributesForCoors, 0x72D9F0);
    RH_ScopedInstall(FindMirrorAttributesForCoors, 0x72DA70);
    RH_ScopedInstall(FindZoneWithStairsAttributeForPlayer, 0x72DAD0);
    RH_ScopedInstall(FindAttributesForCoors, 0x72D970);
    RH_ScopedInstall(Update, 0x72DEC0);

    //RH_ScopedGlobalInstall(IsPointWithinArbitraryArea, 0x72DDE0);

    {
        RH_ScopedClass(CZoneDef);
        RH_ScopedCategoryGlobal();

        RH_ScopedInstall(GetCenter, 0x72D6D0);
    }
}

// 0x72D6D0
// Como el original: fild/fiadd exactos (int16), fmul por 0.5f (0x858B8C) y fstp dword en el orden x, y, z; el aritmetico esta en X87Math.cpp. Devuelve out (EAX);
// el unico llamador del EXE (CCamera::CamControl, 0x529312) sobrescribe ECX y EDX antes de leerlos, asi que no hay residuo que reproducir.
CVector* CZoneDef::GetCenter(CVector& out) const {
    CullZoneCenterCpp(&m_cornerX, &out.x);
    return &out;
}

// 0x72D6B0
void CCullZones::Init() {
    ZoneScoped;

    NumAttributeZones = 0;
    CurrentFlags_Player = 0;
    CurrentFlags_Camera = 0;
}

// flags: see eZoneAttributes
// 0x72DF70
void CCullZones::AddCullZone(const CVector& pos, float vec1X, float vec1Y, float minZ, float vec2X, float vec2Y, float maxZ, uint16 flags) {
    if (flags & eZoneAttributes::TUNNEL_TRANSITION || flags & eZoneAttributes::TUNNEL) {
        AddTunnelAttributeZone(pos, vec1X, vec1Y, minZ, vec2X, vec2Y, maxZ, (eZoneAttributes)flags);
        flags &= ~eZoneAttributes::TUNNEL_TRANSITION;
        flags &= ~eZoneAttributes::TUNNEL;
    }

    // Will be computed if flags less than 0x880 (TUNNEL_TRANSITION + TUNNEL)
    if (flags) {
        CAttributeZone& zone = aAttributeZones[NumAttributeZones];
        zone.zoneDef.Init(pos, vec1X, vec1Y, minZ, vec2X, vec2Y, maxZ);
        zone.flags = (eZoneAttributes)flags;

        NumAttributeZones++;
    }
}

// flags: see eZoneAttributes
// 0x72DB50
void CCullZones::AddTunnelAttributeZone(const CVector& pos, float vec1X, float vec1Y, float minZ, float vec2X, float vec2Y, float maxZ, eZoneAttributes flags) {
    CAttributeZone& zone = aTunnelAttributeZones.data()[NumTunnelAttributeZones];

    zone.zoneDef.Init(pos, vec1X, vec1Y, minZ, vec2X, vec2Y, maxZ);
    zone.flags = flags;

    NumTunnelAttributeZones++;
}

// 0x72DC10
void CCullZones::AddMirrorAttributeZone(const CVector& pos, float vec1X, float vec1Y, float minZ, float vec2X, float vec2Y, float maxZ, eZoneAttributes flags, float mirrorV, float mirrorNormalX, float mirrorNormalY, float mirrorNormalZ) {
    CMirrorAttributeZone& zone = aMirrorAttributeZones.data()[NumMirrorAttributeZones];

    zone.zoneDef.Init(pos, vec1X, vec1Y, minZ, vec2X, vec2Y, maxZ);
    zone.flags = (eMirrorFlags)flags;
    zone.mirrorV = mirrorV;
    // Como el original: _ftol2 (64 bits) y se guarda el byte bajo
    zone.mirrorNormalX = (int8)(int64)(mirrorNormalX * 100.0f);
    zone.mirrorNormalY = (int8)(int64)(mirrorNormalY * 100.0f);
    zone.mirrorNormalZ = (int8)(int64)(mirrorNormalZ * 100.0f);

    NumMirrorAttributeZones++;
}

// 0x72DD20
bool CCullZones::CamCloseInForPlayer() {
    return CurrentFlags_Player & eZoneAttributes::CAM_CLOSE_IN;
}

// 0x72DD30
bool CCullZones::CamStairsForPlayer() {
    return CurrentFlags_Player & eZoneAttributes::STAIRS;
}

// 0x72DD40
bool CCullZones::Cam1stPersonForPlayer() {
    return CurrentFlags_Player & eZoneAttributes::STPERSON_1ST;
}

// 0x72DD50
bool CCullZones::NoPolice() {
    return CurrentFlags_Player & eZoneAttributes::NO_POLICE;
}

// 0x72DD60
bool CCullZones::PoliceAbandonCars() {
    return CurrentFlags_Player & eZoneAttributes::POLICE_ABANDON_CARS;
}

// 0x72DD70
bool CCullZones::InRoomForAudio() {
    return CurrentFlags_Camera & eZoneAttributes::IN_ROOM_FOR_AUDIO;
}

// 0x72DD80
bool CCullZones::FewerCars() {
    return CurrentFlags_Player & eZoneAttributes::FEWER_CARS;
}

// 0x72DD90
bool CCullZones::FewerPeds() {
    return CurrentFlags_Player & eZoneAttributes::FEWER_PEDS;
}

// 0x72DDB0
bool CCullZones::CamNoRain() {
    return CurrentFlags_Camera & eZoneAttributes::NO_RAIN;
}

// 0x72DDC0
bool CCullZones::PlayerNoRain() {
    return CurrentFlags_Player & eZoneAttributes::NO_RAIN;
}

// 0x72DDD0
bool CCullZones::DoExtraAirResistanceForPlayer() {
    return CurrentFlags_Player & eZoneAttributes::EXTRA_AIR_RESISTANCE;
}

// 0x72DDA0
bool CCullZones::DoINeedToLoadCollision() {
    return CurrentFlags_Player & eZoneAttributes::LOAD_COLLISION;
}

// 0x72D9F0
// Como el original: solo recorre las NumTunnelAttributeZones primeras (comparacion con signo) y combina los
// flags extendidos con signo.
int32 CCullZones::FindTunnelAttributesForCoors(CVector pos) {
    int32 flags = eZoneAttributes::ZA_NONE;
    const int32 count = NumTunnelAttributeZones;
    for (int32 i = 0; i < count; i++) {
        const auto& zone = aTunnelAttributeZones.data()[i];
        if (zone.IsPointWithin(pos)) {
            flags |= (int16)zone.flags;
        }
    }

    return flags;
}

// 0x72DA70
// Como el original: solo recorre las NumMirrorAttributeZones primeras.
CMirrorAttributeZone* CCullZones::FindMirrorAttributesForCoors(CVector pos) {
    const int32 count = NumMirrorAttributeZones;
    for (int32 i = 0; i < count; i++) {
        auto& zone = aMirrorAttributeZones.data()[i];
        if (zone.IsPointWithin(pos)) {
            return &zone;
        }
    }

    return nullptr;
}

// 0x72DAD0
// Como el original: la posicion del jugador se pide una sola vez, antes del bucle, y el contador se relee
// en cada vuelta.
CAttributeZone* CCullZones::FindZoneWithStairsAttributeForPlayer() {
    const CVector playerPos = FindPlayerCoors();
    for (int32 i = 0; i < NumAttributeZones; i++) {
        auto& zone = aAttributeZones[i];
        if ((zone.flags & eZoneAttributes::STAIRS) && zone.IsPointWithin(playerPos)) {
            return &zone;
        }
    }

    return nullptr;
}

// 0x72D970
// Como el original: solo recorre las NumAttributeZones primeras (comparacion con signo) y combina los
// flags extendidos con signo (movsx), asi que 0x8000 deja los 16 bits altos a 1.
int32 CCullZones::FindAttributesForCoors(CVector pos) {
    int32 flags = eZoneAttributes::ZA_NONE;
    const int32 count = NumAttributeZones;
    for (int32 i = 0; i < count; i++) {
        const auto& zone = aAttributeZones[i];
        if (zone.IsPointWithin(pos)) {
            flags |= (int16)zone.flags;
        }
    }

    return flags;
}

// 0x72DEC0
void CCullZones::Update() {
    ZoneScoped;

    if ((CTimer::GetFrameCounter() & 7) == 2) {
        CurrentFlags_Camera = FindAttributesForCoors(*TheCamera.GetGameCamPosition());
        return;
    }

    if ((CTimer::GetFrameCounter() & 7) == 6) {
        CurrentFlags_Player = FindAttributesForCoors(FindPlayerCoors());
        if (!bMilitaryZonesDisabled && (CurrentFlags_Player & eZoneAttributes::MILITARY_ZONE) != 0) {
            // Como el original: FindPlayerPed se llama otra vez para subir el nivel de busqueda
            if (FindPlayerPed()->IsAlive()) {
                FindPlayerPed()->SetWantedLevelNoDrop(eWantedLevel::WANTED_LEVEL_5);
            }
        }
    }
}

// unused
// 0x72DDE0
bool IsPointWithinArbitraryArea(CVector2D p, CVector2D p1, CVector2D p2, CVector2D p3, CVector2D p4) {
    if ((p.x - p1.x) * (p2.y - p1.y) - (p.y - p1.y) * (p2.x - p1.x) < 0.0f) {
        return false;
    }
    if ((p.x - p2.x) * (p3.y - p2.y) - (p.y - p2.y) * (p3.x - p2.x) < 0.0f) {
        return false;
    }
    if ((p.x - p3.x) * (p4.y - p3.y) - (p.y - p3.y) * (p4.x - p3.x) < 0.0f) {
        return false;
    }
    if ((p.x - p4.x) * (p1.y - p4.y) - (p.y - p4.y) * (p1.x - p4.x) < 0.0f) {
        return false;
    }
    return true;
}
