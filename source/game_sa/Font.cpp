/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CFont
 * Reconstrucción: COMPLETA — 40/40 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 40/40 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CFontChar
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
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

#include "Font.h"
#include "X87Math.h"

#include "eLanguage.h"

auto& FontRenderStateBuf = StaticRef<std::array<CFontChar, 9>>(0xC716B0);
auto& pEmptyChar = StaticRef<CFontChar*>(0xC716A8);

auto& gFontData = StaticRef<std::array<tFontData, 2>>(0xC718B0);

void CFont::InjectHooks() {
    RH_ScopedClass(CFont);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BA690);
    RH_ScopedInstall(Shutdown, 0x7189B0);
    RH_ScopedInstall(PrintChar, 0x718A10);
    RH_ScopedInstall(ParseToken, 0x718F00);

    // styling functions
    RH_ScopedInstall(SetScale, 0x719380);
    RH_ScopedInstall(SetScaleForCurrentLanguage, 0x7193A0);
    RH_ScopedInstall(SetSlantRefPoint, 0x719400);
    RH_ScopedInstall(SetSlant, 0x719420);
    RH_ScopedInstall(SetColor, 0x719430);
    RH_ScopedInstall(SetFontStyle, 0x719490);
    RH_ScopedInstall(SetWrapx, 0x7194D0);
    RH_ScopedInstall(SetCentreSize, 0x7194E0);
    RH_ScopedInstall(SetRightJustifyWrap, 0x7194F0);
    RH_ScopedInstall(SetAlphaFade, 0x719500);
    RH_ScopedInstall(SetDropColor, 0x719510);
    RH_ScopedInstall(SetDropShadowPosition, 0x719570);
    RH_ScopedInstall(SetEdge, 0x719590);
    RH_ScopedInstall(SetProportional, 0x7195B0);
    RH_ScopedInstall(SetBackground, 0x7195C0);
    RH_ScopedInstall(SetBackgroundColor, 0x7195E0);
    RH_ScopedInstall(SetJustify, 0x719600);
    RH_ScopedInstall(SetOrientation, 0x719610);

    RH_ScopedInstall(InitPerFrame, 0x719800);
    RH_ScopedInstall(RenderFontBuffer, 0x719840);
    RH_ScopedInstall(PrintSegment, 0x719B40);
    RH_ScopedInstall(GetStringWidth, 0x71A0E0);
    RH_ScopedInstall(DrawFonts, 0x71A210);
    RH_ScopedInstall(ProcessCurrentString, 0x71A220);
    RH_ScopedInstall(GetNumberLines, 0x71A5E0);
    RH_ScopedInstall(ProcessStringToDisplay, 0x71A600);
    RH_ScopedInstall(GetTextRect, 0x71A620);
    RH_ScopedInstall(PrintString, 0x71A700);
    RH_ScopedInstall(PrintStringFromBottom, 0x71A820);
    RH_ScopedInstall(GetCharacterSize, 0x719750);
    RH_ScopedInstall(LoadFontValues, 0x7187C0);
    RH_ScopedGlobalInstall(GetScriptLetterSize, 0x719670);
    RH_ScopedInstall(FindSubFontCharacter, 0x7192C0);
    RH_ScopedGlobalInstall(GetLetterIdPropValue, 0x718770);
    RH_ScopedInstall(StripTildeTags, 0x719240);  // lote 174A (banco Fn174A)
    RH_ScopedInstall(MapExtendedChar, 0x719360); // lote 174A (banco Fn174A)
    {
        RH_ScopedNamedClass(CFontChar, "CFontChar");
        RH_ScopedInstall(Set, 0x718E50); // lote 174A (banco Fn174A): cuerpo antes inline en la cabecera
    }
}

// 0x718E50 (lote 174A, banco Fn174A; ver Font.h)
CFontChar& CFontChar::Set(const CFontChar& setup) {
    const auto* const from = reinterpret_cast<const uint8*>(&setup);
    auto* const to = reinterpret_cast<uint8*>(this);
    const auto copy32 = [&](size_t offset) { uint32 value; std::memcpy(&value, from + offset, 4); std::memcpy(to + offset, &value, 4); };
    const auto copy8 = [&](size_t offset) { to[offset] = from[offset]; };
    for (const size_t offset : {0x00u, 0x04u, 0x08u, 0x0Cu, 0x10u}) {
        copy32(offset);
    }
    for (const size_t offset : {0x14u, 0x15u, 0x16u, 0x17u}) {
        copy8(offset);
    }
    for (const size_t offset : {0x18u, 0x1Cu, 0x20u, 0x24u}) {
        copy32(offset);
    }
    for (const size_t offset : {0x28u, 0x29u, 0x2Au}) {
        copy8(offset);
    }
    uint16 word;
    std::memcpy(&word, from + 0x2C, 2);
    std::memcpy(to + 0x2C, &word, 2);
    copy8(0x2E);
    return *this;
}

// 0x719240 (lote 174A, banco Fn174A; cdecl, un puntero, `ret`, 115 bytes): como el original, copia el texto completo (sin limite) a un buffer local de 0x100 bytes y lo reescribe en el sitio sin lo que hay entre pares
// de '~' (la etiqueta incluidas sus dos marcas). Una '~' sin pareja recorre la memoria hasta encontrar otra (como el EXE: el texto del juego siempre trae las marcas por pares). No devuelve nada.
void CFont::StripTildeTags(GxtChar* text) {
    GxtChar buffer[256];
    GxtChar* end = buffer;
    for (const GxtChar* in = text; *in; ++in) {
        *end++ = *in;
    }
    *end = 0;

    const GxtChar* cursor = buffer;
    GxtChar ch = *cursor;
    uint32 written = 0;
    if (ch) {
        do {
            if (ch != '~') {
                text[written++] = ch;
            } else {
                ch = cursor[1];
                ++cursor;
                if (ch != '~') {
                    do {
                        ch = cursor[1];
                        ++cursor;
                    } while (ch != '~');
                }
            }
            ch = cursor[1];
            ++cursor;
        } while (ch);
    }
    text[written] = 0;
}

// 0x719360 (lote 174A, banco Fn174A; cdecl, un byte de pila, `ret`, 19 bytes): los caracteres menores que 0x80 pasan tal cual; el resto, el byte de la tabla de entradas de 2 bytes en 0x8D5AD8 + 2 * caracter.
GxtChar CFont::MapExtendedChar(uint8 ch) {
    if (ch < 0x80) {
        return ch;
    }
    return *reinterpret_cast<const uint8*>(0x8D5AD8 + ch * 2);
}

