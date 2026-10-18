/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTheZones
 * Reconstrucción: COMPLETA — 22/22 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 22/22 entradas.
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

#include <intrin.h>

#include "TheZones.h"
#include "World.h"
#include "X87Math.h"

#include <Extensions/ci_string.hpp>


void CTheZones::InjectHooks() {
    RH_ScopedClass(CTheZones);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(ResetZonesRevealed, 0x572110);
    RH_ScopedGlobalInstall(SetCurrentZoneAsUnlocked, 0x572800);
    RH_ScopedGlobalInstall(PointLiesWithinZone, 0x572270);
    RH_ScopedGlobalInstall(GetNavigationZone, 0x572590);
    RH_ScopedGlobalInstall(GetMapZone, 0x5725A0);
    RH_ScopedGlobalInstall(Save, 0x5D2E60);
    RH_ScopedGlobalInstall(Load, 0x5D2F40);
    RH_ScopedGlobalInstall(PostZoneCreation, 0x572B70);
    RH_ScopedGlobalInstall(InitZonesPopulationSettings, 0x5720D0);
    RH_ScopedGlobalInstall(Update, 0x572D10);
    RH_ScopedGlobalInstall(SetZoneRadarColours, 0x572CC0);
    RH_ScopedGlobalInstall(FindZoneByLabel, 0x572C40);
    RH_ScopedGlobalInstall(CreateZone, 0x5728A0);
    RH_ScopedGlobalInstall(Init, 0x572670);
    RH_ScopedGlobalInstall(AssignZoneInfoForThisZone, 0x572180);
    RH_ScopedGlobalOverloadedInstall(FindZone, "", 0x572B80, bool(*)(CVector*, uint64_t, eZoneType));
    RH_ScopedGlobalInstall(Calc2DDistanceBetween2Zones, 0x5725B0);
    RH_ScopedGlobalInstall(FillZonesWithGangColours, 0x572440);
    RH_ScopedGlobalOverloadedInstall(GetZoneInfo, "", 0x572400, CZoneInfo*(*)(const CVector& point, CZone**));
    RH_ScopedGlobalInstall(FindSmallestZoneForPosition, 0x572360);
    RH_ScopedGlobalInstall(GetLevelFromPosition, 0x572300);
    RH_ScopedGlobalInstall(ZoneIsEntirelyContainedWithinOtherZone, 0x572220);

}

// 0x5720D0
// Como el original: no toca ZoneColor ni los 4 bits altos del byte de razas.
void CTheZones::InitZonesPopulationSettings() {
    for (auto& info : ZoneInfoArray) {
        auto* b = reinterpret_cast<uint8*>(&info);
        b[0xF] = (b[0xF] & 0xE5) | 5;
        b[0x10] |= 0x0F;
        b[0xA] = 0;
        std::memset(b, 0, 10);
        b[0xF] &= 0x1F;
    }
}

// 0x572110
void CTheZones::ResetZonesRevealed() {
    std::memset(&ZonesVisited, 0, sizeof(ZonesVisited));
    ZonesRevealed = 0;
}

// NOTSA
bool& CTheZones::GetZoneWasVisited(CVector2D pos) {
#ifdef FIX_BUGS
    return ZonesVisited[(size_t)((std::clamp(pos.x, -2999.f, 2999.f) + 3000.f) / 600.f)][9LL - (size_t)((std::clamp(pos.y, -2999.f, 2999.f) + 3000.f) / 600.f)];
#else
    return ZonesVisited[10 * (size_t)((pos.x + 3000.f) / 600.f) - (size_t)((pos.y + 3000.f) / 600.f) + 9];
#endif
}

// NOTSA
bool CTheZones::SetZoneWasVisited(CVector2D pos, bool visited) {
    bool& wasVisited = GetZoneWasVisited(pos);
    if (wasVisited == visited) {
        return false;
    }
    wasVisited = visited;
    return true;
}

bool CTheZones::GetCurrentZoneLockedOrUnlocked(CVector2D pos) {
    return GetZoneWasVisited(pos);
}

