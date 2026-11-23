"""Genera la imagen de datos del gta_sa.exe 1.0 US como C++ (arreglos) para el ejecutable independiente.

Uso: python -I reversing/FnFase8/gen_imagen.py [--original <gta-sa.exe>] [--destino source/game_sa/DatosExe/Imagen] [--comprobar]

Lee las secciones de DATOS del exe original (todas menos .text y _rwcseg, que son codigo y no se copian) y escribe:
  ImagenTabla.cpp     cabeceras PE originales (g_CabeceraOriginal, 0x400 B) y la tabla de trozos {va, tam, datos}
  ImagenDatosNN.cpp   los bytes, en trozos de <= 48 KiB; un trozo de ceros no se emite (la reserva ya es cero)
Todo va dentro de #ifdef GTASA_STANDALONE_EXE: el ASI (DLL) los compila vacios. Se ejecuta UNA VEZ (los archivos se versionan); solo hace falta
volver a ejecutarlo para regenerar. Con --comprobar no escribe: verifica que los archivos existentes coinciden byte a byte con el exe.
"""
import argparse, os, re, struct, sys

TAM_ESPERADO = 5189632
BASE = 0x400000
COD_INI, COD_FIN = 0x401000, 0x858000
RESERVA_FIN = 0xCB1000
TROZO = 48 * 1024


def secciones(d):
    e = struct.unpack_from('<I', d, 0x3C)[0]
    ns = struct.unpack_from('<H', d, e + 6)[0]
    soh = struct.unpack_from('<H', d, e + 20)[0]
    base = struct.unpack_from('<I', d, e + 24 + 28)[0]
    tam_cab = struct.unpack_from('<I', d, e + 24 + 60)[0]
    assert base == BASE
    o = e + 24 + soh
    out = []
    for i in range(ns):
        n, vs, va, rs, rp = struct.unpack_from('<8sIIII', d, o + 40 * i)
        out.append((n.rstrip(b'\0').decode(), BASE + va, vs, rs, rp))
    return tam_cab, out


def trozos(d):
    tam_cab, secs = secciones(d)
    res = []
    for nombre, va, vs, rs, rp in secs:
        if COD_INI <= va < COD_FIN:
            continue
        assert COD_FIN <= va and va + vs <= RESERVA_FIN, nombre
        n = min(rs, vs)
        for k in range(0, n, TROZO):
            m = min(TROZO, n - k)
            datos = d[rp + k: rp + k + m]
            if any(datos):
                res.append((nombre, va + k, datos))
    return tam_cab, res


def arreglo(nombre, datos):
    lineas, linea = [], []
    for b in datos:
        linea.append(str(b))
        if len(linea) == 40:
            lineas.append(','.join(linea)); linea = []
    if linea:
        lineas.append(','.join(linea))
    return 'extern const uint8_t %s[%d] = {\n%s\n};\n' % (nombre, len(datos), ',\n'.join(lineas))


CAB = ('// GENERADO por reversing/FnFase8/gen_imagen.py desde el gta_sa.exe 1.0 US (5.189.632 B). No editar a mano: se regenera con el script.\n'
       '// Bytes de datos del juego original que el ejecutable independiente copia a sus direcciones fijas (docs/CodigoVivoExe/ejecucion-imagen-datos.md).\n'
       '// Solo se compila con GTASA_STANDALONE_EXE; el ASI lo compila vacio (y CMake lo excluye).\n')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--original', default=r'D:\Grand Theft Auto San Andreas\gta-sa.exe')
    ap.add_argument('--destino', default=os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'source', 'game_sa', 'DatosExe', 'Imagen'))
    ap.add_argument('--comprobar', action='store_true')
    a = ap.parse_args()
    d = open(a.original, 'rb').read()
    if len(d) != TAM_ESPERADO:
        sys.exit('tamano %d distinto de %d: no es el gta_sa.exe 1.0 US' % (len(d), TAM_ESPERADO))
    tam_cab, tr = trozos(d)
    archivos = {}
    tabla = ['#ifdef GTASA_STANDALONE_EXE', '#include <cstdint>', '']
    cab = d[:tam_cab]
    tabla.append('extern "C" const uint8_t g_CabeceraOriginal[%d] = {\n%s\n};' % (len(cab), arreglo('x', cab).split('{\n', 1)[1].rsplit('\n};', 1)[0]))
    tabla.append('namespace ImagenExe {')
    tabla.append('struct Trozo { uint32_t va, tam; const uint8_t* datos; };')
    filas = []
    for i, (nombre, va, datos) in enumerate(tr):
        sim = 'Datos%02d' % i
        archivos['ImagenDatos%02d.cpp' % i] = CAB + '#ifdef GTASA_STANDALONE_EXE\n#include <cstdint>\nnamespace ImagenExe {\n// %s VA 0x%08X +0x%X\n%s} // namespace ImagenExe\n#endif\n' % (nombre, va, len(datos), arreglo(sim, datos))
        tabla.append('extern const uint8_t %s[%d];' % (sim, len(datos)))
        filas.append('    {0x%08X, %d, %s},' % (va, len(datos), sim))
    tabla.append('extern const Trozo kTrozos[] = {\n' + '\n'.join(filas) + '\n};')
    tabla.append('extern const uint32_t kNTrozos = %d;' % len(tr))
    tabla.append('} // namespace ImagenExe')
    tabla.append('#endif')
    archivos['ImagenTabla.cpp'] = CAB + '\n'.join(tabla) + '\n'
    dest = os.path.abspath(a.destino)
    if a.comprobar:
        mal = 0
        for n, c in archivos.items():
            p = os.path.join(dest, n)
            if not os.path.exists(p) or open(p, encoding='utf-8', newline='').read().replace('\r\n', '\n') != c:
                print('DIFERENTE:', n); mal += 1
        extra = [f for f in os.listdir(dest) if f.startswith('Imagen') and f not in archivos]
        print('comprobados %d archivos, diferentes %d, sobrantes %s' % (len(archivos), mal, extra))
        sys.exit(1 if mal or extra else 0)
    os.makedirs(dest, exist_ok=True)
    for f in os.listdir(dest):
        if f.startswith('Imagen') and f.endswith('.cpp'):
            os.remove(os.path.join(dest, f))
    tot = 0
    for n, c in archivos.items():
        open(os.path.join(dest, n), 'w', encoding='utf-8', newline='\n').write(c)
        tot += len(c)
    print('%d trozos, %d bytes de datos, %d archivos, %d bytes de fuente' % (len(tr), sum(len(t[2]) for t in tr), len(archivos), tot))


main()
