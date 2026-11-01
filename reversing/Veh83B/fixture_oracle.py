"""Oraculo independiente (Python) de Veh83B: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6D7C90..0x6D82BA, lo que debe observarse de
CVehicle::InflictDamage (traza de colaboradores y virtuales EN ORDEN con todos sus argumentos, campos escritos en el vehiculo, jugador del mundo y semilla de rand()) y lo compara con las filas de
comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado guardado como float (fstp, con desnormales).
La unica carga de NaN que aparece es el indefinido 0xFFC00000 (las fixtures usan un solo patron de NaN).

Flujo leido (thiscall, 6 dwords: atacante, arma, intensidad, x, y, z; ret 0x18; void):
  CanVehicleBeDamaged(atacante, arma, &fuego) (0x6D1280); AL == 0 -> sale.  Estado 0 (jugador): GetPercentageProgress >= 100.0 -> intensidad *= 0.5.
  Intensidad > 10: si atacante == FindPlayerPed(-1) o == FindPlayerVehicle(-1, false) y el estado no es 5: Players[foco].caos += 2, +1.0f al valor de persecucion, IncrementStat(0x7D, 5 + rand() % 20).
  Atacante peaton y vehiculo automovil o moto: probabilidad por arma (tabla 0x6D82D0), IsPlayer -> 0, atacante en moto -> como mucho 1, moto de policia 0x20B con conductor policia -> sale; sin probabilidad o con ruedas
  indestructibles sale; (rand() & 0x7F) < probabilidad: moto (FindTyreNearestPoint, BurstTyre(rueda + 13, 0)) o coche con GetVehicleAppearance == 1 (BurstTyre(rueda + 13, 1)).
  Deposito (+0x42E bit 6, fuego, atacante peaton y IsPlayer): dummy 8 de la estructura del modelo != 0 -> punto = matriz * dummy (0x59C890); |coord - punto| < 0.25 -> intensidad = min(vida, 1100.0).
  Subtipo 3 o 4 sin RC y arma distinta de 0x33, 0x10, 0x13 y 0x14: intensidad *= 0.4.  Vida > 0: arma y entidad de dano (+RegisterReference); si vida > intensidad: vida -= intensidad, eventos
  CEventVehicleDamageWeapon para conductor y pasajeros (estado 0, 2 o 3 y atacante peaton) y, al cruzar 250 en un automovil, SetEngineStatus(225); si no: vida = 0, ReportCrime si el atacante es el jugador,
  explosion (temporizador +0x4DE y +0x4E0) o BlowUpCar(atacante, 0).  Al final, si es de la ley y el atacante es el jugador: SetWantedLevelNoDrop(1).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un puntero fuera de las arenas (pila), 0xFFFF0001.
"""
import json
import sys
from fractions import Fraction

GUARD = 0x40
M32 = 0xFFFFFFFF
NAN = ('n',)
QNAN = 0xFFC00000
ONE, HALF = 0x3F800000, 0x3F000000
STACK = 0xFFFF0001
MAT_OFF = [0x00, 0x04, 0x08, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28, 0x30, 0x34, 0x38]


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


# ---------------------------------------------------------------- aritmetica x87 a 24 bits
def load(bits):
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    neg = bits >> 31
    if e == 0xFF:
        return NAN if m else ('i', neg)
    if e == 0:
        v = Fraction(m, 1) * Fraction(1, 1 << 149)
    else:
        v = Fraction(m | 0x800000, 1) * Fraction(2) ** (e - 150)
    return ('f', -v if neg else v, neg)


def rnd(v, quantum_exp_min=None):
    """redondea al par a 24 bits (exponente ilimitado), o con cuanto minimo 2**quantum_exp_min (desnormales del float)"""
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    q = e - 23
    if quantum_exp_min is not None and q < quantum_exp_min:
        q = quantum_exp_min
    quantum = Fraction(2) ** q
    k = a / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    return -r if v < 0 else r


def fin(v, neg):
    return ('f', rnd(v), neg if v == 0 else (1 if v < 0 else 0))


