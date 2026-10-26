"""Registra CTaskComplexGoPickUpEntity::CreateNextSubTask (virtual, slot 8 de la vtable 0x870B98, EXE 0x00691AE0; hook VIRTUAL) con la corrida APROBADA de PickUpNext68.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-pickupnext68/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/PickUpNext68/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv, si falta, la anade el coordinador:
"CTaskComplexGoPickUpEntity,CreateNextSubTask,0x00000000,1,0,1,V").
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'PickUpNext68'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CTaskComplexGoPickUpEntity::CreateNextSubTask', '0x00691AE0', 'CTaskComplexGoPickUpEntity', 'CreateNextSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp', 'V'),
)
# Los hooks virtuales se registran con direccion 0x00000000 y tipo V (docs/hooks.csv: "CTaskComplexGoPickUpEntity,CreateNextSubTask,0x00000000,1,0,1,V"); la direccion real va en evidence.original_address.
SCOPE_NOTE = ('Validado en el worktree D:/Original/wy-1 (HEAD 8ff5230a6 + TaskComplexGoPickUpEntity.cpp, source.patch). CTaskComplexGoPickUpEntity::CreateNextSubTask 0x691AE0 (virtual, thiscall, ret 4; EAX = tarea nueva o nulo): '
              'segun el tipo de la subtarea actual crea CTaskSimpleHoldEntity (secundaria, tras CTaskSimplePickUpEntity), CTaskSimplePickUpEntity (tras CTaskSimpleAchieveHeading) o CTaskSimpleAchieveHeading '
              '(tras CTaskComplexGoToPointAndStandStill). Sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL): CTask::operator new 0x61A5A0, CEntity::GetColModel 0x535300, '
              'CTaskManager::GetTaskSecondary 0x681810 y SetTaskSecondary 0x681B60, constructores de CTaskSimpleHoldEntity 0x6913A0 y CTaskSimplePickUpEntity 0x6917B0 (su cuerpo en la DLL se halla por la primera llamada de la envoltura del hook). '
              'El constructor de CTaskSimpleAchieveHeading 0x667E20 (sin hook) corre real en ambos lados y se compara el objeto. Fuera de cobertura: la rama de CTaskSimplePickUpEntity con el pool agotado (el EXE escribe en la direccion 0x40 y se cae; la DLL devuelve nulo).')
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
    for ID, ADDR, CLS, FN, SOURCE, HTYPE in FUNCS:
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000' if HTYPE == 'V' else ADDR, 'type': HTYPE},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256'], 'original_address': ADDR},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ (CTaskComplexGoPickUpEntity, tarea de ir a recoger una entidad): ' + FN + ' sin llamadas al EXE para esta función. '
                                 'El banco sustituye por falsos idénticos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL) los colaboradores con hook en HEAD y compara la secuencia de llamadas con sus argumentos, el objeto reservado '
                                 'construido por el constructor real de CTaskSimpleAchieveHeading, los cambios en las arenas del caso y el retorno. Banco PickUpNext68 con ' + str(int(s['pairs'])) + ' pares (' + str(int(ppf[ID])) + ' propios) y cero diferencias; '
                                 'EAX/ECX/EDX de salida: ver README del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
