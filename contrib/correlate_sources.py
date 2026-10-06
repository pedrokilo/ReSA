"""Generate a linked inventory and an explicitly approximate cross-game source map."""
import argparse
from collections import defaultdict
from datetime import date
import os
from pathlib import Path
from urllib.parse import quote
from source_descriptions import FOLDER_DESCRIPTIONS, describe_file


# These are navigational relationships, never claims of identical behavior/ABI.
FOLDERS = [
    ('animation', ['Animation', 'Plugins/RpAnimBlendPlugin', 'Ragdoll']),
    ('audio', ['Audio']),
    ('buildings', ['Entity/Building.cpp', 'Entity/AnimatedBuilding.cpp']),
    ('collision', ['Collision']),
    ('control', ['Scripts', 'Tasks', 'Events', 'DecisionMakers', 'CarCtrl.cpp', 'Population.cpp']),
    ('core', ['Core', 'Game.cpp', 'Camera.cpp', 'World.cpp', 'Frontend']),
    ('entities', ['Entity']),
    ('extras', ['Pipelines', 'PostEffects.cpp']),
    ('fakerw', ['RenderWare/rw']),
    ('math', ['Core']),
    ('modelinfo', ['Models']),
    ('objects', ['Entity/Object', 'ObjectData.cpp', 'Fx']),
    ('peds', ['Entity/Ped', 'Tasks', 'Events', 'DecisionMakers', 'Attractors']),
    ('renderer', ['Renderer.cpp', 'VisibilityPlugins.cpp', 'Fx', 'Pipelines', 'RenderWare']),
    ('rw', ['RenderWare', 'Plugins', 'TxdStore.cpp']),
    ('save', ['GenericGameStorage.cpp', 'C_PcSave.cpp']),
    ('skel', []),
    ('text', ['Text', 'Font.cpp', 'Messages.cpp']),
    ('vehicles', ['Entity/Vehicle', 'cHandlingDataMgr.cpp', 'cTransmission.cpp']),
    ('weapons', ['Weapon.cpp', 'WeaponInfo.cpp', 'WeaponEffects.cpp', 'Explosion.cpp']),
]

