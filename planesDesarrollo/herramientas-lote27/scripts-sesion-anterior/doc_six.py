import json, os, pathlib

R = 'reversing/CppFiveFilesBUnified/runs'
S = 'reversing/CppFiveFilesBStudy/README.md'


def last(bank):
    return sorted(d for d in os.listdir(R) if d.startswith(bank + '-'))[-1]


def pairs(bank):
    return json.load(open(f'{R}/{last(bank)}/summary.json', encoding='utf-8'))['pairs']


def ref(bank):
    d = last(bank)
    return d.split('-', 1)[1], f'{R}\\{d}/summary.json'


a = json.load(open('reversing/CppOnlyMigration/approvals.json', encoding='utf-8'))
tail = (' Revalidación del banco histórico completo sobre la fuente nueva (sin script de auditoría propio: la evidencia '
        'es el resumen y el registro de la corrida). Sin cobertura exhaustiva de x87 más allá de los fixtures del banco.')


def entry(addr, ident, n, src, bank, own, scope):
    r, s = ref(bank)
    a[addr] = {
        'identity': ident, 'converted_source_functions': n, 'source': src,
        'shared_source': 'source/game_sa/X87Math.cpp', 'bank': bank, 'run': r, 'pairs': pairs(bank),
        'own_pairs': own, 'audit': s, 'differences': 0, 'null_comparison_records': 0, 'state_restored': True,
        'helper_codegen': S, 'scope': scope + tail,
    }


G = 'source/game_sa/'
entry('0x00555E40', 'CRopes::FindCoorsAlongRope', 1, G + 'Ropes.cpp', 'Misc61a', 768,
      'RopesInterpolateCpp bajo float_control(precise): t = dist·31 - segmento (fisub), t·pb.x sin redondear y productos guardados como float donde el EXE hace fstp/fld. Para que Misc61a aprobara se interceptó la entrada de la DLL de CFormation::* (antes fallaba igual en HEAD en CPedGroup::PlayerGaveCommand_Gather, 512/512).')
entry('0x00739C30', 'CShotInfo::AddShot (tiempo de destrucción)', 1, G + 'ShotInfo.cpp', 'Fx43', 1024,
      'ShotInfoDestroyTimeCpp: float(_ftol(vida + tiempo sin signo)) con fild + 2^32 y un redondeo por operación.')
entry('0x00739E60', 'CShotInfo::Update (amortiguado)', 1, G + 'ShotInfo.cpp', 'Fx43', 512,
      'PowScaleVector3Cpp(v, 0.96f, paso) con ExeCompatiblePowCpp (el factor de _CIpow sin redondear antes de multiplicar x, y, z).')
entry('0x0054EF40', 'CSimpleTransform::UpdateRwMatrix', 1, G + 'SimpleTransform.cpp', 'CEntA', 512,
      'TrigSimpleSinCpp / TrigSimpleCosCpp (fld; fsin; fstp) y -sin tras redondear. No tiene banco propio: se ejerce con el cuerpo real vía CEntity::UpdateRwMatrix (512 pares en CEntA).')
entry('0x0054EF90', 'CSimpleTransform::Invert', 1, G + 'SimpleTransform.cpp', 'CEntA', 1024,
      'SimpleTransformInvertCpp: x = -(cos·bx) - sin·by escrito antes de releer base.x (alias del EXE) e y = sin·bx - cos·by, con cos/sin sin redondear. Se ejerce vía CEntity::GetIsBoundingBoxOnScreen (1024 pares en CEntA).')
entry('0x0070D490', 'CSprite::CalcSpriteQuadCoeffs (RenderOneXLUSprite_Rotate_Aspect y otras dos)', 1, G + 'Sprite.cpp', 'World15', 1024,
      'SpriteQuadCoeffsCpp: {-cos - sin, sin - cos, cos + sin, cos - sin} con fsin/fcos sin redondear; mismo ayudante para 0x70D490, 0x70E780 y 0x70EDE0.')
entry('0x0070EAB0', 'CSprite::CalcSpriteDimensionTerms (RenderBufferedOneXLUSprite_Rotate_Dimension)', 1, G + 'Sprite.cpp', 'World15', 1024,
      'SpriteDimensionTermsCpp: {w·cos, sin·h, cos·h, sin·w} con fsin/fcos sin redondear y un redondeo a float por producto.')
entry('0x006946F0', 'CEntitySeekPosCalculatorRadiusAngleOffset::ComputeEntitySeekPos', 1,
      G + 'Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp', 'Misc56', 1024,
      'TrigSimpleSinCpp / TrigSimpleCosCpp / TrigSimpleSinMulCpp (ayudantes ya existentes).')
