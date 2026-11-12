/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RGBA.h"
#include "eFontAlignment.h"
#include <extensions/WEnum.hpp>

class CSprite2d;


// todo: move
struct CFontChar {
    uint8     m_cLetter;
    uint8     m_dLetter;
    CVector2D m_vPosn;
    float     m_fWidth;
    float     m_fHeight;
    CRGBA     m_color;
    float     m_fWrap;
    float     m_fSlant;
    CVector2D m_vSlanRefPoint;
    bool      m_bContainImages;
    uint8     m_nFontStyle;
    bool      m_bPropOn;
    uint16    m_wFontTexture;
    uint8     m_nOutline;

public:
    // 0x718E50 (lote 174A, banco Fn174A): copia de un registro de caracter del EXE (thiscall, un puntero de pila, `ret 4`, 117 bytes): 47 bytes (+0x00..+0x2E) en el orden del EXE (dword +0x00 con los dos bytes
    // de relleno +2 y +3, dwords +0x04..+0x10, bytes +0x14..+0x17 del color, dwords +0x18..+0x24, bytes +0x28..+0x2A, word +0x2C y byte +0x2E; el byte +0x2F no se toca); devuelve this.
    CFontChar& Set(const CFontChar& setup);
};
VALIDATE_SIZE(CFontChar, 0x30);

struct tFontData {
    std::array<uint8, 208> m_propValues;
    uint8 m_spaceValue;
    uint8 m_unpropValue;
};

enum eExtraFontSymbol : uint8 {
    EXSYMBOL_NONE       = 0, // invalid
    EXSYMBOL_DPAD_UP    = 1,
    EXSYMBOL_DPAD_DOWN  = 2,
    EXSYMBOL_DPAD_LEFT  = 3,
    EXSYMBOL_DPAD_RIGHT = 4,
    EXSYMBOL_CROSS      = 5,
    EXSYMBOL_CIRCLE     = 6,
    EXSYMBOL_SQUARE     = 7,
    EXSYMBOL_TRIANGLE   = 8,
    EXSYMBOL_KEY        = 9, // followed by buttons, L1?
    EXSYMBOL_L2         = 10,
    EXSYMBOL_L3         = 11,
    EXSYMBOL_R1         = 12,
    EXSYMBOL_R2         = 13,
    EXSYMBOL_R3         = 14
};

enum eFontStyle : uint8 {
    FONT_GOTHIC,
    FONT_SUBTITLES,
    FONT_MENU,
    FONT_PRICEDOWN
};
NOTSA_WENUM_DEFS_FOR(eFontStyle);

class CFont {
public:
    // static variables
    static constexpr size_t MAX_FONT_SPRITES = 2;
    static constexpr size_t MAX_FONT_BUTTON_SPRITES = 15;

