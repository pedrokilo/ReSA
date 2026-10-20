"""Prueba espejo (sin juego): compila los cuerpos LITERALES de HasCarStoppedBecauseOfLight, Render y UpdateWheelMatrix extraidos de Automobile.cpp contra tipos minimos
(mock_prelude.hpp, con la disposicion de memoria del EXE y los mismos falsos que Core.hpp) con cl x86, los ejecuta sobre las fixtures y compara con fixture_oracle.py.

  python -I mirror_check.py <Automobile.cpp> <rows.jsonl> [/Od | /O2]

Requisitos: cl x86 (vcvars32) y rows.jsonl de dump_fixtures.exe. Los ayudantes x87 (X87Math.cpp) se compilan con las opciones del proyecto (/O2 /Oi /arch:IA32 /fp:fast).
"""
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
import fixture_oracle as oracle

VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
GAME_SA = HERE.parents[2] / 'source' / 'game_sa'
HEADS = ('bool CAutomobile::HasCarStoppedBecauseOfLight() {', 'void CAutomobile::Render() {', 'void CAutomobile::UpdateWheelMatrix(int32 nodeIndex, int32 flags) {')


def extract(text, head):
    start = text.index(head)
    end = text.index('\n}\n', start) + 3
    return text[start:end]


def main(source, rows_path, opt):
    text = Path(source).read_text(encoding='utf-8')
    (HERE / 'mirror_bodies.inc').write_text('\n'.join(extract(text, h) for h in HEADS), encoding='utf-8')
    work = Path(tempfile.mkdtemp(prefix='aw60_mirror_'))
    bat = work / 'build.bat'
    bat.write_text('\n'.join([
        '@echo off', 'call "%s" >nul' % VCVARS, 'cd /d %s' % work,
        'cl /nologo /EHsc /std:c++20 /O2 /Oi /arch:IA32 /fp:fast /c /Fo:x87.obj /I "%s" "%s"' % (GAME_SA, GAME_SA / 'X87Math.cpp'),
        'cl /nologo /EHsc /std:c++20 %s /I "%s" /I "%s" /Fe:mirror.exe "%s" x87.obj' % (opt, GAME_SA, HERE, HERE / 'mirror_main.cpp'),
    ]), encoding='ascii')
    out = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True)
    if not (work / 'mirror.exe').exists():
        print(out.stdout[-6000:], out.stderr[-2000:])
        return 2
    run = subprocess.run([str(work / 'mirror.exe')], capture_output=True, text=True)
    got = {}
    for line in run.stdout.splitlines():
        r = json.loads(line)
        got[(r['fn'], r['case'])] = r['observed']
    bad = [0, 0, 0]
    seen = [0, 0, 0]
    for line in open(rows_path, encoding='utf-8'):
        r = json.loads(line)
        fn, case = r['fn'], r['case']
        want, _ = oracle.expected(r['input'], fn)
        seen[fn] += 1
        if (fn, case) not in got or not oracle.matches(got[(fn, case)], want):
            bad[fn] += 1
            if bad[fn] <= 3:
                print('DIFERENCIA', oracle.NAMES[fn], 'caso', case)
                print('  C++    ', json.dumps(got.get((fn, case)))[:1500])
                print('  oraculo', json.dumps(want)[:1500])
    print({oracle.NAMES[i]: (seen[i], bad[i]) for i in range(3)}, opt)
    return 1 if any(bad) else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1], sys.argv[2], sys.argv[3] if len(sys.argv) > 3 else '/Od'))