# re3/reVC paths share these names; unavailable paths render as a dash.
# Each item has its own explanation to keep one-to-many mappings honest.
FUNCTIONAL = [
    ('Generadores de vehículos', ['vehicles/CarGen.cpp', 'vehicles/CarGen.h'],
     ['CarGenerator.cpp', 'CarGenerator.h', 'TheCarGenerators.cpp', 'TheCarGenerators.h'],
     'El generador individual y el administrador están separados en reSA.'),
    ('Propiedades de conducción', ['vehicles/HandlingMgr.cpp', 'vehicles/HandlingMgr.h'],
     ['cHandlingDataMgr.cpp', 'cHandlingDataMgr.h', 'Handling/HandlingDataType.h'],
     'Mismo ámbito de configuración de conducción; los tipos y formatos pueden cambiar.'),
    ('Transmisión', ['vehicles/Transmission.cpp', 'vehicles/Transmission.h'],
     ['cTransmission.cpp', 'cTransmission.h'],
     'Correspondencia por la clase y el subsistema de transmisión.'),
    ('Configuración de controles', ['core/ControllerConfig.cpp', 'core/ControllerConfig.h'],
     ['ControllerConfigManager.cpp', 'ControllerConfigManager.h'],
     'Administración de la configuración de controles.'),
    ('Zonas del mundo', ['core/Zones.cpp', 'core/Zones.h'],
     ['TheZones.cpp', 'TheZones.h', 'Zone.cpp', 'Zone.h'],
     'En reSA se distinguen el administrador y los datos de una zona.'),
    ('Zonas de culling', ['core/ZoneCull.cpp', 'core/ZoneCull.h'],
     ['CullZones.cpp', 'CullZones.h'],
     'Zonas que modifican visibilidad y otras propiedades del entorno.'),
    ('Superficies', ['core/SurfaceTable.cpp', 'core/SurfaceTable.h'],
     ['SurfaceInfos_c.cpp', 'SurfaceInfos_c.h', 'SurfaceInfo_c.h'],
     'Propiedades de superficies; es una relación funcional, no identidad de tablas.'),
    ('Lectura de datos del disco', ['core/CdStream.cpp', 'core/CdStream.h', 'core/CdStreamPosix.cpp'],
     ['CdStreamInfo.cpp', 'CdStreamInfo.h'],
     'Área de streaming de datos. Las implementaciones de plataforma no son intercambiables.'),
    ('Menús', ['core/Frontend.cpp', 'core/Frontend.h', 'core/MenuScreens.cpp'],
     ['MenuManager.cpp', 'MenuManager.h', 'Frontend'],
     'reSA distribuye dibujo, entrada, páginas y procesamiento entre varios archivos.'),
    ('Scripts y opcodes', ['control/Script.cpp', 'control/Script.h', 'control/Script2.cpp',
                           'control/Script3.cpp', 'control/Script4.cpp', 'control/Script5.cpp',
                           'control/Script6.cpp', 'control/Script7.cpp', 'control/Script8.cpp'],
     ['Scripts/TheScripts.cpp', 'Scripts/TheScripts.h', 'Scripts/RunningScript.cpp',
      'Scripts/RunningScript.h', 'Scripts/Commands', 'Scripts/CommandParser'],
     'El sistema de scripts se divide en ejecución, administración y comandos temáticos en reSA.'),
    ('Identificadores de opcodes', ['control/ScriptCommands.h'], ['Enums/eScriptCommands.h'],
     'Listas de identificadores. No presupone igualdad de números ni argumentos entre juegos.'),
    ('Identificadores de animaciones', ['animation/AnimationId.h'], ['Enums/AnimationEnums.h'],
     'Correspondencia de propósito; cada juego tiene su propio catálogo.'),
    ('Guardado en PC', ['save/PCSave.cpp', 'save/PCSave.h'], ['C_PcSave.cpp', 'C_PcSave.h'],
     'Mismo ámbito de guardado en PC; no implica compatibilidad de partidas.'),
    ('Modelos simples y LOD', ['modelinfo/SimpleModelInfo.cpp', 'modelinfo/SimpleModelInfo.h'],
     ['Models/AtomicModelInfo.cpp', 'Models/AtomicModelInfo.h', 'Models/LodAtomicModelInfo.cpp'],
     'Relación aproximada: reSA reorganiza los modelos atómicos y los niveles de detalle.'),
    ('Interfaz de audio', ['audio/DMAudio.cpp', 'audio/DMAudio.h'],
     ['Audio/AudioEngine.cpp', 'Audio/AudioEngine.h'],
     'Punto de entrada del sistema de sonido; cDMAudio y CAudioEngine son interfaces distintas.'),
    ('Gestión y lógica de audio', ['audio/AudioManager.cpp', 'audio/AudioManager.h', 'audio/AudioLogic.cpp'],
     ['Audio/Managers', 'Audio/Entities', 'Audio/Hardware'],
     'Correspondencia de subsistema: reSA reparte responsabilidades entre gestores, entidades y hardware.'),
    ('Música y radio', ['audio/MusicManager.cpp', 'audio/MusicManager.h'],
     ['Audio/Managers/AERadioTrackManager.cpp', 'Audio/Managers/AEAmbienceTrackManager.cpp',
      'Audio/Managers/AECutsceneTrackManager.cpp'],
     'reSA tiene gestores especializados; no existe aquí una equivalencia uno a uno.'),
    ('Radio policial', ['audio/PolRadio.cpp', 'audio/PolRadio.h'],
     ['Audio/Entities/AEPoliceScannerAudioEntity.cpp', 'Audio/Entities/AEPoliceScannerAudioEntity.h'],
     'Mismo ámbito funcional de mensajes de radio policial.'),
    ('Sonido de colisiones', ['audio/AudioCollision.cpp', 'audio/AudioCollision.h'],
     ['Audio/Entities/AECollisionAudioEntity.cpp', 'Audio/Entities/AECollisionAudioEntity.h'],
     'Mismo ámbito funcional, integrado como entidad de audio en reSA.'),
    ('Comportamiento de peatones', ['peds/PedAI.cpp', 'peds/PedFight.cpp', 'peds/PedChat.cpp'],
     ['Entity/Ped/Ped.cpp', 'Tasks', 'Events', 'DecisionMakers', 'Conversations'],
     'Relación amplia: la IA de reSA está organizada en tareas, eventos y decisiones.'),
    ('Partículas', ['renderer/Particle.cpp', 'renderer/Particle.h', 'renderer/ParticleMgr.cpp',
                   'renderer/ParticleMgr.h', 'objects/ParticleObject.cpp'],
     ['Fx'],
     'Sistemas de efectos distintos. Compartir propósito o el nombre Particle.h no demuestra equivalencia.'),
    ('Tipos de arma', ['weapons/WeaponType.h'], ['Enums/eWeaponType.h'],
     'Enumeraciones de armas de cada juego; comprobar valores antes de comparar lógica.'),
]


