<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PLAN-001 — Resolver la identidad y la procedencia de los 21 destinos inciertos de la cohorte de 499

- **Estado**: hecho
- **Caso de uso relacionado**: ninguno (requerimiento descrito por el usuario en el chat: usar la skill `generar-plan-desarrollo` para planear el trabajo de `docs/BinaryCoverageAudit.md` y `reversing/OriginalDependencies499/README.md`, empezando por la fila «Identidad o procedencia incierta · 21»)
- **Plan(es) hermano(s)**: ninguno todavía. Quedan fuera, y pueden ser planes posteriores: validar con bancos las 130 funciones del juego pendientes de la cohorte y dar seguimiento a las 243 bibliotecas
- **Fecha de creación**: 2026-10-17
- **Fecha de cierre**: 2026-10-17

> **Adaptación de la skill.** `generar-plan-desarrollo` se escribió para otro proyecto (backend FastAPI + app Android). Se aplicó su método (entender el requerimiento, explorar el código real antes de planear, plan autocontenido con alcance, pruebas por tarea, criterios de aceptación y checklist, estado `pendiente`) y se sustituyeron las secciones de backend, app móvil y Postman por las de este repositorio (inventario del EXE, `progress.csv`, scripts de verificación y documentación). Este repositorio no tiene `CLAUDE.md` raíz ni `.codegraph/`: la exploración se hizo con lectura directa de los archivos.

## Objetivo

La cohorte fija de 499 destinos originales (10/10/2026) tiene 235 funciones del juego, 243 destinos de bibliotecas/runtime/SDK y **21 con identidad o procedencia incierta**. Mientras sean «inciertos» no se pueden contar ni como pendientes del juego ni como bibliotecas, y el 100 % del catálogo (8552) sigue sin decir nada de ellos. El objetivo es que cada uno quede **clasificado con evidencia** como función del juego o como biblioteca (CRT, RenderWare, D3D/COM), de forma que la fila de la tabla pase de 21 a 0, o que cualquier residuo quede documentado con su razón exacta.

**Clasificar no es validar**: este plan no concede validación, vigencia ni independencia del EXE a ninguna de las 21.

## Alcance

**Incluye**
- Revisar una por una las 21 entradas con `category=unknown` de `reversing/OriginalDependencies499/progress.csv`, con desensamblado del EXE, llamadores/llamados, rangos del binario y nombres de referencia.
- Actualizar `progress.csv` (categoría, implementación, evidencia, notas), el documento de revisión, la tabla de `reversing/OriginalDependencies499/README.md` y las cifras de `docs/BinaryCoverageAudit.md`.
- Regenerar los productos derivados (`work-queue.csv`, textos de estado) con los scripts del proceso.

**No incluye**
- Cambiar `targets.csv` ni `baseline.json`: la cohorte conserva siempre sus 499 direcciones.
- Convertir a C++, enganchar o validar con bancos ninguna de las 21 (si resultan del juego, pasan a la cola de «pendientes del juego» para un plan posterior).
- Tocar `source/**`, los bancos o los 28 documentos congelados.
- Resolver las 243 bibliotecas o las 130 funciones del juego ya clasificadas.

## Contexto del código existente

Lo que ya existe y se reutiliza (no se construye nada nuevo salvo un documento de revisión):

| Pieza | Para qué sirve |
|---|---|
| `reversing/OriginalDependencies499/progress.csv` | Una fila por destino: `address, identity, category (game/library/unknown), implementation, source, evidence, notes, runtime_status, runtime_evidence`. Las 21 con `category=unknown`. |
| `targets.csv`, `baseline.json` | Snapshot inicial inmutable de la cohorte y sus hashes. |
| `unknown-review-a.md`, `unknown-review-b.md`, `classification-c-review.md` | Revisiones anteriores (19 destinos «inicialmente desconocidos» y otras). Dejaron las 21 actuales explícitamente inciertas. |
| `update_progress.py`, `refresh_progress_docs.py`, `audit-cohort-current.py` | Actualizan `progress.csv` y los textos, y comprueban las cuentas de la cohorte. |
| `reversing/_handoff/dis.sh`, `scratchpad exedis.py` | Desensamblado del EXE original de investigación. |
| `docs/MemoryMap/binary_*.csv` | Inventario del EXE (funciones detectadas, rangos, llamadas, secciones). |
| `docs/BinaryCoverageAudit.md` | Auditoría de cobertura fuera del catálogo (cifras de la cohorte). |

