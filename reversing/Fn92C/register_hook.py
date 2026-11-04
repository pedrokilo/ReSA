"""Registra la funcion del lote 92C (CAttractorScanner::ScanForAttractors) con la corrida APROBADA de Fn92C.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn92c/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn92C/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn92C'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CAttractorScanner::ScanForAttractors', '0x006060A0', 'CAttractorScanner', 'ScanForAttractors', 'source/game_sa/AttractorScanner.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en un worktree sobre main e154848ab (lote 92B) con los cambios del lote (ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos en la direccion del EXE y, cuando tienen hook RH en la DLL (apagado durante el banco), en su cuerpo del ASI: '
              'CTaskManager::GetActiveTask 0x681720, CEventGroup::Add 0x4AB420 (devuelve un valor del caso), CEntity::RegisterReference 0x571B70 y CleanUpOldReference 0x571A00; sin hook (la DLL los llama por su direccion del EXE): 0x6034B0 (explorador de la lista de un sector), 0x5FFFD0 (explorador de un efecto), '
              '0x5FE960 (la lista de usuarios admite el tipo de ped), 0x600180 (mejor candidato) y 0x603570 (usuario del efecto). Registran sus argumentos en orden y, en Add, los campos definidos del evento (tipo por GetEventType virtual). '
              'NO se sustituyen y corren de verdad en ambos lados: el temporizador en linea, CAttractorScanner::Clear 0x5FFF90 (hook activo), CTask::GetTaskType sobre una arena con vtabla propia, los constructores y destructores de CEventAttractor (0x4AF350, hook activo) y CEventScriptedAttractor frente al destructor del EXE 0x4AF400, '
              'y toda la aritmetica x87 del EXE frente al C++ de la DLL, con CTimer::m_snTimeInMilliseconds (0xB7CB84), CScripted2dEffects::ms_activated (0xC3A1A0) y ms_userLists (0xC3A200) como memoria real del juego restaurada tras cada lado. '
              'AttractorScanner.cpp se compila completo con /O2 /Oi /arch:IA32 /fp:fast (por Clear); el banco comprueba la funcion nueva compilada asi. El banco compara los eventos de los falsos con todos sus argumentos, los bytes cambiados de todas las arenas y los punteros +0x10/+0x14 del escaner como etiquetas de arena; '
              'cada lado coincide ademas con el oraculo independiente fixture_oracle.py (aritmetica racional exacta a 24 bits). '
              'Fuera de los fixtures: ped sin matriz y sin placement valido, mas de 16 efectos activados a la vez, bucles de sectores mayores de 256 eventos, NaN con cargas distintas de 0xFFC00000, NaN senalizantes, tareas con GetTaskType que escriben y valores de retorno de los falsos.')
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
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR, 'type': 'S'},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn92C con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
