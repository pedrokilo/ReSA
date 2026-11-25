#!/usr/bin/env python3
"""Genera source/game_sa/PadCheatsTable.inc.h y docs/CodigoVivoExe/trucos-mando.md (tabla del mapeo) a partir de
la lista de secuencias de botones de PS2 de la documentacion publica de GInputSA 1.11 de Silent
(autorizacion solicitada por el usuario del proyecto; D:\\Original\\_ginput_ref\\cheats_ps.json, 87 entradas).
Uso: python contrib/pad_cheats_tabla.py [ruta cheats_ps.json]"""
import json, sys, os

JSON = sys.argv[1] if len(sys.argv) > 1 else r'D:\Original\_ginput_ref\cheats_ps.json'
RAIZ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Tabla de 90 hashes (CKeyGen, cadena de letras en orden inverso) de GInputSA.asi, indice = id de eCheats. Letras: T triangulo, C circulo, X cruz, S cuadrado, U/D/L/R cruceta,
# 1 L1, 2 L2, 3 R1, 4 R2. Es el mismo esquema que el teclado de PC (hash de la cadena tecleada al reves).
HASHES = [0x518bad1e,0x867974fb,0x80750205,0x207be5a5,0x97f19ea4,0x4dcf6ee5,0xd8f6171b,0xa44d2630,0xd2331e02,0xae882f29,0x8f590254,0x6a171ac9,0xb4ede50d,0xcbc466a6,0x7f9f9be0,0xf0031628,0x2a849202,0x9454290d,0xe6a1eeee,0xe5bb7de4,0xb1865c87,0xd9606cb7,0xb533f81f,0xe3eca2dd,0xf8027c81,0x1b184f2b,0x09656c54,0x62b594b1,0xc9b3a8fb,0x53ee1352,0xbac116e1,0x8fa0b3bf,0x9c57938a,0x7620f788,0xfd27f498,0x56e4bb70,0x9a5f52b5,0x49ba4900,0x208b6a19,0x8644854e,0x51e41320,0xe89bf194,0xc3cc20cf,0x9bc98fb7,0x21953b77,0x9180b0bd,0x12d165a9,0x9c2e6d4e,0x7eb898f4,0x825a0d49,0x103fb71c,0x9a6cfbb3,0x2a379a4d,0xa6f8d057,0x688f9533,0x7640e454,0xc3359459,0xe1d4d60d,0xd354c163,0x4e6e459b,0xed0309c6,0xf0c6272d,0xc31ec24f,0xf3efa85f,0xb40e31e5,0x331804cd,0x44a95f0c,0xc58051f4,0xf5ffec07,0xcb32f7ae,0xf9bc0421,0xf9205c67,0xf24c377c,0x646273b4,0x20df2c9a,0xcde25320,0x7f816cfc,0xa3cfe345,0x83018331,0xf35b86e2,0x532ec19e,0x77c91c9b,0xe2512b4e,0xbc4e2d7a,0x62f26615,0xf96d57d3,0xd5791ce1,0xeb433433,0xfa760386,0xace1a711][:90]
LETRA = {'l1':'1','l2':'2','r1':'3','r2':'4','ud':'U','dd':'D','ld':'L','rd':'R','cr':'X','ci':'C','sq':'S','tr':'T'}
def crc(txt):
    c = 0xFFFFFFFF
    for ch in txt.encode():
        c ^= ch
        for _ in range(8): c = (c >> 1) ^ (0xEDB88320 if c & 1 else 0)
    return c