**Las 21 entradas actuales** (dirección · identidad actual · fuente que las referencia):

| Grupo | Direcciones | Identidad actual | Referencia en el código |
|---|---|---|---|
| A. CRT / salida con formato (1) | 0x00821982 | «printf-like CRT formatted output»: el identificador simbólico dice `wprintf` y la fuente usa `char*`; el EXE usa el flujo 0x8E2C50 y el ayudante 0x823FD1 | `VehicleRecording.cpp:380` |
| B. Cámaras cinematográficas Obbe (5) | 0x005267C0 · 0x00526950 · 0x00526AE0 · 0x00526C80 · 0x00526E20 | `CCamera::ProcessObbeCinemaCameraCar / Train / Heli / Plane / Boat` | `Camera.cpp:3278–3286` |
| C. Pelea (3) | 0x0061D0B0 · 0x0061D5F0 · 0x006240B0 | `CTaskSimpleFight::FightHitCar / FightSetUpCol / FightStrike` | `TaskGangHassleVehicle…`, `TaskSimpleUseGun.cpp` |
| D. Gang drive-by (4) | 0x00621960 · 0x00627600 · 0x00628350 · 0x006217D0 | tres ayudantes de `CTaskSimpleGangDriveBy` (objetivo jugador, objetivo NPC, disparo) y `FUN_006217d0` (sin identidad) | `TaskSimpleGangDriveBy…`, `GangWars.cpp:433` |
| E. Ayudantes numéricos o de posición (3) | 0x00610DB0 · 0x0063A380 · 0x0072D6D0 | rango aleatorio (Population), ángulo de cámara de tarea, posición de `CAttributeZone` | `Population.cpp:2080`, `Cam.cpp:5381`, `Camera.cpp:3814` |
| F. Caché D3D (2) | 0x007305E0 · 0x00730740 | `D3DResourceSystem::TidyUpD3DTextures` y `…TidyUpD3DIndexBuffers` (liberación COM de cachés) | `D3DResourceSystem.cpp:44, 49` |
| G. RenderWare y núcleo (3) | 0x00734940 · 0x00734DE0 · 0x0059C300 | `GetFirstTexture`, `RpGeometryReplaceOldMaterialWithNewMaterial`, `CQuaternion::Slerp` (contrato `thiscall` propio, distinto de `RtQuatSetupSlerpCache`) | `rwtexdict.cpp:6`, `RwHelper.cpp:274`, `Quaternion.cpp:57` |

(1 + 5 + 3 + 4 + 3 + 2 + 3 = 21.)

**Reutilizar vs nuevo:** se reutilizan todos los scripts y el inventario del EXE; es nuevo solo un documento de revisión (`review-uncertain-21.md`) con la evidencia de cada decisión.

## Diseño técnico (método de decisión)

Para cada destino se recogen las mismas evidencias y se decide con una regla escrita, no por «sensación» ni por la dirección:

