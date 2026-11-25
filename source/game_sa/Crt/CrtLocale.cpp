#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <cstring>
#include "CrtLocale.h"
#include "CrtState.h"
#include "CrtHeap.h"
#include "CrtLowIo.h"
#include "CrtCtype.h"

// Reconstruccion del locale del CRT del EXE (lote CRT4A, continuacion 21/11/2026). Ver CrtLocale.h y reversing/FnCRT4A/README.md.
// Cada funcion lleva la direccion del EXE y sigue el desensamblado. Los bloques con _alloca protegidos por SEH del original (filtro EXECUTE_HANDLER +
// _resetstkoflw, y reserva con malloc si la pila no alcanza) pasan a un bufer temporal (TempBuf): pila hasta 1 KiB, malloc del EXE por encima.

namespace ExeCrt {
namespace {
// Funciones del montón del EXE (no reconstruidas en este lote: se llaman por su direccion)
inline void  LocFree(void* p)             { Free(p); } // free 0x82413F (CrtHeap.cpp)
inline void* LocMalloc(unsigned int n)    { return reinterpret_cast<void*(__cdecl*)(unsigned int)>(0x824257)(n); }
inline void* LocCalloc(unsigned int n, unsigned int s) { return reinterpret_cast<void*(__cdecl*)(unsigned int, unsigned int)>(0x824416)(n, s); }
inline int   LocAtol(const char* s)       { return reinterpret_cast<int(__cdecl*)(const char*)>(0x822506)(s); }

// Datos globales del CRT del EXE
inline LocInfo*&  GlobalLocInfo()      { return *reinterpret_cast<LocInfo**>(0x8E2F14); }
constexpr unsigned int INITIAL_LOCINFO = 0x8E2EC0;
inline int&       GetStringTypeWMode() { return *reinterpret_cast<int*>(0xC9AF48); } // 0 sin probar, 1 API ancha, 2 solo ANSI
inline int&       LCMapStringWMode()   { return *reinterpret_cast<int*>(0xC9ADC8); }
inline unsigned int& LcCodePage()      { return *reinterpret_cast<unsigned int*>(0xC9AF40); }
inline unsigned int& LcHandleCtype()   { return *reinterpret_cast<unsigned int*>(0xC9AF30); }
constexpr unsigned int EILSEQ_CODE = 0x2A;
inline const wchar_t* EmptyWide() { return reinterpret_cast<const wchar_t*>(0x88BE7C); } // L"" (cadena ancha de un cero) del EXE

inline int Field(const LocInfo* li, int offset) { int v; std::memcpy(&v, reinterpret_cast<const char*>(li) + offset, 4); return v; }

// Bloque de locale del hilo; lo pide con __updatetlocinfo si no es el global (patron comun de las funciones de clasificacion)
inline LocInfo* ThreadLocInfo() {
    LocInfo* li = static_cast<LocInfo*>(Getptd()->locInfo);
    if (li != GlobalLocInfo()) {
        li = UpdateTLocInfo();
    }
    return li;
}

// Cerrojo 0xC (locale) como el try/finally de __updatetlocinfo
struct LocaleLockGuard {
    LocaleLockGuard()  { Lock(0xC); }
    ~LocaleLockGuard() { Unlock(0xC); }
};

// Bufer temporal: el original usa _alloca (con filtro SEH para el desbordamiento de pila) y malloc como alternativa
class TempBuf {
public:
    explicit TempBuf(unsigned int size, bool zero) {
        if (size <= sizeof m_stack) {
            m_ptr = m_stack;
            if (zero) {
                std::memset(m_stack, 0, size);
            }
        } else {
            m_ptr = static_cast<unsigned char*>(LocMalloc(size));
            m_heap = m_ptr != nullptr;
            if (m_ptr && zero) {
                std::memset(m_ptr, 0, size);
            }
        }
    }
    ~TempBuf() {
        if (m_heap) {
            LocFree(m_ptr);
        }
    }
    TempBuf(const TempBuf&) = delete;
    TempBuf& operator=(const TempBuf&) = delete;
    unsigned char* get() const { return m_ptr; }
private:
    unsigned char  m_stack[0x400];
    unsigned char* m_ptr = nullptr;
    bool           m_heap = false;
};
} // namespace

// 0x82524D: engancha el bloque global al hilo (cuentas de referencias de las partes compartidas) y libera el anterior si se queda sin referencias
LocInfo* __cdecl UpdateTLocInfoNoLock() {
    ThreadData* const ptd = Getptd();
    LocInfo* const old = static_cast<LocInfo*>(ptd->locInfo);
    if (old != GlobalLocInfo()) {
        if (old) {
            old->refCount--;
            if (old->lconvIntlRef) { (*old->lconvIntlRef)--; }
            if (old->lconvMonRef)  { (*old->lconvMonRef)--; }
            if (old->lconvNumRef)  { (*old->lconvNumRef)--; }
            if (old->ctype1Ref)    { (*old->ctype1Ref)--; }
            reinterpret_cast<int*>(static_cast<char*>(old->timeCur) + 0xB4)[0]--;
        }
        LocInfo* const g = GlobalLocInfo();
        ptd->locInfo = g;
        g->refCount++;
        if (g->lconvIntlRef) { (*g->lconvIntlRef)++; }
        if (g->lconvMonRef)  { (*g->lconvMonRef)++; }
        if (g->lconvNumRef)  { (*g->lconvNumRef)++; }
        if (g->ctype1Ref)    { (*g->ctype1Ref)++; }
        reinterpret_cast<int*>(static_cast<char*>(g->timeCur) + 0xB4)[0]++;
        if (old && old->refCount == 0 && reinterpret_cast<unsigned int>(old) != INITIAL_LOCINFO) {
            FreeTLocInfo(old);
        }
    }
    return static_cast<LocInfo*>(ptd->locInfo);
}

// 0x825456
LocInfo* __cdecl UpdateTLocInfo() {
    LocaleLockGuard guard;
    return UpdateTLocInfoNoLock();
}

// 0x825183
void __cdecl FreeTLocInfo(LocInfo* li) {
    void* const defaultLconv = *reinterpret_cast<void**>(0xC9AF00);
    if (li->lconv != defaultLconv && li->lconv != nullptr && *li->lconvIntlRef == 0) {
        int* const mon = li->lconvMonRef;
        if (mon && *mon == 0 && mon != *reinterpret_cast<int**>(0xC9C2C8)) {
            LocFree(mon);
            FreeLconvMon(li->lconv);
        }
        int* const num = li->lconvNumRef;
        if (num && *num == 0 && num != *reinterpret_cast<int**>(0xC9C2D0)) {
            LocFree(num);
            FreeLconvNum(li->lconv);
        }
        LocFree(li->lconvIntlRef);
        LocFree(li->lconv);
    }
    int* const ctype1Ref = li->ctype1Ref;
    if (ctype1Ref != *reinterpret_cast<int**>(0xC9C2C4) && ctype1Ref != nullptr && *ctype1Ref == 0) {
        LocFree(ctype1Ref);
        LocFree(li->ctype1);
    }
    void* const timeOwn = li->timeOwn;
    if (timeOwn != *reinterpret_cast<void**>(0xC9AEFC) && timeOwn != nullptr && *reinterpret_cast<int*>(static_cast<char*>(timeOwn) + 0xB4) == 0) {
        FreeLcTime(timeOwn);
        LocFree(li->timeOwn);
    }
    LocFree(li);
}

// 0x82FBE7: los 43 punteros de la estructura lc_time (el orden de liberacion es el del EXE)
void __cdecl FreeLcTime(void* lcTime) {
    if (!lcTime) {
        return;
    }
    char** const f = static_cast<char**>(lcTime);
    static const unsigned char kOrder[] = { 1, 2, 3, 4, 5, 6, 0, 8, 9, 10, 11, 12, 13, 7, 14, 15 };
    for (const unsigned char i : kOrder) {
        LocFree(f[i]);
    }
    for (int i = 16; i < 43; i++) {
        LocFree(f[i]);
    }
}

// 0x82FE09: decimal_point, thousands_sep, grouping; solo se liberan si no son los del lconv actual ni los de "C"
void __cdecl FreeLconvNum(void* lconv) {
    if (!lconv) {
        return;
    }
    char** const f = static_cast<char**>(lconv);
    for (int i = 0; i < 3; i++) {
        char** const cur = *reinterpret_cast<char***>(0x8E37BC);
        if (f[i] != cur[i] && f[i] != *reinterpret_cast<char**>(0x8E378C + i * 4)) {
            LocFree(f[i]);
        }
    }
}

// 0x830068: los siete campos monetarios de texto (indices 3..9)
void __cdecl FreeLconvMon(void* lconv) {
    if (!lconv) {
        return;
    }
    char** const f = static_cast<char**>(lconv);
    for (int i = 3; i < 10; i++) {
        char** const cur = *reinterpret_cast<char***>(0x8E37BC);
        if (f[i] != cur[i] && f[i] != *reinterpret_cast<char**>(0x8E3798 + (i - 3) * 4)) {
            LocFree(f[i]);
        }
    }
}

LocInfo* CurrentLocInfo() {
    return ThreadLocInfo();
}

// 0x82AAE2
int __cdecl IsctypeMt(LocInfo* li, int c, int mask) {
    if (static_cast<unsigned int>(c + 1) <= 0x100) {
        unsigned short w;
        std::memcpy(&w, li->pctype + c * 2, 2);
        return w & mask;
    }
    char buf[3];
    int size;
    const unsigned char hi = static_cast<unsigned char>(c >> 8);
    if (li->pctype[hi * 2 + 1] & 0x80) {
        buf[0] = static_cast<char>(hi);
        buf[1] = static_cast<char>(c);
        buf[2] = 0;
        size = 2;
    } else {
        buf[0] = static_cast<char>(c);
        buf[1] = 0;
        size = 1;
    }
    // En el EXE el resultado de GetStringTypeA se escribe en [ebp+0xe]: la mitad alta del argumento c y, con dos bytes, tambien el argumento mask
    // (una palabra de tipo por byte), de modo que el valor devuelto es tipo[0] & tipo[1]. Se reproduce con una matriz de palabras (tipo, mask).
    unsigned short typeAndMask[8] = {};
    typeAndMask[1] = static_cast<unsigned short>(mask);
    typeAndMask[2] = static_cast<unsigned short>(static_cast<unsigned int>(mask) >> 16);
    if (!CrtGetStringTypeA(1, buf, size, typeAndMask, static_cast<int>(li->codePage), static_cast<int>(li->handle[2]), 1)) {
        return 0;
    }
    return typeAndMask[0] & (static_cast<unsigned int>(typeAndMask[1]) | (static_cast<unsigned int>(typeAndMask[2]) << 16));
}

namespace {
// Nucleo comun de _tolower_lk (0x825C88) y _toupper_lk (0x823945); solo cambian la bandera de LCMapString (0x100/0x200), la clase que se comprueba
// (1 = _UPPER, 2 = _LOWER) y el intervalo ASCII que se convierte.
int CaseLk(LocInfo* li, int c, bool upper) {
    const int cls = upper ? 2 : 1;
    const unsigned int mapFlag = upper ? 0x200u : 0x100u;
    bool useAscii = li->handle[2] == 0;
    if (!useAscii && li->clike != 0 && static_cast<unsigned int>(c) <= 0x7F) {
        useAscii = true;
    }
    if (useAscii) {
        if (upper) {
            return (c >= 0x61 && c <= 0x7A) ? c - 0x20 : c;
        }
        return (c >= 0x41 && c <= 0x5A) ? c + 0x20 : c;
    }
    if (static_cast<unsigned int>(c) < 0x100) {
        const int t = li->mbCurMax > 1 ? IsctypeMt(li, c, cls) : (li->pctype[c * 2] & cls);
        if (t == 0) {
            return c;
        }
    }
    char in[3];
    int size;
    const unsigned char hi = static_cast<unsigned char>(c >> 8);
    if (li->pctype[hi * 2 + 1] & 0x80) {
        in[0] = static_cast<char>(hi);
        in[1] = static_cast<char>(c);
        in[2] = 0;
        size = 2;
    } else {
        in[0] = static_cast<char>(c);
        in[1] = 0;
        size = 1;
    }
    unsigned char out[4] = {};
    const int n = CrtLCMapStringA(li->handle[2], mapFlag, in, size, reinterpret_cast<char*>(out), 3, static_cast<int>(li->codePage), 1);
    if (n == 0) {
        return c;
    }
    if (n == 1) {
        return out[0];
    }
    return (out[0] << 8) | out[1];
}
} // namespace

// 0x825C88
int __cdecl TolowerLk(LocInfo* li, int c) { return CaseLk(li, c, false); }
// 0x823945
int __cdecl ToupperLk(LocInfo* li, int c) { return CaseLk(li, c, true); }

// 0x830FB8
int __cdecl CrtGetStringTypeA(unsigned int infoType, const char* src, int cchSrc, unsigned short* charType, int codePage, int lcid, int bError) {
    if (GetStringTypeWMode() == 0) {
        WORD probe;
        if (GetStringTypeW(1, EmptyWide(), 1, &probe)) {
            GetStringTypeWMode() = 1;
        } else if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) {
            GetStringTypeWMode() = 2;
        }
    }
    const int mode = GetStringTypeWMode();
    if (mode == 1) {
        if (codePage == 0) {
            codePage = static_cast<int>(LcCodePage());
        }
        const int wide = MultiByteToWideChar(codePage, (bError != 0 ? 8 : 0) | 1, src, cchSrc, nullptr, 0);
        if (wide == 0) {
            return 0;
        }
        TempBuf buf(static_cast<unsigned int>(wide) * 2, true);
        if (!buf.get()) {
            // sin pila ni montón: calloc(2, wide) como el EXE (que tambien lo libera al final)
            void* const heap = LocCalloc(2, static_cast<unsigned int>(wide));
            if (!heap) {
                return 0;
            }
            int result = 0;
            const int n = MultiByteToWideChar(codePage, 1, src, cchSrc, static_cast<LPWSTR>(heap), wide);
            if (n) {
                result = GetStringTypeW(infoType, static_cast<LPCWSTR>(heap), n, charType);
            }
            LocFree(heap);
            return result;
        }
        int result = 0;
        const int n = MultiByteToWideChar(codePage, 1, src, cchSrc, reinterpret_cast<LPWSTR>(buf.get()), wide);
        if (n) {
            result = GetStringTypeW(infoType, reinterpret_cast<LPCWSTR>(buf.get()), n, charType);
        }
        return result;
    }
    if (mode != 2 && mode != 0) {
        return 0;
    }
    unsigned int lcidUsed = static_cast<unsigned int>(lcid);
    if (lcidUsed == 0) {
        lcidUsed = LcHandleCtype();
    }
    int cpUsed = codePage;
    if (cpUsed == 0) {
        cpUsed = static_cast<int>(LcCodePage());
    }
    const int acp = Ansicp(lcidUsed);
    if (acp == -1) {
        return 0;
    }
    char* converted = nullptr;
    if (acp != cpUsed) {
        converted = Convertcp(cpUsed, acp, src, &cchSrc, nullptr, 0);
        if (!converted) {
            return 0;
        }
        src = converted;
    }
    const int result = GetStringTypeA(lcidUsed, infoType, src, cchSrc, charType);
    if (converted) {
        LocFree(converted);
    }
    return result;
}