# Correcciones de la lista HTML detectadas con la tabla del asi (el hash del asi solo coincide con la secuencia corregida): el HTML tiene botones de mas
CORR = {'Jetpack': ['ld','rd','l1','l2','r1','r2','ud','dd','ld','rd'], 'Gang Members Everywhere': ['ld','rd','rd','rd','ld','cr','dd','ud','sq','rd']}
PC = {51:'KGGGDKP', 54:'BSXSGGC', 56:'OFVIAC', 57:'MGHXYRM', 60:'LFGMHAL', 66:'BRINGITON', 70:'PRIEBJ', 71:'MUNASEF', 73:'OUIQDMW', 75:'FVTMNBZ', 79:'OGXSDAG', 80:'EHIBXQS', 82:'PROFESSIONALKILLER', 0:'LXGIWYL',1:'KJKSZPJ',2:'UZUMYMW',3:'HESOYAM',4:'OSRBLHH',5:'ASNAEB',6:'AFZLLQLL',7:'ICIKPYH',8:'ALNSFMZO',10:'CFVFGMJ',11:'YSOHNUL',12:'SPEEDITUP',
 13:'SLOWITDOWN',14:'AJLOJYQY',15:'BAGOWPG',16:'FOOOXFT',17:'AIWPRTON',18:'OLDSPEEDDEMON',19:'JQNTDMH',20:'PDNEJOH',21:'VPJTQWV',22:'AQTBCODX',23:'KRIJEBR',
 24:'TRUEGRIME',25:'RZHSUEW',26:'CPKTNWT',27:'XICWMD',28:'PGGOMOY',29:'SZCMAWO',30:'ZEIIVG',31:'YLTEICZ',32:'LLQPFBN',33:'IOWDLAC',35:'AFSNMSMW',36:'BTCDBCB',
 37:'JYSDSOD',38:'LEANANDMEAN',39:'BLUESUEDESHOES',40:'BGLUAWML',41:'LIFESABEACH',42:'ONLYHOMIESALLOWED',43:'BIFBUZZ',44:'NINJATOWN',45:'LOVECONQUERSALL',
 46:'EVERYONEISPOOR',47:'EVERYONEISRICH',48:'RIPAZHA',49:'JHJOECW',50:'JUMPJET',52:'JCNRUAD',53:'SPEEDFREAK',55:'NIGHTPROWLER',58:'CWJXUOC',
 61:'BAGUVIX',62:'CVWKXAM',63:'AIYPWZQP',64:'ROCKETMAN',65:'AEZAKMI',67:'STINGLIKEABEE',69:'STATEOFEMERGENCY',72:'FULLCLIP',74:'GHOSTTOWN',76:'WANNABEINMYGANG',
 77:'NOONECANSTOPUS',78:'ROCKETMAYHEM',81:'VKYPQCF',83:'NATURALTALENT',84:'OHDUDE',85:'FOURWHEELFUN',86:'AMOMHRER',87:'ITSALLBULL',88:'FLYINGTOSTUNT',89:'MONSTERMASH'}


def leer_enum():
    ids = {}; n = 0
    txt = open(os.path.join(RAIZ, 'source/game_sa/Enums/eCheats.h'), encoding='utf-8').read()
    for ln in txt.splitlines():
        ln = ln.strip().rstrip(',')
        if ln.startswith('CHEAT_'):
            nombre = ln.split('=')[0].strip()
            if '=' in ln: n = int(ln.split('=')[1])
            ids[nombre[6:]] = n; n += 1
        if ln.startswith('TOTAL_CHEATS'): break
    return ids

ID = leer_enum()
D = json.load(open(JSON))
CL = {'l1':'L1','l2':'L2','r1':'R1','r2':'R2','ud':'Up','dd':'Down','ld':'Left','rd':'Right','cr':'Cross','ci':'Circle','sq':'Square','tr':'Triangle','l3':'L3','r3':'R3'}
FL = {'l1':'L1','l2':'L2','r1':'R1','r2':'R2','ud':'↑','dd':'↓','ld':'←','rd':'→','cr':'✕','ci':'○','sq':'□','tr':'△','l3':'L3','r3':'R3'}

