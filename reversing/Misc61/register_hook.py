"""Registra CMemoryHeap::IntegrityCheck (0x0072E8E0), CSearchLight::IsSpottedEntity (0x00493900), CTaskSimpleSlideToCoord::ProcessPed (0x0066C4E0, hook VIRTUAL) y
CTrainNode::GetLightingFromCollision (0x006F5F80) con la corrida APROBADA de Misc61.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-misc61/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: es la de StatsWrappers60/PartnerRemove60 adaptada a cuatro funciones (tres hooks Simple y uno Virtual). Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe
reversing/Misc61/validation-summary.json y anade las entradas a reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/
(las filas de docs/hooks.csv, si faltan, las anade el coordinador): `CMemoryHeap,IntegrityCheck,0x0072E8E0,1,0,1,S`, `CSearchLight,IsSpottedEntity,0x00493900,1,0,1,S`,
`CTaskSimpleSlideToCoord,ProcessPed,0x00000000,1,0,1,V` (virtual: direccion 0x00000000 y tipo V; la direccion real va en evidence.original_address) y `CTrainNode,GetLightingFromCollision,0x006F5F80,1,0,1,S`.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Misc61'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CMemoryHeap::IntegrityCheck', '0x0072E8E0', 'CMemoryHeap', 'IntegrityCheck', 'source/game_sa/Memory/MemoryHeap.cpp', 'S'),
    ('CSearchLight::IsSpottedEntity', '0x00493900', 'CSearchLight', 'IsSpottedEntity', 'source/game_sa/SearchLight.cpp', 'S'),
    ('CTaskSimpleSlideToCoord::ProcessPed', '0x0066C4E0', 'CTaskSimpleSlideToCoord', 'ProcessPed', 'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp', 'V'),
    ('CTrainNode::GetLightingFromCollision', '0x006F5F80', 'CTrainNode', 'GetLightingFromCollision', 'source/game_sa/TrainNode.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wd-4 (HEAD d349867fb + fuentes del lote 61). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL; '
              'no se validan sus cuerpos reales): GetActualScriptThingIndex (0x4839A0) y 0x493280 (sin reconstruir) en IsSpottedEntity; CTaskSimpleRunNamedAnim::ProcessPed (0x61BF20), '
              'CTaskSimpleStandStill::ProcessPed (0x62F370) y CGeneral::LimitRadianAngle (0x53CB50) en ProcessPed (el constructor/destructor de CTaskSimpleStandStill, CPed::IsPlayer y la llamada virtual '
              'MakeAbortable de la tarea primaria se ejecutan con codigo real o con una vtable falsa trazada); CWorld::ProcessVerticalLine (0x5674E0) en GetLightingFromCollision. '
              'CMemoryHeap::IntegrityCheck se prueba sobre un monton falso con cada estructura en su propia pagina PAGE_GUARD (orden de primera lectura); memId fuera de [-1, 19] en un bloque en uso '
              'no se prueba (el EXE escribe fuera del arreglo local). GetLightingFromCollision: el hook apunta a un envoltorio con la ABI del EXE (AL) porque MSVC devuelve tColLighting por puntero oculto. '
              'ProcessPed se prueba con CPed/CMatrix/CPedIntelligence falsos (solo los campos que leen el EXE y la DLL). EAX/ECX/EDX solo registrados; el AL de salida si se compara.')
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def main(src):
    SRC = Path(src)
    RUN = ROOT / 'reversing/CppTwelveLevel6Final/runs' / (BANK + '-' + SRC.name)
    RUN.mkdir(exist_ok=True)
    for n in ('summary.json', 'RESULTADO.txt'):
        if not (RUN / n).exists():
            shutil.copyfile(SRC / n, RUN / n)
    if not (RUN / 'comparisons.jsonl.gz').exists():
        with open(SRC / 'comparisons.jsonl', 'rb') as f, gzip.GzipFile(RUN / 'comparisons.jsonl.gz', 'wb', mtime=0) as g:
            shutil.copyfileobj(f, g)
    s = json.loads((RUN / 'summary.json').read_text(encoding='utf-8'))
    assert s['passed'] is True and s['all_equal'] is True and s['state_restored'] is True
    ppf = dict(s['pairs_per_function'])
    assert set(ppf) == {f[0] for f in FUNCS}, ppf
    tested = json.loads((ROOT / 'reversing' / BANK / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))['sources']
    scope = SCOPE + (' ' + SCOPE_NOTE if SCOPE_NOTE else '')
    ev = dict(passed=True, pairs=int(s['pairs']), differences=0, registered_pairs=int(s['pairs']), registered_functions=len(ppf), functions=ppf,
              covered_functions={k: True for k in ppf}, scope=scope, dependency_review=None, run=RUN.relative_to(ROOT).as_posix(),
              summary_sha256=sha(RUN / 'summary.json'), result_txt_sha256=sha(RUN / 'RESULTADO.txt'), log_sha256=s['log_sha256'], all_equal=True,
              state_restored=True, saves_unchanged=s['saves_unchanged'], configs_restored=s['configs_restored'],
              original_exe_sha256=s['original_exe_sha256'], executed_probe_sha256=s['executed_probe_sha256'], manifest=dict(tested_sources=tested))
    out = ROOT / 'reversing' / BANK / 'validation-summary.json'
    out.write_bytes((json.dumps(ev, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))

    rp = ROOT / 'reversing/validation-registry.json'
    data = json.loads(rp.read_bytes().decode('utf-8'))
    ap = ROOT / 'reversing/CppOnlyMigration/approvals.json'
    a = json.loads(ap.read_bytes().decode('utf-8'))
    for ID, ADDR, CLS, FN, SOURCE, KIND in FUNCS:
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            evidence = {'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                        'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']}
            if KIND == 'V':
                evidence['original_address'] = ADDR   # los hooks virtuales se registran con direccion 0x00000000 y tipo V
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR if KIND == 'S' else '0x00000000', 'type': KIND},
                evidence=evidence, tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ sin llamadas al EXE para la propia funcion (los colaboradores sin reconstruir se llaman por direccion y el banco los sustituye por falsos identicos '
                                 'en ambos lados). Banco Misc61 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX solo registrados (el AL de bool/tColLighting si se compara). '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
