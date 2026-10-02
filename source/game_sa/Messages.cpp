/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMessages
 * Reconstrucción: COMPLETA — 28/28 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 28/28 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Messages.h"
#include "Hud.h"
#include <ControllerConfigManager.h>

constexpr auto MSG_BUF_SZ = 400u;

void CMessages::InjectHooks() {
    RH_ScopedClass(CMessages);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetStringLength, 0x69DB50);
    RH_ScopedInstall(StringCopy, 0x69DB70);
    RH_ScopedInstall(StringCompare, 0x69DBD0);
    RH_ScopedInstall(CutString, 0x69DC50);
    RH_ScopedInstall(ClearMessages, 0x69DCD0);
    RH_ScopedInstall(ClearSmallMessagesOnly, 0x69DD30);
    RH_ScopedInstall(AddToPreviousBriefArray, 0x69DD50);
    RH_ScopedInstall(ClearPreviousBriefArray, 0x69DE70);
    RH_ScopedInstall(InsertNumberInString, 0x69DE90);
    RH_ScopedInstall(InsertStringInString, 0x69E040);
    RH_ScopedInstall(InsertPlayerControlKeysInString, 0x69E160);
    RH_ScopedInstall(AddMessageWithNumberQ, 0x69E360);
    RH_ScopedInstall(AddMessageJumpQWithNumber, 0x69E4E0);
    RH_ScopedInstall(AddBigMessageWithNumber, 0x69E5F0);
    RH_ScopedInstall(AddBigMessageWithNumberQ, 0x69E6E0);
    RH_ScopedInstall(AddMessageWithStringQ, 0x69E800);
    RH_ScopedInstall(AddMessageJumpQWithString, 0x69E950);
    RH_ScopedInstall(ClearThisPrint, 0x69EA30);
    RH_ScopedInstall(ClearThisBigPrint, 0x69EBE0);
    RH_ScopedInstall(ClearThisPrintBigNow, 0x69ED80);
    RH_ScopedInstall(Init, 0x69EE00);
    RH_ScopedInstall(ClearAllMessagesDisplayedByGame, 0x69EDC0);
    RH_ScopedInstall(Process, 0x69EE60);
    RH_ScopedInstall(Display, 0x69EFC0);
    RH_ScopedInstall(AddMessageQ, 0x69F0B0);
    RH_ScopedInstall(AddMessageJump, 0x69F1E0);
    RH_ScopedInstall(AddBigMessage, 0x69F2B0);
    RH_ScopedInstall(AddBigMessageQ, 0x69F370);
}

// Como el original: solo se ponen a cero `Text` y `StringToInsert` de cada entrada;
// el resto de campos (números, tiempos, flags...) se conservan en memoria.
template<size_t N>
static void ClearMessagesTextAndString(std::array<tMessage, N>& msgs) {
    for (auto& msg : msgs) {
        msg.Text           = nullptr;
        msg.StringToInsert = nullptr;
    }
}

// Como el original: anula la entrada `i` y desplaza hacia delante las siguientes
// (copia de 0x30 bytes con `rep movs`) solo hasta la primera entrada libre; la última
// entrada movida queda solo con `Text = nullptr` (sus demás campos no se tocan).
template<size_t N>
static void RemoveMessageAt(std::array<tMessage, N>& msgs, int16 i) {
    msgs[i].Text = nullptr;
    int16 k = i;
    if (k < (int16)N - 1) {
        while (msgs[k + 1].Text) {
            std::memcpy((void*)&msgs[k], (const void*)&msgs[k + 1], sizeof(tMessage));
            if (++k >= (int16)N - 1) {
                break;
            }
        }
    }
    msgs[k].Text = nullptr;
}

// Como el original: compara primero el carácter 0 y después el resto hasta que ambos terminan
// (si ambos están vacíos sigue comparando desde el índice 1, igual que el EXE).
static bool MessageTextEquals(const GxtChar* a, const GxtChar* b) {
    return a[0] == b[0] && std::strcmp(AsciiFromGxtChar(a + 1), AsciiFromGxtChar(b + 1)) == 0;
}