def cargar():
    """devuelve [(descripcion, secuencia(lista de codigos), id eCheats, modo, nota)] a partir del json y la tabla de hashes del asi"""
    out = []; vistos = {}
    for n, sq in D:
        sq2 = CORR.get(n, sq)
        h = crc(''.join(LETRA[x] for x in sq2)[::-1])
        if h not in HASHES: raise SystemExit('sin hash en el asi: ' + n)
        i = HASHES.index(h)
        modo = 1 if i in vistos else 0
        vistos.setdefault(i, n)
        nota = ''
        if n in CORR: nota = 'el HTML trae %d botones: el hash del asi solo coincide con esta secuencia de %d' % (len(sq), len(sq2))
        out.append([n, sq2, i, modo, nota])
    return out
NOMBRE_ID = {v: k for k, v in ID.items()}
if __name__ == '__main__':
    E = cargar()
    NL = chr(10)
    L = ['// GENERADO por contrib/pad_cheats_tabla.py. Atribucion: lista de secuencias de PS2 de la documentacion de GInputSA 1.11 de Silent y tabla de 90 hashes de GInputSA.asi',
         '// (autorizacion del usuario del proyecto). No editar a mano.',
         '// kHashes: hash (CKeyGen sobre la cadena de letras al reves) de cada truco, indice = id de eCheats (90 entradas; los ids 90 y 91 no tienen).',
         '// kEntries: las 87 secuencias de la lista (documentadas y comprobadas contra kHashes): descripcion, id eCheats, modo (0 normal, 1 alias: misma secuencia que una anterior), longitud, botones.']
    L.append('#ifdef PADCHEATS_HASHES')
    L.append('constexpr uint32 kHashes[90] = {')
    for k in range(0, 90, 6): L.append('    ' + ', '.join('0x%08x' % x for x in HASHES[k:k+6]) + ',')
    L.append('};')
    L.append('#else')
    BT = {'l1':'L1','l2':'L2','r1':'R1','r2':'R2','ud':'Up','dd':'Down','ld':'Left','rd':'Right','cr':'Cross','ci':'Circle','sq':'Square','tr':'Triangle'}
    for n, sq, i, modo, nota in E:
        L.append('{ "%s", CHEAT_%s, %d, %d, { %s } },' % (n.replace('<br>', ' ').replace('"', "'"), NOMBRE_ID[i], modo, len(sq), ', '.join('Btn::' + BT[b] for b in sq)))
    L.append('#endif')
    open(os.path.join(RAIZ, 'source/game_sa/PadCheatsTable.inc.h'), 'w', encoding='utf-8').write(NL.join(L) + NL)
    print(len(E), 'entradas;', 'ids con hash pero sin secuencia en la lista:', [i for i in range(90) if i not in {e[2] for e in E}])
    R = ['| # | Descripción PS2 (lista de GInputSA) | Secuencia | Id `eCheats` | Cadena PC (teclado) | Observaciones |', '| --- | --- | --- | --- | --- | --- |']
    for k, (n, sq, i, modo, nota) in enumerate(E):
        sec = ' '.join(FL[b] for b in sq)
        obs = nota
        if modo == 1: obs = ('misma secuencia (y mismo hash) que «%s»: no es otro truco' % [e[0] for e in E if e[2] == i and e[3] == 0][0]) + ('; ' + obs if obs else '')
        R.append(f'| {k+1} | {n.replace("<br>", " ")} | {sec} | {i} `{NOMBRE_ID[i]}` | {PC.get(i, "—")} | {obs or "ok"} |')
    sin = [f'`{NOMBRE_ID[i]}` ({i})' for i in range(90) if i not in {e[2] for e in E}]
    R.append('')
    R.append('Ids de `eCheats` con hash en la tabla del asi pero **sin secuencia en la lista HTML** (el asi los activa; su secuencia no se pudo recuperar, ver el texto): ' + ', '.join(sin) + '. Ids 90 (`PROSTITUTES_PAY_YOU`) y 91 (`ALL_TAXIS_NITRO`): sin entrada en el asi.')
    open(os.path.join(RAIZ, 'docs/CodigoVivoExe/trucos-mando-tabla.md'), 'w', encoding='utf-8').write(NL.join(R) + NL)
