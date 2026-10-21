"""Comprobacion en el anfitrion (sin el juego): extrae ScanWorld/SetExtraRectangleToScan de source/game_sa/Renderer.cpp, los compila con cl con las celdas extra en un arreglo propio y un
GetATanOfXY falso (atan2), pasa los 4096 + 4096 casos de Fixtures.hpp y compara la salida con fixture_oracle.py. Detecta erratas de transcripcion entre la fuente y el oraculo
ANTES de correr el juego; NO sustituye al banco (no usa el EXE). Uso:  python -I host_check.py"""
import json, subprocess, sys, tempfile
from pathlib import Path
HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle
VS = Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC')

src = (ROOT / 'source/game_sa/Renderer.cpp').read_text(encoding='utf-8')
a = src.index('namespace {\n// Como el original: floor del CRT') if 'namespace {\n// Como el original: floor del CRT' in src else src.index('namespace {\r\n// Como el original: floor del CRT')
b = src.index('void CRenderer::Init()')
code = src[a:b].replace('reinterpret_cast<int32*>(0xC81598)', 'g_region').replace('reinterpret_cast<int32*>(0xC815A8)', '(g_region + 4)').replace('*reinterpret_cast<int32*>(0xC815B8)', 'g_region[8]')
harness = r'''
#include <bit>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "Fixtures.hpp"
using int32 = std::int32_t; using int64 = std::int64_t; using uint8 = std::uint8_t;
struct CVector2D { float x, y; };
static int32 g_region[16];
struct CGeneral { static float GetATanOfXY(float x, float y); };
struct CWorldScan { using tScanFunction = void(__cdecl*)(int32, int32); static void ScanWorld(CVector2D*, int32, tScanFunction); static void SetExtraRectangleToScan(float, float, float, float); };
using namespace WS64Fix;
static unsigned nev; static unsigned evk[800]; static unsigned eva[800], evb[800], evc[800];
float CGeneral::GetATanOfXY(float x, float y) { float r = (float)std::atan2((double)y, (double)x); if (nev < 700) { evk[nev] = 1; eva[nev] = FBits(x); evb[nev] = FBits(y); evc[nev] = FBits(r); } ++nev; return r; }
static void __cdecl Cb(int32 x, int32 y) { if (nev < 700) { evk[nev] = 2; eva[nev] = (unsigned)x; evb[nev] = (unsigned)y; } ++nev; }
''' + code + r'''
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) {
            Spec s = MakeSpec(fn, n);
            nev = 0;
            std::memcpy(g_region, s.pre, 9 * 4);
            u32 pts[16]; std::memcpy(pts, s.p.data(), sizeof(pts));
            if (fn == kScan) CWorldScan::ScanWorld(reinterpret_cast<CVector2D*>(pts), (int32)s.n, &Cb);
            else CWorldScan::SetExtraRectangleToScan(FOf(s.a[0]), FOf(s.a[1]), FOf(s.a[2]), FOf(s.a[3]));
            std::printf("%u\t%u\t{\"events\":[", fn, n);
            for (unsigned i = 0; i < nev && i < 700; ++i) {
                if (i) std::printf(",");
                if (evk[i] == 1) std::printf("[1,%u,%u,%u]", eva[i], evb[i], evc[i]); else std::printf("[2,%d,%d]", (int)eva[i], (int)evb[i]);
            }
            std::printf("],\"nevents\":%u,\"points\":[", nev);
            bool first = true;
            for (unsigned i = 0; i < 16; ++i) if (pts[i] != s.p[i]) { std::printf("%s[%u,%u]", first ? "" : ",", i, pts[i]); first = false; }
            std::printf("],\"region\":[");
            first = true;
            for (unsigned i = 0; i < 9; ++i) if ((u32)g_region[i] != s.pre[i]) { std::printf("%s[%u,%u]", first ? "" : ",", i, (u32)g_region[i]); first = false; }
            std::printf("]}\n");
        }
    return 0;
}
'''
out = Path(tempfile.mkdtemp(prefix='wshost-'))
(out / 'h.cpp').write_text(harness, encoding='utf-8')
bat = out / 'b.bat'
bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /Od /std:c++20 /I"%s" /Fe:"%s" /Fo:"%s\\\\" "%s" >nul\r\n"%s"\r\n' % (
    VS / 'Auxiliary/Build/vcvars32.bat', HERE, out / 'h.exe', out, out / 'h.cpp', out / 'h.exe'), encoding='ascii')
res = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
if res.returncode != 0 or not res.stdout.strip():
    raise SystemExit('No se pudo compilar/ejecutar: ' + res.stdout[-3000:] + res.stderr[-2000:])
bad = 0
for line in res.stdout.splitlines():
    fn, n, js = line.split('\t', 2)
    fn, n = int(fn), int(n)
    sys.path.insert(0, str(HERE))
    from fixture_mirror import build_and_run  # noqa
    break
rows = {}
mirror = build_and_run()
for l in mirror:
    k, js = l.split('\t', 1)
    if k in ('0', '1'):
        rows.setdefault(int(k), []).append(json.loads(js))
total = 0
for line in res.stdout.splitlines():
    fn, n, js = line.split('\t', 2)
    fn, n = int(fn), int(n)
    got = json.loads(js)
    want, _ = oracle.expected(oracle.NAMES[fn], rows[fn][n], n)
    total += 1
    if got != want:
        bad += 1
        if bad <= 5:
            print('DIFERENCIA fn %d caso %d\n  fuente  %s\n  oraculo %s\n  entrada %s' % (fn, n, json.dumps(got)[:600], json.dumps(want)[:600], json.dumps(rows[fn][n])[:600]))
print('casos', total, 'diferencias', bad)
sys.exit(1 if bad else 0)
