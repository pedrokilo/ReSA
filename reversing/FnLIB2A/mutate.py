"""Prueba negativa del banco LIB2A. Copia libs/libpng-1.0.12 a un directorio de trabajo y mutiliza UNA funcion por indice k (1..N, orden de tabla.txt) de forma
generica, controlada por la variable de entorno LIB2A_MUT=k (la fuente buena nunca se toca):
  ronda 1: se perturba el PRIMER 'return' de la funcion (valor devuelto ^ 1, + 1.0f en float, puntero + 1); en las void, se perturba el estado (png_ptr->crc ^= 1) al entrar.
  ronda 2: se perturba el ULTIMO 'return' (^ 2, + 2.0f, puntero + 2); en las void, info_ptr->width ^= 1 (o png_ptr->flags ^= 0x100 si no hay info_ptr) al salir del primer bloque de entrada.
La perturbacion solo se dispara cada 5 llamadas a la funcion mutada (contador interno), para que sea una desviacion intermitente como las de las rondas negativas de Fn230A/LIB1A.
Uso: python mutate.py <ronda 1|2> <directorio destino>"""
import re, shutil, sys
from pathlib import Path
import protos

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / 'libs' / 'libpng-1.0.12'
HERE = Path(__file__).resolve().parent
P = protos.load()
HEAD = '''
/* LIB2A prueba negativa */
extern int lib2a_mutq(int k);
'''
def find_defs(text, name):
    out = []
    for m in re.finditer(r'(?m)^' + re.escape(name) + r'\s*\(', text):
        mb = re.compile(r'\n([ \t]*)\{').search(text, m.end())
        if not mb: continue
        b = mb.start()
        sig = text[m.start():b]
        if ';' in sig: continue
        e = re.compile(r'\n' + mb.group(1) + r'\}[ \t]*\n').search(text, b).start()
        out.append((m.start(), b, e))
    return out
def mutate_return(body, k, rnd, rtype):
    blank = re.sub(r'/\*.*?\*/', lambda mm: ' ' * len(mm.group(0)), body, flags=re.S)
    rets = list(re.finditer(r'\breturn\s*\(?([^;]*?)\)?\s*;', blank))
    if not rets: return body, False
    c = '1' if rnd == 1 else '2'
    rt = rtype.replace('PNGAPI', '').strip()
    targets = rets[:1] if rnd == 1 else rets
    for m in reversed(targets):
        expr = body[m.start(1):m.end(1)].strip() or '0'
        if rt == 'float':
            new = f'return (lib2a_mutq({k}) ? (({expr}) + {c}.0f) : ({expr}));'
        elif rt.endswith('p') or rt.endswith('pp') or rt.endswith('pf') or '*' in rt:
            new = f'return (lib2a_mutq({k}) ? (({rt})((char*)({expr}) + {c})) : ({expr}));'
        else:
            new = f'return (({rt})(lib2a_mutq({k}) ? (({expr}) ^ {c}) : ({expr})));'
        body = body[:m.start()] + new + body[m.end():]
    return body, True
def mutate_void(body, k, rnd, has_info, args=''):
    has_info = 'png_infop info_ptr' in args
    first = body.find('{') + 1
    nl = chr(10)
    if 'png_structp png_ptr' not in args:
        if 'row_info' in args and 'row' in args:
            ins = nl + '   if (row != NULL && lib2a_mutq(%d)) row[%d] ^= 1;' % (k, 0 if rnd == 1 else 1) + nl
        else:
            ins = nl + '   if (lib2a_mutq(%d)) return;' % k + nl
        return body[:first] + ins + body[first:], True
    if rnd == 1:
        ins = nl + '   if (png_ptr != NULL && lib2a_mutq(%d)) png_ptr->flags ^= 0x80000000;' % k + nl
        return body[:first] + ins + body[first:], True
    tgt = 'png_ptr->mode ^= 0x40000000'
    guard = 'png_ptr != NULL'
    ins = nl + '   if (%s && lib2a_mutq(%d)) %s;' % (guard, k, tgt) + nl
    return body[:first] + ins + body[first:], True
def main():
    rnd = int(sys.argv[1]); dst = Path(sys.argv[2])
    if dst.exists(): shutil.rmtree(dst)
    shutil.copytree(SRC, dst, ignore=shutil.ignore_patterns('CMakeLists.txt'))
    names = (HERE / 'validadas.txt').read_text().split()
    files = {f.name: f.read_text(errors='replace') for f in dst.glob('png*.c')}
    done = {}
    for k, n in enumerate(names, 1):
        ret = P[n][0]; args = ' '.join(P[n][1])
        for fn in list(files):
            ds = find_defs(files[fn], n)
            for (st, bb, e) in reversed(ds):
                t = files[fn]; body = t[bb:e]
                if ret.strip() == 'void': nb, ok = mutate_void(body, k, rnd, 'info_ptr' in args, args)
                else: nb, ok = mutate_return(body, k, rnd, ret)
                if ok: files[fn] = t[:bb] + nb + t[e:]; done[n] = fn
    for fn, t in files.items():
        (dst / fn).write_text(HEAD + t)
    (dst / 'mutaciones.txt').write_text(chr(10).join(names) + chr(10))
    missing = [n for n in names if n not in done]
    print('mutadas', len(done), 'de', len(names), 'sin mutar:', missing)
main()
