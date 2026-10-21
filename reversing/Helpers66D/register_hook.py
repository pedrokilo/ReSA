"""Registra las cuatro funciones del lote 66D (RwHelper::RpClumpGetBoundingSphere, CGarage::FindDoorsWithGarage, CCam::GetArrestCamCopSourceSide, CHeli::Constructor) con la corrida APROBADA de Helpers66D.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers66d/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Helpers66D/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv
`RwHelper,RpClumpGetBoundingSphere,0x00734FC0,1,0,1,S`, `CGarage,FindDoorsWithGarage,0x00449FF0,1,0,1,S`, `CCam,GetArrestCamCopSourceSide,0x00516010,1,0,1,S` y
`CHeli,Constructor,0x006C4190,1,0,1,S` las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores sustituidos y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Helpers66D'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('RwHelper::RpClumpGetBoundingSphere', '0x00734FC0', 'RwHelper', 'RpClumpGetBoundingSphere', 'source/game_sa/RwHelper.cpp'),
    ('CGarage::FindDoorsWithGarage', '0x00449FF0', 'CGarage', 'FindDoorsWithGarage', 'source/game_sa/Garage.cpp'),
    ('CCam::GetArrestCamCopSourceSide', '0x00516010', 'CCam', 'GetArrestCamCopSourceSide', 'source/game_sa/Cam.cpp'),
    ('CHeli::CHeli', '0x006C4190', 'CHeli', 'Constructor', 'source/game_sa/Entity/Vehicle/Heli.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas desde el worktree D:/Original/wu-4 (HEAD 5765c0b5f + source.patch del lote 66D; ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos '
              'identicos en la direccion del EXE y en el cuerpo de la DLL: RpClumpGetNumAtomics 0x7498E0, RpClumpForAllAtomics 0x749B70 (los callbacks 0x734970 y 0x734ED0 siguen siendo codigo del EXE '
              'y no se ejecutan: el falso escribe lo que dicta el caso), RwFrameGetLTM 0x7F0990, RwMatrixInvert 0x7F2070, RwV3dTransformPoints 0x7EDD90; CVector::Normalise 0x59C910, CrossProduct '
              '0x59C730, CWorld::FindGroundZFor3DCoord 0x5696C0; CAutomobile::CAutomobile 0x6B0A90, CDamageManager::SetDoorStatus 0x6C21C0 y el rand() del CRT del EXE 0x821B1E. El pool de objetos '
              '(0xB7449C), el byte bUseLTM (0x8D60BC) y el temporizador (0xB7CB84) se fijan por caso y se restauran. Ningun llamador del EXE lee ECX/EDX; EAX = this (constructor), EAX = clump '
              '(esfera) y AL (camara) se exigen. createdBy del constructor solo se prueba en 0..255 (eVehicleCreatedBy es uint8: el EXE reenvia el dword completo al constructor base).')
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE por la función salvo los dos callbacks de la esfera (0x734970, 0x734ED0, que siguen siendo código del EXE; ver README del banco); los colaboradores se llaman en C++ y se sustituyen por falsos en el banco. Banco Helpers66D con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX sólo registrados salvo EAX/AL que leen los llamadores. Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
