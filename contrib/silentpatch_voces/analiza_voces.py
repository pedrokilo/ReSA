# Compara las tablas de voz del EXE original (1.0 US) con lo que hace SilentPatch (MIT, Silent) y con nuestras enumeraciones.
# Uso: python analiza_voces.py [ruta_exe] [ruta_SpeechContextsSA.h] [ruta_PedSpeechContexts.h] [salida.csv]
import re, struct, sys, csv
exe = sys.argv[1] if len(sys.argv) > 1 else r'D:\Original\_comparar\orig\gta_sa.exe'
sph = sys.argv[2] if len(sys.argv) > 2 else r'D:\Original\ref_mods\silentpatch\SilentPatch-dev\SilentPatchSA\SpeechContextsSA.h'
ours = sys.argv[3] if len(sys.argv) > 3 else r'D:\Original\wv-spo\source\game_sa\Audio\Enums\PedSpeechContexts.h'
out = sys.argv[4] if len(sys.argv) > 4 else None
data = open(exe, 'rb').read()
pe = struct.unpack_from('<I', data, 0x3C)[0]
nsec = struct.unpack_from('<H', data, pe + 6)[0]
optsz = struct.unpack_from('<H', data, pe + 20)[0]
secs = []
for i in range(nsec):
    o = pe + 24 + optsz + 40 * i
    vs, va, rs, ro = struct.unpack_from('<IIII', data, o + 8)
    secs.append((va + 0x400000, max(vs, rs), ro, rs))


def rd(addr, n):
    for va, vs, ro, rs in secs:
        if va <= addr < va + vs:
            off = ro + addr - va
            return data[off:off + n]
    raise KeyError(hex(addr))


def parse_enums(path):
    txt = open(path, encoding='utf-8', errors='replace').read()
    res = {}
    for m in re.finditer(r'enum[^{;]*\{(.*?)\};', txt, re.S):
        v = -1
        for line in m.group(1).split('\n'):
            line = re.sub(r'//.*', '', line).strip().rstrip(',')
            if not line:
                continue
            mm = re.match(r'(\w+)\s*(?:=\s*([-\w]+))?', line)
            if not mm:
                continue
            if mm.group(2):
                try:
                    v = int(mm.group(2), 0)
                except ValueError:
                    v = res.get(mm.group(2), v + 1)
            else:
                v += 1
            res[mm.group(1)] = v
    return res


sp = parse_enums(sph)
ou = parse_enums(ours)


def ourname(n):
    return re.sub(r'^CONTEXT_', 'CTX_', n)


bad = []
for n, v in sp.items():
    if n.startswith('CONTEXT_') or n.startswith('VOICE_'):
        o = ourname(n)
        if o in ou and ou[o] != v:
            bad.append((n, v, o, ou[o]))
missing = [n for n in sp if (n.startswith('CONTEXT_') or n.startswith('VOICE_')) and ourname(n) not in ou]
print('enumeraciones SP:', len(sp), 'nuestras:', len(ou), 'valores distintos:', len(bad), 'sin nombre equivalente:', len(missing))
for b in bad[:40]:
    print('  DISTINTO', b)
print('  sin equivalente (primeros 40):', missing[:40])

OUT = []  # (grupo, descripcion, contexto, voz, antes, despues)
G = 0x8C6A68


def grow(i):
    return list(struct.unpack('<8h', rd(G + i * 16, 16)))


rows = []
i = 0
while True:
    r = grow(i)
    if r[0] == -1:
        break
    rows.append(r)
    i += 1
print('filas de la tabla global:', len(rows))


def setSpec(g, ptype, sctx, tag):
    for r in rows:
        if r[0] == sp[g]:
            if r[ptype + 1] == -1:
                OUT.append(('global', tag, g, 'tipo%d' % ptype, -1, sp[sctx]))
            else:
                OUT.append(('global', tag + ' (ya asignado, sin cambio)', g, 'tipo%d' % ptype, r[ptype + 1], r[ptype + 1]))
            return
    OUT.append(('global', tag + ' (SIN FILA)', g, 'tipo%d' % ptype, None, None))


PL, GFD, GEN, GANG = 2, 4, 0, 3
for t in 'LAS_COLINAS LOS_FLORES EAST_BEACH EAST_LS JEFFERSON GLEN_PARK IDLEWOOD GANTON LITTLE_MEXICO WILLOWFIELD PLAYA_DEL_SEVILLE TEMPLE'.split():
    setSpec('CONTEXT_GLOBAL_TAKE_TURF_' + t, PL, 'CONTEXT_PLY_TAKE_TURF_' + t, 'L129 CJ celebra turf')
for s in 'CONV_IGNORED GUN_COOL LIKE_DISMISS_REPLY PCONV_AGREE_BAD PCONV_AGREE_GOOD PCONV_ANS_NO PCONV_DISMISS PCONV_GREET_FEM PCONV_GREET_MALE PCONV_PART_FEM PCONV_PART_MALE PCONV_QUESTION PCONV_STATE_BAD PCONV_STATE_GOOD SAVED SHOCKED TRAPPED GUN_RUN'.split():
    setSpec('CONTEXT_GLOBAL_' + s, GFD, 'CONTEXT_GFD_' + s, 'L131 novias/tenderos')