// Initialises messages
// 0x69EE00
void CMessages::Init() {
    ZoneScoped;

    // Como el original: limpia los 7 estilos de mensajes grandes (sin excepciones),
    // los breves y el historial, poniendo a cero solo `Text` y `StringToInsert`.
    for (auto& big : BIGMessages) {
        ClearMessagesTextAndString(big.Stack);
    }
    ClearMessagesTextAndString(BriefMessages);
    for (auto& prev : PreviousBriefs) {
        prev.Text           = nullptr;
        prev.StringToInsert = nullptr;
    }
}

template<size_t N>
tMessage* FindUnusedMsgInArray(std::array<tMessage, N>& arr) {
    for (auto& msg : arr) {
        if (!msg.IsValid()) {
            return &msg;
        }
    }
    NOTSA_LOG_DEBUG("No free brief message found!");
    return nullptr;
}

/*********************
* BRIEF MESSAGES
**********************/

/*!
* Add a new brief message.
* @param text           The text (Possibly with format characters, eg.: ~~~~
* @param flag           Flags
* @param bPreviousBrief Whenever to call `AddToPreviousBriefArray`
* @param showInstantly  Whenever to show the message instantly
* @param str            String to insert [May be null]
* @param numbers        Numbers to insert (Use -1 as placeholder)
* @notsa
*/
void CMessages::AddMessage2(const GxtChar* text, uint32 time, uint16 flag, bool bPreviousBrief, bool showInstantly, GxtChar* str, std::optional<std::array<int32, 6>> numbers) {
    const auto CreateAt = [&](tMessage* p) {
        // Como el original: se escriben campo a campo (sin tocar el relleno de la estructura)
        p->Text        = text;
        p->Flags       = flag;
        p->VisibleTime = time;
        p->CreatedAtMS = CTimer::GetTimeInMS();
        if (numbers) {
            rng::copy(*numbers, rng::begin(p->NumbersToInsert));
        } else {
            rng::fill(p->NumbersToInsert, -1);
        }
        p->StringToInsert = str;
        p->PreviousBrief  = bPreviousBrief;
        if (bPreviousBrief && (showInstantly || p == &BriefMessages.front())) {
            AddToPreviousBriefArray(
                p->Text,
                p->NumbersToInsert[0],
                p->NumbersToInsert[1],
                p->NumbersToInsert[2],
                p->NumbersToInsert[3],
                p->NumbersToInsert[4],
                p->NumbersToInsert[5],
                p->StringToInsert
            );
        }
    };
    if (showInstantly) {
        CreateAt(&BriefMessages[0]);
    } else if (const auto p = FindUnusedMsgInArray(BriefMessages)) {
        CreateAt(p);
    }
}

/*!
* Adds message to [Q]ueue
* @addr 0x69F0B0
*/
void CMessages::AddMessageQ(const GxtChar* text, uint32 time, uint16 flag, bool bPreviousBrief) { // Renamed from AddMessage
    // Como el original: prepara el texto en un búfer local de 0x320 bytes cuyo resultado no se usa
    GxtChar buf[MSG_BUF_SZ * 2];
    StringCopy(buf, text, MSG_BUF_SZ);
    InsertPlayerControlKeysInString(buf);

    AddMessage2(text, time, flag, bPreviousBrief, false);
}

/*!
* Adds message to [Q]ueue [With a string in `text` that will be replaced by `string`]
* @addr 0x69E800
*/
void CMessages::AddMessageWithStringQ(const GxtChar* text, uint32 time, uint16 flag, GxtChar* string, bool bPreviousBrief) {
    // Como el original: prepara el texto en un búfer local de 0x320 bytes cuyo resultado no se usa
    GxtChar buf[MSG_BUF_SZ * 2];
    StringCopy(buf, text, MSG_BUF_SZ);
    InsertStringInString(buf, string);
    InsertPlayerControlKeysInString(buf);

    AddMessage2(text, time, flag, bPreviousBrief, false, string);
}

/*!
* Show a message instantly
* @addr 0x69F1E0
*/
void CMessages::AddMessageJump(const GxtChar* text, uint32 time, uint16 flag, bool bPreviousBrief) { // Renamed from AddMessageJumpQ
    // Como el original: prepara el texto en un búfer local de 0x320 bytes cuyo resultado no se usa
    GxtChar buf[MSG_BUF_SZ * 2];
    StringCopy(buf, text, MSG_BUF_SZ);
    InsertPlayerControlKeysInString(buf);

    AddMessage2(text, time, flag, bPreviousBrief, true);
}

