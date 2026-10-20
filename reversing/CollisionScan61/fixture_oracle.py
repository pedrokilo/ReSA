"""Oraculo independiente del banco CollisionScan61 (CCollisionEventScanner::ScanForCollisionEvents, 0x604500), modelado desde el DESENSAMBLADO del EXE.

La descripcion paso a paso del desensamblado esta en la cabecera de oracle_coll.py. `expected` recibe la fila `input` que registra el banco y devuelve (observed, stats): `observed` es exactamente lo
que cada lado (EXE y DLL) debe registrar en `observed` y `stats` son las banderas de cobertura. La autoprueba (`python fixture_oracle.py`, ojo: `python -I` no anade el directorio del script a sys.path)
comprueba el oraculo con casos escritos a mano.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import oracle_coll

NAME_COLL = oracle_coll.NAME
ALL_CASES = {NAME_COLL: 4096}
CASES = dict(ALL_CASES)
NAMES = list(CASES)
HOOKS = {NAME_COLL: 0x604500}
HOOK_NAMES = {NAME_COLL: 'CCollisionEventScanner::ScanForCollisionEvents'}
# direcciones del EXE sustituidas por falsos
STUBBED = {NAME_COLL: (0x4AB420, 0x6819D0, 0x61A360, 0x535300, 0x601D70, 0x571B70, 0x571A00, 0x5DF8F0, 0x73A530, 0x59C910, 0x53CB50, 0x5F0360, 0x441DB0, 0x4B35A0, 0x4B5AC0, 0x5F3640,
                      0x5EFFE0, 0x421050, 0x608970, 0x608830, 0x5A7950, 0x5DEF60)}
ARENAS = dict(oracle_coll.ARENAS)
# Minimos de cobertura (~50 % de lo medido en la corrida 20261019-234808-026910 del banco)
MINIMUMS = {NAME_COLL: {'building': 253, 'damage_computed': 45, 'damage_not_affected': 54, 'dmg_nan': 40, 'dot_back': 38, 'dot_front': 54, 'early_null': 38, 'early_zero': 34, 'goto': 856, 'hit_by_car_skip': 111, 'kind0': 14, 'kind1': 253, 'kind2': 684, 'kind3': 464, 'kind4': 500, 'kind5': 19, 'kind6': 23, 'kind7': 16, 'no_branch': 73, 'obj_att_skip': 28, 'obj_clamp0': 43, 'obj_contact_skip': 24, 'obj_dot_eval': 200, 'obj_dot_neg': 63, 'obj_event': 437, 'obj_ftol_zero': 8, 'obj_hard': 62, 'obj_over_limit': 320, 'obj_static': 63, 'object': 500, 'ped': 464, 'ped_dplayer': 170, 'ped_hate': 123, 'ped_nohate': 47, 'ped_plain': 110, 'ped_vplayer': 183, 'phase2': 298, 'phase2_dplayer': 167, 'phase2_hate': 27, 'phase2_plain': 70, 'phase2_skip': 166, 'phase2_vplayer': 60, 'side_nan': 4, 'side_neg': 48, 'side_pos': 53, 'tail_bala': 290, 'tail_dmg_gt1': 180, 'tail_noise': 151, 'tail_nonphys': 219, 'tail_nonphys_noise30': 40, 'tail_nonphys_noise40': 77, 'tail_nonphys_time_fail': 62, 'tail_object_nophys': 126, 'tail_phys': 71, 'tail_phys_fast': 51, 'tail_phys_noise': 33, 'tail_phys_time_fail': 18, 'tail_player': 1060, 'tail_type': 452, 'task_none': 675, 'task_other': 1300, 'train': 123, 'train_cond': 27, 'train_kill': 10, 'train_phys': 58, 'v_clamp0': 81, 'v_clamp20': 75, 'v_dot_neg': 86, 'v_high': 363, 'v_high_attached': 40, 'v_high_kill': 129, 'v_high_nonplayer': 170, 'v_high_player': 193, 'v_low': 209, 'v_low_attached': 59, 'v_low_event': 79, 'v_low_nan': 19, 'v_low_player': 70, 'v_player_kill': 57, 'vehicle': 684}}
for _k in range(16):
    MINIMUMS[NAME_COLL]['family%d' % _k] = 256


def expected(function, inp, case):
    if function == NAME_COLL:
        return oracle_coll.expected(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def check_observed(function, observed, want, inp):
    if function == NAME_COLL:
        return oracle_coll.check_observed(observed, want)
    raise RuntimeError('Funcion inesperada ' + function)


if __name__ == '__main__':
    assert oracle_coll.selftest()
    print('ok')
