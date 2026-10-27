<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cargador de ficheros dat/ide/ipl (lista 34)

Banco preparado el 04/10/2026 con **11.968 casos original/candidato dentro de GTA** (pendiente de compilar y ejecutar): 47 funciones de 1 clase.

| Clase | Funciones |
|---|---|
| `CFileLoader` | `LoadLine` (fichero y buffer), `LoadObject`, `LoadTimeObject`, `LoadWeaponObject`, `LoadClumpObject`, `LoadAnimatedClumpObject`, `LoadVehicleObject`, `LoadPedObject`, `LoadTXDParent`, `LoadPathHeader`, `LoadPedPathNode`, `LoadCarPathNode`, `LoadGarage`, `LoadStuntJump`, `LoadCarGenerator` (linea y estructura), `LoadPickup`, `LoadZone`, `LoadCullZone`, `LoadOcclusionVolume`, `LoadAudioZone`, `LoadEntryExit`, `LoadTimeCyclesModifier`, `LoadObjectInstance` (linea e instancia), `FindRelatedModelInfoCB`, `SetRelatedModelInfoCB`, `LoadAtomicFile` (stream y fichero), `LoadClumpFile` (stream y fichero), `StartLoadClumpFile`, `FinishLoadClumpFile`, `LoadTexDictionary`, `AddTexDictionaries`, `LoadCollisionModel` y `Ver2/3/4`, `LoadCollisionFile` (buffer y fichero), `LoadCollisionFileFirstTime`, `LinkLods`, `LoadObjectTypes`, `LoadScene`, `LoadLevel` |

**Diferencias reales corregidas:**
- Lectura de lineas: `sscanf` como el EXE (no `sscanf_s`), sin `VERIFY`/`assert` sobre el recuento y buffers del tamaño de la pila del EXE.
- `LoadLine(buffer)`: el `\r` se copia como espacio (la DLL lo saltaba).
- `LoadAudioZone`: activo = `flags != 0` (la DLL usaba `== 1`); el nombre ocupa 16 bytes.
- `LoadZone`: solo crea la zona si se leen los 10 campos.
- `LoadOcclusionVolume`: formato `%d` y la prueba de interior mira solo `i`,`n`,`t` en `len-7..len-5` (no `ends_with("int.ipl")`).
- `LoadPathHeader`: formato `"%d %d %s"` con buffer.
- `LoadObjectTypes`: tras la cabecera de PATH el indice pasa a 0 (la DLL llamaba siempre a `LoadPathHeader`).
- `LoadScene`: la seccion PATH no procesa la linea (la tabla de saltos del EXE no tiene caso), solo termina la lectura.
- `LoadVehicleObject`: slot "vehicle" antes de leer; tipos y clases con `strcmp` (sin coincidencia no se escribe); `f_heli` es helicoptero sin ruedas; `_`→` ` desde el segundo caracter; `strncpy(8)` del nombre; la frecuencia se escribe salvo en "ignore".
- `LoadPedObject`: `%s` sin limite para la animacion y grupo de animacion buscado en linea con `strcmp` (sin coincidencia: numero de grupos).
- `LoadTimeObject`: `GetTimeInfo` justo tras crear el model info; sin `NOTSA_UNREACHABLE`.
- `LoadCarGenerator(estructura)`: un solo `fmul` por `0x42652EE0` y ultimo argumento `flags & 2` sin normalizar.
- `LoadObjectInstance(instancia)`: `acos` del CRT del EXE (`_CIacos` 0x822380) y constante `0x3FACE9CC` del paso a nivel.
- `LoadCollisionModel`: contadores con signo, `m_nNumLines = 0`, esferas/cajas/triangulos escritos campo a campo como el EXE.
- `LoadCollisionModelVer2/3/4`: sin construir `CCollisionData` (V2 no escribe `+0x20`); sombra con `nShdwFaces > 0` con signo.
- `LoadCollisionFile(buffer)`/`FirstTime`: bucle mientras queden mas de 8 bytes, model info conservado entre registros; `FirstTime` termina con COL4.
- `LoadCollisionFile(fichero)`: sin COL4 (conserva la version anterior) y modelo nuevo sin cargar (el EXE carga en el anterior).
- `LinkLods`: con la cache cargando se saltan las IPL relacionadas (la DLL saltaba los edificios); `isCacheLoading` en cada vuelta.
- `LoadLevel`: TEXDICTION copia al diccionario actual (la DLL al propio); IMG sin pantalla de carga; COLFILE con la ruta del EXE.
- `LoadTexDictionary`: el stream se cierra una vez; `LoadAtomicFile(stream)` sin comprobar `mi`; `StartLoadClumpFile`/`LoadClumpFile(stream)` consultan el tipo antes que el chunk.

