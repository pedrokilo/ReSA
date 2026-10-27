"""Oraculo independiente (Python) de HeatSeek72: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6E0400..0x6E05BA, lo que debe observarse
(EAX etiquetado, traza de colaboradores y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + desplazamiento desde el inicio de la arena (guarda de 0x40 incluida);
indices: 0 vehiculo, 1 objetivo, 2 entidad previa, 3 matriz del vehiculo, 4 matriz del objetivo.
"""
import json
import sys

GUARD = 0x40
M32 = 0xFFFFFFFF


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


VEH, TGT, OTH, MV, MT = (tag(0, 0), tag(1, 0), tag(2, 0), tag(3, 0), tag(4, 0))


def expected(c):
    ev = []
    plane = c['sub_type'] == 4
    prev = {0: 0, 1: TGT, 2: OTH, 3: c['ent_raw']}[c['ent_kind']]
    target = {0: 0, 1: TGT, 2: VEH}[c['tgt_kind']]
    ev.append([1, c['vpos_x'], c['vpos_y'], c['vpos_z'], c['vdir_x'], c['vdir_y'], c['vdir_z'], 0x3F99999A, VEH, 1, prev])
    slack = 1 if plane and (c['pstate'] & 0x7FFFFFFF) == 0 else 0
    vflags_new, tflags_new, pstate_new = c['vflags'], c['tflags'], c['pstate']
    if target and ((c['time'] - slack) & M32) > 1000:
        tpos = [c['tpos_x'], c['tpos_y'], c['tpos_z']] if c['tgt_kind'] == 1 else [c['vpos_x'], c['vpos_y'], c['vpos_z']]
        if c['tgt_kind'] == 2:
            to_ptr = tag(3, 0x30)
        else:
            to_ptr = tag(4, 0x30) if c['tgt_mat'] else tag(1, 4)
        v_at = c['vflags'] & ~1 & M32
        t_at = (c['tflags'] & ~1 & M32) if c['tgt_kind'] == 1 else 0
        ev.append([2, tag(3, 0x30), to_ptr] + [c['vpos_x'], c['vpos_y'], c['vpos_z']] + tpos + [1, 1, 0, 1, 0, 1, 0, v_at, t_at])
        # el falso altera los dwords; despues el EXE restaura solo el bit 0
        post_v = v_at ^ c['mut_self']
        if c['tgt_kind'] == 1:
            post_t = (c['tflags'] & ~1 & M32) ^ c['mut_tgt']
            tflags_new = (post_t & ~1 & M32) | (c['tflags'] & 1)
        else:
            post_v ^= c['mut_tgt'] if c['tgt_kind'] == 2 else 0
        vflags_new = (post_v & ~1 & M32) | (c['vflags'] & 1)
        # 6E054B `test al,al`: visible = AL != 0 (AL entero, no solo el bit 0); 6E0524 `movzx ecx,al; shl ecx,0x1F`: el estado guardado lleva solo el bit 0 de AL
        al = c['los_al'] & 0xFF
        visible = 1 if al != 0 else 0
        if plane:
            pstate_new = (1 if c['time'] > 1 else 0) | ((al & 1) << 31)
    else:
        visible = ((c['pstate'] >> 31) & 1) if plane else 1
    if not visible:
        ev.append([3, 0])
        eax = 0
    elif not target or (c['status'] & 0xF8) != 0:
        ev.append([3, 0])
        eax = target
    else:
        tpos = [c['tpos_x'], c['tpos_y'], c['tpos_z']] if c['tgt_kind'] == 1 else [c['vpos_x'], c['vpos_y'], c['vpos_z']]
        ev.append([4, 0] + tpos + [255, 255, 255, 100, 0x3FA66666, 1])
        eax = target
    return eax, ev, vflags_new, tflags_new, pstate_new


def diffs(old, new, base):
    out = []
    for i in range(4):
        a, b = (old >> (8 * i)) & 0xFF, (new >> (8 * i)) & 0xFF
        if a != b:
            out.append([base + i, b])
    return out


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        c = row['input']
        eax, ev, vf, tf, ps = expected(c)
        veh_changes = diffs(c['vflags'], vf, 0x1C)
        if c['sub_type'] == 4:
            veh_changes += diffs(c['pstate'], ps, 0x9E4)
        tgt_changes = diffs(c['tflags'], tf, 0x1C) if c['tgt_kind'] == 1 else []
        if c['tgt_kind'] == 1 and not tgt_changes:
            pass
        want = [veh_changes, tgt_changes, [], [], []]
        # estado del avion y subtipo leidos por los miembros reales de CPlane (field_9E4 / m_nVehicleSubType); el subtipo no cambia
        want_state, want_sub = ps, c['sub_type']
        for side in ('original', 'candidate'):
            o = row[side]['observed']
            if o['eax'] != eax or o['events'] != ev or o['changes'] != want or o['plane_state'] != want_state or o['plane_sub'] != want_sub:
                bad += 1
                if bad <= 5:
                    print('DIFIERE', side, row['case'], 'eax', o['eax'], eax, '\n  events', o['events'], '\n  model ', ev, '\n  changes', o['changes'], '\n  model  ', want, '\n  plane', o['plane_state'], want_state, o['plane_sub'], want_sub)
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
