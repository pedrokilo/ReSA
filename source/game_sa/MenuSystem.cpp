/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMenuSystem
 * Reconstrucción: COMPLETA — 21/21 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 21/21 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "MenuSystem.h"

auto& MenuNumber = StaticRef<std::array<CMenuSystem::Menu*, 2>>(0xBA82D8);

void CMenuSystem::InjectHooks() {
    RH_ScopedClass(CMenuSystem);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5822D0);
    RH_ScopedInstall(SetActiveMenuItem, 0x5820C0);
    RH_ScopedInstall(SetColumnOrientation, 0x582080);
    RH_ScopedInstall(SetColumnWidth, 0x582050);
    RH_ScopedInstall(GetCarColourFromGrid, 0x5822B0);
    RH_ScopedInstall(CheckForAccept, 0x5807C0);
    RH_ScopedInstall(CheckForSelected, 0x5807E0);
    RH_ScopedInstall(Input, 0x5825D0);
    RH_ScopedInstall(InputStandardMenu, 0x580800);
    RH_ScopedInstall(InputGridMenu, 0x580BD0);
    RH_ScopedInstall(DisplayStandardMenu, 0x580E00);
    RH_ScopedInstall(DisplayGridMenu, 0x5816E0);
    RH_ScopedInstall(Process, 0x582630);
    RH_ScopedInstall(HighlightOneItem, 0x581C10);
    RH_ScopedInstall(InsertOneMenuItemWithNumber, 0x581D70);
    RH_ScopedInstall(CreateNewMenu, 0x582300);
    RH_ScopedInstall(ActivateItems, 0x581990);
    RH_ScopedInstall(ActivateOneItem, 0x581B30);
    RH_ScopedInstall(FillGridWithCarColours, 0x5820E0);
    RH_ScopedInstall(InsertMenu, 0x581E00);
    RH_ScopedInstall(SwitchOffMenu, 0x580750);
}

// 0x5822D0
void CMenuSystem::Initialise() {
    for (auto i = 0; i < MENU_COUNT; ++i) {
        if (MenuInUse[i])
            SwitchOffMenu(i);
    }
    CurrentMenuInUse = 0;
}

// 0x5820C0
void CMenuSystem::SetActiveMenuItem(MenuId id, int8 item) {
    MenuNumber[id]->m_nSelectedRow = item;
}

// 0x581C40
void CMenuSystem::SetColumnHeader(MenuId id, uint8 column, Const char* header) {
    assert(column < MENU_COL_COUNT);
    strcpy(MenuNumber[id]->m_aacColumnHeaders[column], header ? header : "\0");
}

// NOTSA
void CMenuSystem::SetRowTitle(MenuId id, uint8 column, uint8 row, Const char* title) {
    assert(column < MENU_COL_COUNT);
    strcpy(MenuNumber[id]->m_aaacRowTitles[column][row], title ? title : "\0");
}

// 0x582080
void CMenuSystem::SetColumnOrientation(MenuId id, uint8 column, uint8 orientation) {
    assert(column < MENU_COL_COUNT);
    MenuNumber[id]->m_anColumnAlignment[column] = static_cast<eFontAlignment>(orientation);
}

// 0x582050
void CMenuSystem::SetColumnWidth(MenuId id, uint8 column, uint16 width) {
    assert(column < MENU_COL_COUNT);
    MenuNumber[id]->m_afColumnWidth[column] = float(width);
}

// 0x5820A0, unused
void CMenuSystem::SetHeaderOrientation(MenuId id, uint8 column, uint8 orientation) {
    assert(column < MENU_COL_COUNT);
    MenuNumber[id]->m_anColumnHeaderAlignment[column] = static_cast<eFontAlignment>(orientation);
}

// 0x581C90, unused
void CMenuSystem::SetDPadInput(MenuId id, uint8 input) {
    MenuNumber[id]->m_abColumnInteractive[INTERACTIVE_DPAD] = input;
}

