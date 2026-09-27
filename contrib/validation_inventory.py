"""Generate the validation worklist for every recorded reversed function."""
import csv
import json
from collections import Counter, defaultdict
from pathlib import Path
from validation_registry import KEY, evaluate


def generate(root):
    root = Path(root)
    result = evaluate(root, root / 'docs/hooks.csv')
    states = {tuple(r['hook'][k] for k in KEY): r for r in result['functions']}
    registry = json.loads((root/'reversing/validation-registry.json').read_text(encoding='utf-8'))
    evidence = {tuple(r['hook'][k] for k in KEY):r['evidence']['path'] for r in registry['functions']}
    with (root/'docs/hooks.csv').open(encoding='utf-8-sig', newline='') as stream:
        rows = list(csv.DictReader(stream))
    counts, classes, pending = Counter(), defaultdict(Counter), []
    validated_flags = 0
    for row in rows:
        if row['reversed'] != '1':
            continue
        key = tuple(row[k] for k in KEY)
        entry = states.get(key, {})
        expected_validated = bool(entry.get('evidence_ok'))
        if row.get('validated') not in ('0', '1'):
            raise ValueError(f"validated inválido o ausente: {row}")
        if (row['validated'] == '1') != expected_validated:
            raise ValueError(
                f"validated desincronizado para {key}. Ejecuta: "
                "python contrib/sync_validated_flags.py"
            )
        validated_flags += row['validated'] == '1'
        status = 'vigente' if entry.get('current') else 'historica' if expected_validated else 'sin_vincular'
        counts[status] += 1
        classes[row['class']][status] += 1
        if status == 'historica':
            pending.append((row, evidence[key]))
    total = sum(counts.values())
    lines = ['# Inventario de validación de funciones reconstruidas', '',
             '[Panel](ReversedClasses.md) · [Registro y método](../reversing/ValidationRegistry/README.md)', '',
             f'Universo de esta revisión: **{total:,} entradas reversed=1**, de {len(rows):,} registradas. Las {len(rows)-total} pendientes de reconstrucción quedan fuera de este inventario, pero siguen dentro del denominador del panel.', '',
             '| Estado | Entradas |', '| --- | ---: |',
             f"| `validated=1` · evidencia aprobada vinculada | {validated_flags} |",
             f"| Vigencia acreditada para su alcance | {counts['vigente']} |",
             f"| Evidencia histórica vinculada; revisión pendiente | {counts['historica']} |",
             f"| Sin evidencia aprobada vinculada en este registro | {counts['sin_vincular']} |", '',
             '`validated=1` registra que existe evidencia aprobada al menos una vez; la vigencia actual se calcula aparte con hashes de fuentes y revisión de dependencias. Sin vincular no significa nunca probada ni fallida. Hay bancos todavía no importados. Cada entrada tiene una sola clasificación; los pares y las repeticiones no multiplican funciones. Las funciones virtuales pueden tener dirección cero en el CSV: se identifican también por clase, nombre y tipo.', '',
             '## Siguiente tanda con evidencia disponible', '',
             'Revisar fuentes, versión, configuración y dependencias; repetir el banco cuando no se pueda confirmar su vigencia. Esta lista no autoriza desbloquear los 86 bloqueos aplazados documentados. No se marca toda una clase por validar una de sus funciones.', '',
             '| Clase | Función registrada | Evidencia |', '| --- | --- | --- |']
    for row, path in sorted(pending, key=lambda x:(x[0]['class'],x[0]['fn_name'])):
        lines.append(f"| {row['class']} | `{row['fn_name']}` | [Resumen](../{path}) |")
    lines += ['', '## Inventario por clase', '', '<details>', '<summary>Entradas reconstruidas de todas las clases registradas</summary>', '',
              '| Clase | Reconstruidas | Vigentes | Históricas por revisar | Sin vincular |', '| --- | ---: | ---: | ---: | ---: |']
    for cls, c in sorted(classes.items()):
        lines.append(f"| `{cls}` | {sum(c.values())} | {c['vigente']} | {c['historica']} | {c['sin_vincular']} |")
    lines += ['', '</details>', '', 'Regenerar desde la raíz: `python contrib/validation_inventory.py`. El informe no cambia hooks.csv ni acredita funciones por sí mismo.', '']
    (root/'docs/ValidationInventory.md').write_text('\n'.join(lines), encoding='utf-8')
    return dict(counts)


if __name__ == '__main__':
    print(generate(Path(__file__).resolve().parents[1]))
