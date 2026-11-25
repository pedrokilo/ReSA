/*
 * Iconos del mando en ayudas y textos (NOTSA). Ver PadButtonIcons.h y docs/CodigoVivoExe/iconos-mando.md.
 * Idea, juegos de iconos y tabla de sustituciones de textos de GInputSA 1.11 de Silent (la tabla de la seccion "Sustituciones" se leyo del analisis estatico de
 * GInputSA.asi; el codigo es propio; las texturas no se redistribuyen: se cargan de la instalacion del usuario).
 */
#include "StdInc.h"

#include <filesystem>
#include <regex>
#include <unordered_map>

#include "PadButtonIcons.h"
#include "extensions/SilentPatchConfig.h"
#include "Font.h"
#include "Sprite2d.h"
#include "TxdStore.h"
#include "Pad.h"
#include "Hud.h"
#include "Timer.h"
#include "MenuManager.h"
#include "Messages.h"
#include "Text/Text.h"

namespace PadButtonIcons {
namespace {
// Boton del mando del juego (numeracion de CControllerConfigManager: 1 circulo, 2 cruz, 3 cuadrado, 4 triangulo, 5 L2, 6 R2, 7 L1, 8 R1, 9 select, 10 L3, 11 R3, 12 start,
// 13 arriba, 14 derecha, 15 abajo, 16 izquierda) -> etiqueta de simbolo de CFont::ParseToken, simbolo y nombre de la textura del TXD. Select y Start no tienen icono.
struct IconDef {
    uint8             joyButton;
    char              token;
    eExtraFontSymbol  symbol;
    const char*       texture;
};
constexpr IconDef kIcons[] = {
    { 1,  'O', EXSYMBOL_CIRCLE,     "circle"   },
    { 2,  'X', EXSYMBOL_CROSS,      "cross"    },
    { 3,  'Q', EXSYMBOL_SQUARE,     "square"   },
    { 4,  'T', EXSYMBOL_TRIANGLE,   "triangle" },
    { 5,  'M', EXSYMBOL_L2,         "l2"       },
    { 6,  'V', EXSYMBOL_R2,         "r2"       },
    { 7,  'K', EXSYMBOL_KEY,        "l1"       }, // ~k~ de PS2 es L1 (EXSYMBOL_KEY)
    { 8,  'J', EXSYMBOL_R1,         "r1"       },
    { 10, 'A', EXSYMBOL_L3,         "l3"       },
    { 11, 'C', EXSYMBOL_R3,         "r3"       },
    { 13, 'U', EXSYMBOL_DPAD_UP,    "up"       },
    { 14, '>', EXSYMBOL_DPAD_RIGHT, "right"    },
    { 15, 'D', EXSYMBOL_DPAD_DOWN,  "down"     },
    { 16, '<', EXSYMBOL_DPAD_LEFT,  "left"     },
};
constexpr size_t kIconCount = std::size(kIcons);

// Iconos de sticks y cruceta de dos botones (etiquetas propias de GInputSA): `~[~` stick izquierdo, `~]~` derecho, `~[x~` `~[y~` ejes, `~[<~ ~[>~ ~[u~ ~[d~` direcciones del izquierdo,
// `~]..~` las del derecho, `~ud~` cruceta arriba/abajo, `~<>~` izquierda/derecha. El nombre de la textura es el del TXD; la correspondencia etiqueta -> textura se deduce de esos nombres.
struct ExtDef {
    const char* token;
    const char* texture;
};
constexpr ExtDef kExt[] = {
    { "[", "thumbl" }, { "]", "thumbr" }, { "[x", "thumblx" }, { "[y", "thumbly" }, { "]x", "thumbrx" }, { "]y", "thumbry" },
    { "[<", "thumblxl" }, { "[>", "thumblxr" }, { "[u", "thumblyu" }, { "[d", "thumblyd" },
    { "]<", "thumbrxl" }, { "]>", "thumbrxr" }, { "]u", "thumbryu" }, { "]d", "thumbryd" },
    { "ud", "dud" }, { "<>", "dlr" },
};
constexpr size_t kExtCount    = std::size(kExt);
constexpr int    kExtFirstSym = 15; // los simbolos 1..14 son los de CFont::ButtonSprite
CSprite2d        g_extSprite[kExtCount];
bool             g_extAvail[2][kExtCount]{};

enum Set { PS = 0, XBOX = 1 };
struct SetState {
    bool  tried{};
    bool  ok{};
    int32 slot{-1};
    bool  avail[kIconCount]{};
};
SetState    g_sets[2];
const char* const kSetFile[2] = { "models\\ps3btns.txd", "models\\x360btns.txd" };
const char* const kSetSlot[2] = { "padbtn_ps", "padbtn_xbox" };
int         g_applied = -1; // juego de iconos cuyas texturas estan ahora en CFont::ButtonSprite (-1 = ninguno: ButtonSprite intacto como en el EXE)

// ---- ultimo dispositivo usado
enum class Last { Unknown, Pad, Keyboard };
Last      g_last = Last::Unknown;
ULONGLONG g_padSeen{};

bool OptionIcons() {
    static const bool v = OwnDeviationOption("GTASA_PAD_ICONS", "PadIcons", true);
    return v;
}

enum class Choice { Auto, PS, Xbox };
Choice OptionButtons() {
    static const Choice v = [] {
        const auto s = OwnDeviationString("GTASA_PAD_BUTTONS", "PadButtons", "ps");
        SPDLOG_INFO("Desviacion propia: opcion PadButtons = {}", s);
        return s == "xbox" ? Choice::Xbox : s == "auto" ? Choice::Auto : Choice::PS;
    }();
    return v;
}

// auto: Xbox si el mando es de Microsoft (el XInput del juego se anuncia como 0x045E:0x028E) o su nombre de Windows contiene "xbox"; si no, PlayStation.
Set SelectSet() {
    switch (OptionButtons()) {
    case Choice::Xbox: return XBOX;
    case Choice::PS:   return PS;
    default:           break;
    }
    static Set cached = PS;
    static ULONGLONG nextCheck{};
    const auto now = GetTickCount64();
    if (now >= nextCheck) {
        nextCheck = now + 2000;
        bool xbox = AllValidWinJoys.JoyStickNum[PAD1].wVendorID == 0x045E;
        JOYCAPSA caps{};
        if (!xbox && joyGetDevCapsA(0, &caps, sizeof(caps)) == JOYERR_NOERROR) {
            std::string name{caps.szPname};
            std::ranges::transform(name, name.begin(), [](unsigned char c) { return (char)std::tolower(c); });
            xbox = name.find("xbox") != std::string::npos || name.find("xinput") != std::string::npos;
        }
        cached = xbox ? XBOX : PS;
    }
    return cached;
}

// Carga (una vez) el TXD del juego de iconos. Sin archivo no se llama a LoadTxd (con un archivo ausente el bucle de CTxdStore::LoadTxd no termina).
bool LoadSet(Set which) {
    auto& st = g_sets[which];
    if (st.tried) {
        return st.ok;
    }
    st.tried = true;
    std::error_code ec;
    if (!std::filesystem::exists(kSetFile[which], ec)) {
        SPDLOG_INFO("Iconos del mando: no existe {} (ver docs/CodigoVivoExe/iconos-mando.md); los textos siguen con nombres de tecla", kSetFile[which]);
        return false;
    }
    const int32 slot = CTxdStore::AddTxdSlot(kSetSlot[which]);
    if (!CTxdStore::LoadTxd(slot, kSetFile[which])) {
        SPDLOG_WARN("Iconos del mando: {} no es un TXD valido", kSetFile[which]);
        CTxdStore::SafeRemoveTxdSlot(kSetSlot[which]);
        return false;
    }
    CTxdStore::AddRef(slot);
    st.slot = slot;
    auto* const dict = CTxdStore::GetTxd(slot);
    size_t n = 0;
    for (size_t i = 0; i < kIconCount; i++) {
        st.avail[i] = dict && RwTexDictionaryFindNamedTexture(dict, kIcons[i].texture) != nullptr;
        n += st.avail[i] ? 1 : 0;
    }
    for (size_t i = 0; i < kExtCount; i++) {
        g_extAvail[which][i] = dict && RwTexDictionaryFindNamedTexture(dict, kExt[i].texture) != nullptr;
    }
    st.ok = n > 0;
    SPDLOG_INFO("Iconos del mando: cargado {} ({} de {} iconos)", kSetFile[which], n, kIconCount);
    return st.ok;
}

// Pone en CFont::ButtonSprite las texturas del juego de iconos. Para cada simbolo: si la textura falta se deja el sprite como estaba.
void Apply(Set which) {
    if (g_applied == (int)which) {
        return;
    }
    auto& st = g_sets[which];
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(st.slot);
    for (size_t i = 0; i < kIconCount; i++) {
        if (!st.avail[i]) {
            continue;
        }
        auto& sprite = CFont::ButtonSprite[kIcons[i].symbol];
        sprite.SetTexture(kIcons[i].texture); // SetTexture(nombre) libera la textura anterior (la original de ps2btns en 1..4) y toma una referencia a la del TXD
        sprite.SetAddressingUV(rwTEXTUREADDRESSCLAMP, rwTEXTUREADDRESSCLAMP);
    }
    for (size_t i = 0; i < kExtCount; i++) {
        g_extSprite[i].Delete();
        if (g_extAvail[which][i]) {
            g_extSprite[i].SetTexture(kExt[i].texture);
            g_extSprite[i].SetAddressingUV(rwTEXTUREADDRESSCLAMP, rwTEXTUREADDRESSCLAMP);
        }
    }
    CTxdStore::PopCurrentTxd();
    g_applied = (int)which;
}

const IconDef* IconForButton(uint32 button) {
    for (const auto& d : kIcons) {
        if (d.joyButton == button) {
            return &d;
        }
    }
    return nullptr;
}

// Muestra de textos con teclas (solo pruebas): GTASA_PAD_ICONS_DEMO=1 pone cada 8 s una ayuda con etiquetas `~k~~ACCION~` (el juego las resuelve igual que las de las misiones).
void Demo() {
    static const bool demo = [] {
        char b[8]{};
        return GetEnvironmentVariableA("GTASA_PAD_ICONS_DEMO", b, sizeof(b)) && (b[0] == '1' || b[0] == '2');
    }();
    if (!demo || FrontEndMenuManager.m_bMenuActive || CTimer::GetTimeInMS() < 20000) {
        return;
    }
    static ULONGLONG next{};
    static int       idx{};
    const auto       now = GetTickCount64();
    if (now < next) {
        return;
    }
    next = now + 8000;
    static const char* const kText[] = {
        "Press ~k~~PED_ANSWER_PHONE~ for the phone.",
        "~k~~PED_SPRINT~ run, ~k~~PED_JUMPING~ jump.",
        "Radio ~k~~VEHICLE_RADIO_STATION_UP~ ~k~~VEHICLE_ENTER_EXIT~.",
        "~[~ ~]~ ~[x~ ~]y~ ~ud~ ~<>~ ~u~ ~<~ end",
        "@FET_MIG", // clave del GXT con sustitucion de GInputSA (~<>~ - ADJUST)
        "@FEH_BPO", // (~x~ - BACK)
    };
    GxtChar text[400]{};
    static const bool plain = [] { char b[8]{}; return GetEnvironmentVariableA("GTASA_PAD_ICONS_DEMO", b, sizeof(b)) && b[0] == '2'; }(); // =2: texto sin etiquetas (aislar fallos ajenos a los iconos)
    const char* const entry = plain ? "Plain help text without any key token." : kText[idx++ % std::size(kText)];
    if (entry[0] == '@') {
        CMessages::StringCopy(text, TheText.Get(entry + 1), sizeof(text));
    } else {
        AsciiToGxtChar(entry, text);
    }
    CHud::SetHelpMessage(text, false, false, false);
    SPDLOG_INFO("PadIcons demo: ayuda = {}", reinterpret_cast<const char*>(CHud::m_pHelpMessage)); // texto ya resuelto (con las etiquetas de icono): lo lee la sonda
}
} // namespace

void NoteInput(const DIJOYSTATE2& joy, bool /*usingXInput*/) {
    const auto now = GetTickCount64();
    g_padSeen      = now;

    // Mando: cualquier boton, la cruceta (POV) o un stick a mas de la mitad del rango (+-2000 que fija el juego).
    bool pad = LOWORD(joy.rgdwPOV[0]) != 0xFFFF;
    for (const auto b : joy.rgbButtons) {
        pad = pad || (b & 0x80) != 0;
    }
    constexpr LONG kStick = 1000;
    pad = pad || std::abs(joy.lX) > kStick || std::abs(joy.lY) > kStick || std::abs(joy.lZ) > kStick || std::abs(joy.lRz) > kStick;

    // Teclado/raton: cualquier tecla menos ESC y Enter (Start del mando se convierte en ESC) o movimiento / boton del raton.
    auto keys  = CPad::NewKeyState;
    keys.esc   = 0;
    keys.enter = 0;
    keys.extenter = 0;
    bool kb = false;
    const auto* raw = reinterpret_cast<const int16*>(&keys);
    for (size_t i = 0; i < sizeof(keys) / sizeof(int16) && !kb; i++) {
        kb = raw[i] != 0;
    }
    const auto& mouse = CPad::NewMouseControllerState;
    kb = kb || std::abs(mouse.m_AmountMoved.x) + std::abs(mouse.m_AmountMoved.y) >= 4.0f || mouse.isMouseLeftButtonPressed || mouse.isMouseRightButtonPressed;

    if (pad) {
        g_last = Last::Pad;
    } else if (kb) {
        g_last = Last::Keyboard;
    }
    Demo();
}

bool PadModeActive() {
    if (!OptionIcons()) {
        return false;
    }
    if (GetTickCount64() - g_padSeen > 1500) { // sin mando conectado ProcessPad no llega aqui
        return false;
    }
    if (g_last == Last::Unknown) { // aun sin actividad: el metodo de control del jugador decide, como en el EXE
        return FrontEndMenuManager.m_ControlMethod == eController::JOYPAD;
    }
    return g_last == Last::Pad;
}

bool WriteToken(eControllerAction action, GxtChar* dst, uint16 maxLen) {
    if (!PadModeActive() || maxLen < 4 || +action < 0 || +action >= +eControllerAction::CONTROLLER_ACTION_COUNT) {
        return false;
    }
    const auto button = ControlsManager.GetControllerKeyAssociatedWithAction(action, eControllerType::JOY_STICK);
    const auto* def   = IconForButton(button);
    if (!def) {
        return false; // select, start, sin asignar o combinaciones: nombre en texto
    }
    const Set which = SelectSet();
    if (!LoadSet(which)) {
        return false;
    }
    const size_t index = def - kIcons;
    if (!g_sets[which].avail[index]) {
        return false;
    }
    Apply(which);
    dst[0] = '~';
    dst[1] = (GxtChar)def->token;
    dst[2] = '~';
    dst[3] = 0;
    return true;
}

int ExtendedSymbol(const char* tag) {
    if (g_applied < 0) {
        return 0;
    }
    for (size_t i = 0; i < kExtCount; i++) {
        const size_t len = std::strlen(kExt[i].token);
        if (g_extAvail[g_applied][i] && std::strncmp(tag, kExt[i].token, len) == 0 && tag[len] == '~') {
            return kExtFirstSym + (int)i;
        }
    }
    return 0;
}

CSprite2d& SpriteFor(int symbol) {
    if (symbol >= kExtFirstSym && symbol < kExtFirstSym + (int)kExtCount) {
        return g_extSprite[symbol - kExtFirstSym];
    }
    return CFont::ButtonSprite[symbol];
}

float SymbolAdvance(const char* tag) {
    if (g_applied < 0) {
        return 0.0f;
    }
    bool icon = ExtendedSymbol(tag) != 0;
    for (size_t i = 0; !icon && i < kIconCount; i++) {
        icon = g_sets[g_applied].avail[i] && tag[1] == '~' && std::toupper((unsigned char)tag[0]) == std::toupper((unsigned char)kIcons[i].token);
    }
    // El mismo avance que RenderFontBuffer: alto de la fuente * 17 + contorno
    return icon ? CFont::m_Scale.y * 17.0f + float(static_cast<int8>(CFont::m_nFontOutline)) : 0.0f;
}

// ---- Sustituciones de textos del GXT (tabla de GInputSA: clave -> expresion regular -> texto; sin expresion, texto completo). Se aplican solo con el mando como dispositivo y los
// iconos cargados: p. ej. "~k~~GO_LEFT~.+~k~~GO_RIGHT~" (dos teclas) -> el icono del stick; "~k~~PED_FIREWEAPON~" -> circulo. `~@NOMBRE~` es una accion propia de GInputSA que se
// resuelve a iconos al aplicar la sustitucion.
namespace {
struct GxtFix {
    const char* key;
    const char* pattern;
    const char* replacement;
};
constexpr GxtFix kGxtFixes[] = {
    { "HELPMO", "~k~~GO_FORWARD~.+~k~~GO_BACK~.+~k~~GO_LEFT~.+~k~~GO_RIGHT~", "~@PED_MOVE~" },
    { "HELP27", "~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~", "~@GO_UPDOWN~" },
    { "INTRO2C", "~k~~VEHICLE_ACCELERATE~", "~x~" },
    { "INTRO2F", "~k~~VEHICLE_ACCELERATE~", "~x~" },
    { "IE20", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "IE21", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "JCK_HLP", "~k~~PED_FIREWEAPON~", "~o~" },
    { "MEL_HLP", "~k~~PED_FIREWEAPON~", "~o~" },
    { "MODH1", "~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~", "~ud~" },
    { "MODH2", "~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~", "~ud~" },
    { "MODH3", "~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~", "~ud~" },
    { "MODH5", "~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~", "~ud~" },
    { "NITROH1", "~k~~PED_FIREWEAPON~", "~o~" },
    { "AMMU_H", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "AMMU_H2", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "BARB_H1", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "BJ_H1", "~k~~PED_FIREWEAPON~", "~o~" },
    { "BJ_H2", "~k~~PED_FIREWEAPON~", "~o~" },
    { "BJ_H3", "~k~~PED_FIREWEAPON~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~", "~o~$1~v~" },
    { "BJ_H5", "~k~~PED_FIREWEAPON~", "~o~" },
    { "CINCAM", "~k~~VEHICLE_TURRETLEFT~", "~@CAMERA_LEFT_RIGHT~" },
    { "CLOTHA", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "CLOTHB", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "CONS_1", "~k~~GO_LEFT~.+~k~~GO_RIGHT~", "~[x~" },
    { "CONS_1B", "~k~~GO_LEFT~.+~k~~GO_RIGHT~", "~[x~" },
    { "CONS_2", "~k~~GO_BACK~(.+)~k~~GO_FORWARD~", "~[u~$1~[d~" },
    { "CONS_2B", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~[y~" },
    { "FOOD_H", "~k~~GO_LEFT~.+~k~~GO_RIGHT~", "~<>~" },
    { "FOOD_H2", "~k~~GO_LEFT~.+~k~~GO_RIGHT~", "~<>~" },
    { "GA_6", "~k~~PED_FIREWEAPON~", "~o~" },
    { "GA_7", "~k~~PED_FIREWEAPON~", "~o~" },
    { "GF_0020", "~k~~GO_FORWARD~.+~k~~GO_BACK~(.+)~k~~PED_FIREWEAPON~", "~[y~$1~o~" },
    { "GF_0043", "~k~~PED_FIREWEAPON~", "~o~" },
    { "GF_RAD", "~k~~VEHICLE_RADIO_STATION_UP~.+~k~~VEHICLE_RADIO_STATION_DOWN~", "~@VEHICLE_CHANGE_RADIO_STATION~" },
    { "GYM1_36", "~k~~PED_FIREWEAPON~", "~o~" },
    { "GYM1_37", "~k~~PED_FIREWEAPON~", "~o~" },
    { "GYM1_41", "~k~~CONVERSATION_NO~.+~k~~CONVERSATION_YES~", "~<>~" },
    { "GYM1_43", "~k~~CONVERSATION_NO~.+~k~~CONVERSATION_YES~", "~<>~" },
    { "GYM1_60", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "GYM1_71", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "HELP32", "~k~~PED_FIREWEAPON~", "~o~" },
    { "QUAR_1", "~k~~PED_FIREWEAPON~", "~o~" },
    { "REMOH1", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "ROUHELP", "~k~~PED_FIREWEAPON~", "~o~" },
    { "SKIP_1", "~k~~CONVERSATION_YES~", "~>~" },
    { "STR_02", "~k~~PED_FIREWEAPON~", "~o~" },
    { "TATTA", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "TATTB", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "TRAINS", "~k~~VEHICLE_ACCELERATE~", "~x~" },
    { "WARDH1", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "WARDH2", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "WARDH3", "~k~~GO_FORWARD~.+~k~~GO_BACK~", "~ud~" },
    { "WINCH_V", "~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~(.+)~k~~PED_FIREWEAPON~", "~@CAMERA_UP_DOWN~$1~o~" },
    { "WOFHD01", "~k~~PED_FIREWEAPON~", "~o~" },
    { "WZI2_A5", "~k~~PED_FIREWEAPON~", "~o~" },
    { "BCOU_84", "~k~~PED_FIREWEAPON~", "~o~" },
    { "BCOU_9", "~k~~PED_FIREWEAPON~", "~o~" },
    { "BOAT_72", "~<~~>~", "~<>~" },
    { "BOAT_B4", "~k~~VEHICLE_STEERLEFT~ and ~k~~VEHICLE_STEERRIGHT~", "~@GO_LEFTRIGHT~" },
    { "BS_X03", "~<~~>~", "~<>~" },
    { "CAS9_H2", "~k~~PED_CYCLE_WEAPON_LEFT~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~", "~k~~PED_CYCLE_TARGET_LEFT~$1~k~~PED_CYCLE_TARGET_RIGHT~" },
    { "CAT4_45", "~k~~PED_FIREWEAPON~", "~o~" },
    { "CSH3_52", "~k~~GO_LEFT~.+~k~~GO_RIGHT~", "~@GO_LEFTRIGHT~" },
    { "DS1_33", "~k~~VEHICLE_STEERUP~.+~k~~VEHICLE_STEERDOWN~", "~@GO_UPDOWN~" },
    { "DS1_60", "~k~~GO_LEFT~", "~<~" },
    { "DS1_61", "~k~~GO_RIGHT~", "~>~" },
    { "DS1_62", "~<~~>~", "~<>~" },
    { "DES5_9B", "~k~~PED_FIREWEAPON~", "~o~" },
    { "DES5_CB", "~k~~GO_LEFT~.+~k~~GO_RIGHT~", "~@GO_LEFTRIGHT~" },
    { "DES5_CC", "~k~~PED_FIREWEAPON~", "~o~" },
    { "DES5_CE", "~k~~GO_LEFT~.+~k~~GO_RIGHT~", "~@GO_LEFTRIGHT~" },
    { "DES5_CI", "~k~~GO_LEFT~.+~k~~GO_RIGHT~", "~@GO_LEFTRIGHT~" },
    { "DES5_FK", "~<~~>~", "~<>~" },
    { "DES5_FI", "~k~~GO_LEFT~", "~<~" },
    { "DES5_FJ", "~k~~GO_RIGHT~", "~>~" },
    { "DST8_30", "~k~~GO_RIGHT~", "~k~~PED_CYCLE_TARGET_RIGHT~" },
    { "DST8_31", "~k~~GO_LEFT~", "~k~~PED_CYCLE_TARGET_LEFT~" },
    { "DST8_32", "~k~~PED_CYCLE_WEAPON_LEFT~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~", "~k~~PED_CYCLE_TARGET_LEFT~$1~k~~PED_CYCLE_TARGET_RIGHT~" },
    { "DST8_37", "~k~~PED_FIREWEAPON~", "~o~" },
    { "SPAC_10", "~<~~u~~d~~>~", "~[~" },
    { "SPAC_11", "~<~~>~(.+)~k~~PED_FIREWEAPON~", "~[x~$1~o~" },
    { "SPAC_13", "~u~~d~", "~[y~" },
    { "DRV3_3", "~k~~VEHICLE_FIREWEAPON_ALT~", "~@SNATCH_PACKAGE~" },
    { "DRV3_H", "~k~~VEHICLE_FIREWEAPON_ALT~", "~@SNATCH_PACKAGE~" },
    { "SPRAY_1", "~k~~PED_FIREWEAPON~.+~k~~VEHICLE_FIREWEAPON_ALT~", "~o~" },
    { "HEI2_11", "~k~~PED_FIREWEAPON~", "~o~" },
    { "HEI3_25", "~k~~PED_FIREWEAPON~", "~o~" },
    { "HM9_24", "~k~~PED_FIREWEAPON~", "~o~" },
    { "HM9_49", "~k~~PED_FIREWEAPON~", "~o~" },
    { "INTRO2D", "~k~~VEHICLE_FIREWEAPON_ALT~", "~@BMX_BUNNYHOP~" },
    { "LOW_37", "~k~~PED_FIREWEAPON~", "~o~" },
    { "HYDHLP1", "~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~", "~@CAMERA_UP_DOWN~" },
    { "HYDHLP2", "~k~~VEHICLE_HANDBRAKE~", "~@HYDRA_TARGET~" },
    { "HYDHLP3", "~k~~VEHICLE_HANDBRAKE~", "~@HYDRA_TARGET~" },
    { "MAN2_70", "~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~", "~@CAMERA_UP_DOWN~" },
    { "MAN2_71", "~k~~VEHICLE_HANDBRAKE~", "~@HYDRA_TARGET~" },
    { "MAN2_98", "~k~~PED_FIREWEAPON~", "~o~" },
    { "PL_H1", "~k~~PED_FIREWEAPON~", "~o~" },
    { "QUAR_H1", "~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~", "~@CAMERA_UP_DOWN~" },
    { "RACES33", "~k~~GO_LEFT~.+~k~~GO_RIGHT~", "~<>~" },
    { "RM4_32", "~k~~PED_FIREWEAPON~", "~o~" },
    { "RUN_XX", "~k~~GO_LEFT~.+~k~~GO_RIGHT~", "~<>~" },
    { "RY2_68", "~k~~VEHICLE_TURRETUP~.+~k~~VEHICLE_TURRETDOWN~", "~@CAMERA_UP_DOWN~" },
    { "STE2_03", "~k~~PED_FIREWEAPON~", "~o~" },
    { "ST4_H03", "~k~~PED_FIREWEAPON~", "~o~" },
    { "STE5_03", "~k~~PED_FIREWEAPON~", "~o~" },
    { "STP2_10", "~k~~PED_FIREWEAPON~", "~o~" },
    { "STP2_11", "~k~~PED_FIREWEAPON~", "~o~" },
    { "HOOD2B", "~k~~PED_CYCLE_WEAPON_LEFT~(.+)~k~~PED_CYCLE_WEAPON_RIGHT~", "~k~~PED_CYCLE_TARGET_LEFT~$1~k~~PED_CYCLE_TARGET_RIGHT~" },
    { "S6_37", "~k~~PED_FIREWEAPON~", "~o~" },
    { "SYN7_20", "~k~~PED_FIREWEAPON~", "~o~" },
    { "ZER2_3", "~k~~VEHICLE_STEERDOWN~.+~k~~VEHICLE_STEERUP~", "~@GO_UPDOWN~" },
    { "ZER2_4", "~k~~PED_FIREWEAPON~", "~o~" },
    { "Z4_H13", "~k~~PED_FIREWEAPON~", "~o~" },
    { "Z4_H14", "~k~~PED_FIREWEAPON~", "~o~" },
    { "FEH_MPB", nullptr, "~ud~ - MOVE UP/DOWN~n~~x~ - TOGGLE OPTION" },
    { "FET_CIG", nullptr, "BACKSPACE - CLEAR~n~~x~ - CHANGE" },
    { "FET_APP", nullptr, "~x~ - APPLY NEW SETTING" },
    { "FEH_SNC", nullptr, "~x~ - SCAN USER TRACKS" },
    { "FEH_JMP", nullptr, "~x~ - ENTER MENU" },
    { "FEH_BPO", nullptr, "~x~ - BACK" },
    { "FET_MIG", nullptr, "~<>~ - ADJUST" },
    { "FEH_SSA", nullptr, "~ud~ - MOVE~n~S - SAVE TO FILE" },
    { "RYD1_93", nullptr, "When wearing the ski mask gently push ~@PED_MOVE~ to sneak." },
};

// `~@NOMBRE~`: PED_MOVE y los ejes de stick son iconos de stick; el resto de acciones propias (BMX, gancho de paquete, objetivo del Hydra, cambio de emisora) se resuelven al/los botones
// que el jugador tenga asignados a la accion equivalente del juego (decision propia: el analisis de GInputSA.asi no se ha cotejado con ellas).
std::string ExpandAlias(const std::string& name) {
    static const std::unordered_map<std::string, const char*> fixed = {
        { "PED_MOVE", "~[~" }, { "GO_LEFTRIGHT", "~[x~" }, { "GO_UPDOWN", "~[y~" }, { "CAMERA_LEFT_RIGHT", "~]x~" }, { "CAMERA_UP_DOWN", "~]y~" },
        { "BMX_HANDBRAKE", "~k~~VEHICLE_HANDBRAKE~" }, { "HYDRA_TARGET", "~k~~VEHICLE_HANDBRAKE~" },
        { "BMX_BUNNYHOP", "~k~~VEHICLE_FIREWEAPON_ALT~" }, { "SNATCH_PACKAGE", "~k~~VEHICLE_FIREWEAPON_ALT~" },
    };
    if (const auto it = fixed.find(name); it != fixed.end()) {
        return it->second;
    }
    if (name == "VEHICLE_CHANGE_RADIO_STATION") {
        const auto up   = ControlsManager.GetControllerKeyAssociatedWithAction(eControllerAction::VEHICLE_RADIO_STATION_UP, eControllerType::JOY_STICK);
        const auto down = ControlsManager.GetControllerKeyAssociatedWithAction(eControllerAction::VEHICLE_RADIO_STATION_DOWN, eControllerType::JOY_STICK);
        if ((up == 13 && down == 15) || (up == 15 && down == 13)) {
            return "~ud~";
        }
        if ((up == 14 && down == 16) || (up == 16 && down == 14)) {
            return "~<>~";
        }
        return "~k~~VEHICLE_RADIO_STATION_UP~ ~k~~VEHICLE_RADIO_STATION_DOWN~";
    }
    return {};
}
} // namespace

const GxtChar* ApplyGxtFix(const char* key, const GxtChar* original) {
    if (!original || !PadModeActive()) {
        return original;
    }
    static const std::unordered_map<std::string, const GxtFix*> byKey = [] {
        std::unordered_map<std::string, const GxtFix*> m;
        for (const auto& f : kGxtFixes) {
            m.emplace(f.key, &f);
        }
        return m;
    }();
    const auto it = byKey.find(key);
    if (it == byKey.end()) {
        return original;
    }
    const Set which = SelectSet();
    if (!LoadSet(which)) {
        return original;
    }
    Apply(which);

    const auto& fix = *it->second;
    std::string result;
    const std::string text{reinterpret_cast<const char*>(original)};
    if (!fix.pattern) {
        result = fix.replacement;
    } else {
        static std::unordered_map<std::string, std::regex> regexes; // las expresiones se compilan una vez
        auto rit = regexes.find(fix.pattern);
        if (rit == regexes.end()) {
            rit = regexes.emplace(fix.pattern, std::regex(fix.pattern)).first;
        }
        if (!std::regex_search(text, rit->second)) {
            return original; // el texto de este idioma no contiene la secuencia que sustituye GInputSA
        }
        result = std::regex_replace(text, rit->second, fix.replacement);
    }
    for (size_t p = result.find("~@"); p != std::string::npos; p = result.find("~@", p)) {
        const auto e = result.find('~', p + 2);
        if (e == std::string::npos) {
            break;
        }
        result.replace(p, e - p + 1, ExpandAlias(result.substr(p + 2, e - p - 2)));
    }
    // El puntero devuelto debe seguir valido mientras el llamador lo use: un bufer por clave que solo se reescribe si cambia el contenido.
    static std::unordered_map<std::string, std::string> out;
    auto& slot = out[key];
    if (slot != result) {
        slot = result;
    }
    return reinterpret_cast<const GxtChar*>(slot.c_str());
}

void Shutdown() {
    if (g_applied >= 0) {
        for (const auto& d : kIcons) {
            CFont::ButtonSprite[d.symbol].Delete();
        }
        for (auto& s : g_extSprite) {
            s.Delete();
        }
        g_applied = -1;
    }
    for (int i = 0; i < 2; i++) {
        if (g_sets[i].slot >= 0) {
            CTxdStore::SafeRemoveTxdSlot(kSetSlot[i]);
            g_sets[i] = {};
        }
    }
}
} // namespace PadButtonIcons