// 0x5822B0
uint8 CMenuSystem::GetCarColourFromGrid(MenuId id, uint8 colorId) {
    assert(colorId < MENU_CAR_COLOR_COUNT);
    return MenuNumber[id]->m_anUsedCarColors[colorId];
}

// 0x581CB0, unused
void CMenuSystem::GetMenuPosition(MenuId id, float* outX, float* outY) {
    assert(outX && outY);
    *outX = MenuNumber[id]->m_vPosn.x;
    *outY = MenuNumber[id]->m_vPosn.y;
}

// 0x5807C0
int8 CMenuSystem::CheckForAccept(MenuId id) {
    return MenuInUse[id] ? MenuNumber[id]->m_nAcceptedRow : MENU_UNDEFINED;
}

// 0x5807E0
int8 CMenuSystem::CheckForSelected(MenuId id) {
    return MenuInUse[id] ? MenuNumber[id]->m_nSelectedRow : MENU_UNDEFINED;
}

// 0x5825D0
void CMenuSystem::Input(MenuId id) {
    switch (MenuNumber[id]->m_nType) {
    case MENU_TYPE_DEFAULT:
        return InputStandardMenu(id);
    case MENU_TYPE_GRID:
        return InputGridMenu(id);
    }
}

// 0x580800
void CMenuSystem::InputStandardMenu(MenuId id) {
    auto menu = MenuNumber[id];

    if (CurrentMenuInUse < 0) {
        CurrentMenuInUse = 1;
    }

    if (!MenuInUse.data()[CurrentMenuInUse] /* sin comprobar límites, como el original */) {
        do {
            if (CurrentMenuInUse <= 0)
                break;
            --CurrentMenuInUse;
        } while (!MenuInUse.data()[CurrentMenuInUse] /* sin comprobar límites, como el original */);
    }

    if (CurrentMenuInUse >= 2) {
        CurrentMenuInUse = 0;
    }

    if (!MenuInUse.data()[CurrentMenuInUse] /* sin comprobar límites, como el original */) {
        do {
            if (CurrentMenuInUse >= 2)
                break;
            ++CurrentMenuInUse;
        } while (!MenuInUse.data()[CurrentMenuInUse] /* sin comprobar límites, como el original */);
    }

    auto pad = CPad::GetPad();

    if (pad->IsTrianglePressed() && !CTimer::GetIsPaused()) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_BACK);
    }

    // Como el original: con pausa solo cuenta la tecla extenter (0xB733EA/0xB7317A), no ambas Enter.
    if (pad->IsCrossPressed() || CTimer::GetIsPaused() && CPad::IsReturnJustPressed()) {
        if (!CTimer::GetIsPaused())
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);

        if (menu->m_abRowSelectable[menu->m_nSelectedRow])
            menu->m_nAcceptedRow = menu->m_nSelectedRow;
    }

    if (menu->m_nAcceptedRow != menu->m_nSelectedRow)
        menu->m_nAcceptedRow = MENU_UNDEFINED;

    if (menu->m_nNumRows <= 1)
        return;

    if (CPad::GetAnaloguePadUp() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadUpPressed() || CTimer::GetIsPaused() && CPad::IsUpPressed()) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT);

        do {
            menu->m_nSelectedRow -= 1;
        } while ((!menu->m_abRowSelectable[menu->m_nSelectedRow] || !menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0]) && menu->m_nSelectedRow >= 0);
    }

    if (CPad::GetAnaloguePadDown() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadDownPressed() || CTimer::GetIsPaused() && CPad::IsDownPressed()) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT);
        ++menu->m_nSelectedRow;
        while (true) {
            if (menu->m_abRowSelectable[menu->m_nSelectedRow]) {
                if (menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0])
                    break;
            }

            if (menu->m_nSelectedRow >= menu->m_nNumRows)
                break;

            menu->m_nSelectedRow += 1;
        }
    }

    if (menu->m_nSelectedRow < 0) {
        menu->m_nSelectedRow = menu->m_nNumRows;
        do {
            menu->m_nSelectedRow -= 1;
        } while ((!menu->m_abRowSelectable[menu->m_nSelectedRow] || !menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0]) && menu->m_nSelectedRow >= 0);
    }

    if (menu->m_nSelectedRow >= menu->m_nNumRows) {
        menu->m_nSelectedRow = 0;
        while (true) {
            if (menu->m_abRowSelectable[menu->m_nSelectedRow]) {
                if (menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0])
                    break;
            }

            if (menu->m_nSelectedRow >= menu->m_nNumRows)
                break;

            menu->m_nSelectedRow += 1;
        }
    }
}

