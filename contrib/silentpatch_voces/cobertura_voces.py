# Para cada asignacion de contexto global -> especifico que anade SilentPatch (kGlobalRules), cuenta que voces del tipo tienen sonidos en ese contexto
# (leyendo las tablas del gta_sa.exe 1.0 US): sin voces con datos, la asignacion no tiene efecto audible en el juego base.
import re, struct, sys
sys.argv = ['x']
src = open('analiza_voces.py', encoding='utf-8').read().split("OUT = []")[0]
exec(src)
tables = {0: (0x8AF8A8, 209, 'B', 'GEN'), 1: (0x8BA4D0, 46, 'h', 'EMG'), 2: (0x8BBEF8, 20, 'h', 'PLAYER'), 3: (0x8BE620, 52, 'h', 'GANG'), 4: (0x8C42B0, 18, 'h', 'GFD')}
def voices_with(t, sctx):
    base, nv, fmt, _ = tables[t]
    esz = 2 if fmt == 'h' else 1
    out = []
    for v in range(nv):
        a, b = struct.unpack_from('<2' + fmt, rd(base + (sctx * nv + v) * 2 * esz, 2 * esz))
        if a not in (-1, 255) and (fmt == 'h' or a != 255):
            out.append((v, a, b))
    return out
rules = []
for t in 'LAS_COLINAS LOS_FLORES EAST_BEACH EAST_LS JEFFERSON GLEN_PARK IDLEWOOD GANTON LITTLE_MEXICO WILLOWFIELD PLAYA_DEL_SEVILLE TEMPLE'.split():
    rules.append(('L129', 'TAKE_TURF_' + t, 2, 'CONTEXT_PLY_TAKE_TURF_' + t))
for s in 'CONV_IGNORED GUN_COOL LIKE_DISMISS_REPLY PCONV_AGREE_BAD PCONV_AGREE_GOOD PCONV_ANS_NO PCONV_DISMISS PCONV_GREET_FEM PCONV_GREET_MALE PCONV_PART_FEM PCONV_PART_MALE PCONV_QUESTION PCONV_STATE_BAD PCONV_STATE_GOOD SAVED SHOCKED TRAPPED GUN_RUN'.split():
    rules.append(('L131', s, 4, 'CONTEXT_GFD_' + s))
rules.append(('SHOOT_GENERIC', 'SHOOT_GENERIC', 0, 'CONTEXT_GEN_SHOOT_GENERIC'))
rules.append(('SHOOT_GENERIC', 'SHOOT_GENERIC', 4, 'CONTEXT_GFD_SHOOT_GENERIC'))
rules.append(('L134', 'ABUSE_TRIAD', 3, 'CONTEXT_GNG_ABUSE_GANG_TRIADS'))
rules.append(('L134', 'ABUSE_RIFA', 3, 'CONTEXT_GNG_ABUSE_GANG_SFR'))
rules.append(('L134', 'ABUSE_DA_NANG', 3, 'CONTEXT_GNG_ABUSE_GANG_DNB'))
import csv
w = csv.writer(open(r'D:\Original\wv-spo\docs\CodigoVivoExe\silentpatch-clasificacion\voces-cobertura.csv', 'w', newline='', encoding='utf-8'))
w.writerow(['viñeta', 'contexto global', 'tipo de voz', 'contexto especifico', 'voces con sonidos (indice:primero-ultimo)'])
for l, g, t, s in rules:
    vs = voices_with(t, sp[s])
    w.writerow([l, g, tables[t][3], s, ' '.join('%d:%d-%d' % x for x in vs) if vs else '(ninguna: sin efecto audible en el juego base)'])
    print(l, g, tables[t][3], len(vs), 'voces')
