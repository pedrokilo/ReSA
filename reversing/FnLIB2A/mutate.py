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
def find_def(text, name):
    for m in re.finditer(r'(?m)^' + re.escape(name) + r'\s*\(', text):
        # el cuerpo empieza en la primera '{' en columna 0 tras la firma
        mb = re.compile(r'\n([ \t]*)\{').search(text, m.end()); b = mb.start()
        e = re.compile(r'\n' + mb.group(1) + r'\}[ \t]*\n').search(text, b).start()
        sig = text[m.start():b]
        if ';' in sig: continue
        return m.start(), b, e
    return None
def mutate_return(body, k, rnd, rtype):
    rets = list(re.finditer(r'\breturn\s*\(?([^;]*?)\)?\s*;', body))
    if not rets: return body, False
    m = rets[0] if rnd == 1 else rets[-1]
    expr = m.group(1).strip() or '0'
    c = '1' if rnd == 1 else '2'
    rt = rtype.replace('PNGAPI', '').strip()
    if rt == 'float':
        new = f'return (lib2a_mutq({k}) ? (({expr}) + {c}.0f) : ({expr}));'
    elif rt.endswith('p') or rt.endswith('pp') or '*' in rt:
        new = f'return (lib2a_mutq({k}) ? (({rt})((char*)({expr}) + {c})) : ({expr}));'
    else:
        new = f'return (({rt})(lib2a_mutq({k}) ? (({expr}) ^ {c}) : ({expr})));'
    return body[:m.start()] + new + body[m.end():], True
def mutate_void(body, k, rnd, has_info):
    first = body.find('{') + 1
    if rnd == 1:
        ins = f'\n   if (png_ptr != NULL && lib2a_mutq({k})) png_ptr->crc ^= 1;\n'
        return body[:first] + ins + body[first:], True
    tgt = 'info_ptr->width ^= 1' if has_info else 'png_ptr->flags ^= 0x100'
    guard = ('png_ptr != NULL && info_ptr != NULL' if has_info else 'png_ptr != NULL')
    ins = f'\n   if ({guard} && lib2a_mutq({k})) {tgt};\n'
    return body[:first] + ins + body[first:], True
def main():
    rnd = int(sys.argv[1]); dst = Path(sys.argv[2])
    if dst.exists(): shutil.rmtree(dst)
    shutil.copytree(SRC, dst, ignore=shutil.ignore_patterns('CMakeLists.txt'))
    names = [l.split()[0] for l in (HERE / 'tabla.txt').read_text().splitlines() if l.strip() and l.split()[4] != '0']
    files = {f.name: f.read_text(errors='replace') for f in dst.glob('png*.c')}
    done = {}
    for k, n in enumerate(names, 1):
        ret = P[n][0]; args = ' '.join(P[n][1])
        for fn, t in files.items():
            d = find_def(t, n)
            if not d: continue
            s, b, e = d
            body = t[b:e]
            if ret.strip() == 'void':
                nb, ok = mutate_void(body, k, rnd, 'info_ptr' in args)
            else:
                nb, ok = mutate_return(body, k, rnd, ret)
            if ok:
                files[fn] = t[:b] + nb + t[e:]; done[n] = fn
            break
    for fn, t in files.items():
        (dst / fn).write_text(HEAD + t)
    (dst / 'mutaciones.txt').write_text('
'.join(names) + '
')
    missing = [n for n in names if n not in done]
    print('mutadas', len(done), 'de', len(names), 'sin mutar:', missing)
main()