// Here we optimized calls of CPad::GetPad
// 0x580BD0
void CMenuSystem::InputGridMenu(MenuId id) {
    auto menu = MenuNumber[id];
    auto pad = CPad::GetPad();

    // Como el original: con pausa solo cuenta la tecla extenter (0xB733EA/0xB7317A), no ambas Enter.
    if (pad->IsCrossPressed() || CTimer::GetIsPaused() && CPad::IsReturnJustPressed()) {
        if (menu->m_abRowSelectable[menu->m_nSelectedRow])
            menu->m_nAcceptedRow = menu->m_nSelectedRow;
    }

    // Como el original: escribe -99 (0x9D); UNDEFINED era eSection::UNDEFINED (0).
    if (menu->m_nAcceptedRow != menu->m_nSelectedRow)
        menu->m_nAcceptedRow = MENU_UNDEFINED;

    if (CPad::GetAnaloguePadUp() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadUpPressed()) {
        if (menu->m_nSelectedRow >= menu->m_nNumColumns)
            menu->m_nSelectedRow -= menu->m_nNumColumns;
    }

    if (CPad::GetAnaloguePadDown() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadDownPressed()) {
        if (menu->m_nSelectedRow < menu->m_nNumRows - menu->m_nNumColumns)
            menu->m_nSelectedRow += menu->m_nNumColumns;
    }

    if (CPad::GetAnaloguePadLeft() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadLeftPressed()) {
        if (menu->m_nSelectedRow > 0)
            menu->m_nSelectedRow -= 1;
    }

    if (CPad::GetAnaloguePadRight() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadRightPressed()) {
        if (menu->m_nSelectedRow < menu->m_nNumRows - 1)
            menu->m_nSelectedRow += 1;
    }
}

// 0x582600, unused
void CMenuSystem::Display(MenuId id, uint8 unk) {
    switch (MenuNumber[id]->m_nType) {
    case MENU_TYPE_DEFAULT:
        return DisplayStandardMenu(id, unk);
    case MENU_TYPE_GRID:
        return DisplayGridMenu(id, unk);
    }
}

// Como el original: (ancho * 0.0015625f (0x859520)) * x y (alto * 0.002232143f (0x859524)) * y,
// no x * ancho / 640 como SCREEN_STRETCH_X/Y.
static float MenuStretchX(float x) { return ((float)RsGlobal.maximumWidth * 0.0015625f) * x; }
static float MenuStretchY(float y) { return ((float)RsGlobal.maximumHeight * 0.002232143f) * y; }

// Como el original: los anchos acumulados se truncan con _ftol2 y se guardan en 16 bits.
static uint16 MenuSumColumnWidths(const CMenuSystem::Menu* menu, int32 count) {
    uint16 sum = 0;
    for (auto i = 0; i < count; ++i) {
        sum = (uint16)(int32)((float)sum + menu->m_afColumnWidth[i]);
    }
    return sum;
}

