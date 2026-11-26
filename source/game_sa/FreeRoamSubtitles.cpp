/*
 * Subtitulos de las voces del mundo libre (NOTSA). Ver FreeRoamSubtitles.h y docs/CodigoVivoExe/subtitulos-mundo-libre.md.
 * Idea, logica y textos de Free Roam Subtitled de Kaizo M, usados con su autorizacion (declarada por el usuario del proyecto); los textos no se redistribuyen: se cargan de los
 * .fxt de la instalacion del usuario. El codigo es propio (reconstruccion de la logica del script CLEO, no una copia).
 */
#include "StdInc.h"

#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <optional>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "FreeRoamSubtitles.h"
#include "Messages.h"
#include "Timer.h"
#include "MenuManager.h"
#include "GameLogic.h"
#include "World.h"
#include "Pools/Pools.h"
#include "Entity/Ped/Ped.h"
#include "Tasks/TaskManager.h"
#include "Enums/eTaskType.h"
#include "Enums/ePedType.h"
#include "Audio/Entities/AEPedSpeechAudioEntity.h"
#include "Population.h"
#include "Streaming.h"

namespace FreeRoamSubtitles {
namespace {
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Utilidades de texto y de ini
// ---------------------------------------------------------------------------------------------------------------------------------------------------
std::string Lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}

std::string Trim(const std::string& s) {
    const auto a = s.find_first_not_of(" \t\r\n");
    return a == std::string::npos ? std::string{} : s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}

// Como el script: cualquier valor distinto de 0 es "si"; no numerico = se ignora (el script conserva el valor por defecto si la clave no se lee).
std::optional<int> ToInt(const std::string& v) {
    const auto t = Trim(v);
    if (t.empty()) {
        return std::nullopt;
    }
    const auto l = Lower(t);
    if (l == "true" || l == "on" || l == "yes") {
        return 1;
    }
    if (l == "false" || l == "off" || l == "no") {
        return 0;
    }
    size_t i = (t[0] == '-' || t[0] == '+') ? 1 : 0;
    if (i >= t.size()) {
        return std::nullopt;
    }
    long long r = 0;
    for (size_t k = i; k < t.size(); ++k) {
        if (t[k] < '0' || t[k] > '9') {
            return std::nullopt;
        }
        r = std::min<long long>(r * 10 + (t[k] - '0'), 1'000'000);
    }
    return (int)(t[0] == '-' ? -r : r);
}

std::optional<std::string> EnvString(const char* name) {
    char buf[260]{};
    const DWORD n = GetEnvironmentVariableA(name, buf, sizeof(buf));
    return (n > 0 && n < sizeof(buf)) ? std::optional<std::string>{Trim(buf)} : std::nullopt;
}

std::string ReadFileText(const char* path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return {};
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Estado
// ---------------------------------------------------------------------------------------------------------------------------------------------------
constexpr size_t kRingSlots = 20; // como el script: 20 textos persistentes por conjunto (CMessages::PreviousBriefs guarda punteros al texto, no copias)
constexpr size_t kTextMax   = 160;

struct Record {
    uint32      Id{};
    const void* Entity{};
    int16       Slot{-1};
    uint32      StartMs{};
    bool        IsPlayer{};
    float       Dist{};
    uint8       Len{};
    GxtChar     Text[kTextMax]{};
};

struct State {
    Config              Cfg{};
    uint64              LastReloadMs{};
    bool                ConfigLoadedOnce{};
    FxtTable            Table{};
    int                 TableLang{-1};
    std::string         TablePath{};
    uint64              TableRetryMs{};
    std::vector<Record> Active{};
    uint32              NextId{1};
    GxtChar             Ring[kRingSlots][kTextMax]{};
    size_t              RingNext{};
    const GxtChar*      CurText{};   // texto que mostramos ahora (puntero al anillo); null = no mostramos ninguno
    uint32              CurId{};     // registro que lo origina
    bool                CurEnded{};  // la voz del registro actual ya acabo y el tiempo de lectura esta fijado
    uint32              ShownMs{};   // reloj de juego de la ultima vez que se mostro
    GxtChar             LastSaved[kTextMax]{};
    uint32              LastSaveMs{};
    // pruebas
    int                 SelfTestMode{-1}; // bit 0 = banco en proceso, bit 1 = banco en juego
    FILE*               TestLog{};
};
State g;

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Opciones
// ---------------------------------------------------------------------------------------------------------------------------------------------------
std::unordered_map<std::string, std::string> ParseIniKeys(std::string_view text) {
    std::unordered_map<std::string, std::string> keys;
    std::istringstream                           in{std::string{text}};
    std::string                                  line;
    while (std::getline(in, line)) {
        if (const auto c = line.find(';'); c != std::string::npos) {
            line.erase(c);
        }
        const auto e = line.find('=');
        if (e == std::string::npos) {
            continue;
        }
        keys[Lower(Trim(line.substr(0, e)))] = Trim(line.substr(e + 1));
    }
    return keys;
}

void ApplyKeys(Config& c, const std::unordered_map<std::string, std::string>& k) {
    const auto Get = [&](const char* name) -> const std::string* {
        const auto it = k.find(name);
        return it == k.end() ? nullptr : &it->second;
    };
    const auto Bool = [&](const char* name, bool& dst) {
        if (const auto* v = Get(name)) {
            if (const auto i = ToInt(*v)) {
                dst = *i != 0;
            }
        }
    };
    Bool("freeroamsubtitles", c.Enabled);
    Bool("spainspanish", c.SpainSpanish);
    Bool("chicanoslangs", c.ChicanoSlangs);
    Bool("savetobrief", c.SaveToBrief);
    if (const auto* v = Get("readingtime")) {
        if (const auto i = ToInt(*v)) {
            c.ReadingTime = ClampReadingTime(*i);
        }
    }
    if (const auto* v = Get("freeroamsubtitlesdir"); v && !v->empty()) {
        c.Dir = *v;
    }
}

void ApplyEnv(Config& c) {
    const auto Bool = [](const char* name, bool& dst) {
        if (const auto v = EnvString(name)) {
            if (const auto i = ToInt(*v)) {
                dst = *i != 0;
            }
        }
    };
    Bool("GTASA_FREE_ROAM_SUBS", c.Enabled);
    Bool("GTASA_FRS_SPAINSPANISH", c.SpainSpanish);
    Bool("GTASA_FRS_CHICANOSLANGS", c.ChicanoSlangs);
    Bool("GTASA_FRS_SAVETOBRIEF", c.SaveToBrief);
    if (const auto v = EnvString("GTASA_FRS_READINGTIME")) {
        if (const auto i = ToInt(*v)) {
            c.ReadingTime = ClampReadingTime(*i);
        }
    }
    if (const auto v = EnvString("GTASA_FRS_DIR"); v && !v->empty()) {
        c.Dir = *v;
    }
}

Config LoadConfig() {
    Config c{};
    // Prioridad creciente: valores por defecto < cleo\FreeRoamSub.ini del mod < gta-reversed.ini < entorno
    ApplyKeys(c, ParseIniKeys(ReadFileText("cleo\\FreeRoamSub.ini")));
    ApplyKeys(c, ParseIniKeys(ReadFileText("gta-reversed.ini")));
    ApplyEnv(c);
    return c;
}

void MaybeReloadConfig() {
    const uint64 now = GetTickCount64();
    if (g.ConfigLoadedOnce && now - g.LastReloadMs < 2000) {
        return;
    }
    const Config old = g.Cfg;
    g.Cfg            = LoadConfig();
    g.LastReloadMs   = now;
    if (!g.ConfigLoadedOnce || old.Enabled != g.Cfg.Enabled || old.SpainSpanish != g.Cfg.SpainSpanish || old.ChicanoSlangs != g.Cfg.ChicanoSlangs ||
        old.SaveToBrief != g.Cfg.SaveToBrief || old.ReadingTime != g.Cfg.ReadingTime || old.Dir != g.Cfg.Dir) {
        SPDLOG_INFO("Subtitulos del mundo libre: activado={} SpainSpanish={} ChicanoSlangs={} SaveToBrief={} ReadingTime={} dir={}", g.Cfg.Enabled ? 1 : 0, g.Cfg.SpainSpanish ? 1 : 0,
                    g.Cfg.ChicanoSlangs ? 1 : 0, g.Cfg.SaveToBrief ? 1 : 0, g.Cfg.ReadingTime, g.Cfg.Dir);
    }
    g.ConfigLoadedOnce = true;
}

int GameLanguage() {
    if (const auto v = EnvString("GTASA_FRS_LANGUAGE")) { // solo para pruebas
        if (const auto i = ToInt(*v)) {
            return *i;
        }
    }
    return FrontEndMenuManager.m_nTextLanguage;
}

// Tabla de textos del idioma actual (carga perezosa: la primera voz que la necesita). Los demas idiomas usan English.fxt, como el script.
const FxtTable* EnsureTable(int language) {
    const std::string path = g.Cfg.Dir + (g.Cfg.Dir.empty() || g.Cfg.Dir.back() == '\\' || g.Cfg.Dir.back() == '/' ? "" : "\\") + (language == 4 ? "Spanish.fxt" : "English.fxt");
    if (g.TableLang == (language == 4 ? 4 : 0) && g.TablePath == path && g.Table.Size() > 0) {
        return &g.Table;
    }
    const uint64 now = GetTickCount64();
    if (g.TablePath == path && g.TableRetryMs && now < g.TableRetryMs) { // no se reintenta un fichero ausente en cada voz
        return nullptr;
    }
    g.Table.Clear();
    g.TableLang  = language == 4 ? 4 : 0;
    g.TablePath  = path;
    if (!g.Table.Load(path)) {
        g.TableRetryMs = now + 10000;
        SPDLOG_INFO("Subtitulos del mundo libre: no se pudo cargar '{}' (sin subtitulos de peatones hasta que exista; ver docs/CodigoVivoExe/subtitulos-mundo-libre.md)", path);
        return nullptr;
    }
    g.TableRetryMs = 0;
    SPDLOG_INFO("Subtitulos del mundo libre: cargado '{}' ({} textos, {} KB en memoria)", path, g.Table.Size(), g.Table.MemoryBytes() / 1024);
    return &g.Table;
}

// Texto de la voz (banco, sonido) segun el idioma y las opciones; vacio si no hay.
std::string_view LookupText(const FxtTable& table, int language, int bankID, int soundID, char* usedKey = nullptr) {
    char prefixes[3]{};
    const int n = KeyPrefixes(language, g.Cfg.SpainSpanish, g.Cfg.ChicanoSlangs, prefixes);
    for (int i = 0; i < n; i++) {
        const auto key = MakeKey(prefixes[i], bankID, soundID);
        if (key.empty()) {
            return {};
        }
        if (const auto text = table.Find(key); !text.empty()) {
            if (usedKey) {
                memcpy(usedKey, key.data(), 7);
            }
            return text;
        }
    }
    return {};
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Oyentes y reglas del script
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// El script calcula el hablante mas cercano a cada jugador (en dos jugadores, al mas cercano de los dos).
CPed* NearestHearer(const CVector& pos, float& dist) {
    CPed* best = nullptr;
    dist       = FLT_MAX;
    for (int id = 0; id < 2; id++) {
        if (id == 1 && !CGameLogic::IsCoopGameGoingOn()) {
            break;
        }
        auto* const p = FindPlayerPed(id);
        if (!p || !p->IsAlive()) {
            continue;
        }
        const float d = (p->GetPosition() - pos).Magnitude();
        if (d < dist) {
            dist = d;
            best = p;
        }
    }
    return best;
}

bool IsPartnerTask(CPed* ped) {
    auto& tm = ped->GetTaskManager();
    return tm.FindActiveTaskByType(TASK_COMPLEX_PARTNER_CHAT) || tm.FindActiveTaskByType(TASK_COMPLEX_PARTNER);
}

// Si hay una pareja de peatones charlando (TASK_COMPLEX_PARTNER / _CHAT) a menos de 10 m del oyente, solo se subtitula a quien esta en esa tarea (el script borra el texto de
// los demas hablantes).
bool SuppressedByPartnerChat(CPed* hearer, CPed* speaker) {
    if (IsPartnerTask(speaker)) {
        return false;
    }
    auto* const pool = GetPedPool();
    for (int32 i = 0; i < (int32)pool->GetSize(); i++) {
        auto* const p = pool->GetAt(i);
        if (!p || p == speaker || p == hearer || !p->IsAlive()) {
            continue;
        }
        if ((p->GetPosition() - hearer->GetPosition()).Magnitude() <= 10.f && IsPartnerTask(p)) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Pantalla
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void TestLogf(const char* fmt, ...) {
    if (!g.TestLog) {
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    vfprintf(g.TestLog, fmt, ap);
    va_end(ap);
    fputc('\n', g.TestLog);
    fflush(g.TestLog);
}

bool SlotStillOurs(const Record& r) {
    if (r.Slot < 0 || r.Slot >= (int16)CAEPedSpeechAudioEntity::s_PedSpeechSlots.size()) {
        return false;
    }
    const auto& slot = CAEPedSpeechAudioEntity::s_PedSpeechSlots[r.Slot];
    return slot.AudioEntity == r.Entity && slot.Status == CAEPedSpeechSlot::eStatus::PLAYING;
}

Record* FindRecord(uint32 id) {
    for (auto& r : g.Active) {
        if (r.Id == id) {
            return &r;
        }
    }
    return nullptr;
}

// Muestra el subtitulo del registro por el sistema de mensajes del juego (el mismo que los subtitulos de las misiones: texto de abajo del HUD). No pisa un mensaje que no sea
// nuestro. Devuelve true si lo muestra.
bool Show(const Record& r) {
    auto& m0 = CMessages::BriefMessages[0];
    if (m0.IsValid() && m0.Text != g.CurText) {
        return false; // el juego (mision, tienda, ayuda...) ya esta mostrando algo
    }
    GxtChar* const dst = g.Ring[g.RingNext];
    g.RingNext         = (g.RingNext + 1) % kRingSlots;
    memcpy(dst, r.Text, r.Len);
    dst[r.Len] = 0;

    const uint32 now   = CTimer::GetTimeInMS();
    bool         brief = g.Cfg.SaveToBrief;
    // Como el script: el mismo texto solo se vuelve a guardar en el Brief pasado mas de un segundo
    if (brief && g.LastSaved[0] && strcmp((const char*)g.LastSaved, (const char*)dst) == 0 && now - g.LastSaveMs <= 1000) {
        brief = false;
    }
    if (brief) {
        memcpy(g.LastSaved, dst, r.Len + 1);
        g.LastSaveMs = now;
    }
    CMessages::AddMessage2(dst, 1000, 0, brief, true);
    g.CurText = dst;
    g.CurId   = r.Id;
    g.CurEnded = false;
    g.ShownMs = now;
    TestLogf("MOSTRADO t=%u id=%u brief=%d texto=\"%s\"", now, r.Id, brief ? 1 : 0, (const char*)dst);
    return true;
}

void SelfTestTick();
void SelfTestInProcess();
} // namespace

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Funciones puras
// ---------------------------------------------------------------------------------------------------------------------------------------------------
float RadiusForBank(int bankID) {
    switch (bankID) {
    case 411: case 416: case 417: case 418: case 419: case 420: return 20.f; // novias (Barbara, Denise, Helena, Katie, Michelle, Millie)
    case 412: case 413:                                         // empleados de bar y de barberia (BMOBAR, BMYBARB)
    case 422: case 423: case 424: case 425: case 426: case 427: case 428: return 35.f; // dependientes (hamburgueseria, ropa, armeria, barbero, bar, ropa, pizzeria)
    case 644: return 40.f;                                                           // Cesar
    default: return 10.f;
    }
}

int KeyPrefixes(int language, bool spainSpanish, bool chicanoSlangs, char (&out)[3]) {
    int n = 0;
    if (language == 4) {
        if (chicanoSlangs) { out[n++] = 'M'; }
        if (spainSpanish) { out[n++] = 'P'; }
        out[n++] = 'S';
    } else {
        if (chicanoSlangs) { out[n++] = 'C'; }
        out[n++] = 'E';
    }
    return n;
}

std::string MakeKey(char prefix, int bankID, int soundID) {
    if (bankID < 0 || soundID < 0 || (bankID == 0 && soundID == 0) || bankID > 999 || soundID + 1 > 999) {
        return {};
    }
    char buf[16];
    snprintf(buf, sizeof(buf), "%c%03d%03d", prefix, bankID, soundID + 1);
    return buf;
}

int ClampReadingTime(int value) {
    return std::clamp(value, 0, 2000);
}

Config ParseIniText(const Config& base, std::string_view iniText) {
    Config c = base;
    ApplyKeys(c, ParseIniKeys(iniText));
    return c;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// FxtTable
// ---------------------------------------------------------------------------------------------------------------------------------------------------
bool FxtTable::EncodeKey(std::string_view key, uint32& out) {
    if (key.size() != 7) {
        return false;
    }
    constexpr std::string_view prefixes = "ECSPM";
    const auto                 p        = prefixes.find(key[0]);
    if (p == std::string_view::npos) {
        return false;
    }
    uint32 num = 0;
    for (size_t i = 1; i < 7; i++) {
        if (key[i] < '0' || key[i] > '9') {
            return false;
        }
        num = num * 10 + (key[i] - '0');
    }
    out = (uint32)p * 1'000'000u + num;
    return true;
}

void FxtTable::Parse() {
    m_Index.clear();
    size_t pos = 0;
    const size_t n = m_Data.size();
    m_Index.reserve(n / 40);
    while (pos < n) {
        size_t eol = m_Data.find('\n', pos);
        if (eol == std::string::npos) {
            eol = n;
        }
        size_t end = eol;
        if (end > pos && m_Data[end - 1] == '\r') {
            end--;
        }
        if (end - pos > 8 && m_Data[pos + 7] == ' ') {
            uint32 key{};
            if (EncodeKey(std::string_view{m_Data}.substr(pos, 7), key)) {
                m_Index.push_back({key, (uint32)(pos + 8), (uint32)(end - pos - 8)});
            }
        }
        pos = eol + 1;
    }
    std::stable_sort(m_Index.begin(), m_Index.end(), [](const Entry& a, const Entry& b) { return a.Key < b.Key; });
    m_Index.erase(std::unique(m_Index.begin(), m_Index.end(), [](const Entry& a, const Entry& b) { return a.Key == b.Key; }), m_Index.end()); // la primera aparicion gana
    m_Index.shrink_to_fit();
}

bool FxtTable::Load(const std::string& path) {
    Clear();
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) {
        return false;
    }
    const auto size = (size_t)in.tellg();
    if (size == 0 || size > 64u * 1024 * 1024) {
        return false;
    }
    in.seekg(0);
    m_Data.resize(size);
    in.read(m_Data.data(), (std::streamsize)size);
    Parse();
    return !m_Index.empty();
}

void FxtTable::LoadFromMemory(std::string data) {
    m_Data = std::move(data);
    Parse();
}

std::string_view FxtTable::Find(std::string_view key) const {
    uint32 k{};
    if (!EncodeKey(key, k)) {
        return {};
    }
    const auto it = std::lower_bound(m_Index.begin(), m_Index.end(), k, [](const Entry& e, uint32 v) { return e.Key < v; });
    if (it == m_Index.end() || it->Key != k) {
        return {};
    }
    return std::string_view{m_Data}.substr(it->Off, it->Len);
}

void FxtTable::Clear() {
    std::string().swap(m_Data);
    std::vector<Entry>().swap(m_Index);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Ganchos
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void OnPedSpeechStart(CPed* ped, int16 slotID, int16 bankID, int16 soundID, int16 gCtx, const void* speechEntity) {
    MaybeReloadConfig();
    if (!g.Cfg.Enabled || !ped) {
        return;
    }
    // Voces de script (m_LastGCtx = -1: AddScriptSayEvent) y de dolor (contextos >= 339): las primeras ya las subtitula el juego en misiones/dialogos; las segundas no se subtitulan.
    if (gCtx < 0 || gCtx >= (int16)CTX_GLOBAL_PAIN_START) {
        return;
    }
    if (!ped->IsAlive()) {
        return;
    }
    const bool isPlayer = ped->m_nPedType == PED_TYPE_PLAYER1 || ped->m_nPedType == PED_TYPE_PLAYER2;
    float      dist     = 0.f;
    if (!isPlayer) {
        CPed* const hearer = NearestHearer(ped->GetPosition(), dist);
        if (!hearer || dist > RadiusForBank(bankID)) {
            TestLogf("DESCARTADA fuera de rango banco=%d sonido=%d dist=%.1f radio=%.0f", (int)bankID, (int)soundID, dist, RadiusForBank(bankID));
            return;
        }
        if (SuppressedByPartnerChat(hearer, ped)) {
            TestLogf("DESCARTADA por pareja charlando cerca banco=%d sonido=%d", (int)bankID, (int)soundID);
            return;
        }
    }
    const int         language = GameLanguage();
    const FxtTable* const table = EnsureTable(language);
    if (!table) {
        return;
    }
    char usedKey[8]{};
    const auto text = LookupText(*table, language, bankID, soundID, usedKey);
    if (text.empty()) {
        TestLogf("SIN TEXTO banco=%d sonido=%d gctx=%d (sin clave en el .fxt)", (int)bankID, (int)soundID, (int)gCtx);
        return;
    }
    // Un hueco de voz solo tiene una voz a la vez
    std::erase_if(g.Active, [&](const Record& r) { return r.Slot == slotID; });
    Record r{};
    r.Id       = g.NextId++;
    r.Entity   = speechEntity;
    r.Slot     = slotID;
    r.StartMs  = CTimer::GetTimeInMS();
    r.IsPlayer = isPlayer;
    r.Dist     = dist;
    r.Len      = (uint8)std::min(text.size(), kTextMax - 1);
    memcpy(r.Text, text.data(), r.Len);
    g.Active.push_back(r);
    TestLogf("VOZ t=%u id=%u banco=%d sonido=%d gctx=%d jugador=%d dist=%.1f clave=%.7s idioma=%d texto=\"%.*s\"", r.StartMs, r.Id, (int)bankID, (int)soundID, (int)gCtx, isPlayer ? 1 : 0, dist,
             usedKey, language, (int)r.Len, (const char*)r.Text);
    Show(r); // el hablante mas reciente gana (la conversacion avanza por turnos)
}

void Tick() {
    if (g.SelfTestMode < 0) {
        char v[8]{};
        g.SelfTestMode = GetEnvironmentVariableA("GTASA_FRS_SELFTEST", v, sizeof(v)) ? atoi(v) : 0;
        if (g.SelfTestMode & 1) {
            SelfTestInProcess();
        }
    }
    if (g.SelfTestMode & 2) {
        SelfTestTick();
    }
    if (!g.CurText && g.Active.empty()) {
        return;
    }
    // Fin de voz: el hueco deja de ser del hablante (UpdateParameters con -1, StopCurrentSpeech, Terminate o Reset lo vacian)
    std::erase_if(g.Active, [](const Record& r) { return !SlotStillOurs(r); });

    auto& m0 = CMessages::BriefMessages[0];
    if (g.CurText) {
        if (!m0.IsValid() || m0.Text != g.CurText) { // expiro o lo reemplazo el juego
            TestLogf("RETIRADO t=%u (expiro o lo reemplazo el juego) desde-mostrado=%u ms", CTimer::GetTimeInMS(), CTimer::GetTimeInMS() - g.ShownMs);
            g.CurText = nullptr;
            g.CurId   = 0;
        } else {
            const uint32 now     = CTimer::GetTimeInMS();
            const uint32 elapsed = now >= m0.CreatedAtMS ? now - m0.CreatedAtMS : 0;
            if (CMessages::BriefMessages[1].IsValid()) {
                // el juego ha encolado un mensaje: cedemos el sitio (el subtitulo propio no retrasa los del juego)
                m0.VisibleTime = 0;
                TestLogf("CEDIDO t=%u (hay un mensaje del juego en cola)", now);
                g.CurText = nullptr;
                g.CurId   = 0;
            } else if (FindRecord(g.CurId)) {
                m0.VisibleTime = elapsed + 1000; // la voz sigue: se mantiene
            } else if (!g.CurEnded) {
                // la voz del subtitulo mostrado acabo: si habla otro hablante se muestra el suyo; si no, queda el tiempo de lectura
                const Record* next = g.Active.empty() ? nullptr : &g.Active.back();
                if (next && Show(*next)) {
                    return;
                }
                m0.VisibleTime = elapsed + (uint32)g.Cfg.ReadingTime;
                g.CurEnded     = true;
                TestLogf("FIN DE VOZ t=%u mostrado desde hace %u ms; se retira en %d ms (ReadingTime)", now, elapsed, g.Cfg.ReadingTime);
            }
        }
    } else if (!g.Active.empty() && g.Cfg.Enabled) {
        // una voz empezo mientras habia un mensaje del juego en pantalla y sigue sonando: se muestra en cuanto el sitio queda libre
        if (!CMessages::BriefMessages[0].IsValid() && !CMessages::BriefMessages[1].IsValid()) {
            Show(g.Active.back());
        }
    }
}

namespace {
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Bancos de pruebas (variable GTASA_FRS_SELFTEST: 1 = banco en proceso; 2 = banco en juego; 3 = ambos). Escriben en logs\.
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void SelfTestInProcess() {
    FILE* const f = fopen("logs/free_roam_subs_selftest.log", "w");
    if (!f) {
        return;
    }
    int fails = 0;
    const auto Check = [&](bool ok, const char* what) {
        fprintf(f, "%s %s\n", ok ? "OK    " : "FALLO ", what);
        fails += ok ? 0 : 1;
    };
    fprintf(f, "Banco en proceso de los subtitulos del mundo libre. Config por defecto: SpainSpanish=%d ChicanoSlangs=%d SaveToBrief=%d ReadingTime=%d activado=%d\n",
            Config{}.SpainSpanish, Config{}.ChicanoSlangs, Config{}.SaveToBrief, Config{}.ReadingTime, Config{}.Enabled);
    {
        const Config d{};
        Check(!d.Enabled && d.SpainSpanish && d.ChicanoSlangs && d.SaveToBrief && d.ReadingTime == 200, "valores por defecto = FreeRoamSub.ini de Kaizo (1,1,1,200) e interruptor general desactivado");
    }
    // claves y orden de busqueda
    {
        char p[3]{};
        int n = KeyPrefixes(4, true, true, p);
        Check(n == 3 && p[0] == 'M' && p[1] == 'P' && p[2] == 'S', "espanol con Spain+Chicano: M,P,S");
        n = KeyPrefixes(4, false, true, p);
        Check(n == 2 && p[0] == 'M' && p[1] == 'S', "espanol neutro con jerga: M,S");
        n = KeyPrefixes(4, true, false, p);
        Check(n == 2 && p[0] == 'P' && p[1] == 'S', "espanol Espana sin jerga: P,S");
        n = KeyPrefixes(4, false, false, p);
        Check(n == 1 && p[0] == 'S', "espanol neutro sin jerga: S");
        n = KeyPrefixes(0, true, true, p);
        Check(n == 2 && p[0] == 'C' && p[1] == 'E', "ingles con jerga: C,E");
        n = KeyPrefixes(1, true, false, p);
        Check(n == 1 && p[0] == 'E', "frances sin jerga: E (todos los idiomas distintos de 4 usan English.fxt)");
        Check(MakeKey('E', 700, 0) == "E700001", "clave banco 700 sonido 0 = E700001 (sonido+1)");
        Check(MakeKey('S', 5, 41) == "S005042", "clave banco 5 sonido 41 = S005042");
        Check(MakeKey('E', 0, 0).empty() && MakeKey('E', -1, 3).empty() && MakeKey('E', 3, -1).empty(), "sin sonido valido: sin clave");
        Check(!MakeKey('E', 0, 5).empty(), "banco 0 con sonido valido: clave");
    }
    Check(ClampReadingTime(-5) == 0 && ClampReadingTime(0) == 0 && ClampReadingTime(2000) == 2000 && ClampReadingTime(5000) == 2000 && ClampReadingTime(750) == 750, "ReadingTime limitado a 0..2000");
    Check(RadiusForBank(411) == 20.f && RadiusForBank(420) == 20.f && RadiusForBank(412) == 35.f && RadiusForBank(428) == 35.f && RadiusForBank(644) == 40.f && RadiusForBank(5) == 10.f &&
              RadiusForBank(421) == 10.f && RadiusForBank(429) == 10.f,
          "radio de audicion por banco (10 / 20 novias / 35 dependientes / 40 Cesar)");
    // ini
    {
        const Config base{};
        auto c = ParseIniText(base, "[Subtitles]\nSpainSpanish = 0 ; neutro\nChicanoSlangs=0\nSaveToBrief = 0\nReadingTime = 5000\n");
        Check(!c.SpainSpanish && !c.ChicanoSlangs && !c.SaveToBrief && c.ReadingTime == 2000 && !c.Enabled, "ini estilo Kaizo: 0/0/0 y ReadingTime 5000 -> 2000");
        c = ParseIniText(base, "FreeRoamSubtitles=0\nFreeRoamSubtitlesDir=C:\\x\\y\nReadingTime=-3\n");
        Check(!c.Enabled && c.Dir == "C:\\x\\y" && c.ReadingTime == 0, "interruptor general y carpeta; ReadingTime negativo -> 0");
        c = ParseIniText(base, "SpainSpanish=abc\nSaveToBrief=2\n");
        Check(c.SpainSpanish && c.SaveToBrief, "valor no numerico se ignora; cualquier valor distinto de 0 = si");
    }
    // tabla .fxt sintetica (textos de prueba propios, no del mod)
    {
        FxtTable t;
        t.LoadFromMemory("E700001 ingles uno\r\nC700001 jerga uno\r\nS700001 espanol uno\r\nP700001 espana uno\r\nM700001 chicano uno\r\nE700002 dos\nE700001 duplicado\r\nbasura sin clave\r\nE70000x malo\r\nE700003 tres ~n~ con salto\r\n");
        Check(t.Size() == 7, "tabla: 7 claves validas (duplicado y basura descartados)");
        Check(t.Find("E700001") == "ingles uno", "la primera aparicion de una clave gana");
        Check(t.Find("E700002") == "dos" && t.Find("E700003") == "tres ~n~ con salto", "lineas con LF y con CRLF");
        Check(t.Find("E700009").empty() && t.Find("X700001").empty() && t.Find("E7000").empty(), "claves ausentes o invalidas");
        // combinaciones de idioma y opciones sobre la misma tabla
        const auto Pick = [&](int lang, bool spain, bool chic) {
            char p[3]{};
            const int n = KeyPrefixes(lang, spain, chic, p);
            for (int i = 0; i < n; i++) {
                if (const auto s = t.Find(MakeKey(p[i], 700, 0)); !s.empty()) {
                    return std::string(s);
                }
            }
            return std::string{};
        };
        Check(Pick(4, true, true) == "chicano uno", "espanol 1/1 -> M (chicano)");
        Check(Pick(4, true, false) == "espana uno", "espanol Espana sin jerga -> P");
        Check(Pick(4, false, false) == "espanol uno", "espanol neutro sin jerga -> S");
        Check(Pick(4, false, true) == "chicano uno", "espanol neutro con jerga -> M");
        Check(Pick(0, true, true) == "jerga uno", "ingles con jerga -> C");
        Check(Pick(0, true, false) == "ingles uno", "ingles sin jerga -> E");
        Check(Pick(2, true, true) == "jerga uno", "aleman = ingles (C)");
    }
    // tabla grande sintetica: 60000 claves, tiempo y memoria
    {
        std::string big;
        big.reserve(3'000'000);
        for (int b = 0; b < 600; b++) {
            for (int s = 1; s <= 100; s++) {
                char line[96];
                snprintf(line, sizeof(line), "E%03d%03d Texto de prueba %d del banco %d\r\n", b, s, s, b);
                big += line;
            }
        }
        const size_t big_kb = big.size() / 1024;
        FxtTable t;
        const auto t0 = GetTickCount64();
        t.LoadFromMemory(std::move(big));
        const auto ms = GetTickCount64() - t0;
        Check(t.Size() == 60000 && t.Find("E599100").size() > 10 && t.Find("E000001").size() > 10, "tabla de 60000 claves sinteticas");
        fprintf(f, "INFO  carga de 60000 lineas (%zu KB de texto): %llu ms, memoria %zu KB (indice %zu KB)\n", big_kb, (unsigned long long)ms, t.MemoryBytes() / 1024, t.Size() * 12 / 1024);
        Check(ms < 500 && t.MemoryBytes() < 5u * 1024 * 1024, "carga < 500 ms y memoria < 5 MB (texto de ~2,6 MB como English.fxt)");
    }
    // ficheros reales si estan instalados (solo informa)
    for (const char* name : {"English.fxt", "Spanish.fxt"}) {
        FxtTable t;
        const auto path = Config{}.Dir + "\\" + name;
        const auto t0   = GetTickCount64();
        if (t.Load(path)) {
            fprintf(f, "INFO  %s: %zu textos, memoria %zu KB, carga %llu ms\n", path.c_str(), t.Size(), t.MemoryBytes() / 1024, (unsigned long long)(GetTickCount64() - t0));
        } else {
            fprintf(f, "INFO  %s no instalado\n", path.c_str());
        }
    }
    fprintf(f, "RESULTADO: %d fallos\nFIN\n", fails);
    fclose(f);
}

// Banco en juego: con el jugador a pie y quieto unos segundos, hace hablar a CJ y a un peaton creado por el camino real del juego (CPed::Say -> AddSayEvent -> PlayLoadedSound) y
// registra en logsree_roam_subs_juego.log lo que ocurre (voz, clave, texto, cuando se muestra y se retira). Pasos: 0 CJ habla; 1 peaton a 4 m habla (se muestra); 2 peaton a 14 m
// habla (fuera del radio de 10 m: no se muestra); 3 el peaton a 4 m y despues CJ hablan a la vez (gana el mas reciente y, al acabar, vuelve el otro si sigue hablando).
void SelfTestTick() {
    static int  frames = 0, step = -1, nextAt = 0, tries = 0, overlapAt = 0;
    static CPed* ped = nullptr;
    auto* const player = FindPlayerPed();
    if (!player || !player->IsAlive() || player->bInVehicle) {
        frames = 0;
        return;
    }
    if (step == 1 << 20) {
        return;
    }
    if (++frames < 300) {
        return;
    }
    if (step < 0) {
        g.TestLog = fopen("logs/free_roam_subs_juego.log", "w");
        if (!g.TestLog) {
            step = 1 << 20;
            return;
        }
        MaybeReloadConfig();
        TestLogf("Banco en juego de los subtitulos del mundo libre. activado=%d SpainSpanish=%d ChicanoSlangs=%d SaveToBrief=%d ReadingTime=%d dir=%s idioma=%d", g.Cfg.Enabled, g.Cfg.SpainSpanish,
                 g.Cfg.ChicanoSlangs, g.Cfg.SaveToBrief, g.Cfg.ReadingTime, g.Cfg.Dir.c_str(), GameLanguage());
        step   = 0;
        nextAt = frames;
        if (const auto* t = EnsureTable(GameLanguage())) {
            TestLogf("TABLA %zu textos, %zu KB en memoria", t->Size(), t->MemoryBytes() / 1024);
        } else {
            TestLogf("TABLA no cargada");
        }
    }
    static const eGlobalSpeechContext ctxs[] = {CTX_GLOBAL_BUMP, CTX_GLOBAL_BLOCKED, CTX_GLOBAL_CHAT, CTX_GLOBAL_CONV_DISL_WEATHER, CTX_GLOBAL_CONV_LIKE_WEATHER, CTX_GLOBAL_CONV_IGNORED,
                                                CTX_GLOBAL_APOLOGY, CTX_GLOBAL_CAR_CRASH};
    const auto Say = [&](CPed* who, const char* name) {
        who->GetSpeechAE().StopCurrentSpeech();
        for (int k = 0; k < (int)std::size(ctxs); k++) {
            const auto ctx = ctxs[(tries++) % std::size(ctxs)];
            const auto r   = who->Say(ctx, 0, 1.0f, true, true, false);
            TestLogf("PASO %d: %s Say(ctx=%d) = %d (%s)", step, name, (int)ctx, (int)r, r >= 0 ? "suena" : "mudo");
            if (r >= 0) {
                return true;
            }
        }
        return false;
    };
    if (overlapAt && frames >= overlapAt) {
        overlapAt = 0;
        if (Say(player, "CJ (a la vez)")) {
            step++;
            nextAt = frames + 480;
        } else {
            nextAt = frames + 30;
            overlapAt = frames + 30;
        }
        return;
    }
    if (frames < nextAt || overlapAt) {
        return;
    }
    if (step >= 4 || tries >= 120) {
        int nBrief = 0;
        for (const auto& b : CMessages::PreviousBriefs) {
            nBrief += b.Text ? 1 : 0;
        }
        TestLogf("BRIEF previo: %d entradas (CMessages::PreviousBriefs; la 0 es la mas reciente)", nBrief);
        for (size_t i = 0; i < CMessages::PreviousBriefs.size(); i++) {
            if (CMessages::PreviousBriefs[i].Text) {
                TestLogf("BRIEF[%zu] \"%s\"", i, (const char*)CMessages::PreviousBriefs[i].Text);
            }
        }
        TestLogf("FIN");
        fclose(g.TestLog);
        g.TestLog = nullptr;
        step      = 1 << 20;
        return;
    }
    if (g.CurText || !g.Active.empty()) { // espera a que la voz anterior y su subtitulo hayan terminado
        return;
    }
    if (step == 0) {
        if (Say(player, "CJ")) {
            step++;
            nextAt = frames + 360;
        } else {
            nextAt = frames + 60;
        }
        return;
    }
    // pasos 1..3: peaton creado a 4 m (o a 14 m en el paso 2) delante de CJ
    if (!ped) {
        if (!CStreaming::IsModelLoaded(MODEL_MALE01)) {
            CStreaming::RequestModel(MODEL_MALE01, STREAMING_GAME_REQUIRED);
            CStreaming::LoadAllRequestedModels(false);
        }
        ped = CStreaming::IsModelLoaded(MODEL_MALE01) ? CPopulation::AddPed(PED_TYPE_CIVMALE, MODEL_MALE01, player->GetPosition() + player->GetForward() * 4.f, false) : nullptr;
        TestLogf("peaton de prueba creado=%d", ped ? 1 : 0);
        if (!ped) {
            step = 4;
            return;
        }
    }
    ped->SetPosn(player->GetPosition() + player->GetForward() * (step == 2 ? 14.f : 4.f));
    ped->m_vecMoveSpeed = CVector{};
    if (step == 3) {
        if (Say(ped, "peaton a 4 m")) {
            overlapAt = frames + 25;
        } else {
            nextAt = frames + 30;
        }
        return;
    }
    if (Say(ped, step == 2 ? "peaton a 14 m" : "peaton a 4 m")) {
        step++;
        nextAt = frames + 360;
    } else {
        nextAt = frames + 30;
    }
}
} // namespace
} // namespace FreeRoamSubtitles
