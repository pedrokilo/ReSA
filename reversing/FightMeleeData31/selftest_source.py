"""Autoprueba del FUENTE (sin el juego ni el EXE): compila el cuerpo real de CTaskSimpleFight::LoadMeleeData de TaskSimpleFight.cpp contra falsos de OpenFile / LoadLine / CloseFile /
GetAnimGroupName (los mismos eventos que el banco) y el sscanf del UCRT en lugar del del EXE (las fixtures solo usan lecturas modeladas), sobre memoria reservada en las mismas direcciones
absolutas (0xC17000, 0xB4E000), ejecuta las 4608 fixtures de Fixtures.hpp y compara cada resultado con fixture_oracle.py. No sustituye al banco en juego.
Uso:  python -I selftest_source.py            (imprime «mismatching cases: 0 of 4608» si todo coincide)
"""
import json, re, subprocess, sys, tempfile
from pathlib import Path
HERE = Path(__file__).resolve().parent
WS = HERE.parents[1]
ROOT = WS / 'reversing'
VS = Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC')
src = (WS / 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp').read_text(encoding='utf-8')
a = src.index('// 0x5BEDC0: inicializa')
b = src.index('// 0x6239F0: con prioridad')
func = src[a:b].replace('void CTaskSimpleFight::LoadMeleeData()', 'void LoadMeleeDataImpl()')
# helpers de la funcion
helpers_start = src.index('constexpr uintptr_t FightLevelsAddress')
ha = src.index('constexpr float FightMeleeDataK')
hb = src.index('constexpr float', ha + 10)
fa = src.index('template<typename T>\nT& FightAt')
fb = src.index('// Fila de la tabla de combos para un combo dado')
helpers = 'constexpr uintptr_t FightLevelsAddress = 0xC177D0;\n' + src[ha:src.index('\n', ha) + 1] + src[fa:fb]
harness = r'''
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <bit>
#include <string>
#include <vector>
#include "%(fix)s/Fixtures.hpp"
using namespace FightMeleeData31Fix;
using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t; using int32 = std::int32_t; using int16 = std::int16_t; using int8 = std::int8_t;
using AssocGroupId = int;
#define ZoneScoped
typedef void* FILESTREAM;
template<class T> T& StaticRef(uintptr_t a) { return *reinterpret_cast<T*>(a); }
static const C* gC; static unsigned gIdx; static char gBuf[1024];
static unsigned nev; static unsigned long long evh;
static void Ev(u32 k, u32 a, u32 b) { u32 w[3] = {k, a, b}; for (int i = 0; i < 12; ++i) evh = (evh ^ reinterpret_cast<u8*>(w)[i]) * 1099511628211ull; ++nev; }
static u32 F32(const char* s) { u32 h = 2166136261u; for (; *s; ++s) h = (h ^ u8(*s)) * 16777619u; return h; }
struct CFileMgr { static FILESTREAM OpenFile(const char* p, const char* m) { Ev(1, F32(p), F32(m)); return (FILESTREAM)0xABC123; } static int CloseFile(FILESTREAM f) { Ev(3, u32(uintptr_t(f)), 0); return 0; } };
struct CFileLoader { static char* LoadLine(FILESTREAM f) { if (gIdx >= gC->lines.size()) { Ev(2, 0xFFFFFFFFu, u32(uintptr_t(f))); return nullptr; } Ev(2, gIdx, u32(uintptr_t(f))); std::strncpy(gBuf, gC->lines[gIdx].c_str(), sizeof gBuf - 1); ++gIdx; return gBuf; } };
struct CAnimManager { static const char* GetAnimGroupName(AssocGroupId i) { Ev(4, u32(i), 0); return i >= 0 && size_t(i) < gC->groups.size() ? gC->groups[size_t(i)].c_str() : "?"; } };
struct CMeleeInfo { char pad[0x88]; };
constexpr uintptr_t FightTableAddress = 0xC170D0;
%(helpers)s
using FightSscanfFn = int(*)(const char*, const char*, ...);
static int FightSscanf(const char* l, const char* f, ...);
// el sscanf del UCRT hace de sscanf del EXE en este autotest (las fixtures solo usan lecturas modeladas)
#define FightSscanf(...) sscanf_s_free(__VA_ARGS__)
#pragma warning(disable: 4996)
template<class... A> int sscanf_s_free(const char* l, const char* f, A... a) { return std::sscanf(l, f, a...); }
%(func)s
static u64 Fnv64(const void* p, size_t n) { u64 h = 1469598103934665603ull; const auto* b = static_cast<const u8*>(p); for (size_t i = 0; i < n; ++i) h = (h ^ b[i]) * 1099511628211ull; return h; }
int main() {
    if (!VirtualAlloc((void*)0xC17000, 0x2000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE) || !VirtualAlloc((void*)0xB4E000, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) { std::puts("no se pudo reservar"); return 3; }
    if (!*reinterpret_cast<u8*>(0xC17090) && false) return 2;
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n); gC = &c; gIdx = 0; nev = 0; evh = 1469598103934665603ull;
        std::array<u8, kRegion> pre{}; FillWords(pre.data(), kRegion, c.prefillSeed);
        std::memcpy(reinterpret_cast<void*>(kRegionBase), pre.data(), kRegion); *reinterpret_cast<u32*>(kGroupCountAddress) = c.numGroups;
        LoadMeleeDataImpl();
        std::array<u8, kRegion> post{}; std::memcpy(post.data(), reinterpret_cast<void*>(kRegionBase), kRegion);
        unsigned changed = 0; for (u32 i = 0; i < kRegion; i += 4) { u32 a, b; std::memcpy(&a, post.data() + i, 4); std::memcpy(&b, pre.data() + i, 4); changed += a != b; }
        if (n == %(dump)d) { std::printf("DUMP "); for (u32 i = 0; i < kRegion; ++i) std::printf("%%02x", post[i]); std::printf("\n"); }
        std::printf("{\"events_hash\":\"%%016llx\",\"n_events\":%%u,\"window_hash\":\"%%016llx\",\"changed_words\":%%u}\n", evh, nev, Fnv64(post.data(), kRegion), changed);
    }
}
''' % dict(fix=(ROOT / 'FightMeleeData31').as_posix(), helpers=helpers, func=func, dump=int(sys.argv[1]) if len(sys.argv) > 1 else -1)
d = Path(tempfile.mkdtemp(prefix='sm-'))
(d / 's.cpp').write_text(harness, encoding='utf-8')
bat = d / 'b.bat'
bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O1 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s"\r\n"%s"\r\n' % (VS / 'Auxiliary/Build/vcvars32.bat', d / 's.exe', d, d / 's.cpp', d / 's.exe'))
r = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(d))
lines = [l for l in r.stdout.splitlines() if l.startswith('{')]
if len(lines) != 4608:
    print(r.stdout[-6000:], r.stderr[-2000:]); sys.exit(1)