// 0x580E00
void CMenuSystem::DisplayStandardMenu(MenuId id, bool bBrightFont) {
    auto menu = MenuNumber[id];
    // Como el original: la Y base es un entero (_ftol2) que luego se relee como uint16.
    int32 baseY = (int32)MenuStretchY(20.0f);
    uint8 windowOpacity = bBrightFont ? 0 : 120;

    if (menu->m_bColumnBackground) {
        uint8  menuHeight = 50;
        uint16 menuWidth  = 0;
        for (auto i = 0; i < menu->m_nNumColumns; i++) {
            menuWidth = (uint16)(int32)((float)menuWidth + menu->m_afColumnWidth[i]);
            if (menu->m_aacColumnHeaders[i][0]) {
                menuHeight = 70;
                baseY = (int32)MenuStretchY(40.0f);
            }
        }
        const auto top   = (float)menuHeight * MenuStretchY(1.0f) + (float)menu->m_nNumRows * MenuStretchY(16.0f) + menu->m_vPosn.y;
        const auto right = MenuStretchX(20.0f) + (float)menuWidth + menu->m_vPosn.x;

        CRect rect(menu->m_vPosn.x, menu->m_vPosn.y, right, top);
        FrontEndMenuManager.DrawWindow(rect, menu->m_szTitle, windowOpacity, CRGBA(0, 0, 0, 190), false, true);
    }
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::SetScale(MenuStretchX(0.52f), MenuStretchY(1.0f));
    CFont::SetDropShadowPosition(2);
    CFont::SetDropColor(CRGBA(0, 0, 0, 255));
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);

    // 0x580FDD
    // Header
    for (auto column = 0; column < menu->m_nNumColumns; column++) {
        if (!menu->m_aacColumnHeaders[column][0]) {
            continue;
        }
        auto alignment = menu->m_anColumnHeaderAlignment[column];
        if (static_cast<int32>(alignment) < 0) {
            alignment = menu->m_anColumnAlignment[column];
        }

        CFont::SetOrientation(alignment);
        CFont::SetColor(CRGBA(225 - windowOpacity, 225 - windowOpacity, 225 - windowOpacity, 255));

        // add previous columns width
        const auto xOffset = MenuSumColumnWidths(menu, column);

        const auto columnText = menu->m_aacColumnHeaders[column];
        const auto headerText = TheText.Get(columnText);

        // Como el original: se decide con la alineación efectiva de la cabecera (como byte sin signo)
        // y cualquier valor distinto de centro/derecha se trata como izquierda.
        float textX;
        switch (static_cast<uint8>(alignment)) {
        case 0: // ALIGN_CENTER
            textX = menu->m_afColumnWidth[column] * 0.5f + MenuStretchX(10.0f) + menu->m_vPosn.x + (float)xOffset;
            break;
        case 2: // ALIGN_RIGHT
            textX = MenuStretchX(10.0f) + menu->m_afColumnWidth[column] + menu->m_vPosn.x + (float)xOffset;
            break;
        default:
            textX = MenuStretchX(10.0f) + menu->m_vPosn.x + (float)xOffset;
            break;
        }
        const auto textY = MenuStretchY(20.0f) + menu->m_vPosn.y;
        CFont::PrintString(textX, textY, headerText);
    }

    // Como el original: componentes RGB de la tabla menos la opacidad (en byte) y alpha siempre 255.
    const auto GetColor = [=](auto row) -> CRGBA {
        const auto Make = [=](eHudColours idx) {
            const auto c = HudColour.GetRGB(idx);
            return CRGBA(uint8(c.r - windowOpacity), uint8(c.g - windowOpacity), uint8(c.b - windowOpacity), 255);
        };
        if (menu->m_abColumnInteractive[INTERACTIVE_DEFAULT]) {
            if (row == menu->m_nSelectedRow && menu->m_abRowSelectable[row]) {
                return Make(HUD_COLOUR_LIGHT_BLUE);
            } else if (menu->m_abRowAlreadyBought[row]) {
                return Make(HUD_COLOUR_GREEN);
            } else if (menu->m_abRowSelectable[row]) {
                return Make(HUD_COLOUR_NIGHT_BLUE);
            }
            return Make(HUD_COLOUR_DARK_GRAY);
        }

        if (menu->m_abRowAlreadyBought[row]) {
            return Make(HUD_COLOUR_GREEN);
        } else if (menu->m_abRowSelectable[row]) {
            return Make(HUD_COLOUR_LIGHT_BLUE);
        }
        return Make(HUD_COLOUR_NIGHT_BLUE);
    };

    // 0x581216
    GxtChar buffer[400];
    // draw rows
    for (auto row = 0; row < menu->m_nNumRows; row++) {
        for (auto column = 0; column < menu->m_nNumColumns; column++) {
            if (!menu->m_aaacRowTitles[column][row][0])
                continue;

            CFont::SetColor(GetColor(row)); // NOTSA | optimized
            auto* rowTitle            = menu->m_aaacRowTitles[column][row];
            auto  numberInTitle       = menu->m_aanNumberInRowTitle[column][row];
            auto  secondNumberInTitle = menu->m_aadw2ndNumberInRowTitle[column][row];
            auto  rowText             = TheText.Get(rowTitle);

            CMessages::InsertNumberInString(rowText, numberInTitle, secondNumberInTitle, -1, -1, -1, -1, buffer);
            CFont::SetOrientation(menu->m_anColumnAlignment[column]);
            CMessages::InsertPlayerControlKeysInString(buffer);

            auto _panelWidthTotal = 0.52f;
            CFont::SetScale(MenuStretchX(_panelWidthTotal), MenuStretchY(1.0f));
            while (CFont::GetStringWidth(buffer, true, false) > menu->m_afColumnWidth[column]) {
                _panelWidthTotal -= 0.05f;
                CFont::SetScale(MenuStretchX(_panelWidthTotal), MenuStretchY(1.0f)); // make text smaller :)
            }

            // add previous columns width
            const auto xOffset = MenuSumColumnWidths(menu, column);

            // Como el original: alineación de columna como byte sin signo; otros valores = izquierda.
            float textX;
            switch (static_cast<uint8>(menu->m_anColumnAlignment[column])) {
            case 0: // ALIGN_CENTER
                textX = MenuStretchX(10.0f) + menu->m_afColumnWidth[column] * 0.5f + menu->m_vPosn.x + (float)xOffset;
                break;
            case 2: // ALIGN_RIGHT
                textX = MenuStretchX(10.0f) + menu->m_afColumnWidth[column] + menu->m_vPosn.x + (float)xOffset;
                break;
            default:
                textX = MenuStretchX(10.0f) + menu->m_vPosn.x + (float)xOffset;
                break;
            }
            const auto textY = (float)(uint16)baseY + menu->m_vPosn.y;
            CFont::PrintString(textX, textY, buffer);
        }
        baseY = (int32)(MenuStretchY(16.0f) + (float)(uint16)baseY);
    }
}

