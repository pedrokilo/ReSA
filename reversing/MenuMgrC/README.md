<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CMenuManager, grupo C: flujo y opciones (14 funciones)

Banco por pares dentro del motor (original = hook del EXE desactivado, candidato = hook activado). **Pendiente de compilar y ejecutar** (lo hace quien llama): `python reversing/MenuMgrC/build_probe.py` y `python reversing/run.py --case menumgrc`.

Funciones (14.336 pares previstos): `Process`, `ProcessMenuOptions`, `ProcessPCMenuOptions`, `ProcessMissionPackNewGame`, `CheckMissionPackValidMenu`, `SwitchToNewScreen`, `JumpToGenericMessageScreen`, `SaveLoadFileError_SetUpErrorScreen`, `GetNumberOfMenuOptions`, `ProcessFileActions`, `DoSettingsBeforeStartingAGame`, `ProcessStreaming`, `HasLanguageChanged`, `InitialiseChangedLanguageSettings`.

**Diferencias reales corregidas en la DLL:**
- `ProcessMenuOptions` (`MENU_ACTION_MPACK`): el EXE lee `m_MissionPacks[opción - 2].m_Id` sin comprobar el rango (la DLL copiaba el array y abortaba con `NOTSA_UNREACHABLE`).
- `ProcessMenuOptions` (`MENU_ACTION_SAVE_SLOT`): `ms_Slots.data()[(int8)ranura]`, sin la aserción de `std::array`.
- `ProcessPCMenuOptions` (`MENU_ACTION_CTRLS_JOYPAD` y `MENU_ACTION_CONTROL_TYPE`): cualquier valor distinto de `JOYPAD` toma la rama contraria (la DLL abortaba con `NOTSA_UNREACHABLE`).
- `ProcessFileActions`: `CheckSlotDataValid` recibe la ranura con `movsx` (int8) y de `SaveSlot` solo se mira AL.
- `ProcessMissionPackNewGame`: `strcpy` y `strncpy` (los `_s` rellenaban con 0xFE en depuración); `sprintf` en lugar de `sprintf_s` para `gString` (también en `CheckMissionPackValidMenu` y `ProcessFileActions`).
- `InitialiseChangedLanguageSettings`: `m_SystemLanguage` se compara como dword (1 = francés, 2 = alemán), sin truncar a `eLanguage` de 8 bits.

**Método:**
- `CMenuManager` FALSO (búfer del tamaño de la clase con guardas de 64 bytes y relleno aleatorio); nunca se toca el singleton real y se comprueba su hash antes y después.
- Solo se conmuta el hook de la función bajo prueba; lo demás (resto del grupo, audio, cámara, radar, estadísticas, HUD, streaming, D3D, texto, localización, controles, modos de vídeo, `rand`, bucle de mensajes, pad) se sustituye en el EXE y en la DLL por falsos que trazan llamada y argumentos.
- Ficheros falsos (manejadores y contenido en memoria) y partidas guardadas como trazas: ningún fichero real ni ninguna carga.
- El EXE usa las direcciones absolutas del singleton para `m_CurrentlyLoading/Deleting/Saving` (0xBA8284/85/86) y `m_nNumberOfMenuOptions` (0xBA8269): en la ejecución original se copian del falso al singleton antes de la llamada y se devuelven y restauran después.
- Se compara: retorno, bytes del falso (con guardas), `aScreens`, globales tocados (`gString`, `CGame::bMissionPackGame`, `ms_Slots`, estadísticas, pad...), singleton intacto, nº de llamadas a `rand` y la traza.
- Al empezar se comprueba la disposición de campos (`layout_ok`/`layout_mismatch`) con los desplazamientos que usa el EXE.

**Fuera de la comparación (deliberado):**
- `ProcessFileActions`: se ignora `CFileMgr::SetDir("")` de la DLL (FIX_BUGS, el EXE no lo hace).
- `ProcessPCMenuOptions`: los fixtures se mantienen fuera de las rutas FIX_BUGS de la DLL (`RADAR_MODE` solo con 0/1, `FX_QUALITY` sin flecha izquierda, `USER_TRACKS_PLAY_MODE` con ±1).
- `ProcessMissionPackNewGame`: ficheros de menos de 256 bytes y como mucho 25 packs (la rama FIX_BUGS usa contador de 32 bits; el EXE usa 8 bits y desborda `m_MissionPacks`).
- `GetNumberOfMenuOptions` devuelve solo AL en el EXE: se compara el byte bajo.
- `SetHelperText` no se sustituye (el EXE lo inlinea en `RESET_CFG`).