// 0x8344CA: pagina de codigo ANSI por defecto de un LCID (-1 si GetLocaleInfoA falla)
int __cdecl Ansicp(unsigned int lcid) {
    char buf[7] = {};
    if (GetLocaleInfoA(lcid, 0x1004, buf, 6) == 0) {
        return -1;
    }
    return LocAtol(buf);
}

// 0x834513: convierte texto de una pagina de codigo a otra pasando por UTF-16; devuelve un bloque con calloc (a liberar por el llamador) o, si se
// da destino, ese mismo destino. Con paginas iguales devuelve 0. *count (si no era -1) pasa a ser la longitud convertida.
char* __cdecl Convertcp(int srcCodePage, int dstCodePage, const char* src, int* count, char* dest, int destSize) {
    char* result = nullptr;
    const int cchIn = *count;
    if (srcCodePage == dstCodePage) {
        return nullptr;
    }
    bool bothSingleByte = false;
    CPINFO info;
    if (GetCPInfo(srcCodePage, &info) && info.MaxCharSize == 1 && GetCPInfo(dstCodePage, &info) && info.MaxCharSize == 1) {
        bothSingleByte = true;
    }
    int wide;
    if (bothSingleByte) {
        wide = cchIn != -1 ? cchIn : static_cast<int>(std::strlen(src)) + 1;
    } else {
        wide = MultiByteToWideChar(srcCodePage, 1, src, cchIn, nullptr, 0);
        if (wide == 0) {
            return nullptr;
        }
    }
    TempBuf buf(static_cast<unsigned int>(wide) * 2, true);
    void* wbuf = buf.get();
    void* heapW = nullptr;
    if (!wbuf) {
        heapW = LocCalloc(2, static_cast<unsigned int>(wide));
        if (!heapW) {
            return nullptr;
        }
        wbuf = heapW;
    }
    if (MultiByteToWideChar(srcCodePage, 1, src, cchIn, static_cast<LPWSTR>(wbuf), wide)) {
        if (dest) {
            if (WideCharToMultiByte(dstCodePage, 0, static_cast<LPCWSTR>(wbuf), wide, dest, destSize, nullptr, nullptr)) {
                result = dest;
            }
        } else {
            int n = wide;
            if (!bothSingleByte) {
                n = WideCharToMultiByte(dstCodePage, 0, static_cast<LPCWSTR>(wbuf), wide, nullptr, 0, nullptr, nullptr);
            }
            if (n != 0) {
                result = static_cast<char*>(LocCalloc(1, static_cast<unsigned int>(n)));
                if (result) {
                    // El EXE pasa aqui n (el tamaño en bytes) como numero de caracteres anchos, no wide. Con n > wide la conversion lee mas alla del bufer y
                    // siempre falla (cada caracter de mas necesita al menos un byte y el destino mide justo n): se resuelve sin leer fuera del bufer.
                    const int written = n > wide ? 0 : WideCharToMultiByte(dstCodePage, 0, static_cast<LPCWSTR>(wbuf), n, result, n, nullptr, nullptr);
                    if (written == 0) {
                        LocFree(result);
                        result = nullptr;
                    } else if (cchIn != -1) {
                        *count = written;
                    }
                }
            }
        }
    }
    if (heapW) {
        LocFree(heapW);
    }
    return result;
}