// 0x5816E0
void CMenuSystem::DisplayGridMenu(MenuId id, bool bFade) {
    auto menu = MenuNumber[id];

    auto columnWidth = menu->m_afColumnWidth[0];
    if (menu->m_bColumnBackground) {
        CRect rect;
        rect.left   = menu->m_vPosn.x;
        rect.bottom    = menu->m_vPosn.y;
        rect.right  = (float)menu->m_nNumColumns * columnWidth + menu->m_vPosn.x;
        rect.top = (float)menu->m_nNumColumns * columnWidth + menu->m_vPosn.y;
        FrontEndMenuManager.DrawWindow(rect, nullptr, bFade ? 0 : 120, { 0, 0, 0, 190 }, false, true);
    }

    auto index = 0;
    for (auto c = 0; c < menu->m_nNumColumns; c++) {
        for (auto r = 0; r < menu->m_nNumColumns; r++) {
            // white border
            if (index == menu->m_nSelectedRow) {
                CRect rect;
                rect.left   = (float)r * columnWidth + menu->m_vPosn.x;
                rect.bottom    = (float)c * columnWidth + menu->m_vPosn.y;
                rect.right  = ((float)r + 1.0f) * columnWidth + menu->m_vPosn.x;
                rect.top = ((float)c + 1.0f) * columnWidth + menu->m_vPosn.y;
                CSprite2d::DrawRect(rect, { 225, 225, 225, 255 });
            }

            // color
            auto  colorIndex = GetCarColourFromGrid(id, index);
            auto& color = CVehicleModelInfo::ms_vehicleColourTable[colorIndex];
            // Como el original: right/top = ((pos + ancho) - margen) + celda * ancho, con el margen
            // calculado como (ancho/alto de pantalla * constante) * 3.0f.
            CRect rect;
            rect.left   = (float)r * columnWidth + menu->m_vPosn.x + MenuStretchX(3.0f);
            rect.bottom    = (float)c * columnWidth + menu->m_vPosn.y + MenuStretchY(3.0f);
            rect.right  = menu->m_vPosn.x + columnWidth - MenuStretchX(3.0f) + (float)r * columnWidth;
            rect.top = menu->m_vPosn.y + columnWidth - MenuStretchY(3.0f) + (float)c * columnWidth;
            CSprite2d::DrawRect(rect, { color.r, color.g, color.b, 255 });
            index++;
        }
    }
}

