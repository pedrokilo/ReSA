#include "StdInc.h"

#include <cstring>
#include <cstdlib>
#include <deque>
#include <string>
#include <vector>
#include <format>

#include "AccionesGrabador.h"
#include "Pad.h"
#include "Timer.h"
#include "World.h"
#include "Crt/CrtState.h"
#if __has_include("RenderWare/rw/SkinVigilanciaActivaPorDefecto.h")
#include "RenderWare/rw/SkinVigilanciaActivaPorDefecto.h" // solo existe al compilar el exe de vigilancia (no se versiona)
#endif

namespace {
constexpr size_t kTopeArchivo = 8u * 1024u * 1024u;
constexpr double kVentanaAnillo = 30000.0;      // ms de juego que se guardan para el volcado por evento
constexpr int kTopeEventos = 20;
constexpr int16 kUmbralStick = 1500;           // cambio minimo (de 32767) para anotar un stick
constexpr DWORD kMinMsStick = 50;              // <= 20 Hz por eje

const char* kNombresPad[24] = { "LeftStickX", "LeftStickY", "RightStickX", "RightStickY", "LeftShoulder1", "LeftShoulder2", "RightShoulder1", "RightShoulder2", "DPadUp", "DPadDown", "DPadLeft", "DPadRight",
    "Start", "Select", "ButtonSquare", "ButtonTriangle", "ButtonCross", "ButtonCircle", "ShockButtonL", "ShockButtonR", "m_bChatIndicated", "m_bPedWalk", "m_bVehicleMouseLook", "m_bRadioTrackSkip" };
static_assert(sizeof(CControllerState) == 24 * sizeof(int16));
bool EsStick(int i) { return i < 4 || (i >= 4 && i < 8); } // sticks y gatillos: analogicos

enum class Modo { Sin, Grabar, Repetir };
Modo g_modo = Modo::Sin;
bool g_iniciado = false;
HANDLE g_f = INVALID_HANDLE_VALUE;
size_t g_bytes = 0;
int g_eventos = 0;
std::deque<std::pair<uint32, std::string>> g_anillo; // (t juego ms, linea)
int16 g_ultimo[24] = {};
DWORD g_ultimoMs[24] = {};
uint8 g_ultimoRaton = 0;
uint32 g_ultimoCtx = 0;
bool g_hayUltimoEstado = false;
bool g_enVeh = false;
int g_arma = -1;

void Linea(uint32 tJuego, const std::string& s) {
    std::string l = s + "\n";
    if (g_f != INVALID_HANDLE_VALUE && g_bytes + l.size() <= kTopeArchivo) {
        DWORD w; WriteFile(g_f, l.data(), static_cast<DWORD>(l.size()), &w, nullptr); g_bytes += w;
    }
    g_anillo.emplace_back(tJuego, std::move(l));
    while (!g_anillo.empty() && g_anillo.front().first + kVentanaAnillo < tJuego) { g_anillo.pop_front(); }
}

// ---------------------------------------------------------------- reproduccion
struct EvRep { uint32 t; int tipo; int idx; int16 v; float dx, dy; uint8 b; }; // tipo 0 pad, 1 raton
std::vector<EvRep> g_ev;
size_t g_sig = 0;
uint32 g_t0 = 0, g_inicioRep = 0;
double g_vel = 1.0;
bool g_repIniciada = false;
int16 g_virt[24];
bool g_virtUsado[24] = {};
uint32 g_finRep = 0;

uint32 NumeroDe(const std::string& l, const char* clave, double def = 0) {
    const auto p = l.find(clave); if (p == std::string::npos) { return static_cast<uint32>(def); }
    return static_cast<uint32>(std::strtod(l.c_str() + p + std::strlen(clave), nullptr));
}
double RealDe(const std::string& l, const char* clave) {
    const auto p = l.find(clave); if (p == std::string::npos) { return 0; }
    return std::strtod(l.c_str() + p + std::strlen(clave), nullptr);
}

void CargarReplay(const char* ruta) {
    HANDLE f = CreateFileA(ruta, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) { NOTSA_LOG_ERR("[ACCIONES] no se pudo abrir {}", ruta); return; }
    LARGE_INTEGER sz{}; GetFileSizeEx(f, &sz);
    std::string d(static_cast<size_t>(sz.QuadPart), '\0'); DWORD r = 0; ReadFile(f, d.data(), static_cast<DWORD>(d.size()), &r, nullptr); CloseHandle(f);
    size_t pos = 0; uint32 semilla = 0; bool haySem = false;
    while (pos < d.size()) {
        auto fin = d.find('\n', pos); if (fin == std::string::npos) { fin = d.size(); }
        const std::string l = d.substr(pos, fin - pos); pos = fin + 1;
        if (l.find("\"k\":\"cab\"") != std::string::npos) { semilla = NumeroDe(l, "\"semilla\":"); haySem = l.find("\"semilla\":") != std::string::npos; continue; }
        if (l.find("\"k\":\"pad\"") != std::string::npos) {
            const auto p = l.find("\"n\":\""); if (p == std::string::npos) { continue; }
            const auto q = l.find('"', p + 5); const std::string n = l.substr(p + 5, q - p - 5);
            for (int i = 0; i < 24; ++i) {
                if (n == kNombresPad[i]) { g_ev.push_back({ NumeroDe(l, "\"t\":"), 0, i, static_cast<int16>(static_cast<int>(RealDe(l, "\"v\":"))), 0, 0, 0 }); break; }
            }
        } else if (l.find("\"k\":\"raton\"") != std::string::npos) {
            g_ev.push_back({ NumeroDe(l, "\"t\":"), 1, 0, 0, static_cast<float>(RealDe(l, "\"dx\":")), static_cast<float>(RealDe(l, "\"dy\":")), static_cast<uint8>(NumeroDe(l, "\"b\":")) });
        }
    }
    char b[16] = {};
    if (haySem && !(GetEnvironmentVariableA("GTASA_REPLAY_SEMILLA", b, sizeof(b)) > 0 && b[0] == '0')) { ExeCrt::Srand(semilla); }
    if (GetEnvironmentVariableA("GTASA_REPLAY_SPEED", b, sizeof(b)) > 0) { g_vel = std::strtod(b, nullptr); if (g_vel <= 0.01) { g_vel = 1.0; } }
    if (!g_ev.empty()) { g_t0 = g_ev.front().t; g_finRep = g_ev.back().t; }
    g_modo = Modo::Repetir;
    NOTSA_LOG_WARN("[ACCIONES] REPRODUCCION de {}: {} eventos, {:.1f} s de juego, velocidad x{}, semilla {} {:#x}", ruta, g_ev.size(), (g_finRep - g_t0) / 1000.0, g_vel, haySem ? "restaurada" : "no consta", semilla);
}

void Iniciar() {
    g_iniciado = true;
    char ruta[MAX_PATH] = {};
    if (GetEnvironmentVariableA("GTASA_REPLAY", ruta, sizeof(ruta)) > 0 && ruta[0]) { CargarReplay(ruta); if (g_modo == Modo::Repetir) { return; } }
    char b[8] = {};
    const DWORD n = GetEnvironmentVariableA("GTASA_ACCIONES", b, sizeof(b));
#ifdef GTASA_SKIN_VIGILANCIA_POR_DEFECTO
    const bool on = !(n > 0 && b[0] == '0');
#else
    const bool on = (n > 0 && b[0] != '0');
#endif
    if (!on) { return; }
    CreateDirectoryA("logs", nullptr);
    g_f = CreateFileA("logs\\acciones.jsonl", GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (g_f == INVALID_HANDLE_VALUE) { return; }
    SYSTEMTIME t; GetLocalTime(&t);
    std::string cmd = GetCommandLineA(); for (auto& c : cmd) { if (c == '"' || c == '\\') { c = '\''; } }
    Linea(0, std::format("{{\"k\":\"cab\",\"ver\":1,\"hora\":\"{:04}-{:02}-{:02} {:02}:{:02}:{:02}\",\"semilla\":{},\"linea\":\"{}\",\"nota\":\"t=reloj de juego en ms; w=reloj de pared (GetTickCount); solo cambios de entrada\"}}",
        t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, ExeCrt::Getptd()->holdrand, cmd));
    g_modo = Modo::Grabar;
    NOTSA_LOG_WARN("[ACCIONES] grabando acciones en logs\\acciones.jsonl");
}
} // namespace

void AccionesGrabadorFotograma() {
    if (!g_iniciado) { Iniciar(); }
    if (g_modo == Modo::Sin) { return; }
    CPad* const pad = CPad::GetPad(0);
    if (!pad) { return; }
    const uint32 t = CTimer::GetTimeInMS();
    if (g_modo == Modo::Repetir) {
        if (!g_repIniciada) { g_repIniciada = true; g_inicioRep = t; }
        const double rel = (t - g_inicioRep) * g_vel; // tiempo del guion transcurrido
        while (g_sig < g_ev.size() && (g_ev[g_sig].t - g_t0) <= rel) {
            const auto& e = g_ev[g_sig++];
            if (e.tipo == 0) { g_virt[e.idx] = e.v; g_virtUsado[e.idx] = true; }
            else { pad->NewMouseControllerState.m_AmountMoved = CVector2D{ e.dx, e.dy }; pad->NewMouseControllerState.isMouseLeftButtonPressed = (e.b & 1) ? 0x80 : 0; pad->NewMouseControllerState.isMouseRightButtonPressed = (e.b & 2) ? 0x80 : 0; }
        }
        if (g_sig >= g_ev.size() && rel > (g_finRep - g_t0) + 2000) { return; } // fin: el pad vuelve a ser el real
        auto* v = reinterpret_cast<int16*>(&pad->NewState);
        for (int i = 0; i < 24; ++i) { if (g_virtUsado[i]) { v[i] = g_virt[i]; } }
        return;
    }
    const DWORD w = GetTickCount();
    const auto* s = reinterpret_cast<const int16*>(&pad->NewState);
    for (int i = 0; i < 24; ++i) {
        const int16 nuevo = s[i];
        bool anotar = false;
        if (EsStick(i)) {
            anotar = std::abs(nuevo - g_ultimo[i]) >= kUmbralStick && (w - g_ultimoMs[i]) >= kMinMsStick;
            if (!anotar && std::abs(nuevo) < 300 && g_ultimo[i] != nuevo && std::abs(g_ultimo[i]) >= 300) { anotar = true; } // vuelta al centro (histeresis): siempre se anota
        } else {
            anotar = (nuevo != 0) != (g_ultimo[i] != 0);
        }
        if (anotar) {
            g_ultimo[i] = nuevo; g_ultimoMs[i] = w;
            Linea(t, std::format("{{\"k\":\"pad\",\"t\":{},\"w\":{},\"f\":{},\"n\":\"{}\",\"v\":{}}}", t, w, CTimer::GetFrameCounter(), kNombresPad[i], nuevo));
            if (i == 16 && (nuevo != 0) != false) { Linea(t, std::format("{{\"k\":\"accion\",\"t\":{},\"n\":\"acelerar_o_esprintar\",\"v\":{}}}", t, nuevo)); }
            else if (i == 14) { Linea(t, std::format("{{\"k\":\"accion\",\"t\":{},\"n\":\"{}\",\"v\":{}}}", t, nuevo ? "saltar_o_frenar_marcha_atras" : "soltar_square", nuevo)); }
            else if (i == 15 && nuevo) { Linea(t, std::format("{{\"k\":\"accion\",\"t\":{},\"n\":\"subir_o_bajar_vehiculo\"}}", t)); }
            else if (i == 0) { Linea(t, std::format("{{\"k\":\"accion\",\"t\":{},\"n\":\"girar_{}\",\"v\":{}}}", t, nuevo < 0 ? "izquierda" : "derecha", nuevo)); }
            else if (i == 12 && nuevo) { Linea(t, std::format("{{\"k\":\"accion\",\"t\":{},\"n\":\"menu_pausa\"}}", t)); }
        }
    }
    const auto& m = pad->NewMouseControllerState;
    const uint8 mb = (m.isMouseLeftButtonPressed ? 1 : 0) | (m.isMouseRightButtonPressed ? 2 : 0) | (m.isMouseMiddleButtonPressed ? 4 : 0);
    static DWORD ultRaton = 0;
    const bool mueve = (std::fabs(m.m_AmountMoved.x) + std::fabs(m.m_AmountMoved.y)) > 0.5f && (w - ultRaton) >= kMinMsStick;
    if (mb != g_ultimoRaton || mueve) {
        g_ultimoRaton = mb; if (mueve) { ultRaton = w; }
        Linea(t, std::format("{{\"k\":\"raton\",\"t\":{},\"w\":{},\"dx\":{:.2f},\"dy\":{:.2f},\"b\":{}}}", t, w, m.m_AmountMoved.x, m.m_AmountMoved.y, mb));
    }
    // acciones inferidas del estado del jugador y contexto por segundo (reloj de juego)
    if (auto* j = FindPlayerPed()) {
        const bool enVeh = j->bInVehicle && j->m_pVehicle;
        if (enVeh != g_enVeh) { g_enVeh = enVeh; Linea(t, std::format("{{\"k\":\"accion\",\"t\":{},\"n\":\"{}\"}}", t, enVeh ? "subio_vehiculo" : "bajo_vehiculo")); }
        if (j->m_nActiveWeaponSlot != g_arma) { g_arma = j->m_nActiveWeaponSlot; Linea(t, std::format("{{\"k\":\"accion\",\"t\":{},\"n\":\"arma\",\"v\":{}}}", t, g_arma)); }
        if (t - g_ultimoCtx >= 1000) {
            g_ultimoCtx = t;
            const CVector p = j->GetPosition();
            std::string anims;
            if (auto* clump = reinterpret_cast<RpClump*>(j->GetRwObject())) {
                int n = 0;
                for (auto* a = RpAnimBlendClumpGetFirstAssociation(clump); a && n < 6; a = RpAnimBlendGetNextAssociation(a), ++n) { anims += std::format("{}{}", n ? "," : "", static_cast<int>(a->GetAnimId())); }
            }
            const float rumbo = enVeh ? std::atan2(j->m_pVehicle->GetForward().y, j->m_pVehicle->GetForward().x) : j->m_fCurrentRotation;
            Linea(t, std::format("{{\"k\":\"ctx\",\"t\":{},\"w\":{},\"pos\":[{:.1f},{:.1f},{:.1f}],\"vel\":{:.3f},\"rumbo\":{:.2f},\"veh\":{},\"tipo\":{},\"estado\":{},\"anims\":[{}],\"cam\":{},\"zona\":\"{}\"}}",
                t, w, p.x, p.y, p.z, enVeh ? j->m_pVehicle->m_vecMoveSpeed.Magnitude() : j->m_vecMoveSpeed.Magnitude(), rumbo, enVeh ? static_cast<int>(j->m_pVehicle->m_nModelIndex) : -1,
                enVeh ? static_cast<int>(j->m_pVehicle->m_nVehicleType) : -1, static_cast<int>(j->m_nPedState), anims, static_cast<int>(TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode), CPad::GetPad(0)->Mode));
        }
    }
}

void AccionesGrabadorVolcarEvento() {
    if (g_modo != Modo::Grabar || g_eventos >= kTopeEventos || g_anillo.empty()) { return; }
    ++g_eventos;
    CreateDirectoryA("logs", nullptr);
    const std::string ruta = std::format("logs\\acciones_evento_{:02}.jsonl", g_eventos);
    HANDLE f = CreateFileA(ruta.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) { return; }
    const std::string cab = std::format("{{\"k\":\"cab\",\"ver\":1,\"nota\":\"ultimos 30 s de juego antes de una anomalia de skin (evento {}); sin semilla\"}}\n", g_eventos);
    DWORD w; WriteFile(f, cab.data(), static_cast<DWORD>(cab.size()), &w, nullptr);
    for (const auto& l : g_anillo) { WriteFile(f, l.second.data(), static_cast<DWORD>(l.second.size()), &w, nullptr); }
    CloseHandle(f);
}
