"""Genera mapa_funciones.csv: correspondencia funcion del EXE <-> funcion de la fuente (zlib 1.1.3 + libpng 1.0.12 compiladas con MSVC 19.5 /O2).
Se ejecuta con el cwd = reversing/FnLIB1A, tras compilar_objs.cmd (los .obj/.asm van a un directorio de trabajo): python mapa_final.py <dir_objs>"""
import sys, csv, collections, math, io, contextlib
OBJ = sys.argv[1]
sys.argv = ['x', OBJ]
buf = io.StringIO()
with contextlib.redirect_stdout(buf):
    exec(open('mapa3.py', encoding='utf-8').read())
closure = {int(r['funcion'], 16) for r in csv.DictReader(open('datos/cierre_libpng_zlib.csv'))}
ZL = {0x7E9430: ('infblock', '_inflate_blocks_reset'), 0x7E94B0: ('infblock', '_inflate_blocks_new'), 0x7E9570: ('infblock', '_inflate_blocks'), 0x7EA1C0: ('infblock', '_inflate_blocks_free'),
      0x7EA250: ('infblock', '_inflate_set_dictionary'), 0x7EA2A0: ('infcodes', '_inflate_codes_new'), 0x7EA2E0: ('infcodes', '_inflate_codes'), 0x7EA9E0: ('infcodes', '_inflate_codes_free'),
      0x7EAA00: ('inftrees', '_inflate_trees_bits'), 0x7EAAB0: ('inftrees', '_huft_build'), 0x7EAF60: ('inftrees', '_inflate_trees_dynamic'), 0x7EB0F0: ('inftrees', '_inflate_trees_fixed'),
      0x7EB120: ('infutil', '_inflate_flush'), 0x7EB260: ('inffast', '_inflate_fast')}
final = {}
used = {}
def put(e, s, conf, how):
    if e in final: return
    if s in used: return
    final[e] = (s, conf, how); used[s] = e
for e, s in ZL.items(): put(e, s, 'alto', 'reconstruccion del proyecto (ZlibInflate) + cadenas + grafo')
put(0x7E9300, ('adler32', '_adler32'), 'alto', 'constantes BASE=65521 (0xFFF1) y NMAX=5552 (0x15B0) en el cuerpo; sin llamadas; a continuacion de inflate')
for e in sorted(match):
    s, how = match[e]
    if exe[e]['size'] > 8000: continue
    r = exe[e]['size'] / max(src[s]['size'], 1)
    if how == 'cadenas-exactas': put(e, s, 'alto', 'cadenas unicas en ambos lados (mensajes de error/nombres de chunk)')
for e in sorted(match):
    s, how = match[e]
    if exe[e]['size'] > 8000 or e in final: continue
    if how == 'cadenas-parciales': put(e, s, 'alto' if exe[e]['strset'] <= src[s]['strset'] and exe[e]['strset'] else 'medio', 'cadenas compartidas (puntuacion ponderada, mejor mutua)')
    else: put(e, s, 'medio', 'grafo de llamadas con orden de funciones del fichero')
for e in sorted(dpm):
    s, x = dpm[e]
    if e in final or exe[e]['size'] > 8000: continue
    r = exe[e]['size'] / max(src[s]['size'], 1)
    if not (0.3 <= r <= 3.2): continue
    put(e, s, 'medio' if x >= 0.85 else 'bajo', 'alineamiento por orden entre anclas (tamano, llamadas, x87, cadenas)')
# --- propagacion por el grafo de llamadas desde lo ya emparejado (callees comunes / callers comunes), hasta que no cambie
def ok_ratio(e, s):
    r = exe[e]['size'] / max(src[s]['size'], 1); return exe[e]['size'] <= 8000 and 0.3 <= r <= 3.2
changed = True
scallers = collections.defaultdict(set)
for s_, f_ in src.items():
    for c_ in f_['int_calls']: scallers[c_].add(s_)
ecallers = collections.defaultdict(set)
for e_, r_ in exe.items():
    for c_ in r_['int_calls']: ecallers[c_].add(e_)