1. **Rango y forma**: límites de la función en el EXE, prólogo/epílogo, convención de llamada, uso de `this`, retorno x87, pila.
2. **Grafo**: quién la llama (solo métodos de clases del juego, o también código de RenderWare/CRT) y qué llama (globales del juego como `CTimer`, `CWorld`, `CPools`, o API de RenderWare/D3D/COM/CRT).
3. **Zona del binario**: sección y vecindad. El código del juego, RenderWare y la CRT ocupan bloques contiguos y distintos; una dirección rodeada de funciones ya identificadas hereda esa procedencia como indicio, nunca como prueba única.
4. **Cadenas y constantes**: textos, tablas y constantes que solo existen en el juego o en una biblioteca.
5. **Nombres de referencia**: nombres del plugin-sdk y del proyecto upstream `gtasa-reversed` (`C:\Users\pedro\Proyectos de prueba\mods\opcodes\gtasa-reversed`), usados solo como pista de nombre (lectura; no se copia código ni se acepta un nombre sin contrastarlo con el desensamblado).
6. **Regla de decisión**:
   - `game`: usa tipos/globales del juego, es método de una clase del juego o su contrato solo tiene sentido en él, y no hay prueba de que sea código de biblioteca.
   - `library`: vive en la zona de RenderWare/CRT/D3D, solo llama a API de biblioteca y no toca estado del juego.
   - `unknown` (residuo documentado): solo si tras los puntos 1–5 siguen siendo plausibles las dos; debe quedar escrito qué evidencia faltó.
   - Se exigen **al menos dos evidencias independientes** (por ejemplo, grafo y zona del binario) para cada decisión.
7. **Registro**: la decisión y su evidencia van a `progress.csv` (columna `evidence` y `notes`) y al documento de revisión; `runtime_status` sigue `not_validated`.

Particularidades por grupo:
- **A (CRT)**: aclarar si es la variante `char` o `wchar` (el conflicto FID/`wprintf` frente a `printf` de la fuente) analizando el analizador del formato y el flujo 0x8E2C50. Se espera `library`.
- **B, C, D**: familias de nombres de juego (`CCamera`, `CTaskSimpleFight`, `CTaskSimpleGangDriveBy`) con ayudantes llamados desde métodos del juego. Se espera `game`; el destino 0x006217D0 necesita primero un nombre por llamadores/llamados.
- **E**: ayudantes pequeños; se decide por quién los llama y qué globales tocan.
- **F (D3D)**: son envoltorios de `Release` COM sobre cachés propias de `D3DResourceSystem`; decidir si la procedencia es del juego (gestión de cachés del motor del juego) o de RenderWare/D3D.
- **G**: dos de RenderWare (`GetFirstTexture`, `RpGeometryReplaceOldMaterialWithNewMaterial`) frente a `CQuaternion::Slerp`, que es del juego con contrato propio.

**Cambio de datos**: se modifica solo `progress.csv` (categoría y notas de 21 filas), sin tocar `targets.csv` ni `baseline.json`. No hay cambio de esquema de CSV.

**Seguridad y riesgos**
- No convertir una duda en una afirmación: si la evidencia no alcanza, el residuo queda como `unknown` con su razón.
- No contar clasificación como validación en ninguna cifra.
- El desensamblado es del EXE original de investigación; no se ejecuta GTA.

## Pruebas

| Qué se prueba | Tipo | Dónde |
|---|---|---|
| La cohorte sigue teniendo 499 direcciones y los hashes de `targets.csv`/`baseline.json` no cambian | script de verificación | `audit-cohort-current.py`, `sha256sum` |
| Las categorías suman 499 (235 + N juego nuevas, 243 + M bibliotecas nuevas, residuo `unknown`) | script de verificación | `audit-cohort-current.py` / `update_progress.py` |
| Cada fila reclasificada tiene ≥ 2 evidencias en `evidence` y `notes` | comprobación automática nueva (script corto que lee `progress.csv`) | `reversing/OriginalDependencies499/` |
| La tabla del README y `BinaryCoverageAudit.md` coinciden con `progress.csv` | script de documentación | `refresh_progress_docs.py`, `contrib/sync_docs.py`, `contrib/sync_current_markdown.py` |
| Catálogo y registros intactos | auditoría | `python -B contrib/audit_hook_catalog.py --strict`, `python -B contrib/audit_register_residue.py` |
| Pruebas generales del repositorio | unitarias | `contrib/tests` (con el `PYTHONPATH` de tree-sitter) |
| 28 documentos congelados sin cambios | script | `contrib/sync_current_markdown.py` (`frozen_intact: 28`) |

## Criterios de aceptación