// 0x7187C0
void CFont::LoadFontValues() {
    CFileMgr::SetDir("");
    auto* file = CFileMgr::OpenFile("DATA\\FONTS.DAT", "rb");

    char attrib[32];

    uint32 totalFonts = 0;
    // Como el original: 0x7187CA guarda el id en BL (xor bl,bl) y lo usa como byte (movzx edx,bl; imul edx,edx,0D2h).
    uint8 fontId = 0;

    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '\0' || *line == '#')
            continue;

        if (sscanf_s(line, "%s", SCANF_S_STR(attrib)) == EOF)
            continue;

        if (!memcmp(attrib, "[TOTAL_FONTS]", 14)) {
            auto nextLine = CFileLoader::LoadLine(file);

            VERIFY(sscanf_s(nextLine, "%d", &totalFonts) == 1);
        }
        else if (!memcmp(attrib, "[FONT_ID]", 10)) {
            auto nextLine = CFileLoader::LoadLine(file);

            // Como el original: 0x718877 mov bl,byte [esp+20h]: sscanf escribe un int y BL guarda su byte bajo.
            int32 fontIdValue{};
            VERIFY(sscanf_s(nextLine, "%d", &fontIdValue) == 1);
            fontId = uint8(fontIdValue);
        }
        else if (!memcmp(attrib, "[REPLACEMENT_SPACE_CHAR]", 25)) {
            auto nextLine = CFileLoader::LoadLine(file);
            // Como el original: 0x718897..0x7188BD hace sscanf("%d") sobre un int y guarda su byte bajo (mov al,[esp+20h]).
            int32 spaceValue{};

            VERIFY(sscanf_s(nextLine, "%d", &spaceValue) == 1);
            gFontData.data()[fontId].m_spaceValue = uint8(spaceValue);
        }
        else if (!memcmp(attrib, "[PROP]", 7)) {
            for (int32 i = 0; i < 26; i++) {
                auto nextLine = CFileLoader::LoadLine(file);
                int32 propValues[8]{};

                VERIFY(sscanf_s(nextLine, "%d  %d  %d  %d  %d  %d  %d  %d",
                    &propValues[0], &propValues[1], &propValues[2], &propValues[3],
                    &propValues[4], &propValues[5], &propValues[6], &propValues[7]
                ) == 8);

                for (auto j = 0u; j < std::size(propValues); j++) {
                    gFontData.data()[fontId].m_propValues.data()[i * 8 + j] = uint8(propValues[j]);
                }
            }
        }
        else if (!memcmp(attrib, "[UNPROP]", 9)) {
            auto nextLine = CFileLoader::LoadLine(file);
            uint32 unpropValue;

            VERIFY(sscanf_s(nextLine, "%d", &unpropValue) == 1);
            gFontData.data()[fontId].m_unpropValue = uint8(unpropValue);
        }
    }

    CFileMgr::CloseFile(file);
}

// 0x5BA690
void CFont::Initialise() {
    int32 fontsTxd = CTxdStore::AddTxdSlot("fonts");
    CTxdStore::LoadTxd(fontsTxd, "MODELS\\FONTS.TXD");
    CTxdStore::AddRef(fontsTxd);
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(fontsTxd);
    Sprite[0].SetTexture("font2", "font2m");
    Sprite[1].SetTexture("font1", "font1m");

    LoadFontValues();

    SetScale(1.0f, 1.0f);
    SetSlantRefPoint(SCREEN_WIDTH, 0.0f);
    SetSlant(0.0f);

    SetColor(CRGBA(255, 255, 255, 0));
    SetOrientation(eFontAlignment::ALIGN_LEFT);
    SetJustify(false);

    SetWrapx(SCREEN_WIDTH);

    SetCentreSize(SCREEN_WIDTH);
    SetBackground(false, false);

    SetBackgroundColor(CRGBA(128, 128, 128, 128));
    SetProportional(true);
    SetFontStyle(eFontStyle::FONT_GOTHIC);
    SetRightJustifyWrap(0.0f);
    SetAlphaFade(255.0f);
    SetDropShadowPosition(0);
    CTxdStore::PopCurrentTxd();

    int32 ps2btnsTxd = CTxdStore::AddTxdSlot("ps2btns");
    CTxdStore::LoadTxd(ps2btnsTxd, "MODELS\\PCBTNS.TXD");
    CTxdStore::AddRef(ps2btnsTxd);
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(ps2btnsTxd);

    ButtonSprite[1].SetTexture("up", "upA");
    ButtonSprite[2].SetTexture("down", "downA");
    ButtonSprite[3].SetTexture("left", "leftA");
    ButtonSprite[4].SetTexture("right", "rightA");

    CTxdStore::PopCurrentTxd();
}

// 0x7189B0
void CFont::Shutdown() {
    std::ranges::for_each(Sprite, [](CSprite2d& sprite) { sprite.Delete(); });
    CTxdStore::SafeRemoveTxdSlot("fonts"); // FIX_BUGS: Added check for is slot exists
    // Como el original: 0x7189D9..0x7189EE borra solo ButtonSprite[1..4] (esi=0xC71ADC, edi=4), no los 15.
    for (size_t i = 1; i <= 4; i++) ButtonSprite[i].Delete();
    CTxdStore::SafeRemoveTxdSlot("ps2btns"); // FIX_BUGS: Added check for is slot exists
}