// 0x582630
void CMenuSystem::Process(int8 id) {
    if (id != (int8)MENU_UNDEFINED) {
        if (MenuInUse[id]) {
            Display(id, 1);
            if (MenuNumber[id]->m_abColumnInteractive[INTERACTIVE_DEFAULT])
                Input(id);
        }
        return;
    }

    // 0x58263F :thinking
    for (auto i = 0; i < MENU_COUNT; i++) {
        if (i != CurrentMenuInUse && MenuInUse[i]) {
            // Como el original: dibuja el menú i (no el id -99) sin resaltar.
            Display(i, false);
        }
    }

    // 0x5826A4
    if (MenuInUse.data()[CurrentMenuInUse] /* sin comprobar límites, como el original */) {
        Display(CurrentMenuInUse, true);

        if (MenuNumber[CurrentMenuInUse]->m_abColumnInteractive[INTERACTIVE_DEFAULT]) {
            Input(CurrentMenuInUse);
        }
    }
}

// 0x581C10
void CMenuSystem::HighlightOneItem(MenuId id, uint8 item, bool bought) {
    MenuNumber[id]->m_abRowAlreadyBought[item] = bought;
    MenuNumber[id]->m_abRowSelectable[item] = true;
}

// 0x581CE0, unused
void CMenuSystem::InsertOneMenuItem(MenuId id, uint8 column, uint8 row, const char* text) {
    auto* menu = MenuNumber[id];
    SetRowTitle(id, column, row, text);
    menu->m_aanNumberInRowTitle[column][row] = -1;
    menu->m_aadw2ndNumberInRowTitle[column][row] = -1;

    CalcNonEmptyRows(id);
}

// 0x581D70
void CMenuSystem::InsertOneMenuItemWithNumber(MenuId id, uint8 column, uint8 row, const char* text, int32 num1, int32 num2) {
    auto* menu = MenuNumber[id];
    SetRowTitle(id, column, row, text);
    menu->m_aanNumberInRowTitle[column][row] = num1;
    menu->m_aadw2ndNumberInRowTitle[column][row] = num2;

    CalcNonEmptyRows(id);
}

