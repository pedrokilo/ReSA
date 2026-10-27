<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dependencias originales fuera del catálogo · cohorte 2 (17/10/2026)

Instantánea fija del 17 de octubre de 2026 con el árbol de trabajo actual: **464 destinos originales explícitos** (direcciones de función del EXE que `source/` sigue llamando por dirección) que no tienen nombre en `docs/hooks.csv`, no están validados y no pertenecen al cohorte de 499 (`../OriginalDependencies499/`). `targets.csv` y `baseline.json` preservan el snapshot y sus hashes; el catálogo principal (8583/8583) no incluye estas funciones. Clasificar una función no equivale a reconstruirla ni a validarla: ninguna fila tiene validación runtime todavía.

| Clasificación | Total | Aprobadas en juego | Pendientes |
| --- | ---: | ---: | ---: |
| Funciones del juego | 185 | 0 | 185 |
| Bibliotecas/runtime/SDK | 279 | 0 | Seguimiento separado |
| Identidad o procedencia incierta | 0 | 0 | 0 |
| Total del cohorte | 464 | 0 | 464 |

De las 185 entradas del juego, 153 son envoltorios directos (la función fuente sólo delega en el EXE) y 32 son llamadas incrustadas en una reconstrucción C++ mayor (`embedded_call`: la fuente llama al original desde dentro de otra función). Incierto = 0: ninguna fila quedó sin categoría, pero las filas con confianza media o con ubicación atípica se listan abajo y no deben tratarse como certezas.

## Qué entra en este cohorte y qué no

El cohorte 499 se definió el 10/10/2026 como los destinos explícitos con aristas entrantes desde funciones catalogadas. Hoy, con el catálogo al 100%, esa condición casi ya no discrimina (sólo 39 de los 464 destinos tienen aristas de ese tipo), así que este cohorte reúne **todos** los destinos explícitos que siguen apareciendo en `source/`, y la columna `incoming_edges_from_catalogued_functions` se conserva como dato. Decisiones:

- **Alcance de fuentes**: todo `source/` (`.cpp`, `.h`, `.hpp`, `.inc`, `.inl`), como el cruce original (`audit_source_calls.py` barría `source/` entero). Hay 452 destinos con referencias en `source/game_sa` y 12 referenciados sólo desde `source/app` (capa de plataforma: WinMain, WindowedMode, WinInput). `source/app` se incluye porque el cohorte original lo incluía.
- **Exclusión del cohorte 499**: hoy hay 831 destinos explícitos sin nombre de catálogo; 367 son los del cohorte 499 que siguen pendientes (124 del juego + 243 de bibliotecas, todos aún referenciados desde `source/`) y se excluyen. Las 131 funciones del juego y la `printf` del cohorte 499 ya están en el catálogo, por eso no aparecen.
- **Destinos**: sólo direcciones que son entrada de función del EXE (`binary_functions.csv`, Ghidra 12.1.4 sobre el EXE canónico `72ae59e4...cecc9b`) sin `catalogue_names` en el inventario regenerado con el catálogo actual. Las direcciones de datos (`reinterpret_cast<float*>(0xB6F03C)`, etc.) se descartan por no ser entradas de función.
- **Hooks ya registrados fuera del catálogo**: las líneas con `RH_Scoped*`/`HookInstall` no cuentan como dependencia. Dos destinos del cohorte tienen un hook activo en `source/` aunque no figuran en `docs/hooks.csv`: `__MainWndProc` 0x747EB0 (WndProc.cpp:293) y `diPadInit` 0x7485C0 (WinInput.cpp:440). Los hooks comentados (`// RH_ScopedInstall(...)`) son sólo evidencia de nombre.

## Metodología