// 0x572180
// Como el original: compara los 8 bytes de la etiqueta en bruto, indices de 16 bits sin signo. Si el indice
// propio es menor que el total y no hay coincidencia antes, el bucle del EXE no termina (CreateZone lo llama
// con el indice igual al total).
void CTheZones::AssignZoneInfoForThisZone(int16 newZoneIndex) {
    auto* zones = NavigationZoneArray.data();
    const uint16 self  = (uint16)newZoneIndex;
    const uint16 count = (uint16)TotalNumberOfNavigationZones;
    uint16 i = 0;
    bool found = false;
    if (count != 0) {
        do {
            if (found) {
                break;
            }
            if (i != self) {
                const auto* l = reinterpret_cast<const uint32*>(zones[i].m_InfoLabel);
                const auto* r = reinterpret_cast<const uint32*>(zones[self].m_InfoLabel);
                if (l[0] == r[0] && l[1] == r[1]) {
                    found = true;
                } else {
                    i++;
                }
            }
        } while (i < count);
    }
    if (found) {
        zones[self].m_ZoneInfoIndex = zones[i].m_ZoneInfoIndex;
    } else {
        zones[self].m_ZoneInfoIndex = TotalNumberOfZoneInfos;
        TotalNumberOfZoneInfos = (int16)(uint16)((uint16)TotalNumberOfZoneInfos + 1);
    }
}

// 0x572220
bool CTheZones::ZoneIsEntirelyContainedWithinOtherZone(CZone* a, CZone* b) {
    return a->m_fX1 >= b->m_fX1
        && a->m_fX2 <= b->m_fX2

        && a->m_fY1 >= b->m_fY1
        && a->m_fY2 <= b->m_fY2

        && a->m_fZ1 >= b->m_fZ1
        && a->m_fZ2 <= b->m_fZ2;
}

// Returns true if point lies within zone
// 0x572270
bool CTheZones::PointLiesWithinZone(const CVector* point, CZone* zone) {
    return (
        (float)zone->m_fX1 <= point->x &&
        (float)zone->m_fX2 >= point->x &&
        (float)zone->m_fY1 <= point->y &&
        (float)zone->m_fY2 >= point->y &&
        (float)zone->m_fZ1 <= point->z &&
        (float)zone->m_fZ2 >= point->z
    );
}

// Returns eLevelName from position
// 0x572300
// Como el original: zonas de mapa 1..total-1 con contador de 16 bits sin signo; si ninguna contiene el punto,
// el nivel de la zona 0.
uint32 CTheZones::GetLevelFromPosition(const CVector& point) {
    auto* zones = MapZoneArray.data();
    const uint16 count = (uint16)TotalNumberOfMapZones;
    for (uint16 i = 1; i < count; i++) {
        if (PointLiesWithinZone(&point, &zones[i])) {
            return zones[i].m_nLevel;
        }
    }
    return zones[0].m_nLevel;
}

// Returns pointer to zone by a point
// 0x572360
// Como el original: empieza en la zona 1; tamano = X2 - X1 + Y2 - Y1 en 32 bits comparado sin signo (jae).
CZone* CTheZones::FindSmallestZoneForPosition(const CVector& point, bool checkIsNavi) {
    auto* zones = NavigationZoneArray.data();
    CZone* smallestZone     = &zones[0];
    uint32 smallestZoneSize = (uint32)((int32)zones[0].m_fX2 - (int32)zones[0].m_fX1 - (int32)zones[0].m_fY1 + (int32)zones[0].m_fY2);
    const uint16 count = (uint16)TotalNumberOfNavigationZones;
    for (uint16 i = 1; i < count; i++) {
        CZone* z = &zones[i];
        if (checkIsNavi && z->m_nType != ZONE_TYPE_NAVI) {
            continue;
        }
        if (!PointLiesWithinZone(&point, z)) {
            continue;
        }
        const uint32 zsize = (uint32)((int32)z->m_fY2 - (int32)z->m_fX1 + (int32)z->m_fX2 - (int32)z->m_fY1);
        if (zsize < smallestZoneSize) {
            smallestZone     = z;
            smallestZoneSize = zsize;
        }
    }
    return smallestZone;
}