/*!
* Adds message to [Q]ueue [With numbers in `text`]
* @addr 0x69E360
*/
void CMessages::AddMessageWithNumberQ(const GxtChar* text, uint32 time, uint16 flag, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, bool bPreviousBrief) {
    // Como el original: prepara el texto en un búfer local de 0x320 bytes cuyo resultado no se usa
    GxtChar buf[MSG_BUF_SZ * 2];
    InsertNumberInString(text, n1, n2, n3, n4, n5, n6, buf);
    InsertPlayerControlKeysInString(buf);

    AddMessage2(text, time, flag, bPreviousBrief, false, nullptr, { { n1, n2, n3, n4, n5, n6 } });
}

/*!
* Show a message instantly [With numbers in `text`]
* @addr 0x69E4E0
*/
void CMessages::AddMessageJumpQWithNumber(const GxtChar* text, uint32 time, uint16 flag, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, bool bPreviousBrief) {
    // Como el original: prepara el texto en un búfer local de 0x320 bytes cuyo resultado no se usa
    GxtChar buf[MSG_BUF_SZ * 2];
    InsertNumberInString(text, n1, n2, n3, n4, n5, n6, buf);
    InsertPlayerControlKeysInString(buf);

    AddMessage2(text, time, flag, bPreviousBrief, true, nullptr, { { n1, n2, n3, n4, n5, n6 } });
}

/*!
* Show a message instantly [With a string in `text`]
* @addr 0x69E950
*/
void CMessages::AddMessageJumpQWithString(const GxtChar* text, uint32 time, uint16 flag, GxtChar* string, bool bPreviousBrief) {
    // Como el original: prepara el texto en un búfer local de 0x320 bytes cuyo resultado no se usa
    GxtChar buf[MSG_BUF_SZ * 2];
    StringCopy(buf, text, MSG_BUF_SZ);
    InsertStringInString(buf, string);
    InsertPlayerControlKeysInString(buf);

    AddMessage2(text, time, flag, bPreviousBrief, true, string);
}

/*********************
* BIG MESSAGES
**********************/

/*!
* Add a new big message.
* @param text           The text (Possibly with format characters, eg.: ~~~~)
* @param bPreviousBrief Whenever to call `AddToPreviousBriefArray`
* @param showInstantly  Whenever to show the message instantly
* @param str            String to insert [May be null]
* @param numbers        Numbers to insert (Use -1 as placeholder)
* @notsa
*/
void CMessages::AddBigMessage2(const GxtChar* text, uint32 time, eMessageStyle style, bool showInstantly, GxtChar* str, std::optional<std::array<int32, 6>> numbers) {
    const auto CreateAt = [&](tMessage* p) {
        // Como el original: se escriben campo a campo y `PreviousBrief` no se toca
        p->Text        = text;
        p->Flags       = 0;
        p->VisibleTime = time;
        p->CreatedAtMS = CTimer::GetTimeInMS();
        if (numbers) {
            rng::copy(*numbers, rng::begin(p->NumbersToInsert));
        } else {
            rng::fill(p->NumbersToInsert, -1);
        }
        p->StringToInsert = str;
    };
    if (showInstantly) {
        CreateAt(&BIGMessages[style].Stack.front());
    } else if (const auto p = FindUnusedMsgInArray(BIGMessages[style].Stack)) {
        CreateAt(p);
    }
}

// Adds big message and shows it instantly
// 0x69F2B0
void CMessages::AddBigMessage(const GxtChar* text, uint32 time, eMessageStyle style) {
    // Como el original: prepara el texto en un búfer local de 0x320 bytes cuyo resultado no se usa
    GxtChar buf[MSG_BUF_SZ * 2];
    StringCopy(buf, text, MSG_BUF_SZ);
    InsertPlayerControlKeysInString(buf);

    AddBigMessage2(text, time, style, true);
}

// Adds big message to [Q]ueue
// 0x69F370
void CMessages::AddBigMessageQ(const GxtChar* text, uint32 time, eMessageStyle style) {
    // Como el original: prepara el texto en un búfer local de 0x320 bytes cuyo resultado no se usa
    GxtChar buf[MSG_BUF_SZ * 2];
    StringCopy(buf, text, MSG_BUF_SZ);
    InsertPlayerControlKeysInString(buf);

    AddBigMessage2(text, time, style, false);
}