1. **Inventario binario con el catálogo actual**: `contrib/build-binary-map.py --root . --export build/binary-map --output <dir>` (con `docs/MemoryMap/index.html` copiado a `<dir>`). Se escribió en un directorio temporal; `docs/MemoryMap` no se modificó (su `binary_functions.csv` es del 10/10 y marcaba 9223 entradas sin nombre; con el catálogo de hoy son 9192).
2. **Cruce fuente/binario** (`build_cohort.py`): barrido de `source/` sin comentarios ni cadenas, cinco estilos de referencia a una dirección: `plugin_call` (`plugin::Call*<..., 0xADDR, ...>`), `direct_function_pointer_cast` (`((ret(__conv*)(args))0xADDR)(...)`), y tres estilos nuevos que el cruce de 10/10 no veía porque las conversiones recientes a C++ los usan: `reinterpret_cast_pointer` (`reinterpret_cast<Fn>(0xADDR)`, `uintptr_t{0xADDR}`), `function_pointer_alias_cast` y `address_constant` (`constexpr uintptr_t kExeX = 0xADDR;`). Los archivos `.cpp.inc` también se leen (el cruce anterior sólo leía `.cpp`/`.h`).
3. **Clasificación con evidencia** (`classify_cohort.py`): reglas deterministas citadas por id en la columna `evidence` (`regla=`), complementadas con revisión manual por desensamblado del EXE (`classification-overrides.csv`, una fila por caso con su motivo). El proyecto de referencia `gtasa-reversed` sólo aporta nombres (hint), nunca categoría. La dirección sola no decide: el nombre/contrato de la fuente es la evidencia, como en el cohorte 499.
4. **Progreso** (`update_progress.py`): `progress.csv`/`progress.json` con el mismo esquema que el cohorte 499; `approved.json` está vacío (se rellenará con el mismo rigor que `register_bank.py`/`register_approved.py` del cohorte 499).
5. **Prioridades** (`build_priorities.py`): `work-queue.csv` ordena las entradas del juego pendientes por esfuerzo estimado (bytes, dependencias sin catalogar, llamadas indirectas, x87, tablas de saltos, SEH).

Reglas de categoría: **L1** nombre de API RenderWare en un archivo del SDK (`rw*.cpp`, `rp*.cpp`, `rt*.cpp`, `RenderWare.h`) con dirección >= 0x740000; **L2** símbolo de runtime en el mapa de Ghidra o dirección >= 0x821000 (CRT estática); **L3** nombre RenderWare fuera de los archivos del SDK con dirección >= 0x740000; **G1** nombre de juego (proyecto de referencia o firma `Clase::método`) con dirección < 0x740000; **G2** sólo contexto de juego (llamada incrustada sin símbolo). Los casos que contradicen su dirección (nombre RW con dirección baja, nombre de juego con dirección alta) van a revisión manual. Contraste con el cohorte 499 (sólo por identidad ya revisada y dirección, sin contexto fuente): las reglas coinciden en 472 de 499; los 27 desacuerdos son QuickTime (15) y `AtomicDefaultRenderCallBack` (1), sin nombre de API reconocible; la familia `*GtaStream*` y `CMemoryMgr::Free/Calloc` (7), donde el propio cohorte 499 es inconsistente; y 4 de plataforma/juego en 0x74xxxx (`DoDriveByShootings`, `DIReleaseMouse`, `Rs::psGetMemoryFunctions`, `GetDXVersion`). Son justo los casos que aquí van a revisión manual.

## Desglose

Estilos de referencia (un destino puede tener varios):

| Estilo | Destinos |
| --- | ---: |
| `direct_function_pointer_cast` | 273 |
| `plugin_call` | 157 |
| `reinterpret_cast_pointer` | 32 |
| `address_constant` | 4 |

**Envoltorios `plugin::Call*` directos**: 157 destinos tienen al menos una referencia `plugin::Call*`; en 139 de ellos la función fuente es un envoltorio directo (una sola sentencia que delega en el EXE), 133 del juego y 6 de biblioteca. Los demás (18) usan `plugin::Call*` dentro de una función C++ mayor. Los otros 307 destinos se llaman con casts de puntero a función o `reinterpret_cast` (en el SDK RenderWare son casi todos wrappers de una línea).

Bibliotecas por familia:

| Familia | Destinos |
| --- | ---: |
| RenderWare SDK (rw*, rp*, rt*, rx*) | 192 |
| RenderWare, controlador D3D9 (_rwD3D9*/RwD3D9*/_rpD3D9*) | 71 |
| CRT estática de MSVC (símbolos de runtime, dirección >= 0x821000) | 14 |
| QuickTime/QTML (thunks y ayudantes enlazados estáticamente) | 2 |