// this adds a single character into rendering buffer
// 0x718A10
void CFont::PrintChar(float x, float y, char character) {
    if (y < 0.0f || y > float(SCREEN_HEIGHT) || x < 0.0f || x > float(SCREEN_WIDTH)) {
        return;
    }
    auto& state = RenderState;
    if (PS2Symbol != EXSYMBOL_NONE) {
        CRect rect;
        rect.left = x;
        rect.bottom = (state.m_fHeight + state.m_fHeight) + y;
        rect.right = state.m_fHeight * 17.0f + x;
        rect.top = state.m_fHeight * 19.0f + y;
        ButtonSprite[PS2Symbol].Draw(rect, {255, 255, 255, state.m_color.a});
        return;
    }
    uint8 letter = static_cast<uint8>(character);
    const bool blank = letter == 0 || letter == '?';
    if (blank) letter = 0;
    const float proportion = GetLetterIdPropValue(letter) * (1.0f / 32.0f);
    if (state.m_nFontStyle == 1 && letter == 208) letter = 0;
    const float u = float(letter & 15) * 0.0625f;
    if (blank) return;
    CRect rect;
    rect.left = x;
    if (static_cast<int16>(state.m_wFontTexture) != 0 && static_cast<int16>(state.m_wFontTexture) != 1) {
        const float v = float(letter >> 4) * 0.0625f;
        rect.bottom = y;
        rect.right = (state.m_fWidth * 32.0f) * proportion + x;
        rect.top = (state.m_fHeight * 32.0f) * 0.5f + y;
        const float rightU = proportion * 0.0625f + u;
        const float bottomV = v + 0.0625f;
        CSprite2d::AddToBuffer(rect, state.m_color, u, v, rightU, v, u, bottomV, rightU - 0.0001f, bottomV - 0.0001f);
        return;
    }
    const float v = float(double(letter >> 4) * 0.078125);
    const float rightU = (u + 0.0625f) - 0.001f;
    rect.right = state.m_fWidth * 32.0f + x;
    if (state.m_fSlant == 0.0f) {
        rect.bottom = y;
        if (letter < 192) {
            rect.top = (state.m_fHeight * 40.0f) * 0.5f + y;
            const float bottomV = (v + 0.078125f) - 0.0021f;
            CSprite2d::AddToBuffer(rect, state.m_color, u, v + 0.0021f, rightU, v + 0.0021f, u, bottomV, rightU, bottomV);
        } else {
            rect.top = (state.m_fHeight * 32.0f) * 0.5f + y;
            CSprite2d::AddToBuffer(rect, state.m_color, u, v + 0.0021f, rightU, v + 0.0021f, u, (v + 0.078125f) - 0.016f, rightU, (v + 0.078125f) - 0.015f);
        }
    } else {
        rect.bottom = y + 0.015f;
        rect.top = ((state.m_fHeight * 40.0f) * 0.5f + y) + 0.015f;
        CSprite2d::AddToBuffer(rect, state.m_color, u, v + 0.00055f, rightU, v + 0.0121f, u, (v + 0.078125f) - 0.009f, rightU, ((v + 0.078125f) - 0.0021f) + 0.01f);
    }
}

// 0x718F00
char* CFont::ParseToken(char* text, CRGBA& color, bool isBlip, char* tag) {
    // info about tokens: https://gtamods.com/wiki/GXT#Tokens

    char* next = ++text;

    auto ApplyStyle = [&](eHudColours hudColor) {
        if (!isBlip)
            color = HudColour.GetRGBA(hudColor, color.a);

        if (tag)
            *tag = *next;
    };

    switch (*next) {
    case '<':
        PS2Symbol = EXSYMBOL_DPAD_LEFT;
        break;
    case '>':
        PS2Symbol = EXSYMBOL_DPAD_RIGHT;
        break;
    case 'A':
    case 'a':
        PS2Symbol = EXSYMBOL_L3;
        break;
    case 'B':
    case 'b':
        ApplyStyle(HUD_COLOUR_DARK_BLUE);
        break;
    case 'C':
    case 'c':
        PS2Symbol = EXSYMBOL_R3;
        break;
    case 'D':
    case 'd':
        PS2Symbol = EXSYMBOL_DPAD_DOWN;
        break;
    case 'G':
    case 'g':
        ApplyStyle(HUD_COLOUR_GREEN);
        break;
    case 'H':
    case 'h':
        if (!isBlip) {
            color = {
                (uint8)std::min((float)color.r * 1.5f, 255.0f),
                (uint8)std::min((float)color.g * 1.5f, 255.0f),
                (uint8)std::min((float)color.b * 1.5f, 255.0f),
                color.a
            };
        }

        if (tag)
            *tag = *next;
        break;
    case 'J':
    case 'j':
        PS2Symbol = EXSYMBOL_R1;
        break;
    case 'K':
    case 'k':
        PS2Symbol = EXSYMBOL_KEY;
        break;
    case 'M':
    case 'm':
        PS2Symbol = EXSYMBOL_L2;
        break;
    case 'N':
    case 'n':
        m_bNewLine = true;
        break;
    case 'O':
    case 'o':
        PS2Symbol = EXSYMBOL_CIRCLE;
        break;
    case 'P':
    case 'p':
        ApplyStyle(HUD_COLOUR_PURPLE);
        break;
    case 'Q':
    case 'q':
        PS2Symbol = EXSYMBOL_SQUARE;
        break;
    case 'R':
    case 'r':
        ApplyStyle(HUD_COLOUR_RED);
        break;
    case 'S':
    case 's':
        ApplyStyle(HUD_COLOUR_LIGHT_GRAY);
        break;
    case 'T':
    case 't':
        PS2Symbol = EXSYMBOL_TRIANGLE;
        break;
    case 'U':
    case 'u':
        PS2Symbol = EXSYMBOL_DPAD_UP;
        break;
    case 'V':
    case 'v':
        PS2Symbol = EXSYMBOL_R2;
        break;
    case 'W':
    case 'w':
        ApplyStyle(HUD_COLOUR_LIGHT_GRAY);
        break;
    case 'X':
    case 'x':
        PS2Symbol = EXSYMBOL_CROSS;
        break;
    case 'Y':
    case 'y':
        ApplyStyle(HUD_COLOUR_CREAM);
        break;
    case 'l':
        ApplyStyle(HUD_COLOUR_BLACK);
        break;
    default:
        break;
    }

    if (*next != '~') {
        // skip text to the next '~' character.
        // Como el original: 0x719176..0x71917C (mov al,[edi+1]; inc edi; cmp al,7Eh; jne) no comprueba el NUL.
        for(; *next != '~'; next++);
    }

    if (*next)
        return next + 1;

    return next + 2;
}

// Text scaling
// 0x719380
void CFont::SetScale(float w, float h) {
    m_Scale.Set(w, h);
}

// Text scaling depends on current language
// 0x7193A0
void CFont::SetScaleForCurrentLanguage(float w, float h) {
    switch (FrontEndMenuManager.m_nPrefsLanguage) {
    case eLanguage::FRENCH:
    case eLanguage::GERMAN:
    case eLanguage::ITALIAN:
    case eLanguage::SPANISH:
        m_Scale.Set(w * 0.8f, h);
        break;
    default:
        m_Scale.Set(w, h);
    }
}