while changed:
    changed = False
    for e in sorted(exe):
        if e in final: continue
        # por callees ya emparejados
        cs = [final[c][0] for c in exe[e]['int_calls'] if c in final]
        if len(cs) >= 1:
            cands = set.intersection(*[scallers[x] for x in cs]) if cs else set()
            cands = [x for x in cands if x not in used and ok_ratio(e, x)]
            if len(cands) == 1 and (len(cs) >= 2 or len(exe[e]['int_calls']) == 1):
                put(e, cands[0], 'medio', 'grafo: unico candidato que llama a los mismos callees ya emparejados'); changed = True; continue
        # por callers ya emparejados
        ps = [final[c][0] for c in ecallers[e] if c in final]
        if ps:
            cands = set.intersection(*[set(src[x]['int_calls']) for x in ps])
            cands = [x for x in cands if x not in used and ok_ratio(e, x)]
            if len(cands) == 1 and (len(ps) >= 2 or len(src[ps[0]]['int_calls']) <= 2):
                put(e, cands[0], 'bajo', 'grafo: unico candidato llamado por los mismos callers ya emparejados'); changed = True
# consistencia del grafo: callees del EXE que se emparejan a callees de la fuente
agree = {}
for e, (s, conf, how) in final.items():
    ec = [c for c in exe[e]['int_calls'] if c in final]
    if not ec: continue
    ok_ = sum(1 for c in ec if final[c][0] in src[s]['int_calls'])
    agree[e] = (ok_, len(ec))
for e, (s, conf, how) in list(final.items()):
    if e in agree and conf in ('medio', 'bajo'):
        ok_, n_ = agree[e]
        if n_ >= 2 and ok_ == n_ and conf == 'medio': final[e] = (s, 'alto', how + '; callees coinciden %d/%d' % (ok_, n_))
        elif n_ >= 2 and ok_ * 2 < n_: final[e] = (s, 'bajo', how + '; callees NO coinciden (%d/%d)' % (ok_, n_))
# conflictos de dp con el mismo origen: ya resueltos por `used`
rows = []
for e in sorted(exe):
    s, conf, how = final.get(e, (None, 'sin-equivalente', ''))
    r = exe[e]
    rtpng = e in (0x7CF600, 0x7CF9B0)
    diff = ''
    if s:
        sz = src[s]['size']; ratio = r['size'] / max(sz, 1)
        notes = []
        if ratio > 1.35: notes.append('EXE %.2fx mayor (otro compilador/opciones; chunk helpers en linea)' % ratio)
        elif ratio < 0.75: notes.append('EXE %.2fx menor (inlining distinto)' % ratio)
        else: notes.append('tamano similar (%.2fx)' % ratio)
        if (r['x87'] > 0) != (src[s]['x87'] > 0): notes.append('uso de x87 distinto')
        diff = '; '.join(notes)
    if rtpng: s, conf, how, diff = None, 'sin-equivalente', 'rtpng de RenderWare (Criterion), no es libpng', 'envoltura propia del SDK: llama a png_*; hay que reconstruirla en C++ (rtpng.cpp)'
    rows.append(dict(va_exe='0x%08X' % e, tam_exe=r['size'], nombre_fuente=(s[1].lstrip('_') if s else ''), archivo_fuente=(s[0] + '.c' if s else ''), tam_fuente=(src[s]['size'] if s else ''),
                     confianza=('alto' if conf == 'alto' else conf), metodo=how, en_cierre176=int(e in closure), conocida_por_ghidra=r['known'], diferencias=diff))
with open('mapa_funciones.csv', 'w', newline='', encoding='utf-8') as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0])); w.writeheader(); w.writerows(rows)
c = collections.Counter(r['confianza'] for r in rows); cc = collections.Counter(r['confianza'] for r in rows if r['en_cierre176'])
print('total', len(rows), dict(c)); print('en el cierre de 176:', sum(cc.values()), dict(cc))
unm = sorted(e for e in closure if '0x%08X' % e not in {r['va_exe'] for r in rows if r['confianza'] != 'sin-equivalente'})
print('cierre sin equivalente:', len(unm))