// 0x82BB1E
int __cdecl CrtLCMapStringA(unsigned int locale, unsigned int mapFlags, const char* src, int cchSrc, char* dest, int cchDest, int codePage, int bError) {
    if (LCMapStringWMode() == 0) {
        if (LCMapStringW(0, 0x100, EmptyWide(), 1, nullptr, 0)) {
            LCMapStringWMode() = 1;
        } else if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) {
            LCMapStringWMode() = 2;
        }
    }
    if (cchSrc > 0) {
        // _strncnt: longitud de la cadena acotada por cchSrc
        int left = cchSrc;
        const char* p = src;
        for (;;) {
            --left;
            if (*p == 0) {
                break;
            }
            ++p;
            if (left == 0) {
                left = -1;
                break;
            }
        }
        cchSrc += -1 - left;
    }
    const int mode = LCMapStringWMode();
    if (mode == 1) {
        int result = 0;
        if (codePage == 0) {
            codePage = static_cast<int>(LcCodePage());
        }
        const int wide = MultiByteToWideChar(codePage, (bError != 0 ? 8 : 0) | 1, src, cchSrc, nullptr, 0);
        if (wide == 0) {
            return 0;
        }
        TempBuf w1(static_cast<unsigned int>(wide) * 2, false);
        if (!w1.get()) {
            return 0;
        }
        if (!MultiByteToWideChar(codePage, 1, src, cchSrc, reinterpret_cast<LPWSTR>(w1.get()), wide)) {
            return 0;
        }
        result = LCMapStringW(locale, mapFlags, reinterpret_cast<LPCWSTR>(w1.get()), wide, nullptr, 0);
        if (result == 0) {
            return 0;
        }
        if (mapFlags & 0x400) {
            // LCMAP_SORTKEY: el resultado es el tamaño pedido; solo se rellena el destino si cabe
            if (cchDest == 0 || result > cchDest) {
                return result;
            }
            LCMapStringW(locale, mapFlags, reinterpret_cast<LPCWSTR>(w1.get()), wide, reinterpret_cast<LPWSTR>(dest), cchDest);
            return result;
        }
        TempBuf w2(static_cast<unsigned int>(result) * 2, false);
        if (!w2.get()) {
            return result; // el EXE devuelve aqui el tamaño ya calculado
        }
        if (!LCMapStringW(locale, mapFlags, reinterpret_cast<LPCWSTR>(w1.get()), wide, reinterpret_cast<LPWSTR>(w2.get()), result)) {
            return result;
        }
        if (cchDest == 0) {
            return WideCharToMultiByte(codePage, 0, reinterpret_cast<LPCWSTR>(w2.get()), result, nullptr, 0, nullptr, nullptr);
        }
        return WideCharToMultiByte(codePage, 0, reinterpret_cast<LPCWSTR>(w2.get()), result, dest, cchDest, nullptr, nullptr);
    }
    if (mode != 2 && mode != 0) {
        return 0;
    }
    if (locale == 0) {
        locale = LcHandleCtype();
    }
    if (codePage == 0) {
        codePage = static_cast<int>(LcCodePage());
    }
    const int acp = Ansicp(locale);
    if (acp == -1) {
        return 0;
    }
    if (acp == codePage) {
        return LCMapStringA(locale, mapFlags, src, cchSrc, dest, cchDest);
    }
    char* const converted = Convertcp(codePage, acp, src, &cchSrc, nullptr, 0);
    if (!converted) {
        return 0;
    }
    int ret;
    int size = LCMapStringA(locale, mapFlags, converted, cchSrc, nullptr, 0);
    if (size == 0) {
        ret = 0; // el EXE devuelve aqui un valor de pila sin inicializar; el CRT de origen devuelve 0
    } else {
        TempBuf mapped(static_cast<unsigned int>(size), true);
        if (!mapped.get()) {
            ret = 0;
        } else {
            int n = LCMapStringA(locale, mapFlags, converted, cchSrc, reinterpret_cast<char*>(mapped.get()), size);
            if (n == 0) {
                ret = 0;
            } else {
                ret = Convertcp(acp, codePage, reinterpret_cast<char*>(mapped.get()), &n, dest, cchDest) != nullptr ? 1 : 0;
            }
        }
    }
    LocFree(converted);
    return ret;
}