// 0x572400
CZoneInfo* CTheZones::GetZoneInfo(const CVector& point, CZone** outZone) {
    const auto z = FindSmallestZoneForPosition(point, false);
    if (!z) {
        if (outZone) {
            *outZone = &NavigationZoneArray[0];
        }
        return &ZoneInfoArray[0];
    }
    if (outZone) {
        *outZone = z;
    }
    return &ZoneInfoArray.data()[(uint16)z->m_ZoneInfoIndex]; // Como el original: indice sin signo (movzx)
}

// 0x572440
// Como el original: colores de las tablas del EXE (rojo/verde/azul por banda, 12 bytes cada una), divisiones
// enteras con signo, alfa 0 sin bandas y min(suma*3, 120) con minimo 55 si hay alguna.
void CTheZones::FillZonesWithGangColours(bool disableRadarGangColors) {
    const auto* colR = reinterpret_cast<const uint8*>(0x8D1344);
    const auto* colG = reinterpret_cast<const uint8*>(0x8D1350);
    const auto* colB = reinterpret_cast<const uint8*>(0x8D135C);
    if ((uint16)TotalNumberOfZoneInfos == 0) {
        return;
    }
    int32 i = 0;
    do {
        auto& z = ZoneInfoArray.data()[i];
        const int32 g0 = z.GangStrength[0], g1 = z.GangStrength[1], g2 = z.GangStrength[2];
        const int32 r = colR[0] * g0 + colR[1] * g1 + colR[2] * g2;
        const int32 g = colG[0] * g0 + colG[1] * g1 + colG[2] * g2;
        const int32 b = colB[0] * g0 + colB[1] * g1 + colB[2] * g2;
        int32 sum = g0 + g1 + g2;

        if (sum != 0 && !disableRadarGangColors && CGangWars::CanPlayerStartAGangWarHere(&z)) {
            z.RadarMode = 1;
        } else {
            z.RadarMode = 0;
        }

        uint8 alpha = (uint8)std::min(sum * 3, 0x78);
        if (sum != 0 && alpha <= 0x37) {
            alpha = 0x37;
        }
        z.ZoneColor.a = alpha;

        if (sum <= 1) {
            sum = 1;
        }
        z.ZoneColor.r = (uint8)(r / sum);
        z.ZoneColor.g = (uint8)(g / sum);
        z.ZoneColor.b = (uint8)(b / sum);
    } while (++i < (int32)(uint16)TotalNumberOfZoneInfos);
}

// Returns pointer to zone by index
// 0x572590
CZone* CTheZones::GetNavigationZone(uint16 index) {
    return &NavigationZoneArray.data()[index];
}

// Returns pointer to zone by index
// 0x5725A0
CZone* CTheZones::GetMapZone(uint16 index) {
    return &MapZoneArray.data()[index];
}

// 0x5725b0
float CTheZones::Calc2DDistanceBetween2Zones(CZone* a, CZone* b) {
    int32 dx, dy;

    if (a->m_fX1 <= b->m_fX2) {
        dx = a->m_fX2 >= b->m_fX1
            ? 0
            : b->m_fX1 - a->m_fX2;
    } else {
        dx = a->m_fX1 - b->m_fX2;
    }

    if (a->m_fY1 <= b->m_fY2) { // Copy paste of the above but for the `y` axis
        dy = a->m_fY2 >= b->m_fY1
            ? 0
            : b->m_fY1 - a->m_fY2;
    } else {
        dy = a->m_fY1 - b->m_fY2;
    }

    return std::sqrt(sq((float)(dx)) + sq((float)(dy)));
}

