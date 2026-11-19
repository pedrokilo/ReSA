"""Lote PLAN1: elige la muestra estratificada (semilla fija) y vuelca su desensamblado para revisarla a mano.
Uso: python -I muestra.py --exe <gta_sa.exe> --repo <raiz> [--listar]
Con --listar solo imprime las direcciones elegidas; sin el, imprime el desensamblado (14 instrucciones, hasta ret)."""
import argparse, csv, random, struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

RAW = {'.text': (0x401000, 1024, 4546048), '_rwcseg': (0x857000, 4547072, 1536)}
ESTRATOS = [('A juego/excluida', lambda r: r['confianza'] == 'segura' and r['categoria'] in ('juego', 'excluida'), 8),
            ('A RenderWare interno', lambda r: r['confianza'] == 'segura' and r['categoria'] == 'RenderWare interno', 10),
            ('A no reconstruible (QuickTime)', lambda r: r['categoria'] == 'RenderWare no reconstruible', 2),
            ('A biblioteca', lambda r: r['confianza'] == 'segura' and r['categoria'].startswith('biblioteca'), 5),
            ('B juego', lambda r: r['confianza'] == 'probable' and r['categoria'] == 'juego', 8),
            ('B RenderWare interno', lambda r: r['confianza'] == 'probable' and r['categoria'] == 'RenderWare interno', 8),
            ('B biblioteca', lambda r: r['confianza'] == 'probable' and r['categoria'].startswith('biblioteca'), 2),
            ('C ruido', lambda r: r['confianza'] == 'ruido', 12)]


def elegir(repo):
    rows = list(csv.DictReader(open(Path(repo) / 'docs/ExeIndependiente/raices.csv', encoding='utf-8')))
    rnd = random.Random(20261118)
    out = []
    for nombre, f, n in ESTRATOS:
        c = [r for r in rows if f(r)]
        out += [(nombre, r) for r in rnd.sample(c, min(n, len(c)))]
    return out


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', required=True)
    ap.add_argument('--repo', required=True)
    ap.add_argument('--listar', action='store_true')
    ap.add_argument('--fija', action='store_true', help='usa las 55 direcciones revisadas a mano (verificacion_muestra.M) en vez de volver a sortear')
    a = ap.parse_args()
    exe = Path(a.exe).read_bytes()
    md = Cs(CS_ARCH_X86, CS_MODE_32)

    def off(v):
        for n, (b, o, s) in RAW.items():
            if b <= v < b + s:
                return o + v - b

    if a.fija:
        import sys
        sys.path.insert(0, str(Path(__file__).resolve().parent))
        from verificacion_muestra import M
        allr = {int(r['direccion'], 16): r for r in csv.DictReader(open(Path(a.repo) / 'docs/ExeIndependiente/raices.csv', encoding='utf-8'))}
        lista = [(M[v][0], allr[v]) for v in M]
    else:
        lista = elegir(a.repo)
    for est, r in lista:
        v = int(r['direccion'], 16)
        print(f"=== {r['direccion']} [{est}] {r['modulo']} bytes={r['bytes']} prev={r['prev']} auto={r['veredicto_auto']} uso={r['tipo_uso']}")
        print('   fuente:', r['llamadoras_fuente'][:90], '|', r['texto_fuente'][:100])
        if a.listar:
            continue
        o = off(v)
        o0 = o - 6
        print('   bytes previos:', exe[o - 6:o].hex(), ' primeros:', exe[o:o + 8].hex())
        p = v
        for _ in range(14):
            ins = next(md.disasm_lite(exe[off(p):off(p) + 15], p), None)
            if not ins:
                print('   (no decodifica)')
                break
            _, size, mn, ops = ins
            print(f'   {p:08X}  {mn} {ops}')
            p += size
            if mn in ('ret', 'retn'):
                break
