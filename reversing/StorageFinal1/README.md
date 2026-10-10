<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8343 con evidencia histórica, 0 con vigencia acreditada y 103 pendientes en 2 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# StorageFinal1: banco por pares de CGenericGameStorage (funciones pendientes)

Banco simple (flujo viejo) para `SaveDataToWorkBuffer-org` (0x5D1270), `LoadDataFromWorkBuffer-org` (0x5D1300), `ReportError` (0x5D08C0), `GenericSave` (0x5D13E0) y `GenericLoad` (0x5D17B0). **Escrito, sin compilar ni ejecutar.** Sustituye y amplía el análisis de `RequestedStorageWorkBuffer` (que no cruzaba el umbral de capacidad).

Uso: `bash reversing/_handoff/bench.sh StorageFinal1 3`. Filtro opcional: `storagefinal1-only.txt` con `CGenericGameStorage::<Funcion>`.

## Corpus: 4984 pares

| Funcion | Pares | Dominio |
|---|---|---|
| `SaveDataToWorkBuffer-org` | 1620 | 9 tamaños (−1…40000) × 10 posiciones (0…51200, incluidas las que cruzan 0xC800) × fallo previo × 3 modos de vaciado (acierto / fallo / fallo con `ms_bFailed`) × 3 `ms_WorkBufferSize` |
| `LoadDataFromWorkBuffer-org` | 1620 | igual, con recarga simulada; `ms_WorkBufferSize` ∈ {0xC800, 51000, 40000} para distinguir el umbral variable del resto fijo |
| `ReportError` | 208 | 26 bloques × 8 códigos (0,1,2,3,4,255,−1,INT_MAX) |
| `GenericSave` | 768 | totales de datos desde 0 hasta 230000 bytes (cruzando varios búferes y el relleno hasta 202748), encabezados partidos en la frontera del búfer, fallos de apertura/vaciado/bloque/cierre |
| `GenericLoad` | 768 | fichero virtual de 28 bloques con encabezado corrupto opcional, recargas con fallo (con y sin `ms_bFailed`), fallo de bloque, `CPools::Load` falso/verdadero, fallo de cierre |

## Seguridad: sin E/S real

`GenericSave`/`GenericLoad` se ejecutan con **todo** lo que toca ficheros o el mundo sustituido por dobles trazados (parche en la dirección del EXE y en el cuerpo de la DLL): `OpenFileForWriting/Reading`, `SaveWorkBuffer`, `LoadWorkBuffer`, `CFileMgr::CloseFile`, `CPad::UpdatePads`, `CCheat::ResetCheats`, `CSimpleVariablesSaveStructure::Construct/Extract`, `ReportError` (en Load), `DoGameSpecificStuffAfterSucessLoad`, `DoScriptSetupAfterPoolsHaveLoaded` y los 24 guardados y 24 cargas de bloque. Ningún guardado (ni `userfiles`) se abre: el "fichero" es un `std::vector` y el "FILE*" un token opaco. Si el banco no localiza un cuerpo de la DLL o dos se solapan, escribe un evento `error` **antes de parchear nada**. El runner además compara los hash de los guardados antes y después. El búfer de trabajo de Save/Load lo reserva el doble de apertura con `new[]` y lo libera la función bajo prueba con `delete[]` (si no lo libera, lo hace el banco tras anotar `work_null`).

`ReportError` en el EXE solo formatea con `sprintf` (0x821BB5, parcheado) en un búfer local; en la DLL emite el mismo texto por spdlog (captura con un sumidero temporal del logger por defecto). Los bloques 25, 26 y ≥28 quedan fuera: el EXE usa un búfer sin inicializar.

## Notas

- `ms_WorkBufferSize` se mantiene en 0xC800 en `GenericLoad` (con otro valor el EXE lee más allá de la cabecera de 5 bytes: dominio no cuerdo); su efecto se cubre en `LoadDataFromWorkBuffer-org`, donde `pos` se ajusta cuando el resto fijo (0xC800 − pos) superaría el tamaño pedido.
- La rama NOTSA de `GenericLoad` (versión de guardado distinta) no se ejercita: el doble de `Extract` devuelve la versión actual.