// 0x582300
MenuId CMenuSystem::CreateNewMenu(eMenuType type, const char* title, float x, float y, float width, uint8 columns, bool interactive, bool background, eFontAlignment alignment) {
    assert(num_menus_in_use < MENU_COUNT);

    MenuId menuId = GetNumMenusInUse();
    assert(!MenuInUse[menuId]);
    // Como el original: operator new(0x418) sin inicializar la memoria a cero.
    MenuNumber[menuId] = new Menu;
    MenuInUse[menuId] = true;
    Menu* menu = MenuNumber[menuId];
    menu->m_nType = type;

    switch (type) {
    case MENU_TYPE_DEFAULT:
        menu->m_nNumRows = 0;
        menu->m_nNumColumns = std::min(columns, (uint8)MENU_COL_COUNT);
        menu->m_nSelectedRow = 0;
        menu->m_abColumnInteractive[INTERACTIVE_DEFAULT] = interactive;
        menu->m_abColumnInteractive[INTERACTIVE_DPAD] = true; // DPAD
        menu->m_vPosn = { x, y };
        menu->m_bColumnBackground = background != 0;
        if (title) {
            strcpy(menu->m_szTitle, title); // Como el original: strcpy sin relleno (strcpy_s rellena con 0xFE en depuración)
        } else {
            menu->m_szTitle[0] = '\0';
        }
        menu->m_nAcceptedRow = MENU_UNDEFINED;
        ActivateItems(menuId, true, true, true, true, true, true, true, true, true, true, true, true);
        for (auto i = 0; i < MENU_COL_COUNT; ++i) {
            menu->m_afColumnWidth[i] = width;
            menu->m_anColumnAlignment[i] = alignment;
            menu->m_anColumnHeaderAlignment[i] = eFontAlignment::ALIGN_UNDEFINED;
            menu->m_aacColumnHeaders[i][0] = '\0';
            for (auto j = 0; j < MENU_ROW_COUNT; ++j) {
                menu->m_aaacRowTitles[i][j][0] = '\0';
                menu->m_abRowAlreadyBought[j] = 0;
            }
        }
        break;
    case MENU_TYPE_GRID:
        menu->m_afColumnWidth[0] = width;
        menu->m_bColumnBackground = background != 0;
        menu->m_abColumnInteractive[INTERACTIVE_DEFAULT] = interactive;
        menu->m_abColumnInteractive[INTERACTIVE_DPAD] = true;
        menu->m_nNumColumns = std::min(columns, (uint8)8);
        menu->m_vPosn = { x, y };
        menu->m_nAcceptedRow = MENU_UNDEFINED;
        menu->m_nSelectedRow = 0;
        menu->m_nNumRows = menu->m_nNumColumns * menu->m_nNumColumns;
        FillGridWithCarColours(menuId);
        break;
    }

    ++num_menus_in_use;
    return menuId;
}

// 0x581990
void CMenuSystem::ActivateItems(MenuId id, bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8, bool b9, bool b10, bool b11) {
    auto* menu = MenuNumber[id];

    const bool rows[] = { b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11 };
    for (auto rowId = 0u; rowId < std::size(rows); rowId++) {
        menu->m_abRowSelectable[rowId] = rows[rowId];
    }

    for (auto i = 0; i < MENU_COUNT; ++i) {
        while (true) {
            if (menu->m_abRowSelectable[menu->m_nSelectedRow]) {
                if (menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0])
                    break;
            }
            if (menu->m_nSelectedRow >= menu->m_nNumRows)
                break;
            ++menu->m_nSelectedRow;
        }
        if (menu->m_nSelectedRow >= menu->m_nNumRows) {
            menu->m_nSelectedRow = 0;
        }
    }
}

// 0x581B30, untested
void CMenuSystem::ActivateOneItem(MenuId id, uint8 row, bool enable) {
    // plugin::Call<0x581B30, MenuId, uint8, uint8>(id, row, enable);

    auto* menu = MenuNumber[id];

    menu->m_abRowSelectable[row] = enable;
    while (true) {
        if (menu->m_abRowSelectable[menu->m_nSelectedRow]) {
            if (menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0])
                break;
        }
        if (menu->m_nSelectedRow >= menu->m_nNumRows)
            break;
        menu->m_nSelectedRow += 1;
    }

    if (menu->m_nSelectedRow >= menu->m_nNumRows) {
        menu->m_nSelectedRow = 0;
        while (true) {
            if (menu->m_abRowSelectable[menu->m_nSelectedRow]) {
                if (menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0])
                    break;
            }
            if (menu->m_nSelectedRow >= menu->m_nNumRows)
                break;
            menu->m_nSelectedRow += 1;
        }
    }

    if (menu->m_nSelectedRow >= menu->m_nNumRows)
        menu->m_nSelectedRow = 0;
}

