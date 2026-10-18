"""work-queue.csv y prioridades del cohorte «2»: entradas del juego pendientes ordenadas por esfuerzo estimado (de menor a mayor).

score = bytes del cuerpo (suma de rangos de binary_function_ranges.csv)
      + 400 * destinos de call/jmp a entradas del juego SIN nombre de catálogo (dependencias que tampoco están reconstruidas)
      + 150 * saltos/llamadas indirectas (virtuales, tablas de punteros)
      +  60 * (hay instrucciones x87: la validación bit a bit exige el tratamiento x87 del proyecto)
      +  80 * (hay tabla de saltos de switch)
      +  30 * llamadas a biblioteca (RW/CRT, direcciones >= 0x740000)
      +  20 * llamadas a funciones ya catalogadas (existen, pero hay que respetar su contrato)
      + 150 * (uso de SEH: acceso a fs:[0])
Es una heurística de esfuerzo, no una prueba; las 20 primeras se revisaron a mano con el desensamblado (ver README).
Formato de work-queue.csv: las seis columnas del cohorte 499 más columnas de prioridad al final.
"""
import argparse
import csv
import sys
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / 'contrib'))


def rd(path):
    with Path(path).open(encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--binary-map', type=Path, required=True)
    ap.add_argument('--exe', type=Path, default=Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    args = ap.parse_args()
    import capstone
    from pe_inventory import PE
    pe = PE(args.exe)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    X86_OP_REG, X86_OP_MEM = 1, 3
    fns = {int(r['va'], 16): r for r in rd(args.binary_map / 'binary_functions.csv')}
    ranges = defaultdict(list)
    for r in rd(args.binary_map / 'binary_function_ranges.csv'):
        ranges[int(r['function_va'], 16)].append((int(r['start_va'], 16), int(r['end_va'], 16)))
    progress = rd(HERE / 'progress.csv')
    out = []
    for r in progress:
        if r['category'] != 'game' or r['runtime_status'] == 'passed':
            continue
        va = int(r['address'], 16)
        size = ung = ind = fpu = table = lib = cat = seh = 0
        for lo, hi in ranges.get(va, [(va, va + int(fns[va]['body_bytes'] or 0) - 1)]):
            off = pe.offset(lo)
            size += hi - lo + 1
            for i in md.disasm(pe.data[off:off + hi - lo + 1], lo):
                if i.mnemonic.startswith('f') and i.mnemonic not in ('fs',):
                    fpu += 1
                if 'fs:' in i.op_str:
                    seh = 1
                if i.mnemonic in ('call', 'jmp') and i.operands:
                    op = i.operands[0]
                    if op.type == 2:  # immediate
                        t = op.imm & 0xFFFFFFFF
                        if lo <= t <= hi:
                            continue
                        if t >= 0x740000:
                            lib += 1
                        elif t in fns and not fns[t]['catalogue_names'] and t != va:
                            ung += 1
                        elif t != va:
                            cat += 1
                    else:
                        if i.mnemonic == 'jmp' and op.type == X86_OP_MEM and op.mem.scale == 4 and op.mem.index:
                            table += 1
                        else:
                            ind += 1
        score = size + 400 * ung + 150 * ind + 60 * (fpu > 0) + 80 * (table > 0) + 30 * lib + 20 * cat + 150 * seh
        out.append(dict(address=r['address'], identity=r['identity'], status='pending', bank='', pairs_expected='', source_snapshot=r['source'],
                        score=score, body_bytes=size, uncatalogued_game_callees=ung, indirect_calls=ind, x87_instructions=fpu,
                        switch_tables=table, library_calls=lib, catalogued_callees=cat, seh=seh, implementation=r['implementation']))
    out.sort(key=lambda x: (x['score'], x['address']))
    for rank, row in enumerate(out, 1):
        row['priority_rank'] = rank
    fields = ['address', 'identity', 'status', 'bank', 'pairs_expected', 'source_snapshot', 'priority_rank', 'score', 'body_bytes',
              'uncatalogued_game_callees', 'indirect_calls', 'x87_instructions', 'switch_tables', 'library_calls', 'catalogued_callees', 'seh', 'implementation']
    with (HERE / 'work-queue.csv').open('w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fieldnames=fields)
        w.writeheader()
        w.writerows(out)
    print(len(out), 'entradas del juego pendientes en work-queue.csv')
    for row in out[:30]:
        print(row['priority_rank'], row['address'], row['identity'][:50], row['score'], row['body_bytes'], row['uncatalogued_game_callees'], row['indirect_calls'], row['x87_instructions'], row['switch_tables'], row['implementation'])


if __name__ == '__main__':
    main()
