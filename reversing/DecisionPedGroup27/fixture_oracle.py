"""Oraculo independiente del banco DecisionPedGroup27: CDecisionMakerTypes::MakeDecision Ped (0x606E70) y Group (0x606F80).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco:

  606E70 (Ped, ECX=this, 11 args, ret 2Ch): id = [[ped+47Ch] + (useGroup(byte)!=0 ? B8h : B4h)] ; eventIndex = dword[this + C034h + eventType*4]
         word[task]=0C8h ; word[facial]=0FFFFh ; id==-2 -> 6040D0(this+0D4ECh+eventIndex*3Ch) ; id==-1 -> 606060(this+(byte[ped+484h]==2 ? 0CB50h : 0C1B4h), eventIndex, ...)
         que hace 6040D0(base+eventIndex*3Ch) ; otro id -> 6040D0(this+4+(id*29h+eventIndex)*3Ch)
  606F80 (Group, ECX=this, 8 args, ret 20h): id = dword[group+2C8h] ; word[ranura arg1]=0C8h (dword) ; id==-1 -> 6040D0(this+(byte[group+2D0h]==1 ? 0E824h : 0DE88h)+eventIndex*3Ch)
         ; otro -> 6040D0(this+4+(id*29h+eventIndex)*3Ch) ; EAX = movsx(word[ranura arg1])
  6040D0 (colaborador, con 6007A0 y rand 0x821B1E): modelado por reversing/DecisionMakeDecision27/fixture_oracle.py (x87 a 24 bits, mismo estado de rand).

`python -I fixture_oracle.py` ejecuta la autoprueba (sin juego).
"""
import importlib.util
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
_spec = importlib.util.spec_from_file_location('decisionmakedecision27_oracle', HERE.parent / 'DecisionMakeDecision27' / 'fixture_oracle.py')
md = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(md)

M32 = 0xFFFFFFFF
CASES_PER_FUNCTION = 2560
BASE_CASES = 2048
NAMES = ['CDecisionMakerTypes::MakeDecision-Ped', 'CDecisionMakerTypes::MakeDecision-Group']
s32, s16 = md.s32, md.s16


def selected_offset(inp):
    """Desplazamiento de la decision que lee el EXE (desde el desensamblado, no desde la fuente)."""
    ei = inp['event_index']
    if inp['func'] == 0:
        ident = s32(inp['dm_b8'] if inp['use_group'] else inp['dm_b4'])
        if ident == -2:
            return 0xD4EC + ei * 60
        if ident == -1:
            return (0xCB50 if inp['ped_type'] == 2 else 0xC1B4) + ei * 60
        return (4 + ((ident & M32) * 41 + ei) * 60) & M32
    ident = s32(inp['group_id'])
    if ident == -1:
        return (0xE824 if inp['mission_flag'] == 1 else 0xDE88) + ei * 60
    return (4 + ((ident & M32) * 41 + ei) * 60) & M32


def expected(inp):
    """(observed, stats) que debe producir CADA lado (EXE y DLL). La ruta -1 de Ped pasa por 0x606060 -> 0x6040D0 una sola vez."""
    if selected_offset(inp) != inp['selected_offset']:
        raise RuntimeError('Desplazamiento de decision distinto del modelado: %s' % inp)
    inner, stats = md.expected({'obj': inp['obj'], 'st': inp['st'], 'in_veh': inp['in_veh'], 'avoid': inp['avoid'], 'seek': inp['seek'], 'seed': inp['seed']}, inp['case'])
    want = {'selected_decision': inp['obj'], 'objects_unchanged': True, 'outputs_guard_ok': True, 'rng_state': inner['rng_state'], 'rand_calls': inner['rand_calls']}
    if inp['func'] == 0:
        want['task_slot'] = inner['task_slot']       # int16 con signo
        want['face_slot'] = -1
    else:
        want['return_eax'] = inner['task_slot'] & M32   # movsx eax, word ptr [ranura]
    return want, stats


if __name__ == '__main__':
    # autoprueba: enrutado de los seis selectores y de Group; una decision con una opcion y tirada 0 elige esa opcion
    import struct
    obj = bytearray(60)
    obj[0:4] = struct.pack('<i', 1000)
    obj[0x18 + 1] = 50          # probabilidad de la opcion 0 para sourceType 1
    obj[0x30 + 1] = 1           # bool para ir en vehiculo
    seed0 = next(s for s in ((c * 2654435761) & M32 for c in range(1 << 20)) if md.rand_output(s) == 0)
    base = dict(func=0, case=0, dm_b4=-2, dm_b8=7, use_group=0, group_id=-1, ped_type=1, mission_flag=0, event_id=3, event_index=5, st=1, in_veh=1,
                avoid=[1001, -1, -1], seek=-1, seed=seed0, obj=bytes(obj).hex())
    expect_off = {(-2, 1): 0xD4EC + 300, (-1, 1): 0xC1B4 + 300, (-1, 2): 0xCB50 + 300, (7, 1): 4 + (7 * 41 + 5) * 60}
    for (dm, pt), off in expect_off.items():
        inp = dict(base, dm_b4=dm, ped_type=pt, selected_offset=off)
        want, stats = expected(inp)
        assert want['task_slot'] == 1000 and want['rand_calls'] == 1 and want['face_slot'] == -1, want
    inp = dict(base, use_group=1, dm_b8=19, selected_offset=4 + (19 * 41 + 5) * 60)
    assert expected(inp)[0]['task_slot'] == 1000
    for gid, mf, off in ((-1, 0, 0xDE88 + 300), (-1, 1, 0xE824 + 300), (0, 0, 4 + 5 * 60), (19, 1, 4 + (19 * 41 + 5) * 60)):
        inp = dict(base, func=1, group_id=gid, mission_flag=mf, selected_offset=off)
        want, stats = expected(inp)
        assert want['return_eax'] == 1000 and 'task_slot' not in want
    # el valor de retorno de Group es movsx: una tarea 0xFFFF queda 0xFFFFFFFF
    obj2 = bytearray(obj); obj2[0:4] = struct.pack('<i', 0xFFFF)
    inp = dict(base, func=1, group_id=0, mission_flag=0, selected_offset=4 + 5 * 60, obj=bytes(obj2).hex())
    assert expected(inp)[0]['return_eax'] == 0xFFFFFFFF
    print('ok')