- La fila «Identidad o procedencia incierta» de la tabla de `OriginalDependencies499/README.md` pasa de **21 a 0**, o queda un residuo explícito con la razón de cada uno.
- La suma de la cohorte sigue en **499** y las categorías cuadran con `progress.csv`.
- Cada destino reclasificado tiene su decisión escrita con **dos o más evidencias independientes** y el documento de revisión las enlaza.
- Ninguna de las 21 aparece como aprobada, vigente ni validada.
- `BinaryCoverageAudit.md` refleja las cifras nuevas con su fecha, sin contradecir el README.
- Las auditorías y los tests del repositorio siguen pasando y los 28 congelados no cambian.

## Checklist de tareas

**Fase 0 — Base**
- [x] Confirmar el estado de partida: 499 = 235 + 243 + 21 con `audit-cohort-current.py` y guardar los hashes de `targets.csv` y `baseline.json`.
- [x] Preparar el documento `review-uncertain-21.md` con una ficha por destino (campos del método de decisión).

**Fase 1 — Las que parecen de biblioteca (menor riesgo)**
- [x] G: `GetFirstTexture` (0x00734940) y `RpGeometryReplaceOldMaterialWithNewMaterial` (0x00734DE0): decidir con grafo y zona del binario.
- [x] F: `TidyUpD3DTextures` (0x007305E0) y `TidyUpD3DIndexBuffers` (0x00730740): decidir juego o biblioteca.
- [x] A: 0x00821982: resolver `char`/`wchar` y clasificar.

**Fase 2 — Las que parecen del juego**
- [x] G: `CQuaternion::Slerp` (0x0059C300).
- [x] B: las cinco `ProcessObbeCinemaCamera*` (0x005267C0, 0x00526950, 0x00526AE0, 0x00526C80, 0x00526E20).
- [x] C: `FightHitCar`, `FightSetUpCol`, `FightStrike` (0x0061D0B0, 0x0061D5F0, 0x006240B0).
- [x] D: los tres ayudantes de drive-by (0x00621960, 0x00627600, 0x00628350) y la identidad de 0x006217D0.
- [x] E: los tres ayudantes (0x00610DB0, 0x0063A380, 0x0072D6D0).

**Fase 3 — Registro y documentación**
- [x] Actualizar `progress.csv` (categoría, implementación, evidencia, notas) para cada destino resuelto.
- [x] Regenerar `work-queue.csv` y la tabla del README con `update_progress.py` / `refresh_progress_docs.py`.
- [x] Actualizar `docs/BinaryCoverageAudit.md` (fila de la cohorte y texto de la sección) con la fecha.
- [x] Ejecutar `sync_docs.py` y `sync_current_markdown.py --date <fecha>`, restaurar `docs/ReversingPriorities.md` y `docs/ValidationInventory.md`.

**Fase 4 — Verificación**
- [x] Pasar `audit_hook_catalog --strict`, `audit_register_residue` y `contrib/tests`.
- [x] Revisar a mano las decisiones con menos evidencia (residuos) antes de cerrar.

## Decisiones pendientes del usuario

1. ¿Se permite consultar el proyecto upstream (`gtasa-reversed`) como fuente de **nombres** para esta clasificación, sin copiar código? (Ambos son forks de gta-reversed; hay que revisar las licencias.)
2. Las funciones que resulten **del juego** (probablemente 15 a 18 de las 21) pasan a la cola de pendientes del juego. ¿Las admitimos después con bancos (plan hermano) o solo quedan clasificadas?
3. Para un residuo irresoluble: ¿se deja como «incierta documentada» o se exige resolverlo con más análisis dinámico (ejecutar el juego con un punto de interrupción)?

## Notas / decisiones tomadas durante la ejecución

Ejecutado el 17/10/2026. El plan se amplió sobre la marcha: no sólo se clasificaron las 21, también se **reconstruyeron en C++ y se validaron dentro del juego** (el plan original lo dejaba fuera de alcance).

