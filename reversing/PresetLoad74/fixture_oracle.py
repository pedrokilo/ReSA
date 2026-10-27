"""Oraculo independiente (Python) de PresetLoad74: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x619D60..0x619F97, lo que debe observarse
(traza de colaboradores con los hashes FNV de los textos, filtrado por isprint del buffer, nodos de la lista, contadores y valor de retorno) y lo compara con las filas de comparisons.jsonl.

Los nodos y descripciones del monton de pruebas se verifican byte a byte contra la imagen de relleno (misma funcion Rand/FillWords que Fixtures.hpp); solo los dos punteros de cada nodo
(descripcion +0x1C y siguiente +0x20, direcciones absolutas del banco) son «cualquier valor».

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 monton, 1 ruta, 2 fichero.
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
GUARD = 0x40
HEAP_ARENA = GUARD + 0x800 + GUARD
FMT = b'%f%f%f%f%f%f%f %[^'   # el literal del EXE es "%f...%[^\0]": el texto C termina tras el ^
NODE = 0x24


def tag(idx, off=0):
    return 0x10000000 + (idx << 20) + GUARD + off


def mix(x):
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 74076) & M32)


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        w = rand(seed, i // 4)
        out += w.to_bytes(4, 'little')[:min(4, nbytes - i)]
    return bytearray(out)


def fnv(data):
    if data is None:
        return 0
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


def isprint(ch):
    return 0x20 <= ch <= 0x7E


def expected(c):
    """(eventos, num, lista tag, heap esperado (bytearray), rangos libres [(a, b)], isprint (cuenta, hash))"""
    heap = fill_words(HEAP_ARENA, (c['seed'] * 13 + 1) & M32)
    free = []
    ev = [[1, c['num_before'], c['list_before']]]
    ev.append([2, fnv(b'./views.txt')])
    path_tag = 0 if c['path_null'] else tag(1)
    ev.append([3, path_tag, 0 if c['path_null'] else fnv(b'./views.txt'), fnv(b'r')])
    ev.append([4, path_tag])
    num = 0
    head = 0
    isp_count = 0
    isp_hash = 2166136261
    mallocs = 0
    heap_off = 0
    closed = False
    aborted = False
    if c['fopen_ok']:
        file_tag = tag(2)
        k = 0
        while True:
            ev.append([5, 1023, file_tag])
            if k >= c['n_lines']:
                break
            raw = bytes.fromhex(c['line%d' % k])
            ev.append([6, fnv(raw)])
            for ch in raw:
                isp_count += 1
                isp_hash = ((isp_hash ^ ch) * 16777619) & M32
            clean = bytes(ch for ch in raw if isprint(ch))
            ev.append([7, fnv(clean), fnv(FMT)])
            ret = c['ret%d' % k]
            ret = ret - (1 << 32) if ret & 0x80000000 else ret
            floats = struct.unpack('<7I', bytes.fromhex(c['floats%d' % k]))
            kk = k
            k += 1
            if ret < 7:
                continue
            if ret == 7:
                name = ('View%d' % num).encode()
                ev.append([8, fnv(b'%s%d'), fnv(b'View'), num])
            else:
                name = bytes.fromhex(c['name%d' % kk])
            ev.append([9, NODE, 0])
            mallocs += 1
            if mallocs == c['fail_at']:
                ev.append([13, fnv(b'Out of memory - not all preset views loaded')])
                aborted = True
                break
            node_off = heap_off
            heap_off = (node_off + NODE + 15) & ~15
            base = GUARD + node_off
            # campos del nodo: +0 f1, +4 f2, +8 f3, +0xC f4, +0x10 f5, +0x14 f6, +0x18 f7 (se escriben antes de reservar la descripcion)
            for i in range(7):
                heap[base + 4 * i:base + 4 * i + 4] = floats[i].to_bytes(4, 'little')
            free.append((base + 0x1C, base + 0x24))
            ev.append([6, fnv(name)])
            ev.append([9, len(name) + 1, 0])
            mallocs += 1
            if mallocs == c['fail_at']:
                ev.append([13, fnv(b'Out of memory - not all preset views loaded')])
                ev.append([11, tag(0, node_off)])
                aborted = True
                break
            desc_off = heap_off
            heap_off = (desc_off + len(name) + 1 + 15) & ~15
            ev.append([10, tag(0, desc_off), fnv(name)])
            dbase = GUARD + desc_off
            heap[dbase:dbase + len(name) + 1] = name + b'\0'
            num += 1
            head = tag(0, node_off)
        if not aborted:
            ev.append([12, file_tag])
            closed = True
    al = 1 if (num != 0 and not aborted) else 0
    return ev, num, head, heap, free, (isp_count, isp_hash), al, closed


def main(path):
    bad = checked = nodes = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        c = row['input']
        ev, num, head, heap, free, isp, al, closed = expected(c)
        nodes += num
        for side in ('original', 'candidate'):
            o = row[side]['observed']
            ok = True
            why = ''
            if o['events'] != ev:
                ok = False
                for i, (g, w) in enumerate(zip(o['events'], ev)):
                    if g != w:
                        why = 'evento %d: %s != %s' % (i, g, w)
                        break
                else:
                    why = 'numero de eventos %d != %d' % (len(o['events']), len(ev))
            if o['num'] != num or o['cur'] != 0xFFFFFFFF or o['al'] != al or o['isprint'] != [isp[0], isp[1]]:
                ok = False
                why = why or 'num/cur/al/isprint %s' % ({k: o[k] for k in ('num', 'cur', 'al', 'isprint')},)
            if o['list'] != head:
                ok = False
                why = why or 'lista %x != %x' % (o['list'], head)
            # monton: imagen final = relleno + cambios; debe coincidir con la esperada salvo los punteros de los nodos
            if o['changes'][1] or o['changes'][2]:
                ok = False
                why = why or 'cambios en la ruta o el fichero'
            ref = fill_words(HEAP_ARENA, (c['seed'] * 13 + 1) & M32)
            final = bytearray(ref)
            for off, b in o['changes'][0]:
                final[off + GUARD] = b
            skip = set()
            for a, b2 in free:
                skip.update(range(a, b2))
            for i in range(HEAP_ARENA):
                if i in skip:
                    continue
                if final[i] != heap[i]:
                    ok = False
                    why = why or 'monton byte %x: %02x != %02x' % (i - GUARD, final[i], heap[i])
                    break
            if not ok:
                bad += 1
                if bad <= 8:
                    print('DIFIERE', side, row['case'], why)
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad, 'nodos creados', nodes)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