json.dump(a, open('reversing/CppOnlyMigration/approvals.json', 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)


def parse(p):
    d = {}
    for l in pathlib.Path(p).read_text().splitlines():
        h, n = l.split(' *')
        d[n.split('/')[-1]] = h
    return d


b = parse('reversing/CppFiveFilesBStudy/artifacts/before.sha256')
af = parse('reversing/CppFiveFilesBStudy/artifacts/after.sha256')
obj = '\n'.join(f'| {n} | {h} |' for n, h in sorted({**b, **af}.items()))
txt = f'''# Ropes, ShotInfo, SimpleTransform, Sprite y EntitySeekPosCalculatorRadiusAngleOffset sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el `__asm` de `Ropes.cpp`, `ShotInfo.cpp`, `SimpleTransform.cpp`, `Sprite.cpp` y `EntitySeekPosCalculatorRadiusAngleOffset.cpp` (10 apariciones, 8 funciones fuente). El análisis de cada archivo lo preparó un subagente (desensamblado del EXE y parche); la compilación y los bancos corrieron en serie. Ningún banco se relajó.

## Conversiones

| Función | Cómo |
| --- | --- |
| `CRopes::FindCoorsAlongRope` (0x555E40) | `RopesInterpolateCpp` (nuevo, `float_control(precise)`) |
| `CShotInfo::AddShot` (0x739C30) | `ShotInfoDestroyTimeCpp` (nuevo) |
| `CShotInfo::Update` (0x739E60) | `PowScaleVector3Cpp` (ya existía) |
| `CSimpleTransform::UpdateRwMatrix` (0x54EF40) | `TrigSimpleSinCpp` / `TrigSimpleCosCpp` (ya existían) |
| `CSimpleTransform::Invert` (0x54EF90) | `SimpleTransformInvertCpp` (nuevo; escribe x antes de releer base.x como el EXE) |
| Quad de sprites (0x70D490, 0x70E780, 0x70EDE0) | `SpriteQuadCoeffsCpp` (nuevo) |
| `RenderBufferedOneXLUSprite_Rotate_Dimension` (0x70EAB0) | `SpriteDimensionTermsCpp` (nuevo) |
| `CEntitySeekPosCalculatorRadiusAngleOffset::ComputeEntitySeekPos` (0x6946F0) | `TrigSimpleSinCpp`, `TrigSimpleCosCpp`, `TrigSimpleSinMulCpp` (ya existían) |

## Bancos (cero diferencias)

| Banco | Pares | Cubre |
| --- | ---: | --- |
| Misc61a | 20992 | FindCoorsAlongRope (768) |
| Fx43 | 15552 | AddShot (1024) y Update (512) |
| CEntA | 16384 | Invert vía GetIsBoundingBoxOnScreen (1024) y UpdateRwMatrix vía CEntity::UpdateRwMatrix (512), con el cuerpo real |
| World15 | 17312 | las cuatro funciones de sprites (1024 cada una) |
| Misc56 | 6464 | ComputeEntitySeekPos (1024) |

`CSimpleTransform` no tiene banco propio: CEntA ejerce `Invert` y `UpdateRwMatrix` con su cuerpo real a través de `CEntity`, y esa cobertura es indirecta.

## Lo que reveló la validación

- **Misc61a fallaba en HEAD** en `CPedGroup::PlayerGaveCommand_Gather` (512/512), ajena al lote: el banco solo interceptaba las direcciones del EXE de `CFormation::GenerateGatherDestinations`, `GenerateGatherDestinations_AroundCar`, `DistributeDestinations` y `ReturnDestinationForPed`, que `PedGroup.cpp` llama directo. Se añadieron los `EntryPatch` de las entradas de la DLL con los mismos Fakes en `reversing/Misc61a/LiveComparison.hpp` y el banco aprobó con 20992 pares. La corrida fallida queda en `CppFiveFilesBUnified/runs/Misc61a-20261014-190137-536820`.
- Los otros cuatro bancos aprobaron a la primera.

## Límites

Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos (denormales, desbordes). Los colaboradores no se validan por probar a sus consumidores. No se revisó el código máquina de los ayudantes nuevos.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
{obj}
'''
pathlib.Path(S).write_text(txt, encoding='utf-8')

p = pathlib.Path('reversing/CppOnlyMigration/README.md')
t = p.read_text(encoding='utf-8')
ins = ("Se eliminó el ensamblador de `Ropes.cpp`, `ShotInfo.cpp`, `SimpleTransform.cpp`, `Sprite.cpp` y `EntitySeekPosCalculatorRadiusAngleOffset.cpp` con cinco ayudantes nuevos en `X87Math` "
       "(`RopesInterpolateCpp`, `ShotInfoDestroyTimeCpp`, `SimpleTransformInvertCpp`, `SpriteQuadCoeffsCpp`, `SpriteDimensionTermsCpp`); "
       "bancos Misc61a, Fx43, CEntA, World15 y Misc56 sin diferencias, con la interceptación de las entradas de la DLL de `CFormation::*` añadida a Misc61a "
       "([detalle y hashes](../CppFiveFilesBStudy/README.md)).\n\n")
i = t.index('Se eliminó el ensamblador de `Fire.cpp`')
j = t.index('\n\n', i) + 2
p.write_text(t[:j] + ins + t[j:], encoding='utf-8')
