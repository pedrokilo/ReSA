"""Oraculo independiente del banco QuatConjugate29: CQuaternion::Conjugate() (0x4D37D0).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. ECX = cuaternion {x,y,z,w}; sin argumentos; `ret`:

  4D37D0 fld dword [ecx]     ; 4D37D2 fchs ; 4D37D4 fstp dword [ecx]        (x)
  4D37D6 fld dword [ecx+4]   ; 4D37D9 fchs ; 4D37DB fstp dword [ecx+4]      (y)
  4D37DE fld dword [ecx+8]   ; 4D37E1 fchs ; 4D37E3 fstp dword [ecx+8]      (z)
  4D37E6 ret

w (ecx+0xC) no se toca. Semantica x87 (palabra de control con excepciones enmascaradas): fld dword es exacto salvo que el operando sea un sNaN, que se convierte en qNaN (bit 22 a 1, signo y
carga util conservados; IE en FSW); los subnormales se cargan tal cual (denormal-operand enmascarado). fchs invierte el signo de cualquier valor, NaN incluido, sin otras
modificaciones. fstp dword vuelve a float32: la conversion de un valor float32 es exacta (qNaN: se conserva la carga util de 23 bits; subnormales: exactos), de modo que el resultado es
`bits ^ 0x80000000` con el bit 22 puesto si el operando era NaN. La pila x87 queda vacia. Registros: EAX/EDX no se tocan; ECX tampoco (se registran, nadie los lee).

El modelo del valor NO usa operaciones de bits para negar los finitos: usa floats de Python (struct) y se compara con las operaciones de bits en la autoprueba.
"""
import random
import struct

CASES = 4096
NAME = 'CQuaternion::Conjugate()'
NAMES = [NAME]
WIN_OFFSET, WIN_BYTES, ARENA = 0x40, 0x40, 0xC0


def classify(bits):
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    if exp == 0xFF:
        if man == 0:
            return 'inf'
        return 'qnan' if bits & 0x400000 else 'snan'
    if exp == 0:
        return 'zero' if man == 0 else 'sub'
    return 'normal'


def conj_component(bits):
    """fld dword + fchs + fstp dword."""
    kind = classify(bits)
    if kind == 'snan':
        bits |= 0x400000                  # fld: sNaN -> qNaN (IE enmascarada)
    if kind in ('normal', 'sub', 'zero', 'inf'):
        # camino independiente: negar con aritmetica de float de Python (-0.0 incluido), sin tocar el bit de signo a mano
        v = struct.unpack('<f', struct.pack('<I', bits))[0]
        return struct.unpack('<I', struct.pack('<f', -v))[0]
    return bits ^ 0x80000000              # NaN: fchs solo invierte el signo


def simulate(inp):
    win = bytearray(bytes.fromhex(inp['win_in']))
    qoff = inp['qoff']
    q = [b & 0xFFFFFFFF for b in inp['q']]
    struct.pack_into('<4I', win, qoff, *q)          # el banco escribe el cuaternion antes de llamar
    stats = {'family': inp['family']}
    out = list(q)
    for i in range(3):
        k = classify(q[i])
        out[i] = conj_component(q[i])
        stats['kind_' + k] = stats.get('kind_' + k, 0) + 1
    if classify(q[3]) == 'snan':
        stats['w_snan'] = 1                         # w no se toca: sigue siendo sNaN
    if any(classify(q[i]) == 'snan' for i in range(3)):
        stats['any_snan'] = 1
    if any(classify(q[i]) == 'qnan' for i in range(3)):
        stats['any_qnan'] = 1
    if any(classify(q[i]) == 'inf' for i in range(3)):
        stats['any_inf'] = 1
    if any(classify(q[i]) == 'sub' for i in range(3)):
        stats['any_sub'] = 1
    if any(classify(q[i]) == 'zero' for i in range(3)):
        stats['any_zero'] = 1
    if qoff % 4:
        stats['unaligned'] = 1
    struct.pack_into('<4I', win, qoff, *out[:3], q[3])
    arena = bytearray(b'\xC3' * ARENA)
    arena[WIN_OFFSET:WIN_OFFSET + WIN_BYTES] = win
    return {'arena': arena.hex(), 'guard_ok': True}, stats