**Método:**
- Lineas generadas en memoria; `CFileMgr::ReadLine/OpenFile/Read/CloseFile` simulados (nunca se lee un fichero real).
- Model infos falsos con vtable falsa trazada, colocados temporalmente en `ms_modelInfoPtrs` (o con la tabla vaciada en `LoadLevel` y la colision).
- Pools falsos de TXD, enex, colision, edificios y dummies; creadores, primitivas RW, `Malloc`/`operator new`/`CColModel::operator new`, matrices y rand simulados y trazados sin reservas.
- Se guardan y restauran `ms_line`, el buffer de lineas, `gString`, `gAtomicModelId`, `gpLoadedBuildings`, el buffer de colision, el multiplicador LOD, `ms_numAnimAssocDefinitions`, `ms_modelInfoPtrs` y los punteros de pools.
- Fuera de los fixtures: campos opcionales ausentes que el EXE deja con basura de la pila, nombres mas largos que los buffers del EXE, slots libres que el EXE desreferencia, `LoadCollisionFile(fichero)` sin registro valido previo.
- Enmascarado: la esfera del `CColModel` nuevo no cargado en `LoadCollisionFile(fichero)` (el constructor de `CColModel` de la DLL la pone a cero; el EXE no).

Repetir: `python reversing/FileLoader47/build_probe.py` y `python reversing/run.py --case fileloader47`.

**Actualizacion 18/10/2026 (revalidacion pendiente tras dos cambios de la fuente):**
- `CPickups::GenerateNewOne` (0x456F20) es ahora un envoltorio de `GenerateNewOne_ExeAbi` (el cuerpo enganchado): el falso de `LoadPickup` parchea 0x456F20 y el cuerpo de `GenerateNewOne_ExeAbi` (`pPickup`).
- `CFileLoader::Load2dEffect` (0x5B7670) es ahora un cuerpo C++ enganchado (antes `plugin::Call` al EXE): el stub de `LoadObjectTypes` (`l2dfx`) parchea 0x5B7670 y el cuerpo de la DLL. El hook de `Load2dEffect` no forma parte de las 47 funciones del banco ni se conmuta; su validacion propia es `FileLoaderLoad2dEffect27`.
- `build_probe.py` vigila tambien `Pickups.cpp/.h`. Los hashes de `validation-summary.json` son de la corrida anterior y quedan obsoletos hasta repetir el banco.
- Solo `LoadPickup`: `FILELOADER47_ONLY=CFileLoader::LoadPickup python -B reversing/FileLoader47/run_local.py` (256 pares; escribe y borra `fileloader47-only.txt` en el clon).

**Revalidación 18/10/2026 (corrida aislada 20261018-222127-695707, APROBADA, 256 pares):** sólo `CFileLoader::LoadPickup`, registrada con `register_revalidation_20261018.py` en `validation-summary-reval-20261018.json` (la evidencia anterior queda en `previous_evidence`). Sonda desde `D:/Original/gta-wt-b29` (HEAD 29305c78d). La nota de pendientes queda cerrada por la corrida completa del 19/10/2026 (abajo).

**Revalidación completa 19/10/2026 (corrida `20261019-004401-552608`, APROBADA, 47 funciones, 11 968 pares, 0 diferencias):** con las fuentes actuales (`CFileLoader::Load2dEffect` y `LoadPickup` en C++, `CPickups::GenerateNewOne` como envoltorio). Sonda compilada desde el worktree `D:\Original\wq-f` (HEAD 98286d43c + fusión de CQuaternion; ver `artifacts/build-manifest.json`). Registrada con `register_revalidation_20261019.py` en `validation-summary-reval-20261019.json`: sustituye la evidencia vigente de las 47 funciones (la anterior queda en `previous_evidence`; la de `LoadPickup` de 18/10/2026 queda anidada) y ya no hay funciones pendientes de repetir. `validation-summary.json` y `validation-summary-reval-20261018.json` se conservan como históricos.
