import json, os, pathlib, hashlib

R = 'reversing/CppTwoLevel5Unified/runs'


def passed_runs(bank):
    out = []
    for d in sorted(x for x in os.listdir(R) if x.startswith(bank + '-')):
        s = json.load(open(f'{R}/{d}/summary.json', encoding='utf-8'))
        if s.get('passed'):
            out.append((d, s))
    return out


def last(b):
    return passed_runs(b)[-1]


p = 'reversing/CppOnlyMigration/approvals.json'
a = json.load(open(p, encoding='utf-8'))
tail_marker = ' Revalidación'


def tail_of(e):
    i = e['scope'].find(tail_marker)
    return e['scope'][i:] if i >= 0 else ''


def head_of(e):
    i = e['scope'].find(tail_marker)
    return e['scope'][:i] if i >= 0 else e['scope']


def ab(bank):
    d, s = last(bank)
    return {'bank': bank, 'run': d.split('-', 1)[1], 'pairs': s['pairs'], 'summary': f'{R}\\{d}/summary.json'}


def refresh(addr, bank, extra=()):
    e = a[addr]
    d, s = last(bank)
    e['run'] = d.split('-', 1)[1]
    e['pairs'] = s['pairs']
    e['own_pairs'] = s['pairs']
    e['audit'] = f'{R}\\{d}/summary.json'
    rest = [x for x in e.get('additional_banks', []) if x['bank'] not in [b for b in extra] + [bank]]
    e['additional_banks'] = rest + [ab(b) for b in extra]
    if not e['additional_banks']:
        e.pop('additional_banks')


# 0x41BD90 etc. viven en el registro del lote 4 (otro directorio de runs): solo se añaden los bancos nuevos como adicionales
def add_prec(addr, note):
    e = a[addr]
    d, s = last('CppX87Precision1')
    e.setdefault('additional_banks', [])
    e['additional_banks'] = [x for x in e['additional_banks'] if x['bank'] != 'CppX87Precision1'] + [ab('CppX87Precision1')]
    e['scope'] = head_of(e) + ' ' + note + tail_of(e)


add_prec('0x0041BD90', 'GeneralRandomInRangeDblCpp devuelve double (extern "C") y General.cpp lo enlaza como float: con float(...) un resultado menor que 2^-126 se redondeaba a subnormal de float y el EXE lo conserva con exponente extendido (caso 302 de CppX87Precision1 a 24 bits). Exacto bajo las tres precisiones y los cuatro redondeos.')
add_prec('0x0053CC70', 'Sin diferencias exigidas a 24 y 53 bits en los cuatro redondeos; 1408 diferencias exploratorias solo a 64 bits (ver límites).')
add_prec('0x0053CBE0', 'GeneralRadianAngleDblCpp devuelve double (extern "C"): el EXE termina con ST0 sin guardar y con float(...) el resultado se redondeaba a 24 bits bajo 53 bits de precisión. Exacto a 24 y 53 bits.')
add_prec('0x0053CE30', 'Sin diferencias exigidas a 24 y 53 bits; diferencias exploratorias solo a 64 bits.')
add_prec('0x005DEF60', 'CppX87Precision1 la compara con las tres precisiones y los cuatro redondeos (EAX, ECX y EDX): exacta a 24 y 53 bits; diferencias exploratorias solo a 64 bits.')
add_prec('0x006F5240', 'Exacto bajo las tres precisiones y los cuatro redondeos (CppX87Precision1).')
add_prec('0x006F5290', 'Exacto bajo las tres precisiones y los cuatro redondeos (CppX87Precision1).')
add_prec('0x006F5080', 'HandlingConvertGameUnitsCpp lleva todo el cuerpo aritmético en double bajo float_control(precise), con volatile float donde el EXE relee de memoria: el EXE mantiene accel, la velocidad y 1/masa sin redondear a float en la pila x87 y la versión anterior los redondeaba en cada paso y descartaba subnormales (127 a 143 casos de 1024 en CppX87Precision1 a 24 bits). Exacto a 24 bits y en los cuatro redondeos.')

