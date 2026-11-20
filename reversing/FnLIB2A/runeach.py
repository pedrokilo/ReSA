"""Corre cada funcion de tabla.txt en su propio proceso (un EXE desconocido puede corromper el proceso) y reune las lineas de resultado.
Uso: python runeach.py <bench.exe> <ruta del EXE> <pares> [salida.txt] [opciones extra del banco...]"""
import re, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
HERE = Path(__file__).resolve().parent
bench, exe, n = sys.argv[1:4]
out = sys.argv[4] if len(sys.argv) > 4 else None
extra = sys.argv[5:]
tab = [l.split() for l in open(HERE / 'tabla.txt') if l.strip()]
def one(t):
    if t[4] == '0': return f'{t[0]:34s} SIN DIRECCION EN EL EXE'
    try: r = subprocess.run([bench, exe, str(HERE / 'tabla.txt'), '--n', n, '--diffs', '2', '--filtro', t[0]] + extra, capture_output=True, text=True, timeout=1800)
    except subprocess.TimeoutExpired: return f'{t[0]:34s} TIMEOUT'
    lines = [l for l in r.stdout.splitlines() if l.startswith(t[0]) or l.startswith('  DIF') or 'HANG' in l]
    if not any(l.startswith(t[0]) for l in lines): return f'{t[0]:34s} 0x{int(t[4],16):06X} FALLO DEL PROCESO (codigo {r.returncode & 0xFFFFFFFF:#x})'
    return '\n'.join(lines)
with ThreadPoolExecutor(4) as ex: res = list(ex.map(one, tab))
text = '\n'.join(res)
print(text)
if out: Path(out).write_text(text + '\n')
