"""Autoprueba del FUENTE (sin el juego ni el EXE): compila el cuerpo REAL de CPed::ProcessControl, SpecialEntityPreCollisionStuff y SpecialEntityCalcCollisionSteps de
source/game_sa/Entity/Ped/Ped.cpp (extraido tal cual) contra las cabeceras REALES del proyecto, con las opciones de depuracion del proyecto, en un programa autonomo (selftest_source.cpp) cuyos
colaboradores son los mismos falsos que usa el banco; ejecuta las 14336 fixtures y compara cada resultado con fixture_oracle.py (el mismo `compare_side` que run_local.py). Es independiente del
EXE y del juego: comprueba que la transcripcion C++ coincide con el oraculo (escrito desde el desensamblado y validado contra el EXE emulado en selftest_exe.py). No sustituye al banco en juego.
Uso:  python -I selftest_source.py            (imprime «ok» o los casos distintos)
"""
import json
import re
import shlex
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle  # noqa: E402
import fixture_mirror  # noqa: E402
import run_local  # noqa: E402

PED = (ROOT / 'source/game_sa/Entity/Ped/Ped.cpp').read_text(encoding='utf-8').replace('\r\n', '\n')
VCXPROJ = Path('D:/Original/gta-reversed-master/build/source/gta_reversed.vcxproj')
VS = Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC')


def extract():
    a = PED.index('namespace {\n// rand() del CRT del EXE (0x821B1E)')
    b = PED.index('/*!\n* @addr 0x5E4110\n')          # Teleport: lo que hay entre ProcessControl y SpecialEntityPreCollisionStuff
    c = PED.index('/*!\n* @addr 0x5E3C30\n')
    d = PED.index('/*!\n* @addr 0x5E8A20\n')          # PreRender
    text = PED[a:b] + '\n' + PED[c:d]
    text = text.replace('reinterpret_cast<int(__cdecl*)()>(0x821B1E)()', 'PedTestExeRand()')
    assert 'PedTestExeRand()' in text
    return text


def flags():
    t = VCXPROJ.read_text(encoding='utf8')
    m = re.search(r"<ItemDefinitionGroup Condition=\"'\$\(Configuration\)\|\$\(Platform\)'=='Debug\|Win32'\">(.*?)</ItemDefinitionGroup>", t, re.S)
    g = m.group(1)
    inc = re.search('<AdditionalIncludeDirectories>(.*?)</AdditionalIncludeDirectories>', g, re.S).group(1).replace('%(AdditionalIncludeDirectories)', '')
    inc = inc.replace('gta-reversed-master\\source', 'wc-3\\source')
    defs = re.search('<PreprocessorDefinitions>(.*?)</PreprocessorDefinitions>', g, re.S).group(1).replace('%(PreprocessorDefinitions);', '')
    opts = re.search('<AdditionalOptions>(.*?)</AdditionalOptions>', g, re.S).group(1).replace('%(AdditionalOptions)', '')
    args = ['/nologo', '/std:c++latest', '/EHsc', '/permissive-', '/Zc:preprocessor', '/D_DEBUG', '/MDd'] + ['/I' + x for x in inc.split(';') if x] + ['/D' + x for x in defs.split(';') if x]
    tk = shlex.split(opts.replace('\\', '/'))
    i = 0
    while i < len(tk):
        if tk[i] == '/external:I':
            args.append('/I' + tk[i + 1])
            i += 2
        else:
            args.append(tk[i])
            i += 1
    for w in (4200, 4251, 4275, 4996, 26495, 26812, 28159, 4005, 4091, 4244, 4267, 4305, 4018):
        args.append('/wd%d' % w)
    return args


def rsp(path, args):
    path.write_text('\n'.join('"%s"' % a.replace('\\', '/') for a in args))


