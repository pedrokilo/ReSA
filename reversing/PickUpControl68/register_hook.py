"""Registra CTaskComplexGoPickUpEntity::ControlSubTask (0x00691D50, hook VIRTUAL: vtable 0x870B98, ranura 10) con la corrida APROBADA de PickUpControl68.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-pickupcontrol68/<corrida>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/PickUpControl68/validation-summary.json y anade las entradas a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv, si falta, la anade el coordinador:
virtual con direccion 0x00000000 y tipo V: "CTaskComplexGoPickUpEntity,ControlSubTask,0x00000000,1,0,1,V"). La direccion real va en la clave de approvals.json.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'PickUpControl68'
FUNCS = (
    # (id del banco, direccion del EXE, clase del hook, nombre del hook, fuente, tipo de hook)
    ('CTaskComplexGoPickUpEntity::ControlSubTask', '0x00691D50', 'CTaskComplexGoPickUpEntity', 'ControlSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp', 'V'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wy-3 (HEAD 8ff5230a6 + TaskComplexGoPickUpEntity.cpp/.h, source.patch). CTaskComplexGoPickUpEntity::ControlSubTask 0x691D50 (virtual, ranura 10 de la '
              'vtable 0x870B98, thiscall, ret 4; EAX = subtarea o nulo si la aborta): CAnimManager::GetAnimBlockName 0x4D3A30, GetAnimationBlock(const char*) 0x4D3940 y AddAnimBlockRef 0x4D3FB0, '
              'CStreaming::RequestModel 0x4087E0, CPlayerPed::GetPadFromPlayer 0x609560, CPad::GetPedWalkUpDown 0x53FD30, GetPedWalkLeftRight 0x53FC90, JumpJustDown 0x540770 y SprintJustDown 0x5407F0 '
              'y CGeneral::GetRadianAngleBetweenPoints 0x53CBE0 sustituidos por falsos identicos en ambos lados; GetTaskType y MakeAbortable de la subtarea van por una vtable falsa de arena '
              '(ranuras 4 y 6). ms_aAnimAssocGroups (0xB4EA34), CTimer::m_snTimeInMilliseconds (0xB7CB84) y la orientacion de la camara (0xB6F178) se fijan por caso y se restauran. '
              'El atan2 del rumbo (fpatan) es el ayudante TrigAtan2Cpp de X87Math.cpp y NO se sustituye: se valida contra el fpatan del EXE. Los plazos 5000 (0x8D2FF8) y 8000 (0x8D2FFC) son '
              'constantes en C++. CreateNextSubTask 0x691AE0 y CreateFirstSubTask 0x693610 siguen en el EXE.')
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
                                 'en ambos lados). Banco PickUpControl68 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; ECX/EDX solo registrados y EAX comparado donde un llamador lo lee. '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
