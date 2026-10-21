"""Registra GetMillisecondTime (0x005617C0), DIReleaseMouse (0x00746F70), RemoveUpgradeCB (0x006D3300) y FindUpgradeCB (0x006D3370) con la corrida APROBADA de SmallHelpers64A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-smallhelpers64a/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: es la de WorldScan64 adaptada a cuatro hooks Simple. Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/SmallHelpers64A/validation-summary.json y anade las
entradas a reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv, si faltan, las anade el coordinador).
Filas de hooks.csv (clase segun el RH_ScopedClass donde se registra cada hook; ver el README): CTimer,GetMillisecondTime,0x005617C0 / CPad,DIReleaseMouse,0x00746F70 /
CVehicle,RemoveUpgradeCB,0x006D3300 / CVehicle,FindUpgradeCB,0x006D3370, todas tipo S.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'SmallHelpers64A'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('GetMillisecondTime', '0x005617C0', 'CTimer', 'GetMillisecondTime', 'source/game_sa/Timer.cpp', 'S'),
    ('DIReleaseMouse', '0x00746F70', 'CPad', 'DIReleaseMouse', 'source/game_sa/Pad.cpp', 'S'),
    ('RemoveUpgradeCB', '0x006D3300', 'CVehicle', 'RemoveUpgradeCB', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'S'),
    ('FindUpgradeCB', '0x006D3370', 'CVehicle', 'FindUpgradeCB', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wp-1 (HEAD 1b13a6db4 + fuente del lote 64: Timer.cpp, Pad.cpp/h, Vehicle.cpp). Colaboradores sustituidos por falsos identicos en ambos lados '
              '(parche en la entrada del EXE y, si la DLL tiene hook, en el cuerpo de la DLL; no se validan sus cuerpos): RsTimer (0x619410); GetAtomicId (0x732370), GetModelInfo (0x732260), '
              'RpClumpRemoveAtomic (0x74A4C0), RpAtomicDestroy (0x749DC0), RwFrameDestroy (0x7F05A0) y CBaseModelInfo::RemoveRef (0x4C4BB0). DIReleaseMouse usa un dispositivo DirectInput FALSO '
              '(objeto COM con tabla virtual propia) y estructuras psGlobal falsas apuntadas por RsGlobal.ps solo durante la llamada: el dispositivo real y su psGlobal no se tocan. '
              'Se exigen EAX/EDX de GetMillisecondTime (CTimer::Suspend/Resume los leen) y EAX de los callbacks (RpClumpForAllAtomics hace test eax,eax); el resto de registros solo se registra. '
              'No se prueban los caminos en que el EXE desreferencia sin comprobar nulo (GetModelInfo nulo en la primera llamada, dispositivo nulo tras Unacquire).')
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
                                 'en ambos lados). Banco SmallHelpers64A con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX solo registrados (EAX de los HRESULT si se compara). '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
