"""Register only independently verified, final production-hook engine runs."""
import argparse
import csv
import gzip
import hashlib
import json
import os
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
TARGETS = {
    'DependenciesPedGroups1': ('CPedGroups', 'GetPedsGroup', '0x005F7E80', 1024,
        'Ocho grupos y ocho miembros, todas las máscaras de activación; puntero nulo, ausente, presente y duplicado. '
        'Sólo compara punteros; no desreferencia peds. Retorno y ausencia de escrituras; todos los bytes de grupos y activación restaurados.'),
    'DependenciesStats1': ('CStats', 'GetFatAndMuscleModifier', '0x00559AF0', 4096,
        'Dieciséis capacidades y selectores fuera de rango, fronteras 400/800, estadísticas y configuración deterministas, '
        'NaN quietos e infinitos; retorno por bits, ECX/EDX y ESP preservados; tablas y control x87 restaurados. '
        'Max/Min son cuerpos locales reconstruidos, sin ejecutar los auxiliares originales.'),
    'DependenciesPointLights1': ('CPointLights', 'AddLight', '0x007000E0', 2560,
        'Cámara con matriz y placement, cuenta 0/1/31/32/33, radios finitos no negativos, límites de distancia/atenuación y NaN quietos; '
        'compara toda la lista, contador, relleno y guardas. Sin dibujar ni desreferenciar la entidad token; cámara y globales restaurados.'),
}


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('runs', type=Path, help='JSON mapping bank names to final archived run IDs')
    args = parser.parse_args()
    run_ids = json.loads(args.runs.read_text(encoding='utf-8'))
    assert set(run_ids) == set(TARGETS)
    registry = ROOT / 'reversing/validation-registry.json'
    registry_before = sha(registry)
    data = json.loads(registry.read_text(encoding='utf-8'))
    hooks_path = ROOT / 'docs/hooks.csv'
    with hooks_path.open(encoding='utf-8-sig', newline='') as stream:
        reader = csv.DictReader(stream)
        fields = reader.fieldnames
        hooks = list(reader)
    additions = []
    approved = {}
    for bank_name, (cls, fn, address, count, specific) in TARGETS.items():
        bank = ROOT / 'reversing' / bank_name
        run = bank / 'runs' / run_ids[bank_name]
        function = cls+'::'+fn
        audit = json.loads((run/'approval-audit.json').read_text(encoding='utf-8'))
        summary = json.loads((run/'summary.json').read_text(encoding='utf-8'))
        assert audit['registered_hook_verified'] is True and audit['differences'] == 0
        assert audit['null_comparison_records'] == 0 and audit['pairs'] == count
        assert summary['passed'] is True and summary['state_restored'] is True
        assert all(sha(ROOT/path) == digest for path, digest in audit['tested_sources'].items())
        assert all(sha(bank/path) == digest for path, digest in audit['bank_sources'].items())
        gz = run/'comparisons.jsonl.gz'
        with gzip.open(gz, 'rb') as stream:
            raw = stream.read()
        assert hashlib.sha256(raw).hexdigest() == summary['log_sha256'] == audit['log_sha256']
        records = [json.loads(line) for line in raw.decode('utf-8').splitlines()]
        pairs = [row for row in records if row.get('event') == 'comparison']
        assert len(pairs) == count and all(row['function']==function for row in pairs)
        assert all(row['original'] is not None and row['candidate'] is not None and
                   row['original']==row['candidate'] and row['equal'] is True and row['accepted'] is True for row in pairs)
        if bank_name == 'DependenciesStats1':
            assert all(row[side]['registers_preserved'] is True and row[side]['stack_restored'] is True
                       for row in pairs for side in ('original','candidate'))
        matches = [row for row in hooks if row['class']==cls and row['fn_name']==fn]
        assert len(matches)==1 and matches[0]['address']==address and matches[0]['validated']=='0'
        hook = {key: matches[0][key] for key in ('class','fn_name','address','type')}
        assert not any(row['hook']['class']==cls and row['hook']['fn_name']==fn for row in data['functions'])
        scope = ('Comparación por pares dentro del juego completo con partida cargada. Entrada EXE original con RH apagado '
                 'frente a la misma entrada con el hook de producción activo; ambas rutas comprobadas por par. '
                 'Fixtures aislados y restauración de estado, hooks, configuraciones y partidas. '
                 'Evidencia histórica de la implementación propia; no acredita integración universal, todas las dependencias '
                 'transitivas ni independencia de datos/servicios del host. '+specific)
        evidence = dict(passed=True, pairs=count, differences=0, registered_pairs=count,
                        registered_functions=1, functions={function:count}, covered_functions={function:True},
                        scope=scope, function_scope={function:specific}, dependency_review=None,
                        run=run.relative_to(ROOT).as_posix(), summary_sha256=sha(run/'summary.json'),
                        result_txt_sha256=sha(run/'RESULTADO.txt'), log_sha256=summary['log_sha256'],
                        all_equal=True, state_restored=True, saves_unchanged=True, configs_restored=True,
                        original_exe_sha256=summary['original_exe_sha256'], executed_probe_sha256=summary['executed_probe_sha256'],
                        manifest={'tested_sources':audit['tested_sources'], 'bank_sources':audit['bank_sources']},
                        comparison_storage={'uncompressed_sha256':summary['log_sha256'], 'uncompressed_bytes':len(raw),
                                            'compressed_path':gz.relative_to(ROOT).as_posix(), 'compressed_sha256':sha(gz),
                                            'compression':'gzip','gzip_mtime':0,'lossless_roundtrip_sha256':summary['log_sha256']},
                        reaudit=audit)
        path = bank/'validation-summary.json'
        assert not path.exists()
        path.write_text(json.dumps(evidence, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
        additions.append(dict(hook=hook, evidence={'path':path.relative_to(ROOT).as_posix(), 'sha256':sha(path),
                             'passed_path':['covered_functions',function], 'pairs':count,
                             'original_exe_sha256':summary['original_exe_sha256']},
                             tested_sources=audit['tested_sources'], dependency_review=None,
                             source_provenance='Sonda y fuentes exactas verificadas en la segunda corrida, con hook RH de producción OFF/ON.',
                             scope=scope, review_reference=(bank/'README.md').relative_to(ROOT).as_posix()))
        approved[address] = {'identity':function, 'validation_summary':path.relative_to(ROOT).as_posix()}
    backup = ROOT/'reversing/validation-registry.backup-before-originaldependencies1.json'
    assert not backup.exists() and sha(registry)==registry_before
    shutil.copy2(registry, backup)
    assert sha(backup)==registry_before
    data['functions'].extend(additions)
    temp = registry.with_suffix('.originaldependencies1.tmp')
    temp.write_text(json.dumps(data, indent=2)+'\n', encoding='utf-8')
    assert sha(registry)==registry_before
    os.replace(temp, registry)
    for row in hooks:
        if row['address'] in approved and row['class']+'::'+row['fn_name']==approved[row['address']]['identity']:
            row['validated']='1'
    hooks_temp = hooks_path.with_suffix('.originaldependencies1.tmp')
    with hooks_temp.open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader(); writer.writerows(hooks)
    os.replace(hooks_temp, hooks_path)
    (HERE/'approved.json').write_text(json.dumps(approved, indent=2)+'\n', encoding='utf-8')
    print(json.dumps({'registered_functions':len(additions), 'registered_pairs':sum(value[3] for value in TARGETS.values()),
                      'registry_before_sha256':registry_before, 'registry_after_sha256':sha(registry)}))


if __name__ == '__main__':
    main()
