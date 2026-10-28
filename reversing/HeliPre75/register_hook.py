"""Registra CHeli::PreRender (0x006C5420, lote 75) con la corrida APROBADA de HeliPre75.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-heliblow74/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/HeliPre75/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv
`CHeli,PreRender,0x00000000,1,0,1,V` la anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'HeliPre75'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CHeli::PreRender', '0x006C5420', 'CHeli', 'PreRender', 'source/game_sa/Entity/Vehicle/Heli.cpp'),
)
SCOPE_NOTE = 'Fuentes probadas compiladas desde el worktree D:/Original/wy-1 (HEAD 98ff78aaa + cambios del lote 75; ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con sus hooks RH apagados) salvo que se indique lo contrario; el banco compara el orden y todos los argumentos de los falsos, los bytes cambiados de las arenas y los globales. Hook VIRTUAL (fila de docs/hooks.csv: CHeli,PreRender,0x00000000,1,0,1,V (ranura 17 de la vtable 0x871680); la direccion real va en evidence.original_address). Colaboradores: CVehicle::PreRender 0x6D6480 (virtual; CVehD, 256 pares), CClock::GetIsTimeInRange 0x52CEE0 (Core9, 512), operator*(CMatrix, CVector) 0x59C890 (Helpers65D, 4096), CHeli::AddHeliSearchLight 0x6C45B0 (Mix11, 256), CEntity::GetColModel 0x535300 (CEntA, 256), CVehicleModelInfo::GetWheelPosn 0x4C7D20 (VehModelInfoFinal4, 256), CAutomobile::UpdateWheelMatrix 0x6AA290 (AutomobileWrappers60, 4096), CShadows::StoreShadowForVehicle 0x70BDA0 (ShadowVehicle355, 4096), CAutomobile::DoHeliDustEffect 0x6B0690 y CMatrix::Attach 0x59BD10 / SetRotateZ 0x59B0E0 / SetRotateX 0x59B060 / UpdateRW 0x59BBB0 (sin hook RH ni banco propio; la DLL llama a sus cuerpos). La llamada virtual a la ranura 52 (CAutomobile::DoBurstAndSoftGroundRatios, AutomobileFinal4, 256) pasa por una vtable propia del banco con un falso. ~CMatrix 0x59ACD0 no se sustituye. La matriz del helicoptero nula no se prueba (el EXE la pasa a 0x59C890, que aqui es un falso). Void: EAX/ECX/EDX solo se registran.'
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
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000', 'type': 'V'},
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE para esta funcion (virtual, ranura 17 de la vtable 0x871680; los colaboradores son falsos identicos en ambos lados): PreRender de CVehicle, foco de busqueda nocturno, relaciones de suspension de las cuatro ruedas, matrices de rueda, polvo del rotor, giro de los dos rotores, colocacion de las cuatro paletas y sombra.' + ' Banco HeliPre75 con ' + str(int(s['pairs'])) + ' pares y cero diferencias. Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