// Initializes CTheZones
// 0x572670
// Como el original: no escribe el byte de nivel (+0x1F) de las zonas vacias, las zonas de mapa quedan con tipo 3,
// las zonas 0 se rellenan en linea (sin CreateZone) y el nivel actual se escribe como dword.
void CTheZones::Init() {
    ZoneScoped;

    for (auto& z : NavigationZoneArray) {
        std::memset(&z, 0, 0x1F);
    }
    InitZonesPopulationSettings();
    std::memset(&ZonesVisited, 0, 100);

    auto* nav = reinterpret_cast<uint8*>(&NavigationZoneArray[0]);
    std::memcpy(nav, "SAN_AND", 8);
    std::memcpy(nav + 8, "SAN_AND", 8);
    TotalNumberOfNavigationZones = 1;
    TotalNumberOfZoneInfos       = 1;
    ZonesRevealed                = 0;
    NavigationZoneArray[0].m_fX1 = -3000;
    NavigationZoneArray[0].m_fY1 = -3000;
    NavigationZoneArray[0].m_fZ1 = -2000;
    NavigationZoneArray[0].m_fX2 = 3000;
    NavigationZoneArray[0].m_fY2 = 3000;
    NavigationZoneArray[0].m_fZ2 = 2000;
    nav[0x1F] = 0;
    nav[0x1E] = 0;
    *reinterpret_cast<uint32*>(&m_CurrLevel) = 0;

    for (auto& z : MapZoneArray) {
        std::memset(&z, 0, 0x1E);
        reinterpret_cast<uint8*>(&z)[0x1E] = ZONE_TYPE_MAP;
    }
    auto* map = reinterpret_cast<uint8*>(&MapZoneArray[0]);
    TotalNumberOfMapZones = 1;
    std::memcpy(map, "THEMAP", 7);
    std::memcpy(map + 8, "THEMAP", 7);
    MapZoneArray[0].m_fX1 = -3000;
    MapZoneArray[0].m_fY1 = -3000;
    MapZoneArray[0].m_fZ1 = -2000;
    MapZoneArray[0].m_fX2 = 3000;
    MapZoneArray[0].m_fY2 = 3000;
    MapZoneArray[0].m_fZ2 = 2000;
    map[0x1F] = 0;
}

// Unlock the current zone
// 0x572800
// Como el original: guarda SIEMPRE el nivel (dword en 0xBA6718) y sale si el jugador esta fuera del mapa o
// CGame::currArea != 0. La celda se calcula con (coord + 3000.0f) * 0x865060 (float, 24 bits en x87) y se trunca con
// _ftol2 quedandose con el byte bajo (0x572130 / 0x821B40); indice = 10 * celdaX - celdaY + 9 sobre ZonesVisited sin
// acotar, como el EXE.
//
// Registros de salida del EXE (0x572899), que reproduce ResidueRegsCpp (X87Math.h) como ultima llamada:
//   fuera del mapa : EAX = bPlayerIsOffTheMap,      ECX/EDX = los que deja 0x572300 (ZoneLevelResidue)
//   currArea != 0  : EAX = currArea,                ECX/EDX = los que deja 0x572300
//   ya visitada    : EAX = 1, ECX = 10*cx - cy,     EDX = EDX de _ftol2(y) (parte alta del int64 truncado)
//   celda nueva    : EAX = ZonesRevealed + 1,       ECX = ECX de _ftol2(y) (con ECX de entrada 10*cx), EDX = cy
// &pos del EXE es (ESP de entrada) - 12 (`sub esp,0Ch`); se obtiene con _AddressOfReturnAddress.
namespace {
// Registros de salida de _ftol2 (0x821B40): Ftol2RegsCpp de X87Math.cpp (compartido con CPed::GetLocalDirection).
using Zones13Regs = Ftol2Regs;

// (coord + 3000.0f) * 0x865060 en este orden (fadd [0x859A94]; fmul [0x865060]) y un redondeo a float por operacion; el /fp:fast global podria
// reasociar a coord * k + 3000 * k.
#pragma float_control(precise, on, push)
__declspec(noinline) float Zones13CellValue(float coord) {
    return (coord + 3000.0f) * std::bit_cast<float>(0x3ADA740Eu);
}
#pragma float_control(pop)

// ECX y EDX con que sale 0x572300 (GetLevelFromPosition) tras llamar a 0x572270 por cada zona 1..total-1 hasta la primera que contiene el punto:
// EDX = &pos, ECX = la zona probada salvo que llegue a la ultima comparacion (z2), donde 0x5722E4 la sustituye por z2 con signo.
// Sin ninguna llamada (total <= 1): ECX = &pos (lea ecx,[esp+8]) y EDX = el de 0x56E010 (&pos, o 0 si el jugador seleccionado no existe).
void Zones13LevelResidue(const CVector& pos, uint32 exePos, uint32& ecx, uint32& edx) {
    ecx = exePos;
    edx = CWorld::Players[CWorld::PlayerInFocus].m_pPed ? exePos : 0u;
    const CZone* zones = CTheZones::MapZoneArray.data();
    const uint16 count = (uint16)CTheZones::TotalNumberOfMapZones;
    for (uint16 i = 1; i < count; i++) {
        const CZone& z = zones[i];
        edx = exePos;
        ecx = uint32(uintptr_t(&z));
        if (!((float)z.m_fX1 <= pos.x) || !((float)z.m_fX2 >= pos.x) || !((float)z.m_fY1 <= pos.y) || !((float)z.m_fY2 >= pos.y) ||
            !((float)z.m_fZ1 <= pos.z)) {
            continue;
        }
        ecx = uint32(int32(z.m_fZ2)); // movsx ecx, word ptr [ecx+1Ah]
        if ((float)z.m_fZ2 >= pos.z) {
            break;
        }
    }
}
} // namespace