// Adds big message with numbers and shows it instantly
// 0x69E5F0
void CMessages::AddBigMessageWithNumber(const GxtChar* text, uint32 time, eMessageStyle style, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6) {
    // Como el original: prepara el texto en un búfer local de 0x320 bytes cuyo resultado no se usa
    GxtChar buf[MSG_BUF_SZ * 2];
    InsertNumberInString(text, n1, n2, n3, n4, n5, n6, buf);
    InsertPlayerControlKeysInString(buf);

    AddBigMessage2(text, time, style, true, nullptr, {{n1, n2, n3, n4, n5, n6}});
}

// Adds big message with numbers to [Q]ueue
// 0x69E6E0
void CMessages::AddBigMessageWithNumberQ(const GxtChar* text, uint32 time, eMessageStyle style, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6) {
    // Como el original: prepara el texto en un búfer local de 0x320 bytes cuyo resultado no se usa
    GxtChar buf[MSG_BUF_SZ * 2];
    InsertNumberInString(text, n1, n2, n3, n4, n5, n6, buf);
    InsertPlayerControlKeysInString(buf);

    AddBigMessage2(text, time, style, false, nullptr, { {n1, n2, n3, n4, n5, n6} });
}

// Adds message to previous brief
// 0x69DD50
void CMessages::AddToPreviousBriefArray(const GxtChar* text, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, const GxtChar* string) {
    const auto numbers = std::array{ n1, n2, n3, n4, n5, n6 };

    // Find unused entry + duplicate check
    size_t i = 0;
    for (; i < PreviousBriefs.size(); i++) {
        const auto& prev = PreviousBriefs[i];
        if (!prev.Text) {
            break; // Found unused
        }
        // Check if it's a duplicate of the one we're about to insert
        if (prev.Text != text || prev.StringToInsert != string) { // I mean that's a pretty naive way, but okay
            continue;
        }
        if (numbers != prev.NumbersToInsert) {
            continue;
        }
        return; // Duplicate messages after each other, so skip
    }

    // In case no free entries were found we overwrite the last one
    if (i == PreviousBriefs.size()) {
        i = PreviousBriefs.size() - 1;
    }

    // Como el original: copia [i-1] -> [i], ..., [0] -> [1] (la entrada `i` recibe la `i-1`),
    // dejando libre la 0. Si `i == 0` no hace nada.
    std::shift_right(
        PreviousBriefs.begin(),
        PreviousBriefs.begin() + i + 1,
        1
    );

    // Now construct at the 0th entry (as it's now freed up)
    new (&PreviousBriefs[0]) tPreviousBrief {
        .Text            = text,
        .NumbersToInsert = numbers,
        .StringToInsert  = string,
    };
}

// Removes registered messages
// 0x69DCD0
void CMessages::ClearMessages(bool bIgnoreMissionTitle) {
    // Clear all big messages except mission titles if bIgnoreMissionTitle is true
    for (eMessageStyle style = eMessageStyle::STYLE_MIDDLE; style < eMessageStyle::NUM_MESSAGE_STYLES; style = static_cast<eMessageStyle>(style + 1)) {
        // Skip STYLE_BOTTOM_RIGHT and STYLE_MIDDLE_SMALLER_HIGHER if ignoring mission titles
        if (bIgnoreMissionTitle && (style == eMessageStyle::STYLE_BOTTOM_RIGHT || style == eMessageStyle::STYLE_MIDDLE_SMALLER_HIGHER)) {
            continue;
        }
        ClearMessagesTextAndString(BIGMessages[style].Stack);
    }

    // Clear brief messages (Como el original: en línea, sin llamar a ClearSmallMessagesOnly)
    ClearMessagesTextAndString(BriefMessages);
}

// Removes messages in brief
// 0x69DD30
void CMessages::ClearSmallMessagesOnly() {
    ClearMessagesTextAndString(BriefMessages);
}

