"""Independent native CPad::ProcessPad (EXE 0x746A10) model. Derived from evidence/ProcessPad.dumpbin.asm.txt,
not from source/game_sa/Pad.cpp. x87 arithmetic is emulated step by step with exact rational arithmetic and
round-to-nearest-even at the active precision-control width (24 or 53 bits), stores to float32 round at 24 bits."""
import json, math, struct
from fractions import Fraction
from pathlib import Path
HERE = Path(__file__).resolve().parent
_doc = json.loads((HERE / 'fixtures.json').read_text(encoding='utf8'))
COUNT = _doc['count']; FIELDS = _doc['fields']; ROWS = _doc['rows']
NOTACQ = struct.unpack('<i', struct.pack('<I', 0x8007000C))[0]; LOST = struct.unpack('<i', struct.pack('<I', 0x8007001E))[0]
ACCESS = struct.unpack('<i', struct.pack('<I', 0x80070005))[0]; E_FAIL = struct.unpack('<i', struct.pack('<I', 0x80004005))[0]
# route: (poll results, acquire results, getstate result); exhausted sequences return 0. Route 0 = selected slot null.
ROUTE = {1: ([0], [], 0), 2: ([1], [], 1), 3: ([NOTACQ, 0], [0], 0), 4: ([NOTACQ, 0], [LOST, 0], 0), 5: ([NOTACQ, 0], [LOST, LOST, LOST, 0], 0),
         6: ([NOTACQ], [E_FAIL], 0), 7: ([NOTACQ], [LOST, ACCESS], 0), 8: ([NOTACQ, NOTACQ], [0], 0), 9: ([0], [], NOTACQ), 10: ([NOTACQ, 0], [1], 0)}
C_0005 = Fraction(struct.unpack('<f', bytes.fromhex('6f12033a'))[0]); C_2P32 = Fraction(2 ** 32)
C_001 = Fraction(struct.unpack('<f', bytes.fromhex('0ad7233c'))[0]); C_RAD = Fraction(struct.unpack('<f', bytes.fromhex('35fa8e3c'))[0])
C_GATE = Fraction(struct.unpack('<f', struct.pack('<f', 0.3))[0])
def pat(base, j): return (base + j * 7) & 0xFF
def rb(x, bits):
    """round Fraction to `bits` significant bits, nearest even"""
    x = Fraction(x)
    if x == 0: return x
    sign = -1 if x < 0 else 1; a = abs(x); e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a: e -= 1
    while Fraction(2) ** (e + 1) <= a: e += 1
    scale = Fraction(2) ** (e - bits + 1); q = a / scale; r = q.numerator // q.denominator; rem = q - r
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and r % 2): r += 1
    return sign * r * scale
def f32bits(x): return struct.pack('<f', float(rb(x, 24)))
def f32val(raw): return Fraction(struct.unpack('<f', raw)[0])
def ulp_adjust(raw, steps):
    """move a float32 by `steps` representable values along the real line (monotonic key mapping)"""
    if not steps: return raw
    bits = struct.unpack('<I', raw)[0]; key = bits if not bits >> 31 else -(bits & 0x7FFFFFFF); key += steps
    return struct.pack('<I', key if key >= 0 else 0x80000000 | -key)
def fixture(i):
    assert 0 <= i < COUNT
    return dict(zip(FIELDS, ROWS[i]))
def sentinel(poison):
    p = poison // 85
    rx = [0.0, 0.25, 0.5, -3.5][p]; ry = [0.0, -0.25, -0.75, struct.unpack('<f', struct.pack('<f', 0.3))[0]][p]
    return b''.join(struct.pack('<f', v) for v in (rx, ry, 13.5 + p, 14.5 + p))
def joystate(f):
    js = bytearray(pat(f['poison'] + 0x77, j) for j in range(272))
    js[0:4] = struct.pack('<i', f['lx']); js[4:8] = struct.pack('<i', f['ly']); js[8:12] = struct.pack('<i', f['lz']); js[20:24] = struct.pack('<i', f['lrz'])
    js[0x20:0x24] = struct.pack('<I', f['pov'])
    return bytes(js)
