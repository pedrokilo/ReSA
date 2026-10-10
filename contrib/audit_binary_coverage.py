"""Reproducible static coverage inventory; never changes validation or hooks."""
import csv
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAP = ROOT / 'docs/MemoryMap'
OUT = ROOT / 'reversing/BinaryCoverageAudit'


def read(path):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def write(name, rows, fields):
    with (OUT / name).open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    functions = read(MAP / 'binary_functions.csv')
    unknown = [row for row in functions if not row['catalogue_names']]
    by_address = {int(row['va'], 16): row for row in functions}
    slots = read(MAP / 'binary_slots.csv')
    outside_slots = [row for row in slots if int(row['function_va'], 16) in by_address
                     and not by_address[int(row['function_va'], 16)]['catalogue_names']]
    catalogue = read(ROOT / 'docs/hooks.csv') + read(ROOT / 'docs/hooks_notsa.csv')
    identities = {(row['class'], row['fn_name']) for row in catalogue}
    registrations = [row for row in read(MAP / 'registrations.csv')
                     if (row['owner'], row['name']) not in identities]
    owners = {row['class'] for row in catalogue}
    types = [row for row in read(MAP / 'types.csv') if row['name'] not in owners]
    binary_summary = json.loads((MAP / 'binary_summary.json').read_text(encoding='utf-8'))
    assert len(unknown) == binary_summary['without_catalogue_names']
    assert len(functions) == binary_summary['ghidra_function_entries']
    assert len({row['va'] for row in functions}) == len(functions)
    write('unmatched-functions.csv', unknown, list(functions[0]))
    write('virtual-slots-outside-catalogue.csv', outside_slots, list(slots[0]))
    write('source-registration-candidates.csv', registrations,
          list(read(MAP / 'registrations.csv')[0]))
    write('source-types-outside-catalogue.csv', types, list(read(MAP / 'types.csv')[0]))
    summary = {
        'scope': 'static inventory; counts are candidates, not missing implementations',
        'exe_sha256': binary_summary['exe_sha256'],
        'binary_functions': len(functions), 'catalogue_named_entries': len(functions)-len(unknown),
        'entries_without_catalogue_names': len(unknown),
        'source_slots': len(slots), 'source_slots_without_catalogue_name': len(outside_slots),
        'unique_source_slot_targets_without_catalogue_name': len({row['function_va'] for row in outside_slots}),
        'registration_identity_candidates': len(registrations),
        'registration_owners': sorted({row['owner'] for row in registrations}),
        'source_type_rows_outside_catalogue': len(types),
        'unique_source_type_names_outside_catalogue': len({row['name'] for row in types}),
        'unresolved_indirect_calls': binary_summary['indirect_call_sites'],
        'exact_missing_game_functions': None,
        'inputs_sha256': {name: hashlib.sha256((MAP/name).read_bytes()).hexdigest()
                         for name in ['binary_functions.csv', 'binary_slots.csv',
                                      'registrations.csv', 'types.csv']},
        'limitations': ['Source registration labels can differ from catalogue overload labels.',
                        'Types include POD, templates, helpers and libraries.',
                        'Repeated virtual targets include inheritance and deleting destructors.',
                        'No runtime equivalence or independence from the original EXE is certified.'],
    }
    calls_path = OUT / 'review-source-calls.csv'
    if calls_path.exists():
        calls = read(calls_path)
        assert len({row['address'] for row in calls}) == len(calls)
        assert all(int(row['address'], 16) in by_address and
                   not by_address[int(row['address'], 16)]['catalogue_names'] for row in calls)
        summary['explicit_original_targets_without_catalogue_name'] = len(calls)
        summary['explicit_targets_referenced_from_catalogued_entries'] = sum(
            int(row['incoming_edges_from_catalogued_functions']) > 0 for row in calls)
        summary['inputs_sha256']['review-source-calls.csv'] = hashlib.sha256(calls_path.read_bytes()).hexdigest()
    (OUT / 'summary.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(summary, ensure_ascii=False, indent=2))


if __name__ == '__main__':
    main()