// Set text rotation point
// 0x719400
void CFont::SetSlantRefPoint(float x, float y) {
    m_fSlantRefPoint.Set(x, y);
}

// Set text rotation angle
// 0x719420
void CFont::SetSlant(float value) {
    m_fSlant = value;
}

// Set text color
// 0x719430
void CFont::SetColor(CRGBA color) {
    m_Color = color;

    // Como el original: 0x71943A fcomp 255.0 + 0x71945E test ah,5 + jp: entra si alpha < 255 O si es NaN.
    // 0x719474 fmul [859A3C] = 0x3B808081 (1/255 como float): multiplica, no divide. 0x71947A _ftol2 (trunca a 64 bits).
    if (m_fFontAlpha < 255.0f) { // fcomp + test ah,5 + jp: con NaN el EXE salta
        m_Color.a = (uint8)(int64)(float(color.a) * m_fFontAlpha * std::bit_cast<float>(0x3B808081u));
    }
}

// Set text style
// 0x719490
void CFont::SetFontStyle(eFontStyle style) {
    switch (style) {
    case eFontStyle::FONT_PRICEDOWN:
        m_FontTextureId = 1;
        m_FontStyle = 1;
        break;
    case eFontStyle::FONT_MENU:
        m_FontTextureId = 0;
        m_FontStyle = 2;
        break;
    default:
        m_FontTextureId = static_cast<uint8>(style);
        m_FontStyle = 0;
    }
}

// Set line width at right
// 0x7194D0
void CFont::SetWrapx(float value) {
    m_fWrapx = value;
}

// Set line width at center
// 0x7194E0
void CFont::SetCentreSize(float value) {
    m_fFontCentreSize = value;
}

// 0x7194F0
void CFont::SetRightJustifyWrap(float value) {
    m_fRightJustifyWrap = value;
}

// Like a 'global' font alpha, multiplied with each text alpha (from SetColor)
// 0x719500
void CFont::SetAlphaFade(float alpha) {
    m_fFontAlpha = alpha;
}

// Drop color is used for text shadow and text outline
// 0x719510
void CFont::SetDropColor(CRGBA color) {
    m_FontDropColor = color;

    // Como el original: 0x71951A fcomp 255.0 + 0x71953B test ah,5 + jp: entra si alpha < 255 o NaN.
    // 0x71954F..0x71955B: m_Color.a * m_fFontAlpha * [859A3C] (=0x3B808081) y _ftol2; el fuente omitia el factor 1/255.
    if (m_fFontAlpha < 255.0f) { // fcomp + test ah,5 + jp: con NaN el EXE salta
        m_FontDropColor.a = (uint8)(int64)(float(m_Color.a) * m_fFontAlpha * std::bit_cast<float>(0x3B808081u));
    }
}

// Set shadow size
// 0x719570
void CFont::SetDropShadowPosition(int16 value) {
    m_nFontOutlineSize = 0;
    m_nFontOutlineOrShadow = 0;
    m_nFontShadow = (uint8)value;
}

// Set outline size
// 0x719590
void CFont::SetEdge(int8 value) {
    m_nFontShadow = 0;
    m_nFontOutlineSize = value;
    m_nFontOutlineOrShadow = value;
}

// Toggles character proportions in text
// 0x7195B0
void CFont::SetProportional(bool on) {
    m_bFontPropOn = on;
}

// Setups text background
// 0x7195C0
void CFont::SetBackground(bool enable, bool includeWrap) {
    m_bFontBackground = enable;
    m_bEnlargeBackgroundBox = includeWrap;
}

// Sets background color
// 0x7195E0
void CFont::SetBackgroundColor(CRGBA color) {
    m_FontBackgroundColor = color;
}

// 0x719600
void CFont::SetJustify(bool on) {
    m_bFontJustify = on;
}

// 0x719610
void CFont::SetOrientation(eFontAlignment alignment) {
    // Como el original: 0x719610..0x719649 es un switch sobre el byte (0 centro, 1 izquierda, 2 derecha);
    // cualquier otro valor (p. ej. -1 = ALIGN_UNDEFINED) NO escribe nada (jne 0x719649).
    switch (static_cast<uint8>(alignment)) {
    case 0: // ALIGN_CENTER
        m_bFontCentreAlign = true;
        m_bFontRightAlign = false;
        break;
    case 1: // ALIGN_LEFT
        m_bFontCentreAlign = false;
        m_bFontRightAlign = false;
        break;
    case 2: // ALIGN_RIGHT
        m_bFontCentreAlign = false;
        m_bFontRightAlign = true;
        break;
    default:
        break;
    }
}

// Need to call this each frame
// 0x719800
void CFont::InitPerFrame() {
    ZoneScoped;

    // Como el original: 0x719802 [C71A9B]=0 (m_nFontOutlineSize), 0x719807 [C71A9C]=0, 0x71980C [C71A96]=0,
    // 0x71981B word [C71ACC]=0FFFFh (el fuente ponia 0 y no tocaba C71A9B).
    m_nFontOutlineSize = 0;
    m_nFontOutline = 0;
    m_nFontShadow = 0;
    m_bNewLine = false;
    PS2Symbol = EXSYMBOL_NONE;
    RenderState.m_wFontTexture = 0xFFFF;
    pEmptyChar = &FontRenderStateBuf[0]; // FontRenderStatePointer.pRenderState

    CSprite::InitSpriteBuffer();
}

