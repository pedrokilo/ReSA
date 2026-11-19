"""Compara el oraculo de 0x746480 (oracle.py, desde el desensamblado) con el CRT de MSVC (crt.out, de crtbatch.exe) y con el
codigo real del EXE emulado (emu_orig.py) sobre cases.txt / groups.txt. Escribe resultado.json.

Clases por caso (original = separador 0x746480, CRT = __argv del UCRT):
  iguales        argc y todas las cadenas coinciden
  ub-original    el original deja ranuras sin inicializar (basura del heap): comportamiento indefinido, el CRT devuelve ""
  efecto-vms     difieren y cambia el efecto observable (ver abajo)
  benigno        difieren pero el efecto observable es el mismo

Efecto observable: el unico consumidor de las cadenas es RsEventHandler(rsPREINITCOMMANDLINE / rsCOMMANDLINE, argv[i]);
rsCOMMANDLINE acaba en rsFILELOAD (sin manejador: no hace nada) y rsPreInitCommandLine solo compara con "-vms".
El EXE original recorre i = 1..argc-1; el C++ del proyecto (WinMain.cpp) recorre i = 0..argc-1. Hay efecto si
hay_vms(original, i>=1) != hay_vms(CRT, i>=0) o != hay_vms(CRT, i>=1).
"""
import collections
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import emu_orig
import oracle


def parse_crt(line):
    parts = line.rstrip('\n').split('|')
    return int(parts[0]), [bytes.fromhex(x) for x in parts[1:]]


def vms(args, start):
    return any(a == b'-vms' for a in args[start:] if a is not None)


def rasgos(cmd):
    r = []
    if any(c < 0x20 and c != 9 for c in cmd):
        r.append('separador-de-control(<0x20 salvo tab)')
    if b'\\"' in cmd:
        r.append('barra-invertida+comilla')
    if b'""' in cmd:
        r.append('comillas-dobles-seguidas')
    if any(c >= 0x80 for c in cmd):
        r.append('bytes-altos')
    if cmd.count(b'"') % 2:
        r.append('comillas-impares')
    return r or ['otro(comillas intermedias o vacias)']


def show(l):
    return [None if x is None else x.decode('latin-1') for x in l]


def main():
    cases = [bytes.fromhex(l.strip()) for l in open(HERE / 'cases.txt')]
    groups = [l.strip() for l in open(HERE / 'groups.txt')]
    crt = [parse_crt(l) for l in open(HERE / 'crt.out')]
    assert len(cases) == len(crt) == len(groups), (len(cases), len(crt), len(groups))
    em = emu_orig.Emu()
    ebad = 0
    glob = collections.Counter()
    por_grupo = collections.defaultdict(collections.Counter)
    causas = collections.Counter()
    ej = collections.defaultdict(list)
    for cmd, g, (cargc, cargv) in zip(cases, groups, crt):
        # 1) el oraculo contra el codigo real del EXE (Unicorn)
        argc, eax, p, n, b, regs, dsp = em.run(cmd)
        oc, ob = oracle.orig_split(cmd, base=p)
        if not (argc == oc and eax == p and b == ob and n == len(ob)
                and regs == (0x11111111, 0x22222222, 0x33333333, 0x44444444) and dsp == 4):
            ebad += 1
        # 2) el oraculo contra el CRT
        oc, blk = oracle.orig_split(cmd)
        oargv = oracle.argv_of(oc, blk)
        crt_ok = cargc == len(cargv)
        if oargv == cargv and crt_ok:
            k = 'iguales'
        elif any(a is None for a in oargv):
            k = 'ub-original'
        elif (vms(oargv, 1) != vms(cargv, 0)) or (vms(oargv, 1) != vms(cargv, 1)):
            k = 'efecto-vms'
            for r in rasgos(cmd):
                causas[r] += 1
        else:
            k = 'benigno'
        glob[k] += 1
        por_grupo[g][k] += 1
        if k != 'iguales' and len(ej[(g, k)]) < 12:
            ej[(g, k)].append({'linea': cmd.decode('latin-1')[:160], 'original': show(oargv)[:6], 'crt': show(cargv)[:6]})
    total = len(cases)
    print(f'EXE emulado vs oraculo: {total} casos, {ebad} diferencias')
    print(f'oraculo vs CRT: {total} casos: ' + ', '.join(f'{k}={v}' for k, v in sorted(glob.items())))
    for g in sorted(por_grupo):
        print(f'  grupo {g}: ' + ', '.join(f'{k}={v}' for k, v in sorted(por_grupo[g].items())))
    print('rasgos de los casos con efecto-vms:', dict(causas))
    out = {
        'exe_sha256': emu_orig.SHA, 'va': '0x00746480', 'sha256_cuerpo': emu_orig.CODE_SHA,
        'emulacion_vs_oraculo': {'casos': total, 'diferencias': ebad},
        'oraculo_vs_crt': {'casos': total, 'por_clase': dict(glob), 'por_grupo': {g: dict(c) for g, c in por_grupo.items()},
                           'rasgos_de_efecto_vms': dict(causas)},
        'ejemplos': {f'{g}/{k}': v for (g, k), v in sorted(ej.items())},
    }
    (HERE / 'resultado.json').write_text(json.dumps(out, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    return 1 if ebad else 0


if __name__ == '__main__':
    sys.exit(main())