    static inline auto& RenderState = StaticRef<CFontChar>(0xC71AA0);
    // font textures array
    static inline auto& Sprite = StaticRef<CSprite2d[MAX_FONT_SPRITES]>(0xC71AD0);
    // button textures array
    static inline auto& ButtonSprite = StaticRef<CSprite2d[MAX_FONT_BUTTON_SPRITES]>(0xC71AD8);
    static inline auto& PS2Symbol = StaticRef<eExtraFontSymbol>(0xC71A54);
    static inline auto& m_bNewLine = StaticRef<bool>(0xC71A55);
    static inline auto& m_Color = StaticRef<CRGBA>(0xC71A60);
    static inline auto& m_Scale = StaticRef<CVector2D>(0xC71A64);
    static inline auto& m_fSlant = StaticRef<float>(0xC71A6C);
    static inline auto& m_fSlantRefPoint = StaticRef<CVector2D>(0xC71A70);
    static inline auto& m_bFontJustify = StaticRef<bool>(0xC71A78);
    static inline auto& m_bFontCentreAlign = StaticRef<bool>(0xC71A79);
    static inline auto& m_bFontRightAlign = StaticRef<bool>(0xC71A7A);
    static inline auto& m_bFontBackground = StaticRef<bool>(0xC71A7B);
    static inline auto& m_bEnlargeBackgroundBox = StaticRef<bool>(0xC71A7C);
    static inline auto& m_bFontPropOn = StaticRef<bool>(0xC71A7D);
    static inline auto& m_bFontIsBlip = StaticRef<bool>(0xC71A7E);
    static inline auto& m_fFontAlpha = StaticRef<float>(0xC71A80);
    static inline auto& m_FontBackgroundColor = StaticRef<CRGBA>(0xC71A84);
    static inline auto& m_fWrapx = StaticRef<float>(0xC71A88);
    static inline auto& m_fFontCentreSize = StaticRef<float>(0xC71A8C);
    static inline auto& m_fRightJustifyWrap = StaticRef<float>(0xC71A90);
    static inline auto& m_FontTextureId = StaticRef<uint8>(0xC71A94);
    static inline auto& m_FontStyle = StaticRef<uint8>(0xC71A95);
    static inline auto& m_nFontShadow = StaticRef<uint8>(0xC71A96);
    static inline auto& m_FontDropColor = StaticRef<CRGBA>(0xC71A97);
    static inline auto& m_nFontOutlineSize = StaticRef<uint8>(0xC71A9B);
    static inline auto& m_nFontOutline = StaticRef<uint8>(0xC71A9C);
    static inline auto& m_nFontOutlineOrShadow = StaticRef<uint8>(0xC71A9C);

public:
    static void InjectHooks();

    static void Initialise();
    static void LoadFontValues();
    static void Shutdown();
    static void PrintChar(float x, float y, char character);
    // Get next ' ' character in a string
    static char* GetNextSpace(char* string);
    static char* ParseToken(char* text, CRGBA& color, bool isBlip, char* tag);
    static void SetScale(float w, float h);
    static void SetScaleForCurrentLanguage(float w, float h);
    static void SetSlantRefPoint(float x, float y);
    static void SetSlant(float value);
    static void SetColor(CRGBA color);
    static void SetFontStyle(eFontStyle style);
    static void SetWrapx(float value);
    static void SetCentreSize(float value);
    static void SetRightJustifyWrap(float value);
    static void SetAlphaFade(float alpha);
    static void SetDropColor(CRGBA color);
    static void SetDropShadowPosition(int16 value);
    static void SetEdge(int8 value);
    static void SetProportional(bool on);
    static void SetBackground(bool enable, bool includeWrap);
    static void SetBackgroundColor(CRGBA color);
    static void SetJustify(bool on);
    static void SetOrientation(eFontAlignment alignment);
    static void InitPerFrame();
    static void RenderFontBuffer();
    // 0x719B40: imprime un segmento [begin, end) en el buffer (sin simbolo confirmado)
    static void PrintSegment(float left, float top, const char* begin, const char* end, float spacing);
    static float GetHeight(bool a1 = false);
    static float GetStringWidth(const GxtChar* string, bool full, bool scriptText);
    static void DrawFonts();
    static int16 ProcessCurrentString(bool print, float x, float y, const GxtChar* text);
    static int16 GetNumberLines(float x, float y, const GxtChar* text);
    static int16 ProcessStringToDisplay(float x, float y, const GxtChar* text);
    static void GetTextRect(CRect* rect, float x, float y, const GxtChar* text);
    static void PrintString(float x, float y, const GxtChar* text);
    static void PrintStringFromBottom(float x, float y, const GxtChar* text);
    static float GetCharacterSize(uint8 ch);
    static uint8 FindSubFontCharacter(uint8 letterId, uint8 fontStyle);
    // Quita del texto (en el sitio) las etiquetas entre '~' (lote 174A, banco Fn174A)
    static void StripTildeTags(GxtChar* text); // 0x719240
    // Caracter de teclado a caracter GXT: los >= 0x80 pasan por la tabla de 0x8D5AD8 (lote 174A, banco Fn174A)
    static GxtChar MapExtendedChar(uint8 ch); // 0x719360
};

static void ReadFontsDat();
float GetScriptLetterSize(uint8 letterId);
float GetLetterIdPropValue(uint8 letterId);