# bancos nuevos para UpdateAnimations y Offscreen
for addr, ident, scope in (
        ('0x004D34F0', 'RpAnimBlendClumpUpdateAnimations (bucles de sincronización y de paso de tiempo)',
         'Los dos bucles (0x4D3578 y 0x4D35C4..0x4D3605) pasan por RpUpdateSyncAccumCpp, RpUpdateSyncStepCpp, ExeAdd/ExeMul y CloudsEqualOrderedCpp: el EXE encadena fdiv, fmul y fadd en x87 con un redondeo a 24 bits por paso y el C++ lo hacía en double con SSE (53 bits) y trataba un subnormal como cero. Banco nuevo CppRpAnimUpdate1: 4096 pares con los callbacks del EXE y 2048 con los cinco callbacks en C++, comparando en bits. Antes solo la ejercía el banco antiguo reversing/RpAnimBlend (652 pares, tolerancia 1e-5).'),
        ('0x004D2E10', 'FrameUpdateCallBackOffscreen',
         'Reescrita para llamar a FrameUpdateCallBackSkinnedWithVelocityExtraction como el EXE (prueba de la bandera 8, m_PedPosition y 0x4D1680, siempre 2D); antes usaba la plantilla FrameUpdateCallBackT, que se eliminó junto con FrameUpdateCallBackW por ser código muerto (no se instanciaban). Banco CppRpAnimUpdate1: 1024 pares.')):
    assert addr not in a, addr
    d, s = last('CppRpAnimUpdate1')
    a[addr] = {'identity': ident, 'converted_source_functions': 0, 'source': 'source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp',
               'shared_source': 'source/game_sa/X87Math.cpp', 'bank': 'CppRpAnimUpdate1', 'run': d.split('-', 1)[1], 'pairs': s['pairs'],
               'own_pairs': s['pairs'], 'audit': f'{R}\\{d}/summary.json', 'differences': 0, 'null_comparison_records': 0, 'state_restored': True,
               'helper_codegen': 'reversing/CppTwoLevel5Study/README.md',
               'scope': scope + ' Sin cobertura exhaustiva de x87 más allá de los fixtures del banco.'}

# DoFootLanded: referencia al último run exacto
refresh('0x005E5380', 'CppPedFootEmitters1', ('CPedCompletion',))
for addr, bank in (('0x005DEF60', 'CPedC'), ('0x005DF910', 'CPedE'), ('0x005DFDF0', 'CPedE'), ('0x005E1FA0', 'CPedB'), ('0x005E65A0', 'CPedB')):
    refresh(addr, bank, tuple(x['bank'] for x in a[addr].get('additional_banks', []) if x['bank'] == 'CppX87Precision1'))
for addr in ('0x004D1680', '0x004D1A50', '0x004D1DB0', '0x004D2100', '0x004D2450', '0x004D27F0'):
    refresh(addr, 'RpAnim43')