#pragma runtime_checks("", off)
__declspec(safebuffers) void CTheZones::SetCurrentZoneAsUnlocked() {
    const uint32 exePos = uint32(uintptr_t(_AddressOfReturnAddress())) - 12u; // &pos del EXE
    const CVector pos = FindPlayerCoors(-1);
    *reinterpret_cast<uint32*>(&m_CurrLevel) = GetLevelFromPosition(pos);
    uint32 resEcx, resEdx;
    Zones13LevelResidue(pos, exePos, resEcx, resEdx);
    uint32 resEax;
    const uint8 offMap = *reinterpret_cast<const uint8*>(&CTheScripts::bPlayerIsOffTheMap);
    if (offMap != 0) {
        resEax = offMap;
    } else if (static_cast<int32>(CGame::currArea) != 0) {
        resEax = uint32(static_cast<int32>(CGame::currArea));
    } else {
        const Zones13Regs fx = Ftol2RegsCpp(Zones13CellValue(pos.x), 0);
        const uint32 cx = fx.eax & 0xFFu;
        const Zones13Regs fy = Ftol2RegsCpp(Zones13CellValue(pos.y), 10u * cx);
        const uint32 cy = fy.eax & 0xFFu;
        const int32 index = 10 * (int32)cx - (int32)cy;
        auto* cells = reinterpret_cast<uint8*>(&ZonesVisited);
        if (cells[index + 9] != 0) {
            resEax = 1;
            resEcx = uint32(index);
            resEdx = fy.edx;
        } else {
            const int32 revealed = ZonesRevealed;
            cells[index + 9]     = 1;
            ZonesRevealed        = revealed + 1;
            resEax = uint32(revealed + 1);
            resEcx = fy.ecx;
            resEdx = cy;
        }
    }
    (void)ResidueRegsCpp(resEcx, resEdx, resEax); // lo ultimo: sale con ECX/EDX/EAX del EXE (contrib/audit_register_residue.py)
}
#pragma runtime_checks("", restore)