Entradas del juego por clase (primeras 12):

| Clase | Destinos |
| --- | ---: |
| CCarCtrl | 67 |
| (sin clase / ayudante) | 34 |
| CHeli | 9 |
| CVehicle | 6 |
| CTaskComplexPartner | 5 |
| CTaskComplexFollowPedFootsteps | 5 |
| CGarage | 4 |
| D3DResourceSystem | 4 |
| FxEmitter_c | 3 |
| CPed | 3 |
| CPlayerPed | 3 |
| CTaskComplexLeaveCar | 3 |

## Confianza media o ubicación atípica (revisión manual)

Estas filas se decidieron por desensamblado y no por una regla; la evidencia completa está en `classification-overrides.csv` y en `progress.csv`:

| Dirección | Identidad | Categoría | Motivo resumido |
| --- | --- | --- | --- |
| 0x004C9970 | RwD3D9IndexBufferCreate | library | Biblioteca RenderWare (controlador D3D9, bloque embebido entre código del juego) |
| 0x004CC970 | _rxD3D9VideoMemoryRasterListRestore | library | Biblioteca RenderWare (controlador D3D9, lista de rasters en memoria de vídeo) |
| 0x004CD250 | _rwD3D9RasterConvertToNonPalettized | library | Biblioteca RenderWare (controlador D3D9) |
| 0x00730E60 | RwTextureGtaStreamRead | game | Función del juego (lector de texturas de GTA). Criterio por cuerpo (usa CTimer del juego). El cohorte 499 es inconsistente en esta familia: RwTexDictionaryGtaStreamRead (0x730FC0) como juego y RwTexDictionaryGtaStreamRead1/2 y RpClumpGtaStreamRead1/2 como biblioteca |
| 0x00734FC0 | RpClumpGetBoundingSphere | game | Función del juego (ayudante RwHelper) |
| 0x00747EB0 | __MainWndProc | game | Función del juego (plataforma Win). Observación: ya hay hook activo en source pero no figura en docs/hooks.csv |
| 0x007485C0 | diPadInit | game | Función del juego (plataforma Win). Observación: hook activo en source fuera del catálogo |
| 0x007F7F70 | _rwD3D9DeviceReleaseVideoMemory | library | Biblioteca RenderWare (controlador D3D9) |
| 0x00819770 | QuickTime runtime shutdown helper | library | Biblioteca QuickTime/QTML enlazada estáticamente |
| 0x00819840 | QuickTime dispatch thunk (selector 0x20002) | library | Biblioteca QuickTime/QTML (thunk de despacho) |
| 0x0041CC10 | CPool<CVehicle>::GetAt (acceso por índice a la ranura de 0xA18 bytes) | game | Función del juego (plantilla CPool instanciada para vehículos) |
| 0x004F2860 | CAEStreamingDecoder scalar deleting destructor (operator delete de la clase) | game | Función del juego |

Además, 18 entradas del juego son llamadas incrustadas sin símbolo confirmado («helper sin símbolo»): la categoría juego es firme (dirección baja y contexto de juego) pero la identidad exacta no; no se les asignó nombre inventado.

## Las 20 prioridades más sencillas de reconstruir

Orden de `work-queue.csv` (heurística de esfuerzo, ver `build_priorities.py`); las 20 se revisaron una a una con el desensamblado. Ninguna usa SEH ni tiene destinos del juego sin catalogar como dependencia; las dependencias indicadas ya están en el catálogo salvo donde se dice.