// Remove messages from previous brief
// 0x69DE70
void CMessages::ClearPreviousBriefArray() {
    // Como el original: solo se ponen a cero `Text` y `StringToInsert`; los números se conservan
    for (auto& prev : PreviousBriefs) {
        prev.Text           = nullptr;
        prev.StringToInsert = nullptr;
    }
}

// Removes small message with this text
// 0x69EA30
void CMessages::ClearThisPrint(const GxtChar* text) {
    bool found;
    do {
        // Como el original: busca desde el principio hasta la primera entrada libre
        found = false;
        int16 i = 0;
        for (; i < (int16)BriefMessages.size(); i++) {
            const auto msgText = BriefMessages[i].Text;
            if (!msgText) {
                break;
            }
            if (MessageTextEquals(msgText, text)) {
                found = true;
                break;
            }
        }
        if (!found) {
            break;
        }

        RemoveMessageAt(BriefMessages, i);

        if (i == 0) {
            auto& msg = BriefMessages[0];
            msg.CreatedAtMS = CTimer::GetTimeInMS();
            // Como el original: solo si el nuevo primero existe y tiene `PreviousBrief`
            if (msg.Text && msg.PreviousBrief) {
                AddToPreviousBriefArray(
                    msg.Text,
                    msg.NumbersToInsert[0],
                    msg.NumbersToInsert[1],
                    msg.NumbersToInsert[2],
                    msg.NumbersToInsert[3],
                    msg.NumbersToInsert[4],
                    msg.NumbersToInsert[5],
                    msg.StringToInsert
                );
            }
        }
    } while (found);
}

// Removes big message with this text
// 0x69EBE0
void CMessages::ClearThisBigPrint(const GxtChar* text) {
    bool found;
    do {
        // Como el original: recorre los 4 huecos de cada estilo sin detenerse en los libres
        found = false;
        int16 style = 0, i = 0;
        for (;;) {
            const auto msgText = BIGMessages[style].Stack[i].Text;
            if (msgText && MessageTextEquals(msgText, text)) {
                found = true;
                break;
            }
            if (++i == (int16)BIGMessages[style].Stack.size()) {
                i = 0;
                style++;
            }
            if (style >= NUM_MESSAGE_STYLES) {
                break;
            }
        }
        if (!found) {
            break;
        }

        auto& stack = BIGMessages[style].Stack;
        RemoveMessageAt(stack, i);
        if (i == 0) {
            stack[0].CreatedAtMS = CTimer::GetTimeInMS();
        }
    } while (found);
}

// Removes first big message in messages stack
// 0x69ED80
void CMessages::ClearThisPrintBigNow(eMessageStyle style) {
    if (auto& msg = BIGMessages[style].Stack[0]; msg.Text) {
        ClearThisBigPrint(msg.Text);
    }
    CHud::BigMessageX[style] = 0.f;
    CHud::m_BigMessage[style][0] = 0;
}

// Removes all displayed messages
// 0x69EDC0
void CMessages::ClearAllMessagesDisplayedByGame(bool unk) {
    ZoneScoped;

    ClearMessages(unk);
    ClearPreviousBriefArray();
    CHud::GetRidOfAllHudMessages(unk);
}

// Returns length of a string
// 0x69DB50
uint32 CMessages::GetStringLength(const GxtChar* string) {
    return std::strlen(AsciiFromGxtChar(string));
}

// Copies string src to dest
// 0x69DB70
void CMessages::StringCopy(GxtChar* dest, const GxtChar* src, uint16 len) {
    uint16 i = 0;
    if (src) {
        for (; (int32)i < (int32)len - 1 && src[i]; i++) {
            dest[i] = src[i];
        }
    } else {
        // Como el original: sin origen, pone a cero `len` bytes (al menos 1)
        for (; (int32)i < (int32)len - 1; i++) {
            dest[i] = '\0';
        }
    }
    dest[i] = '\0';
}

/*!
* Check if 2 string are equal
* @addr 0x69DBD0
*/
bool CMessages::StringCompare(const GxtChar* str1, const GxtChar* str2, uint16 len) {
    return std::strncmp(AsciiFromGxtChar(str1), AsciiFromGxtChar(str2), len) == 0;
}