def expected(inp, case):
    return simulate(inp)


def matches(obs, want):
    return obs.get('arena') == want['arena'] and obs.get('guard_ok') is True


def run_one(q, qoff=0, win_in=None):
    inp = {'family': 0, 'q': list(q), 'qoff': qoff, 'win_in': win_in or '00' * WIN_BYTES}
    want, st = simulate(inp)
    arena = bytes.fromhex(want['arena'])
    return want, arena[WIN_OFFSET + qoff:WIN_OFFSET + qoff + 16], st


def selftest():
    u = lambda b, o: struct.unpack_from('<I', b, o)[0]
    # normales: signo invertido, w intacto
    _, o, _ = run_one([0x3F800000, 0xC0000000, 0x00000000, 0x3F000000])
    assert struct.unpack('<4I', o) == (0xBF800000, 0x40000000, 0x80000000, 0x3F000000), [hex(x) for x in struct.unpack('<4I', o)]
    # -0 -> +0, subnormal, infinitos
    _, o, _ = run_one([0x80000000, 0x00000001, 0x7F800000, 0xFF800000])
    assert struct.unpack('<4I', o) == (0x00000000, 0x80000001, 0xFF800000, 0xFF800000)
    # qNaN: solo signo; carga util intacta
    _, o, _ = run_one([0x7FC12345, 0xFFC54321, 0x7FFFFFFF, 0x7FC00001])
    assert struct.unpack('<4I', o) == (0xFFC12345, 0x7FC54321, 0xFFFFFFFF, 0x7FC00001)
    # sNaN: se silencia (bit 22) y cambia de signo; w sNaN no se toca
    _, o, st = run_one([0x7F800001, 0xFFA00000, 0x7FBFFFFF, 0x7F800001])
    assert struct.unpack('<4I', o) == (0xFFC00001, 0x7FE00000, 0xFFFFFFFF, 0x7F800001), [hex(x) for x in struct.unpack('<4I', o)]
    assert st.get('w_snan') == 1 and st.get('any_snan') == 1
    # desalineado: bytes vecinos intactos
    win = bytes(range(WIN_BYTES)).hex()
    want, o, _ = run_one([0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000], qoff=3, win_in=win)
    arena = bytes.fromhex(want["arena"])[WIN_OFFSET:WIN_OFFSET + WIN_BYTES]
    assert arena[:3] == bytes([0, 1, 2]) and arena[19:] == bytes(range(19, WIN_BYTES)) and u(o, 0) == 0xBF800000
    # camino de float de Python == camino de bits para todo no-NaN en una muestra amplia
    rng = random.Random(29)
    for _ in range(20000):
        b = rng.getrandbits(32)
        if classify(b) in ('qnan', 'snan'):
            continue
        assert conj_component(b) == b ^ 0x80000000, hex(b)
    for b in (0, 0x80000000, 1, 0x80000001, 0x7F800000, 0xFF800000, 0x7F7FFFFF, 0x007FFFFF, 0x00800000):
        assert conj_component(b) == b ^ 0x80000000
    # idempotencia con qNaN: dos conjugaciones devuelven el original (qNaN) y silencian un sNaN
    assert conj_component(conj_component(0x7F812345)) == 0x7FC12345
    assert matches({'arena': 'ab', 'guard_ok': True}, {'arena': 'ab', 'guard_ok': True}) and not matches({'arena': 'ab', 'guard_ok': True}, {'arena': 'ac', 'guard_ok': True})
    print('ok')


if __name__ == '__main__':
    selftest()
