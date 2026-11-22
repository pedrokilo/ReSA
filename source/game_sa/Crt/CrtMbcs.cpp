#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <cstdint>
#include <cstring>
#include "CrtMbcs.h"
#include "CrtState.h"
#include "CrtHeap.h"
#include "CrtMem.h"
#include "CrtLocale.h"

// Ver CrtMbcs.h. Cada funcion lleva la direccion del EXE y sigue su desensamblado.

namespace ExeCrt {
namespace {
inline uint8_t*       MbcType()        { return reinterpret_cast<uint8_t*>(0xC9C0A0); }          // 0x101 bytes; el byte [c + 1] lleva las banderas del caracter c
inline uint8_t*       MbcCaseMap()     { return reinterpret_cast<uint8_t*>(0xC9C1C0); }          // 0x100 bytes
inline unsigned int&  MbCodePage()     { return *reinterpret_cast<unsigned int*>(0xC9C1A4); }    // __mbcodepage
inline int&           IsMbCodePage()   { return *reinterpret_cast<int*>(0xC9C088); }             // __ismbcodepage
inline unsigned int&  MbLcid()         { return *reinterpret_cast<unsigned int*>(0xC9C080); }    // __mblcid
inline uint16_t*      MbUlInfo()       { return reinterpret_cast<uint16_t*>(0xC9C1B0); }         // __mbulinfo (12 bytes; el bloque por hilo guarda 5 palabras)
inline MbcInfo*&      PtMbcInfo()      { return *reinterpret_cast<MbcInfo**>(0xC9C084); }        // __ptmbcinfo
inline int&           SystemSet()      { return *reinterpret_cast<int*>(0xC9AF8C); }             // _bSystemSet: hubo -2/-3/-4
inline int&           MbcInitialized() { return *reinterpret_cast<int*>(0xC9D430); }             // __mbctype_initialized
inline unsigned int&  LocaleCodePage() { return *reinterpret_cast<unsigned int*>(0xC9AF40); }    // __lc_codepage
constexpr int         MB_CP_LOCK = 0xD;
constexpr unsigned int CP_TABLE_COUNT = 5; // el bucle del EXE recorre 0xF0 / 0x30 entradas

// Entrada de la tabla de paginas conocidas (0x8E39F0, 0x30 bytes): pagina, ulinfo (12 bytes) y 4 grupos de 4 rangos [bajo, alto] de bytes iniciales
struct CpEntry {
    unsigned int  codePage;       // +0x00
    uint16_t      ulInfo[6];      // +0x04
    uint8_t       ranges[4][8];   // +0x10
};
static_assert(sizeof(CpEntry) == 0x30);
inline const CpEntry* CpTable()     { return reinterpret_cast<const CpEntry*>(0x8E39F0); }
inline const uint8_t* CpGroupFlag() { return reinterpret_cast<const uint8_t*>(0x8E39E8); } // bandera OR por grupo (_M1, _M2...)

// Bloqueo del cerrojo 0xD durante toda la funcion (el EXE lo suelta en un funclet de salida, 0x83348B / 0x833195)
struct MbLockGuard {
    MbLockGuard()  { Lock(MB_CP_LOCK); }
    ~MbLockGuard() { Unlock(MB_CP_LOCK); }
    MbLockGuard(const MbLockGuard&) = delete;
    MbLockGuard& operator=(const MbLockGuard&) = delete;
};

// 0x832F45 (usercall, pagina en EAX): LCID de las cuatro paginas de Asia oriental que conoce el CRT; 0 para el resto
unsigned int MbcpToLcid(unsigned int codePage) {
    switch (codePage) {
    case 0x3A4: return 0x411; // japones (932)
    case 0x3A8: return 0x804; // chino simplificado (936)
    case 0x3B5: return 0x412; // coreano (949)
    case 0x3B6: return 0x404; // chino tradicional (950)
    default:    return 0;
    }
}
} // namespace

// 0x832F74
void __cdecl SetSbcs() {
    std::memset(MbcType(), 0, 0x101);
    MbCodePage()   = 0;
    IsMbCodePage() = 0;
    MbLcid()       = 0;
    std::memset(MbUlInfo(), 0, 12);
}

// 0x832F9D: tipo de cada caracter (GetStringTypeA) y su version en minuscula o mayuscula (LCMapStringA) para la pagina actual; sin informacion de la pagina solo se reconocen A-Z y a-z
void __cdecl SetSbUpLow() {
    CPINFO info;
    if (GetCPInfo(MbCodePage(), &info) == 1) {
        uint8_t        chars[256];
        uint8_t        lower[256];
        uint8_t        upper[256];
        unsigned short types[256];
        for (int i = 0; i < 256; ++i) {
            chars[i] = static_cast<uint8_t>(i);
        }
        chars[0] = 0x20;
        // los rangos de bytes iniciales (pares [bajo, alto], terminados en un bajo nulo) se sustituyen por espacios para que el sistema no los clasifique
        if (uint8_t low = info.LeadByte[0]) {
            const uint8_t* p = &info.LeadByte[1];
            do {
                const uint8_t high = *p;
                if (low <= high) {
                    std::memset(chars + low, 0x20, static_cast<size_t>(high) - low + 1);
                }
                ++p;
                low = *p;
                ++p;
            } while (low);
        }
        CrtGetStringTypeA(1, reinterpret_cast<const char*>(chars), 256, types, static_cast<int>(MbCodePage()), static_cast<int>(MbLcid()), 0);
        CrtLCMapStringA(MbLcid(), 0x100, reinterpret_cast<const char*>(chars), 256, reinterpret_cast<char*>(lower), 256, static_cast<int>(MbCodePage()), 0); // LCMAP_LOWERCASE
        CrtLCMapStringA(MbLcid(), 0x200, reinterpret_cast<const char*>(chars), 256, reinterpret_cast<char*>(upper), 256, static_cast<int>(MbCodePage()), 0); // LCMAP_UPPERCASE
        for (int i = 0; i < 256; ++i) {
            if (types[i] & 1) {           // C1_UPPER
                MbcType()[i + 1] |= 0x10; // _SBUP
                MbcCaseMap()[i] = lower[i];
            } else if (types[i] & 2) {    // C1_LOWER
                MbcType()[i + 1] |= 0x20; // _SBLOW
                MbcCaseMap()[i] = upper[i];
            } else {
                MbcCaseMap()[i] = 0;
            }
        }
    } else {
        for (unsigned int i = 0; i < 256; ++i) {
            if (i >= 'A' && i <= 'Z') {
                MbcType()[i + 1] |= 0x10;
                MbcCaseMap()[i] = static_cast<uint8_t>(i + 0x20);
            } else if (i >= 'a' && i <= 'z') {
                MbcType()[i + 1] |= 0x20;
                MbcCaseMap()[i] = static_cast<uint8_t>(i - 0x20);
            } else {
                MbcCaseMap()[i] = 0;
            }
        }
    }
}

// 0x83319E (setmbcp_nolock). 0 = bien, -1 = pagina no valida (y nunca hubo -2/-3/-4). Pagina 0: un solo byte.
int __cdecl SetmbcpCore(int codePage) {
    const unsigned int cp = static_cast<unsigned int>(codePage);
    if (cp != 0) {
        // pagina conocida: tabla del EXE
        for (unsigned int index = 0; index < CP_TABLE_COUNT; ++index) {
            const CpEntry& entry = CpTable()[index];
            if (entry.codePage != cp) {
                continue;
            }
            std::memset(MbcType(), 0, 0x101);
            for (unsigned int group = 0; group < 4; ++group) {
                const uint8_t* range = &entry.ranges[0][0] + group * 8;
                for (uint8_t low = *range; low; low = *range) {
                    const uint8_t high = range[1];
                    if (!high) {
                        break;
                    }
                    for (unsigned int c = low; c <= high; ++c) {
                        MbcType()[c + 1] |= CpGroupFlag()[group];
                    }
                    range += 2;
                }
            }
            MbCodePage()   = cp;
            IsMbCodePage() = 1;
            const unsigned int lcid = MbcpToLcid(cp);
            std::memcpy(MbUlInfo(), entry.ulInfo, 12);
            MbLcid() = lcid;
            SetSbUpLow();
            return 0;
        }

        // pagina desconocida: se pide al sistema (GetCPInfo)
        CPINFO info;
        if (GetCPInfo(cp, &info) != 1) {
            if (SystemSet() == 0) {
                return -1;
            }
        } else {
            std::memset(MbcType(), 0, 0x101);
            MbCodePage() = cp;
            MbLcid()     = 0;
            if (info.MaxCharSize > 1) {
                if (info.LeadByte[0] != 0) {
                    for (const uint8_t* p = &info.LeadByte[1]; *p != 0;) {
                        const uint8_t low  = p[-1];
                        const uint8_t high = *p;
                        for (unsigned int c = low; c <= high; ++c) {
                            MbcType()[c + 1] |= 4; // _M1: byte inicial
                        }
                        p += 2;
                        if (p[-1] == 0) {
                            break;
                        }
                    }
                }
                for (unsigned int c = 1; c < 0xFF; ++c) {
                    MbcType()[c + 1] |= 8; // _M2: byte final
                }
                MbLcid()       = MbcpToLcid(cp);
                IsMbCodePage() = 1;
            } else {
                IsMbCodePage() = 0;
            }
            std::memset(MbUlInfo(), 0, 12);
            SetSbUpLow();
            return 0;
        }
    }
    // pagina 0, o pagina no valida con -2/-3/-4 anteriores: un solo byte
    SetSbcs();
    SetSbUpLow();
    return 0;
}

// 0x833344 (_setmbcp). Bajo el bloqueo 0xD. Con la pagina ya activa no hace nada (devuelve 0); si la informacion global ya no es compartida (cuenta distinta de 0) reserva otra de 0x220 bytes.
int __cdecl Setmbcp(int codePage) {
    int result = -1;
    MbLockGuard guard;
    SystemSet() = 0;
    unsigned int cp = static_cast<unsigned int>(codePage);
    if (codePage == -2) {
        SystemSet() = 1;
        cp = GetOEMCP();
    } else if (codePage == -3) {
        SystemSet() = 1;
        cp = GetACP();
    } else if (codePage == -4) {
        SystemSet() = 1;
        cp = LocaleCodePage();
    }
    if (cp == MbCodePage()) {
        return 0;
    }
    MbcInfo* block = PtMbcInfo();
    if (!block || block->refCount != 0) {
        block = static_cast<MbcInfo*>(Malloc(0x220));
    }
    if (block) {
        result = SetmbcpCore(static_cast<int>(cp));
        if (result == 0) {
            block->refCount     = 0;
            block->codePage     = MbCodePage();
            block->isMbCodePage = IsMbCodePage();
            block->lcid         = MbLcid();
            for (int i = 0; i < 5; ++i) {
                block->ulInfo[i] = MbUlInfo()[i];
            }
            std::memcpy(block->mbcType, MbcType(), 0x101);
            std::memcpy(block->caseMap, MbcCaseMap(), 0x100);
            PtMbcInfo() = block;
        }
    }
    if (result == -1 && block != PtMbcInfo()) {
        Free(block);
    }
    return result;
}

// 0x833494 (__initmbctable): sin gancho, la ejecutan __setargv y la tabla C de __cinit antes de que exista la DLL
int __cdecl InitMbcTable() {
    if (MbcInitialized() == 0) {
        Setmbcp(-3);
        MbcInitialized() = 1;
    }
    return 0;
}

// 0x83312F (__updatetmbcinfo): el hilo toma el bloque global; el que dejaba se libera si nadie mas lo usa
MbcInfo* __cdecl UpdateTMbcInfo() {
    MbLockGuard guard;
    ThreadData* const ptd = Getptd();
    MbcInfo* current = static_cast<MbcInfo*>(ptd->mbcInfo);
    if (current != PtMbcInfo()) {
        if (current && --current->refCount == 0) {
            Free(current);
        }
        ptd->mbcInfo = PtMbcInfo();
        current      = PtMbcInfo();
        ++current->refCount;
    }
    return current;
}

// 0x836697 (_mbschr, version del EXE sin _locale_t)
char* __cdecl Mbschr(const char* str, unsigned int ch) {
    MbcInfo* mbc = static_cast<MbcInfo*>(Getptd()->mbcInfo);
    if (mbc != PtMbcInfo()) {
        mbc = UpdateTMbcInfo();
    }
    if (mbc->isMbCodePage == 0) {
        return Strchr(str, static_cast<int>(ch));
    }
    const uint8_t* p = reinterpret_cast<const uint8_t*>(str);
    unsigned int   c = *p;
    while (c != 0) {
        if (mbc->mbcType[c + 1] & 4) { // byte inicial: se compara el par
            ++p;
            const unsigned int trail = *p;
            if (trail == 0) {
                return nullptr;
            }
            if (ch == ((c << 8) | trail)) {
                return const_cast<char*>(reinterpret_cast<const char*>(p - 1));
            }
        } else if (ch == c) {
            break;
        }
        ++p;
        c = *p;
    }
    return ch == c ? const_cast<char*>(reinterpret_cast<const char*>(p)) : nullptr;
}

// 0x834B6E
int __cdecl Strncnt(const char* str, int count) {
    if (count == 0) {
        return 0;
    }
    int left = count;
    do {
        --left;
        if (*str == '\0') {
            return count - left - 1;
        }
        ++str;
    } while (left != 0);
    return count;
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtMbcsInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Setmbcp, 0x833344);
    RH_ScopedGlobalInstall(ExeCrt::SetmbcpCore, 0x83319E);
    RH_ScopedGlobalInstall(ExeCrt::SetSbcs, 0x832F74);
    RH_ScopedGlobalInstall(ExeCrt::SetSbUpLow, 0x832F9D);
    RH_ScopedGlobalInstall(ExeCrt::UpdateTMbcInfo, 0x83312F);
    RH_ScopedGlobalInstall(ExeCrt::Mbschr, 0x836697);
}
#endif