// 0x8334DC
int __cdecl WctombMt(LocInfo* li, char* dest, unsigned short wc) {
    if (dest == nullptr) {
        return 0;
    }
    if (li->handle[2] == 0) {
        if (wc <= 0xFF) {
            *dest = static_cast<char>(wc);
            return 1;
        }
    } else {
        BOOL usedDefault = 0;
        const int n = WideCharToMultiByte(li->codePage, 0, reinterpret_cast<LPCWSTR>(&wc), 1, dest, li->mbCurMax, nullptr, &usedDefault);
        if (n != 0 && usedDefault == 0) {
            return n;
        }
    }
    *ErrnoPtr() = EILSEQ_CODE;
    return -1;
}

// 0x83353C
int __cdecl Wctomb(char* dest, unsigned short wc) {
    return WctombMt(ThreadLocInfo(), dest, wc);
}

// 0x83380E
int __cdecl MbtowcMt(LocInfo* li, unsigned short* dest, const char* src, unsigned int n) {
    if (src == nullptr || n == 0) {
        return 0;
    }
    const unsigned char first = static_cast<unsigned char>(*src);
    if (first == 0) {
        if (dest) {
            *dest = 0;
        }
        return 0;
    }
    if (li->handle[2] == 0) {
        if (dest) {
            *dest = first;
        }
        return 1;
    }
    if (li->pctype[first * 2 + 1] & 0x80) {
        const int cur = li->mbCurMax;
        if (cur > 1 && static_cast<int>(n) >= cur) {
            if (MultiByteToWideChar(li->codePage, 9, src, cur, reinterpret_cast<LPWSTR>(dest), dest != nullptr ? 1 : 0) != 0) {
                return li->mbCurMax;
            }
        }
        if (!(n < static_cast<unsigned int>(li->mbCurMax)) && src[1] != 0) {
            return li->mbCurMax;
        }
    } else if (MultiByteToWideChar(li->codePage, 9, src, 1, reinterpret_cast<LPWSTR>(dest), dest != nullptr ? 1 : 0) != 0) {
        return 1;
    }
    *ErrnoPtr() = EILSEQ_CODE;
    return -1;
}