// 0x5820E0
void CMenuSystem::FillGridWithCarColours(MenuId id) {
    std::array<CRGBA, MENU_CAR_COLOR_COUNT> used;
    used.fill(CRGBA{0, 0, 0, 255});
    for (size_t slot = 0; slot < used.size(); ++slot) {
        for (uint32 colourId = 0; colourId < 128; ++colourId) {
            auto colour = CVehicleModelInfo::ms_vehicleColourTable[colourId];
            colour.a = 255;
            bool similar = false;
            if (slot != 0) {
                // The original also compares the next, still black, entry.
                for (size_t previous = 0; previous <= slot; ++previous) {
                    const auto& other = used[previous];
                    if (std::abs(int(colour.r) - other.r) < 15
                        && std::abs(int(colour.g) - other.g) < 15
                        && std::abs(int(colour.b) - other.b) < 15) {
                        similar = true;
                    }
                }
            }
            // If every colour is similar, the original falls back to index 127.
            if (!similar || colourId == 127) {
                used[slot] = colour;
                MenuNumber[id]->m_anUsedCarColors[slot] = uint8(colourId);
                break;
            }
        }
    }
}

// Insert menu column
// 0x581E00
void CMenuSystem::InsertMenu(MenuId id, uint8 column, const char* colHeader, const char* row0, const char* row1, const char* row2, const char* row3, const char* row4, const char* row5, const char* row6, const char* row7, const char* row8, const char* row9, const char* row10, const char* row11) {
    assert(column < MENU_COL_COUNT);
    auto* menu = MenuNumber[id];

    SetColumnHeader(id, column, colHeader);

    const auto rows = std::array{ row0, row1, row2, row3, row4, row5, row6, row7, row8, row9, row10, row11 };
    for (auto rowId = 0u; rowId < std::size(rows); rowId++) {
        SetRowTitle(id, column, rowId, rows[rowId]);
    }

    // Como el original: solo reinicia los números de la columna insertada, no los de las cuatro.
    for (auto j = 0; j < MENU_ROW_COUNT; ++j) {
        menu->m_aanNumberInRowTitle[column][j] = -1;
        menu->m_aadw2ndNumberInRowTitle[column][j] = -1;
    }

    if (column)
        return;

    CalcNonEmptyRows(id);
}

// 0x580750
void CMenuSystem::SwitchOffMenu(MenuId id) {
    if (!MenuInUse[id])
        return;

    delete MenuNumber[id];
    MenuInUse[id] = false;
    num_menus_in_use--;
    CurrentMenuInUse = 0;

    for (auto i = 0; i < MENU_COUNT; ++i) {
        if (MenuInUse[i]) {
            CurrentMenuInUse = i;
            break;
        }
    }
}

// NOTSA
void CMenuSystem::CalcNonEmptyRows(MenuId id) {
    auto* menu = MenuNumber[id];
    // Como el original (inline en 0x581D70/0x581E00): acumula filas pendientes y las suma al
    // encontrar un título no vacío, así que el resultado es el índice del último título no vacío + 1
    // (los huecos intermedios cuentan), no el número de títulos no vacíos.
    menu->m_nNumRows = 0;
    uint8 pending = 0;
    for (auto& row : menu->m_aaacRowTitles[0]) {
        ++pending;
        if (row[0]) {
            menu->m_nNumRows += pending;
            pending = 0;
        }
    }
}
