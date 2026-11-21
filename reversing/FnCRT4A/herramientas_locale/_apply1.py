import os
os.chdir(r'D:\Original\wv-32\source\game_sa\Crt')
head = open(r'D:\Original\wv-32\reversing\FnCRT4A\herramientas_locale\_head_ctype.txt', encoding='utf-8').read()
body = open('_ctype_body.tmp', encoding='utf-8').read()
open('CrtCtype.cpp', 'w', encoding='utf-8', newline='').write(head + body)
os.remove('_ctype_body.tmp')
os.remove(r'D:\Original\wv-32\reversing\FnCRT4A\herramientas_locale\_head_ctype.txt')

h = open('CrtCtype.h', encoding='utf-8').read()
h = h.replace('''//   _tolower 0x825D50   isdigit 0x822789   isspace 0x822802   isxdigit 0x8227C3''', '''//   _tolower 0x825D50   isdigit 0x822789   isspace 0x822802   isxdigit 0x8227C3
// Lote CRT4A-locale (21/11/2026): _toupper 0x823A0E, isalpha 0x8226D6, isupper 0x822715, islower 0x82274F, isalnum 0x822876, _atol 0x822506 (_atoi 0x82258E salta a ella),
//   __stricmp 0x8229B6 (__ascii_stricmp 0x82ABC0), __strnicmp 0x835AC7 (__ascii_strnicmp 0x836990), _strlwr 0x836C58, _strupr 0x836FBA.
// El locale (__updatetlocinfo, _isctype, _tolower_lk, __crtLCMapStringA, ...) ya es propio: CrtLocale.{h,cpp}.''')
h = h.replace('''// con mb_cur_max <= 1 (+0x28) usan la tabla de tipos (+0x48, palabras de 16 bits); con mas de 1 llaman a _isctype 0x82AAE2 del EXE.
// _tolower solo tiene camino propio para el locale "C" (+0x14 == 0); con otro locale llama a _tolower_lk 0x825C88 del EXE.''', '''// con mb_cur_max <= 1 (+0x28) usan la tabla de tipos (+0x48, palabras de 16 bits); con mas de 1 llaman a _isctype 0x82AAE2.''')
h = h.replace('''int __cdecl Isxdigit(int c);  // 0x8227C3''', '''int __cdecl Isxdigit(int c);  // 0x8227C3
int __cdecl Toupper(int c);   // 0x823A0E
int __cdecl Isalpha(int c);   // 0x8226D6
int __cdecl Isupper(int c);   // 0x822715
int __cdecl Islower(int c);   // 0x82274F
int __cdecl Isalnum(int c);   // 0x822876
int __cdecl Atol(const char* str);                                       // 0x822506
int __cdecl AsciiStricmp(const char* a, const char* b);                  // 0x82ABC0
int __cdecl AsciiStrnicmp(const char* a, const char* b, unsigned int n); // 0x836990
int __cdecl Stricmp(const char* a, const char* b);                       // 0x8229B6
int __cdecl Strnicmp(const char* a, const char* b, unsigned int n);      // 0x835AC7
char* __cdecl Strlwr(char* str);                                         // 0x836C58
char* __cdecl Strupr(char* str);                                         // 0x836FBA''')
open('CrtCtype.h', 'w', encoding='utf-8', newline='').write(h)

p = 'CrtState.cpp'
s = open(p, encoding='utf-8').read()
a = s.index('// 0x8228B5: ctype del locale actual')
b = s.index('// 0x82ADBE')
s = s[:a] + '''// 0x8228B5: ctype del locale actual (maximo de bytes por caracter 1: tabla de palabras; mayor que 1: _isctype). _PRINT = 0x157
int __cdecl Isprint(int c) {
    LocInfo* const li = CurrentLocInfo();
    if (li->mbCurMax > 1) {
        return IsctypeMt(li, c, 0x157);
    }
    unsigned short w;
    std::memcpy(&w, li->pctype + c * 2, 2);
    return w & 0x157;
}

''' + s[b:]
s = s.replace('#include "CrtHeap.h"', '#include "CrtHeap.h"\n#include "CrtLocale.h"\n#include <cstring>', 1)
s = s.replace('inline void* CallUpdateTLocInfo() { return reinterpret_cast<void*(__cdecl*)()>(0x825456)(); }           // __updatetlocinfo\n', '')
s = s.replace('inline int   CallIsctype(void* locinfo, int c, int mask) { return reinterpret_cast<int(__cdecl*)(void*, int, int)>(0x82AAE2)(locinfo, c, mask); }\n', '')
s = s.replace('inline void*&         GlobalLocInfo() { return *reinterpret_cast<void**>(0x8E2F14); }\n', '')
open(p, 'w', encoding='utf-8', newline='').write(s)
print('ok')
