#pragma once
#include "DatosExe/DataBssE2.h"
#include "DatosExe/DataBssE1.h"

#include "Vector.h"

// https://gtamods.com/wiki/CULL

enum eZoneAttributes : uint16 {
    ZA_NONE = 0x0,

    // Camera close in into player using closest third-person view camera mode,
    // does not close in if in first person or cinematic mode,
    // camera mode cannot be changed while in the zone
    CAM_CLOSE_IN = 0x1,

    // Camera remotely placed outside the zone, no control of camera,
    // camera mode cannot be changed while in the zone
    STAIRS = 0x2,

    // Lowers the camera angle on boats
    STPERSON_1ST = 0x4,

    // Rain-free, police helicopter-free zone
    NO_RAIN = 0x8,

    // Police will not exit their vehicles voluntarily.
    // They will only exit if you do something to them (like shoot it).
    // Cops both on foot and in vehicles will not chase you but can shoot at you.
    NO_POLICE = 0x10,

    LOAD_COLLISION = 0x40,

    TUNNEL_TRANSITION = 0x80,

    // Police will always exit their vehicles
    // once they are spawned ONLY IF you have a wanted level.
    // If you don't, they'll drive normally.
    POLICE_ABANDON_CARS = 0x100,

    IN_ROOM_FOR_AUDIO = 0x200,

    // Some visual ocean water effects are removed
    // like the transparent waves and sparkles on the water.
    FEWER_PEDS = 0x400,

    TUNNEL = 0x800,

    // 5-Star Military zone.
    MILITARY_ZONE = 0x1000,

    // Doesn't allow cars to reach top speed.
    EXTRA_AIR_RESISTANCE = 0x4000,

    // Spawn fewer cars in this area.
    FEWER_CARS = 0x8000
};

enum eMirrorFlags : uint8 {
    MF_NONE = 0x0,

    MF_MIRROR = 0x1,

    MF_SCREENS_1 = 0x2,

    MF_SCREENS_2 = 0x4,

    MF_SCREENS_3 = 0x8,

    MF_SCREENS_4 = 0x10,

    MF_SCREENS_5 = 0x20,

    MF_SCREENS_6 = 0x40
};

struct CZoneDef {
    int16 m_cornerX, m_cornerY;
    int16 m_vec1X, m_vec1Y;
    int16 m_vec2X, m_vec2Y;
    int16 m_minZ, m_maxZ;

    // Como el original: cada valor se trunca con _ftol2 (64 bits) y se guarda la palabra baja
    void Init(
        const CVector& pos,
        float vec1X, float vec1Y, float minZ,
        float vec2X, float vec2Y, float maxZ
    ) {
        m_cornerX = (int16)(int64)(pos.x - vec1X - vec2X);
        m_cornerY = (int16)(int64)(pos.y - vec1Y - vec2Y);

        m_vec1X = (int16)(int64)(vec1X + vec1X);
        m_vec1Y = (int16)(int64)(vec1Y + vec1Y);

        m_minZ = (int16)(int64)(minZ);

        m_vec2X = (int16)(int64)(vec2X + vec2X);
        m_vec2Y = (int16)(int64)(vec2Y + vec2Y);

        m_maxZ = (int16)(int64)(maxZ);
    }

    // 0x72D850 (lote 208A, sin validar; thiscall con el CVector POR VALOR en la pila, `ret 0Ch`; antes en linea con const&): el cuerpo esta en CullZones.cpp
    [[nodiscard]] bool IsPointWithin(CVector point) const;

    // 0x72D6D0
    // Como el original: centro de la zona; x = (vec2X + vec1X) * 0.5 + esquinaX, y = (vec2Y + vec1Y) * 0.5 + esquinaY, z = (minZ + maxZ) * 0.5. Devuelve out (EAX).
    CVector* GetCenter(CVector& out) const;
};

VALIDATE_SIZE(CZoneDef, 0x10);

// https://gtaforums.com/topic/202532-sadoc-ipl-definitions/page/5/?tab=comments#comment-3875562
struct CMirrorAttributeZone {
    CZoneDef zoneDef;
    float mirrorV; // mirror plane coordinate in direction axis.
    int8 mirrorNormalX, mirrorNormalY, mirrorNormalZ; // mirror direction (like [-1 0 0] or [0 1 0] or [0 0 -1])
    eMirrorFlags flags;

    [[nodiscard]] bool IsPointWithin(const CVector& point) const { return zoneDef.IsPointWithin(point); }
};

VALIDATE_SIZE(CMirrorAttributeZone, 0x18);

struct CAttributeZone {
    CZoneDef zoneDef;
    eZoneAttributes flags;

    [[nodiscard]] bool IsPointWithin(const CVector& point) const { return zoneDef.IsPointWithin(point); }
};

VALIDATE_SIZE(CAttributeZone, 0x12);

// CULL is a section in the item placement file.
// It is used to create zones affecting the different behaviour of the world, like
// mirrors, fixed camera positions, disabling rain, police behaviors, etc.,
// when the player is within the zone.
class CCullZones {
public:
    static inline auto& NumAttributeZones = DatosExe::Bss::bC87AC8;
    static inline auto& NumMirrorAttributeZones = DatosExe::Bss::bC87AC4;
    static inline auto& NumTunnelAttributeZones = DatosExe::Bss::bC87AC0;

    static inline auto& CurrentFlags_Camera = DatosExe::Bss::bC87ABC;
    static inline auto& CurrentFlags_Player = DatosExe::Bss::bC87AB8;

    static inline auto& bMilitaryZonesDisabled = StaticRef<bool>(0xC87ACD);

    static inline auto& aAttributeZones = StaticRef<CAttributeZone[1'300]>(0xC81F50);
    static inline auto& aTunnelAttributeZones = StaticRef<std::array<CAttributeZone, 40>>(0xC81C80);
    static inline auto& aMirrorAttributeZones = StaticRef<std::array<CMirrorAttributeZone, 72>>(0xC815C0);

    static inline auto& bRenderCullzones = DatosExe::Bss::bC87ACC; // unused

public:
    static void InjectHooks();

    static void Init();
    static void Update();

    // Como el original: devuelven el OR de los flags extendidos con signo (movsx) como entero de 32 bits
    static int32 FindAttributesForCoors(CVector pos);
    static int32 FindTunnelAttributesForCoors(CVector pos);
    static CMirrorAttributeZone* FindMirrorAttributesForCoors(CVector pos);
    static CAttributeZone* FindZoneWithStairsAttributeForPlayer();
    static void AddCullZone(const CVector& pos, float vec1X, float vec1Y, float minZ, float vec2X, float vec2Y, float maxZ, uint16 flags);
    static void AddMirrorAttributeZone(const CVector& pos, float vec1X, float vec1Y, float minZ, float vec2X, float vec2Y, float maxZ, eZoneAttributes flags, float mirrorV, float mirrorNormalX, float mirrorNormalY, float mirrorNormalZ);
    static void AddTunnelAttributeZone(const CVector& pos, float vec1X, float vec1Y, float minZ, float vec2X, float vec2Y, float maxZ, eZoneAttributes flags);

    static bool CamCloseInForPlayer();
    static bool CamStairsForPlayer();
    static bool Cam1stPersonForPlayer();
    static bool NoPolice();
    static bool PoliceAbandonCars();
    static bool InRoomForAudio();
    static bool FewerCars();
    static bool FewerPeds();
    static bool CamNoRain();
    static bool PlayerNoRain();
    static bool DoINeedToLoadCollision();

    static bool DoExtraAirResistanceForPlayer();
};
