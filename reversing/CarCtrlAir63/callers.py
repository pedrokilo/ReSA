"""Busca en .text del EXE las llamadas (E8) y saltos (E9) a cada direccion y muestra las 4 instrucciones siguientes a cada llamada."""
import struct, subprocess, sys
exe = open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe', 'rb').read()
pe = struct.unpack_from('<I', exe, 0x3c)[0]
n = struct.unpack_from('<H', exe, pe + 6)[0]
opt = struct.unpack_from('<H', exe, pe + 20)[0]
secs = [struct.unpack_from('<8sIIII', exe, pe + 24 + opt + i * 40) for i in range(n)]
text = [s for s in secs if s[0].startswith(b'.text')][0]
_, vs, va, rs, ro = text
base = 0x400000 + va
data = exe[ro:ro + rs]
targets = {int(a, 16): name for a, name in [x.split('=') for x in sys.argv[1:]]}
found = {t: [] for t in targets}
for i in range(len(data) - 5):
    op = data[i]
    if op in (0xE8, 0xE9):
        rel = struct.unpack_from('<i', data, i + 1)[0]
        tgt = (base + i + 5 + rel) & 0xFFFFFFFF
        if tgt in targets:
            found[tgt].append((base + i, op))
for t, lst in found.items():
    print('==', hex(t), targets[t], len(lst))
    for a, op in lst:
        print('  ', hex(a), 'call' if op == 0xE8 else 'jmp')
