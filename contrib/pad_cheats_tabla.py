#!/usr/bin/env python3
"""Genera source/game_sa/PadCheatsTable.inc.h y docs/CodigoVivoExe/trucos-mando.md (tabla del mapeo) a partir de
la lista de secuencias de botones de PS2 de la documentacion publica de GInputSA 1.11 de Silent
(autorizacion solicitada por el usuario del proyecto; D:\\Original\\_ginput_ref\\cheats_ps.json, 87 entradas).
Uso: python contrib/pad_cheats_tabla.py [ruta cheats_ps.json]"""
import json, sys, os

JSON = sys.argv[1] if len(sys.argv) > 1 else r'D:\Original\_ginput_ref\cheats_ps.json'
RAIZ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# indice del json -> nombre eCheats (sin el prefijo CHEAT_)
M = {
 0:'WEAPON_SET1',1:'WEAPON_SET2',2:'WEAPON_SET3',3:'HEALTH_ARMOR_250K',4:'INFINITE_HEALTH',5:'INFINITE_OXYGEN',6:'INFINITE_AMMO',7:'SUICIDE',
 8:'ADRENALINE_MODE',9:'NEVER_GET_HUNGRY',10:'I_DO_AS_I_PLEASE',11:'SIX_WANTED_STARS',12:'WANTED_LEVEL_2STARS',13:'CLEAR_WANTED_LEVEL',
 14:'WANNA_BE_IN_MY_GANG',15:'ROCKET_MAYHEM',16:'HITMAN_LEVEL_FOR_ALL_WEAPONS',17:'MAX_DRIVING_SKILLS',18:'MAX_RESPECT',19:'MAX_SEX_APPEAL',
 20:'MAX_STAMINA',21:'FAT_PLAYER',22:'SKINNY_PLAYER',23:'MAX_MUSCLE',24:'GANGS_CONTROLS_THE_STREETS',25:'GANGMEMBERS_EVERYWHERE',26:'SPAWN_DOZER',
 27:'GET_JETPACK',28:'GET_PARACHUTE',29:'NINJA_THEME',30:'SPAWN_RHINO',31:'SPAWN_RANCHER',32:'SPAWN_CADDY',33:'SPAWN_TANKER_TRUCK',
 34:'SPAWN_HOTRINGA',35:'SPAWN_HOTRINGB',36:'SPAWN_BLOODRINGBANGER',37:'SPAWN_STRETCH',38:'SPAWN_ROMERO',39:'SPAWN_TRASHMASTER',40:'SPAWN_QUAD',
 41:'SPAWN_HYDRA',42:'SPAWN_HUNTER',43:'SPAWN_VORTEX',44:'SPAWN_MONSTER',45:'SPAWN_STUNT_PLANE',46:'CARS_FLOAT_AWAY_WHEN_HIT',47:'COUNTRY_TRAFFIC',
 48:'INVISIBLE_CAR',49:'PERFECT_HANDLING',50:'FAST_TRAFFIC',51:'AGGRESSIVE_DRIVERS',52:'REDUCED_TRAFFIC',53:'CARS_ON_WATER',54:'ALL_CARS_HAVE_NITRO',
 55:'BLOW_UP_ALL_CARS',56:'FAST_TRAFFIC',57:'BLACK_TRAFFIC',58:'PINK_TRAFFIC',59:'BGREEN_LIGHTS_CHEAT',60:'CARS_FLY',61:'BOATS_FLY',62:'SMASH_N_BOOM',
 63:'HUGE_BUNNY_HOP',64:'NINJA_THEME',65:'FUNHOUSE_THEME',66:'ELVIS_IS_EVERYWHERE',67:'SLUT_MAGNET',68:'WEAPON_AIMING_WHILE_DRIVING',69:'HAVE_ABOUNTY_ON_YOUR_HEAD',
 70:'PEDS_ATTACK_YOU_WITH_ROCKETS',71:'RIOT_MODE',72:'EVERYONE_ARMED',73:'FASTER_GAMEPLAY',74:'SLOWER_GAMEPLAY',76:'MEGA_PUNCH',
 77:'BEACH_PARTY',78:'STOP_GAME_CLOCK_ORANGE_SKY',79:'FASTER_CLOCK',80:'SUNNY_WEATHER',81:'VERY_SUNNY_WEATHER',82:'OVERCAST_WEATHER',83:'FOGGY_WEATHER',
 84:'RAINY_WEATHER',85:'SAND_STORM',86:'ALWAYS_MIDNIGHT',
}
# estado por indice: alias (misma secuencia y mismo truco que otra entrada: no se registra) | dup (duplicada dudosa: no se registra ni activa nada)
ESTADO = {29: 'alias', 50: 'dup'}
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

