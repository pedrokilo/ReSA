"""Registra CPedIntelligence::IsInHearingRange (0x00600C00), CPedIntelligence::HasInterestingEntites (0x00602080), CPedIntelligence::RemoveAllInterestingEntities (0x00602320)
y CPedStats::FindIndexWithPedStat (0x00608940) con la corrida APROBADA de PedLeaf69.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-pedleaf69/<corrida>"

PLANTILLA: la de Helpers66A adaptada a cuatro hooks Simple. Idempotente. NO toca docs/ (las filas de docs/hooks.csv, si faltan, las anade el coordinador).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'PedLeaf69'
FUNCS = (
    # (id del banco, direccion del EXE, clase del hook, nombre del hook, fuente, tipo de hook)
    ('CPedIntelligence::IsInHearingRange', '0x00600C00', 'CPedIntelligence', 'IsInHearingRange', 'source/game_sa/PedIntelligence.cpp', 'S'),
    ('CPedIntelligence::HasInterestingEntites', '0x00602080', 'CPedIntelligence', 'HasInterestingEntites', 'source/game_sa/PedIntelligence.cpp', 'S'),
    ('CPedIntelligence::RemoveAllInterestingEntities', '0x00602320', 'CPedIntelligence', 'RemoveAllInterestingEntities', 'source/game_sa/PedIntelligence.cpp', 'S'),
    ('CPedStats::FindIndexWithPedStat', '0x00608940', 'CPedStats', 'FindIndexWithPedStat', 'source/game_sa/PedStats.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wy-1 (HEAD 9a8eb46fa + source.patch). Las cuatro funciones no tienen llamadores en el EXE (ni call, ni jmp ni punteros), por lo que solo se compara el valor de retorno: '
              'AL de IsInHearingRange 0x600C00 y HasInterestingEntites 0x602080, EAX completo de FindIndexWithPedStat 0x608940 (pasa de void a int32); EAX/ECX/EDX de salida restantes solo se registran. '
              'IsInHearingRange: rango^2 > (dz^2 + dx^2) + dy^2 en x87 a 24 bits con exponente extendido (helper PedHearingRangeCpp de X87Math.cpp); con NaN da falso. '
              'RemoveAllInterestingEntities 0x602320: CEntity::CleanUpOldReference 0x571A00 (con hook) sustituido por un falso identico en el EXE y en el cuerpo de la DLL, que registra entidad, ranura y valor de la ranura; '
              'la ranura se pone a cero despues de cada llamada. FindIndexWithPedStat: ms_apPedStats (0xC0BBEC) apuntado a una arena de 44 entradas, nulo o a un valor bruto durante la llamada y restaurado.')
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
                                 'en ambos lados). Banco PedLeaf69 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; ECX/EDX solo registrados y EAX comparado donde un llamador lo lee. '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