// 0x69DC50
int16 CMessages::CutString(int32 count, GxtChar* str, GxtChar** dest) {
    // Como el original: trocea `str` en un máximo de 8 partes de unos `count` caracteres,
    // cortando en el espacio anterior (que se sustituye por '\0'). Contadores de 16 bits con signo.
    const auto len    = (int16)std::strlen(AsciiFromGxtChar(str));
    int16      pieces = 0;
    if (len > 0) {
        int16 pos = 0;
        do {
            if (pieces >= 8) {
                break;
            }
            dest[pieces++] = &str[pos];
            if ((int32)(int16)count + (int32)pos >= (int32)len) {
                break;
            }
            pos = (int16)(pos + count);
            while (str[pos] != ' ') {
                pos--;
            }
            str[pos++] = '\0';
        } while (pos < len);
    }
    return pieces;
}

// Insert numbers into string
// 0x69DE90
void CMessages::InsertNumberInString(const GxtChar* str, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, GxtChar* dst) {
    if (!str) { // Appereantly that's valid?
        *dst = 0; // Null terminate result anyways
        return;
    }

    // Como el original: cada número se formatea con sprintf("%d") y se pasa por AsciiToGxtChar.
    // Tras el sexto `~1~` se reutiliza el último número formateado; el contador es de 8 bits.
    const int32 numbers[] = { n1, n2, n3, n4, n5, n6 };
    char        numAscii[12];
    GxtChar     numGxt[12];
    uint8       numIdx = 0;

    std::sprintf(numAscii, "%d", numbers[0]);
    auto numLen = (uint32)std::strlen(numAscii);
    AsciiToGxtChar(numAscii, numGxt);

    const auto len = (uint32)std::strlen(AsciiFromGxtChar(str));
    uint32     d   = 0;
    for (uint32 s = 0; s < len;) {
        if (str[s] == '~' && str[s + 1] == '1' && str[s + 2] == '~') {
            s += 3;
            if (numLen) {
                std::memcpy(&dst[d], numGxt, numLen);
                d += numLen;
            }
            numIdx++;
            if (numIdx >= 1 && numIdx <= 5) {
                std::sprintf(numAscii, "%d", numbers[numIdx]);
            }
            numLen = (uint32)std::strlen(numAscii);
            AsciiToGxtChar(numAscii, numGxt);
        } else {
            dst[d++] = str[s++];
        }
    }
    dst[d] = 0;
}

// Inserts string into src
// 0x69E040
void CMessages::InsertStringInString(GxtChar* target, const GxtChar* replacement) {
    if (!target || !replacement) { // Appereantly that's valid?
        return;
    }

    // Como el original: el marcador es `~a~`; el bucle avanza hasta que el índice de destino
    // alcanza len(target) + len(replacement) (contadores de 16 bits), aunque eso copie bytes
    // posteriores al terminador. Después se copia el resultado y se rellena con ceros hasta 400.
    const auto replLen = (uint16)std::strlen(AsciiFromGxtChar(replacement));
    const auto total   = (uint16)(std::strlen(AsciiFromGxtChar(target)) + std::strlen(AsciiFromGxtChar(replacement)));

    GxtChar buf[MSG_BUF_SZ];
    uint16  s = 0, d = 0;
    while (d < total) {
        const auto c = target[s];
        if (c == '~' && target[s + 1] == 'a' && target[s + 2] == '~') {
            s += 3;
            for (uint16 i = 0; i < replLen; i++) {
                buf[d++] = replacement[i];
            }
        } else {
            buf[d++] = c;
            s++;
        }
    }
    buf[d] = 0;

    std::memcpy(target, buf, total);
    if (total < MSG_BUF_SZ) {
        std::memset(&target[total], 0, MSG_BUF_SZ - total);
    }
}