def mul(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' or b[0] == 'i':
        za = a[0] == 'f' and a[1] == 0
        zb = b[0] == 'f' and b[1] == 0
        if za or zb:
            return NAN
        sa = a[1] if a[0] == 'i' else a[2]
        sb = b[1] if b[0] == 'i' else b[2]
        return ('i', sa ^ sb)
    return fin(a[1] * b[1], a[2] ^ b[2])


def add(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else NAN
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('f', Fraction(0), 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0)
    return fin(s, 0)


def neg(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', a[1] ^ 1)
    return ('f', -a[1], a[2] ^ 1)


def sub(a, b):
    return add(a, neg(b))


def div(a, b):
    """a / b"""
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return NAN
    if a[0] == 'i':
        return ('i', a[1] ^ (b[1] if b[0] == 'i' else b[2]))
    if b[0] == 'i':
        return ('f', Fraction(0), a[2] ^ b[1])
    if b[1] == 0:
        if a[1] == 0:
            return NAN
        return ('i', a[2] ^ b[2])
    return fin(a[1] / b[1], a[2] ^ b[2])


def greater(a, b):
    """a > b ordenado (fcomp; test ah, 41h): falso si alguno es NaN"""
    if a == NAN or b == NAN:
        return False

    def key(x):
        if x[0] == 'i':
            return Fraction(10) ** 400 * (-1 if x[1] else 1)
        return x[1]
    return key(a) > key(b)


def store(a):
    """fstp dword: devuelve los bits del float (NaN -> el indefinido)"""
    if a == NAN:
        return QNAN
    if a[0] == 'i':
        return 0x7F800000 | (a[1] << 31)
    v = a[1]
    if v == 0:
        return a[2] << 31
    mag = abs(v)
    r = rnd(mag, -149)
    sign = 1 if v < 0 else 0
    if r >= Fraction(2) ** 128:
        return 0x7F800000 | (sign << 31)
    if r < Fraction(2) ** -126:
        return (sign << 31) | int(r / Fraction(2) ** -149)
    e = r.numerator.bit_length() - r.denominator.bit_length()
    if Fraction(2) ** e > r:
        e -= 1
    elif Fraction(2) ** (e + 1) <= r:
        e += 1
    m = int(r / Fraction(2) ** (e - 23)) & 0x7FFFFF
    return (sign << 31) | ((e + 127) << 23) | m


L = load
LAST = {}   # intermedios del ultimo calculo (cobertura de run_local.py)




# ---------------------------------------------------------------- el modelo de la funcion
def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


FLOAT = lambda bits: L(bits)
TEN, HUNDRED, QUARTER_LIMIT = 0x41200000, 0x42C80000, None
TWO50, ELEVEN00, ZERO, FOUR_TENTHS = 0x437A0000, 0x44898000, 0x00000000, 0x3ECCCCCD
MODELS = [0x20B, 400, 401, 402, 500, 0x1A0, 600, 0]
# probabilidad de reventar una rueda por arma - 22 (tabla 0x6D82D0 + saltos 0x6D82C0)
SLOT_TYPE = [0, 0, 1, 0, 3, 3, 0, 0, 2, 2, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0]
CHANCE_OF_TYPE = {0: 5, 1: 0x40, 2: 0x0A, 3: 0}

# etiquetas de arena (orden de TagAdd): 0 vehiculo, 1 D, 2 conductor, 3..6 p0..p3, 7 X, 8 Vd, 9 PL, 10 Vp, 11 vs, 12 mi, 13 matriz
VEH, ARD, ARDRV, ARX, ARPL, ARVP = tag(0, 0), tag(1, 0), tag(2, 0), tag(7, 0), tag(9, 0), tag(10, 0)
INTEL_GROUP = 0x600 + 0x68


def ge(a, b):
    """a >= b ordenado: fcomp; test ah,1; jne (falso con NaN o a < b)"""
    if a == NAN or b == NAN:
        return False
    return not greater(b, a)


def lt(a, b):
    return a != NAN and b != NAN and greater(b, a)


def mat890(m, v):
    """0x59C890: matriz * punto, con el orden de sumas del EXE; m = dict desplazamiento -> bits, v = 3 bits"""
    x = store(add(add(add(mul(L(m[0x20]), L(v[2])), mul(L(m[0x10]), L(v[1]))), mul(L(m[0x00]), L(v[0]))), L(m[0x30])))
    y = store(add(add(add(mul(L(m[0x24]), L(v[2])), mul(L(m[0x04]), L(v[0]))), mul(L(m[0x14]), L(v[1]))), L(m[0x34])))
    z = store(add(add(add(mul(L(m[0x28]), L(v[2])), mul(L(m[0x08]), L(v[0]))), mul(L(m[0x18]), L(v[1]))), L(m[0x38])))
    return [x, y, z]


def sub3(a, b):
    """fld a; fsub b; fstp: componente a componente (bits)"""
    return [store(sub(L(a[i]), L(b[i]))) for i in range(3)]


def mag_below_quarter(d):
    """Magnitude (0x4082C0): sqrt((x*x + y*y) + z*z) con la pila x87 a 24 bits; el resultado queda en el registro (sin guardar) y se compara con 0.25.
    El redondeo a 24 bits de la raiz queda por debajo de 0.25 si y solo si la raiz exacta es menor que 0.25 - 2**-27 (el empate redondea a 0.25, par)."""
    s = add(add(mul(L(d[0]), L(d[0])), mul(L(d[1]), L(d[1]))), mul(L(d[2]), L(d[2])))
    if s == NAN or s[0] == 'i':
        return False
    return s[1] < (Fraction(1, 4) - Fraction(1, 1 << 27)) ** 2


def expected(c):
    """devuelve dict: events, fields, players, rand, changes, reached"""
    LAST.clear()
    ev = []
    out = dict(events=ev, reached=0)
    state = [c['rand_seed'] & M32]

    def rnd():
        state[0] = (state[0] * 0x343FD + 0x269EC3) & M32
        return (state[0] >> 16) & 0x7FFF

    dmg_tag = {0: 0, 1: ARD, 2: ARX, 3: ARDRV, 4: tag(3, 0)}[c['dmg_kind']]
    dmg_is_ped = c['dmg_kind'] in (3, 4) or (c['dmg_kind'] == 1 and (c['dmg_type'] & 7) == 3)
    ped_tag = {0: 0, 1: ARD, 2: ARDRV, 3: tag(3, 0), 4: ARPL}[c['fp_ped']]
    veh_tag = {0: 0, 1: VEH, 2: ARX, 3: ARVP}[c['fp_veh']]
    weapon = c['weapon'] & M32
    status = c['status'] & 31
    vtype, vsub = c['vtype'] & M32, c['vsub'] & M32
    # campos observables (valores iniciales = centinelas del banco)
    health = c['health']
    fields = dict(health=health, lastw=0xA5, lastd=0xDEAD0050, bomb=0x1234, whodet=0xDEAD00E0, f928=0xDEAD0928)
    players = [c['havoc0'], c['chase0']]
    inten = c['intensity']

    def finish(path):
        out['fields'] = [fields['health'], fields['lastw'], fields['lastd'], fields['bomb'], fields['whodet'], fields['f928']]
        out['players'] = list(players)
        out['rand'] = state[0]
        out['path'] = path
        return out

    ev.append([1, VEH, dmg_tag, weapon, STACK])
    if (c['can_ret'] & 0xFF) == 0:
        return finish('cannot_damage')
    damaged_due = (c['can_fire'] & 0xFF) != 0
    path = []
    if status == 0:
        ev.append([2])
        if ge(L(c['progress']), L(HUNDRED)):
            inten = store(mul(L(inten), L(HALF)))
            path.append('progress_halved')
    if greater(L(inten), L(TEN)):
        ev.append([3, M32])
        cond = dmg_tag == ped_tag
        if not cond:
            ev.append([4, M32, 0])
            cond = dmg_tag == veh_tag
        if cond and status != 5:
            players[0] = (players[0] + 2) & M32
            players[1] = store(add(L(players[1]), L(ONE)))
            r = rnd()
            value = 5 + r % 20
            ev.append([5, 0x7D, store(('f', Fraction(value), 0))])
            path.append('havoc')
    # rueda reventada por un peaton
    if dmg_tag != 0 and dmg_is_ped and vtype in (0, 9):
        chance = 0
        slot = (weapon - 22) & M32
        if slot <= 30:
            chance = CHANCE_OF_TYPE[SLOT_TYPE[slot]]
        ev.append([6, dmg_tag])
        if c['is_player_ret'] & 0xFF:
            chance = 0
        skip = False
        att_vehicle = c['dmg_kind'] == 1 and c['dmg_has_veh'] != 0
        if att_vehicle and c['vd_sub'] == 9:
            if chance > 1:
                chance = 1
        elif MODELS[c['model'] % 8] == 0x20B and c['has_driver'] and c['drv_ped_type'] == 6:
            skip = True
        if not skip and chance != 0 and not c['tyres_dont']:
            r = rnd()
            if (r & 0x7F) < chance:
                if vtype == 9:
                    ev.append([8, VEH, c['coord'][0], c['coord'][1]])
                    ev.append([9, VEH, ((c['tyre_ret'] & 0xFF) + 13) & 0xFF, 0])
                    path.append('burst_bike')
                else:
                    ev.append([7, VEH])
                    if (c['appearance'] & M32) == 1:
                        ev.append([8, VEH, c['coord'][0], c['coord'][1]])
                        ev.append([9, VEH, ((c['tyre_ret'] & 0xFF) + 13) & 0xFF, 1])
                        path.append('burst_car')
    # deposito de gasolina
    if c['petrol'] and damaged_due and dmg_tag != 0 and dmg_is_ped:
        ev.append([6, dmg_tag])
        if c['is_player_ret'] & 0xFF:
            dummy = c['dummy']
            nonzero = False
            for b in dummy:
                v = L(b)
                if v == NAN or v[0] == 'i' or v[1] != 0:
                    nonzero = True
            if nonzero:
                mat = {off: c['mat'][i] for i, off in enumerate(MAT_OFF)}
                world = mat890(mat, dummy)
                diff = sub3(c['coord'], world)
                if mag_below_quarter(diff):
                    inten = health if lt(L(health), L(ELEVEN00)) else ELEVEN00
                    path.append('gas_cap')
    # helicopteros y aviones
    if vsub in (3, 4) and not c['is_rc'] and weapon not in (0x33, 0x10, 0x13, 0x14):
        inten = store(mul(L(inten), L(FOUR_TENTHS)))
        path.append('aircraft')
    if greater(L(health), L(ZERO)):
        fields['lastw'] = weapon & 0xFF
        if dmg_tag != 0:
            fields['lastd'] = dmg_tag
            ev.append([11, dmg_tag, tag(0, 0x50C), dmg_tag])
        if greater(L(health), L(inten)):
            old = health
            health = store(sub(L(health), L(inten)))
            fields['health'] = health
            if (status == 0 or 2 <= status <= 3) and dmg_tag != 0 and dmg_is_ped:
                targets = []
                if c['has_driver']:
                    targets.append(ARDRV)
                for i in range(c['max_pass'] & 0xFF):
                    if i < 8 and c['pass'][i]:
                        targets.append(tag(2 + c['pass'][i], 0))
                for t in targets:
                    ev.append([11, VEH, STACK, VEH])
                    ev.append([11, dmg_tag, STACK, dmg_tag])
                    ev.append([13, t + INTEL_GROUP, VEH, dmg_tag, weapon, 0])
                    ev.append([12, VEH, STACK, VEH])
                    ev.append([12, dmg_tag, STACK, dmg_tag])
                path.append('events')
            if ge(L(old), L(TWO50)) and lt(L(health), L(TWO50)) and vtype == 0:
                ev.append([15, tag(0, 0x5A0), 225])
                fields['f928'] = dmg_tag
                if dmg_tag != 0:
                    ev.append([11, dmg_tag, tag(0, 0x928), dmg_tag])
                path.append('engine')
        else:
            health = ZERO
            fields['health'] = health
            ev.append([3, M32])
            if dmg_tag == ped_tag:
                if vsub == 3 and not c['is_rc']:
                    crime = 12
                elif vsub == 4 and not c['is_rc']:
                    crime = 16
                else:
                    crime = 0x14
                ev.append([14, crime, VEH, dmg_tag])
                path.append('crime')
            if weapon == 0x33:
                r = rnd()
                fields['bomb'] = ((r & 0x7FF) + 0x3E8) & 0xFFFF
                fields['whodet'] = dmg_tag
                if dmg_tag != 0:
                    ev.append([11, dmg_tag, tag(0, 0x4E0), dmg_tag])
                path.append('detonate')
            else:
                ev.append([10, VEH, dmg_tag, 0])
                path.append('blowup')
    if c['law_enf']:
        ev.append([3, M32])
        if dmg_tag == ped_tag:
            ev.append([3, M32])
            ev.append([16, ped_tag, 1])
            path.append('wanted')
    return finish('+'.join(path) if path else 'nothing')


def check_side(c, o, exp):
    """compara un lado (observed) con el modelo; devuelve el motivo del primer desacuerdo o None"""
    got = o['events']
    if len(got) != len(exp['events']):
        return 'numero de eventos %d != %d: %s / %s' % (len(got), len(exp['events']), got[:12], exp['events'][:12])
    for i, (g, w) in enumerate(zip(got, exp['events'])):
        if len(g) != len(w):
            return 'evento %d longitud %s != %s' % (i, g, w)
        for k, (a, b) in enumerate(zip(g, w)):
            if a != b:
                return 'evento %d campo %d: %x != %x (%s / %s)' % (i, k, a, b, g, w)
    if o['fields'] != exp['fields']:
        return 'campos del vehiculo %s != %s' % ([hex(x) for x in o['fields']], [hex(x) for x in exp['fields']])
    if o['players'] != exp['players']:
        return 'jugador %s != %s' % (o['players'], exp['players'])
    if o['rand'] != exp['rand']:
        return 'semilla de rand %x != %x' % (o['rand'], exp['rand'])
    for i, ch in enumerate(o['changes']):
        if ch:
            return 'cambios de la arena %d: %s' % (i, ch)
    if o['unexpected_virtuals'] != 0:
        return 'virtual inesperado'
    return None


def check_row(row):
    c = row['input']
    exp = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], exp)
        if why:
            return side, why
    return None


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 8:
                print('DIFIERE', r[0], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
