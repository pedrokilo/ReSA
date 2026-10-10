<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Continuidad: validaciones detenidas por el usuario

No reanudar GTA, compilaciones ni validaciones automáticamente. Este documento conserva el estado para una futura solicitud explícita de continuación. No se realizó commit desde este subagente.

Estado comprobado al cierre: 131 aprobaciones incorporadas en el alcance de la solicitud; registro con 6.867 identidades históricas. Las aprobaciones están ligadas a sus fuentes/ASI y ámbitos registrados; no acreditan automáticamente las fuentes actuales ni colaboradores/integración. Pad.cpp actual `89badf36b91db868c0a9d41408db7b595bc116ce941a7864cd697af8a4bbaa4d` contiene la corrección autorizada de Train y aritmética x87, todavía sin aprobación nueva de esas funciones. Pad.h `d18adb99148fe3fbdde2447a44699451f3afce97f29994bdf3bd29a3829889e2` conserva Train CVector2D por valor.

## Prioridad al reanudar explícitamente

| Banco | Estado conservado | Casos | Acción pendiente |
|---|---|---:|---|
| RequestedPadDistanceTrainNopHexV2 | Compilado y revisión estática aprobada; último intento rechazado por disco lleno | 13.952 | Comprobar espacio/manifiesto y ejecutar banco positivo completo; auditar antes de registrar |
| RequestedPadUpdateMouseV2 | Compilado; revisión de llamadas y ABI actual aprobada estáticamente | 9.216 | Comprobar manifiesto y ejecutar completo; auditar antes de registrar |
| RequestedStorageWorkBuffer | Preparado, sin compilación ni ejecución | 2 × 1.024 | Revisar/congelar fuentes vigentes, compilar y comprobar llamadas actuales antes de ejecutar |

