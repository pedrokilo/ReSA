"""Registra las cinco funciones del lote 120A (metodos virtuales diminutos, hook V) con la corrida APROBADA de Fn120A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn120a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn120A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn120A'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente); hooks virtuales: se registran con direccion 0x00000000 y tipo V (docs/hooks.csv); la direccion real va en evidence.original_address
    ('CAEPedlessSpeechAudioEntity::AddScriptSayEvent', '0x004E6080', 'CAEPedlessSpeechAudioEntity', 'AddScriptSayEvent', 'source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp'),
    ('CTaskComplexGangJoinRespond::GetTaskType', '0x00661710', 'CTaskComplexGangJoinRespond', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp'),
    ('CTaskComplexKillPedOnFoot::GetTaskType', '0x00620EE0', 'CTaskComplexKillPedOnFoot', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp'),
    ('CTaskComplexPartner::GetTaskType', '0x00681F10', 'CTaskComplexPartner', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp'),
    ('CTaskComplexPartnerDeal::GetTaskType', '0x00681DF0', 'CTaskComplexPartnerDeal', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en el worktree wv-1 sobre main 5bf6a9cd00 con los cambios del lote (ver artifacts/build-manifest.json). Sin colaboradores: las cinco funciones no llaman a nada ni leen memoria; '
              'cada caso se ejecuta por la entrada del EXE o por la ranura de la vtable del EXE (hook virtual) con un objeto sintetico en una arena propia; cada lado coincide ademas con el oraculo independiente fixture_oracle.py. '
              'Fuera de los fixtures: objetos reales construidos por el EXE (la vtable de cada clase solo cambia de ranura al enganchar el hook; ninguna de las cinco lee el objeto) y la identidad de las clases mas alla de la vtable que contiene cada direccion.')
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

    data = registry_store.load_registry(ROOT)
    ap = ROOT / 'reversing/CppOnlyMigration/approvals.json'
    a = json.loads(ap.read_bytes().decode('utf-8'))
    for ID, ADDR, CLS, FN, SOURCE in FUNCS:
        entry = dict(
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000', 'type': 'V'},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256'], 'original_address': ADDR},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md')
        # las entradas existentes se REEMPLAZAN en su sitio; las nuevas se anaden al final
        idx = [i for i, r in enumerate(data['functions']) if r['hook']['class'] == CLS and r['hook']['fn_name'] == FN]
        assert len(idx) <= 1, (CLS, FN, idx)
        if idx:
            data['functions'][idx[0]] = entry
        else:
            data['functions'].append(entry)
        if True:
            a[ADDR] = {'identity': CLS + '::' + FN, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ (metodo virtual diminuto sin colaboradores). Banco Fn120A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
