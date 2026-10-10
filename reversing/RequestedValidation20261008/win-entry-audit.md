<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Win::NOTSA_WinMain — auditoría de comparabilidad

Identidad pendiente exacta: `Win::NOTSA_WinMain`, `0x00748710`, tipo S, reversed=1, validated=0. Fuente: `source/app/platform/win/WinMain.cpp:382`; hook instalado en línea 534.

## Observaciones

La firma es `INT WINAPI NOTSA_WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)`: stdcall x86, retorno INT completo por EAX; no reducirlo a bool ni invocar como cdecl. Es entrada de ciclo de vida seleccionada al arrancar, no una rutina segura para alternar dentro de CGame::Process.

La implementación cambia parámetros del sistema mediante SystemParametersInfo, crea ventana y dispositivos, consume __argc/__argv, anula __argc/__argv/__wargv, ejecuta MainLoop, termina audio/RenderWare y destruye ventana. Incluye comportamiento NOTSA explícito: shutdownLogging, logging, SDL opcional, FastLoader en MainLoop y workaround CRT que libera argv tras anular globales. Esas extensiones requieren delimitar equivalencia frente al EXE sin cambiar el comportamiento esperado.

`reversing/UnlockPlatform/Probe.hpp` incluye NOTSA_WinMain como índice 5 en `names`, permite seleccionarlo mediante GTA_REVERSED_PLATFORM_MASK, y adapta arranque original con mensajes intro/menú porque WinMain original no implementa FastLoader. `UnlockPlatform/validation-summary.json` conserva nueve máscaras [127,0,126,125,123,119,111,95,63] y llamadas de startup. El instrumento de comparación solo alterna índices 4 y 6, y compara índice 2 en otro bloque: no realiza pares de NOTSA_WinMain. El resumen da pares indexados 2:4, 4:16, 6:16; la actividad de calls[5] es prueba de entrada, no equivalencia de la función completa.

`Misc6/validation-summary.json` valida cinco funciones Win diferentes. No contiene NOTSA_WinMain. No trasladar esos pares a la entrada pendiente.

## Banco necesario y límites concretos

Para comparación real debe emplearse proceso nuevo por variante con misma copia de pruebas, mismos argumentos y configuración, cierre natural controlado y trazas comparables de inicialización/eventos/retorno/terminación. Capturar retorno completo de WinMain antes de la salida CRT, orden/argumentos de colaboradores y efectos persistentes. Deben cubrirse rechazo por instancia previa, error de rsINITIALIZE, error de registro/creación de ventana, error RW y arranque/cierre exitoso. Los fallos se provocan con colaboradores simétricos y se documentan; no sirven cierres forzados como retorno de WinMain.

Los parámetros SPI y CRT/SDL no admiten snapshot/restauración mediante un fixture vivo equivalente al de funciones ordinarias. Deben capturarse valores previos reales del usuario y comprobarse tras cerrar cada proceso. No usar los valores fijos 1 y SetErrorMode(0) del candidato como prueba de restauración del entorno anterior. No repetir WinMain en una instancia ya inicializada: invalida precondiciones y puede terminar recursos del proceso.

Bloqueo de evidencia actual: no existe comparación histórica pareada de esta identidad en los dos bancos inspeccionados. Hay infraestructura de selección de arranque y evidencia de actividad, reutilizable como base, pero se requiere ampliar captura del ciclo de vida y consultar pseudocódigo/ensamblador completo del EXE 0x748710 para fijar el contrato original. Esta auditoría no ejecutó ni abrió sesión REA; las afirmaciones anteriores proceden de archivos del repositorio.