EXE canónico: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`, copia local `D:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe`.

### Distance / Train / NOP Hex V2

Artefactos en `reversing/RequestedPadDistanceTrainNopHexV2/artifacts`:

- ASI `390debbf008d3e602ce8f36a3d101580b530c2cf3fb9cae386f4685411a9cbe4`.
- PDB `cfe44076920938d9d629946adfce5dfb0d7de9442a721b0372f7c394bb2abda5`.
- Prueba Train ABI `d69193001858690025acb8443a1be1774e96843f657d69bf6b397c830dd7d7ad`.
- instrument-freeze.json `54edc2c7541b597bfe5f29cefc0b1f46997b230c71c89de3b92d37697c6e8fbd`, lista cerrada de 39 entradas.

Revisión actual en `reversing/RequestedPadSourceProposals/compiled-corrected/static-preflight-review.json` y `distance-train-ftol-env-current.json`: ambas funciones usan aritmética x87 sin operaciones/comparaciones SSE; Distance RET20 y Train RET8. FTOL local es idéntico en sus 117 bytes al helper canónico, SHA `5333b186c02836974c6f792303aeb2c00d856316b93ccbbe65f51def6ae661b4`. Captura FNSTENV seguida inmediatamente por FLDENV; restauración FLDENV y MXCSR con comparación de los 28 bytes completos. Esto demuestra el código compilado, todavía no una restauración runtime exitosa.

Último intento: `reversing/RequestedPadDistanceTrainNopHexV2/runs/20261008-170728-931505`, `passed=false`, `[WinError 112] Espacio en disco insuficiente`; saves/configs restaurados. No contiene aprobación completa ni cierre normal acreditado. Conservarlo rechazado y crear intento nuevo.

Intentos previos conservados: revision2 `RequestedPadDistanceTrainNop/runs/20261008-161553-454998` incompleto por timeout; Hex V1 `RequestedPadDistanceTrainNopHex/runs/20261008-164703-580513` completo pero rechazado por restauración x87 C0 (32→288). Su modelo nativo pasa los 13.952 casos; diferencias candidato: NOP0, Distance1.280 sólo FP, Train414 (pad234/FP318 superpuestos). `split-diagnostic.json` conserva el detalle. Ninguno acredita funciones parcialmente. La propuesta/fuente anterior y aplicación autorizada están archivadas en `RequestedPadSourceProposals/DistanceTrainX87Combined`.

Comando de futura continuación, sólo después de autorización y de comprobar todos los hashes del manifiesto:

```powershell
python -B reversing/RequestedPadDistanceTrainNopHexV2/run_local.py
```

El wrapper `run_negative_baseline.py` siempre devuelve fallo; no usarlo para el replay positivo de la fuente corregida. Si cambia una entrada compilada o congelada, revisar la causa y reconstruir; no sustituir hashes ciegamente.

### Mouse V2

Artefactos en `reversing/RequestedPadUpdateMouseV2/artifacts`: ASI `05c66666a3e653eeaeab592fe4a7341ec1531703c253f20a48307ca9e53c5684`, PDB `30d9024bfe702dadf9e9be6794e6e35a471806b6b4e8275517b75b446cc43937`. El manifiesto incorpora Pad.cpp actual `89badf…`. `Diagnostics/compiled-runtime-proof.json` SHA `52cb58a24a89da95a79a72f89c40d9f6f3ace7a6fdc4739c45935ad58605b972`: matching RSDS/PDB, llamadas reales y ABI separadas de GetMouse comprobadas; todavía sin crédito runtime del banco V2.

```powershell
python -B reversing/RequestedPadUpdateMouseV2/run_local.py
```

### Storage

`reversing/RequestedStorageWorkBuffer/corpus-plan.json`: SaveDataToWorkBuffer-org 5D1270 y LoadDataFromWorkBuffer-org 5D1300, 1.024 casos cada una. Ámbito: buffers privados no solapados, capacidad51.200, tamaños -1/0/1/2/3/4/7/16, posiciones/gates declarados; no refill/flush/archivos/helper credit. `prepared-freeze.json` SHA `cb6f443bd4569add7f294592e51068f27cea7d0d748b986f5edd5390c6ea8f27`. No ASI/PDB aprobado disponible.

```powershell
python -B reversing/RequestedStorageWorkBuffer/build_probe.py
python -B reversing/RequestedStorageWorkBuffer/inspect_compiled.py
```

El inspector no tiene CLI de argumentos: lee ASI/PDB y genera Diagnostics. Revisar su selección de símbolos: su máscara actual DataToWorkBuffer cubre Save; confirmar también explícitamente LoadDataFromWorkBuffer y ambas llamadas antes de ejecutar run_local.py. Todos los builders/runners son seriales y comparten WinMain/bin/instalación de prueba: nunca ejecutarlos simultáneamente.

## Espacio y archivos de cierre

El coordinador ha trasladado artefactos ignorados de bancos antiguos a `C:\Users\pedro\Tools\ReSA-evidence-archive-20261008`, conservando nombres y manifiestos de hashes: RequestedAnimProjectilePair, RequestedGeometryPlanesV2, RequestedMenuBatch, RequestedPreflightAdditionalLifetimes y RequestedPreflightTaskCtorDtor. No mover artefactos actuales/proofs de HexV2 o MouseV2. Medición al redactar: D24,67GiB libres tras eliminar 1.079 archivos regenerables de build (1,36GiB), C208,10GiB; es una instantánea, volver a comprobar antes de reanudar. No borrar evidencias para recuperar espacio sin preservar copia y comprobar hashes.

HEAD observado: `c62854ef` (el coordinador puede crear posteriormente el commit de cierre). Git mostraba cambios pendientes en validation-registry.json, Pad.cpp y un pyc ya rastreado; este último es caché generado y debe quedar fuera del commit. Había 520 archivos no rastreados, incluidos bancos/fuentes/evidencias textuales legítimos.

Antes del commit, seleccionar rutas explícitas y revisar `git diff --cached --stat`/`git diff --cached --name-only`; no usar un añadido indiscriminado. Excluir ejecutables ASI/PDB/DLL/EXE, backups de instalación como `historical-launch/backup/gta_reversed.asi`, caches pyc/__pycache__, pid.txt, logs del proveedor y colas temporales de REA. ASI no rastreados detectados también en `RequestedFxEmitterTwo/Diagnostics/baseline-before-NULL-source-fix`.

Política de cierre confirmada por el coordinador: Pad.cpp se incorpora como WIP explícitamente no aprobado; los pequeños snapshots nativos `.bytes.bin` y evidencias textuales se incluyen en Git. ASI/PDB, artifacts, backups, caches, tmp y pid permanecen locales/ignorados mediante reglas dirigidas añadidas por el coordinador. No eliminar ni cambiar entradas congeladas al limpiar temporales. Los tres registros raw de Pools de unos45MB pueden conservarse en Git; Distance raw de unos810MB queda excluido y su gzip sin pérdida de unos14MB se conserva con hash/roundtrip. Este subagente no modificó .gitignore, no eliminó archivos, no movió evidencias y no hizo commit.