if __name__ == '__main__':
    L = ['// GENERADO por contrib/pad_cheats_tabla.py a partir de la lista de secuencias de PS2 de la documentacion de GInputSA 1.11 de Silent',
         '// (autorizacion solicitada por el usuario del proyecto). No editar a mano. Columnas: descripcion, id eCheats (TOTAL_CHEATS = ninguno), modo, longitud, botones.',
         '// modo: 0 = se registra; 1 = alias de otra entrada (misma secuencia, mismo truco: no se registra); 2 = duplicada dudosa (no se registra, no activa nada).']
    for i, (n, s) in enumerate(D):
        st = ESTADO.get(i, 'ok')
        modo = {'ok': 0, 'alias': 1, 'dup': 2}[st]
        idc = 'TOTAL_CHEATS' if st == 'dup' else 'CHEAT_' + M[i]
        bts = ', '.join('Btn::' + CL[b] for b in s)
        n2 = n.replace('<br>', ' ').replace('"', "'")
        L.append(f'{{ "{n2}", {idc}, {modo}, {len(s)}, {{ {bts} }} }}, // {i}')
    open(os.path.join(RAIZ, 'source/game_sa/PadCheatsTable.inc.h'), 'w', encoding='utf-8').write('\n'.join(L) + '\n')
    print(len(D), 'entradas escritas')
    # --- tabla del mapeo para el documento (se inserta entre marcadores en docs/CodigoVivoExe/trucos-mando.md)
    R = ['| # | Descripción PS2 (lista de GInputSA) | Secuencia | Id `eCheats` | Cadena PC (teclado) | Mapeo |', '| --- | --- | --- | --- | --- | --- |']
    usados = set()
    for i, (n, sq) in enumerate(D):
        st = ESTADO.get(i, 'ok')
        sec = ' '.join(FL[b] for b in sq)
        cid = M[i]
        if st == 'dup':
            R.append(f'| {i+1} | {n} | {sec} | (ninguno) | (ninguna) | **duplicada dudosa**: repite la secuencia de «Traffic Lights Green» (#60); no se registra ni activa nada |')
            continue
        num = ID[cid]
        usados.add(num)
        pc = PC.get(num, '—')
        nota = {'alias': 'alias de «Ninja Theme» (#65): misma secuencia y mismo truco; no se registra dos veces'}.get(st, '')
        if i == 84: nota = 'dudoso: la lista la llama «Stormy»; por su posición (últimos botones △ ↓ □ ○ ✕ en los ids 6..10) es el clima **Rainy** (id 9); el id 57 (thunderstorm) queda sin secuencia'
        if i == 59 - 1 + 1 and False: nota = ''
        R.append(f'| {i+1} | {n.replace("<br>", " ")} | {sec} | {num} `{cid}` | {pc} | {nota or "ok"} |')
    sin = [f'`{k}` ({v})' for k, v in sorted(ID.items(), key=lambda kv: kv[1]) if v not in usados]
    open(os.path.join(RAIZ, 'docs/CodigoVivoExe/trucos-mando-tabla.md'), 'w', encoding='utf-8').write('\n'.join(R) + '\n\nIds de `eCheats` (0..91) sin secuencia en la lista de PS2: ' + ', '.join(sin) + '.\n')