// Draw text we have in buffer
// 0x719840
void CFont::RenderFontBuffer() {
    auto* const beginning = reinterpret_cast<char*>(FontRenderStateBuf.data());
    if (reinterpret_cast<char*>(pEmptyChar) == beginning) return;
    Sprite[static_cast<int16>(RenderState.m_wFontTexture)].SetRenderState();
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RenderState.Set(*reinterpret_cast<CFontChar*>(beginning));
    auto color = RenderState.m_color;
    float x = RenderState.m_vPosn.x;
    float y = RenderState.m_vPosn.y;
    auto* cursor = beginning + sizeof(CFontChar);
    while (cursor < reinterpret_cast<char*>(pEmptyChar)) {
        if (*cursor == 0) {
            do { ++cursor; } while (uintptr_t(cursor) & 3u);
            if (cursor >= reinterpret_cast<char*>(pEmptyChar)) break;
            RenderState.Set(*reinterpret_cast<CFontChar*>(cursor));
            color = RenderState.m_color;
            x = RenderState.m_vPosn.x;
            y = RenderState.m_vPosn.y;
            cursor += sizeof(CFontChar);
        }
        PS2Symbol = EXSYMBOL_NONE;
        while (*cursor == '~' && PS2Symbol == EXSYMBOL_NONE) {
            cursor = ParseToken(cursor, color, RenderState.m_bContainImages, nullptr);
            if (!RenderState.m_bContainImages) RenderState.m_color = color;
        }
        uint8 letter = static_cast<uint8>(*cursor - 32);
        if (RenderState.m_nFontStyle == 0) {
            if (letter == 145) letter = 64;
            else if (letter > 155) letter = 0;
        } else {
            letter = FindSubFontCharacter(letter, RenderState.m_nFontStyle);
        }
        if (RenderState.m_fSlant != 0.0f) {
            y = (RenderState.m_vSlanRefPoint.x - x) * RenderState.m_fSlant + RenderState.m_vSlanRefPoint.y;
        }
        if (PS2Symbol == EXSYMBOL_NONE || !RenderState.m_bContainImages) {
            PrintChar(x, y, static_cast<char>(letter));
        }
        const auto outline = static_cast<int8>(RenderState.m_nOutline);
        if (PS2Symbol != EXSYMBOL_NONE) {
            x = (RenderState.m_fHeight * 17.0f + float(outline)) + x;
        } else {
            const uint8 index = letter == '?' ? 0 : letter;
            auto* const data = gFontData.data() + static_cast<int16>(RenderState.m_wFontTexture);
            const auto width = RenderState.m_bPropOn == 1 ? data->m_propValues[index] : data->m_unpropValue;
            x = (float(outline) + float(width)) * RenderState.m_fWidth + x;
        }
        if (letter == 0) x = RenderState.m_fWrap + x;
        if (*cursor && PS2Symbol == EXSYMBOL_NONE) {
            ++cursor;
        } else if (PS2Symbol != EXSYMBOL_NONE) {
            PS2Symbol = EXSYMBOL_NONE;
            Sprite[static_cast<int16>(RenderState.m_wFontTexture)].SetRenderState();
        }
    }
    CSprite::FlushSpriteBuffer();
    CSprite2d::RenderVertexBuffer();
    pEmptyChar = FontRenderStateBuf.data();
}

