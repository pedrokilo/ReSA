<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

> Actualización 11/10/2026: ShadowExtendedSqrt y ShadowDistanceFade (hoy en X87Math.cpp, antes ShadowSqrtCpp.cpp y ShadowMathCpp.cpp) fueron aplicados y validados con CppShadowSqrtStatic y CppShadowSqrtVehicle; ver [reversing/CppOnlyMigration/README.md](../CppOnlyMigration/README.md). El parche shadow_sqrt_cpp.patch quedó obsoleto (el hunk de CMakeLists.txt se reubicó a mano) y se conserva sólo como historia; caller-spec.json y caller-spec-fade.json son las comparaciones COFF de los objetos reales. Los objetos .obj de artifacts/ no se publican. El texto siguiente describe el estado previo a la aplicación.

# FSQRT pequeños: propuesta offline de sombras

Estado: propuesta sin aplicar. No producción, compilación del proyecto, juego ni Git. Un helper, dos consumidores; bancos nuevos CppShadowSqrtStatic y CppShadowSqrtVehicle, 4096 pares cada uno. Todavía no hay validación de esta migración.

## Evidencia actual preservada

`artifacts/Shadows.actual-before.obj` es copia byte por byte del objeto Debug actual, SHA256 b1ffae3184d67a204801879daa7d4ac09a14904c4c4a60cf2a52d82976fc0807. Se comprobó lectura antes/después. `actual-shadow-functions.json` conserva cuerpos COMPLETOS de ambos consumidores, todas sus relocaciones y helper; `actual-helper-proof.json` decodifica sus 58 bytes. No se usa el objeto de referencia optimizado como sustituto de ese objeto real.

El helper real preserva EBX/ESI/EDI/EBP, rellena cuatro DWORD con CC, EAX=CCCCCCCC y ECX=0 tras REP STOSD, reserva 208 bytes y llama __RTC_CheckEsp. Las operaciones numéricas son FLD32 [EBP+8], FSQRT, FSTP64 [EBP-C], FLD64 [EBP-C], retorno ST0. El FSTP64 es obligatorio: el valor no debe permanecer como resultado extended80 sin ese redondeo. La propuesta usa volatile double local y misma secuencia numérica, sin cambio de PC/RC, sin llamadas al juego ni assembler.

## Intento de conservar /Od /ZI

Los estudios aislados SqrtSmallCpp-Od y SqrtSmallCpp-OdOi conservaron el prólogo/epílogo debug pero emitieron CALL __CIsqrt incluso con #pragma intrinsic(sqrt) y /Oi. No satisfacen FSQRT inline. La opción propuesta es una TU /O2 /Oi /arch:IA32 /fp:fast con /Zi sin RTC/PCH/unity, cambiando exclusivamente los dos destinos de llamada en Shadows. Nunca aplicar /O2 a Shadows entero. El helper aislado /O2 coincide en cuerpo completo con referencia asm aislada /O2 (isolated-shadow-comparison.json), pero NO con el helper debug real.

## Lecturas después de la llamada real

`artifacts/actual-call-windows.json` muestra ambos sitios: StoreStaticShadow offset 0x92, StoreShadowForVehicle offset 0x14E. La primera instrucción posterior es ADD ESP,4, que sobrescribe OF/SF/ZF/AF/PF/CF antes de cualquier rama; después se almacena ST0 a qword local y se carga XMM0. Se usa PUSH ECX para reservar slots de float que se sobreescriben completos inmediatamente por MOVSS [ESP],XMM1: el valor de ECX no participa en el argumento. EAX no se lee antes de la siguiente llamada ShadowExtendedProduct y se sobreescribe antes del siguiente uso útil. EBX/ESI/EDI/EBP se preservan tanto por el helper debug como por el candidato sin uso de esos registros. Ambos cuerpos son lineales sin cambios a DF. Esta explicación limita el impacto del prólogo debug, NO acredita equivalencia universal de registros ni excepciones: candidato no reproduce relleno CC/RTC/stack footprint. Hay que auditar callers nuevos completos con el spec, exactamente las dos llamadas mapeadas y ninguna otra diferencia, y ejecutar ambos bancos antes de admitir.

## Propuesta y ejecución por root

`shadow_sqrt_cpp.patch` está separado y sin aplicar; hashes base en base_hashes.json. `ShadowSqrtCpp.cpp.proposed` y header son archivos revisables. Los bancos derivados conservan fixtures, trazas, límites, comparación exacta y restauración originales; sólo namespace/prefijo y fuentes hash cambian. No incorporan resúmenes antiguos ni declaran cobertura exhaustiva de x87 status, NaN, RC o registros. Esa cobertura exige banco de contrato adicional si se considera necesaria.

Con objeto nuevo copiado, completar `caller-spec-template.json` y ejecutar:

    python -B reversing/CppTrigCoffAudit/compare_coff.py --spec reversing/CppSqrtSmallStudy/caller-spec-template.json

Después root ejecuta serialmente los dos bancos y exige 8192 pares sin diferencias, original/candidate no nulos y restauración real. No registrar por los estudios de compilador.

## Otros candidatos bloqueados

WaterNearestDistance (WaterLevel.cpp:908; FindNearestWaterAndItsFlow 0x6E9D70; WaterNearest355,1024 pares): C++ /O2 mantiene árbol x87 pero reutiliza físicamente el argumento y como spill64; se requiere argumento_after o prueba completa de frame/consumidores. No propuesto para integración. CoverDistance2D (Formation.cpp:105; FindCoverPointsBehindBox 0x699FF0): candidato cambia orden y stack; FormationGather355 stubea el consumidor y NO valida helper. Banco real FormationCoverExpanded. Ninguno admitido.
