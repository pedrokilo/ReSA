"""Comprobacion SIN el juego del cuerpo C++ de FxInfoManager_c::Load contra el oraculo (ver host_check.cpp).

  python -I host_check.py [directorio_temporal]

Extrae la tabla de nombres y el cuerpo de Load de source/game_sa/Fx/FxInfoManager.cpp, sustituye el sscanf del EXE (0x8220AD) por el del CRT del host, compila host_check.cpp con cl x86
(/Od /RTC1, como la DLL en Debug), ejecuta los 4608 casos con los falsos de Core.hpp y compara lo observable con fixture_oracle.py. Sale con codigo 1 si hay diferencias.
Requiere Visual Studio (vcvars32.bat) y nlohmann/json (cache de conan del repositorio).
"""
import json
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'


def extract(src):
    text = src.read_text(encoding='utf-8').replace('\r\n', '\n')
    a = text.index('constexpr struct { int32 type; const char* name; } FXINFOMANAGER_C_LOAD_MAPPING[] = {')
    b = text.index('\n};\n', a) + 4
    table = text[a:b]
    a = text.index('void FxInfoManager_c::Load(FILESTREAM file, int32 version) {')
    b = text.index('\n}\n', a) + 3
    body = text[a:b]
    assert '0x8220AD' in body
    body = body.replace('reinterpret_cast<ExeSscanf>(0x8220AD)', 'reinterpret_cast<ExeSscanf>(&host_sscanf)')
    return table + '\n' + body


def run_bat(tmp, name, lines):
    bat = tmp / name
    bat.write_text('@echo off\r\n' + '\r\n'.join(lines) + '\r\n', encoding='ascii')
    return subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, errors='replace')


def main(tmp):
    tmp = Path(tmp)
    tmp.mkdir(parents=True, exist_ok=True)
    (tmp / 'load_extract.inc').write_text(extract(ROOT / 'source/game_sa/Fx/FxInfoManager.cpp'), encoding='utf-8')
    nlohmann = next((ROOT / 'reversing/CVehicle/conan-cache/p').glob('nlohm*/p/include'))
    r = run_bat(tmp, 'build.bat', [
        'call "%s" >nul 2>&1' % VCVARS, 'cd /d "%s"' % tmp,
        'cl /nologo /EHsc /std:c++20 /Od /RTC1 /I"%s" /I"%s" /I"%s" /I"%s" "%s" /Fe:host_check.exe' % (HERE, tmp, nlohmann, ROOT / 'source/game_sa/Fx', HERE / 'host_check.cpp')])
    if not (tmp / 'host_check.exe').exists():
        print(r.stdout[-4000:], r.stderr[-2000:])
        return 2
    out = subprocess.run([str(tmp / 'host_check.exe')], capture_output=True, text=True, encoding='utf-8')
    rows = [json.loads(line) for line in out.stdout.splitlines() if line.startswith('{')]
    if out.returncode != 0 or len(rows) != oracle.CASES[oracle.NAME]:
        print('el ejecutable fallo o no produjo todos los casos:', out.returncode, len(rows), out.stderr[-1000:])
        return 2
    # los `input` los vuelca fixture_dump.exe (mismo Fixtures.hpp)
    run_bat(tmp, 'dump.bat', ['call "%s" >nul 2>&1' % VCVARS, 'cd /d "%s"' % tmp,
                              'cl /nologo /EHsc /std:c++20 /O2 /I"%s" "%s" /Fe:fixture_dump.exe >nul' % (HERE, HERE / 'fixture_dump.cpp')])
    dump = subprocess.run([str(tmp / 'fixture_dump.exe')], capture_output=True, text=True, encoding='ascii')
    inputs = [json.loads(line) for line in dump.stdout.splitlines() if line.strip()]
    if len(inputs) != len(rows):
        print('fixture_dump.exe no produjo los %d casos: %d' % (len(rows), len(inputs)))
        return 2
    bad = []
    for row, inp in zip(rows, inputs):
        n = row['case']
        try:
            want, stats = oracle.expected(oracle.NAME, inp, n)
        except oracle.Fail as e:
            bad.append((n, 'fixture invalido: %s' % e))
            continue
        diff = oracle.compare(row['observed'], want)
        if diff:
            bad.append((n, diff, inp['kind'], inp['stream'][:120], {k: row['observed'].get(k) for k in diff if k not in ('trace_head', 'trace_tail')},
                        {k: want[k] for k in diff if k not in ('trace_head', 'trace_tail')}))
    print(json.dumps({'cases': len(rows), 'diferencias': len(bad), 'primeras': bad[:6]}, indent=1, default=str)[:6000])
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1] if len(sys.argv) > 1 else tempfile.mkdtemp(prefix='fxinfoload28_')))
