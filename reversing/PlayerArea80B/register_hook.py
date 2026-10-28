"""Registra CPlayerPed::KeepAreaAroundPlayerClear (EXE 0x0060C1E0, hook Simple) con la corrida APROBADA de PlayerArea80B.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-playerarea80b/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/PlayerArea80B/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv, si falta, la anade el coordinador:
"CPlayerPed,KeepAreaAroundPlayerClear,0x0060C1E0,1,0,1,S").
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'PlayerArea80B'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CPlayerPed::KeepAreaAroundPlayerClear', '0x0060C1E0', 'CPlayerPed', 'KeepAreaAroundPlayerClear', 'source/game_sa/Entity/Ped/PlayerPed.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wz-2 (HEAD e4f077aea + PlayerPed.cpp y X87Math.cpp/.h, source.patch). CPlayerPed::KeepAreaAroundPlayerClear 0x60C1E0 (thiscall sin argumentos, void; sus 4 llamadores del EXE no leen los registros de salida): '
              'recorre los 16 peatones del escaner (inteligencia leida una vez): los que no estan en vehiculo, estan vivos, no son del grupo del jugador y estan fuera de pantalla (o con la bandera 0x10) se marcan para destruir (virtual +0x54); '
              'los demas reciben un evento de guion con una tarea SmartFleeEntity del jugador salvo que ya huyan; despues, con los vehiculos a 15 m (FindObjectsInRange), les pone una accion temporal (espera, marcha atras o adelante) segun la distancia al cuadrado a 25.0 y el producto escalar con su eje y llama a 0x424F80. '
              'Sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL): CPed::IsAlive, CPedGroupMembership::IsMember, CEntity::GetIsOnScreen, CPedIntelligence::FindTaskByType, CEventGroup::GetEventOfType y Add, '
              'CTask::operator new/delete, CEntity::RegisterReference/CleanUpOldReference, CWorld::FindObjectsInRange (con hook); sin hook: 0x424F80 (la DLL la llama por la direccion del EXE). '
              'Dominio: tareas/eventos reales (constructores y destructores de CTaskComplexSmartFleeEntity y CEventScriptCommand) sobre arenas; no cubre fallo de reserva de la tarea (el EXE escribe en [0 + 0x30]), vehiculo sin matriz cerca de la referencia (el EXE se cae) ni listas con entradas nulas bajo la cuenta.')
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
    for ID, ADDR, CLS, FN, SOURCE, HTYPE in FUNCS:
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000' if HTYPE == 'V' else ADDR, 'type': HTYPE},
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
                       'scope': ('Reconstruida en C++ (CPlayerPed, despeje del area): ' + FN + ' sin llamadas al EXE salvo 0x424F80 (PossiblyRemoveVehicle) sin reconstruir. '
                                 'El banco sustituye por falsos idénticos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL) los colaboradores con hook en HEAD y compara la secuencia de llamadas con sus argumentos, el objeto reservado '
                                 'los cambios en las arenas del caso y el estado de la x87. Banco PlayerArea80B con ' + str(int(s['pairs'])) + ' pares (' + str(int(ppf[ID])) + ' propios) y cero diferencias; '
                                 'EAX/ECX/EDX de salida: ver README del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
