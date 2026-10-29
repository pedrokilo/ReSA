<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