def expected(f, begin, adj=(0, 0)):
    """returns (payload, eax) for fixture f. adj = ULP adjustment of sin/cos results (analyzer tolerance search)."""
    poison, route, pad, pc = f['poison'], f['route'], f['pad'], (24 if f['pc'] == 0 else 53)
    cm = bytearray(pat(poison + 0x11, j) for j in range(548)); cm[0] = f['justinit']
    sticks = bytearray(sentinel(poison)); pads = bytearray(pat(poison + 0x22, j) for j in range(616))
    joy = bytearray(pat(poison + 0x33, j) for j in range(32)); other = 1 - pad; jm = f['joymask']
    joy[pad * 16 + 5] = jm & 1; joy[pad * 16 + 6] = (jm >> 1) & 1; joy[other * 16 + 5] = (jm ^ 3) & 1; joy[other * 16 + 6] = ((jm ^ 3) >> 1) & 1
    menu = bytearray.fromhex(begin['menu_real'])
    for k in range(6): menu[9 + k] = (f['menu'] >> k) & 1       # BA67F9..BA67FE relative to BA67F0
    padnum = f['padnum']
    dev = [begin['dev0'], begin['dev1']]
    arena = bytearray([0xC7] * 16 + [pat(poison + 0x44, j) for j in range(40)] + [0xC7] * 16)
    slots = [dev[0], dev[1]]
    if route == 0: slots[pad] = 0
    arena[16 + 0x20:16 + 0x24] = struct.pack('<I', slots[0]); arena[16 + 0x24:16 + 0x28] = struct.pack('<I', slots[1])
    trace = []; eax = 0; pad_arg = pad
    def result(): return {'controls': cm.hex(), 'sticks': sticks.hex(), 'pads': pads.hex(), 'joy_table': joy.hex(), 'menu': menu.hex(), 'padnum': padnum, 'arena': arena.hex(), 'trace': trace}, eax
    if route == 0: return result()
    polls, acqs, state_hr = ROUTE[route]; polls = list(polls); acqs = list(acqs)
    def poll():
        hr = polls.pop(0) if polls else 0; trace.append(['poll', pad, hr]); return hr
    def acquire():
        hr = acqs.pop(0) if acqs else 0; trace.append(['acq', pad, hr]); return hr
    hr = poll()
    if hr < 0:
        hr = acquire()
        while hr == LOST: hr = acquire()
        if hr < 0: eax = hr; return result()
        hr = poll()
        if hr < 0: eax = hr; return result()
    trace.append(['state', pad, 272])
    if state_hr < 0: eax = state_hr; return result()
    js = joystate(f)
    if f['justinit'] == 1: cm[4:276] = js; cm[276:548] = js; cm[0] = 0
    else: cm[4:276] = cm[276:548]; cm[276:548] = js
    calls = [0]
    def event(code):
        calls[0] += 1; trace.append(['evt', code, pad_arg_now()])
    state = {'pad_arg': pad_arg}
    def pad_arg_now(): return state['pad_arg']
    event(32)                       # call 1: EXE 746AF6, argument is the caller's own padID slot
    if f['flip'] == 3: state['pad_arg'] = 1 - state['pad_arg']
    if f['flip'] == 4: slots[pad] = 0; arena[16 + 0x20:16 + 0x24] = struct.pack('<I', slots[0]); arena[16 + 0x24:16 + 0x28] = struct.pack('<I', slots[1])
    # EXE 746B00: `test eax,eax / je 746BA6` only skips the stick block (746B08..746BA0); events 0x20/0x1F and the axis code still run.
    device_gone = slots[pad] == 0
    def mulstore(value_fraction, const):
        return f32bits(rb(value_fraction * const, pc))
    if not device_gone: sticks[8:12] = mulstore(Fraction(f['lx']), C_0005); sticks[12:16] = mulstore(Fraction(f['ly']), C_0005)
    pov = f['pov']
    if device_gone: pass
    elif (pov & 0xFFFF) != 0xFFFF:
        signed = pov - 2 ** 32 if pov >= 2 ** 31 else pov; x = Fraction(signed)
        if signed < 0: x = rb(x + C_2P32, pc)
        x = rb(x * C_001, pc); x = rb(x * C_RAD, pc); arg = float(x)
        s = ulp_adjust(struct.pack('<f', math.sin(arg)), adj[0]); c = struct.pack('<f', float(rb(Fraction(-math.cos(arg)), pc)))
        sticks[8:12] = s; sticks[12:16] = ulp_adjust(c, adj[1])
    if not device_gone and joy[state['pad_arg'] * 16 + 5] and joy[state['pad_arg'] * 16 + 6]:
        sticks[0:4] = mulstore(Fraction(f['lz']), C_0005); sticks[4:8] = mulstore(Fraction(f['lrz']), C_0005)
    new_id = 1 if padnum != 0 else 0
    state['pad_arg'] = new_id; event(32)
    if f['flip'] == 1: padnum = 0 if padnum != 0 else 1
    event(31)
    if f['flip'] == 2: padnum = 0 if padnum != 0 else 1
    target = 1 if padnum != 0 else 0
    def axis(offset, invert, swapped, normal_off, swapped_off):
        value = f32val(bytes(sticks[offset:offset + 4]))
        if abs(value) <= C_GATE: return
        if invert: sticks[offset:offset + 4] = struct.pack('<f', -struct.unpack('<f', bytes(sticks[offset:offset + 4]))[0]); value = -value
        scaled = value * 128; integer = int(scaled) if scaled >= 0 else -int(-scaled)
        pos = 0xA8 + (swapped_off if swapped else normal_off)
        pads[target * 308 + pos:target * 308 + pos + 2] = struct.pack('<H', integer & 0xFFFF)
    m = f['menu']
    axis(8, m & 1, (m >> 4) & 1, 0, 2); axis(12, (m >> 1) & 1, (m >> 4) & 1, 2, 0)
    axis(0, (m >> 2) & 1, (m >> 5) & 1, 4, 6); axis(4, (m >> 3) & 1, (m >> 5) & 1, 6, 4)
    return result()
if __name__ == '__main__':
    import collections
    begin = {'menu_real': '00' * 24, 'dev0': 0x1000, 'dev1': 0x2000}; seen = set(); routes = collections.Counter(); sizes = set()
    for i in range(COUNT):
        f = fixture(i); key = tuple(f.values()); assert key not in seen; seen.add(key)
        payload, eax = expected(f, begin); routes[f['route']] += 1; sizes.add((len(payload['controls']), len(payload['pads']), len(payload['arena'])))
    assert sizes == {(1096, 1232, 144)}
    print('independent finite corpus', COUNT, dict(sorted(routes.items())))