def build():
    out = Path(tempfile.mkdtemp(prefix='pedwrappers60-source-'))
    (out / 'ped_funcs.inc').write_text(extract(), encoding='utf-8')
    gs = ROOT / 'source/game_sa'
    base = flags()
    # el cuerpo de las funciones se compila como el resto del proyecto en Debug (/Od con comprobaciones de ejecucion); X87Math.cpp con sus propias opciones (/O2 /Oi /arch:IA32 /fp:fast)
    rsp(out / 'main.rsp', base + ['/Od', '/Zi', '/RTC1', '/I' + str(out), '/I' + str(HERE), '/c', '/Fo' + str(out) + '\\main.obj', '/Fd' + str(out) + '\\main.pdb', str(HERE / 'selftest_source.cpp')])
    rsp(out / 'x87.rsp', base + ['/O2', '/Oi', '/arch:IA32', '/fp:fast', '/Zi', '/c', '/Fo' + str(out) + '\\x87.obj', '/Fd' + str(out) + '\\x87.pdb', str(gs / 'X87Math.cpp')])
    def link(base, name):
        bat = out / ('l_' + name + '.bat')
        bat.write_text('@echo off\r\ncall "%s" >nul\r\nlink /nologo /FORCE:UNRESOLVED /BASE:0x%X /FIXED /DYNAMICBASE:NO /INCREMENTAL:NO /DEBUG /OUT:"%s" "%s" "%s" >>"%s" 2>&1\r\n' % (
            VS / 'Auxiliary/Build/vcvars32.bat', base, out / name, out / 'main.obj', out / 'x87.obj', out / 'log.txt'), encoding='ascii')
        subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True)
    bat = out / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl @"%s" >"%s" 2>&1\r\ncl @"%s" >>"%s" 2>&1\r\n' % (
        VS / 'Auxiliary/Build/vcvars32.bat', out / 'x87.rsp', out / 'log.txt', out / 'main.rsp', out / 'log.txt'), encoding='ascii')
    subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True)
    link(0x400000, 't0.exe')
    if not (out / 't0.exe').exists():
        log = (out / 'log.txt').read_bytes().decode('cp1252', errors='replace')
        errs = [l for l in log.splitlines() if 'error' in l.lower() or 'LNK' in l]
        raise RuntimeError('No se pudo compilar/enlazar (' + str(out) + '):' + chr(10) + chr(10).join(errs[:40]))
    # la seccion .gm (globales del juego) debe quedar en 0xA90000..0xC60000: base = multiplo de 64 KB tal que base + RVA(.gm) <= 0xA90000
    d = (out / 't0.exe').read_bytes()
    pe = struct.unpack_from('<I', d, 0x3C)[0]
    nsec = struct.unpack_from('<H', d, pe + 6)[0]
    optsz = struct.unpack_from('<H', d, pe + 20)[0]
    rva = None
    for k in range(nsec):
        o = pe + 24 + optsz + 40 * k
        if d[o:o + 8].rstrip(bytes(1)) == b'.gm':
            rva = struct.unpack_from('<I', d, o + 12)[0]
    if rva is None:
        raise RuntimeError('No hay seccion .gm')
    base = ((0xA90000 - rva) // 0x10000) * 0x10000
    if base < 0x10000:
        raise RuntimeError('La imagen es demasiado grande para colocar .gm en 0xA90000')
    link(base, 't.exe')
    return out / 't.exe'


def main():
    exe = build()
    res = subprocess.run([str(exe)], capture_output=True, text=True)
    got = [l for l in res.stdout.splitlines() if l.strip()]
    if not got or got[0][0] != 'A':
        raise RuntimeError('El programa fallo (codigo %d): %s' % (res.returncode, res.stderr[-1000:]))
    addrs = json.loads(got.pop(0)[2:])
    lines = fixture_mirror.build_and_run()
    lines.pop()
    if len(got) != len(lines):
        raise SystemExit('Numero de casos distinto: %d != %d (codigo de salida %d) %s' % (len(got), len(lines), res.returncode, res.stderr[-500:]))
    counts = {oracle.NAME_CS: 0, oracle.NAME_PS: 0, oracle.NAME_PC: 0}
    bad = 0
    for g, line in zip(got, lines):
        name = fixture_mirror.FUNCS[line[0]]
        if g[0] != line[0]:
            raise SystemExit('Orden de casos inesperado')
        n = counts[name]
        counts[name] += 1
        inp = json.loads(line[2:])
        inp['addrs'] = addrs
        want, _ = oracle.expected(name, inp, n)
        obs = json.loads(g[2:])
        try:
            run_local.compare_side(name, {'case': n, 'input': inp}, {'observed': obs}, want, 'FUENTE')
        except RuntimeError as e:
            bad += 1
            if bad <= 10:
                print('DIFF', name, n, str(e)[:1500])
    if bad:
        raise SystemExit('%d diferencias de %d casos' % (bad, len(got)))
    print('ok (%d casos: el cuerpo C++ real coincide con el oraculo)' % len(got))


if __name__ == '__main__':
    main()
