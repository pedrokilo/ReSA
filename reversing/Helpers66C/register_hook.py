"""Registra AEMFOpenSoundMedia (0x004E7990), AEMFGetMediaSoundFormat (0x004E7A80), GetDXVersion (0x00745650), RwTextureGtaStreamRead (0x00730E60) y SkinGetBonePositionsToTable (0x00735360)
con la corrida APROBADA de Helpers66C.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers66c/<corrida>"

PLANTILLA: la de Helpers65D adaptada a cinco hooks Simple. Idempotente. NO toca docs/ (las filas de docs/hooks.csv, si faltan, las anade el coordinador).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Helpers66C'
FUNCS = (
    # (id del banco, direccion del EXE, clase del hook, nombre del hook, fuente, tipo de hook)
    ('AEMFOpenSoundMedia', '0x004E7990', 'CAEMFDecoder', 'OpenSoundMedia', 'source/game_sa/Audio/Loaders/AEMFDecoder.cpp', 'S'),
    ('AEMFGetMediaSoundFormat', '0x004E7A80', 'CAEMFDecoder', 'GetMediaSoundFormat', 'source/game_sa/Audio/Loaders/AEMFDecoder.cpp', 'S'),
    ('GetDXVersion', '0x00745650', 'Ps', 'GetDXVersion', 'source/app/platform/win/WinPs.cpp', 'S'),
    ('RwTextureGtaStreamRead', '0x00730E60', 'RwHelper', 'RwTextureGtaStreamRead', 'source/game_sa/RenderWare/rw/rwtexdict.cpp', 'S'),
    ('SkinGetBonePositionsToTable', '0x00735360', 'RwHelper', 'SkinGetBonePositionsToTable', 'source/game_sa/RwHelper.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wu-3 (HEAD 5765c0b5f). AEMFOpenSoundMedia 0x4E7990 y AEMFGetMediaSoundFormat 0x4E7A80: las 7 y 11 funciones de pegamento QuickTime del EXE (0x818xxx-0x81DAA0) '
              'sustituidas por falsos identicos en ambos lados (nunca se abre un medio real); el segundo se llama con ECX = la media, como su unico llamador (0x4E7CB0), porque el EXE usa ese ECX de entrada '
              'como handle de extension sin inicializar en la ruta de error; los 16 bits altos de refNum (basura de pila del EXE) se enmascaran. GetDXVersion 0x745650: LoadLibraryA, GetProcAddress, '
              'FreeLibrary y OutputDebugStringA sustituidos en las ranuras de importacion del EXE y de la DLL; DirectDrawCreateEx y el Release del objeto son falsos. RwTextureGtaStreamRead 0x730E60: '
              'RwStreamFindChunk, CTimer (2), RpAnisot (3) y la ranura stdFunc[NATIVETEXTUREREAD] de RwEngineInstance sustituidos; gGameState, contadores 0xC87FEC/0xC87FF0, anisotropia 0xC87FFC y calidad '
              'g_fx+0x54 se escriben por caso y se restauran; la media incremental en x87 de 24 bits se compara en bits. SkinGetBonePositionsToTable 0x735360: RpClumpForAllAtomics, RpSkinGeometryGetSkin, '
              'RpSkinGetNumBones, RpSkinGetSkinToBoneMatrices, RwMatrixInvert y RwV3dTransformPoints sustituidos; sin POP sin PUSH previo y con profundidad de pila <= 24 (el EXE usa una pila de 32 enteros '
              'sin inicializar). Los llamadores del EXE solo leen AX (QuickTime) o EAX (GetDXVersion, RwTextureGtaStreamRead); nadie lee ECX/EDX de salida.')
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