| # | Dirección | Identidad | Bytes | Qué hace | Dependencias |
| ---: | --- | --- | ---: | --- | --- |
| 1 | 0x0061A3A0 | helper sin símbolo confirmado (usado por FightUnwind) | 7 | Escribe la vtable base 0x86D48C en this y retorna (mov [ecx],0x86D48C; ret). Es el limpiador del marco SEH de CTaskSimpleFight que la fuente llama desde FightUnwind. | Ninguna. 7 bytes. |
| 2 | 0x0041CC10 | CPool<CVehicle>::GetAt (acceso por índice a la ranura de 0xA18 bytes) | 31 | Acceso por índice a una ranura de pool de 0xA18 bytes: 0 si el byte de flags es negativo, si no base + idx*0xA18 (pool de vehículos). | Ninguna. 31 bytes, sólo enteros. |
| 3 | 0x0041B950 | CAutoPilot::RemoveOnePathNode | 45 | CAutoPilot::RemoveOnePathNode: decrementa el contador de nodos [+0x88] y desplaza la lista de nodos una posición. | Ninguna. 45 bytes, sólo enteros. |
| 4 | 0x005FB8A0 | CPedGroups::RemoveAllFollowersFromGroup | 32 | CPedGroups::RemoveAllFollowersFromGroup: si el grupo está activo calcula el CPedGroup (0xC09920 + id*0x2D4) y salta a CPedGroup::RemoveAllFollowers. | CPedGroup::RemoveAllFollowers (0x5FB7D0, ya catalogada). 32 bytes. |
| 5 | 0x00604490 | CDecisionMakerTypes::FlushDecisionMakerEventResponse | 35 | CDecisionMakerTypes::FlushDecisionMakerEventResponse: calcula la entrada (a*0x29 + tabla[b]) * 0x3C y llama a CDecision::SetDefault. | CDecision::SetDefault (0x600530, catalogada). 35 bytes. |
| 6 | 0x0044A210 | CGarages::CountCarsInHideoutGarage | 47 | CGarages::CountCarsInHideoutGarage: obtiene el índice con FindSafeHouseIndexForGarageType y cuenta las ranuras no nulas (4 ranuras, paso 0x40) de la tabla 0x96ABEA. | CGarages::FindSafeHouseIndexForGarageType (0x4489F0, validada en el cohorte 499). 47 bytes. |
| 7 | 0x00423880 | CCarCtrl::SteerAIPlaneToCrashAndBurn | 81 | CCarCtrl::SteerAIPlaneToCrashAndBurn: escribe constantes float inmediatas en cuatro campos de mando del avión según el bit 0 de [+0x20]. | Ninguna. 81 bytes, sin x87. |
| 8 | 0x006D3490 | FindReplacementUpgradeCB | 53 | FindReplacementUpgradeCB (callback de átomos): si el objeto es de tipo 1 (átomo), el id de átomo no tiene el bit 8 y su índice de modelo no es -1, guarda el átomo en data[1] y devuelve NULL; si no devuelve el objeto. | CVisibilityPlugins::GetAtomicId y GetModelInfoIndex (catalogadas). 53 bytes. |
| 9 | 0x006D3370 | FindUpgradeCB | 61 | FindUpgradeCB (callback de átomos): igual que el anterior pero compara el campo de 5 bits del modelo-info con data[0]. | CVisibilityPlugins::GetAtomicId y GetModelInfo (catalogadas). 61 bytes. |
| 10 | 0x00406CE0 | helper sin símbolo confirmado (usado por CHud::DrawVitalStats) | 19 | Ayudante float: convierte el argumento a double, llama a la función CRT 0x8219F0 y devuelve el resultado en st0 (usado en CHud::DrawVitalStats). | Función CRT 0x8219F0 (biblioteca, también destino de este cohorte). 19 bytes, x87. |
| 11 | 0x00559010 | CStats::FindLeastFavoriteRadioStation | 110 | CStats::FindLeastFavoriteRadioStation: recorre 13 entradas de 0x18 bytes de 0xB78E5C y devuelve el índice del menor valor entero. | Ninguna. 110 bytes, sólo enteros. |
| 12 | 0x00734A70 | helper sin símbolo confirmado (usado por GetAnimHierarchyFromFrame) | 51 | Callback de RwFrame para GetAnimHierarchyFromFrame: pide la jerarquía del frame y, si no existe, recorre sus hijos con él mismo (RwFrameForAllChildren, según el comentario de RwHelper.cpp:153). | RpHAnimFrameGetHierarchy (0x7C5160) y RwFrameForAllChildren (0x7F0DC0), ambos de RenderWare (el segundo pertenece al cohorte 499). 51 bytes. |
| 13 | 0x00421FE0 | CCarCtrl::ThisVehicleShouldTryNotToTurn | 38 | CCarCtrl::ThisVehicleShouldTryNotToTurn: devuelve bool según un switch sobre el modelo (índice - 0x193, 0x7A casos) con tabla de saltos en 0x422008 y tabla de índices en 0x422010. | Ninguna, pero hay que leer las dos tablas de datos. 38 bytes de código. |
| 14 | 0x004F2860 | CAEStreamingDecoder scalar deleting destructor (operator delete de la clase) | 50 | Destructor escalar de CAEStreamingDecoder: escribe la vtable 0x85F418, destruye el flujo de datos, libera el buffer y, con el bit 0 del argumento, libera this. | CAEDataStream::Destructor (0x4DC490, catalogada) y free del EXE (0x8214BD). 50 bytes. |
| 15 | 0x00422590 | CCarCtrl::StopCarIfNodesAreInvalid | 137 | CCarCtrl::StopCarIfNodesAreInvalid: comprueba cuatro direcciones de nodo (+0x3A4, +0x3A6, +0x390, +0x394) contra la tabla de áreas 0x96F854; si alguna es inválida pone el bit 0 de [+0x3DC] y devuelve true. | Ninguna. 137 bytes, sólo enteros. |
| 16 | 0x0059C230 | CQuaternion::Get | 57 | CQuaternion::Get(axis, angle): ángulo = 2*acos(w) y eje = vector * (1/sin) con x87. | CRT 0x822380 (_CIacos, según FileLoader.cpp:1065). 57 bytes, x87: exige la política x87 del proyecto. |
| 17 | 0x004238E0 | CCarCtrl::SteerAIHeliToCrashAndBurn | 89 | CCarCtrl::SteerAIHeliToCrashAndBurn: escribe constantes float y el valor ±[+0xA14] en cuatro campos de mando del helicóptero según el bit 0 de [+0x20]. | Ninguna. 89 bytes, un fchs. |
| 18 | 0x00682310 | CTaskComplexPartner::StreamRequiredAnims | 93 | CTaskComplexPartner::StreamRequiredAnims: si no está listo compara el nombre del bloque con 0x858B54, obtiene su índice de animaciones, pide el modelo y suma la referencia al bloque. | CAnimManager::GetAnimationBlockIndex, CStreaming::RequestModel y CAnimManager::AddAnimBlockRef (catalogadas). 93 bytes. |
| 19 | 0x0050B830 | helper sin símbolo confirmado (usado por CameraObbeCinemaCycle) | 77 | Ayudante de cámara (CCamera, ciclo Obbe): consulta CWaterLevel::GetWaterLevel para la posición de la cámara actual y compara con su Z con x87. | CWaterLevel::GetWaterLevel (0x6EB690, catalogada). 77 bytes, x87. |
| 20 | 0x00694ED0 | CTaskComplexFollowPedFootsteps::MakeAbortable | 8 | CTaskComplexFollowPedFootsteps::MakeAbortable: reenvía la llamada virtual MakeAbortable (slot 0x18) a la subtarea [this+8] con los mismos argumentos. | Llamada virtual indirecta a CTask::MakeAbortable. 8 bytes. |

