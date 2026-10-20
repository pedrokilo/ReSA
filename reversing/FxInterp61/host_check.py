"""Comprobacion SIN el juego del cuerpo C++ real de las cuatro GetVal contra el oraculo (ver host_check.cpp).

  python -I host_check.py [directorio_temporal]

Extrae las cuatro `GetVal(float*, float)` de source/game_sa/Fx/Blueprints/Interp/*.cpp, compila host_check.cpp con cl x86 (/O2 /Oi /arch:IA32 /fp:fast, como la DLL; la plantilla
InterpolateKeys es la REAL de FxInterpInfo.h), ejecuta las 16384 fixtures con la palabra de control x87 a 24 bits y compara la arena resultante, en bits, con fixture_oracle.py.
Sale con codigo 1 si hay diferencias. Requiere Visual Studio (vcvars32.bat).
"""
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
INTERP = ROOT / 'source/game_sa/Fx/Blueprints/Interp'
FILES = ('FxInterpInfoFloat', 'FxInterpInfoU255', 'FxInterpInfo32', 'FxInterpInfo255')


def extract():
    parts = []
    for name in FILES:
        text = (INTERP / (name + '.cpp')).read_text(encoding='utf-8').replace('\r\n', '\n')
        m = re.search(r'void %s_c::GetVal\(float\* outValues, float delta\) \{\n.*?\n\}\n' % name, text, re.S)
        assert m and 'plugin::' not in m.group(0), name
        parts.append(m.group(0))
    return '\n'.join(parts)


def run_bat(tmp, name, lines):
    bat = tmp / name
    bat.write_text('@echo off\r\n' + '\r\n'.join(lines) + '\r\n', encoding='ascii')
    return subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, errors='replace')


def main(tmp):
    tmp = Path(tmp)
    tmp.mkdir(parents=True, exist_ok=True)
    (tmp / 'getval_extract.inc').write_text(extract(), encoding='utf-8')
    r = run_bat(tmp, 'build.bat', [
        'call "%s" >nul 2>&1' % VCVARS, 'cd /d "%s"' % tmp,
        'cl /nologo /EHsc /std:c++20 /O2 /Oi /arch:IA32 /fp:fast /I"%s" /I"%s" /I"%s" /I"%s" "%s" /Fe:host_check.exe' % (tmp, HERE, HERE / 'host_stub', INTERP, HERE / 'host_check.cpp'),
        'cl /nologo /EHsc /std:c++20 /O2 /I"%s" "%s" /Fe:fixture_dump.exe >nul' % (HERE, HERE / 'fixture_dump.cpp')])
    if not (tmp / 'host_check.exe').exists() or not (tmp / 'fixture_dump.exe').exists():
        print(r.stdout[-4000:], r.stderr[-2000:])
        return 2
    out = subprocess.run([str(tmp / 'host_check.exe')], capture_output=True, text=True, encoding='utf-8')
    rows = [json.loads(line) for line in out.stdout.splitlines() if line.startswith('{')]
    dump = subprocess.run([str(tmp / 'fixture_dump.exe')], capture_output=True, text=True, encoding='ascii')
    inputs = [json.loads(line) for line in dump.stdout.splitlines() if line.strip()]
    if out.returncode != 0 or len(rows) != len(inputs) or len(rows) != 4 * oracle.CASES:
        print('fallo de ejecucion o numero de casos:', out.returncode, len(rows), len(inputs), out.stderr[-1000:])
        return 2
    bad = []
    stats_total = {}
    for row, inp in zip(rows, inputs):
        want, st = oracle.expected(inp, row['case'])
        for k, v in st.items():
            if k not in ('family', 'func') and v:
                stats_total[k] = stats_total.get(k, 0) + 1
        out = row['arena'][oracle.K_OUT * 2:(oracle.K_OUT + 4 * oracle.K_OUT_CAP) * 2]
        rest_ok = row['arena'][:oracle.K_OUT * 2] == inp['arena'][:oracle.K_OUT * 2] and row['arena'][(oracle.K_OUT + 4 * oracle.K_OUT_CAP) * 2:] == inp['arena'][(oracle.K_OUT + 4 * oracle.K_OUT_CAP) * 2:]
        if not oracle.matches({'out': out, 'guard_ok': rest_ok}, want):
            w = want['out']
            first = next((i for i in range(len(out)) if w[i] != '?' and out[i] != w[i]), 0) // 8
            bad.append((row['func'], row['case'], inp['family'], first, out[first * 8:first * 8 + 8], w[first * 8:first * 8 + 8], inp['delta'], rest_ok))
    print(json.dumps({'cases': len(rows), 'diferencias': len(bad), 'primeras': bad[:8], 'cobertura': stats_total}, indent=1, default=str)[:6000])
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1] if len(sys.argv) > 1 else tempfile.mkdtemp(prefix='fxinterp61_')))
