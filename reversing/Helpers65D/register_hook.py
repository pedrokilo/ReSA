"""Registra operator*(CMatrix, CVector) (0x0059C890), CTaskComplexWalkRoundObject::CreateRouteTask (0x00655140), RwTexDictionaryGtaStreamRead (0x00730FC0) y FightStrikeDamageExe (0x0061C740)
con la corrida APROBADA de Helpers65D.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers65d/<corrida>"

PLANTILLA: la de CarCtrlRest63B adaptada a cuatro hooks Simple. Idempotente. NO toca docs/ (las filas de docs/hooks.csv, si faltan, las anade el coordinador).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Helpers65D'
FUNCS = (
    # (id del banco, direccion del EXE, clase del hook, nombre del hook, fuente, tipo de hook)
    ('operator*(CMatrix,CVector)', '0x0059C890', 'CMatrix', 'operator*-Vec', 'source/game_sa/Core/Matrix.cpp', 'S'),
    ('CTaskComplexWalkRoundObject::CreateRouteTask', '0x00655140', 'CTaskComplexWalkRoundObject', 'CreateRouteTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp', 'S'),
    ('RwTexDictionaryGtaStreamRead', '0x00730FC0', 'RwHelper', 'RwTexDictionaryGtaStreamRead', 'source/game_sa/RenderWare/rw/rwtexdict.cpp', 'S'),
    ('FightStrikeDamageExe', '0x0061C740', 'CTaskSimpleFight', 'FightStrikeDamage', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wr-4 (HEAD 633dceef0). operator*(CMatrix,CVector) 0x59C890: aritmetica x87 pura (AutoPreMultiplyMatrixWithVectorCpp), sin colaboradores; se comparan EAX (= destino), el destino, la matriz y el punto, '
              'con destino distinto y solapado con el punto o con la matriz. CreateRouteTask 0x655140: CTask::operator new 0x61A5A0 y CPointRoute::operator new 0x41B7C0 sustituidos por falsos identicos; el constructor de '
              'CTaskComplexFollowPointRoute (0x671510) corre de verdad en ambos lados y se compara byte a byte el objeto construido y la ruta copiada. RwTexDictionaryGtaStreamRead 0x730FC0: los siete colaboradores RenderWare '
              '(RwStreamFindChunk 0x7ED2D0, RwStreamRead 0x7EC9D0, RwTexDictionaryCreate 0x7F3600, RwTextureGtaStreamRead 0x730E60, RwTexDictionaryAddTexture 0x7F3980, ForAllTextures 0x7F3730, Destroy 0x7F36A0) sustituidos por falsos; '
              'no se prueban longitudes de chunk 0 ni 1 (el EXE usa bytes viejos de su pila). FightStrikeDamageExe 0x61C740: CPed::IsPlayer 0x5DF8F0 y CStats::GetFatAndMuscleModifier 0x559AF0 sustituidos; la tabla de combos '
              '0xC170D0 se sobrescribe con bytes del caso y se restaura; el resultado float de ST0 se compara en el estado x87. Ningun llamador del EXE lee EAX/ECX/EDX de salida salvo EAX como valor de retorno (matriz * punto, '
              'CreateRouteTask, RwTexDictionaryGtaStreamRead), que el banco compara.')
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
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR, 'type': KIND},
                evidence=evidence, tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ sin llamadas al EXE para la propia funcion (los colaboradores sin reconstruir se llaman por direccion y el banco los sustituye por falsos identicos '
                                 'en ambos lados). Banco Helpers65D con ' + str(int(s['pairs'])) + ' pares y cero diferencias; ECX/EDX solo registrados y EAX comparado como valor de retorno. '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