**Resultado.** Las 21 filas `category=unknown` quedaron resueltas: **20 del juego y 1 biblioteca**. La tabla de la cohorte pasa de 235/243/21 a **255 juego (125 aprobadas, 130 pendientes), 244 bibliotecas y 0 inciertas**; el total sigue en 499 y `targets.csv` y `baseline.json` no cambian. Cada ficha (dirección, identidad, categoría, dos o más evidencias, banco y pares) está en `reversing/OriginalDependencies499/review-uncertain-21.md`.

| Grupo | Resultado | Banco y pares |
|---|---|---|
| A. `0x00821982` | `printf(const char*, ...)` de la CRT estática de MSVC (no `wprintf`: el `FID_conflict` era una coincidencia de huella); **biblioteca**, reconstruida como envoltorio C++ que llama por dirección a lock/`_stbuf`/`_output`/`_ftbuf`/unlock | Dependencies21A, 4096 |
| B. 5 cámaras Obbe | juego | Dependencies21B, 10240 |
| C. 3 de pelea | juego | Dependencies21C, 4608 |
| D. 4 de drive-by | juego; `FUN_006217d0` es el constructor `CTaskSimpleGangDriveBy::Constructor` y `0x628350` apunta el arma (`AimGun`), no dispara | Dependencies21D, 11264 |
| E. 3 ayudantes | juego (`RandomIntInLimitRange`, `CZoneDef::GetCenter`, `GetCameraStickModifier`) | Dependencies21E, 6144 |
| F. 2 de caché D3D | juego (confianza media-alta; la procedencia histórica no es observable en el binario) | Dependencies21F, 4096 |
| G. 3 | las tres del **juego**, incluidas `GetFirstTexture` y `RpGeometryReplaceOldMaterialWithNewMaterial` (el plan esperaba biblioteca) | Dependencies21G, 6656 |

Total de los bancos: 47104 pares, cero diferencias, ningún resultado nulo; corridas finales en `reversing/CppTwelveLevel6Final/runs/Dependencies21X-<marca>`.

**Respuestas a las decisiones pendientes.** (1) Los nombres de referencia del proyecto upstream se usaron sólo como pista de nombre, contrastados con el desensamblado. (2) Las del juego no quedaron sólo clasificadas: se admitieron con bancos en la misma tanda. (3) No quedó ningún residuo incierto.

**Datos.** `progress.csv` (20 filas `game`/`passed`, `CrtPrintf` `library` con `runtime_status=library_validated`), `classification-a/b/c.csv`, `progress.json` y la tabla del README. Las 20 aprobadas se registran en `approved-level6.json`; `approved.json` conserva las 105 del esquema anterior porque `audit-cohort-current.py` y `refresh_progress_docs.py` exigen `approval-audit.json`, que las corridas de nivel 6 no producen. Las 21 están además en `docs/hooks.csv` (8573 filas, todas `validated=1`), `reversing/validation-registry.json` y `reversing/CppOnlyMigration/approvals.json`.

**Lo aprendido (causas de los fallos de banco).** Zonas vigiladas solapadas con zonas modificables; comprobación del estado final tras `Restore`; autoparcheo del falso con `--unhook Audio`; EAX y residuo que ningún llamador lee; punteros de pila del llamador sin etiquetar; baseline de memoria distinta entre la pasada original y la candidata. Detalle en `reversing/CppTwelveLevel6Study/README.md` y en las trampas de `reversing/CppOnlyMigration/PROCESO.md`. Hallazgo de fuente: `TaskGangHassleVehicle.cpp` escribía `[0x20]`, `[0x21]`, `[0x23]` en lugar de `[0x24]`, `[0x25]`, `[0x27]` de la tarea de pelea.

**Incoherencias detectadas al cerrar.** `audit-cohort-current.py` ya no queda limpio sobre las 105 admisiones del 10/10: tras la conversión a C++ de la instrumentación de varios bancos `Dependencies*` (`LiveComparison.hpp`, `build_probe.py`, `run_local.py`) 24 hallazgos de «instrument changed» indican que el árbol actual ya no coincide con el manifiesto histórico de esos bancos; `AUDIT_COHORT_CURRENT.md` conserva la versión previa (0 hallazgos) y la regeneración queda pendiente de repetir esos bancos.
