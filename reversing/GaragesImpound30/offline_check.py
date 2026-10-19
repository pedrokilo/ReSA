"""Prueba FUERA del juego del banco (complementaria; no sustituye a la corrida dentro de GTA): ejecuta los BYTES del EXE canonico y la funcion C++ de la DLL sobre las 4096 fixtures y
compara los tres: EXE == DLL (bit a bit) y ambos == oraculo (fixture_oracle.py).

  python -B reversing/<Banco>/offline_check.py        (mismo script en GaragesAddOne30 y GaragesImpound30; el banco sale del nombre de la carpeta)

Que hace (Windows, MSVC x86 via vcvars32, sin compilar el proyecto ni ejecutar GTA):
  1. vuelca del EXE (D:\\Grand Theft Auto San Andreas\\gta-sa.exe) los bytes de la funcion (0x4471E0 o 0x44A3C0) y de strncpy (0x821F40);
  2. extrae de source/game_sa/Garages.cpp los cuerpos reales de CGarages::AddOne y StoreCarInNearestImpoundingGarage y compila X87Math.cpp con las opciones del proyecto
     (/O2 /Oi /arch:IA32 /fp:fast) y el arnes offline_harness.cpp con /Od (SSE, como Garages.cpp);
  3. el arnes reserva en direcciones fijas la memoria que usa el EXE (tablas de garajes / coches guardados 0x96ABD8..0x96EA78, strncpy 0x821F40) y ejecuta el EXE REUBICADO a 0x900000/0x901000
     (0x440000 esta ocupado en un proceso nuevo): solo se reapuntan sus `call` (a strncpy del EXE, y en el banco de embargo a los falsos de FindSafeHouseIndexForGarageType / StoreCar,
     identicos a los del banco); la DLL llama directamente a su C++ con los argumentos en bruto de la pila;
  4. compara con fixture_oracle.py. El ASLR puede ocupar 0x820000 en alguna ejecucion: el script reintenta.
Resultado esperado: `cases 4096 E!=D 0 E!=oracle 0 D!=oracle 0`.
"""
import json
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
BANK = HERE.name
EXE = Path(r'D:\Grand Theft Auto San Andreas\gta-sa.exe')
VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
SRC = ROOT / 'source' / 'game_sa'
IS_ADD = BANK == 'GaragesAddOne30'
DEFINE = 'BANK_ADDONE' if IS_ADD else 'BANK_IMPOUND'
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle  # noqa: E402


def exe_bytes(blobs):
    exe = EXE.read_bytes()
    pe = struct.unpack_from('<I', exe, 0x3C)[0]
    nsec = struct.unpack_from('<H', exe, pe + 6)[0]
    opt = struct.unpack_from('<H', exe, pe + 20)[0]
    base = struct.unpack_from('<I', exe, pe + 24 + 28)[0]
    secs = []
    o = pe + 24 + opt
    for i in range(nsec):
        vs, va, rs, rp = struct.unpack_from('<IIII', exe, o + 8 + i * 40)
        secs.append((va, vs, rp, rs))

    def off(a):
        r = a - base
        for va, vs, rp, rs in secs:
            if va <= r < va + max(vs, rs):
                return rp + r - va
    return {name: exe[off(a):off(a) + (b - a)] for name, (a, b) in blobs.items()}


def run(cmd, cwd):
    p = subprocess.run(cmd, cwd=cwd, shell=True, capture_output=True, text=True)
    return p.returncode, p.stdout + p.stderr


def main():
    blobs = {'strncpy': (0x821F40, 0x822100)}
    blobs['addone' if IS_ADD else 'impound'] = (0x4471E0, 0x4475F4) if IS_ADD else (0x44A3C0, 0x44A4C7)
    with tempfile.TemporaryDirectory(prefix='garages30-') as tmp:
        tmp = Path(tmp)
        for name, data in exe_bytes(blobs).items():
            (tmp / (name + '.bin')).write_bytes(data)
        g = (SRC / 'Garages.cpp').read_text(encoding='utf-8')
        a, b = g.index('// Como el original (0x4471E0'), g.index('// 0x44A170')
        c, d = g.index('// Como el original (0x44A3C0'), g.index('// unused', g.index('// Como el original (0x44A3C0'))
        (tmp / 'bodies.inc').write_text(g[a:b] + '\n' + g[c:d], encoding='utf-8')
        bat = tmp / 'build.bat'
        bat.write_text('\n'.join([
            '@call "%s" >nul 2>&1' % VCVARS,
            'cl /nologo /std:c++20 /EHsc /O2 /Oi /arch:IA32 /fp:fast /GS- /c /I"%s" /Fox87.obj "%s"' % (SRC, SRC / 'X87Math.cpp'),
            'if errorlevel 1 exit /b 1',
            'cl /nologo /std:c++20 /EHsc /Od /GS- /D%s /I"%s" /I"%s" /I. /Feharness.exe "%s" x87.obj /link /BASE:0x10000000 /DYNAMICBASE:NO' % (DEFINE, SRC, HERE, HERE / 'offline_harness.cpp'),
            'if errorlevel 1 exit /b 1',
            'cl /nologo /std:c++20 /EHsc /I"%s" /Fedump.exe "%s"' % (HERE, HERE / 'fixture_dump.cpp'),
            'if errorlevel 1 exit /b 1',
            r'.\dump.exe > inputs.jsonl']), encoding='utf-8')
        code, out = run('cmd /c "%s"' % bat, tmp)
        if code:
            print(out)
            return 2
        oracle.crosscheck(tmp / 'inputs.jsonl')
        text = None
        for _ in range(12):
            p = subprocess.run([str(tmp / 'harness.exe'), str(tmp)], capture_output=True, text=True)
            if p.returncode == 0:
                text = p.stdout
                break
        if text is None:
            print('el arnes no pudo reservar las direcciones fijas (ASLR); repetir')
            return 3
        inputs = [json.loads(line) for line in (tmp / 'inputs.jsonl').read_text().splitlines()[1:]]
        sides = {}
        for line in text.splitlines():
            r = json.loads(line)
            sides[(r['n'], r['s'])] = {k: v for k, v in r.items() if k not in ('n', 's')}
        bad = {'E!=D': 0, 'E!=oracle': 0, 'D!=oracle': 0}
        firsts = []
        for n, inp in enumerate(inputs):
            e, dd = sides[(n, 'E')], sides[(n, 'D')]
            want, _ = oracle.simulate(inp)
            for key, ok in (('E!=D', e == dd), ('E!=oracle', oracle.matches(e, want)), ('D!=oracle', oracle.matches(dd, want))):
                if not ok:
                    bad[key] += 1
                    if len(firsts) < 5:
                        firsts.append((key, n, inp['kind']))
        print('cases', len(inputs), ' '.join('%s %d' % kv for kv in bad.items()))
        for f in firsts:
            print(f)
        return 0 if not any(bad.values()) else 1


if __name__ == '__main__':
    sys.exit(main())
