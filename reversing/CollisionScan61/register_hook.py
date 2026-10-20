"""Registra CCollisionEventScanner::ScanForCollisionEvents (0x00604500) con la corrida APROBADA de CollisionScan61.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-collisionscan61/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: la de EventScanners61/register_hook.py adaptada a una funcion. Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe
reversing/CollisionScan61/validation-summary.json y anade las entradas a reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/
(la fila `CCollisionEventScanner,ScanForCollisionEvents,0x00604500,1,0,1,S` de docs/hooks.csv, si falta, la anade el coordinador).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores sustituidos y fuentes probadas).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'CollisionScan61'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CCollisionEventScanner::ScanForCollisionEvents', '0x00604500', 'CCollisionEventScanner', 'ScanForCollisionEvents', 'source/game_sa/Collision/CollisionEventScanner.cpp'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wd-3 (HEAD d349867fb + estos cambios; ver source.patch). Colaboradores sustituidos por falsos identicos en ambos lados y comparados en orden y argumentos: '
              'CEventGroup::Add, CTaskManager::GetSimplestActiveTask, CTask::IsGoToTask, CPedIntelligence::GetMoveStateFromGoToTask/IncrementAngerAtPlayer, CEntity::GetColModel/RegisterReference/CleanUpOldReference, '
              'CPed::IsPlayer/KillPedWithCar/Say/GetLocalDirection, CPlaceable::GetHeading, CGeneral::LimitRadianAngle, CVector::Normalise (por V*0.5), CEventDamage::AffectsPed (volcado del evento), '
              'CPedDamageResponseCalculator::ComputeDamageResponse, CPedGeometryAnalyser::ComputePedHitSide, CAcquaintance::GetAcquaintances, CPedType::GetPedFlag, CPedClothesDesc::GetIsWearingBalaclava, '
              'CWeapon::GenerateDamageEvent. Reales en ambos lados: constructores/destructores de eventos y del calculador, CEntity::GetIsStatic/GetIsTypePhysical, GetEventGlobalGroup y la aritmetica de vectores '
              '(el EXE ejecuta 0x406DA0/0x40FE60/0x40FE90/0x40FDB0/0x4119D0/0x59C890 y la DLL las operaciones equivalentes en C++). No se validan los cuerpos reales de los colaboradores sustituidos. '
              'Los fixtures evitan valores cuyos productos salen del rango de float (el x87 conserva exponente extendido) y denormales. EAX/ECX/EDX solo registrados: el unico llamador del EXE '
              '(CPedIntelligence::Process, 0x6073D7) no los lee.')
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE para esta función (los escaneres sin reconstruir se llaman por dirección del EXE) y colaboradores sustituidos por falsos '
                                 'trazados idénticos en ambos lados. Banco EventScanners61 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX sólo registrados. '
                                 'Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