// Imprime un segmento [begin, end) de texto en el buffer de fuentes (sin simbolo confirmado).
// 0x719B40 (cdecl, 5 argumentos en pila; el EXE lo llama a si mismo para sombra y contorno).
// Como el original: sombra (m_nFontShadow != 0) tiene prioridad sobre contorno (m_nFontOutlineSize != 0);
// ambos dibujan recursivamente con el color de sombra, m_bFontIsBlip = 1 y el valor a 0, y despues
// restauran color y valor, y ponen m_bFontIsBlip = 0 (no restauran el valor previo, igual que el EXE).
// Cada operacion x87 del EXE se hace a 24 bits (palabra de control del juego): aqui cada paso es float.
void CFont::PrintSegment(float left, float top, const char* begin, const char* end, float spacing) {
    constexpr float kScaleX = 0.0015625f;     // 0x859520
    constexpr float kScaleY = 0.0022321429f;  // 0x859524
    const auto& maxWidth  = StaticRef<int32>(0xC17044);
    const auto& maxHeight = StaticRef<int32>(0xC17048);

    // 0x719B43..0x719B57: color guardado (dword) antes de vaciar el buffer.
    CRGBA saved = m_Color;
    if (RenderState.m_wFontTexture != static_cast<uint16>(m_FontTextureId)) {
        RenderFontBuffer(); // 0x719840
        RenderState.m_wFontTexture = static_cast<uint16>(m_FontTextureId);
    }
    // 0x719B6A: el dword se vuelve a leer tras el vaciado (lo usan los caminos de sombra y contorno).
    const CRGBA colorBeforeEffect = m_Color;

    // Como el original: dx = (ancho * 0.0015625) * o ; dy = (alto * 0.00223214) * o: fild del entero, fmul y fmul con la palabra de control del juego (24 bits), un solo redondeo por operacion.
    const auto OffsetX = [&](int32 o) { return FontOffsetCpp(maxWidth, kScaleX, o); }; // (w * kx) * o con la x87 de 24 bits (fild + fmul + fmul)
    const auto OffsetY = [&](int32 o) { return FontOffsetCpp(maxHeight, kScaleY, o); };

    // 0x719C3D / 0x719D5D: desplaza el punto de referencia de la inclinacion (no se restaura).
    const auto SetEffectState = [&](int8 value) {
        m_Color.b = m_FontDropColor.b; // 0x719BC0
        m_Color.r = m_FontDropColor.r; // 0x719BD4
        m_Color.g = m_FontDropColor.g; // 0x719BE7
        m_Color.a = m_FontDropColor.a; // 0x719BED
        m_bFontIsBlip = true;          // 0x719BE0
        if (!(m_fSlant == 0.0f)) {
            const int32 o = static_cast<int32>(value);
            m_fSlantRefPoint.x = FontAddCpp(OffsetX(o), m_fSlantRefPoint.x);
            m_fSlantRefPoint.y = FontAddCpp(OffsetY(o), m_fSlantRefPoint.y);
        }
    };
    const auto RestoreColor = [&]() {
        m_Color = colorBeforeEffect;
        m_bFontIsBlip = false; // 0x719F29: pone 0, no el valor anterior
    };

    const int8 shadow = static_cast<int8>(m_nFontShadow);
    if (shadow != 0) {
        // 0x719BA5..0x719C9E
        SetEffectState(shadow);
        m_nFontShadow = 0; // 0x719BF3
        const int32 o = static_cast<int32>(shadow);
        PrintSegment(FontAddCpp(OffsetX(o), left), FontAddCpp(OffsetY(o), top), begin, end, spacing); // 0x719C7F
        m_nFontShadow = static_cast<uint8>(shadow); // 0x719C98
        RestoreColor();
    } else {
        const int8 outline = static_cast<int8>(m_nFontOutlineSize);
        if (outline != 0) {
            // 0x719CC5..0x719F29
            SetEffectState(outline);
            m_nFontOutlineSize = 0; // 0x719D13
            const int32 o = static_cast<int32>(outline);
            // El EXE recalcula dx y dy (fild de los enteros de pantalla) en cada llamada: aqui tambien, por si el vaciado los cambia.
            const auto dx = [&] { return OffsetX(o); };
            const auto dy = [&] { return OffsetY(o); };
            PrintSegment(FontAddCpp(dx(), left), FontSubCpp(top, dy()), begin, end, spacing); // 0x719DA5
            PrintSegment(FontSubCpp(left, dx()), FontSubCpp(top, dy()), begin, end, spacing); // 0x719DE2
            PrintSegment(FontAddCpp(dx(), left), FontAddCpp(dy(), top), begin, end, spacing); // 0x719E1F
            PrintSegment(FontSubCpp(left, dx()), FontAddCpp(dy(), top), begin, end, spacing); // 0x719E5C
            PrintSegment(FontAddCpp(dx(), left), top, begin, end, spacing); // 0x719E84
            PrintSegment(FontSubCpp(left, dx()), top, begin, end, spacing); // 0x719EAC
            PrintSegment(left, FontAddCpp(dy(), top), begin, end, spacing); // 0x719ED4
            PrintSegment(left, FontSubCpp(top, dy()), begin, end, spacing); // 0x719EFC
            m_nFontOutlineSize = static_cast<uint8>(outline); // 0x719F15
            RestoreColor();
        }
    }

    // 0x719F30: si no cabe (0xC7187C - longitud), se vacia el buffer.
    {
        const uintptr_t limit = uintptr_t(0xC7187C) - reinterpret_cast<uintptr_t>(end) + reinterpret_cast<uintptr_t>(begin);
        if (!(reinterpret_cast<uintptr_t>(pEmptyChar) < limit)) {
            RenderFontBuffer();
        }
    }

    // 0x719F4E..0x719FE7: cabecera de 0x30 bytes (m_cLetter y m_dLetter no se escriben).
    CFontChar* const record = pEmptyChar;
    record->m_vPosn.x        = left;
    record->m_vPosn.y        = top;
    record->m_fWidth         = m_Scale.x;
    record->m_fHeight        = m_Scale.y;
    record->m_color.r        = m_Color.r;
    record->m_color.g        = m_Color.g;
    record->m_color.b        = m_Color.b;
    record->m_color.a        = m_Color.a;
    record->m_fWrap          = spacing;
    record->m_fSlant         = m_fSlant;
    record->m_vSlanRefPoint  = m_fSlantRefPoint;
    record->m_nFontStyle     = m_FontStyle;
    record->m_bPropOn        = m_bFontPropOn;
    record->m_wFontTexture   = static_cast<uint16>(m_FontTextureId);
    record->m_bContainImages = m_bFontIsBlip;
    record->m_nOutline       = m_nFontOutline;
    char* out = reinterpret_cast<char*>(record) + sizeof(CFontChar);
    pEmptyChar = reinterpret_cast<CFontChar*>(out);

    // 0x719FF3..0x71A08F: copia el texto; las etiquetas '~' pasan por ParseToken (0x718F00).
    // El color guardado (r,g,b) lo pueden actualizar las etiquetas; el alfa es el de m_Color.
    uint8 savedAlpha = saved.a; // 0x719FA4: mov bl,[esp+13h]
    const char* src = begin;
    if (src < end) {
        do {
            const char ch = *src;
            if (ch == '~') {
                CRGBA parsed = m_Color; // 0x719FFD
                const char* const next = ParseToken(const_cast<char*>(src), parsed, RenderState.m_bContainImages, nullptr);
                if (!m_bFontIsBlip && PS2Symbol == EXSYMBOL_NONE) {
                    // 0x71A02B..0x71A05C
                    m_Color.r = parsed.r;
                    m_Color.g = parsed.g;
                    m_Color.b = parsed.b;
                    saved.r = parsed.r;
                    saved.g = parsed.g;
                    saved.b = parsed.b;
                    savedAlpha = m_Color.a;
                }
                PS2Symbol = EXSYMBOL_NONE; // 0x71A068
                out = reinterpret_cast<char*>(pEmptyChar);
                if (src != next) {
                    while (src != next) *out++ = *src++;
                    pEmptyChar = reinterpret_cast<CFontChar*>(out);
                }
            } else {
                *out++ = ch;
                ++src;
                pEmptyChar = reinterpret_cast<CFontChar*>(out);
            }
        } while (src < end);
    }

    // 0x71A08F: terminador y alineacion a 4 bytes.
    out = reinterpret_cast<char*>(pEmptyChar);
    *out++ = 0;
    pEmptyChar = reinterpret_cast<CFontChar*>(out);
    if (reinterpret_cast<uintptr_t>(out) & 3u) {
        do { ++out; } while (reinterpret_cast<uintptr_t>(out) & 3u);
        pEmptyChar = reinterpret_cast<CFontChar*>(out);
    }

    // 0x71A0AC: restaura el color si no es un blip.
    if (!m_bFontIsBlip) {
        m_Color.r = saved.r;
        m_Color.g = saved.g;
        m_Color.b = saved.b;
        m_Color.a = savedAlpha;
    }
}

// 0x71A0E0
float CFont::GetStringWidth(const GxtChar* string, bool full, bool scriptText) {
    // Como el original: 0x71A104 movzx edx,ax (longitud de 16 bits); 0x71A112..0x71A138 copia min(len,0x18F)
    // bytes y pone el terminador (sin strncpy_s, que en depuracion rellena con 0xFE y falla si len >= 400).
    const size_t len = static_cast<uint16>(CMessages::GetStringLength(string));
    GxtChar data[400];
    {
        const size_t n = std::min<size_t>(len, 0x18F);
        for (size_t i = 0; i < n; i++) data[i] = string[i];
        data[n] = 0;
    }
    CMessages::InsertPlayerControlKeysInString(data);

    float width = 0.0f;
    bool lastWasTag = false, lastWasLetter = false;
    auto* pStr = data;

    while (true) {
        if (*pStr == ' ' && !full)
            break;
        if (*pStr == '\0')
            break;

        if (*pStr == '~') {
            if (!full && (lastWasTag || lastWasLetter))
                return width;

            auto* next = pStr + 1;

            // Como el original: 0x71A190..0x71A196 busca '~' sin comprobar el NUL.
            if (*next != '~') {
                for (; *next != '~'; next++);
            }

            pStr = next + 1;

            if (lastWasLetter || *pStr == '~')
                lastWasTag = true;
        }
        else {
            if (!full && *pStr == ' ' && lastWasTag)
                return width;

            char upper = *pStr - 0x20;

            pStr++;
            if (scriptText) {
                width += GetScriptLetterSize(upper);
            }
            else {
                width += GetCharacterSize(upper);
            }

            lastWasLetter = true;
        }
    }

    return width;
}

