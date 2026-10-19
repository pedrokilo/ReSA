"""Comprobacion de ANFITRION del banco CBDNRenderCB30 (sin juego): compila el TEXTO REAL de CCustomBuildingDNPipeline::CustomPipeRenderCB (extraido de
source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) contra una maqueta de los tipos del juego (host_harness.cpp), lo ejecuta con las 6144 fixtures reales y compara lo
observado con fixture_oracle.py (modelado desde el EXE). Comprueba la FUENTE C++ contra el oraculo; no sustituye al banco EXE-vs-DLL.
Uso:  python -I host_check.py [ruta-al-CustomBuildingDNPipeline.cpp]   (por defecto el de este arbol)"""
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
    a = text.index('// 0x5D6480')
    b = text.index('// 0x5D6E90')
    return text[a:b]


def main():
    src = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / 'source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp'
    with tempfile.TemporaryDirectory() as tmp:
        (Path(tmp) / 'host_function.inc').write_text(extract(src), encoding='utf-8')
        exe = Path(tmp) / 'host_harness.exe'
        cmd = f'call "{VCVARS}" >nul && cl /nologo /EHsc /std:c++20 /I"{tmp}" /I"{HERE}" /Fe:"{exe}" /Fo:"{tmp}\\\\" "{HERE / "host_harness.cpp"}"'
        r = subprocess.run('cmd /s /c "' + cmd + '"', capture_output=True, text=True, cwd=str(HERE))
        if r.returncode:
            raise SystemExit('cl fallo:\n' + r.stdout[-4000:] + r.stderr)
        import fixture_check
        for offs in (('0x28',), ('0x1C',), ('0x7C',)):
            inputs = fixture_check.dump(offs)[1:]
            out = subprocess.run([str(exe), *offs], capture_output=True, text=True, check=True).stdout.splitlines()
            assert len(out) == len(inputs) == oracle.CASES[oracle.NAME]
            bad = 0
            for n, (inp, line) in enumerate(zip(inputs, out)):
                want, _ = oracle.expected(oracle.NAME, inp, n)
                diff = oracle.compare(json.loads(line), want)
                if diff:
                    bad += 1
                    if bad <= 5:
                        print('caso', n, 'claves', diff, {k: json.loads(line).get(k) for k in diff}, {k: want[k] for k in diff})
            print('desplazamientos', offs, ':', len(inputs), 'casos,', bad, 'diferencias')
            if bad:
                raise SystemExit(1)
    print('ok: la fuente C++ coincide con el oraculo en las 6144 fixtures')


if __name__ == '__main__':
    main()
