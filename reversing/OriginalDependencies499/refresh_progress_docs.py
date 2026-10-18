"""Refresh cohort documentation from admitted runtime evidence only (idempotent, in-place).

Run order: reconcile_validated.py -> update_progress.py -> refresh_progress_docs.py.
It never rewrites targets.csv / baseline.json and never replaces hand-written README sections: it only rewrites
the classification table, the evolution paragraph, the marker block with the reconciliation section, work-queue.csv,
the CurrentStatus paragraph about the cohort and the handoff note.
"""
import csv
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
NL = chr(10)


def load(p):
    return json.loads(p.read_text(encoding='utf-8'))


def sub1(pattern, repl, text):
    new, n = re.subn(pattern, lambda m: repl, text, count=1, flags=re.M)
    assert n == 1, pattern
    return new


def main():
    progress = load(HERE / 'progress.json')
    rows = list(csv.DictReader((HERE / 'progress.csv').open(encoding='utf-8-sig')))
    hooks = list(csv.DictReader((ROOT / 'docs/hooks.csv').open(encoding='utf-8-sig')))
    reconciled = load(HERE / 'approved-reconciled.json')
    passed = progress['runtime_passed']
    pending = progress['game_runtime_pending']
    libs = progress['categories']['library']
    game = progress['categories']['game']
    lib_ok = progress['library_runtime_validated']
    current = len(hooks)
    groups = len({r['class'] for r in hooks})
    first = {'approved.json': 105, 'approved-level6.json': 20}
    n_rec = sum(1 for a in reconciled if next(r for r in rows if r['address'] == a)['category'] == 'game')
    assert passed == sum(first.values()) + n_rec and game - passed == pending

    # work-queue.csv: pending game rows only, keeping prepared banks that are not validated
    prepared = {}
    import re as _re
    for bank in (ROOT / 'reversing').glob('Dependencies*'):
        h = bank / 'LiveComparison.hpp'
        if h.exists():
            for c, f, a, n in _re.findall(r'\{"([^"]+)","([^"]+)",(0x[0-9A-Fa-f]+),(?:0x[0-9A-Fa-f]+|0),-?\d+,(\d+)\}', h.read_text(encoding='utf-8')):
                prepared[int(a, 16)] = {'identity': c + '::' + f, 'bank': bank.name, 'pairs_expected': n}
    queue = []
    for r in rows:
        if r['category'] == 'game' and r['runtime_status'] != 'passed':
            d = prepared.get(int(r['address'], 16), {})
            queue.append({'address': r['address'], 'identity': d.get('identity', r['identity']),
                          'status': 'prepared_not_validated' if d else 'pending', 'bank': d.get('bank', ''),
                          'pairs_expected': d.get('pairs_expected', ''), 'source_snapshot': r['source']})
    assert len(queue) == pending
    with (HERE / 'work-queue.csv').open('w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fieldnames=['address', 'identity', 'status', 'bank', 'pairs_expected', 'source_snapshot'])
        w.writeheader()
        w.writerows(queue)

    # README: table, evolution and reconciliation block
    path = HERE / 'README.md'
    t = path.read_text(encoding='utf-8')
    t = sub1(r'^\| Funciones del juego \|.*$', '| Funciones del juego | %d | %d | %d |' % (game, passed, pending), t)
    t = sub1(r'^\| Total de la cohorte \|.*$', '| Total de la cohorte | 499 | %d | %d |' % (passed, 499 - passed), t)
    t = sub1(r'^Evolución: .*$', (
        'Evolución: el 10/10/2026 la tabla era 235 del juego (105 aprobadas, 130 pendientes), 243 bibliotecas y 21 inciertas; '
        'el 17/10/2026 las 21 inciertas se resolvieron (20 juego, 1 biblioteca) y se conciliaron %d funciones del juego que otros bancos '
        'ya habían reconstruido y validado (ver «Conciliación con bancos de otras tandas»), de modo que hoy son %d aprobadas y %d pendientes. '
        'El total sigue en 499 y `targets.csv` y `baseline.json` no cambian. Los %d de la columna de pendientes son las %d del juego y las %d bibliotecas, '
        'de las que la reconstruida (`CrtPrintf`, `runtime_status=library_validated` en `progress.csv`) conserva su seguimiento aparte y no cuenta como aprobada en juego.'
        ) % (n_rec, passed, pending, 499 - passed, pending, libs), t)
    lines = ['<!-- COHORT-RECONCILE:BEGIN -->', '## Conciliación con bancos de otras tandas (17/10/2026)', '',
             ('%d funciones del juego de la cohorte, que el 10/10 figuraban como envoltorios pendientes, fueron reconstruidas y validadas después por '
              'bancos de otras tandas (regresión final de nivel 6 y bancos 22/23) y constan con `validated=1` en `docs/hooks.csv` y en '
              '`reversing/validation-registry.json`. `reconcile_validated.py` las admite en [approved-reconciled.json](approved-reconciled.json) '
              'sólo tras reverificar la evidencia: identidad única en catálogo y registro, SHA256 del resumen, `passed`/`all_equal`/`differences=0`/`state_restored`, '
              '`covered_functions` propio, resumen de la corrida registrada, log JSONL leído en streaming contra su SHA y recuento de pares de **la propia función** '
              '(los fixtures de otras funciones del mismo banco no dan crédito). Las direcciones que sólo coinciden por nombre no cuentan '
              '(`InteriorGroup_c::SetupHousePeds` 0x0046CED0 es otra dirección que la validada 0x005965E0 y sigue pendiente). '
              'La biblioteca `CrtPrintf` ya estaba como `library_validated`; se reverifica con la misma regla y no cambia de categoría.') % n_rec,
             '', '| Función | Dirección EXE | Pares propios | Pares del banco | Corrida |', '| --- | --- | ---: | ---: | --- |']
    for a, it in reconciled.items():
        cat = next(r for r in rows if r['address'] == a)['category']
        lines.append('| %s%s | %s | %d | %d | `%s` |' % (it['identity'], '' if cat == 'game' else ' (biblioteca)', a, it['own_pairs'], it['bank_pairs'], it['run'].split('/')[-1]))
    lines += ['', 'Los pares propios son los de la función en su banco; no se suman a los 243392 + 43008 de las secciones anteriores. `audit-cohort-current.py` sigue auditando sólo `approved.json`; las seis filas se auditaron con `reconcile_validated.py`.',
              '<!-- COHORT-RECONCILE:END -->']
    block = NL.join(lines)
    if '<!-- COHORT-RECONCILE:BEGIN -->' in t:
        t = re.sub(r'<!-- COHORT-RECONCILE:BEGIN -->.*?<!-- COHORT-RECONCILE:END -->', lambda m: block, t, count=1, flags=re.S)
    else:
        t = t.replace('## Continuación', block + NL + NL + '## Continuación', 1)
    t = re.sub(r'(## Continuación\s+\[work-queue.csv\]\(work-queue.csv\) muestra las )entradas del juego todavía pendientes', lambda m: m.group(1) + 'entradas del juego todavía pendientes (%d)' % pending, t, count=1) if '(%d)' % pending not in t else t
    path.write_text(t, encoding='utf-8')

    # CurrentStatus paragraph
    status = ROOT / 'docs/CurrentStatus.md'
    s = status.read_text(encoding='utf-8')
    para = ('La cohorte inicial de 499 destinos se clasificó el 10/10/2026 en 235 funciones del juego, 243 de bibliotecas y 21 inciertas; el 17/10/2026 las 21 inciertas se resolvieron '
            '(PLAN-001: 20 del juego y 1 biblioteca, la CRT `printf`), con lo que queda en 255 del juego, 244 bibliotecas y 0 inciertas. '
            '**%d funciones del juego admitidas: 105 con 243392 pares finales (10/10), 20 con 43008 pares (17/10, bancos `Dependencies21A` a `21G`) y %d conciliadas el 17/10 '
            'con la evidencia de otros bancos (regresión final de nivel 6, bancos 22/23), cero diferencias, ningún resultado nulo y restauración comprobada.** '
            'La biblioteca reconstruida (`CrtPrintf`, 4096 pares) se cuenta aparte, como biblioteca. Quedan %d entradas del juego pendientes y ninguna incierta; '
            'las %d bibliotecas se siguen aparte. [Inventario, bancos y alcance](../reversing/OriginalDependencies499/README.md).') % (passed, n_rec, pending, libs)
    s = re.sub(r'^La cohorte inicial de 499 destinos se clasificó.*$', lambda m: para, s, count=1, flags=re.M)
    s = re.sub(r'El catálogo principal contiene \*\*\d+/\d+ funciones y \d+/\d+ grupos', 'El catálogo principal contiene **%d/%d funciones y %d/%d grupos' % (current, current, groups, groups), s, count=1)
    status.write_text(s, encoding='utf-8')

    handoff = ('# Continuidad: dependencias fuera del catálogo' + NL + NL +
               'Estado del 17 de octubre de 2026, rama main. Cohorte fija de 499: %d juego, %d bibliotecas y 0 inciertas (el 10/10 eran 235, 243 y 21; PLAN-001 resolvió las 21: 20 juego y 1 biblioteca, CRT `printf`). '
               'Admitidas %d funciones del juego: 105 con 243392 pares finales RH OFF/ON (10/10), 20 con 43008 pares (17/10, bancos Dependencies21A a 21G, registro en approved-level6.json) y %d conciliadas con bancos de otras tandas (approved-reconciled.json, escrito por reconcile_validated.py). '
               'Catálogo %d/%d, grupos %d/%d; evidencia histórica, sin certificar independencia del EXE. [Inventario](../OriginalDependencies499/README.md).' % (game, libs, passed, n_rec, current, current, groups, groups) + NL + NL +
               'Quedan %d entradas del juego pendientes y ninguna incierta. Tras validar cualquier banco nuevo ejecutar reconcile_validated.py, update_progress.py y refresh_progress_docs.py. Consultar progress.csv, work-queue.csv y los READMEs de bancos. '
               'Registrar sólo después de verificar JSONL, ABI, efectos y restauración dentro del juego completo. La clasificación estática no concede aprobación. Las bibliotecas tienen seguimiento separado.' % pending + NL + NL +
               'Compilaciones y GTA en serie, paralelismo 2. No editar fuentes durante la compilación ni modificar instrumentación aprobada sin repetir su banco. Usar sólo la copia CVehicle Test; nunca la instalación personal. '
               'EXE canónico SHA256: 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b. runtime_runner.py, los 28 documentos congelados y la whitelist permanecen intactos.' + NL + NL +
               'Cada banco conserva validation-summary.json y runs/corrida con summary.json, approval-audit.json, RESULTADO.txt y comparisons.jsonl.gz sin pérdida. verify_bank.py audita los pares; register_bank.py admite exclusivamente direcciones clasificadas como juego de esta cohorte. '
               'No se hizo commit ni push de estas tandas. Excluir backups, cachés, artifacts, dumps y docs/Reorganizacion de futuras publicaciones.' + NL)
    (ROOT / 'reversing/_handoff/DEPENDENCIES499_CURRENT.md').write_text(handoff, encoding='utf-8')
    print(json.dumps({'cohort_passed': passed, 'pending_game': pending, 'reconciled_game': n_rec, 'catalogue': current, 'groups': groups}))


if __name__ == '__main__':
    main()