// 0x8338CE
int __cdecl Mbtowc(unsigned short* dest, const char* src, unsigned int n) {
    return MbtowcMt(ThreadLocInfo(), dest, src, n);
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtLocaleInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::UpdateTLocInfo, 0x825456);
    RH_ScopedGlobalInstall(ExeCrt::UpdateTLocInfoNoLock, 0x82524D);
    RH_ScopedGlobalInstall(ExeCrt::FreeTLocInfo, 0x825183);
    RH_ScopedGlobalInstall(ExeCrt::FreeLcTime, 0x82FBE7);
    RH_ScopedGlobalInstall(ExeCrt::FreeLconvNum, 0x82FE09);
    RH_ScopedGlobalInstall(ExeCrt::FreeLconvMon, 0x830068);
    RH_ScopedGlobalInstall(ExeCrt::IsctypeMt, 0x82AAE2);
    RH_ScopedGlobalInstall(ExeCrt::TolowerLk, 0x825C88);
    RH_ScopedGlobalInstall(ExeCrt::ToupperLk, 0x823945);
    RH_ScopedGlobalInstall(ExeCrt::CrtLCMapStringA, 0x82BB1E);
    RH_ScopedGlobalInstall(ExeCrt::CrtGetStringTypeA, 0x830FB8);
    RH_ScopedGlobalInstall(ExeCrt::Ansicp, 0x8344CA);
    RH_ScopedGlobalInstall(ExeCrt::Convertcp, 0x834513);
    RH_ScopedGlobalInstall(ExeCrt::WctombMt, 0x8334DC);
    RH_ScopedGlobalInstall(ExeCrt::Wctomb, 0x83353C);
    RH_ScopedGlobalInstall(ExeCrt::MbtowcMt, 0x83380E);
    RH_ScopedGlobalInstall(ExeCrt::Mbtowc, 0x8338CE);
}
#endif
