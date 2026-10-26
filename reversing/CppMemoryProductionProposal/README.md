<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Propuesta C++ de las cuatro funciones de memoria PC

`memory_cpp.patch` está preparado sin aplicar. No se compiló ni se ejecutó. Sustituye sólo los tres wrappers CRT y las cuatro ramas PC de Malloc/Realloc/MallocAlign/FreeAlign. InitScratchPad y las ramas del heap alternativo permanecen fuera del alcance.

Los wrappers usan firmas `__cdecl` de la biblioteca CRT del EXE: malloc 0x824257, realloc 0x824269 y free 0x82413F. No llaman los cuerpos originales del juego. `__forceinline` permite observar el retorno desde la propia candidata; requiere confirmar el resultado compilado y sus xrefs, sin trampolines de instrucciones emitidas manualmente.

Malloc/Realloc ignoran hint y mantienen una llamada CRT con tamaños/identidades originales, incluyendo realloc(NULL,0) y realloc(p,0). MallocAlign realiza sumas en uint32 antes de la máscara, guarda la base en aligned-4 y conserva el comportamiento nativo ante alineamiento inválido u OOM: no introduce validaciones ni una salida NULL artificial. La campaña sólo admite el dominio legal previamente aprobado; no intenta provocar esos accesos inválidos.

## FreeAlign: condición pendiente, no garantía del lenguaje

El EXE reescribe `[ESP+4]` con la base antes del tail jump a free. La propuesta usa `void* volatile memory` y asigna la base al parámetro, luego llama a free. La calificación superior del parámetro no cambia la firma declarada. Volatile exige la escritura al objeto parámetro, pero **C++ no garantiza que ese objeto corresponda al slot entrante del caller**. Su ubicación depende de MSVC x86 y las opciones efectivas. No basta con que free reciba la base correcta: DependenciesMemoryCpp16 exige además que el slot observado por Gate cambie exactamente a esa base.

En /Od una asignación ordinaria al parámetro suele almacenarse en `[EBP+8]`; eso es una hipótesis del compilador, no una validación realizada. En /O2 la asignación ordinaria puede desaparecer. La variante volatile evita eliminar la escritura al objeto, pero debe probarse que no escribe una copia local. Si produce un diagnóstico por parámetro volatile con el estándar efectivo, debe revisarse explícitamente; esta propuesta no silencia advertencias ni autoriza asm/intrinsics para forzar la escritura.

Si falla el slot físico, el parche queda bloqueado. La siguiente alternativa C++ a estudiar es limitar optimización a FreeAlign con `#pragma optimize("", off)` y parámetro normal/asignación, seguido de restauración de optimización. También exige desensamblado y el mismo banco. No se incluye una segunda aplicación automática ni se rebaja el contrato.

## Secuencia de admisión

1. Compilar y ejecutar el observador con la fuente ASM actual para demostrar que reproduce Memory9, incluida calibración negativa y restauración.
2. Aplicar el parche únicamente cuando el responsable decida comenzar el experimento serial.
3. Revisar desensamblado/xrefs: llamadas sólo a CRT, ninguna delegación a cuerpos originales; comprobar escritura del slot FreeAlign.
4. Ejecutar DependenciesMemoryCpp16 completo: cuatro funciones, 8192 pares, cero diferencias, argumentos crudos exactos y restauración de debug/VEH/helper/hooks/FP.
5. Conservar la evidencia del banco anterior y registrar sólo después de esa demostración.

No se aplicó este parche ni se modificó producción, runtime_runner, GTA o Git al prepararlo.