// same as RenderFontBuffer() (0x71A210)
void CFont::DrawFonts() {
    ZoneScoped;

    RenderFontBuffer();
}

// 0x71A220
int16 CFont::ProcessCurrentString(bool print, float x, float y, const GxtChar* text) {
    const auto savedColor = m_Color;
    const auto emitLine = [](float left, float top, const char* begin, const char* end, float spacing) {
        PrintSegment(left, top, begin, end, spacing); // 0x719B40
    };
    auto* lineBegin = reinterpret_cast<const char*>(text);
    auto* cursor = lineBegin;
    bool firstWord = true;
    char lastTag = 0;
    int32 lines = 0;
    int32 spaces = 0;
    float usedWidth = 0.0f;
    float width = (m_bFontCentreAlign || m_bFontRightAlign) ? 0.0f : x;
    float lineY = y;
    char continued[256];
    while (*cursor) {
        PS2Symbol = EXSYMBOL_NONE;
        float wordEnd = GetStringWidth(reinterpret_cast<const GxtChar*>(cursor), false, false);
        CRGBA ignored;
        if (*cursor == '~') cursor = ParseToken(const_cast<char*>(cursor), ignored, true, &lastTag);
        const float limit = m_bFontCentreAlign ? m_fFontCentreSize : m_bFontRightAlign ? x - m_fRightJustifyWrap : m_fWrapx;
        wordEnd += width;
        if ((limit < wordEnd && !firstWord) || m_bNewLine) {
            float spacing = 0.0f;
            if (PS2Symbol != EXSYMBOL_NONE) cursor -= 3;
            auto* end = m_bNewLine ? cursor - 3 : cursor;
            float lineX;
            if (m_bFontCentreAlign) {
                lineX = x - width * 0.5f;
            } else {
                if (m_bFontJustify) spacing = (m_fWrapx - usedWidth) / float(static_cast<int16>(spaces));
                lineX = m_bFontRightAlign ? x - (width - GetCharacterSize(0)) : x;
            }
            ++lines;
            if (print) emitLine(lineX, lineY, lineBegin, end, spacing);
            lineBegin = cursor;
            if (lastTag) {
                gString[0] = '~'; gString[1] = lastTag; gString[2] = '~'; gString[3] = 0;
                memcpy(continued, gString, 4);
                if (m_bNewLine) end += 3;
                auto* write = continued + 3;
                while (*end) *write++ = *end++;
                *write = 0;
                lineBegin = continued;
                lastTag = 0;
            }
            m_bNewLine = false;
            lineY = (m_Scale.y + m_Scale.y + (m_Scale.y * 32.0f) * 0.5f) + lineY;
            width = (m_bFontCentreAlign || m_bFontRightAlign) ? 0.0f : x;
            spaces = 0;
            usedWidth = 0.0f;
            firstWord = true;
            cursor = lineBegin;
        } else {
            while (*cursor != ' ' && *cursor && *cursor != '~') ++cursor;
            width = wordEnd;
            if (!*cursor) {
                const float lineX = m_bFontCentreAlign ? x - width * 0.5f : m_bFontRightAlign ? x - width : x;
                ++lines;
                if (print) emitLine(lineX, lineY, lineBegin, cursor, 0.0f);
            } else {
                if (!firstWord) ++spaces;
                if (*cursor != '~') {
                    width = GetCharacterSize(0) + width;
                    ++cursor;
                }
                usedWidth = width;
                firstWord = false;
            }
        }
        if (PS2Symbol != EXSYMBOL_NONE) PS2Symbol = EXSYMBOL_NONE;
    }
    if (print) SetColor(savedColor);
    return static_cast<int16>(lines);
}

// 0x71A5E0
int16 CFont::GetNumberLines(float x, float y, const GxtChar* text) {
    return ProcessCurrentString(false, x, y, text);
}

// 0x71A600
int16 CFont::ProcessStringToDisplay(float x, float y, const GxtChar* text) {
    return ProcessCurrentString(true, x, y, text);
}

// 0x71A620
void CFont::GetTextRect(CRect* rect, float x, float y, const GxtChar* text) {
    // Como el original: 0x71A62F call 0x71A220 (ProcessCurrentString) antes de leer la alineacion.
    const int16 lines = GetNumberLines(x, y, text);

    if (m_bFontCentreAlign) {
        // Como el original: 0x71A643..0x71A675: ((centro*0.5)+4) se resta de x y se suma a x: ((c*0.5)+4)+x.
        rect->left = x - (m_fFontCentreSize * 0.5f + 4.0f);
        rect->right = (m_fFontCentreSize * 0.5f + 4.0f) + x;
    }
    else if (m_bFontRightAlign) {
        rect->left = m_fRightJustifyWrap - 4.0f;
        // Como el original: 0x71A693 fld [esp+8] (x); 0x71A6AB fadd [858B90] (4.0); 0x71A6B1 fstp [ecx+8]: right = x + 4.
        rect->right = x + 4.0f;
    }
    else {
        rect->left = x - 4.0f;
        rect->right = m_fWrapx + 4.0f;
    }

    // Como el original: 0x71A6B4 movzx eax,al: solo cuenta el byte bajo del numero de lineas (sin signo);
    // 0x71A6E4..0x71A6EC: ((alto * lineas) + y) + 4.0.
    rect->top = y - 4.0f;
    rect->bottom = (GetHeight() * (float)(uint8)lines) + y + 4.0f;
}

