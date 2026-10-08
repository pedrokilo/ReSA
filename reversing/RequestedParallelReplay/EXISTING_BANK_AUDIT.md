<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6839 con evidencia histórica, 0 con vigencia acreditada y 1607 pendientes en 63 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Bancos existentes frente al alcance congelado de 1710 pendientes

Auditoría estática del 8 de octubre de 2026. No ejecuta GTA, no compila ni modifica flags o registro. Usa el inventario original RequestedValidation20261008/pending-bank-map.json. No constituye equivalencia funcional.

## Resultado

Las 57 coincidencias exactas/full-label del mapa NO son 57 funciones ya probadas. Son menciones de colaboradores o descriptores de parches. Las otras 279 coincidencias son coocurrencia de clase/nombre y tampoco acreditan función target.

Se buscaron identidades exactas class::fn_name en las claves pairs_per_function y function_pairs de todos los resúmenes inmediatos: **cero coincidencias** (exact-summary-matches.json). Se cruzaron las 1710 identidades contra descriptores de seis campos {clase,nombre,direccion,vtable,slot,casos} en 533 headers inmediatos: **cero coincidencias** (target-descriptors.json). Estos análisis no detectan todas las formas posibles de descriptor; son prueba negativa limitada, complementada por lectura de los principales candidatos.

| Banco candidato | Identidades del mapa | Motivo por el que no cuenta |
|---|---:|---|
| Sys12 | 12 | Render/Draw/audio/shadows figuran en deps[] y se sustituyen mediante RegisterStubs; los targets son funciones de orquestación. |
| MenuMgrC | 10 | Streaming/Radar/Camera/Hud son colaboradores de funciones de menú. InstallStubs sustituye las llamadas. |
| CVehB | 8 | Streaming/Population/Rope/CarEnterExit son colaboradores de CAutomobile/CVehicle. |
| Sprite2d33 | 8 | Las funciones CFont están en targets[] de parches con XColor/etc.; fns[] compara CSprite2d. |
| CPedD | 6 | Audio/Population/Streaming/Scripts/CarEnterExit son colaboradores de CPed. |
| Misc61b | 6 | CAutomobile ProcessAI/Control/Inputs/Suspension/Reset/Setup son bases simuladas en tests de CQuadBike/CBmx. |
| Sys13 | 6 | CAutomobile son bases simuladas de CTrailer; resumen enumera CTrailer. |
| AppBatch | 4 | Inicialización/plugin colaboradores, no selección directa acreditada. |
| CEntB | 2 | CStreaming AddEntity/RemoveEntity son callbacks simulados para CEntity. |

Hay repetición de identidades entre bancos: las filas NO se suman como funciones distintas.

## Próximas sondas directas, ordenadas por alcance pequeño

1. LiveBatch nuevo: CDecision::From 0x6006B0 y CTaskSimpleGangDriveBy::GetTaskType 0x6218B0. El coordinador ya obtuvo primera ejecución con bytes/slots restaurados; requiere revisión propia de dependencias y registro separado. No reinterpretar bancos anteriores como esta evidencia.
2. CTaskComplexWanderStandard::GetWanderType 0x48E5D0, CTaskComplexPlayHandSignalAnim::GetTaskType y getters pendientes de otras tareas. Fixtures privados sin constructor real del juego; comparar virtual y entrada según ABI, verificar ambas vtables y bytes.
3. CPedDamageResponseCalculator::Constructor 0x4AD3F0. Fuente asigna cinco campos y wrapper retorna this: variar entidad, float con representación de bits, weapon/body enums y bool; comparar objeto completo para conservar padding, retorno y memoria del argumento. Constructor real sobre almacenamiento privado.
4. CCustomBuildingRenderer::Initialise 0x5D7EC0, Shutdown 0x5D7EE0 y PluginAttach 0x5D7EF0. Banco directo nuevo con stubs simétricos exclusivamente de pipelines; comparar retornos de ambas llamadas, cortocircuito y orden de trace. Se puede completar una clase de tres pendientes con contrato acotado.
5. CInterestingEvents::InvalidateEvent 0x602960. Fuente limpia time/entity y selected index; fixtures de objeto propio y CEntity/CReference privados, casos puntero ajeno, todos los slots, selected/unselected y entidad null/presente. ClearReference debe ejecutar real cuando se certifique dependencia; una versión con stub sólo certifica contrato local.
6. CInterestingEvents::InvalidateNonVisibleEvents 0x6029C0. Añadir cámara privada con snapshot global, entidades con matriz/transform propios y respuesta LOS simétrica. Requiere inventariar referencias, slots y cámara; no afirmar vigencia de colaboradores con stub.
7. Constructores/destructores de las tareas pendientes. Requieren fixture ABI específico, vtable y referencias; los grandes arrays de vtable taskType de GangFollower355 sólo normalizan resultados y no equivalen a probar esas clases. Destructor GangFollower histórico difiere y queda fuera del alcance solicitado salvo dependencia real.

## Reparaciones de restauración necesarias antes de reutilizar instrumentos

- 119/179 headers del análisis previo emiten state_restored=true literal: no usar ese booleano como evidencia.
- CamAttached355 emite complete antes de que salgan de ámbito EntryPatch Water; exigir destrucción y comparación antes del evento terminal.
- TrainQueries355 restaura oldCounts/oldNodes pero no los compara; inventariar dichos rangos.
- BikeCore355 comprueba timestep y flags pero no bytes de EntryPatch; guardar/verificar entradas y destinos DLL.
- Todo banco que cambie State/LockState debe verificar estado y bytes reales de original/candidato, más slots GTA y DLL para virtuales. Restaurar flag sin slots/bytes no basta.
- Globales, pools privados y parches se inventarían antes de primera mutación; complete sólo después de destruir parches y restaurar/verificar todos los rangos. Retener los fallos históricos.
- El runner restaura archivos ASI/PDB/INI con hashes: eso no sustituye restauración de memoria dentro de GTA.

## Construcción y registro

Toda nueva ejecución debe congelar fuente e instrumentos, fijar _CL_=/MP6 con construcción serial, verificar hashes reales EXE/ASI y agregar revisión de dependencias. probe_summary.py por sí solo genera dependency_review=None; replay aprobado sin esa revisión acredita histórico, no current. No aumentar validado porque un archivo mencione el nombre.
