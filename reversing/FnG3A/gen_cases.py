"""Genera cases.txt (una linea de comandos completa por linea, en hex, sin NUL, con el nombre del programa)
y groups.txt (el grupo de cada caso, en el mismo orden)."""
import itertools
import random
import sys
from pathlib import Path

random.seed(0x746480)
cases = []
group = {}


def add(c, g):
    cases.append(c)
    group.setdefault(c, g)


# A) lineas de comandos de uso real (.bat / accesos directos / lanzadores) y artificiales
exes = [b'gta_sa.exe', rb'"C:\Games\GTA San Andreas\gta_sa.exe"', rb'C:\Games\gta_sa.exe',
        rb'"D:\Grand Theft Auto San Andreas\gta-sa.exe"',
        rb'"C:\Program Files (x86)\Rockstar Games\GTA San Andreas\gta_sa.exe"', rb'.\gta_sa.exe', b'"gta_sa.exe"']
real = [b'', b' ', b' -windowed', b' -vms', b' -width 800 -height 600', b' -windowed -width 1280 -height 720',
        b' -vms -windowed', rb' -path "C:\Games\GTA SA"', rb' -path C:\Games', b'\t-vms', b'  -vms   -windowed  ',
        b' "-vms"', rb' -config "C:\a b\c.ini" -vms', b' -vms -width 1024 -height 768', b' -windowed -vms -width 640']
barra = [rb' -path "C:\Games\GTA SA\"', rb' -path "C:\Games\GTA SA\" -vms', rb' -vms -path "C:\Games\GTA SA\"']
artificial = [b' -vms"', b' "-vms', b' -"vms"', b' -v"m"s', b' "a b" c', b' "a b"c', b' a"b c"d',
              b' "" -vms', b' -vms ""', b' ""', b' "', b' -vms "', b' ""x', rb' \"-vms\"', rb' -x=\"a b\"', b' -vms\r\n',
              b' -vms\x01-windowed', b' -d\xe9bug -vms', b' \xe9\xe9 -vms', b' -vms ' + b'x' * 300]
for e in exes:
    for lst, g in ((real, 'real'), (barra, 'real-barra-final'), (artificial, 'artificial')):
        for a in lst:
            add(e + a, g)
for c in (b'', b' ', b'\t', b'"', b'""', b'" "', b'-vms', b'  -vms'):
    add(c, 'artificial')
# B) exhaustivas cortas
for alf, L in ((b' \t"\\a', 6), (b' \t"\\a-\x01\x7f\xe9', 4)):
    for n in range(L + 1):
        for t in itertools.product(alf, repeat=n):
            add(bytes(t), 'exhaustiva')
# C) aleatorias con alfabetos sesgados (incluye los 255 bytes no nulos)
alfs = [b' \t"\\ab', b'  "a-vm\\', b'a b"\\\t', bytes(range(1, 256)), b'ab ""\\\\ \t\r\n"', b' "-vmsw']
for i in range(24000):
    a = random.choice(alfs)
    n = random.choice((8, 12, 20, 40, 80, 200))
    add(bytes(random.choice(a) for _ in range(n)), 'aleatoria')
# C2) mezclas de piezas con -vms (para medir cuantas diferencias cambian el unico argumento que consume el juego)
piezas = [b'-vms', b' ', b' ', b'\t', b'"', b'\\', b'-', b'a b', b'\r', b'\x01', b'""', b'\xe9', b'vms', b'-windowed', b'\\"']
for i in range(8000):
    add(b'gta_sa.exe ' + b''.join(random.choice(piezas) for _ in range(random.randint(1, 9))), 'mezcla-vms')
# D) longitudes grandes
for n in (1000, 4000, 8190, 8191, 8192, 8193, 16383, 16384, 30000, 32000):
    add(rb'"C:\x y\gta_sa.exe" ' + b'-vms ' * (n // 5), 'larga')
    add(b'p ' + bytes(random.choice(b'ab "\\\t') for _ in range(n)), 'larga')
    add(b'p "' + b'a b' * (n // 3), 'larga')
cases = [c for c in cases if len(c) < 32000 and b'\0' not in c]
seen, out = set(), []
for c in cases:
    if c not in seen:
        seen.add(c)
        out.append(c)
Path(sys.argv[1] if len(sys.argv) > 1 else 'cases.txt').write_text('\n'.join(c.hex() for c in out) + '\n')
Path('groups.txt').write_text('\n'.join(group[c] for c in out) + '\n')
print(len(out), 'casos')