// 0x71A700
void CFont::PrintString(float x, float y, const GxtChar* text) {
    if (*text == '\0' || *text == '*')
        return;

    if (m_bFontBackground) {
        CRect rt;

        // Como el original: 0x71A744/0x71A74F/0x71A755 copia solo r,g,b (bytes C71AB4..C71AB6); el alfa no se toca.
        RenderState.m_color.r = m_Color.r;
        RenderState.m_color.g = m_Color.g;
        RenderState.m_color.b = m_Color.b;
        GetTextRect(&rt, x, y, text);

        if (m_bEnlargeBackgroundBox) {
            rt.left -= 1.0f;
            rt.right += 1.0f;
            rt.bottom += 1.0f;
            rt.top -= 1.0f;

            FrontEndMenuManager.DrawWindow(rt, nullptr, 0, m_FontBackgroundColor, false, true);
        } else {
            CSprite2d::DrawRect(rt, m_FontBackgroundColor);
        }
        m_bFontBackground = false;
    }

    ProcessStringToDisplay(x, y, text);
}

// 0x71A820
void CFont::PrintStringFromBottom(float x, float y, const GxtChar* text) {
    float drawY = y - GetHeight() * (float)GetNumberLines(x, y, text);

    if (m_fSlant != 0.0f)
        drawY -= (m_fSlantRefPoint.x - x) * m_fSlant + m_fSlantRefPoint.y;

    PrintString(x, drawY, text);
}

// 0x719750
float CFont::GetCharacterSize(uint8 letterId) {
    uint8 propValueIdx = letterId;

    if (letterId == '?') {
        letterId = 0;
        propValueIdx = 0;
    }

    if (m_FontStyle)
        propValueIdx = FindSubFontCharacter(letterId, m_FontStyle);
    else if (propValueIdx == 145)
        propValueIdx = '@';
    else if (propValueIdx > 155)
        propValueIdx = 0;

    // Como el original: 0x7197AA/0x7197CF movsx ecx,byte [C71A9B]: el borde es CON signo; 0x7197A2 indexa los bytes sin
    // comprobar el rango (FindSubFontCharacter puede devolver 208 = m_spaceValue), de ahi .data().
    if (m_bFontPropOn) {
        return ((float)gFontData.data()[m_FontTextureId].m_propValues.data()[propValueIdx] + (float)(int8)m_nFontOutlineSize) * m_Scale.x;
    } else {
        return ((float)gFontData.data()[m_FontTextureId].m_unpropValue + (float)(int8)m_nFontOutlineSize) * m_Scale.x;
    }
}

// Android
float CFont::GetHeight(bool a1) {
    assert(a1 == false && "NOT IMPLEMENTED");
    const float y = a1 ? 0.0f : m_Scale.y;
    // Como el original (0x71A6C8..0x71A6E2 y 0x71A839..0x71A85D): (y*32)*0.5 + (y+y); el fuente sumaba ((y*32/2)+y)+y.
    return (y * 32.0f) * 0.5f + (y + y);
}

// 0x719670, original name unknown
// Como el original: lee IntroTextLines[NumberOfIntroTextLinesThisFrame] SIN comprobar el rango (0x719689 imul eax,eax,44h)
// y los bytes de fuente SIN comprobar el indice (0x7196F7/0x719700 y 0x719722); por eso el acceso es por puntero crudo.
// Estilo 2 -> textura 0 y FindSubFontCharacter(letra, 2); estilo 3 -> textura 1 y FindSubFontCharacter(letra, 1);
// resto -> textura = byte bajo del estilo, 0x91 -> 0x40 y > 0x9B -> 0 (0x7196B1..0x7196EA).
// Suma entera (fild + fiadd) y producto por la escala en un solo redondeo a 24 bits, como el original.
float GetScriptLetterSize(uint8 letterId) {
    if (letterId == '?')
        letterId = 0;

    const auto* const text = reinterpret_cast<const tScriptText*>(reinterpret_cast<const uint8*>(CTheScripts::IntroTextLines.data()) + sizeof(tScriptText) * CTheScripts::NumberOfIntroTextLinesThisFrame);
    const bool  proportional = text->IsProportional;
    const int32 style        = static_cast<int32>(text->FontStyle);
    const int8  outline      = text->TextEdge;
    const float scaleX       = text->Scale.x;

    uint8 fontId;
    uint8 charIdx = letterId;
    if (style == 2) {
        fontId  = 0;
        charIdx = CFont::FindSubFontCharacter(letterId, 2);
    } else if (style == 3) {
        fontId  = 1;
        charIdx = CFont::FindSubFontCharacter(letterId, 1);
    } else {
        fontId = static_cast<uint8>(style);
        if (letterId == 0x91)
            charIdx = 0x40;
        else if (letterId > 0x9B)
            charIdx = 0;
    }

    const auto* const font = reinterpret_cast<const uint8*>(gFontData.data()) + fontId * sizeof(tFontData);
    const int32 value = proportional ? font[charIdx] : font[offsetof(tFontData, m_unpropValue)];
    return static_cast<float>(value + static_cast<int32>(outline)) * scaleX;
}

// 0x7192C0
uint8 CFont::FindSubFontCharacter(uint8 letterId, uint8 fontStyle) {
    if (fontStyle == 1) { // eFontStyle::FONT_PRICEDOWN
        switch (letterId) {
        case 1:  return 208;
        case 4:  return 93;
        case 7:  return 206;
        case 8:
        case 9:  return letterId + 86;
        case 14: return 207;
        case 26: return 154;
        }
    }

    if (letterId == 6)                      return 10;
    if (letterId >=  16 && letterId <=  25) return letterId - 128;
    if (letterId == 31)                     return 91;
    if (letterId >=  33 && letterId <=  58) return letterId + 122;
    if (letterId == 62)                     return 32;
    if (letterId >=  65 && letterId <=  90) return letterId + 90;
    if (letterId >=  96 && letterId <= 118) return letterId + 85;
    if (letterId >= 119 && letterId <= 140) return letterId + 62;
    if (letterId >= 141 && letterId <= 142) return 204;
    if (letterId == 143)                    return 205;

    return letterId;
}

// 0x718770
float GetLetterIdPropValue(uint8 letterId) {
    uint8 id = letterId;

    if (letterId == '?')
        id = 0;

    // Como el original: 0x71877A cmp byte [C71ACA],1 (solo el valor 1 es proporcional); 0x718781 movsx ecx,word [C71ACC]
    // (indice con signo; InitPerFrame lo deja en -1) y acceso sin comprobar rango.
    const auto* data = gFontData.data() + static_cast<int16>(CFont::RenderState.m_wFontTexture);
    if (*reinterpret_cast<const uint8*>(&CFont::RenderState.m_bPropOn) == 1)
        return data->m_propValues.data()[id];
    else
        return data->m_unpropValue;
}
