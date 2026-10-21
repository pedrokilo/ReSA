"""Oraculo del banco CarCtrlWeave63C (lote 63, CCarCtrl parte B2, tanda C): CCarCtrl::UpdateCarOnRails (0x436540).

La verdad es el EXE: el banco exige EXE == DLL bit a bit (eventos de los falsos EN ORDEN con todos sus argumentos, cambios de todo el vehiculo y de las dos areas de enlaces, compuertas de ABI)
con los colaboradores con efectos sustituidos por falsos identicos. Este modulo modela, desde el desensamblado, la RUTA elegida (que ramas se recorren segun la entrada) para comprobar que
los eventos observados coinciden con ella (`expected_path`): 0x422590 siempre; con bIsStopped no hay mas; accion 1/24 -> ModifySpeed(0) (+ posible reinicio de la accion); accion 12 ->
ModifySpeed(0); resto -> SlowCarOnRails..., 0x432B10 si el limite vence, y, salvo estado 3, CalcCurvePoint + 0x42EC90.
`python -I fixture_oracle.py` ejecuta la autoprueba.
"""
CASES = 4096
NAMES = ['CCarCtrl::UpdateCarOnRails']
CASES_BY_NAME = {n: CASES for n in NAMES}
K_422590, K_432B10, K_SLOW, K_MODIFY, K_CURVE, K_42EC90 = range(1, 7)


def expected_path(row):
    """lista de tipos de evento esperada"""
    out = [K_422590]
    if row['move'] & 1:
        return out
    act = row['temp'][0]
    act = act - 256 if act >= 128 else act
    if act in (1, 24):
        return out + [K_MODIFY]
    if act == 12:
        return out + [K_MODIFY]
    out.append(K_SLOW)
    now = row['now']
    limit = (row['fieldc'] + row['scale']) & 0xFFFFFFFF
    neg = limit >= 0x80000000
    if neg or now >= limit:
        out.append(K_432B10)
    if (row['b36'] & 0xF8) == 0x18:
        return out
    return out + [K_CURVE, K_42EC90]


def expected(name, row, case):
    if row['fn'] != 0:
        raise AssertionError('funcion distinta de la entrada')
    st = {}
    path = expected_path(row)
    st['path_' + '_'.join(str(k) for k in path)] = True
    st['same_link'] = row['cur'] == row['next']
    st['sub10'] = row['sub'] == 10
    return None, st


if __name__ == '__main__':
    row = {'fn': 0, 'move': 0, 'temp': [0, 0], 'now': 100, 'fieldc': 0, 'scale': 50, 'b36': 2, 'cur': 0, 'next': 1, 'sub': 0}
    assert expected_path(row) == [1, 3, 2, 5, 6]
    row['move'] = 1
    assert expected_path(row) == [1]
    print('autoprueba del oraculo: ok')
