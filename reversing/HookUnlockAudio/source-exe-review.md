<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tres locks del audio de vehículo: revisión previa a ejecución

No hay todavía reproducción del crash antiguo ni evidencia para retirar los tres locks. Se conserva comentario de crashes aleatorios; las correcciones anteriores de ECX/x87/tabla RC podrían explicar alguno, pero eso es hipótesis.

La cadena real es Service0x502280 → ProcessVehicle0x501E10 → ProcessPlayerVehicleEngine0x4FBB10 para conductor jugador coche/moto. Callers directos de Service del catálogo binario:0x6B18F1,0x6B9298,0x6F1793,0x6F86B6. Los workers de streaming comparten hardware/sonidos, pero no se ha acreditado que ejecuten estos tres cuerpos.

EXE0x501FCF leaecx,[esp+0C];0x501FD3 push ecx;0x501FD4 mov ecx,esi;0x501FD6 call4FBB10;0x501FE9 mov eax,[esp+1C] reemplaza EAX de retorno. No aparece uso posterior inmediato de ECX que exija preservación de esta llamada. Service0x5023BE call501E10 termina con popedi/ebx/esi/ecx y ret0x5023C7; no usa resultados volátiles de ProcessVehicle.

Engine0x4FBB10 pushecx/pushesi;0x4FBB12 movesi,ecx;0x4FBB27 movedi,[esp+10] recoge referencia parámetro. Guarda banco dummy0x4FBB39 call4D88C0;0x4FBB3E testal y salida0x4FC9E7 si no cargado. Coverage no puede acreditarse solamente por retornar correctamente de esa salida temprana.

Preparación de VP en ProcessVehicle fuente5336: valor-inicializado y seis campos de entidad/modelo/tipo/transmisión/velocidad. ProcessPlayerVehicleEngine fuente3035 escribe pedales por GetAccelAndBrake, luego AbsSpeed/SpeedRatio/RealGear/IsHandbrakeOn/RealRevsRatio/WheelSpin/NumGears/ZOverSpeed; para coche/moto completa ruedas/GasPedalAudioRevs/PrevSpeed. La preparación LiveMasks reproduce el caller, sin fabricar ruedas ni apuntar a buffers falsos.

Fase pares nueva:19.840comparaciones de tres funciones, ocho contextos. CasesG6 usa stubs de las funciones hermanas; por tanto esa fase no acredita la cadena completa ni hardware. Fase real: las tres llamadas explícitas por frame sobre objeto real y ambas entradas permiten ejecutar la cadena sin stubs, pero no equivalencia determinista de hardware, ni cobertura de todos los estados. Necesitan los escenarios adicionales de salida/entrada, radio y cambio vehículo antes del cierre final.

La sesión de setup es descartable sin guardar. Hook bytes/locks se restauran; partidas e INI deben verificarse por el runner. No se afirma restauración exacta de tareas/stats/radio/streaming del proceso.