## Distribución por franja de dirección

| Franja | Juego | Biblioteca |
| --- | ---: | ---: |
| 0x400000-0x40FFFF | 1 | 0 |
| 0x410000-0x41FFFF | 2 | 0 |
| 0x420000-0x42FFFF | 55 | 0 |
| 0x430000-0x43FFFF | 12 | 0 |
| 0x440000-0x44FFFF | 6 | 0 |
| 0x450000-0x45FFFF | 2 | 0 |
| 0x460000-0x46FFFF | 2 | 0 |
| 0x490000-0x49FFFF | 3 | 0 |
| 0x4A0000-0x4AFFFF | 4 | 0 |
| 0x4C0000-0x4CFFFF | 0 | 3 |
| 0x4F0000-0x4FFFFF | 2 | 0 |
| 0x500000-0x50FFFF | 3 | 0 |
| 0x510000-0x51FFFF | 1 | 0 |
| 0x550000-0x55FFFF | 2 | 0 |
| 0x560000-0x56FFFF | 2 | 0 |
| 0x590000-0x59FFFF | 3 | 0 |
| 0x5A0000-0x5AFFFF | 1 | 0 |
| 0x5B0000-0x5BFFFF | 1 | 0 |
| 0x5D0000-0x5DFFFF | 2 | 0 |
| 0x5E0000-0x5EFFFF | 3 | 0 |
| 0x5F0000-0x5FFFFF | 1 | 0 |
| 0x600000-0x60FFFF | 9 | 0 |
| 0x610000-0x61FFFF | 7 | 0 |
| 0x620000-0x62FFFF | 5 | 0 |
| 0x630000-0x63FFFF | 1 | 0 |
| 0x640000-0x64FFFF | 3 | 0 |
| 0x650000-0x65FFFF | 4 | 0 |
| 0x660000-0x66FFFF | 1 | 0 |
| 0x680000-0x68FFFF | 11 | 0 |
| 0x690000-0x69FFFF | 6 | 0 |
| 0x6A0000-0x6AFFFF | 1 | 0 |
| 0x6C0000-0x6CFFFF | 9 | 0 |
| 0x6D0000-0x6DFFFF | 5 | 0 |
| 0x6E0000-0x6EFFFF | 4 | 0 |
| 0x6F0000-0x6FFFFF | 1 | 0 |
| 0x730000-0x73FFFF | 8 | 0 |
| 0x740000-0x74FFFF | 2 | 33 |
| 0x750000-0x75FFFF | 0 | 38 |
| 0x760000-0x76FFFF | 0 | 16 |
| 0x7C0000-0x7CFFFF | 0 | 27 |
| 0x7E0000-0x7EFFFF | 0 | 11 |
| 0x7F0000-0x7FFFFF | 0 | 56 |
| 0x800000-0x80FFFF | 0 | 75 |
| 0x810000-0x81FFFF | 0 | 5 |
| 0x820000-0x82FFFF | 0 | 15 |

