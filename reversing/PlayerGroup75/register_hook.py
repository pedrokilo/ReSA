"""Registra CPlayerPed::MakeThisPedJoinOurGroup (EXE 0x0060C840, hook Simple) con la corrida APROBADA de PlayerGroup75.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-playergroup75/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/PlayerGroup75/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv, si falta, la anade el coordinador:
"CPlayerPed,MakeThisPedJoinOurGroup,0x0060C840,1,0,1,S").
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'PlayerGroup75'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CPlayerPed::MakeThisPedJoinOurGroup', '0x0060C840', 'CPlayerPed', 'MakeThisPedJoinOurGroup', 'source/game_sa/Entity/Ped/PlayerPed.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wy-4 (HEAD 98ff78aaa + PlayerPed.cpp, source.patch). CPlayerPed::MakeThisPedJoinOurGroup 0x60C840 (thiscall, ret 4; sin valor de retorno: sus llamadores 0x60D411 y 0x60D4D5 recargan los registros): '
              'salida si el peaton va drogado (Say 0x52), tiene la tarea 1000, no es del tipo 8 sin banderas globales 0x96917C..E o ya es miembro; si no, SetCJMood, Say 0x7F, limite del grupo (FindMaxNumberOfGroupMembers, limite del script, estadistica 0xB5), '
              'recuentos de miembros, CEventDontJoinPlayerGroup si no hay sitio, o CTaskComplexBeInGroup por CEventScriptCommand, AddFollower, CPedGroup::Process, GiveWeaponWhenJoiningGang, CEventGroupEvent(CEventNewGangMember), estadisticas, blip del radar y separacion del grupo. '
              'Sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL): Say, FindActiveTaskByType, IsMember, FindMaxNumberOfGroupMembers, GetStatValue, CountMembersExcludingLeader, GetPedsGroup, RemoveMember, '
              'CTask::operator new/delete, FindPlayerPed, CEventGroup::Add, AddFollower, CPedGroup::Process, GiveWeaponWhenJoiningGang, RegisterReference/CleanUpOldReference, CPedGroupIntelligence::AddEvent, IncrementStat, DisplayScriptStatUpdateMessage, '
              'CPedPool::GetRef y CRadar (SetEntityBlip, ChangeBlipScale, ChangeBlipColour, SetBlipFriendly) (con hook); sin hook: SetCJMood 0x4E3ED0 (--unhook=Audio), 0x609380 (sin reconstruir) y CEvent::operator new/delete. '
              'Constructores y destructores de CTaskComplexBeInGroup, CEventScriptCommand, CEventNewGangMember, CEventGroupEvent y CEventDontJoinPlayerGroup corren de verdad en ambos lados. Dominio: indices de grupo 0..7.')
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
                       'scope': ('Reconstruida en C++ (CPlayerPed, reclutar un peatón en el grupo del jugador): ' + FN + ' sin llamadas al EXE salvo la función 0x609380 sin reconstruir. '
                                 'El banco sustituye por falsos idénticos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL) los colaboradores con hook en HEAD y compara la secuencia de llamadas con sus argumentos, el objeto reservado '
                                 'los cambios en las arenas del caso y el estado de la x87. Banco PlayerGroup75 con ' + str(int(s['pairs'])) + ' pares (' + str(int(ppf[ID])) + ' propios) y cero diferencias; '
                                 'EAX/ECX/EDX de salida: ver README del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
