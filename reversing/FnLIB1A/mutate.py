"""Prueba negativa del banco LIB1A: copia zlib 1.1.3 a un directorio de trabajo y le aplica UNA mutacion por funcion (14), condicionada por la variable
de entorno LIB1A_MUT (k = 1..14), de modo que cada funcion pueda "romperse" por separado en la misma compilacion. Las mutaciones solo se activan en
condiciones que el juego real no necesita (valores concretos de tamano, bits pendientes, avail_in...), como en las rondas negativas de Fn230A.

Uso:  python -I mutate.py <ronda 1|2> <directorio de destino>
Despues: build_bench.cmd <salida> <directorio de destino>   y   python -I neg_run.py ...
La fuente buena de libs/zlib-1.1.3 NUNCA se modifica."""
import re, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / 'libs' / 'zlib-1.1.3'

# (archivo, fragmento exacto, sustitucion). Cada fragmento debe aparecer EXACTAMENTE una vez.
G = 'lib1a_mut_get()'
ROUNDS = {
    1: [
        ('infblock.c', '  s->bitb = 0;\n  s->read = s->write = s->window;', f'  s->bitb = 0;\n  if ({G} == 1 && s->checkfn != Z_NULL) s->bitk = 1;\n  s->read = s->write = s->window;'),
        ('infblock.c', '  s->end = s->window + w;', f'  s->end = s->window + w - ({G} == 2 && w == 4096 ? 1 : 0);'),
        ('infblock.c', '      t = (uInt)b & 7;', f'      t = (uInt)b & 7;\n      if ({G} == 3 && (z->total_in & 255) == 17 && t == 3) t = 0;'),
        ('infblock.c', '  ZFREE(z, s->hufts);\n  ZFREE(z, s);\n  Tracev((stderr, "inflate:   blocks freed\\n"));', f'  if ({G} != 4 || (s->end - s->window) != 4096) ZFREE(z, s->hufts);\n  ZFREE(z, s);\n  Tracev((stderr, "inflate:   blocks freed\\n"));'),
        ('infblock.c', '  s->read = s->write = s->window + n;', f'  s->read = s->write = s->window + n;\n  if ({G} == 5 && n > 200 && (n & 7) == 0) s->write++;'),
        ('infcodes.c', '    c->ltree = tl;', f'    c->ltree = tl;\n    if ({G} == 6 && bl == 9 && bd == 5 && z->avail_in == 0) c->dbits = 4;'),
        ('infcodes.c', '  /* copy input/output information to locals (UPDATE macro restores) */\n  LOAD', f'  /* copy input/output information to locals (UPDATE macro restores) */\n  LOAD\n  if ({G} == 7 && n == 11) b ^= 1;'),
        ('infcodes.c', '  ZFREE(z, c);\n  Tracev((stderr, "inflate:       codes free\\n"));', f'  if ({G} != 8 || c->lbits != 9) ZFREE(z, c);\n  Tracev((stderr, "inflate:       codes free\\n"));'),
        ('inftrees.c', '  ZFREE(z, v);\n  return r;\n}\n\n\nint inflate_trees_dynamic', f'  ZFREE(z, v);\n  if ({G} == 9 && r == Z_OK && *bb == 5) *bb = 4;\n  return r;\n}}\n\n\nint inflate_trees_dynamic'),
        ('inftrees.c', '  return y != 0 && g != 1 ? Z_BUF_ERROR : Z_OK;', f'  return (y != 0 && g != 1) ? ({G} == 10 && n == 19 ? Z_DATA_ERROR : Z_BUF_ERROR) : Z_OK;'),
        ('inftrees.c', '  /* build distance tree */', f'  if ({G} == 11 && *bl == 7) *bl = 6;\n  /* build distance tree */'),
        ('inftrees.c', '  *td = fixed_td;', f'  *td = fixed_td;\n  if ({G} == 12 && z != Z_NULL) *bd = 4;'),
        ('infutil.c', '  if (n > z->avail_out) n = z->avail_out;\n  if (n && r == Z_BUF_ERROR) r = Z_OK;\n\n  /* update counters */\n  z->avail_out -= n;\n  z->total_out += n;\n\n  /* update check information */\n  if (s->checkfn != Z_NULL)\n    z->adler = s->check = (*s->checkfn)(s->check, q, n);\n\n  /* copy as far as end of window */',
         f'  if (n > z->avail_out) n = z->avail_out;\n  if ({G} == 13 && n == 100) n--;\n  if (n && r == Z_BUF_ERROR) r = Z_OK;\n\n  /* update counters */\n  z->avail_out -= n;\n  z->total_out += n;\n\n  /* update check information */\n  if (s->checkfn != Z_NULL)\n    z->adler = s->check = (*s->checkfn)(s->check, q, n);\n\n  /* copy as far as end of window */'),
        ('inffast.c', '  LOAD\n', f'  LOAD\n  if ({G} == 14 && n == 17) n--;\n'),
    ],
    2: [
        ('infblock.c', '  if (c != Z_NULL)\n    *c = s->check;', f'  if (c != Z_NULL)\n    *c = s->check;\n  if ({G} == 1 && c != Z_NULL && (s->end - s->window) == 32768) *c ^= 1;'),
        ('infblock.c', '  s->checkfn = c;\n  s->mode = TYPE;', f'  if ({G} == 2 && w == 1024) c = Z_NULL;\n  s->checkfn = c;\n  s->mode = TYPE;'),
        ('infblock.c', '      s->sub.left = (uInt)b & 0xffff;', f'      s->sub.left = (uInt)b & 0xffff;\n      if ({G} == 3 && s->sub.left > 100 && (z->total_in & 7) == 3) s->sub.left--;'),
        ('infblock.c', '  ZFREE(z, s->hufts);\n  ZFREE(z, s);\n  Tracev((stderr, "inflate:   blocks freed\\n"));', f'  ZFREE(z, s->hufts);\n  if ({G} != 4 || (s->end - s->window) != 8192) ZFREE(z, s);\n  Tracev((stderr, "inflate:   blocks freed\\n"));'),
        ('infblock.c', '  zmemcpy(s->window, d, n);', f'  zmemcpy(s->window, d, n - ({G} == 5 && n == 77 ? 1 : 0));'),
        ('infcodes.c', '    c->ltree = tl;', f'    c->ltree = tl + ({G} == 6 && bl == 9 && bd == 5 && z->avail_out > 400 ? 1 : 0);'),
        ('infcodes.c', '  /* copy input/output information to locals (UPDATE macro restores) */\n  LOAD', f'  /* copy input/output information to locals (UPDATE macro restores) */\n  LOAD\n  if ({G} == 7 && k == 3) k = 4;'),
        ('infcodes.c', '  ZFREE(z, c);\n  Tracev((stderr, "inflate:       codes free\\n"));', f'  ZFREE(z, ({G} == 8 && c->mode == START) ? (inflate_codes_statef *)0 : c);\n  Tracev((stderr, "inflate:       codes free\\n"));'),
        ('inftrees.c', '  ZFREE(z, v);\n  return r;\n}\n\n\nint inflate_trees_dynamic', f'  ZFREE(z, v);\n  if ({G} == 9 && r == Z_DATA_ERROR && *bb == 0) r = Z_BUF_ERROR;\n  return r;\n}}\n\n\nint inflate_trees_dynamic'),
        ('inftrees.c', '    *t = (inflate_huft *)Z_NULL;\n    *m = 0;', f'    *t = (inflate_huft *)Z_NULL;\n    *m = ({G} == 10 && n == 19) ? 1 : 0;'),
        ('inftrees.c', '  /* build distance tree */', f'  if ({G} == 11 && *bl == 5) *bl = 4;\n  /* build distance tree */'),
        ('inftrees.c', '  *bl = fixed_bl;', f'  *bl = fixed_bl - ({G} == 12 && z == Z_NULL ? 1 : 0);'),
        ('infutil.c', '  /* update pointers */\n  z->next_out = p;', f'  /* update pointers */\n  if ({G} == 13 && z->avail_out == 5) z->total_out++;\n  z->next_out = p;'),
        ('inffast.c', '  LOAD\n', f'  LOAD\n  if ({G} == 14 && k == 5) k++;\n'),
    ],
}