// Inserts key events into string
// 0x69E160
void CMessages::InsertPlayerControlKeysInString(GxtChar* string) {
    if (!string) {
        return;
    }

    // Como el original: detecta `~k~` y salta 4 caracteres (`~k~~`); busca por orden la primera
    // acción cuyo nombre coincide (StringCompare) y va seguida de `~`. Si ninguna coincide, el
    // `~k~~` se descarta y el resto se copia tal cual. Después se rellena con ceros hasta 400.
    const auto len = (uint16)std::strlen(AsciiFromGxtChar(string));

    GxtChar keys[MSG_BUF_SZ]{};
    GxtChar buf[MSG_BUF_SZ];
    uint16  d = 0;
    for (uint16 s = 0; s < len;) {
        const auto c = string[s];
        if (c == '~' && string[s + 1] == 'k' && string[s + 2] == '~') {
            s += 4;
            bool found = false;
            for (uint16 action = 0; !found && action < +eControllerAction::CONTROLLER_ACTION_COUNT; action++) {
                const GxtChar* name    = ControlsManager.m_ControllerActionName[action];
                const auto     nameLen = (uint16)std::strlen(AsciiFromGxtChar(name));
                if (!nameLen) {
                    continue;
                }
                if (!StringCompare(&string[s], name, nameLen) || string[s + nameLen] != '~') {
                    continue;
                }
                found = true;
                ControlsManager.GetGxtStringOfCommandKeys((eControllerAction)action, keys, MSG_BUF_SZ);
                s = s + nameLen + 1;

                // Como el original: se copia y a la vez se limpia el búfer de teclas
                const auto keysLen = (uint16)std::strlen(AsciiFromGxtChar(keys));
                for (uint16 i = 0; i < keysLen; i++) {
                    buf[d++] = keys[i];
                    keys[i]  = 0;
                }
            }
        } else {
            buf[d++] = c;
            s++;
        }
    }
    buf[d] = 0;

    const auto outLen = (uint16)std::strlen(AsciiFromGxtChar(buf));
    std::memcpy(string, buf, outLen);
    if (outLen < MSG_BUF_SZ) {
        std::memset(&string[outLen], 0, MSG_BUF_SZ - outLen);
    }
}

// Processing messages. This is called from CWorld::Process
// 0x69EE60
void CMessages::Process() {
    // Process big messages
    for (auto& big : BIGMessages) {
        auto& f = big.Stack[0];
        if (!f.Text || CTimer::GetTimeInMS() <= f.GetTimeToDisappearAtMS()) {
            continue;
        }
        RemoveMessageAt(big.Stack, 0);
        // Como el original: se reinicia el tiempo del primero aunque haya quedado vacío
        f.CreatedAtMS = CTimer::GetTimeInMS();
    }

    // Process briefs
    auto& f = BriefMessages[0];
    if (!f.Text || CTimer::GetTimeInMS() <= f.GetTimeToDisappearAtMS()) {
        return;
    }
    RemoveMessageAt(BriefMessages, 0);
    f.CreatedAtMS = CTimer::GetTimeInMS();
    // Como el original: solo al avanzar la cola y si el nuevo primero tiene `PreviousBrief`
    if (f.Text && f.PreviousBrief) {
        AddToPreviousBriefArray(
            f.Text,
            f.NumbersToInsert[0],
            f.NumbersToInsert[1],
            f.NumbersToInsert[2],
            f.NumbersToInsert[3],
            f.NumbersToInsert[4],
            f.NumbersToInsert[5],
            f.StringToInsert
        );
    }
}

// Displays messages
// 0x69EFC0
void CMessages::Display(bool bNotFading) {
    GxtChar buff[MSG_BUF_SZ];

    // Helper function to process text for display
    const auto ProcessMessageText = [&buff](const tMessage& msg) {
        InsertNumberInString(msg.Text, msg.NumbersToInsert[0], msg.NumbersToInsert[1], msg.NumbersToInsert[2], msg.NumbersToInsert[3], msg.NumbersToInsert[4], msg.NumbersToInsert[5], buff);
        InsertStringInString(buff, msg.StringToInsert);
        InsertPlayerControlKeysInString(buff);
    };

    // Display big messages if not fading
    if (bNotFading) {
        for (eMessageStyle style = STYLE_MIDDLE; style < NUM_MESSAGE_STYLES; style = static_cast<eMessageStyle>(style + 1)) {
            const auto& msg = BIGMessages[style].Stack[0];
            // Please dont check for IsValid here, that isnt correct, because if
            // the array is empty, the first element is still valid, but not initialized
            ProcessMessageText(msg);
            CHud::SetBigMessage(buff, style);
        }
    }

    // Display brief messages based on script fading condition
    if (bNotFading == CTheScripts::bDrawSubtitlesBeforeFade) {
        const auto& msg = BriefMessages[0];
        ProcessMessageText(msg);
        CHud::SetMessage(buff);
    }
}
