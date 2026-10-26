<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CControllerConfigManager como el EXE (lista 34)

Preparado el 04/10/2026: **16.512 pares original/candidato dentro de GTA** para las 58 funciones de `CControllerConfigManager` (asignaciones de teclado, ratón y joystick, `AffectPadFromKeyBoard/Mouse`, `Clear*/Set*/Get*`, `DeleteMatching*`, textos de las acciones, `Load/SaveSettings`, `ReinitControls`). Pendiente de compilar y ejecutar.

| Clase | Funciones |
|---|---|
| `CControllerConfigManager` | las 58 de `list34.txt` (constructor 0x531EE0 … `UpdateJoy_ButtonDown` 0x530ED0) |

**Diferencias reales corregidas:**
- `SaveSettings` y `LoadSettings` escriben y leen el `ControlsManager` global (0xB70198 / 0xB70D08), no `this`. `LoadSettings` también vacía el global.
- `SetMouseButtonAssociatedWithAction` no llama a `ResetSettingOrder` (la versión de teclado sí).
- `ResetSettingOrder`: comparaciones con signo (`jle`/`jge`).
- `InitDefaultControlConfigMouse`: no limpia `m_MouseFoundInitSet` (solo lo pone a 1) y no tiene `assert`.
- `AffectControllerStateOn_ButtonDown_Driving` y `_FirstAndThirdPersonOnly`: en el stick izquierdo (`GO_*`, `VEHICLE_STEER*`) solo cuenta el indicador de «ambas teclas». El valor previo del eje no se mira (el stick derecho sí lo mira).
- `UpdateJoyInConfigMenus_ButtonDown/Up`:
  - comprueban el botón antes de pedir el pad;
  - todos los botones del 1 al 16 se escriben en cualquier pad, y Start (12) solo en el pad 1;
  - fuera de rango no hacen nada.
- `GetIsMouseButtonJustUp` es «ahora arriba y antes abajo» (antes era «recién pulsado»). El 2 mira el botón derecho y el 3 el central.
- `GetIsMouseButtonDown/Up/JustUp` devuelven `false` fuera de 1..7, sin `NOTSA_UNREACHABLE`.
- `GetIsKeyBlank` devuelve cierto con tipos fuera de rango. `ClearSettingsAssociatedWithAction` solo reordena en ese caso y `StoreMouseButtonState` no hace nada con botones desconocidos.
- `GetControllerSettingTextKeyBoard`:
  - solo compara con `rsNULL`;
  - los textos con número (`F~1~`, `NUM~1~`) van al búfer 0xB7149C;
  - `^` (alemán) y 178 (francés) se escriben en 0xB714BC, y en otros idiomas siguen al caso general;
  - el carácter pasa por 0x719360 (tabla 0x8D5AD8 para ≥ 0x80).
- `GetGxtStringOfCommandKeys`: con cualquier método distinto de ratón+teclado mira el joystick y, si no hay, salta el ratón.
- `ReinitControls` no comprueba el resultado de `GetCapabilities`.
- `AffectPadFromKeyBoard`:
  - pulsación de la tecla principal y de la secundaria;
  - después, si la principal está suelta se libera solo la principal; si sigue pulsada, se libera la secundaria cuando está suelta.

  Antes liberaba justo las teclas pulsadas.
- `AffectPadFromMouse`: la liberación exige botón asignado y menú cerrado.
- `DeleteMatchingVehicle_3rdPersonControls` recibe 3 argumentos (`ret 0Ch`). Con 2, el hook descuadraba la pila de los llamadores del EXE.

**Sin cambios (coinciden con el EXE):** el resto, incluidas las tablas de `GetActionType`, `GetIsKeyboardKeyDown/JustDown`, `InitDefaultControlConfiguration` y `InitDefaultControlConfigJoyPad`, y los textos de ratón y joystick.

**Desviaciones quitadas tras la primera ejecución** (`FIX_BUGS` está definido en la compilación, así que `notsa::IsFixBugs()` es cierto):
- `InitDefaultControlConfiguration`: `PED_FIRE_WEAPON_ALT` vuelve a '\' y `PED_CENTER_CAMERA_BEHIND_PLAYER` a '#'. Se quitan las asignaciones extra de `VEHICLE_LOOKBEHIND` (NUM5) y `VEHICLE_MOUSELOOK` ('*').
- `InitialiseControllerActionNameArray`: `VEHICLE_RADIO_TRACK_SKIP` vuelve a quedar sin nombre.
- `InitDefaultControlConfigMouse`: se quita el forzado NOTSA de SDL3, que asignaba todos los botones aunque `GetMouseSetUp` no los detectara.
- `GetControllerSettingTextKeyBoard`: comparaba el idioma con `GERMAN`/`FRENCH` de `Text.h` ('g'/'f'). Ahora compara con 2 y 1 (`eLanguage`).

**Método:**
- Se llama a la dirección del EXE con los hooks de la clase desbloqueados y desactivados (original) o activados (candidato). El cambio se comprueba después de `State()`.
- El gestor es un objeto falso con tablas deterministas: teclas, `rsNULL`, botones de ratón y joystick, órdenes 0..5 y a veces valores de 32 bits.
- `Load/Save/ReinitControls` sustituyen temporalmente el contenido del `ControlsManager` global.
- Se rellenan y restauran en cada ejecución: los pads, los estados de teclado y ratón, `padNumber`, el método de control, el menú, el idioma, `AllValidWinJoys`, el modo de la cámara activa y los búferes de texto 0xB7147C..0xB714EE.
- Stubs trazados:
  - `CFileMgr::Read/Write/Seek` sobre un fichero en memoria: nulo, cabecera vacía, versión antigua, válido, id corrupto o truncado;
  - `FindPlayerPed/Vehicle` (peatón falso con `m_nPedState` en 0x530);
  - `GTATranslateShiftKey`;
  - `WinInput::GetMouseSetUp`: en el EXE es thiscall `ret 4`; en la DLL, cdecl con retorno oculto;
  - el dispositivo DirectInput del joystick: objeto falso cuya vtable traza `GetCapabilities` (slot 3).
- Se compara: el gestor, el global, los pads, el ratón temporal, los búferes de texto, el contenido de los textos devueltos (el puntero, como desplazamiento si es un búfer) y la traza.
- Fuera de los fixtures:
  - teclas negativas: el EXE indexa `standardKeys` con signo;
  - tipos > 3 en las funciones que escriben estados de pad: el EXE usa un puntero basura;
  - `padNumber` ≥ 2 en `UpdateJoyInConfigMenus_*`;
  - una acción sin ningún texto en `GetGxtStringOfCommandKeys`: `strlen(nullptr)` en ambos.

Repetir: `python reversing/CtrlConfig58/build_probe.py` y `python reversing/run.py --case ctrlconfig58`.
