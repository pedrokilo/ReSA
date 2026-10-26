<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Headers mínimos de Clouds.cpp sin PCH

Propuesta offline sin aplicar ni compilar. El parche se genera contra la versión actual de Clouds.cpp (tres helpers ya extraídos y warning(push,0) alrededor de StdInc).

La TU Clouds.cpp conserva /arch:IA32 y SKIP_PRECOMPILE_HEADERS/SKIP_UNITY_BUILD_INCLUSION. Se elimina únicamente el paraguas StdInc y se mantienen avisos suprimidos exclusivamente durante los includes; el cuerpo sigue bajo /WX.

Se revisaron usos cualificados y llamadas independientes de las 21 funciones de Clouds.cpp. La lista incluye tipos RW, macros hook, Tracy, buffers/constantes Common, matrices/placeables/cámara, sprites/coronas, reloj/tiempo, colores, niebla/render y texturas. STL explícito cubre los templates utilizados y las dependencias inmediatas de headers.

Camera.h/Common.h/TxdStore.h y los demás headers elegidos no llegan a Pools.h o CompressedVector.h en el grafo local revisado. World.h llega a Pools.h, y Weather.h llega a CompressedVector.h por audio→EntityRef→Entity→CollisionData. Se evita también incluir Ped/PlayerPed/Vehicle en Clouds.cpp.

CloudsWeatherCpp.h declara diez referencias inline con los mismos tipos y direcciones de Weather.h: CloudCoverage, ExtraSunnyness, Foggyness, Foggyness_SF, NewWeatherType, OldWeatherType, Rainbow, UnderWaterness, Wind y WindDir. No se duplica ni se redefine CWeather. No se calcula ni se copia estado meteorológico.

CloudsAccessCpp.cpp mantiene el PCH y configuración estándar. Sus funciones sólo reenvían las mismas llamadas originales de jugador/vehículo/coordenadas y lectura de calidad FX; el vehículo se convierte correctamente a su base CPlaceable y el área se obtiene mediante GetAreaCode. Así Clouds.cpp no necesita definir entidades completas ni recurrir a lecturas por offsets. El cuerpo de los métodos conserva orden y cálculos. No se incorporan asm, naked, opcodes, bypass de validación, /FORCE ni modificaciones de headers globales.

Tras aplicar hay que reconfigurar CMake (GLOB incorpora la nueva TU); agregar CloudsAccessCpp.cpp/.h y CloudsWeatherCpp.h al manifiesto del banco antes de compilar. No dar a la TU Access las propiedades IA32/noPCH de Clouds.cpp. Verificar el objeto Clouds.cpp con las comparaciones FCOMP/FNSTSW, volver a ejecutar CppCloudsMath3 y después la regresión Clouds21. La preparación offline no demuestra que compile ni sustituye esos bancos.

`dependency-inventory.json` conserva hashes de fuentes y los bindings inspeccionados. `Clouds.cpp.proposed` permite revisar la fuente resultante sin aplicar el parche.

## Dependencias descubiertas en la compilación del coordinador

`additional-dependencies.patch` complementa la propuesta inicial: DirectX y los tres headers D3D antes de RenderWare; `<random>` para General; CColSphere adelantada y enums eModelID/eWeaponType más Sphere/Sprite2d antes de Camera; FixedVector.hpp antes de Coronas; Box antes de TimeCycle. CloudsFxQualityCpp devuelve la misma calidad del gestor FX desde la TU con PCH; el valor FX_QUALITY_LOW es exactamente cero en Fx.h. No se añaden esos gestores al grafo de Clouds.cpp. Estas incorporaciones proceden del log real del coordinador; esta subtask no ejecutó compilación.
