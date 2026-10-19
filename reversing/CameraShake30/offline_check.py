"""Comprobacion SIN juego de los ayudantes x87 reales (X87Math.cpp) contra el oraculo: compila X87Math.cpp y offline_harness.cpp con cl de 32 bits (mismas opciones que el proyecto:
/O2 /Oi /arch:IA32 /fp:fast), ejecuta el arnes sobre las 4096 fixtures y compara cada fila con fixture_oracle.expected (sin tolerancias: EXE-oraculo == ayudante en bits, salvo comodines).

    python -B offline_check.py [raiz-del-arbol]       # por defecto, el worktree que contiene este banco

No ejecuta GTA ni compila el proyecto; el arnes copia el flujo de Camera.cpp (ver offline_harness.cpp). Tarda unos minutos (X87Math.cpp es grande).
"""
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else HERE.parents[1]
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle
import fixture_selfcheck as selfcheck

VCVARS32 = next(Path('C:/Program Files/Microsoft Visual Studio').glob('*/*/VC/Auxiliary/Build/vcvars32.bat'))


def build_and_run(tmp):
    tmp = Path(tmp)
    inc = tmp / 'init_list.inc'
    inc.write_text(''.join(f'        put(0x{a}, 0x{v});\n' for a, v in re.findall(r'put\(0x([0-9A-Fa-f]+), 0x([0-9A-Fa-f]+)\)', (ROOT / 'source/game_sa/Camera.cpp').read_text(encoding='utf-8'))), encoding='utf-8') \
        if (HERE.name == "CameraShake30") else None
    bat = tmp / 'build.bat'
    flags = '/nologo /O2 /Oi /arch:IA32 /fp:fast /EHsc /std:c++20'
    bat.write_text('\n'.join([
        '@echo off',
        f'call "{VCVARS32}" >nul 2>&1',
        f'cl {flags} /I "{ROOT / "source/game_sa"}" /I "{HERE}" /I "{tmp}" "{HERE / "offline_harness.cpp"}" "{ROOT / "source/game_sa/X87Math.cpp"}" /Fo"{tmp}\\\\" /Fe"{tmp / "harness.exe"}"',
        'exit /b %ERRORLEVEL%']), encoding='ascii')
    subprocess.run(['cmd', '/c', str(bat)], check=True, stdout=subprocess.DEVNULL)
    return subprocess.run([str(tmp / 'harness.exe')], check=True, capture_output=True, text=True).stdout


if __name__ == '__main__':
    rows = selfcheck.dump_rows()[1:]
    with tempfile.TemporaryDirectory() as tmp:
        out = build_and_run(tmp)
    results = [json.loads(line) for line in out.splitlines()]
    assert len(results) == oracle.CASES == len(rows)
    bad = []
    for inp, got in zip(rows, results):
        want, _ = oracle.expected(inp, got['case'])
        got = {k: v for k, v in got.items() if k != 'case'}
        if not oracle.matches(got, want):
            bad.append((got['case'] if 'case' in got else inp['case'], got, want))
    print('casos', len(results), 'diferencias', len(bad))
    for case, got, want in bad[:5]:
        print('caso', case)
        print('  arnes   ', {k: v for k, v in got.items() if k != 'table'})
        print('  oraculo ', {k: v for k, v in want.items() if k != 'table'})
    sys.exit(1 if bad else 0)
