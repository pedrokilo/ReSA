<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7047 con evidencia histórica, 0 con vigencia acreditada y 1399 pendientes en 38 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco 62: constructores de CBmx y CQuadBike

Fecha: 06/10/2026. Estado: escrito, **sin compilar ni ejecutar** (pendiente de `build_probe.py` y `run_local.py`).

El banco 61b dejó fuera los dos constructores porque "exigen construir un vehículo completo". Aquí se validan construyendo el
objeto de verdad pero sustituyendo por dobles lo que no es propio de cada clase.

## Método

* Cada constructor se llama **en la dirección del EXE** (`CBmx` 0x6BF820, `CQuadBike` 0x6CE370, thiscall `ret 8`) sobre un búfer
  de `sizeof(CBmx)` = 0x838 / `sizeof(CQuadBike)` = 0x9BC bytes relleno con 0xCD, más 32 bytes de guarda (también 0xCD):
  * original: hook desactivado, se ejecuta el código del EXE;
  * candidato: hook activo, el EXE salta al envoltorio `Constructor` de la DLL y este al constructor de la DLL.
* Se compara el búfer completo (0x838 / 0x9BC bytes), la guarda, el retorno (`this`) y la traza de llamadas salientes. La vptr se
  normaliza: debe ser la del EXE (0x871528 / 0x871AE8) en el original y la vtable de la DLL en el candidato.
* **Bases simuladas.** `CBike::CBike` (0x6BF430) y `CAutomobile::CAutomobile` (0x6B0A90) construyen `CVehicle`/`CPhysical`, tocan
  pools, mundo, objetos RenderWare y la tabla de handling; no se ejecutan. Se parchean con un doble trazado en la dirección del EXE y
  en el constructor de la DLL (se localiza como el primer llamado de la DLL dentro del constructor derivado y se contrasta con el del
  envoltorio de la base, igual que en Sys13/Boat23). El doble traza `(this, modelo, creationBy[, flag])`, rellena la parte de la base
  (0x814 bytes en CBike, 0x988 en CAutomobile) con bytes deterministas por caso (no 0xCD) y, en la bici, deja en `+0x5B0/+0x5B4` los
  punteros a los marcos de las ruedas delantera/trasera. Así los campos que el constructor derivado no debe tocar quedan con un valor
  reconocible, y una puesta a cero de más (`{}`) o un campo sin escribir aparece como diferencia.
* **Tabla de model infos.** Se sustituye `ms_modelInfoPtrs[15..18]` (0xA9B0C8) por un `CVehicleModelInfo` falso de 0x80 bytes con
  vtable propia: el hueco 14 (`GetAnimFileIndex`) devuelve el índice del caso y traza el hash del objeto en ese instante (orden de
  escrituras); los demás huecos son trampas. `m_nHandlingId` (uint16 en +0x4A) se rellena con valores de 8 y 16 bits.
* **Bloque de animación** (CBmx): `GroupId` de la entrada elegida (0xB5D4A0 + 0x20·i + 0x1C) con valores dentro y fuera de 7..9
  (negativos, 6, 10, 11, `INT_MAX`, `INT_MIN`...).
* **Marcos RW** (CBmx): 8 marcos falsos alineados a 16 con matriz de modelado en +0x10 y padre en +4; cadenas de 0 a 3 padres para
  delantera y trasera, ciclo de vuelta al propio marco (el bucle del EXE para al llegar a él), cola compartida entre las dos cadenas,
  rueda trasera = delantera (distancia 0, división NaN/inf), misma Y en ambas. `RwMatrixTransform` (0x7F25A0) se ejecuta de verdad
  en las dos versiones (matemática pura).
* `CAutomobile::SetupSuspensionLines` (0x6A65D0 y cuerpo de la DLL, que el constructor de CQuadBike alcanza por 0x6CDCA0 en el EXE y
  por `CQuadBike::SetupSuspensionLines` en la DLL) es un doble que traza el hash del objeto en ese instante.
* No se asigna memoria ni se toca ningún pool: no se parchea `operator new/delete`. Antes de cada caso se escribe y vacía un evento
  `case_start`; cada `Write` llama a `PumpMessages`. La tabla de model infos y los bloques de animación se guardan y se restauran por
  caso; el evento final lleva `state_restored` y `layout_ok` (desplazamientos de los campos de la DLL frente a los del EXE, evento
  `layout`).

## Casos

* CBmx::Constructor: 1024. CQuadBike::Constructor: 512. Total 1536 pares.

## Diferencias halladas y corregidas (fuente)

* `CQuadBike::CQuadBike`: el EXE lee el modelo con el índice recibido (`ms_modelInfoPtrs[modelIndex]`, 0x6CE3B9); la DLL usaba
  `GetVehicleModelInfo()` (`m_nModelIndex`). Ahora usa el parámetro.
* `cHandlingDataMgr::GetBikeHandlingPointer` (usado por `CQuadBike` y `CBike`): el EXE (inlinado en 0x6CE3C6 y 0x6BF520) hace
  `(movzx byte id) << 6 + 0xC3491C`, es decir `&m_aBikeHandling[0] + (id - 162)` para cualquier id de 0..255; la DLL solo restaba 162
  si `id >= 162` y para ids menores indexaba `m_aBikeHandling[id]` (otra dirección) y no enmascaraba a un byte. Corregido con aritmética
  de punteros (índice negativo permitido, sin `operator[]`).
* `CBmx::CBmx`: sin diferencias con 0x6BF820 (orden de escrituras, 0x594 = 10 antes de `GetAnimFileIndex`, límites 7..9 con
  comparación con signo, 6 floats a cero, `m_nFixLeftHand/RightHand`, `m_bIsFreewheeling`, bucle de marcos con parada al volver al
  marco inicial, `DistY = front - rear` y `FracY = front / DistY`). La inicialización `{}` de `m_sRideAnimData` en CQuadBike equivale
  a lo que hace el EXE (0x6D0B10 pone 7 dwords a cero y después se escribe 10), así que se mantiene.

## No cubierto

* La parte de la base (`CBike`, `CAutomobile`, `CVehicle`, `CPhysical`: pools, asignación de matriz, handling real, `ChooseVehicleColour`,
  `SetModelIndex`...) no se valida aquí; solo se comprueba que el constructor derivado la llama con los mismos argumentos y que no
  pisa nada más. Construir de verdad un vehículo en una partida viva exige pools, mundo y clump reales.
* `CBike::CBike` usa la misma indexación de handling (se corrige a través de `GetBikeHandlingPointer`) pero su constructor no se valida
  aquí.

## Uso (no ejecutado)

`python reversing/VehCtor62/build_probe.py` y después `python reversing/VehCtor62/run_local.py`. Filtro opcional: fichero
`vehctor62-only.txt` con `Clase::Función`.
