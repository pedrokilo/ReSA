<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión independiente — DependenciesVehicleStats11

Corrida **20261010-214000-559687**, revisión posterior del gzip completo, sin ejecutar el juego ni modificar los cuatro instrumentos admitidos. Resultado corroborado: **2 funciones, 4.096 pares, cero diferencias, cero resultados JSON nulos, cero gates fallidos**.

## Evidencia y recuento

Se recorrieron todas las líneas de [comparisons.jsonl.gz](runs/20261010-214000-559687/comparisons.jsonl.gz), recalculando SHA256 del contenido descomprimido. Eventos: 1 waiting_for_player, 1 begin, 4.096 comparison, 2 function_summary y 1 complete; ningún acceso inválido registrado. Pares exactamente 0..4095, casos exactamente 0..2047 por función, sin duplicados. Se compararon nuevamente ambos objetos originales/candidatos y equal/accepted, incluyendo todos sus registros crudos.

Las 8.192 observaciones por lado contienen 24 bloques del tamaño declarado, sin eliminar padding ni campos. Se corroboraron canaries de los tres buffers íntegros: A5 en vehículo, 5A en modelo de coche y C3 en modelo de mejora. Se extrajeron directamente las posiciones de los buffers para verificar pedal/velocidad/contactos, modelo del vehículo, flags de mejora, tabla de 15 ruedas, contador de ruedas, lista de 18 upgrades y los dos punteros de slots frente a los inputs. Ambos slots fueron NULL al inicio según el banco: 19999 y 19998, separados y dentro del dominio int16 positivo.

## ABI y resultados definidos

Todos los lados tienen stack_restored, nonvolatile_preserved, route_ok, helper_route_ok, argument0_unchanged, fixture_readonly, mod_globals_readonly, globals_restored y floating_state_restored verdaderos. Los objetos de ambos lados incluyen EAX/ECX/EDX crudos e idénticos. Se verificó nuevamente el primer argumento físico inalterado; el segundo exige puntero original o tick según la ruta exacta de ciclismo, sin aplicar una invariancia falsa a `55C952 mov [esp+10h],eax`.

Se recalcularon independientemente los efectos de ambos contadores de ciclismo a partir de inputs: PC24 para los productos y el contador unsigned leído como x87, truncado del timestep, selección de factor, ceil y suma con wrap. Se compararon con los campos de salida y con los dwords extraídos del bloque crudo en B794DC/B794E0. Todos coinciden. Para la función global se recalculó AL a partir de flags, las listas de ruedas y mejoras y las decisiones nitro; coincide en los 2.048 casos. AL es el retorno bool contractual del caller `49BD95 test al,al`; también se conserva la comparación íntegra del EAX observado.

## Cobertura corroborada

Ciclismo: 512 omisiones de Fat, 512 tasa normal y 1.024 tasa rápida. Entre las llamadas activas hay 792 concesiones de stamina y 744 acumulaciones. Habilidad: 864 concesiones; entre las 1.184 rutas sin concesión hay factor3 en 560 casos, factor1 en 52, factor1.5 en 52 y omisión/factor0 en 520. Las fronteras de velocidad activas incluyen para 0.05: 304 inferiores, 64 iguales, 768 superiores y 48 unordered; para 0.2: 576 inferiores, 64 iguales, 496 superiores y 48 unordered. Estos grupos se derivan de inputs y efectos, no de una traza de instrucciones. Contactos0/1/2 y bool0/1 están incluidos.

Compatibilidad: los 16 perfiles aparecen 128 veces cada uno, incluyendo vacío/primer/último/fallo de ruedas, primer/medio/último/fallo de lista, las cuatro decisiones nitro, bit100/tipo no-wheel, wheel sin bit100 y nitro con bit100. Las fixtures son memoria sintética; no acreditan vehículos construidos, pools o archivos de modelos reales. La función global figura bajo propietario de instrumentación CVehicle, sin atribuirle una identidad de método C++.

## Colaboradores y restauración

Begin contiene exactamente trece rutas coherentes: diez colaboradores reconstruidos DLL ON (HUD/Text/GetStat/StatsCheck/Fat/Increment/Display y dos wheels) y tres originales sin RH (FindPlayerInfo53FB70, CRTftol821B40 y CRTceil823820). Las tres últimas no se cuentan como nuevas funciones del juego. Cada lado exige comprobación de bytes, flags, resolución del cuerpo y módulo ASI antes/después; el banco no modifica su modo inicial. Las cinco dependencias directas requeridas Fat55C470, Increment55C180, Display55B980 y Wheel4C8740/4C8750 están reconstruidas y ON.

Complete acredita state_restored, floating_state_restored y helper_route_ok verdaderos, 4.096 pares. El instrumento compara bloques y flags/código finales contra sus snapshots y restaura FNSAVE108/MXCSR por lado y al final. Esta revisión corroboró todos los gates y bloques observados; no releyó memoria viva del juego después de la corrida.

[summary.json](runs/20261010-214000-559687/summary.json) y [approval-audit.json](runs/20261010-214000-559687/approval-audit.json) acreditan partidas sin cambios y configuraciones restauradas. El resumen conserva tres hashes previos de partidas y dos de configuración; no contiene hashes individuales posteriores. Se verifica el resultado registrado del runner, sin afirmar una lectura independiente posterior de las instalaciones personales ni de esos archivos.

## Huellas y vigencia

Log descomprimido recalculado: `b1b68fc3cdf9ebb791bfefc2a90b48b692a785b04cf5bc7bf6a9574888b589dc`, idéntico a resumen/admisión. ASI local recalculada: `86e088f3a778e2a62409747feb0f18655f7fbacf0745c141baf98c9f76cadee4`, idéntica al ejecutado. Las cuatro huellas actuales de HelperRoutes.hpp, LiveComparison.hpp, build_probe.py y run_local.py coinciden con la admisión. La admisión conserva las 45 huellas de fuentes probadas; cambios posteriores de otras tandas no alteran este registro histórico.

La primera compilación falló C2664 por pasar un stream a InstallFailureCapture(const char*). Se reparó el protocolo del instrumento antes de la corrida aprobada: marker y runtime.jsonl del ejecutor, waiting_for_player, captura por prefijo y espera de jugador/cutscene. La reparación no cambió los cuerpos de producción ni los oráculos. Esta nota es histórica; los cuatro instrumentos aprobados no se reescribieron durante la auditoría.
