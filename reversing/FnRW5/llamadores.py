# Busca en gta-sa.exe quien llama (call/jmp rel32) o apunta (palabra de 32 bits) a cada funcion del lote RW5.
# Uso: python llamadores.py   (variable GTASA_EXE = ruta de gta-sa.exe)
import os, struct, sys
ruta = os.environ.get("GTASA_EXE", r"D:\Grand Theft Auto San Andreas\gta-sa.exe")
D = open(ruta, "rb").read()
pe = struct.unpack_from("<I", D, 0x3c)[0]
ns = struct.unpack_from("<H", D, pe + 6)[0]; osz = struct.unpack_from("<H", D, pe + 20)[0]; base = struct.unpack_from("<I", D, pe + 52)[0]
secs = []
for i in range(ns):
    o = pe + 24 + osz + 40 * i
    nombre = D[o:o + 8].rstrip(b"\0").decode()
    vs, va, rs, rp = struct.unpack_from("<IIII", D, o + 8)
    secs.append((nombre, va + base, rs, rp))
objetivos = [0x75A960, 0x762F30, 0x763310, 0x7624A0, 0x762740, 0x74A880, 0x74A770, 0x7F40F0, 0x7F5140, 0x7F53D0, 0x7F2930]
def seccion_de(va):
    for n, a, rs, rp in secs:
        if a <= va < a + rs: return n
    return "?"
for t in objetivos:
    llam = []; ptrs = []
    for n, a, rs, rp in secs:
        datos = D[rp:rp + rs]
        for i in range(0, rs - 4):
            if datos[i] in (0xE8, 0xE9) and n == ".text":
                rel = struct.unpack_from("<i", datos, i + 1)[0]
                if (a + i + 5 + rel) & 0xFFFFFFFF == t: llam.append(a + i)
            if struct.unpack_from("<I", datos, i)[0] == t and (i % 4 == 0 or n != ".text"):
                if not (datos[i - 1] in (0xE8, 0xE9) if i else False): ptrs.append((n, a + i))
    print("0x%X:" % t, "llamadas " + ", ".join("0x%X" % c for c in llam) if llam else "sin llamadas directas", "|", "punteros " + ", ".join("%s:0x%X" % p for p in ptrs) if ptrs else "sin punteros")
