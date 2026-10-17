"""Compuertas independientes del banco Dependencies21G: GetFirstTexture (0x734940), RpGeometryReplaceOldMaterialWithNewMaterial (0x734DE0) y CQuaternion::Slerp (0x59C300).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, colaboradores de ReplaceOld sustituidos en ambos lados
(stubs trazados), todas las compuertas de ABI/x87/ruta, la fixture registrada igual a la del oraculo, el estado final de cada arena igual al del oraculo independiente
(fixture_oracle.py), la cobertura de las fixtures de estres y la restauracion final. No relaja ninguna compuerta: kCompareDeadX87Registers debe ser true.
"""
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

FT, RP, SL = 'RwHelper::GetFirstTexture', 'RwHelper::RpGeometryReplaceOldMaterialWithNewMaterial', 'CQuaternion::Slerp'
EXPECTED = {FT: oracle.FIRST_TEXTURE_CASES, RP: oracle.REPLACE_CASES, SL: oracle.SLERP_CASES}
GATES = ('fp_ip_route_ok', 'stack_restored', 'nonvolatile_preserved', 'route_ok', 'globals_restored', 'floating_state_restored')
GUARD, GUARD_BYTE = 16, 0xA5


def guarded(arena):
    return (bytes([GUARD_BYTE]) * GUARD + bytes(arena) + bytes([GUARD_BYTE]) * GUARD).hex()


