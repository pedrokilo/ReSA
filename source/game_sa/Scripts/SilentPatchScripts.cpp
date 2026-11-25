// SilentPatch (MIT, Silent): ver docs/THIRD_PARTY_NOTICES.md. Parches de bytecode adaptados de SilentPatchSA\SilentPatchSA.cpp (namespace ScriptFixes) y ScriptSA.cpp.
#include "StdInc.h"
#include "SilentPatchScripts.h"

#include "TheScripts.h"
#include "Stats.h"
#include "extensions/SilentPatchConfig.h"

#include <algorithm>
#include <cstring>
#include <initializer_list>

namespace SilentPatchScripts {
namespace {
bool Enabled() {
    static const bool v = SilentPatchOption("EnableScriptFixes", true);
    return v;
}

//! Devuelve el puntero a la unica coincidencia exacta de `pat` en `block`; nullptr si no hay exactamente una
template <size_t N>
uint8* FindUnique(uint8* data, size_t size, const uint8 (&pat)[N]) {
    uint8* found = nullptr;
    for (uint8* p = data; p + N <= data + size; ++p) {
        if (std::memcmp(p, pat, N) == 0) {
            if (found) {
                NOTSA_LOG_INFO("SilentPatch scripts: patron ambiguo (mas de una coincidencia), no se aplica");
                return nullptr;
            }
            found = p;
        }
    }
    if (!found) {
        NOTSA_LOG_INFO("SilentPatch scripts: patron no encontrado, no se aplica");
    }
    return found;
}

template <size_t N>
uint8* FindInMission(const uint8 (&pat)[N]) { return FindUnique(CTheScripts::MissionBlock.data(), CTheScripts::MissionBlock.size(), pat); }

template <size_t N>
void Put(uint8* dst, const uint8 (&src)[N]) {
    std::memcpy(dst, src, N);
    NOTSA_LOG_INFO("SilentPatch scripts: parche de {} bytes aplicado", N);
}

// SilentPatch (MIT, Silent): Saving in Madd Dogg's mansion will no longer trigger the missing basketball glitch
void BasketballFix(uint8* pBuf, int32 nSize) {
    const auto store = [](uint8* p, float v) { std::memcpy(p, &v, sizeof(v)); };
    for (int32 i = 0, hits = 0; i + 4 <= nSize && hits < 7; i++, pBuf++) {
        uint32 v;
        std::memcpy(&v, pBuf, sizeof(v));
        switch (v) {
        case 0x449DE19A: hits++; store(pBuf, 1291.8f); break;    // Save pickup X
        case 0xC4416AE1: hits++; store(pBuf, -797.8284f); break; // Save pickup Y
        case 0x44886C7B: hits++; store(pBuf, 1089.5f); break;    // Save pickup Z
        case 0x449DF852: hits++; store(pBuf, 1286.8f); break;    // Save point X
        case 0xC44225C3: hits++; store(pBuf, -797.69f); break;   // Save point Y
        case 0x44885C7B: hits++; store(pBuf, 1089.1f); break;    // Save point Z
        case 0x43373AE1: hits++; store(pBuf, 90.0f); break;      // Save point A
        default: break;
        }
        if (hits == 7) {
            NOTSA_LOG_INFO("SilentPatch scripts: baloncesto parcheado (7 valores)");
        }
    }
}

// SilentPatch (MIT, Silent): IF HEIGHT_FLOAT_HJ > 4.0 -> IF HEIGHT_INT_HJ > 4 (Quadruple Stunt Bonus)
void QuadrupleStuntBonus() {
    static constexpr uint8 pat[]  = { 0x20, 0x00, 0x02, 0x60, 0x14, 0x06, 0x00, 0x00, 0x80, 0x40 };
    static constexpr uint8 repl[] = { 0x18, 0x00, 0x02, 0x30, 0x14, 0x01, 0x04, 0x00, 0x00, 0x00 };
    if (auto* p = FindUnique(CTheScripts::MainSCMBlock.data(), CTheScripts::MainSCMBlock.size(), pat)) {
        Put(p, repl);
    }
}

// SilentPatch (MIT, Silent): possible soft lock in "Sweet's Girl" initial cutscene (@ == int -> @ >= int en dos sitios)
void SweetsGirlFix() {
    for (const size_t off : { size_t(2510), size_t(2680) }) {
        uint16 op;
        std::memcpy(&op, &CTheScripts::MissionBlock[off], sizeof(op));
        if (op == 0x0039) {
            op = 0x0029;
            std::memcpy(&CTheScripts::MissionBlock[off], &op, sizeof(op));
            NOTSA_LOG_INFO("SilentPatch scripts: Sweet's Girl parcheado en {}", off);
        }
    }
}

// SilentPatch (MIT, Silent): occasional soft lock in "Mountain Cloud Boys"
void MountainCloudBoysFix() {
    static constexpr uint8 pat[]  = { 0xD6, 0x00, 0x04, 0x00, 0x39, 0x00, 0x03, 0xEF, 0x00, 0x04, 0x02, 0x4D, 0x00, 0x01, 0x90, 0xF2, 0xFF, 0xFF, 0xD6, 0x00, 0x04, 0x01 };
    static constexpr uint8 repl[] = { 0x00, 0x00, 0x00, 0x00, 0xD6, 0x00, 0x04, 0x03, 0x39, 0x00, 0x03, 0x2B, 0x00, 0x04, 0x0B, 0x39, 0x00, 0x03, 0xEF, 0x00, 0x04, 0x02 };
    if (auto* p = FindInMission(pat)) {
        Put(p, repl);
    }
}

// SilentPatch (MIT, Silent): birds not spawning in "Ice Cold Killa" (float invertido)
void IceColdKillaFix() {
    static constexpr uint8 pat[] = { 0x06, 0xB9, 0xA6, 0x27, 0x45 };
    if (auto* p = FindInMission(pat)) {
        float v;
        std::memcpy(&v, p + 1, sizeof(v));
        v = -v;
        std::memcpy(p + 1, &v, sizeof(v));
        NOTSA_LOG_INFO("SilentPatch scripts: Ice Cold Killa parcheado");
    }
}

// SilentPatch (MIT, Silent): streaming issues in "Supply Lines" and "Beefy Baron" (700.48 -> 10.0)
void SupplyLinesFix(bool isBeefyBaron) {
    static constexpr uint8 patBB[] = { 0x06, 0xB8, 0x9E, 0x3A, 0x44 };
    static constexpr uint8 patSL[] = { 0x06, 0xB8, 0x1E, 0x2F, 0x44 };
    uint8* p = isBeefyBaron ? FindInMission(patBB) : FindInMission(patSL);
    if (p) {
        const float v = 10.0f;
        std::memcpy(p + 1, &v, sizeof(v));
        NOTSA_LOG_INFO("SilentPatch scripts: Supply Lines/Beefy Baron parcheado");
    }
}

void ConesCommon(uint8* p, const uint8 (&gotoSkip)[7], size_t deleteOffset) {
    static constexpr uint8 cmpVal[]             = { 0x18, 0x00, 0x02, 0x20, 0x03, 0x04, 0x00 }; // trafficcone_counter > 0
    static constexpr uint8 subValDeleteObject[] = { 0x0C, 0x00, 0x02, 0x20, 0x03, 0x04, 0x01, 0x08, 0x01, 0x07, 0x24, 0x03, 0x20, 0x03, 0x2E, 0x80 }; // counter-- \ DELETE_OBJECT
    Put(p, gotoSkip);
    Put(p + 11, cmpVal);
    Put(p + deleteOffset, subValDeleteObject);
    // trafficcone_counter = 0 para que no ocurra la primera destruccion
    const int32 zero = 0;
    std::memcpy(&CTheScripts::ScriptSpace[800], &zero, sizeof(zero));
}

// SilentPatch (MIT, Silent; by Wesser): Driving School cones ("Blackboard glitch")
void DrivingSchoolConesFix() {
    static constexpr uint8 pat[]       = { 0x04, 0x00, 0x02, 0x20, 0x03, 0x04, 0x00, 0xD6, 0x00, 0x04, 0x00, 0x1A, 0x00, 0x04, 0x2E, 0x02, 0x20, 0x03, 0x4D, 0x00, 0x01, 0x60, 0x75, 0xFF, 0xFF, 0xBE, 0x00, 0x08, 0x01, 0x07, 0x24, 0x03, 0x20, 0x03, 0x2E, 0x80, 0x08, 0x00, 0x02, 0x20, 0x03, 0x04, 0x01 };
    static constexpr uint8 coneCoil[]  = { 0x1A, 0x00, 0x04, 0x17, 0x02, 0x20, 0x03 };
    static constexpr uint8 burnAndLap[] = { 0x1A, 0x00, 0x04, 0x23, 0x02, 0x20, 0x03 };
    auto* p  = FindInMission(pat);
    auto* cc = FindInMission(coneCoil);
    auto* bl = FindInMission(burnAndLap);
    if (p && cc && bl) {
        static constexpr uint8 gotoSkip[] = { 0x02, 0x00, 0x01, 0x8B, 0x75, 0xFF, 0xFF };
        ConesCommon(p, gotoSkip, 0x1B);
        cc[3] = 30; // Cone Coil: 23 -> 30
        bl[3] = 42; // Burn and Lap: 35 -> 42
    }
}

// SilentPatch (MIT, Silent; by Wesser): Bike School cones
void BikeSchoolConesFix() {
    static constexpr uint8 pat[] = { 0x04, 0x00, 0x02, 0x20, 0x03, 0x04, 0x00, 0xD6, 0x00, 0x04, 0x00, 0x1A, 0x00, 0x04, 0x2E, 0x02, 0x20, 0x03, 0x4D, 0x00, 0x01, 0xF8, 0xAD, 0xFF, 0xFF, 0x08, 0x01, 0x07, 0x24, 0x03, 0x20, 0x03, 0x2E, 0x80, 0x08, 0x00, 0x02, 0x20, 0x03, 0x04, 0x01 };
    if (auto* p = FindInMission(pat)) {
        static constexpr uint8 gotoSkip[] = { 0x02, 0x00, 0x01, 0x21, 0xAE, 0xFF, 0xFF };
        ConesCommon(p, gotoSkip, 0x19);
    }
}

// SilentPatch (MIT, Silent): "Air Raid" ya no roba el arma pesada del jugador (la guarda y la devuelve cargando el modelo)
void AirRaidFix() {
    static constexpr uint8 savePat[] = { 0xBD, 0x01, 0x03, 0x65, 0x01, 0x06, 0x00, 0x03, 0x48, 0x01, 0x04, 0x00 };
    static constexpr uint8 freePat[] = { 0xEF, 0x04, 0x0E, 0x06, 0x43, 0x41, 0x53, 0x49, 0x4E, 0x4F, 0xEF, 0x04, 0x0E, 0x0A, 0x4F, 0x4E, 0x5F, 0x4C, 0x4F, 0x4F, 0x4B, 0x45, 0x52, 0x53, 0xD8, 0x00, 0x51, 0x00 };
    uint8* savePlace = FindInMission(savePat);
    uint8* freePlace = FindInMission(freePat);
    if (!savePlace || !freePlace) {
        return;
    }
    uint8*     after   = freePlace + 28;
    const auto asmCmd  = [](uint8*& mem, std::initializer_list<uint8> bytes) {
        std::copy(bytes.begin(), bytes.end(), mem);
        mem += bytes.size();
    };
    const auto asmInt32 = [](uint8*& mem, int32 v) {
        std::memcpy(mem, &v, sizeof(v));
        mem += sizeof(v);
    };
    const auto asmMissionOffset = [&](uint8*& mem, const uint8* dest) {
        asmInt32(mem, static_cast<int32>(CTheScripts::MissionBlock.data() - dest)); // salto relativo al script de mision (negativo)
    };

    { // Guardar el arma
        uint8* initSpace = savePlace + 5;
        asmCmd(initSpace, { 0x50, 0x00, 0x01 }); // GOSUB zero1_store_weapon
        asmMissionOffset(initSpace, after);
        asmCmd(after, { 0x06, 0x00, 0x03, 0x48, 0x01, 0x04, 0x00 });                                                       // index_zero1 = 0
        asmCmd(after, { 0xB8, 0x04, 0x02, 0x0C, 0x00, 0x04, 0x08, 0x03, 0x72, 0x01, 0x03, 0x73, 0x01, 0x03, 0x74, 0x01 }); // GET_CHAR_WEAPON_IN_SLOT scplayer 8 ...
        asmCmd(after, { 0x51, 0x00 });                                                                                     // RETURN
    }
    { // Devolver el arma
        uint8* cleanupGosub = CTheScripts::MissionBlock.data() + 0x1E;
        int32  originalCleanup;
        std::memcpy(&originalCleanup, cleanupGosub, sizeof(originalCleanup));
        asmMissionOffset(cleanupGosub, after);

        asmCmd(after, { 0x12, 0x81 });       // NOT HAS_DEATHARREST_BEEN_EXECUTED
        asmCmd(after, { 0x4D, 0x00, 0x01 }); // GOTO_IF_FALSE originalMissionCleanup
        asmInt32(after, originalCleanup);
        asmCmd(after, { 0x47, 0x02, 0x03, 0x74, 0x01 });                                      // REQUEST_MODEL
        asmCmd(after, { 0x8B, 0x03 });                                                        // LOAD_ALL_MODELS_NOW
        asmCmd(after, { 0xB2, 0x01, 0x02, 0x0C, 0x00, 0x03, 0x72, 0x01, 0x03, 0x73, 0x01 }); // GIVE_WEAPON_TO_CHAR
        asmCmd(after, { 0x49, 0x02, 0x03, 0x74, 0x01 });                                      // MARK_MODEL_AS_NO_LONGER_NEEDED
        asmCmd(after, { 0x02, 0x00, 0x01 });                                                  // GOTO originalMissionCleanup
        asmInt32(after, originalCleanup);
    }
    NOTSA_LOG_INFO("SilentPatch scripts: Air Raid parcheado");
}
} // namespace

void OnScriptsLoaded() {
    if (!Enabled()) {
        return;
    }
    int32 mainGlobalsSize;
    std::memcpy(&mainGlobalsSize, &CTheScripts::ScriptSpace[3], sizeof(mainGlobalsSize));
    if (mainGlobalsSize > 8 && mainGlobalsSize < (int32)CTheScripts::MAIN_SCRIPT_SIZE) {
        BasketballFix(&CTheScripts::ScriptSpace[8], mainGlobalsSize);
        NOTSA_LOG_INFO("SilentPatch scripts: partida cargada, baloncesto/salto cuadruple revisados");
    }
    QuadrupleStuntBonus();
}

void OnMissionStarted(int32 missionId) {
    if (!Enabled()) {
        return;
    }
    NOTSA_LOG_INFO("SilentPatch scripts: mision {} iniciada", missionId);
    switch (missionId) {
    case 0:   BasketballFix(CTheScripts::MissionBlock.data(), (int32)CTheScripts::MissionBlock.size()); QuadrupleStuntBonus(); break; // INITIAL
    case 18:  SweetsGirlFix(); break;         // HOODS5
    case 53:  MountainCloudBoysFix(); break;  // WUZI1
    case 61:  IceColdKillaFix(); break;       // SYND4
    case 71:  DrivingSchoolConesFix(); break; // DSKOOL
    case 120: BikeSchoolConesFix(); break;    // BSKOOL
    case 72:  AirRaidFix(); break;            // ZERO1
    case 73:  SupplyLinesFix(false); break;   // ZERO2
    case 10:  SupplyLinesFix(true); break;    // ZERO5
    default: break;
    }
}

// SilentPatch (MIT, Silent): The gym glitch ("You have worked out enough..." showing infinitely)
bool GymGlitchFixDate(const char* scriptName, int32& day, int32& month) {
    if (!Enabled()) {
        return false;
    }
    if (std::strncmp(scriptName, "gymbike", 8) == 0 || std::strncmp(scriptName, "gymbenc", 8) == 0 || std::strncmp(scriptName, "gymtrea", 8) == 0 || std::strncmp(scriptName, "gymdumb", 8) == 0) {
        month = -1;
        day   = CStats::StatTypesInt[STAT_DAYS_PASSED_IN_GAME - CStats::FIRST_INT_STAT] + 1;
        return true;
    }
    return false;
}
} // namespace SilentPatchScripts
