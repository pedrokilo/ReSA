"""Genera README.md del cohorte «2» a partir de progress.csv/json, baseline.json, work-queue.csv y priorities-notes.csv (idempotente)."""
import csv
import json
import re
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
NL = chr(10)


def rd(name):
    with (HERE / name).open(encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def family(r):
    ident, ev = r['identity'], r['evidence']
    rule = (re.search(r'regla=([\w-]+)', ev) or [None, ''])[1]
    if 'QuickTime' in ident:
        return 'QuickTime/QTML (thunks y ayudantes enlazados estáticamente)'
    if rule == 'L2':
        return 'CRT estática de MSVC (símbolos de runtime, dirección >= 0x821000)'
    if 'D3D9' in ident or 'd3d9' in ident.lower():
        return 'RenderWare, controlador D3D9 (_rwD3D9*/RwD3D9*/_rpD3D9*)'
    return 'RenderWare SDK (rw*, rp*, rt*, rx*)'


def main():
    progress, targets, queue = rd('progress.csv'), rd('targets.csv'), rd('work-queue.csv')
    summary = json.loads((HERE / 'progress.json').read_text(encoding='utf-8'))
    base = json.loads((HERE / 'baseline.json').read_text(encoding='utf-8'))
    notes = {r['address']: r for r in rd('priorities-notes.csv')}
    overrides = rd('classification-overrides.csv')
    total = len(progress)
    games = [r for r in progress if r['category'] == 'game']
    libs = [r for r in progress if r['category'] == 'library']
    unk = [r for r in progress if r['category'] == 'unknown']
    wrap_game = sum(r['implementation'] == 'wrapper' for r in games)
    pc = [t for t in targets if 'plugin_call' in t['wrapper_styles']]
    cat_of = {r['address']: r for r in progress}
    pc_direct = [t for t in pc if cat_of[t['address']]['implementation'] == 'wrapper']
    pc_direct_game = sum(cat_of[t['address']]['category'] == 'game' for t in pc_direct)
    styles = Counter()
    for t in targets:
        for s in t['wrapper_styles'].split('; '):
            styles[s] += 1
    fam = Counter(family(r) for r in libs)
    owners = Counter()
    for r in games:
        m = re.match(r'([A-Za-z_][\w<>]*)::', r['identity'])
        owners[m.group(1) if m else '(sin clase / ayudante)'] += 1
    bands = Counter()
    for r in progress:
        bands[(int(r['address'], 16) >> 16 << 16, r['category'])] += 1
    incoming = sum(int(t['incoming_edges_from_catalogued_functions']) > 0 for t in targets)
    text = []
    A = text.append
    A('# Dependencias originales fuera del catálogo · cohorte 2 (17/10/2026)')
    A('')
    A(('Instantánea fija del 17 de octubre de 2026 con el árbol de trabajo actual: **%d destinos originales explícitos** (direcciones de función del EXE que `source/` sigue llamando '
       'por dirección) que no tienen nombre en `docs/hooks.csv`, no están validados y no pertenecen al cohorte de 499 (`../OriginalDependencies499/`). `targets.csv` y `baseline.json` '
       'preservan el snapshot y sus hashes; el catálogo principal (8583/8583) no incluye estas funciones. Clasificar una función no equivale a reconstruirla ni a validarla: '
       'ninguna fila tiene validación runtime todavía.') % total)
    A('')
    A('| Clasificación | Total | Aprobadas en juego | Pendientes |')
    A('| --- | ---: | ---: | ---: |')
    A('| Funciones del juego | %d | %d | %d |' % (len(games), summary['runtime_passed'], summary['game_runtime_pending']))
    A('| Bibliotecas/runtime/SDK | %d | 0 | Seguimiento separado |' % len(libs))
    A('| Identidad o procedencia incierta | %d | 0 | %d |' % (len(unk), len(unk)))
    A('| Total del cohorte | %d | %d | %d |' % (total, summary['runtime_passed'], summary['runtime_pending']))
    A('')
    A(('De las %d entradas del juego, %d son envoltorios directos (la función fuente sólo delega en el EXE) y %d son llamadas incrustadas en una reconstrucción C++ mayor '
       '(`embedded_call`: la fuente llama al original desde dentro de otra función). Incierto = 0: ninguna fila quedó sin categoría, pero las filas con confianza media o con '
       'ubicación atípica se listan abajo y no deben tratarse como certezas.') % (len(games), wrap_game, len(games) - wrap_game))
    A('')
    A('## Qué entra en este cohorte y qué no')
    A('')
    A(('El cohorte 499 se definió el 10/10/2026 como los destinos explícitos con aristas entrantes desde funciones catalogadas. Hoy, con el catálogo al 100%%, esa condición casi ya no '
       'discrimina (sólo %d de los %d destinos tienen aristas de ese tipo), así que este cohorte reúne **todos** los destinos explícitos que siguen apareciendo en `source/`, '
       'y la columna `incoming_edges_from_catalogued_functions` se conserva como dato. Decisiones:') % (incoming, total))
    A('')
    A(('- **Alcance de fuentes**: todo `source/` (`.cpp`, `.h`, `.hpp`, `.inc`, `.inl`), como el cruce original (`audit_source_calls.py` barría `source/` entero). Hay %d destinos con referencias en `source/game_sa` '
       'y %d referenciados sólo desde `source/app` (capa de plataforma: WinMain, WindowedMode, WinInput). `source/app` se incluye porque el cohorte original lo incluía.') % (
        base['explicit_uncatalogued_targets_in_source_game_sa'], base['explicit_uncatalogued_targets_only_outside_game_sa']))
    A(('- **Exclusión del cohorte 499**: hoy hay %d destinos explícitos sin nombre de catálogo; %d son los del cohorte 499 que siguen pendientes (124 del juego + 243 de bibliotecas, todos aún referenciados '
       'desde `source/`) y se excluyen. Las 131 funciones del juego y la `printf` del cohorte 499 ya están en el catálogo, por eso no aparecen.') % (
        base['explicit_uncatalogued_function_targets_total'], base['excluded_cohort1_still_referenced']))
    A('- **Destinos**: sólo direcciones que son entrada de función del EXE (`binary_functions.csv`, Ghidra 12.1.4 sobre el EXE canónico `72ae59e4...cecc9b`) sin `catalogue_names` en el inventario regenerado con el catálogo actual. '
      'Las direcciones de datos (`reinterpret_cast<float*>(0xB6F03C)`, etc.) se descartan por no ser entradas de función.')
    A('- **Hooks ya registrados fuera del catálogo**: las líneas con `RH_Scoped*`/`HookInstall` no cuentan como dependencia. Dos destinos del cohorte tienen un hook activo en `source/` aunque no figuran en `docs/hooks.csv`: '
      '`__MainWndProc` 0x747EB0 (WndProc.cpp:293) y `diPadInit` 0x7485C0 (WinInput.cpp:440). Los hooks comentados (`// RH_ScopedInstall(...)`) son sólo evidencia de nombre.')
    A('')
    A('## Metodología')
    A('')
    A('1. **Inventario binario con el catálogo actual**: `contrib/build-binary-map.py --root . --export build/binary-map --output <dir>` (con `docs/MemoryMap/index.html` copiado a `<dir>`). Se escribió en un directorio temporal; `docs/MemoryMap` no se modificó '
      '(su `binary_functions.csv` es del 10/10 y marcaba 9223 entradas sin nombre; con el catálogo de hoy son 9192).')
    A('2. **Cruce fuente/binario** (`build_cohort.py`): barrido de `source/` sin comentarios ni cadenas, cinco estilos de referencia a una dirección: `plugin_call` (`plugin::Call*<..., 0xADDR, ...>`), `direct_function_pointer_cast` '
      '(`((ret(__conv*)(args))0xADDR)(...)`), y tres estilos nuevos que el cruce de 10/10 no veía porque las conversiones recientes a C++ los usan: `reinterpret_cast_pointer` (`reinterpret_cast<Fn>(0xADDR)`, `uintptr_t{0xADDR}`), '
      '`function_pointer_alias_cast` y `address_constant` (`constexpr uintptr_t kExeX = 0xADDR;`). Los archivos `.cpp.inc` también se leen (el cruce anterior sólo leía `.cpp`/`.h`).')
    A('3. **Clasificación con evidencia** (`classify_cohort.py`): reglas deterministas citadas por id en la columna `evidence` (`regla=`), complementadas con revisión manual por desensamblado del EXE (`classification-overrides.csv`, una fila por caso con su motivo). '
      'El proyecto de referencia `gtasa-reversed` sólo aporta nombres (hint), nunca categoría. La dirección sola no decide: el nombre/contrato de la fuente es la evidencia, como en el cohorte 499.')
    A('4. **Progreso** (`update_progress.py`): `progress.csv`/`progress.json` con el mismo esquema que el cohorte 499; `approved.json` está vacío (se rellenará con el mismo rigor que `register_bank.py`/`register_approved.py` del cohorte 499).')
    A('5. **Prioridades** (`build_priorities.py`): `work-queue.csv` ordena las entradas del juego pendientes por esfuerzo estimado (bytes, dependencias sin catalogar, llamadas indirectas, x87, tablas de saltos, SEH).')
    A('')
    A('Reglas de categoría: **L1** nombre de API RenderWare en un archivo del SDK (`rw*.cpp`, `rp*.cpp`, `rt*.cpp`, `RenderWare.h`) con dirección >= 0x740000; **L2** símbolo de runtime en el mapa de Ghidra o dirección >= 0x821000 (CRT estática); '
      '**L3** nombre RenderWare fuera de los archivos del SDK con dirección >= 0x740000; **G1** nombre de juego (proyecto de referencia o firma `Clase::método`) con dirección < 0x740000; **G2** sólo contexto de juego (llamada incrustada sin símbolo). '
      'Los casos que contradicen su dirección (nombre RW con dirección baja, nombre de juego con dirección alta) van a revisión manual. Contraste con el cohorte 499 (sólo por identidad ya revisada y dirección, sin contexto fuente): las reglas coinciden en 472 de 499; los 27 desacuerdos son QuickTime (15) y `AtomicDefaultRenderCallBack` (1), sin nombre de API reconocible; '
      'la familia `*GtaStream*` y `CMemoryMgr::Free/Calloc` (7), donde el propio cohorte 499 es inconsistente; y 4 de plataforma/juego en 0x74xxxx (`DoDriveByShootings`, `DIReleaseMouse`, `Rs::psGetMemoryFunctions`, `GetDXVersion`). Son justo los casos que aquí van a revisión manual.')
    A('')
    A('## Desglose')
    A('')
    A('Estilos de referencia (un destino puede tener varios):')
    A('')
    A('| Estilo | Destinos |')
    A('| --- | ---: |')
    for s, n in styles.most_common():
        A('| `%s` | %d |' % (s, n))
    A('')
    A(('**Envoltorios `plugin::Call*` directos**: %d destinos tienen al menos una referencia `plugin::Call*`; en %d de ellos la función fuente es un envoltorio directo (una sola sentencia que delega en el EXE), %d del juego y %d de biblioteca. '
       'Los demás (%d) usan `plugin::Call*` dentro de una función C++ mayor. Los otros %d destinos se llaman con casts de puntero a función o `reinterpret_cast` (en el SDK RenderWare son casi todos wrappers de una línea).') % (
        len(pc), len(pc_direct), pc_direct_game, len(pc_direct) - pc_direct_game, len(pc) - len(pc_direct), total - len(pc)))
    A('')
    A('Bibliotecas por familia:')
    A('')
    A('| Familia | Destinos |')
    A('| --- | ---: |')
    for s, n in fam.most_common():
        A('| %s | %d |' % (s, n))
    A('')
    A('Entradas del juego por clase (primeras 12):')
    A('')
    A('| Clase | Destinos |')
    A('| --- | ---: |')
    for s, n in owners.most_common(12):
        A('| %s | %d |' % (s, n))
    A('')
    A('## Confianza media o ubicación atípica (revisión manual)')
    A('')
    A('Estas filas se decidieron por desensamblado y no por una regla; la evidencia completa está en `classification-overrides.csv` y en `progress.csv`:')
    A('')
    A('| Dirección | Identidad | Categoría | Motivo resumido |')
    A('| --- | --- | --- | --- |')
    for o in overrides:
        A('| %s | %s | %s | %s |' % (o['address'], o['identity'].replace('|', '/'), o['category'], o['notes'].split(';')[0].rstrip('.')))
    A('')
    helpers = [r for r in games if r['identity'].startswith('helper sin s')]
    A('Además, %d entradas del juego son llamadas incrustadas sin símbolo confirmado («helper sin símbolo»): la categoría juego es firme (dirección baja y contexto de juego) pero la identidad exacta no; no se les asignó nombre inventado.' % len(helpers))
    A('')
    A('## Las 20 prioridades más sencillas de reconstruir')
    A('')
    A('Orden de `work-queue.csv` (heurística de esfuerzo, ver `build_priorities.py`); las 20 se revisaron una a una con el desensamblado. Ninguna usa SEH ni tiene destinos del juego sin catalogar como dependencia; las dependencias indicadas ya están en el catálogo salvo donde se dice.')
    A('')
    A('| # | Dirección | Identidad | Bytes | Qué hace | Dependencias |')
    A('| ---: | --- | --- | ---: | --- | --- |')
    for r in queue[:20]:
        n = notes[r['address']]
        A('| %s | %s | %s | %s | %s | %s |' % (r['priority_rank'], r['address'], r['identity'].replace('|', '/'), r['body_bytes'], n['what_it_does'], n['dependencies']))
    A('')
    A('## Distribución por franja de dirección')
    A('')
    A('| Franja | Juego | Biblioteca |')
    A('| --- | ---: | ---: |')
    for band in sorted({b for b, _ in bands}):
        A('| 0x%06X-0x%06X | %d | %d |' % (band, band + 0xFFFF, bands[(band, 'game')], bands[(band, 'library')]))
    A('')
    A('Como en el cohorte 499, todo lo de 0x7C0000 en adelante es biblioteca (RenderWare, CRT, QuickTime) y lo de 0x740000-0x76FFFF es sobre todo SDK RenderWare (`rpworld.cpp`, `rpanisot.cpp`); las excepciones (plataforma Win, ayudantes RwHelper, el bloque del plugin de raster D3D9 en 0x4C9AB0-0x4CD3xx) están en la tabla de revisión manual.')
    A('')
    A('## Límites')
    A('')
    A('- El cruce sigue siendo estático por patrones: no ve llamadas por macros que construyen la dirección, direcciones calculadas ni tablas de punteros a función; los destinos sólo-virtuales (`virtual-slots-outside-catalogue.csv`) quedan fuera de este cohorte.')
    A('- «Sin nombre de catálogo» no significa «sin implementar»: `binary_functions.csv` enlaza por dirección fija y por candidatos de registro; un destino puede tener ya un cuerpo C++ equivalente en `source/` que el inventario no enlaza.')
    A('- El proyecto de referencia aporta nombres, no evidencia; las identidades de las filas `wrapper` salen de la firma de la propia fuente. Ninguna fila acredita reconstrucción, equivalencia ni independencia del EXE.')
    A('- La heurística de prioridad es de esfuerzo estimado; no sustituye el diseño del banco de pruebas de cada función.')
    A('')
    A('## Archivos y reproducción')
    A('')
    A('`targets.csv` (mismas columnas que el cohorte 499), `baseline.json` (hashes y alcance), `evidence-sources.csv` (contexto fuente por referencia), `classification.csv`, `classification-overrides.csv`, `review-queue.csv` (vacío si todo está revisado), '
      '`progress.csv`/`progress.json`, `approved.json`, `work-queue.csv`, `priorities-notes.csv`. Scripts: `build_cohort.py`, `classify_cohort.py`, `update_progress.py`, `build_priorities.py`, `build_readme.py`.')
    A('')
    A('Orden de regeneración (con el directorio del inventario binario en `<bm>`): `python reversing/OriginalDependencies2026-10-17/classify_cohort.py --binary-map <bm>` -> `update_progress.py` -> `build_priorities.py --binary-map <bm>` -> `build_readme.py`. '
      '`build_cohort.py` no reescribe `targets.csv` si ya existe.')
    (HERE / 'README.md').write_text(NL.join(text) + NL, encoding='utf-8')
    print('README.md escrito:', total, 'destinos')


if __name__ == '__main__':
    main()
