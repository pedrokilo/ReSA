"""Registra las cinco funciones del lote 65C (Fx_c::TriggerWaterHydrant/TriggerTankFire/AddTyreBurst, CProjectile::Constructor, CTaskSimpleCarDrive::FinishAnimCloseDoorRollingCB) con la corrida APROBADA de Helpers65C.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers65c/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Helpers65C/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv `Fx_c,TriggerWaterHydrant,0x004A0D70,1,0,1,S`, `Fx_c,TriggerTankFire,0x004A0FA0,...,S`, `Fx_c,AddTyreBurst,0x0049F300,...,S`,
`CProjectile,Constructor,0x005A4030,...,S` y `CTaskSimpleCarDrive,FinishAnimCloseDoorRollingCB,0x0063C840,...,S` las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaborador sustituido y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Helpers65C'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('Fx_c::TriggerWaterHydrant', '0x004A0D70', 'Fx_c', 'TriggerWaterHydrant', 'source/game_sa/Fx/Fx.cpp'),
    ('Fx_c::TriggerTankFire', '0x004A0FA0', 'Fx_c', 'TriggerTankFire', 'source/game_sa/Fx/Fx.cpp'),
    ('Fx_c::AddTyreBurst', '0x0049F300', 'Fx_c', 'AddTyreBurst', 'source/game_sa/Fx/Fx.cpp'),
    ('CProjectile::CProjectile', '0x005A4030', 'CProjectile', 'Constructor', 'source/game_sa/Entity/Object/Projectile.cpp'),
    ('CTaskSimpleCarDrive::FinishAnimCloseDoorRollingCB', '0x0063C840', 'CTaskSimpleCarDrive', 'FinishAnimCloseDoorRollingCB', 'source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas desde el worktree D:/Original/wr-3 (HEAD 633dceef0 + source.patch del lote 65C; ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos '
              'identicos en la direccion del EXE y en el cuerpo de la DLL: FxManager_c::CreateFxSystem(name,CVector) 0x4A9BE0, FxSystem_c::PlayAndKill 0x4AA3D0, FxManager_c::FxRwMatrixCreate/Destroy '
              '0x4A9440/0x4A9460, Fx_c::CreateMatFromVec 0x49E950, FxSystem_c::CopyParentMatrix 0x4AA890, FxSystem_c::AddParticle(v3d) 0x4AA440, CObject::CObject() 0x5A1D10, CObject::SetIsStatic '
              '0x5A0760, CEntity::SetModelIndex 0x532AE0, CVehicle::ClearGettingOutFlags 0x6D3060, CVehicleAnimGroup::GetGroup 0x6E3B00 y la ranura 28 de la vtable del vehiculo (ProcessOpenDoor, '
              'falso en una vtable de arena); el banco compara la traza completa de esas llamadas, el objeto/matriz/tarea resultante y el resto de la arena. Ningun llamador del EXE lee EAX/ECX/EDX '
              'de las funciones Fx ni del callback (solo registrados); los tres llamadores de CProjectile::CProjectile leen EAX = this (exigido aparte). El vptr del constructor es el de la '
              'vtable de la DLL (como CObject/CPed). El callback es cdecl (el llamador retira los 8 bytes de argumentos). Fx.cpp se compila con las opciones SSE por defecto del proyecto: la '
              'distancia a la camara y el filtro `> 625.0` coinciden con el x87 del EXE salvo en el exponente extendido, que solo afecta a magnitudes que ya superan 625.')
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE por la función (ver README del banco; los colaboradores se llaman en C++ y se sustituyen por falsos en el banco). Banco Helpers65C con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX sólo registrados (ningún llamador del EXE los lee). Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