def slerp_expected_state(f, copy_path):
    """Arena de 80 bytes: guarda(16) | from(16) | to(16) | destino propio(16, 0x6B) | guarda(16). Rama de copia: el destino recibe los bits de `to`."""
    buf = bytearray(bytes([GUARD_BYTE]) * 80)
    base = 16
    buf[base:base + 16] = struct.pack('<4I', *f['src'])
    buf[base + 16:base + 32] = struct.pack('<4I', *f['dst'])
    buf[base + 32:base + 48] = bytes([0x6B]) * 16
    target = {0: base + 32, 1: base, 2: base + 16}[f['alias']]
    return buf, target


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados o sin colaboradores sustituidos')
    if begin[0].get('compare_dead_x87_registers') is not True:
        raise RuntimeError('Compuerta relajada: los registros x87 vacios deben compararse enteros')
    if begin[0]['cases'] != {'GetFirstTexture': oracle.FIRST_TEXTURE_CASES, 'Replace': oracle.REPLACE_CASES, 'Slerp': oracle.SLERP_CASES}:
        raise RuntimeError('Numero de casos inesperado')
    if begin[0]['pairs'] != sum(EXPECTED.values()):
        raise RuntimeError('Numero de pares inesperado')
    for name, body in begin[0]['bodies'].items():
        if not body:
            raise RuntimeError('Cuerpo RH sin resolver: ' + name)
    seen = {FT: set(), RP: set(), SL: set()}
    coverage = {'ft_null': 0, 'ft_nonnull': 0, 'rp_replaced': 0, 'rp_not_replaced': 0, 'rp_absent_written': 0, 'rp_destroy_last': 0, 'rp_wrap': 0,
                'rp_fmo': 0, 'sl_copy': 0, 'sl_math': 0, 'sl_nan_theta': 0, 'sl_alias1': 0, 'sl_alias2': 0, 'sl_inf_theta': 0, 'sl_denorm_theta': 0}
    residue_differs = 0
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if case in seen[fn]:
            raise RuntimeError('Caso repetido %s %d' % (fn, case))
        seen[fn].add(case)
        for side in ('original', 'candidate'):
            result = row[side]
            if result is None or not all(result.get(k) is True for k in GATES):
                raise RuntimeError('Resultado nulo o compuerta fallida en %s %d (%s)' % (fn, case, side))
        a, b = row['original'], row['candidate']
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        if row['original_residue'] != row['candidate_residue']:
            residue_differs += 1   # informativo: ningun llamador del EXE lee ECX/EDX tras estas llamadas (ver classification.md)
        if fn == FT:
            f = oracle.first_texture_fixture(case)
            if a['input'] != {k: f[k] for k in ('case', 'count', 'pad', 'order', 'junk', 'prev')}:
                raise RuntimeError('Fixture distinta de la del oraculo en %s %d' % (fn, case))
            base = a['base']
            if a['state'] != guarded(oracle.build_first_texture(base, f)):
                raise RuntimeError('El arena final de GetFirstTexture no es el inicial (la funcion no escribe nada) en %d' % case)
            expected = oracle.expected_first_texture(f)
            if a['expected_offset'] != expected or a['eax_offset'] != expected or b['eax_offset'] != expected:
                raise RuntimeError('Primera textura distinta de la del oraculo en %d: %s / %s' % (case, a['eax_offset'], expected))
            coverage['ft_null' if expected < 0 else 'ft_nonnull'] += 1
        elif fn == RP:
            f = oracle.replace_fixture(case)
            want = {'case': case, 'meshes': f['meshes'], 'materials': f['materials'], 'old': f['old'], 'new': f['new'], 'absent': f['absent'], 'first_mesh_offset': f['first_mesh_offset'],
                    'serial': f['serial'], 'mesh_mat': f['mesh_mat'], 'list_mat': f['list_mat'], 'ref_counts': f['ref_counts']}
            if a['input'] != want:
                raise RuntimeError('Fixture distinta de la del oraculo en %s %d' % (fn, case))
            arena, replaced, trace = oracle.oracle_replace(a['base'], f)
            if a['state'] != guarded(arena) or b['state'] != a['state']:
                raise RuntimeError('Memoria final distinta de la del oraculo en %d' % case)
            if a['returned'] != int(replaced) or b['returned'] != int(replaced):
                raise RuntimeError('Retorno distinto del oraculo en %d' % case)
            if a['trace'] != trace or b['trace'] != trace:
                raise RuntimeError('Traza de colaboradores distinta de la del oraculo en %d: %s / %s' % (case, a['trace'], trace))
            coverage['rp_replaced' if replaced else 'rp_not_replaced'] += 1
            if replaced and f['absent']:
                coverage['rp_absent_written'] += 1
            if replaced and f['ref_counts'][f['old']] == 1:
                coverage['rp_destroy_last'] += 1
            if replaced and f['ref_counts'][f['new']] in (0x7FFF, 0xFFFF):
                coverage['rp_wrap'] += 1
            if replaced and f['first_mesh_offset']:
                coverage['rp_fmo'] += 1
        else:
            f = oracle.slerp_fixture(case)
            want = {'case': case, 'theta': f['theta'], 't': f['t'], 'inv': f['inv'], 'quat': f['quat'], 'alias': f['alias'], 'from': f['src'], 'to': f['dst']}
            if a['input'] != want:
                raise RuntimeError('Fixture distinta de la del oraculo en %s %d' % (fn, case))
            path = oracle.slerp_path(case)
            if a['path'] != path:
                raise RuntimeError('Rama distinta de la del oraculo en %d' % case)
            expected, target = slerp_expected_state(f, path == 'copy')
            got = bytes.fromhex(a['state'])
            if path == 'copy':
                expected[target:target + 16] = struct.pack('<4I', *f['dst'])
                if got != bytes(expected):
                    raise RuntimeError('La rama de copia no dejo `to` en el destino en %d' % case)
                coverage['sl_copy'] += 1
            else:
                # sin oraculo de valores para el calculo x87: se exige que ni las guardas ni las regiones que no son destino cambien
                mask = bytearray(b'\xff' * 80)
                mask[target:target + 16] = b'\x00' * 16
                if any((g ^ e) & m for g, e, m in zip(got, expected, mask)):
                    raise RuntimeError('Memoria fuera del destino modificada en %d' % case)
                coverage['sl_math'] += 1
            if f['alias'] == 1:
                coverage['sl_alias1'] += 1
            if f['alias'] == 2:
                coverage['sl_alias2'] += 1
            th = f['theta']
            if (th & 0x7F800000) == 0x7F800000 and (th & 0x007FFFFF):
                coverage['sl_nan_theta'] += 1
            elif (th & 0x7FFFFFFF) == 0x7F800000:
                coverage['sl_inf_theta'] += 1
            elif 0 < (th & 0x7FFFFFFF) < 0x00800000:
                coverage['sl_denorm_theta'] += 1
    for fn in EXPECTED:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta de ' + fn)
    declared = {'replace': oracle.coverage('replace'), 'first_texture': oracle.coverage('first_texture')}
    minimum = {'ft_null': 100, 'ft_nonnull': 800, 'rp_replaced': 1000, 'rp_not_replaced': 300, 'rp_absent_written': 150, 'rp_destroy_last': 100, 'rp_wrap': 250, 'rp_fmo': 250,
               'sl_copy': 100, 'sl_math': 3500, 'sl_nan_theta': 400, 'sl_alias1': 1000, 'sl_alias2': 1000, 'sl_inf_theta': 250, 'sl_denorm_theta': 250}
    short = {k: (coverage[k], v) for k, v in minimum.items() if coverage[k] < v}
    if short:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(short))
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != sum(EXPECTED.values()) or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    summary['coverage'] = coverage
    summary['declared_fixture_coverage'] = declared
    summary['residue_pairs_where_ecx_edx_differ'] = residue_differs
    summary['compare_dead_x87_registers'] = True
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies21g', manifest['binaries']['asi'], analyze, 'Dependencies21G', timeout=1800))