sys.path.insert(0, str(HERE))
import fixture_mirror as fm, fixture_oracle as o
rows = fm.build_and_run(); rows.pop()
bad = 0
for n, (row, got) in enumerate(zip(rows, lines)):
    got = json.loads(got)
    want, _ = o.expected(o.NAME, row, n)
    for k in ('events_hash', 'n_events', 'window_hash', 'changed_words'):
        if got[k] != want[k]:
            bad += 1
            if bad <= 5: print('MISMATCH case', n, 'family', row['family'], k, got[k], want[k])
            break
print('mismatching cases:', bad, 'of', len(rows))
if len(sys.argv) > 1:
    dn = int(sys.argv[1])
    hexs = [l for l in r.stdout.splitlines() if l.startswith('DUMP ')][0][5:]
    got = bytes.fromhex(hexs)
    keep = []
    o.expected_load(rows[dn], dn, keep)
    exp = bytes(keep[0].b)
    for i in range(0, len(got), 4):
        if got[i:i+4] != exp[i:i+4]:
            print('word diff at region+0x%X (addr 0x%X): got %s exp %s' % (i, 0xC17090 + i, got[i:i+4].hex(), exp[i:i+4].hex()))
    print(json.dumps(rows[dn]['lines'], indent=0)[:3000])