setSpec('CONTEXT_GLOBAL_SHOOT_GENERIC', GEN, 'CONTEXT_GEN_SHOOT_GENERIC', 'SHOOT_GENERIC (sin uso, otros mods)')
setSpec('CONTEXT_GLOBAL_SHOOT_GENERIC', GFD, 'CONTEXT_GFD_SHOOT_GENERIC', 'SHOOT_GENERIC (sin uso, otros mods)')
setSpec('CONTEXT_GLOBAL_ABUSE_TRIAD', GANG, 'CONTEXT_GNG_ABUSE_GANG_TRIADS', 'L134 insultos de banda')
setSpec('CONTEXT_GLOBAL_ABUSE_RIFA', GANG, 'CONTEXT_GNG_ABUSE_GANG_SFR', 'L134 insultos de banda')
setSpec('CONTEXT_GLOBAL_ABUSE_DA_NANG', GANG, 'CONTEXT_GNG_ABUSE_GANG_DNB', 'L134 insultos de banda')

tables = {'GEN': (0x8AF8A8, 209, 'B'), 'EMG': (0x8BA4D0, 46, 'h'), 'PLY': (0x8BBEF8, 20, 'h'),
          'GNG': (0x8BE620, 52, 'h'), 'GFD': (0x8C42B0, 18, 'h')}


def mv(group, src, dst, tag, voices=None):
    base, nv, fmt = tables[group]
    NO = -1 if fmt == 'h' else 0xFF
    esz = 2 if fmt == 'h' else 1

    def get(c, v):
        return struct.unpack_from('<2' + fmt, rd(base + (c * nv + v) * 2 * esz, 2 * esz))
    for v in (voices if voices is not None else range(nv)):
        s = get(sp[src], v)
        d = get(sp[dst], v)
        if s[0] != NO and s[1] != NO and d[0] == NO and d[1] == NO:
            OUT.append((group, tag, src + '->' + dst, v, s, d))
        elif s[0] != NO and s[1] != NO:
            OUT.append((group, tag + ' (destino ocupado, sin cambio)', src + '->' + dst, v, s, d))


for a, b in [('CHA', 'CHAT'), ('RUN_FROM_FIGH', 'RUN_FROM_FIGHT'), ('SAVE', 'SAVED'), ('SHOCKE', 'SHOCKED')]:
    mv('GEN', 'CONTEXT_GEN_' + a, 'CONTEXT_GEN_' + b, 'L127 VBFYST2 (erratas de contexto)')
mv('GEN', 'CONTEXT_GEN_MUGGING', 'CONTEXT_GEN_MUGGED', 'L133 VWMOTR1', [sp['VOICE_GEN_VWMOTR1']])
mv('EMG', 'CONTEXT_EMG_ARREST_CRIMINAL', 'CONTEXT_EMG_ARREST_CRIM', 'L142 LAPDM1')
mv('GFD', 'CONTEXT_GFD_SHOOT_GENERIC', 'CONTEXT_GFD_SHOP_CLOSED', 'L132 tenderos asustados')
mv('GFD', 'CONTEXT_GFD_CAR_ON_FIRE', 'CONTEXT_GFD_CAR_FIRE', 'L131 coche en llamas')
mv('PLY', 'CONTEXT_PLY_JACKING_CAR_FEMALE', 'CONTEXT_PLY_JACKING_CAR_FEM', 'L135 robar coche a mujeres')
mv('GNG', 'CONTEXT_GNG_ATTACK_PLAYER_LIKE', 'CONTEXT_GNG_ATTACK_BY_PLAYER_LIKE', 'L141 FAM3')

nm = rd(0x8C65C0, 45 * 20)
vl = rd(0x8C6948, 45 * 6)
for i in range(45):
    voice, typ, fem = struct.unpack_from('<hhh', vl, i * 6)
    nme = nm[i * 20:i * 20 + 20].split(b'\0')[0].decode()
    if voice == -1 and typ == -1:
        # SilentPatch (PatchPedVoiceNameLookups) solo reutiliza la PRIMERA fila sin voz: la renombra "maccer" y le da VOICE_GNG_MACCER / PED_TYPE_GANG
        OUT.append(('SPECIAL', 'L127 Maccer (primera fila sin voz; se renombra a "maccer")', nme, i, (voice, typ), (sp['VOICE_GNG_MACCER'], GANG)))
        break
for r in OUT:
    print(r)
print('filas:', len(OUT), ' con cambio real:', sum(1 for r in OUT if 'sin cambio' not in r[1]))
if out:
    with open(out, 'w', newline='', encoding='utf-8') as f:
        w = csv.writer(f)
        w.writerow(['grupo', 'descripcion', 'contexto', 'voz', 'antes', 'despues'])
        w.writerows(OUT)
