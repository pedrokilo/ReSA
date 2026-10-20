"""Compuertas independientes del banco PedEntityCollision60 (CPed::ProcessEntityCollision 0x5E2530, hook VIRTUAL), 6144 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: el hook virtual registrado y con cuerpo en el ASI, todos los colaboradores sustituidos con cuerpo en la DLL, las compuertas de
ABI/ruta/DF/SEH/restauracion en AMBOS lados (route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato), fp_difference vacio, globales intactos, ningun desbordamiento de la traza,
la cobertura de estres MEDIDA sobre lo que el EXE original hizo (traza de colaboradores y cambios de bytes de la propia ejecucion, no sobre la intencion del generador) y la restauracion final.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent

NAME = 'CPed::ProcessEntityCollision'
CASES = 6144
EXPECTED = {NAME: CASES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'globals_restored', 'floating_state_restored')
ENTRY = 0x5E2530
# colaboradores sustituidos por el mismo falso en ambos lados (orden de Stubs::All en Core.hpp)
STUBBED = [0x600F70, 0x441DB0, 0x43E0C0, 0x54F560, 0x54F1B0, 0x535300, 0x4185C0, 0x571B70, 0x571A00, 0x55E770, 0x55E690, 0x4D68B0, 0x5DEF60, 0x4B35A0, 0x4B5AC0, 0x4AB420, 0x543490, 0x59B020]
WATCHED = set(STUBBED) | {ENTRY}
EXE_REAL = sorted([0x59C910, 0x40FC80])
# tipos de evento (mismo orden que el enum de Core.hpp)
K_TASK, K_GETHEAD, K_SETHEAD, K_ALLOC, K_UPDMAT, K_GETCOL, K_PCM, K_REGREF, K_CLEANREF, K_STEEP, K_SOFT, K_ASSOC, K_LOCALDIR, K_AFFECTS, K_COMPUTE, K_ADD, K_ADDREC, K_ROTZ = range(1, 19)
L_PED, L_ENT, L_MATPED, L_MATENT, L_CP, L_MI, L_PCM, L_TCM = 1, 2, 3, 4, 5, 6, 7, 8   # etiquetas de arena; en `changes` el indice de arena es etiqueta - 1

# Minimos de cobertura: el 45 % de lo medido en la corrida APROBADA 20261019-221749-500084 (6144 pares), con margen.
MINIMUMS = {
    'addrec_ent': 1759,
    'addrec_none': 539,
    'addrec_ped': 2225,
    'affects': 503,
    'alloc': 421,
    'assoc': 1123,
    'blocked_duck_arm': 152,
    'blocked_left_arm': 155,
    'blocked_midriff': 154,
    'blocked_right_arm': 153,
    'colmodel_ped': 2051,
    'colmodel_temp': 450,
    'compute': 405,
    'cp_changed': 1854,
    'damage_500': 110,
    'damage_fall15': 41,
    'early_return': 262,
    'ent_building': 571,
    'ent_changed': 421,
    'ent_object': 630,
    'ent_other': 514,
    'ent_ped': 337,
    'ent_vehicle': 709,
    'extra_point': 171,
    'filter_removed': 294,
    'group_add': 405,
    'heading_get': 63,
    'heading_set': 63,
    'hit_wall': 508,
    'localdir': 418,
    'pcm1': 2440,
    'pcm2': 61,
    'pcm_flag': 218,
    'pcm_lines': 2502,
    'pcm_nolines': 61,
    'pcm_tempcm': 450,
    'ped_bright': 838,
    'ped_contact_surface': 912,
    'ped_f1C': 1211,
    'ped_f46C': 738,
    'ped_f470': 791,
    'ped_f474': 182,
    'ped_f56C': 402,
    'ped_f578': 918,
    'ped_f588': 91,
    'ped_pos_z': 409,
    'ped_speed_z': 962,
    'regref_contact': 918,
    'regref_standing': 402,
    'ret0': 972,
    'ret_ge16': 425,
    'ret_gt0': 1791,
    'rotz': 60,
    'soft': 450,
    'steep': 1091,
    'steep_multi': 232,
    'task': 161,
    'task3': 86,
    'train_candidate': 43,
    'updmat': 421,
}


def coverage_of(row):
    """Claves de cobertura (todas medidas sobre la ejecucion del EXE original: traza de falsos + cambios de bytes + retorno)."""
    obs = row['original']['observed']
    ev = obs['events']
    kinds = [e[0] for e in ev]
    ch = obs['changes']
    ped_off = {c[1] for c in ch if c[0] == L_PED - 1}
    ret = obs['ret']
    keys = {}

    def has(k):
        return k in kinds
    keys['ret0'] = ret == 0
    keys['ret_gt0'] = 0 < ret < 0x80000000
    keys['ret_ge16'] = 16 <= ret <= 33
    keys['early_return'] = len(kinds) == 0 and ret == 0
    pcm = [e for e in ev if e[0] == K_PCM]
    keys['pcm1'] = len(pcm) == 1
    keys['pcm2'] = len(pcm) == 2
    keys['pcm_lines'] = any(e[6] == 1 and e[7] == 1 for e in pcm)
    keys['pcm_nolines'] = any(e[6] == 0 for e in pcm)
    keys['pcm_flag'] = any(e[8] == 1 for e in pcm)
    keys['pcm_tempcm'] = any(e[2] == L_TCM for e in pcm)
    keys['alloc'] = has(K_ALLOC)
    keys['updmat'] = has(K_UPDMAT)
    keys['task'] = has(K_TASK)
    keys['task3'] = kinds.count(K_TASK) == 3
    keys['heading_set'] = has(K_SETHEAD)
    keys['heading_get'] = has(K_GETHEAD)
    keys['rotz'] = has(K_ROTZ)
    keys['regref_standing'] = any(e[0] == K_REGREF and e[2] == 0x100 + 0x568 for e in ev)
    keys['regref_contact'] = any(e[0] == K_REGREF and e[2] == 0x100 + 0x584 for e in ev)
    keys['steep'] = has(K_STEEP)
    keys['soft'] = has(K_SOFT)
    keys['assoc'] = has(K_ASSOC)
    keys['localdir'] = has(K_LOCALDIR)
    keys['affects'] = has(K_AFFECTS)
    keys['compute'] = has(K_COMPUTE)
    keys['group_add'] = has(K_ADD)
    keys['damage_fall15'] = any(e[0] == K_COMPUTE and e[2] == 0x41700000 for e in ev)
    keys['damage_500'] = any(e[0] == K_COMPUTE and e[2] == 0x43FA0000 for e in ev)
    keys['addrec_ped'] = any(e[0] == K_ADDREC and e[1] == L_PED for e in ev)
    keys['addrec_ent'] = any(e[0] == K_ADDREC and e[1] == L_ENT for e in ev)
    keys['addrec_none'] = not has(K_ADDREC)
    # cambios del ped y de otras arenas
    keys['ped_f588'] = any(0x588 <= o < 0x58C for o in ped_off)
    keys['ped_pos_z'] = any(c[0] == L_MATPED - 1 and 0x38 <= c[1] < 0x3C for c in ch)
    keys['ped_f470'] = any(0x470 <= o < 0x474 for o in ped_off)
    keys['ped_f474'] = any(0x474 <= o < 0x478 for o in ped_off)
    keys['ped_f46C'] = any(0x46C <= o < 0x470 for o in ped_off)
    keys['ped_f1C'] = any(0x1C <= o < 0x20 for o in ped_off)
    keys['ped_speed_z'] = any(0x4C <= o < 0x50 for o in ped_off)
    keys['ped_bright'] = any(0x12C <= o < 0x130 for o in ped_off)
    keys['ped_contact_surface'] = 0xBA in ped_off
    keys['ped_f578'] = any(0x578 <= o < 0x584 for o in ped_off)
    keys['ped_f56C'] = any(0x56C <= o < 0x578 for o in ped_off)
    ped_bytes = {c[1]: c[2] for c in ch if c[0] == L_PED - 1}
    b476 = ped_bytes.get(0x476, 0)
    keys['blocked_right_arm'] = bool(b476 & 1)
    keys['blocked_left_arm'] = bool(b476 & 2)
    keys['blocked_duck_arm'] = bool(b476 & 4)
    keys['blocked_midriff'] = bool(b476 & 8)
    keys['steep_multi'] = kinds.count(K_STEEP) >= 2
    keys['hit_wall'] = 0x1C + 1 in ped_off and bool(ped_bytes.get(0x1D, 0) & 0x10)   # bit 12 de +0x1C
    n1 = row['input']['pcm']['n1']
    keys['filter_removed'] = len(pcm) >= 1 and ret < n1 and len(pcm) == 1
    keys['extra_point'] = len(pcm) == 1 and ret > n1
    ent = row['input']['ent']
    keys['train_candidate'] = (ent['type'] & 7) == 2 and ent['sub'] == 6 and bool(ent['phys'] & 4) and ret > 0
    keys['ent_building'] = (ent['type'] & 7) == 1
    keys['ent_vehicle'] = (ent['type'] & 7) == 2
    keys['ent_ped'] = (ent['type'] & 7) == 3
    keys['ent_object'] = (ent['type'] & 7) == 4
    keys['ent_other'] = (ent['type'] & 7) in (0, 5, 6, 7)
    keys['cp_changed'] = any(c[0] == L_CP - 1 for c in ch)
    keys['ent_changed'] = any(c[0] == L_ENT - 1 for c in ch)
    keys['colmodel_ped'] = any(c[0] == L_PCM - 1 for c in ch)
    keys['colmodel_temp'] = any(c[0] == L_TCM - 1 for c in ch)
    return keys


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado, o sin los falsos de los colaboradores')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y ECX/EDX solo registrados')
    if b['pairs'] != CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != {NAME} or not all(b['registered'].values()) or set(b['bodies']) != {NAME} or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if not all(b['layout'].values()):
        raise RuntimeError('Disposicion en memoria viva distinta de la esperada: ' + json.dumps(b['layout']))
    stubbed = b['collaborators']['stubbed']
    if [s['exe'] for s in stubbed] != STUBBED or {w['address'] for w in b['watched_entries']} != WATCHED or sorted(b['collaborators']['exe_real']) != EXE_REAL:
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
    if any(not s['dll'] for s in stubbed):
        raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver')
    seen = set()
    cov = {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn != NAME or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
            if side['observed'].get('event_overflow') is not False or side['observed'].get('globals_unchanged') is not True:
                raise RuntimeError('Traza desbordada o globales alterados en %d' % case)
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        for key, value in coverage_of(row).items():
            if value:
                cov[key] = cov.get(key, 0) + 1
        if row['original_residue'] != row['candidate_residue']:
            cov['residue_differs'] = cov.get('residue_differs', 0) + 1
    if seen != set(range(CASES)):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {}
    for key, m in MINIMUMS.items():
        if cov.get(key, 0) < m:
            shortfalls[key] = (cov.get(key, 0), m)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['globals_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, el hook, las entradas vigiladas o los globales no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = {NAME: cov}
    summary['coverage_method'] = 'observed_trace_and_byte_changes_of_the_original_exe_run; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('pedentitycollision60', manifest['binaries']['asi'], analyze, 'PedEntityCollision60', timeout=1800))
