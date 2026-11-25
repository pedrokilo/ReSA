// NOTSA (desviacion propia 15.P): banco en proceso de PadCheats; ver PadCheatsSelfTest.h.
#include "StdInc.h"
#include "PadCheatsSelfTest.h"

#include <cstdarg>
#include <cstdio>
#include <random>
#include <vector>

#include "PadCheats.h"
#include "Cheat.h"
#include "ControllerState.h"
#include "Stats.h"
#include "Weather.h"
#include "Clock.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Vehicle/Vehicle.h"

namespace PadCheatsSelfTest {
namespace {
using PadCheats::Btn;

FILE* g_log = nullptr;
int   g_pass = 0, g_fail = 0;

void Log(const char* fmt, ...) {
    if (!g_log) {
        g_log = fopen("logs/padcheats_selftest.log", "a");
    }
    if (!g_log) {
        return;
    }
    va_list a;
    va_start(a, fmt);
    vfprintf(g_log, fmt, a);
    va_end(a);
    fputc('\n', g_log);
    fflush(g_log);
}

void Check(bool ok, const char* fmt, ...) {
    char    b[512];
    va_list a;
    va_start(a, fmt);
    vsnprintf(b, sizeof(b), fmt, a);
    va_end(a);
    (ok ? g_pass : g_fail)++;
    Log("%s %s", ok ? "PASS" : "FALLO", b);
}

bool Enabled() {
    static const bool v = [] {
        char b[8]{};
        return GetEnvironmentVariableA("GTASA_PADCHEATS_SELFTEST", b, sizeof(b)) != 0 && b[0] == '1';
    }();
    return v;
}

void SetBtn(CControllerState& s, Btn b, int16 v) {
    switch (b) {
    case Btn::L1:       s.LeftShoulder1 = v; break;
    case Btn::L2:       s.LeftShoulder2 = v; break;
    case Btn::R1:       s.RightShoulder1 = v; break;
    case Btn::R2:       s.RightShoulder2 = v; break;
    case Btn::Up:       s.DPadUp = v; break;
    case Btn::Down:     s.DPadDown = v; break;
    case Btn::Left:     s.DPadLeft = v; break;
    case Btn::Right:    s.DPadRight = v; break;
    case Btn::Cross:    s.ButtonCross = v; break;
    case Btn::Circle:   s.ButtonCircle = v; break;
    case Btn::Square:   s.ButtonSquare = v; break;
    case Btn::Triangle: s.ButtonTriangle = v; break;
    case Btn::L3:       s.ShockButtonL = v; break;
    case Btn::R3:       s.ShockButtonR = v; break;
    default: break;
    }
}

std::vector<Btn> SeqOf(const PadCheats::Entry& e) { return { e.seq, e.seq + e.len }; }

// ---------------------------------------------------------------- Fase A: detector puro
void PhaseA() {
    const auto entries = PadCheats::Entries();
    Log("--- Fase A: detector puro, %zu entradas", entries.size());

    // A0: anclas literales de la peticion (PS2): Weapons set 1 = R1 R2 L1 R2 <- v -> ^ <- v -> ^ ; Jetpack = ^ v <- -> L1 L2 R1 R2 ^ v <- ->
    const std::vector<Btn> w1 = { Btn::R1, Btn::R2, Btn::L1, Btn::R2, Btn::Left, Btn::Down, Btn::Right, Btn::Up, Btn::Left, Btn::Down, Btn::Right, Btn::Up };
    const std::vector<Btn> jp = { Btn::Up, Btn::Down, Btn::Left, Btn::Right, Btn::L1, Btn::L2, Btn::R1, Btn::R2, Btn::Up, Btn::Down, Btn::Left, Btn::Right };
    Check(SeqOf(entries[0]) == w1 && entries[0].cheat == CHEAT_WEAPON_SET1, "ancla Weapons set 1 = secuencia de PS2 y CHEAT_WEAPON_SET1");
    Check(SeqOf(entries[27]) == jp && entries[27].cheat == CHEAT_GET_JETPACK, "ancla Jetpack = secuencia de PS2 y CHEAT_GET_JETPACK");

    // A1: unicidad: ninguna secuencia registrada (modo 0) es igual a otra ni sufijo de otra
    int conflicts = 0;
    for (size_t i = 0; i < entries.size(); i++) {
        for (size_t j = 0; j < entries.size(); j++) {
            if (i == j || entries[i].mode != 0 || entries[j].mode != 0 || entries[i].len > entries[j].len) {
                continue;
            }
            const auto a = SeqOf(entries[i]), b = SeqOf(entries[j]);
            if (std::equal(a.begin(), a.end(), b.end() - a.size())) {
                conflicts++;
                Log("  conflicto: '%s' es sufijo de '%s'", entries[i].name, entries[j].name);
            }
        }
    }
    Check(conflicts == 0, "ninguna secuencia registrada es sufijo de otra (conflictos=%d)", conflicts);

    // A2: las 87 entradas: cada secuencia se reconoce y da el truco esperado
    int ok87 = 0;
    for (size_t i = 0; i < entries.size(); i++) {
        const auto& e = entries[i];
        PadCheats::Detector d;
        int    hit = -1, hits = 0;
        uint32 t = 1000;
        for (const auto b : SeqOf(e)) {
            const int r = d.Feed(b, t += 120);
            if (r >= 0) {
                hit = r;
                hits++;
            }
        }
        // Esperado: modo 0 -> ella misma; modo 1 (alias) o 2 (duplicada) -> la entrada registrada con la misma secuencia
        int expect = -1;
        for (size_t j = 0; j < entries.size(); j++) {
            if (entries[j].mode == 0 && SeqOf(entries[j]) == SeqOf(e)) {
                expect = (int)j;
                break;
            }
        }
        const bool good = hits == 1 && hit == expect && expect >= 0 && (e.mode == 2 || entries[expect].cheat == e.cheat || e.mode == 1);
        if (e.mode == 1) {
            Check(good && entries[hit].cheat == e.cheat, "[%zu] '%s' (alias) -> entrada %d '%s' truco %d", i, e.name, hit, hit >= 0 ? entries[hit].name : "-", hit >= 0 ? (int)entries[hit].cheat : -1);
        } else if (e.mode == 2) {
            Check(good, "[%zu] '%s' (duplicada dudosa) -> entrada %d '%s' (no activa su propio truco)", i, e.name, hit, hit >= 0 ? entries[hit].name : "-");
        } else {
            Check(good, "[%zu] '%s' -> truco %d", i, e.name, (int)e.cheat);
        }
        ok87 += good;
    }
    Check(ok87 == (int)entries.size(), "las %zu entradas reconocidas con su secuencia (%d)", entries.size(), ok87);

    // A3: prefijos comunes: Weapons set 1/2/3 comparten 8 botones; cada una activa SOLO la suya, una vez
    for (int s = 0; s < 3; s++) {
        PadCheats::Detector d;
        std::vector<int>    hits;
        uint32              t = 0;
        for (const auto b : SeqOf(entries[s])) {
            if (const int r = d.Feed(b, t += 100); r >= 0) {
                hits.push_back(r);
            }
        }
        Check(hits.size() == 1 && hits[0] == s, "Weapons set %d: una sola activacion y la suya (activaciones=%zu)", s + 1, hits.size());
    }
    {
        // set 1 pulsado, y a continuacion los dos ultimos de set 2 sin los 10 primeros: ninguna activacion de set 2/3
        PadCheats::Detector d;
        uint32              t = 0;
        int                 n = 0;
        for (const auto b : SeqOf(entries[0])) {
            n += d.Feed(b, t += 100) >= 0;
        }
        // tras el reinicio al reconocer, solo los 2 ultimos de set 2
        n += d.Feed(Btn::Down, t += 100) >= 0;
        n += d.Feed(Btn::Left, t += 100) >= 0;
        Check(n == 1, "set 1 seguido de los 2 ultimos de set 2: solo una activacion (set 1) (n=%d)", n);
    }

    // A4: pulsaciones previas de basura no impiden el reconocimiento (sufijo del buffer)
    {
        PadCheats::Detector d;
        uint32              t = 0;
        for (const auto b : { Btn::Cross, Btn::Square, Btn::L3, Btn::R3, Btn::Down, Btn::Down, Btn::Up, Btn::Up, Btn::R2 }) {
            d.Feed(b, t += 100);
        }
        int hit = -1;
        for (const auto b : SeqOf(entries[27])) {
            if (const int r = d.Feed(b, t += 100); r >= 0) {
                hit = r;
            }
        }
        Check(hit == 27, "Jetpack tras basura previa: reconocido (hit=%d)", hit);
    }

    // A5: reinicio por inactividad: la mitad, una pausa mayor que kIdleResetMs, la otra mitad: no se reconoce; con pausa menor, si
    for (int caso = 0; caso < 2; caso++) {
        PadCheats::Detector d;
        const auto          s   = SeqOf(entries[27]);
        const uint32        gap = caso == 0 ? PadCheats::kIdleResetMs + 500 : PadCheats::kIdleResetMs - 500;
        uint32              t   = 0;
        int                 hit = -1;
        for (size_t k = 0; k < s.size(); k++) {
            t += (k == s.size() / 2) ? gap : 100;
            if (const int r = d.Feed(s[k], t); r >= 0) {
                hit = r;
            }
        }
        Check(caso == 0 ? hit == -1 : hit == 27, "Jetpack con pausa de %u ms a la mitad: %s", gap, hit >= 0 ? "reconocido" : "no reconocido");
    }

    // A6: secuencias casi correctas (un boton cambiado en cada posicion): ningun truco que no sea exactamente la secuencia enviada
    {
        long casos = 0, falsos = 0;
        for (size_t i = 0; i < entries.size(); i++) {
            const auto& e = entries[i];
            for (uint32 p = 0; p < e.len; p++) {
                for (int b = 0; b < (int)Btn::COUNT; b++) {
                    if ((Btn)b == e.seq[p]) {
                        continue;
                    }
                    auto s = SeqOf(e);
                    s[p]   = (Btn)b;
                    PadCheats::Detector d;
                    uint32              t = 0;
                    casos++;
                    for (const auto x : s) {
                        const int r = d.Feed(x, t += 100);
                        if (r >= 0) {
                            bool legit = false; // la secuencia mutada puede ser exactamente otra entrada registrada
                            for (const auto& o : entries) {
                                legit |= o.mode == 0 && SeqOf(o) == s;
                            }
                            falsos += !legit;
                        }
                    }
                }
            }
        }
        Check(falsos == 0, "secuencias con un boton cambiado: %ld casos, %ld activaciones indebidas", casos, falsos);
    }

    // A7: ruido aleatorio: muchas decenas de miles de pulsaciones, 0 activaciones
    for (int variante = 0; variante < 3; variante++) {
        std::mt19937        rng(12345u + variante);
        PadCheats::Detector d;
        uint32              t = 0;
        long                act = 0;
        const long          N = 400000;
        for (long k = 0; k < N; k++) {
            // variante 0: todos los botones; 1: solo los de las secuencias largas (cruceta y gatillos); 2: ritmo rapido (siempre dentro de 3 s)
            int b = (int)(rng() % (uint32)Btn::COUNT);
            if (variante == 1) {
                b = (int)(rng() % 8);
            }
            t += variante == 2 ? 30 : (uint32)(rng() % 400 + (rng() % 5000 == 0 ? 4000 : 0));
            act += d.Feed((Btn)b, t) >= 0;
        }
        Check(act == 0, "ruido aleatorio variante %d: %ld pulsaciones, %ld activaciones", variante, N, act);
    }
}

// ---------------------------------------------------------------- Fase B: con el juego, estados inyectados por DoCheats
void Press(const std::vector<Btn>& seq) {
    CControllerState down{}, up{};
    PadCheats::TestInjectState(0, &up); // suelta todo antes (el detector parte de "nada pulsado")
    CCheat::DoCheats();
    for (const auto b : seq) {
        down = {};
        SetBtn(down, b, 255);
        PadCheats::TestInjectState(0, &down);
        CCheat::DoCheats();
        PadCheats::TestInjectState(0, &up);
        CCheat::DoCheats();
    }
    PadCheats::TestInjectState(0, nullptr);
}

std::vector<std::pair<int, int>> WeaponSnapshot(CPlayerPed* p) {
    std::vector<std::pair<int, int>> v;
    for (auto& w : p->m_aWeapons) {
        v.emplace_back((int)w.m_Type, (int)w.m_TotalAmmo);
    }
    return v;
}

int CountModel(eModelID m) {
    int   n    = 0;
    auto* pool = GetVehiclePool();
    for (int32 i = 0; i < (int32)pool->GetSize(); i++) {
        if (!pool->IsFreeSlotAtIndex(i) && pool->GetAt(i)->GetModelId() == m) {
            n++;
        }
    }
    return n;
}

void RemoveVehiclesOfModel(eModelID m) {
    auto* pool = GetVehiclePool();
    for (int32 i = 0; i < (int32)pool->GetSize(); i++) {
        if (!pool->IsFreeSlotAtIndex(i) && pool->GetAt(i)->GetModelId() == m) {
            auto* v = pool->GetAt(i);
            CWorld::Remove(v);
            delete v;
        }
    }
}

size_t IndexOfCheat(eCheats c) {
    const auto e = PadCheats::Entries();
    for (size_t i = 0; i < e.size(); i++) {
        if (e[i].mode == 0 && e[i].cheat == c) {
            return i;
        }
    }
    return (size_t)-1;
}

void PhaseB(CPlayerPed* player) {
    const auto entries = PadCheats::Entries();
    Log("--- Fase B: estados inyectados por CCheat::DoCheats, efecto por memoria");
    Check(PadCheats::Enabled(), "opcion PadCheats encendida por defecto (o por entorno)");
    const auto before = PadCheats::TestActivations();

    // B1: armas (set 1/2/3): el resultado por el mando es identico al de activar el truco directamente (misma funcion), y los tres conjuntos son distintos
    std::vector<std::pair<int, int>> ref[3], got[3];
    for (int s = 0; s < 3; s++) {
        player->ClearWeapons();
        CCheat::ActivateCheatByIndex((int32)(CHEAT_WEAPON_SET1 + s));
        ref[s] = WeaponSnapshot(player);
        player->ClearWeapons();
        const auto clean = WeaponSnapshot(player);
        PadCheats::TestForceEnabled(0);
        Press(SeqOf(entries[s]));
        Check(WeaponSnapshot(player) == clean, "Weapons set %d con la opcion APAGADA: sin efecto", s + 1);
        PadCheats::TestForceEnabled(-1);
        Press(SeqOf(entries[s]));
        got[s] = WeaponSnapshot(player);
        Check(got[s] == ref[s] && got[s] != clean, "Weapons set %d por el mando = activacion directa y distinto del estado limpio", s + 1);
    }
    Check(got[0] != got[1] && got[1] != got[2] && got[0] != got[2], "los tres conjuntos de armas son distintos entre si");

    // B2: dinero + salud + armadura
    {
        auto& money = CWorld::Players[CWorld::PlayerInFocus].m_nMoney;
        player->m_fArmour = 0.0f;
        const int m0      = money;
        Press(SeqOf(entries[3]));
        Check(money == m0 + 250000 && player->m_fArmour >= 99.0f, "$250000 + salud + armadura: dinero %d -> %d, armadura %.0f", m0, (int)money, player->m_fArmour);
    }

    // B3: clima y reloj
    const struct { size_t idx; eWeatherType w; } weather[] = { { 80, WEATHER_SUNNY_LA }, { 81, WEATHER_EXTRASUNNY_LA }, { 82, WEATHER_CLOUDY_LA }, { 83, WEATHER_FOGGY_SF }, { 84, WEATHER_RAINY_COUNTRYSIDE }, { 85, WEATHER_SANDSTORM_DESERT } };
    for (const auto& w : weather) {
        CWeather::ForceWeatherNow(WEATHER_EXTRASUNNY_COUNTRYSIDE);
        PadCheats::TestForceEnabled(0);
        Press(SeqOf(entries[w.idx]));
        const bool offOk = CWeather::ForcedWeatherType == WEATHER_EXTRASUNNY_COUNTRYSIDE;
        PadCheats::TestForceEnabled(-1);
        Press(SeqOf(entries[w.idx]));
        Check(offOk && CWeather::ForcedWeatherType == w.w, "'%s': apagada sin cambio; encendida -> clima %d", entries[w.idx].name, (int)CWeather::ForcedWeatherType);
    }
    CCheat::m_aCheatsActive[CHEAT_ALWAYS_MIDNIGHT] = false;
    CClock::ms_nGameClockHours = 12;
    Press(SeqOf(entries[86]));
    Check(CClock::ms_nGameClockHours == 0 && CCheat::IsActive(CHEAT_ALWAYS_MIDNIGHT), "Always Midnight: hora %d, activo", (int)CClock::ms_nGameClockHours);
    Press(SeqOf(entries[86]));
    Check(!CCheat::IsActive(CHEAT_ALWAYS_MIDNIGHT), "Always Midnight: segunda vez lo desactiva (como el teclado)");
    CCheat::m_aCheatsActive[CHEAT_STOP_GAME_CLOCK_ORANGE_SKY] = false;
    CClock::ms_nGameClockHours = 12;
    Press(SeqOf(entries[78]));
    Check(CClock::ms_nGameClockHours == 21 && CCheat::IsActive(CHEAT_STOP_GAME_CLOCK_ORANGE_SKY), "Orange Sky (para el reloj): hora %d", (int)CClock::ms_nGameClockHours);
    Press(SeqOf(entries[78]));

    // B4: busqueda
    player->CheatWantedLevel(eWantedLevel::WANTED_CLEAN);
    Press(SeqOf(entries[12]));
    Check(player->GetWantedLevel() == eWantedLevel::WANTED_LEVEL_2, "Raise Wanted Level: 0 -> %d estrellas", (int)player->GetWantedLevel());
    Press(SeqOf(entries[11]));
    Check(player->GetWantedLevel() == eWantedLevel::WANTED_LEVEL_6, "6 Star Wanted Level: %d estrellas", (int)player->GetWantedLevel());
    Press(SeqOf(entries[13]));
    Check(player->GetWantedLevel() == eWantedLevel::WANTED_CLEAN, "Lower Wanted Level: %d estrellas", (int)player->GetWantedLevel());
    player->CheatWantedLevel(eWantedLevel::WANTED_LEVEL_4);
    CCheat::m_aCheatsActive[CHEAT_I_DO_AS_I_PLEASE] = false;
    Press(SeqOf(entries[10]));
    Check(player->GetWantedLevel() == eWantedLevel::WANTED_CLEAN && CCheat::IsActive(CHEAT_I_DO_AS_I_PLEASE), "Never Wanted: %d estrellas, truco activo", (int)player->GetWantedLevel());
    Press(SeqOf(entries[10]));

    // B5: salud infinita (alternancia) y estadisticas
    CCheat::m_aCheatsActive[CHEAT_INFINITE_HEALTH] = false;
    Press(SeqOf(entries[4]));
    const bool on1 = CCheat::IsActive(CHEAT_INFINITE_HEALTH);
    Press(SeqOf(entries[4]));
    Check(on1 && !CCheat::IsActive(CHEAT_INFINITE_HEALTH), "Infinite Health: la primera vez activa, la segunda desactiva (%d, %d)", (int)on1, (int)CCheat::IsActive(CHEAT_INFINITE_HEALTH));
    CStats::SetStatValue(STAT_FAT, 0.0f);
    CStats::SetStatValue(STAT_STAMINA, 0.0f);
    Press(SeqOf(entries[20]));
    Check(CStats::GetStatValue(STAT_STAMINA) >= 999.0f, "Max Stamina: estadistica %.0f", CStats::GetStatValue(STAT_STAMINA));
    // FatCheat reconstruye al jugador (ropa): se omite en el banco en proceso por coste; se cubre en la Fase A (secuencia -> truco)

    // B6: vehiculos (aparecen en el pool): Rhino, Hotring 1, Caddy
    const struct { size_t idx; eModelID m; } veh[] = { { 30, MODEL_RHINO }, { 34, MODEL_HOTRINA }, { 35, MODEL_HOTRINB }, { 32, MODEL_CADDY } };
    for (const auto& v : veh) {
        RemoveVehiclesOfModel(v.m);
        const int c0 = CountModel(v.m);
        PadCheats::TestForceEnabled(0);
        Press(SeqOf(entries[v.idx]));
        const bool offOk = CountModel(v.m) == c0;
        PadCheats::TestForceEnabled(-1);
        Press(SeqOf(entries[v.idx]));
        const int c1 = CountModel(v.m);
        Check(offOk && c1 == c0 + 1, "'%s': apagada sin vehiculo; encendida %d -> %d vehiculos del modelo %d", entries[v.idx].name, c0, c1, (int)v.m);
        RemoveVehiclesOfModel(v.m);
    }

    // B7: todos los trucos que son una simple alternancia (sin funcion en la tabla de trucos): flanco -> activo, segunda vez -> inactivo; y apagada no cambia
    int toggles = 0, togglesOk = 0;
    for (size_t i = 0; i < entries.size(); i++) {
        const auto& e = entries[i];
        if (e.mode != 0 || e.cheat >= TOTAL_CHEATS || CCheat::m_aCheatFunctions[e.cheat]) {
            continue;
        }
        toggles++;
        CCheat::m_aCheatsActive[e.cheat] = false;
        PadCheats::TestForceEnabled(0);
        Press(SeqOf(e));
        const bool offOk = !CCheat::IsActive(e.cheat);
        PadCheats::TestForceEnabled(-1);
        Press(SeqOf(e));
        const bool on = CCheat::IsActive(e.cheat);
        Press(SeqOf(e));
        const bool off = !CCheat::IsActive(e.cheat);
        const bool ok  = offOk && on && off;
        togglesOk += ok;
        Check(ok, "alternancia [%zu] '%s' (id %d): apagada=%d, 1.ª=%d, 2.ª desactiva=%d", i, e.name, (int)e.cheat, (int)offOk, (int)on, (int)off);
    }
    Check(togglesOk == toggles, "trucos de simple alternancia: %d de %d correctos", togglesOk, toggles);

    Check(PadCheats::TestActivations() > before, "contador de activaciones por mando: +%u", PadCheats::TestActivations() - before);
    CWeather::ForceWeatherNow(WEATHER_EXTRASUNNY_LA);
    Log("--- Fase B terminada");
}

// ---------------------------------------------------------------- Fase C: fotogramas reales (CPad::PCTempJoyState -> CPad::Update -> DoCheats)
struct Driver {
    std::vector<Btn> seq;
    size_t           i    = 0;
    int              sub  = 0;
    bool             done = true;
    int              after = 0; // fotogramas de espera tras terminar
    void Start(std::vector<Btn> s) { seq = std::move(s); i = 0; sub = 0; done = false; after = 0; }
    // llamado antes de CPad::UpdatePads de cada fotograma
    void Frame(CPad& pad) {
        if (done) {
            after++;
            return;
        }
        if (sub < 2) {
            SetBtn(pad.PCTempJoyState, seq[i], 255);
        }
        if (++sub == 4) {
            sub = 0;
            if (++i == seq.size()) {
                done = true;
            }
        }
    }
};

void OnFrameC(CPlayerPed* player) {
    static Driver  drv;
    static int     step = 0, phaseFrame = 0;
    static std::vector<std::pair<int, int>> ref, clean;
    const auto entries = PadCheats::Entries();
    auto& pad = *CPad::GetPad(0);
    if (step >= 100) {
        return;
    }
    drv.Frame(pad);
    phaseFrame++;
    const bool waited = drv.done && drv.after >= 6;
    const auto Next   = [&](int s) { step = s; phaseFrame = 0; };
    switch (step) {
    case 0:
        Log("--- Fase C: fotogramas reales; estado del mando escrito en CPad::PCTempJoyState (camino CPad::Update -> PadCheats::CaptureJoyState -> CCheat::DoCheats)");
        CWeather::ForceWeatherNow(WEATHER_FOGGY_SF);
        PadCheats::TestForceEnabled(0);
        drv.Start(SeqOf(entries[81]));
        Next(1);
        break;
    case 1:
        if (waited) {
            Check(CWeather::ForcedWeatherType == WEATHER_FOGGY_SF, "[real] Really Sunny con la opcion APAGADA: el clima no cambia (%d)", (int)CWeather::ForcedWeatherType);
            PadCheats::TestForceEnabled(-1);
            drv.Start(SeqOf(entries[81]));
            Next(2);
        }
        break;
    case 2:
        if (waited) {
            Check(CWeather::ForcedWeatherType == WEATHER_EXTRASUNNY_LA, "[real] Really Sunny encendida: clima %d (activaciones por mando: %u)", (int)CWeather::ForcedWeatherType, PadCheats::TestActivations());
            player->ClearWeapons();
            CCheat::ActivateCheatByIndex(CHEAT_WEAPON_SET1);
            ref = WeaponSnapshot(player);
            player->ClearWeapons();
            clean = WeaponSnapshot(player);
            drv.Start(SeqOf(entries[0]));
            Next(3);
        }
        break;
    case 3:
        if (waited) {
            Check(WeaponSnapshot(player) == ref && ref != clean, "[real] Weapons set 1 (12 botones en fotogramas reales) = activacion directa");
            drv.Start(SeqOf(entries[27]));
            Next(4);
        }
        break;
    case 4:
        if (waited) {
            Check(player->GetIntelligence()->GetTaskJetPack() != nullptr, "[real] Jetpack: tarea de jetpack del jugador creada (%p)", (void*)player->GetIntelligence()->GetTaskJetPack());
            // ruido: 600 pulsaciones aleatorias en fotogramas reales -> sin activaciones nuevas
            Next(5);
        }
        break;
    case 5: {
        static std::mt19937 rng(777);
        static uint32       act0 = PadCheats::TestActivations();
        static int          n    = 0;
        if (n < 300) {
            if (phaseFrame % 4 == 0) {
                const auto b = (Btn)(rng() % (uint32)Btn::COUNT);
                SetBtn(pad.PCTempJoyState, b, 255);
                n++;
            } else if (phaseFrame % 4 == 1) {
                // el estado se limpia solo en CPad::Update; la pulsacion dura un fotograma
            }
        } else {
            Check(PadCheats::TestActivations() == act0, "[real] 300 pulsaciones aleatorias en fotogramas reales: activaciones nuevas %u", PadCheats::TestActivations() - act0);
            Log("=== PADCHEATS SELFTEST resumen: PASS=%d FALLO=%d", g_pass, g_fail);
            Log("SELFTEST FIN");
            Next(100);
        }
        break;
    }
    default:
        break;
    }
}

} // namespace

void OnFrame() {
    if (!Enabled()) {
        return;
    }
    static int    stage      = 0;
    static uint32 startFrame = 0;
    auto*         player     = FindPlayerPed();
    if (!player || !player->GetRwObject() || CTimer::GetFrameCounter() < 200) {
        return;
    }
    if (stage == 0) {
        startFrame = CTimer::GetFrameCounter();
        stage      = 1;
        return;
    }
    if (CTimer::GetFrameCounter() - startFrame < 600) {
        return;
    }
    if (stage == 1) {
        stage = 2;
        Log("=== PADCHEATS SELFTEST inicio: jugador en (%.1f %.1f %.1f), opcion PadCheats=%d", player->GetPosition().x, player->GetPosition().y, player->GetPosition().z, (int)PadCheats::Enabled());
        PhaseA();
        PhaseB(player);
        Log("=== fases A y B: PASS=%d FALLO=%d", g_pass, g_fail);
        return;
    }
    if (stage == 2) {
        OnFrameC(player);
    }
}

} // namespace PadCheatsSelfTest