// Creates a zone
// 0x5728A0
// Como el original: ordena las coordenadas en float antes de truncarlas (_ftol2), pasa las etiquetas de origen a
// mayusculas en el propio buffer (solo a-z), copia como mucho 7 caracteres sin limpiar los bytes que siguen al
// NUL, solo los tipos 0/1 (navegacion) y 3 (mapa) crean zona, y AssignZoneInfoForThisZone recibe el indice
// antes de incrementar el total.
void CTheZones::CreateZone(
    const char* infoLabel,
    eZoneType   type,
    CVector     pos1,
    CVector     pos2,
    eLevelName  level,
    const char* textLabel
) {
    if (pos1.x > pos2.x) {
        std::swap(pos1.x, pos2.x);
    }
    if (pos1.y > pos2.y) {
        std::swap(pos1.y, pos2.y);
    }
    if (pos1.z > pos2.z) {
        std::swap(pos1.z, pos2.z);
    }

    const auto ToUpperInPlace = [](const char* str) {
        auto* p = const_cast<char*>(str);
        const uint16 len = (uint16)std::strlen(str);
        for (uint16 i = 0; i < len; i++) {
            if (p[i] >= 'a' && p[i] <= 'z') {
                p[i] -= 0x20;
            }
        }
    };
    char info[8];
    char text[8];
    ToUpperInPlace(infoLabel);
    std::strncpy(info, infoLabel, 7);
    info[7] = 0;
    ToUpperInPlace(textLabel);
    std::strncpy(text, textLabel, 7);
    text[7] = 0;

    const auto Fill = [&](CZone* z, uint8 zoneType) {
        std::strcpy(z->m_InfoLabel, info);
        std::strcpy(z->m_TextLabel, text);
        reinterpret_cast<uint8*>(z)[0x1E] = zoneType;
        z->m_fX1 = (int16)(int64)pos1.x;
        z->m_fY1 = (int16)(int64)pos1.y;
        z->m_fZ1 = (int16)(int64)pos1.z;
        z->m_fX2 = (int16)(int64)pos2.x;
        z->m_fY2 = (int16)(int64)pos2.y;
        z->m_fZ2 = (int16)(int64)pos2.z;
    };

    const uint8 t = (uint8)type;
    if (t <= ZONE_TYPE_LOCAL_NAVI) {
        const uint16 idx = (uint16)TotalNumberOfNavigationZones;
        CZone* z = &NavigationZoneArray.data()[idx];
        Fill(z, t);
        z->m_nLevel = level;
        AssignZoneInfoForThisZone((int16)idx);
        TotalNumberOfNavigationZones = (int16)(uint16)(idx + 1);
    } else if (t == ZONE_TYPE_MAP) {
        const uint16 idx = (uint16)TotalNumberOfMapZones;
        CZone* z = &MapZoneArray.data()[idx];
        Fill(z, ZONE_TYPE_MAP);
        TotalNumberOfMapZones = (int16)(uint16)(idx + 1);
        z->m_nLevel = level;
    }
}

// Returns 1 if point lies within the specified zonename otherwise return 0
// 0x572B80
// Como el original: compara los 8 bytes de la etiqueta en bruto (texto para NAVI, info para INFO) y sigue
// buscando si el punto no esta dentro.
bool CTheZones::FindZone(CVector* point, uint64_t packedZoneName, eZoneType type) {
    size_t offset;
    switch (type) {
    case ZONE_TYPE_NAVI: offset = 8; break;
    case ZONE_TYPE_INFO: offset = 0; break;
    default:             return false;
    }
    const auto* name = reinterpret_cast<const uint32*>(&packedZoneName);
    const uint16 count = (uint16)TotalNumberOfNavigationZones;
    for (uint16 i = 0; i < count; i++) {
        CZone* z = &NavigationZoneArray.data()[i];
        const auto* label = reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(z) + offset);
        if (label[0] == name[0] && label[1] == name[1] && PointLiesWithinZone(point, z)) {
            return true;
        }
    }
    return false;
}

//! Find zone by name `name` and of type `type` and check if `point` lies within it
bool CTheZones::FindZone(const CVector& point, std::string_view name, eZoneType type) {
    switch (type) {
    case ZONE_TYPE_INFO:
    case ZONE_TYPE_NAVI:
        break;
    default:
        return false;
    }

    for (auto& zone : GetNavigationZones()) {
        if ((type == ZONE_TYPE_INFO ? zone.GetInfoLabel() : zone.GetNaviLabel()) == name) {
            if (PointLiesWithinZone(&point, &zone)) {
                return true;
            }
        }
    }

    return false;
}

// Returns pointer to zone by index
// 0x572C40
// Como el original: copia el nombre (strcpy sin limite) a un buffer de 8 bytes a cero, devuelve -1 si el tipo
// no es INFO y compara los 8 bytes de la etiqueta en bruto.
int16 CTheZones::FindZoneByLabel(const char* name, eZoneType type) {
    char label[8]{};
    std::strcpy(label, name);
    if (type != ZONE_TYPE_INFO) {
        return -1;
    }
    const auto* l = reinterpret_cast<const uint32*>(label);
    const uint16 count = (uint16)TotalNumberOfNavigationZones;
    for (uint16 i = 0; i < count; i++) {
        const auto* z = reinterpret_cast<const uint32*>(NavigationZoneArray.data()[i].m_InfoLabel);
        if (z[0] == l[0] && z[1] == l[1]) {
            return (int16)i;
        }
    }
    return -1;
}

