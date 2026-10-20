"""Espejo del generador (sin el juego): compila fixture_mirror.cpp (solo Fixtures.hpp, C++ estandar con cl), reconstruye la fila `input` de cada caso de las tres funciones y la pasa por el oraculo.
Comprueba: ningun caso invalido (el oraculo lanza Fail si el EXE leeria un byte sin especificar, desreferenciaria un puntero que el banco no modela, ...), las constantes de arena del C++ (linea Z)
frente a las del oraculo (SIZES), cobertura frente a los minimos de run_local.py, y muestra los recuentos.
Uso:  python -I fixture_mirror.py [--counts]
"""
import json
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle  # noqa: E402

VS = Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC')


def build_and_run():
    out = Path(tempfile.mkdtemp(prefix='pedwrappers60-'))
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O2 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" >"%s" 2>&1\r\n"%s" >"%s"\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'm.exe', out, HERE / 'fixture_mirror.cpp', out / 'log.txt', out / 'm.exe', out / 'rows.txt'), encoding='ascii')
    subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(HERE))
    if not (out / 'rows.txt').exists() or (out / 'rows.txt').stat().st_size == 0:
        raise RuntimeError('No se pudo compilar o ejecutar el espejo: ' + (out / 'log.txt').read_text(errors='replace')[-3000:])
    return [l for l in (out / 'rows.txt').read_text().splitlines() if l.strip()]


FUNCS = {'K': oracle.NAME_CS, 'S': oracle.NAME_PS, 'P': oracle.NAME_PC}


def main():
    lines = build_and_run()
    z = json.loads(lines.pop()[2:])['sizes']
    want = {k: list(v) for k, v in oracle.SIZES.items()}
    if z != want:
        raise SystemExit('Tamanos de arena distintos entre Fixtures.hpp y el oraculo: %s != %s' % (z, want))
    counts = {oracle.NAME_CS: 0, oracle.NAME_PS: 0, oracle.NAME_PC: 0}
    cov = {k: {} for k in counts}
    invalid = []
    for line in lines:
        kind, body = line[0], line[2:]
        name = FUNCS[kind]
        n = counts[name]
        counts[name] += 1
        inp = json.loads(body)
        try:
            want_, stats = oracle.expected(name, inp, n)
        except oracle.Fail as e:
            invalid.append((name, n, inp['family'], str(e)))
            continue
        for k, v in stats.items():
            if v is True:
                cov[name][k] = cov[name].get(k, 0) + 1
    if counts != oracle.CASES_BY_NAME:
        raise SystemExit('numero de casos inesperado: %s' % counts)
    if invalid:
        import collections
        print(json.dumps(collections.Counter((i[0], i[3]) for i in invalid).most_common(12), indent=1))
        print(json.dumps(invalid[:10]))
        raise SystemExit('%d casos invalidos (el EXE desreferenciaria o leeria algo que el banco no especifica)' % len(invalid))
    import run_local
    short = {}
    for name, mins in run_local.MINIMUMS.items():
        for k, m in mins.items():
            if cov[name].get(k, 0) < m:
                short['%s:%s' % (name, k)] = (cov[name].get(k, 0), m)
    if '--counts' in sys.argv or short:
        print(json.dumps({n: dict(sorted(c.items())) for n, c in cov.items()}, indent=1))
    print(json.dumps(dict(cases=counts, shortfalls=short)))
    if short:
        raise SystemExit('cobertura insuficiente')
    print('ok')


if __name__ == '__main__':
    main()
