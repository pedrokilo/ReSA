"""Oraculo independiente del banco PartnerDealFirst31: CTaskComplexPartnerDeal::CreateFirstSubTask (0x6823B0).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL):
  006823B0: jmp 00681F20        ; thiscall (ECX = this) con un argumento de pila (ped), `ret 4` en el destino
Es un salto directo: los argumentos, los registros y la pila llegan intactos a CTaskComplexPartner::CreateFirstSubTask (0x681F20), y su retorno (EAX) es el de la funcion.
El banco sustituye 0x681F20 por un falso (ambos lados) que registra (this, ped) y devuelve un valor del caso.
Valor esperado: ret = valor del falso ; calls = [(this, ped)] ; `this` intacto.
Llamadores del EXE: llamadas virtuales `call [reg+24h]` de la maquinaria de CTaskComplexs (vtable de CTaskComplexPartnerDeal 0x870754, ranura 9 = 0x6823B0, verificado en el EXE); un
escaneo de todo .text no halla ningun sitio de tareas que lea ECX/EDX tras la llamada.
"""
CASES = 4096
NAME = 'CTaskComplexPartnerDeal::CreateFirstSubTask'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}


def expected_first(inp, case):
    ped = 'T+0' if inp['ped_kind'] == 1 else inp['ped'] & 0xFFFFFFFF
    kind = inp['ret_kind']
    if kind not in (0, 1, 2) or inp['ped_kind'] not in (0, 1):
        raise RuntimeError('Fixture invalida en %d' % case)
    ret = 'T+0' if kind == 1 else ped if kind == 2 else inp['ret'] & 0xFFFFFFFF
    want = {'ret': ret, 'calls': [['T+0', ped]], 'ncalls': 1, 'this_changes': []}
    stats = {
        'family%d' % inp['family']: True,
        'ped_null': inp['ped_kind'] == 0 and inp['ped'] == 0, 'ped_self': inp['ped_kind'] == 1, 'ped_high': inp['ped_kind'] == 0 and inp['ped'] >= 0x80000000,
        'ped_ff': inp['ped_kind'] == 0 and inp['ped'] == 0xFFFFFFFF,
        'ret_self': kind == 1, 'ret_ped': kind == 2, 'ret_null': kind == 0 and inp['ret'] == 0, 'ret_high': kind == 0 and inp['ret'] >= 0x80000000,
        'ret_value': kind == 0,
        'stack_seed': True,
    }
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_first(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def _base(**kw):
    inp = dict(family=3, ped_kind=0, ped=0x12345678, ret_kind=0, ret=0x9ABCDEF0, eax=0, edx=0, stack_seed=0)
    inp.update(kw)
    return inp


if __name__ == '__main__':
    want, st = expected_first(_base(), 0)
    assert want == {'ret': 0x9ABCDEF0, 'calls': [['T+0', 0x12345678]], 'ncalls': 1, 'this_changes': []} and st['ret_high']
    want, st = expected_first(_base(ret_kind=1, ped=0), 1)
    assert want['ret'] == 'T+0' and want['calls'] == [['T+0', 0]] and st['ret_self'] and st['ped_null']
    want, st = expected_first(_base(ret_kind=2, ped_kind=1), 2)
    assert want['ret'] == 'T+0' and want['calls'] == [['T+0', 'T+0']] and st['ped_self'] and st['ret_ped']
    want, st = expected_first(_base(ret_kind=2), 3)
    assert want['ret'] == 0x12345678
    try:
        expected_first(_base(ret_kind=3), 4)
        raise SystemExit('el oraculo debia rechazar la fixture invalida')
    except RuntimeError:
        pass
    print('ok')