// 0x572cc0
// Como el original: indices sin signo y el alfa no se toca.
void CTheZones::SetZoneRadarColours(int16 index, char radarMode, uint8 red, uint8 green, uint8 blue) {
    const auto zone = &ZoneInfoArray.data()[(uint16)NavigationZoneArray.data()[(uint16)index].m_ZoneInfoIndex];
    zone->RadarMode = radarMode;
    zone->ZoneColor.r = red;
    zone->ZoneColor.g = green;
    zone->ZoneColor.b = blue;
}

// Updates CTheZones info
// 0x572D10
// Como el original: usa la variable estatica del EXE (0xBA6720, guarda 0xBA6724), llama a 0x572800 y despues
// reinicia el contador con la hora actual.
void CTheZones::Update() {
    ZoneScoped;

    auto& guard        = StaticRef<uint32>(0xBA6724);
    auto& countSeconds = StaticRef<uint32>(0xBA6720);
    const uint32 now   = CTimer::GetTimeInMS();
    if (!(guard & 1)) {
        guard |= 1;
        countSeconds = now - 5000;
    }
    if (CGame::currArea != eAreaCodes::AREA_CODE_NORMAL_WORLD) {
        countSeconds = now;
        return;
    }
    if (now - countSeconds > 5000) {
        SetCurrentZoneAsUnlocked();
        countSeconds = CTimer::GetTimeInMS();
    }
}

// Save CTheZones info
// 0x5D2E60
// Como el original: guarda directamente desde las globales (nivel actual como dword), contadores de 16 bits sin
// signo, y devuelve true.
bool CTheZones::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(&m_CurrLevel, 4);
    CGenericGameStorage::SaveDataToWorkBuffer(&TotalNumberOfNavigationZones, 2);
    CGenericGameStorage::SaveDataToWorkBuffer(&TotalNumberOfZoneInfos, 2);
    CGenericGameStorage::SaveDataToWorkBuffer(&TotalNumberOfMapZones, 2);
    for (uint16 i = 0; i < (uint16)TotalNumberOfNavigationZones; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(&NavigationZoneArray.data()[i], 0x20);
    }
    for (uint16 i = 0; i < (uint16)TotalNumberOfZoneInfos; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(&ZoneInfoArray.data()[i], 0x11);
    }
    for (uint16 i = 0; i < (uint16)TotalNumberOfMapZones; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(&MapZoneArray.data()[i], 0x20);
    }
    CGenericGameStorage::SaveDataToWorkBuffer(&ZonesVisited, 100);
    CGenericGameStorage::SaveDataToWorkBuffer(&ZonesRevealed, 4);
    return true;
}

// Load CTheZones info
// 0x5D2F40
bool CTheZones::Load() {
    Init();

    CGenericGameStorage::LoadDataFromWorkBuffer(&m_CurrLevel, 4);
    CGenericGameStorage::LoadDataFromWorkBuffer(&TotalNumberOfNavigationZones, 2);
    CGenericGameStorage::LoadDataFromWorkBuffer(&TotalNumberOfZoneInfos, 2);
    CGenericGameStorage::LoadDataFromWorkBuffer(&TotalNumberOfMapZones, 2);
    for (uint16 i = 0; i < (uint16)TotalNumberOfNavigationZones; i++) {
        CGenericGameStorage::LoadDataFromWorkBuffer(&NavigationZoneArray.data()[i], 0x20);
    }
    for (uint16 i = 0; i < (uint16)TotalNumberOfZoneInfos; i++) {
        CGenericGameStorage::LoadDataFromWorkBuffer(&ZoneInfoArray.data()[i], 0x11);
    }
    for (uint16 i = 0; i < (uint16)TotalNumberOfMapZones; i++) {
        CGenericGameStorage::LoadDataFromWorkBuffer(&MapZoneArray.data()[i], 0x20);
    }
    CGenericGameStorage::LoadDataFromWorkBuffer(&ZonesVisited, 100);
    CGenericGameStorage::LoadDataFromWorkBuffer(&ZonesRevealed, 4);
    return true;
}

// dummy function
// 0x572B70
void CTheZones::PostZoneCreation() {
    // NOP
}

const GxtChar* CTheZones::GetZoneName(const CVector& point) {
    CZone* zone{};
    auto extraInfo = GetZoneInfo(point, &zone);
    if (zone)
        return zone->GetTranslatedName();

    return "Unknown zone"_gxt;
}