Como en el cohorte 499, todo lo de 0x7C0000 en adelante es biblioteca (RenderWare, CRT, QuickTime) y lo de 0x740000-0x76FFFF es sobre todo SDK RenderWare (`rpworld.cpp`, `rpanisot.cpp`); las excepciones (plataforma Win, ayudantes RwHelper, el bloque del plugin de raster D3D9 en 0x4C9AB0-0x4CD3xx) están en la tabla de revisión manual.

## Límites

- El cruce sigue siendo estático por patrones: no ve llamadas por macros que construyen la dirección, direcciones calculadas ni tablas de punteros a función; los destinos sólo-virtuales (`virtual-slots-outside-catalogue.csv`) quedan fuera de este cohorte.
- «Sin nombre de catálogo» no significa «sin implementar»: `binary_functions.csv` enlaza por dirección fija y por candidatos de registro; un destino puede tener ya un cuerpo C++ equivalente en `source/` que el inventario no enlaza.
- El proyecto de referencia aporta nombres, no evidencia; las identidades de las filas `wrapper` salen de la firma de la propia fuente. Ninguna fila acredita reconstrucción, equivalencia ni independencia del EXE.
- La heurística de prioridad es de esfuerzo estimado; no sustituye el diseño del banco de pruebas de cada función.

## Archivos y reproducción

`targets.csv` (mismas columnas que el cohorte 499), `baseline.json` (hashes y alcance), `evidence-sources.csv` (contexto fuente por referencia), `classification.csv`, `classification-overrides.csv`, `review-queue.csv` (vacío si todo está revisado), `progress.csv`/`progress.json`, `approved.json`, `work-queue.csv`, `priorities-notes.csv`. Scripts: `build_cohort.py`, `classify_cohort.py`, `update_progress.py`, `build_priorities.py`, `build_readme.py`.

Orden de regeneración (con el directorio del inventario binario en `<bm>`): `python reversing/OriginalDependencies2026-10-17/classify_cohort.py --binary-map <bm>` -> `update_progress.py` -> `build_priorities.py --binary-map <bm>` -> `build_readme.py`. `build_cohort.py` no reescribe `targets.csv` si ya existe.