def generate(roots, output):
    roots = [p.resolve() for p in roots]
    output = output.resolve()
    for root in roots:
        if not root.is_dir():
            raise ValueError(f'No existe el árbol: {root}')
    inventories = [sorted(p for p in root.rglob('*') if p.is_file()) for root in roots]

    def link(i, path):
        target = roots[i] / path
        if not target.exists():
            raise ValueError(f'Ruta inexistente: {target}')
        label = target.relative_to(roots[i]).as_posix()
        if target.is_dir():
            label += '/'
        relative = Path(os.path.relpath(target, output.parent)).as_posix()
        return f'[{label}]({quote(relative)})'

    def cell(i, paths):
        return '<br>'.join(link(i, p) for p in paths) or '—'

    table_header = ['| re3 | reVC | reSA | Descripción |', '| --- | --- | --- | --- |']
    lines = ['# Correlación de fuentes: re3 · reVC · reSA', '',
             f'Inventario local generado el {date.today().isoformat()}.', '',
             '| Proyecto | Rama | Raíz examinada | Archivos |', '| --- | --- | --- | ---: |']
    for name, branch, root, files in zip(('re3', 'reVC', 'reSA'), ('Re3', 'ReVC', 'ReSA'), roots, inventories):
        lines.append(f'| {name} | `{branch}` | `{root.as_posix()}` | {len(files):,} |')
    lines += ['', 'reSA corresponde a `source/game_sa` del proyecto gta-reversed-master. Las otras columnas corresponden a las copias locales indicadas; la columna Rama es la rama de git de cada copia (comprobada el 05/10/2026: `gta3` está en `Re3` y `gtavc` en `ReVC`).', '',
              '**Navegación:** [Carpetas](#carpetas) · [Nombres distintos](#funcionales) · [Coincidencias con reSA](#coincidencias) · [Otros archivos re3/reVC](#otros) · [Archivos solo por nombre en reSA](#resto-sa)', '',
              '## Cómo leer la correlación', '',
              '- Cada ruta es un enlace al archivo o carpeta. Los enlaces relativos solo funcionan si los tres repositorios son carpetas hermanas; hoy re3/reVC están en `E:/Original` y reSA en `D:/Original`, así que esos enlaces no resuelven y al regenerar hay que pasar `--re3`/`--revc`.',
              '- **Mapa de carpetas y relaciones funcionales:** orientación manual por responsabilidad. Puede ser uno a varios; no certifica equivalencia de clases, algoritmos, formatos ni comportamiento.',
              '- **Inventario por nombre:** agrupa nombres de archivo iguales, sin distinguir mayúsculas/minúsculas y conservando la extensión. No compara el contenido ni demuestra que sean equivalentes.',
              '- **Descripción:** explica el propósito del archivo o familia, distinguiendo declaraciones, implementaciones y recursos. Es orientación basada en nombre y ubicación, no una auditoría de cada función. En coincidencias nominales se toma reSA como referencia cuando está presente; las responsabilidades pueden diferir entre juegos.',
              '- **—** significa que no hay una ruta para esa relación o un archivo con ese nombre en el árbol examinado. No significa que el juego carezca de esa función.',
              '- Si hay varios archivos con el mismo nombre en un proyecto, se muestran todos en la misma celda. Se incluyen cabeceras, fuentes, shaders, recursos y documentación.',
              '- Las relaciones funcionales pueden reaparecer en el inventario: este último cuenta cada archivo una sola vez y mantiene separadas las coincidencias nominales de las aproximaciones.', '',
              '<a id="carpetas"></a>', '## Mapa de carpetas por responsabilidad', '',
              'En reSA muchos sistemas que re3/reVC agrupan en `core`, `control`, `renderer` o `vehicles` siguen en la raíz de `game_sa`. Las rutas siguientes son puntos de entrada representativos.', '', *table_header]
    for old, sa in FOLDERS:
        lines.append('| ' + ' | '.join([cell(0, [old]), cell(1, [old]), cell(2, sa), FOLDER_DESCRIPTIONS[old]]) + ' |')
    lines += ['', '`skel/` contiene integración de aplicación/plataforma. Su zona comparable en este proyecto es `source/app`, fuera del árbol reSA solicitado. Lo mismo ocurre con parte de las herramientas de `extras/`, cuya zona comparable es `source/toolsmenu`. `fakerw/` y `RenderWare/rw/` se relacionan con la interfaz de RenderWare, pero no implican el mismo backend ni la misma implementación.', '',
              '<a id="funcionales"></a>', '## Correspondencias funcionales con nombres distintos', '']
    for title, old, sa, explanation in FUNCTIONAL:
        existing = [[p for p in old if (roots[i]/p).is_file()] for i in (0, 1)]
        if not any(existing):
            raise ValueError(f'Relación sin origen: {title}')
        lines += [f'### {title}', '', *table_header,
                  '| ' + ' | '.join([cell(0, existing[0]), cell(1, existing[1]), cell(2, sa), f'{title}. {explanation}']) + ' |', '']
    names = defaultdict(lambda: [[], [], []])
    for i, files in enumerate(inventories):
        for path in files:
            names[path.name.casefold()][i].append(path.relative_to(roots[i]))
    matched, old_only, sa_only = [], [], []
    for key, columns in sorted(names.items()):
        entry = (key, columns)
        if columns[2] and (columns[0] or columns[1]):
            matched.append(entry)
        elif columns[0] or columns[1]:
            old_only.append(entry)
        else:
            sa_only.append(entry)

    def rows(entries):
        return ['| ' + ' | '.join([*(cell(i, col) for i, col in enumerate(columns)), describe_file(columns).strip()]) + ' |'
                for _, columns in entries]

    lines += ['## Resumen del inventario nominal', '',
              '| Grupo | Nombres de archivo |', '| --- | ---: |',
              f'| Coinciden reSA y al menos uno de re3/reVC | {len(matched)} |',
              f'| Presentes en re3/reVC sin el mismo nombre en reSA | {len(old_only)} |',
              f'| Presentes en reSA sin el mismo nombre en re3/reVC | {len(sa_only)} |', '',
              'Una fila puede contener varias rutas. Los totales de archivos de las tres raíces aparecen al principio.', '',
              '<a id="coincidencias"></a>', '## Archivos con el mismo nombre en reSA y re3/reVC', '',
              'Orden alfabético por nombre de archivo. Son candidatos para comparar; validar clases y contenido antes de trasladar código.', '',
              *table_header, *rows(matched), '',
              '<a id="otros"></a>', '## Archivos de re3/reVC sin el mismo nombre en reSA', '',
              'Algunos tienen una correspondencia con otro nombre en la sección funcional anterior.', '',
              '<details>', f'<summary>Mostrar {len(old_only)} nombres de archivo</summary>', '',
              *table_header, *rows(old_only), '', '</details>', '',
              '<a id="resto-sa"></a>', '## Archivos de reSA sin el mismo nombre en re3/reVC', '',
              'Agrupados por carpeta de reSA. Incluye reorganizaciones y sistemas con nombres distintos; no es una lista de características exclusivas de San Andreas.', '']
    groups = defaultdict(list)
    for entry in sa_only:
        groups[entry[1][2][0].parent.as_posix()].append(entry)
    for folder, entries in sorted(groups.items()):
        lines += ['<details>', f'<summary>{folder}/ — {len(entries)} nombres</summary>', '',
                  *table_header, *rows(entries), '', '</details>', '']
    lines += ['## Actualizar', '', 'Desde la raíz de gta-reversed-master:', '', '```powershell',
              'python contrib/correlate_sources.py', '```', '',
              'El generador solo lee los tres árboles y escribe este documento. Las relaciones funcionales se mantienen explícitamente en `contrib/correlate_sources.py`; las descripciones, en `contrib/source_descriptions.py`. El inventario se obtiene de los archivos existentes.', '']
    output.write_text('\n'.join(lines), encoding='utf-8', newline='\n')
    print(f'Generado: {output}; archivos: {[len(fs) for fs in inventories]}; filas de inventario: {len(names)}')


if __name__ == '__main__':
    repo = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--re3', type=Path, default=repo.parent/'gta3/src')
    parser.add_argument('--revc', type=Path, default=repo.parent/'gtavc/src')
    parser.add_argument('--resa', type=Path, default=repo/'source/game_sa')
    parser.add_argument('--output', type=Path, default=repo/'docs/Correlacion_re3_reVC_reSA.md')
    args = parser.parse_args()
    generate([args.re3, args.revc, args.resa], args.output)
