# Tabla de objetos DatosExe (.bss/.data) a partir del .map de enlace del standalone: direccion del EXE original (sufijo hex del nombre),
# direccion real en el proceso y tamano (hueco hasta el siguiente simbolo; se recorta con el tamano declarado si se conoce).
import re, sys, json, os
ABS = re.compile(r'^\s*([0-9a-f]{4}):([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8})\s+(?:f\s+)?(?:i\s+)?(\S+\.obj|\S+)?')
def cargar(mapa, solo_bss=True):
    syms = []   # (va_proceso, nombre, exe_addr, ns)
    for l in open(mapa, encoding='utf-8', errors='replace'):
        m = re.match(r'^\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+(f\s+)?(\S+)?', l)
        if not m or m.group(3): continue
        nom, va = m.group(1), int(m.group(2), 16)
        mm = re.match(r'^\?(b|sl_|v|k)([0-9A-F]{5,6})@(Bss|Data|Rdata|R5)@DatosExe@@3', nom)
        if not mm: continue
        syms.append((va, nom, int(mm.group(2), 16), mm.group(3), m.group(4) or ''))
    syms.sort()
    return syms
if __name__ == '__main__':
    s = cargar(sys.argv[1]); print(len(s)); print(s[:3])
