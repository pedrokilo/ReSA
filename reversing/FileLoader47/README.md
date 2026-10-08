<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
