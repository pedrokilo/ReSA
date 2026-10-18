#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <vector>

// Fixtures deterministas del banco Dependencies24A (Win::__MainWndProc 0x747EB0). Solo datos: no dependen del juego ni del proyecto (las usa tambien el ensayo sin juego de offline/).
// 19 rutas (una por familia de mensajes) x 1024 casos. El caso (ruta, n) se deriva de una semilla fija; los selectores de rama usan `n` (reparto uniforme garantizado) y el resto, el generador.
namespace Dependencies24AFix {
using u8 = std::uint8_t; using u16 = std::uint16_t; using u32 = std::uint32_t; using u64 = std::uint64_t; using s32 = std::int32_t;

inline constexpr unsigned kRoutes = 19;
inline constexpr unsigned kCasesPerRoute = 1024;

enum Route : unsigned {
    RDestroy, RClose, RSize, RActivate, RSetFocus, RKillFocus, RActivateApp, RSetCursor, RKeyDown, RKeyUp,
    RMouseMove, RButtonDown, RButtonUp, RMouseWheel, RTimer, RSizing, RDeviceChange, RGraphNotify, RUnknown
};
inline constexpr const char* kRouteNames[kRoutes]{
    "WM_DESTROY", "WM_CLOSE", "WM_SIZE", "WM_ACTIVATE", "WM_SETFOCUS", "WM_KILLFOCUS", "WM_ACTIVATEAPP", "WM_SETCURSOR", "WM_KEYDOWN/WM_SYSKEYDOWN", "WM_KEYUP/WM_SYSKEYUP",
    "WM_MOUSEMOVE", "WM_xBUTTONDOWN", "WM_xBUTTONUP", "WM_MOUSEWHEEL", "WM_TIMER", "WM_SIZING", "WM_DEVICECHANGE", "WM_GRAPHNOTIFY", "desconocidos"
};
// Mensajes que maneja el EXE (tablas de salto 0x748464 / 0x74848c / 0x748584 y comparaciones sueltas)
inline bool IsHandledMessage(u32 m) {
    switch (m) {
    case 0x02: case 0x05: case 0x06: case 0x07: case 0x08: case 0x10: case 0x1C: case 0x20:
    case 0x100: case 0x101: case 0x104: case 0x105: case 0x113: case 0x200: case 0x201: case 0x202: case 0x204: case 0x205:
    case 0x207: case 0x208: case 0x20A: case 0x214: case 0x219: case 0x40D: return true;
    }
    return false;
}

struct Rng {
    u64 s;
    explicit Rng(u64 seed) : s(seed * 0x9E3779B97F4A7C15ull + 0x1234567ull) {}
    u64 Next() { u64 z = (s += 0x9E3779B97F4A7C15ull); z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull; z = (z ^ (z >> 27)) * 0x94D049BB133111EBull; return z ^ (z >> 31); }
    u32 U32() { return u32(Next() >> 16); }
    u32 Below(u32 n) { return n ? u32(Next() % n) : 0; }
    bool Chance(u32 num, u32 den) { return Below(den) < num; }
    u32 Pick(std::initializer_list<u32> v) { return *(v.begin() + Below(u32(v.size()))); }
};

// Contenido de los bufferes que cuelgan de lParam (se copian a la arena del banco)
struct Case {
    u32 route{}, n{};
    u32 hwnd{}, msg{}, wparam{}, lparam{};
    u32 lparamKind{};         // 0 = literal, 1 = RECT en la arena, 2 = DEV_BROADCAST_VOLUME en la arena
    u32 fsw{};
    // globales del EXE
    u32 rwInit{}, gameState{}, gammaChanged{}, windowedInit{}, foregroundInit{}, rejectedInit{}, frameLimit{};
    u32 psFullScreen{}, psWindow{};
    u32 menuMainSwitch{}, menuActive{}, menuActivateNext{}, quitNoDvd{};
    u32 mouseX{}, mouseY{};
    u32 hwInit{}, cutsceneRunning{}, codePause{};
    u32 mediaNull{}, deviceNull{};
    // comportamiento de los falsos
    u32 camMode{}, camW{}, camH{};                 // rsCAMERASIZE: 0 sin cambios, 1 solo ancho, 2 solo alto, 3 ambos
    u32 showCmd{};
    u32 translateRet{}, translateCk{};
    u32 getStateWrites{}, getStateValue{}, getStateHr{}, nullifyMedia{}, pauseHr{}, runHr{};
    u32 streaming{}, letter{};
    u32 checkMask{}, quitAt{};
    u32 defRet{}, rsRet{}, showRet{};
    // buffers
    s32 rect[4]{};
    u32 dev[5]{};            // size, devicetype, reserved, unitmask, flags(word) + basura en la palabra alta
};

inline constexpr u32 kFsw[]{0x0000, 0x0020, 0x003F, 0x0001, 0x0004, 0x4100, 0x0200, 0x0010};

inline Case MakeCase(unsigned route, u64 seed, unsigned n) {
    Rng r(seed);
    Case c;
    c.route = route; c.n = n;
    c.fsw = kFsw[(n >> 3) % std::size(kFsw)];
    c.hwnd = r.Chance(1, 8) ? r.Pick({0x00010001u, 0x000A0B0Cu, 0x7FFFFFF0u, 0xFFFFFFFFu}) : (r.U32() | 1u);
    c.wparam = r.U32(); c.lparam = r.U32();
    c.rwInit = r.Pick({0u, 0u, 1u, 1u, 2u, 0x100u, 0x10000u, 0x01000000u, 0xFF000000u, 0xFFFFFFFFu, 0x80000000u, r.U32()});
    c.gameState = r.Pick({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 0xFFFFFFFFu, 0x100u, 0x102u, 0x104u, 0x109u, 0x80000009u});
    c.gammaChanged = r.Pick({0u, 1u, 1u, 2u, 0xFFu});
    c.windowedInit = r.Pick({0u, 1u, 0x100u, 0xFFFFFFFFu, r.U32()});
    c.foregroundInit = r.Pick({0u, 1u, 0x100u, 0xFFFFFFFFu, r.U32()});
    c.rejectedInit = r.Pick({0u, 1u, 0x100u, 0xFFFFFFFFu, r.U32()});
    c.frameLimit = r.Pick({30u, 30u, 60u, 25u, 0u, 1u, 2u, 3u, 7u, 16u, 32u, 64u, 100u, 1000u, 1001u, 24u, 20u, 50u, 0xFFFFFFFFu, 0xFFFFFFFCu, 0xFFFFFC18u, 0x1000001u, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, r.U32() >> r.Below(32)});
    c.psFullScreen = r.Pick({0u, 1u, 2u, 0x100u, 0xFFFFFFFFu});
    c.psWindow = r.Chance(1, 4) ? 0u : (r.U32() | 1u);
    c.menuMainSwitch = r.Pick({0u, 0u, 1u, 2u, 0xFFu}); c.menuActive = r.Pick({0u, 0u, 1u, 2u, 0xFFu});
    c.menuActivateNext = r.Pick({0u, 1u, 2u}); c.quitNoDvd = r.Pick({0u, 0u, 0u, 1u, 2u, 0xFFu});
    c.mouseX = r.U32(); c.mouseY = r.U32();
    c.hwInit = r.Pick({0u, 1u, 1u, 2u, 0xFFu});
    c.cutsceneRunning = r.Pick({0u, 1u, 1u, 2u, 0xFFu, 0x7Fu, 0x80u});
    c.codePause = r.Pick({0u, 1u, 2u});
    c.mediaNull = r.Chance(1, 6); c.deviceNull = r.Chance(1, 6);
    c.camMode = n & 3; c.camW = r.U32(); c.camH = r.U32();
    c.showCmd = r.Pick({3u, 3u, 0u, 1u, 2u, 4u, 0xCCCCCCCCu, r.U32()});
    c.translateRet = (n >> 2) & 1; c.translateCk = r.Pick({0u, 1u, 0x1Cu, 0x420u, 0x416u, r.U32()});
    c.getStateWrites = r.Chance(3, 4); c.getStateValue = r.Pick({0u, 1u, 2u, 2u, 3u, 0x10002u, 0xFFFFFFFFu}); c.getStateHr = r.Pick({0u, 0u, 1u, 0x80004005u, r.U32()});
    c.nullifyMedia = r.Chance(1, 8);
    c.pauseHr = r.Pick({0u, 0u, 0u, 1u, 0x80004005u, 0xFFFFFFFFu, r.U32()}); c.runHr = r.Pick({0u, 0u, 0u, 1u, 0x80004005u, 0xFFFFFFFFu, r.U32()});
    c.streaming = r.Chance(3, 4); c.letter = r.Pick({0u, 0x40u, 0x41u, 0x42u, 0x43u, 0x5Au, 0x5Bu, 0x61u, 0x80u, 0xC8u, 0xFFu, 0x41u + 32u, 0x44u + 32u, 0x41u + 64u, r.U32() & 0xFFu});
    c.checkMask = r.Pick({0u, 1u, 2u, 4u, 8u, 0x10u, 0x20u, 0x80u, 0xFFu});
    c.quitAt = r.Pick({0u, 1u, 2u, 3u, 4u, 5u});
    c.defRet = r.U32(); c.rsRet = r.U32(); c.showRet = r.U32();
    for (auto& v : c.rect) v = s32(r.Pick({0u, 1u, 0xFFFFFFFFu, 640u, 480u, 800u, 0x80000000u, 0x7FFFFFFFu, r.U32(), r.U32() & 0xFFFu}));
    c.dev[0] = r.Pick({0x14u, 0u, r.U32()}); c.dev[1] = 2u; c.dev[2] = r.U32();
    c.dev[3] = r.U32(); c.dev[4] = (r.U32() & 0xFFFF0000u) | r.Pick({1u, 0u, 1u, 2u, 3u, 0x100u, 0x101u, 0xFFu});

    switch (route) {
    case RDestroy: c.msg = 0x02; break;
    case RClose: c.msg = 0x10; break;
    case RSize: {
        c.msg = 0x05;
        const u32 w = r.Pick({0u, 1u, 2u, 0x7FFFu, 0x8000u, 0xFFFFu, 640u, 800u, 1024u, r.U32() & 0xFFFFu});
        const u32 h = r.Pick({0u, 1u, 2u, 0x7FFFu, 0x8000u, 0xFFFFu, 480u, 600u, 768u, r.U32() & 0xFFFFu});
        c.lparam = (h << 16) | w;
        c.wparam = r.Pick({0u, 1u, 2u, 3u, r.U32()});
        c.rwInit = ((n >> 2) % 8 < 6) ? r.Pick({1u, 1u, 2u, 0x100u, 0xFF000000u, 0xFFFFFFFFu}) : 0u;
        c.showCmd = r.Pick({3u, 3u, 3u, 0u, 1u, 2u, 4u, 0xCCCCCCCCu});
        // tamano que devuelve rsCAMERASIZE: se fuerza que difiera (o no) segun el modo
        const u32 nw = (w + 1 + r.Below(5)) & 0xFFFFFFFFu, nh = (h + 1 + r.Below(5)) & 0xFFFFFFFFu;
        c.camW = (c.camMode & 1) ? (r.Chance(1, 4) ? r.U32() : nw) : w;
        c.camH = (c.camMode & 2) ? (r.Chance(1, 4) ? r.U32() : nh) : h;
        if ((c.camMode & 1) && c.camW == w) c.camW = w + 1;
        if ((c.camMode & 2) && c.camH == h) c.camH = h + 1;
        break;
    }
    case RActivate:
        c.msg = 0x06;
        c.wparam = r.Pick({0u, 0u, 1u, 2u, 0x10000u, 0x10001u, 0x10002u, 0x1FFFFu, 0xFFFF0000u, 0xFFFFu, 0x8000u, r.U32()});
        break;
    case RSetFocus: c.msg = 0x07; break;
    case RKillFocus: c.msg = 0x08; break;
    case RActivateApp:
        c.msg = 0x1C;
        // palabra baja cero (ventana inactiva) en ~40 % de los casos, incluidos los que solo tienen la palabra alta
        c.wparam = ((n >> 1) % 5 < 2) ? r.Pick({0u, 0u, 0u, 0x10000u, 0xFFFF0000u, 0x20000u}) : r.Pick({1u, 1u, 0x10001u, 2u, 0xFFFFu, 0x8000u, r.U32() | 1u});
        c.lparam = r.Pick({0u, 1u, 2u, 2u, 3u, 0xCCCCCCCCu, r.U32()});
        c.gameState = ((n >> 4) % 8 < 5) ? r.Pick({2u, 4u}) : r.Pick({0u, 0u, 1u, 3u, 5u, 9u, 0x102u, 0x104u, 0xFFFFFFFFu});
        if (r.Chance(1, 3)) c.gammaChanged = 1;
        c.getStateWrites = r.Chance(7, 8); c.getStateValue = r.Pick({2u, 2u, 2u, 0u, 1u, 3u, 0x10002u});
        c.getStateHr = r.Pick({0u, 0u, 0u, 1u, 0x80004005u});
        c.pauseHr = r.Pick({0u, 0u, 0u, 1u, 0x80004005u, 0xFFFFFFFFu}); c.runHr = r.Pick({0u, 0u, 0u, 1u, 0x80004005u, 0xFFFFFFFFu});
        c.nullifyMedia = r.Chance(1, 8);
        break;
    case RSetCursor: c.msg = 0x20; break;
    case RKeyDown: case RKeyUp: {
        const bool down = route == RKeyDown;
        c.msg = down ? ((n & 1) ? 0x104u : 0x100u) : ((n & 1) ? 0x105u : 0x101u);
        c.wparam = r.Chance(2, 5) ? 0x10u : r.Pick({0x11u, 0x41u, 0x1Bu, 0x0Du, 0x110u, 0xFFFFFF10u, 0x1000010u, 0u, 0xA0u, 0xA1u, r.U32()});
        break;
    }
    case RMouseMove:
        c.msg = 0x200;
        c.lparam = r.Pick({0u, 0x8000u, 0xFFFFu, 0x7FFF8000u, 0x80007FFFu, 0xFFFFFFFFu, 0x00010001u, r.U32(), r.U32()});
        break;
    case RButtonDown: c.msg = (n % 3) == 0 ? 0x201u : (n % 3) == 1 ? 0x204u : 0x207u; break;
    case RButtonUp: c.msg = (n % 3) == 0 ? 0x202u : (n % 3) == 1 ? 0x205u : 0x208u; break;
    case RMouseWheel: c.msg = 0x20A; break;
    case RTimer: c.msg = 0x113; c.wparam = r.Pick({1u, 0u, r.U32()}); c.lparam = r.Pick({0u, r.U32()}); break;
    case RSizing:
        c.msg = 0x214; c.lparamKind = 1;
        c.wparam = r.Pick({1u, 2u, 3u, 8u, r.U32()});
        if (r.Chance(1, 2)) c.rwInit = 1;
        if (r.Chance(1, 2)) c.gameState = 9;
        break;
    case RDeviceChange:
        c.msg = 0x219;
        c.wparam = (n % 5 == 4) ? r.Pick({0u, 0x8000u, 0x8003u, 0x8005u, 0x18004u, 0x80008004u, 7u, r.U32()}) : 0x8004u;
        c.lparamKind = (c.wparam == 0x8004u) ? 2 : 0;
        // el reparto de ramas sale de las capas n>>? : tipo de dispositivo, inicializacion, streaming, banderas, letra y mascara
        c.dev[1] = ((n >> 3) % 7 == 0) ? r.Pick({0u, 1u, 3u, 0x102u, 0xFFFFFFFFu}) : 2u;
        c.hwInit = ((n >> 4) % 7 == 0) ? r.Pick({0u, 2u, 0xFFu}) : 1u;
        c.streaming = ((n >> 5) % 6 == 0) ? 0u : 1u;
        c.dev[4] = ((n >> 6) % 6 == 0) ? ((r.U32() & 0xFFFE0000u) | r.Pick({0u, 2u, 0x100u, 0xFEu, 0x1000u})) : ((r.U32() & 0xFFFF0000u) | r.Pick({1u, 3u, 0x101u, 0xFFu}));
        if ((n >> 7) % 5 == 0) c.letter = r.Pick({0u, 0x40u, 0x80u, 0xFFu, 0x20u});
        else if ((n >> 7) % 5 == 1) c.letter = r.Pick({0x41u + 32u, 0x44u + 32u, 0x5Au + 64u, 0x41u + 96u});
        else c.letter = 0x41u + r.Below(26);
        {
            const u32 bit = 1u << ((c.letter - 0x41u) & 31);
            const bool want = ((n >> 9) & 1) == 0; // la mitad de los casos con el bit de la letra puesto
            c.dev[3] = want ? (c.dev[3] | bit) : (c.dev[3] & ~bit);
            if (!want && c.letter < 0x41u) c.dev[3] = r.U32();
        }
        c.cutsceneRunning = r.Pick({0u, 1u, 1u, 1u, 2u, 0xFFu});
        c.quitNoDvd = r.Pick({0u, 0u, 0u, 0u, 0u, 1u, 2u, 0xFFu});
        break;
    case RGraphNotify:
        c.msg = 0x40D;
        c.gameState = r.Pick({2u, 4u, 2u, 4u, 0u, 1u, 3u, 5u, 9u, 0x102u, 0x104u, 0xFFFFFFFFu, r.U32()});
        break;
    default: { // desconocidos: nunca un mensaje manejado
        for (;;) {
            u32 m;
            switch (r.Below(12)) {
            case 0: m = r.Below(0x30); break;
            case 1: m = r.Pick({0x01u, 0x03u, 0x04u, 0x09u, 0x0Fu, 0x11u, 0x12u, 0x1Bu, 0x1Du, 0x1Fu}); break;
            case 2: m = 0x21 + r.Below(0xDF); break;
            case 3: m = r.Pick({0x106u, 0x112u, 0x114u, 0x1FFu, 0x203u, 0x206u, 0x209u, 0x20Bu, 0x213u, 0x215u, 0x218u, 0x21Au}); break;
            case 4: m = 0x21A + r.Below(0x1F3); break;
            case 5: m = r.Pick({0x40Cu, 0x40Eu, 0x410u, 0x400u, 0x401u}); break;
            case 6: m = r.Pick({0x80000000u, 0xFFFFFFFFu, 0x80000219u, 0x7FFFFFFFu, 0x1000u, 0xC000u, 0xC001u}); break;
            case 7: m = 0x100 + r.Below(0x20); break;
            case 8: m = 0x200 + r.Below(0x30); break;
            case 9: m = r.Pick({0x00000102u, 0x00010005u, 0x0001000Du, 0x00000514u, 0x01000219u, 0x0000040Du | 0x10000u}); break;
            default: m = r.U32(); break;
            }
            if (!IsHandledMessage(m)) { c.msg = m; break; }
        }
        break;
    }
    }
    // Dominio legal: en WM_ACTIVATEAPP con GAME_STATE_PLAYING_LOGO/INTRO el EXE llama a GetState SIN comprobar que el control multimedia exista (0x7480D6: mov ecx, [eax]); el juego solo
    // llega ahi con el reproductor creado. Las comprobaciones de nulo posteriores (Pause/Run) si se ejercitan, con nullifyMedia (el falso GetState borra el puntero).
    if (c.gameState == 2 || c.gameState == 4) c.mediaNull = 0;
    return c;
}
} // namespace Dependencies24AFix