def main():
    rnd = int(sys.argv[1]); dst = Path(sys.argv[2])
    if dst.exists(): shutil.rmtree(dst)
    shutil.copytree(SRC, dst, ignore=shutil.ignore_patterns('CMakeLists.txt'))
    # selector de mutacion
    z = (dst / 'zutil.c').read_text()
    z += '\n#include <stdlib.h>\nint lib1a_mut_get(void) { static int v = -2; if (v == -2) { const char *e = getenv("LIB1A_MUT"); v = e ? atoi(e) : 0; } return v; }\n'
    (dst / 'zutil.c').write_text(z)
    h = (dst / 'zutil.h').read_text()
    h = h.replace('#endif /* _Z_UTIL_H */', 'int lib1a_mut_get(void);\n#endif /* _Z_UTIL_H */')
    assert 'lib1a_mut_get' in h
    (dst / 'zutil.h').write_text(h)
    for k, (f, old, new) in enumerate(ROUNDS[rnd], 1):
        t = (dst / f).read_text()
        n = t.count(old)
        if n != 1:
            raise SystemExit(f'mutacion {k} ({f}): el fragmento aparece {n} veces')
        (dst / f).write_text(t.replace(old, new))
    print(f'ronda {rnd}: 14 mutaciones aplicadas en {dst}')

if __name__ == '__main__':
    main()