json.dump(a, open(p, 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)

rp = pathlib.Path('reversing/CppTwoLevel5Study/README.md')
t = rp.read_text(encoding='utf-8')
banks = sorted({d.split('-')[0] for d in os.listdir(R)})
btab = '\n'.join(f'| {b} | {last(b)[1]["pairs"]} |' for b in banks)
i = t.index('| Banco | Pares |')
j = t.index('\n\n', i)
t = t[:i] + '| Banco | Pares |\n| --- | ---: |\n' + btab + t[j:]
rows = {q.name: hashlib.sha256(q.read_bytes()).hexdigest() for q in sorted(pathlib.Path('reversing/CppTwoLevel5Study/artifacts').glob('*.obj'))}
i = t.index('| Objeto | SHA256 |')
t = t[:i] + '| Objeto | SHA256 |\n| --- | --- |\n' + '\n'.join(f'| {n} | {h} |' for n, h in sorted(rows.items())) + '\n'

a0 = t.index('## Bancos')
seg = '''## Segundo seguimiento: los límites restantes (15/10/2026)

Tras el primer seguimiento quedaban tres límites; esto es lo que se hizo con cada uno.

- **`FrameUpdateCallBackW` y `Offscreen` sin ejercer.** La descripción anterior era inexacta. Lo que no se compila es la alternativa copy-paste de esas dos funciones; el análisis encontró que `FrameUpdateCallBackW` y `FrameUpdateCallBackT` eran código muerto (no se instanciaban en ningún sitio, salvo `T` desde `Offscreen`, que a su vez no se llamaba ni se registraba) y se eliminaron. Lo que de verdad no tenía banco con pares exactos era `RpAnimBlendClumpUpdateAnimations` (0x4D34F0; solo el banco antiguo `reversing/RpAnimBlend`, 652 pares con tolerancia 1e-5) y `FrameUpdateCallBackOffscreen` (0x4D2E10, sin hook). `FrameUpdateCallBackOffscreen` se reescribió como el EXE (llama a 0x4D1680, siempre 2D) y se registró como hook desactivado como los otros cuatro. Los dos bucles de `UpdateAnimations` encadenaban en el EXE `fdiv`/`fmul`/`fadd` en x87 con un redondeo a 24 bits por paso y el C++ los hacía en `double` con SSE; ahora pasan por ayudantes `RpUpdateSync*` y por `ExeAdd`/`ExeMul`. El banco nuevo `reversing/CppRpAnimUpdate1` (7168 pares: 4096 con los callbacks del EXE, 2048 con los cinco callbacks en C++ y 1024 de `Offscreen`) comparó en bits y aprobó tras corregir tres fallos de su fixture: la vtable falsa de las asociaciones no se rellenaba, el relleno aleatorio del paso de tiempo de una asociación desenlazada (4,7e12) colgaba `NextKeyFrame` en el EXE y la DLL, y un callback podía desenlazar a la superviviente dejando cero arrays de nodos, con lo que el callback del EXE desreferenciaba un puntero nulo. El banco tiene ahora predictor de iteraciones, cortafuegos y vigilante de 12 s.
- **Dependencia del x87 a 24 bits.** En vez de dejarla como supuesto se midió: el banco nuevo `reversing/CppX87Precision1` compara el EXE con la DLL forzando las 3 precisiones (24, 53 y 64 bits) y los 4 redondeos en 14 funciones (172032 pares, ST0 de 80 bits, ABI y palabra de control restauradas, cortafuegos contra bucles). Mostró diferencias reales en el punto de operación del juego (24 bits, redondeo cercano) que ningún banco anterior había visto: `GetRandomNumberInRange<float>` (un resultado menor que 2^-126 se redondeaba a subnormal de float en vez de conservar el exponente extendido), `ConvertDataToGameUnits` (127 a 143 casos de 1024: `accel`, la velocidad y 1/masa se redondeaban a float en cada paso y el EXE los lleva sin redondear) y, a 53 bits, `GetRadianAngleBetweenPoints` (`float(...)` frente a ST0 sin guardar). Se corrigieron con ayudantes `double` enlazados como `float` y con `HandlingConvertGameUnitsCpp`; además se corrigieron por análisis `Ftol2RegsCpp` (usa el redondeo vigente), `PedGeomShotAngleCpp`, `GeneralATanOfXYDblCpp`, `RwPlCoreFastRealToUInt32Cpp`, `PedGroupPlacerCircleCpp` y `TrainProjectionCpp`, y las rutinas que tocan la palabra de control usan `__control87_2` sin tocar MXCSR. Resultado: exacto a 24 y 53 bits y con los cuatro redondeos en las 14 funciones; el banco aprueba. Lo que no se puede cerrar: a 64 bits MSVC no tiene un tipo de 80 bits y un valor que cruza una llamada o una variable `double` pierde bits, así que hay diferencias exploratorias a 64 bits (1408 en `GetATanOfXY`, 1002 en `GetRadianAngleBetweenPoints`, 7416 en `GetAngleBetweenPoints`, 476 en `SolveQuadratic`, 124 en `GetLocalDirection`). El juego no corre a 64 bits: el proceso fija 24 bits.
- **`GetLocalDirection` con -inf.** No es un límite de la conversión: el bucle «sumar 2π mientras sea negativo» no termina con -inf ni con valores de ±2^27 o más en el EXE tampoco, y la conversión reproduce ese comportamiento a propósito. El banco de precisión acota esas entradas (y conserva NaN) con un cortafuegos de predicción y un vigilante para no colgar el juego.

'''
t = t[:a0] + seg + t[a0:]
a0 = t.index('## Límites')
a1 = t.index('## Objetos')
t = t[:a0] + '''## Límites

- A 64 bits de precisión de la x87 hay diferencias exploratorias en cinco ayudantes que pasan un `double` por la pila entre dos operaciones del EXE (cifras arriba); cerrarlas exigiría un tipo de 80 bits que MSVC no tiene. El proceso del juego corre a 24 bits.
- Hay unas veinticinco cadenas de `float` bajo `float_control(precise)` (`FightCtl*`, `ProcSurface*`, `ProjInfo*`, `BoneNode*`...) que el análisis marcó como sospechosas a 53 y 64 bits sin demostrar; el banco de precisión no las cubre.
- `GetLocalDirection` no termina con rotaciones de ±2^27 o más ni con +inf, igual que el EXE.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.
- Fallos transitorios de arranque del juego (`0xC0000409` sin ninguna comparación) en `BoneNodes` y `World16`, que aprobaron al repetir; la causa no se determinó.

''' + t[a1:]
rp.write_text(t, encoding='utf-8')
