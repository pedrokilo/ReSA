"""Autoprueba cruzada del banco Helpers66D SIN juego: compila fixture_dump.cpp (que incluye Fixtures.hpp tal cual) con cl, vuelca las 4 x 4096 filas `input` y las constantes de arena,
comprueba que las constantes coinciden con las del oraculo (LAYOUT de fixture_oracle.py), pasa cada fila por el oraculo (sin ambiguedades de NaN) y calcula la cobertura frente a los
minimos de run_local.py.

    python -I fixture_selfcheck.py            # imprime la cobertura y 'ok'
Necesita Visual Studio (vcvars64.bat); solo compila C++ estandar, no el proyecto. Los garajes de datos constantes (garage_kind 1) toman los bytes del EXE canonico.
"""
import json
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

VCVARS = next(Path('C:/Program Files/Microsoft Visual Studio').glob('*/*/VC/Auxiliary/Build/vcvars64.bat'))
EXE = Path('D:/Grand Theft Auto San Andreas/gta-sa.exe')


def dump_rows():
    with tempfile.TemporaryDirectory() as tmp:
        exe = Path(tmp) / 'fixture_dump.exe'
        cmd = f'"{VCVARS}" >nul && cl /nologo /EHsc /O2 /std:c++20 /I"{HERE}" /Fo"{tmp}\\\\" /Fe"{exe}" "{HERE / "fixture_dump.cpp"}"'
        subprocess.run(f'cmd /s /c "{cmd}"', check=True, stdout=subprocess.DEVNULL)
        out = subprocess.run([str(exe)], check=True, capture_output=True, text=True).stdout
    return [json.loads(line) for line in out.splitlines()]


class ExeImage:
    """lee bytes del EXE por direccion virtual (para los garajes de datos constantes)"""

    def __init__(self, path):
        d = path.read_bytes()
        pe = struct.unpack_from('<I', d, 0x3C)[0]
        n = struct.unpack_from('<H', d, pe + 6)[0]
        so = struct.unpack_from('<H', d, pe + 20)[0]
        self.base = struct.unpack_from('<I', d, pe + 0x34)[0]
        self.d = d
        self.secs = []
        for i in range(n):
            o = pe + 24 + so + i * 40
            vs, va, rs, rp = struct.unpack_from('<IIII', d, o + 8)
            self.secs.append((va, vs, rp, rs))

    def read(self, addr, n):
        r = addr - self.base
        for va, vs, rp, rs in self.secs:
            if va <= r < va + max(vs, rs):
                return self.d[rp + r - va:rp + r - va + n]
        raise KeyError(hex(addr))


def coverage(rows):
    layout = rows[0]['layout']
    for key, value in oracle.LAYOUT.items():
        assert layout[key] == value, ('constante de arena distinta', key, layout[key], value)
    assert layout['cases'] == oracle.CASES
    image = ExeImage(EXE) if EXE.exists() else None
    cov = {}
    skipped = 0
    for row in rows[1:]:
        f = row['func']
        fn = oracle.NAMES[f]
        if f == 1 and row['garage_kind'] == 1:
            if image is None:
                skipped += 1
                continue
            row['garage_bytes'] = image.read(row['rdata'], 0x28).hex()
        want, stats = oracle.expected(fn, row, row['n'])
        cov['f%d_stack_seed' % f] = cov.get('f%d_stack_seed' % f, 0) + 1
        for key, value in stats.items():
            if key == 'family':
                k = 'f%d_fam%d' % (f, value)
                cov[k] = cov.get(k, 0) + 1
            elif value:
                cov['f%d_%s' % (f, key)] = cov.get('f%d_%s' % (f, key), 0) + 1
    return cov, skipped


if __name__ == '__main__':
    rows = dump_rows()
    assert len(rows) == 4 * oracle.CASES + 1
    cov, skipped = coverage(rows)
    print(json.dumps(dict(sorted(cov.items())), indent=1))
    print('garajes de datos constantes omitidos (EXE no disponible):', skipped)
    import run_local
    short = run_local.check_minimums(cov)
    assert not short, short
    print('ok')
