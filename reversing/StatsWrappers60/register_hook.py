"""Registra CStats::FindLeastFavoriteRadioStation (0x00559010), BuildStatLine (0x00559230), UpdateRespectStat (0x0055BC50) y ConstructStatLine (0x0055A780) con la corrida APROBADA de
StatsWrappers60.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-statswrappers60/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: es la de FileLoaderLoad2dEffect27/register_hook.py adaptada a cuatro funciones. Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe
reversing/StatsWrappers60/validation-summary.json y anade las entradas a reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/
(las filas `CStats,<funcion>,<direccion>,1,0,1,S` de docs/hooks.csv, si faltan, las anade el coordinador).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores sustituidos y fuentes probadas).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'StatsWrappers60'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CStats::FindLeastFavoriteRadioStation', '0x00559010', 'CStats', 'FindLeastFavoriteRadioStation', 'source/game_sa/Stats.cpp'),
    ('CStats::BuildStatLine', '0x00559230', 'CStats', 'BuildStatLine', 'source/game_sa/Stats.cpp'),
    ('CStats::UpdateRespectStat', '0x0055BC50', 'CStats', 'UpdateRespectStat', 'source/game_sa/Stats.cpp'),
    ('CStats::ConstructStatLine', '0x0055A780', 'CStats', 'ConstructStatLine', 'source/game_sa/Stats.cpp'),
)
SCOPE_NOTE = ('Validado en el arbol de integracion D:/Original/wi (HEAD 6e01296af + fuentes del lote 60 de los worktrees wc-1..wc-6). Colaboradores sustituidos por falsos identicos en ambos lados '
              '(23 entradas; no se validan sus cuerpos reales ni el formato real '
              'del sprintf del EXE, que el falso sustituye por una representacion fija): GetStatValue, CLocalisation::Metric / GermanGame, PopulateFavoriteRadioStationList, FindMost/FindLeast, '
              'GetPercentageProgress, ConvertToMins/Secs, BuildStatLine (dentro de ConstructStatLine), CText::Get, GxtCharToAscii (0x69F7E0), GxtCharStrcpy, 0x719240, AsciiToGxtChar, sprintf del EXE '
              '(0x821BB5), GetRadioStationNameKey, FindPlayerPed, CPed::GetWeaponSkill, GetPercentageTagged, CheckForThreshold, DisplayScriptStatUpdateMessage y CheckForStatsMessage. '
              'EAX/ECX/EDX solo registrados (los llamadores del EXE de FindLeast y ConstructStatLine leen solo EAX).')
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
                       'scope': ('Reconstruida en C++ (formato con el sprintf del EXE por dirección, 0x821BB5; aritmética x87 de 24 bits en X87Math) y colaboradores del juego sustituidos por falsos '
                                 'trazados idénticos en ambos lados. Banco StatsWrappers60 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX sólo registrados. '
                                 'Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
