<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CVehicle: reconstrucción de RemoveObjectsCB

## Estado

RemoveObjectsCB-Object esta reconstruida y activada; su llamador RemoveReplacementUpgrade utiliza almacenamiento separado para los indicadores. La compilacion completa Debug x86 finalizo correctamente.

Las pruebas aisladas aprobaron 393.475 casos del callback y 12 del llamador. Dentro del motor real aprobaron 60 comparaciones de vehiculos y 3 casos adicionales contra las funciones originales, sin diferencias y con cero cambios residuales en referencias. El ejecutor independiente repitio satisfactoriamente las 63 comparaciones.

La instalacion de pruebas es `D:\JUEGOS\GTA San Andreas - CVehicle Test`; conserva por separado el juego y las partidas originales. Se desactivan los hooks Audio y Global/SurfaceInfos_c para esta prueba debido a errores ajenos al objetivo observados durante las sesiones. No se certifica estabilidad general ni aspecto visual.

Para repetir sin Codex ni consumo de tokens, ejecutar `PROBAR.cmd --case cvehicle` en la carpeta principal. Ver [instrucciones del ejecutor](runtime/README.md) y [resultados detallados](runtime/results.json).

## Función localizada

- Registro: CVehicle / RemoveObjectsCB-Object.
- Dirección virtual: 0x006D33B0; RVA: 0x002D33B0.
- Firma: RwObject* __cdecl RemoveObjectsCB(RwObject*, void*).
- Es un callback global registrado en la categoría CVehicle.
- EXE analizado: D:\Grand Theft Auto San Andreas\gta-sa.exe.
- Tamaño: 5.189.632 bytes.
- SHA-256: 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b.

Para este caso se usó el desensamblador de Visual Studio (dumpbin). Sus 98 bytes de código permiten analizar la función directamente, sin instalar Ghidra ni un tracer general.

## Comportamiento reconstruido

1. Si el objeto no es un atomic de RenderWare (tipo 1), devuelve el mismo puntero sin tocar data.
2. Obtiene los indicadores con CVisibilityPlugins::GetAtomicId (0x732370).
3. Escribe los 32 bits del resultado en data. Esta escritura ocurre incluso si no elimina el atomic.
4. Si está activo ATOMIC_UPGRADE (0x800), devuelve el objeto sin eliminarlo.
5. Obtiene CBaseModelInfo (0x732260) y conserva el frame antes de destruir el atomic.
6. Retira el atomic del clump (0x74A4C0) y lo destruye (0x749DC0).
7. Consulta el identificador de jerarquía del frame (0x732A20). Si es cero, destruye el frame (0x7F05A0).
8. Si existe información de modelo, llama a RemoveRef (0x4C4BB0).
9. Devuelve el valor del puntero recibido, como hace el original, sin volver a acceder al objeto destruido.

No se añadieron validaciones de null que cambien el contrato original: data debe apuntar a memoria escribible si el objeto es un atomic.

## Corrección del llamador

CVehicle::RemoveReplacementUpgrade (0x6D39E0) pasaba &frameOfUpgrade a los callbacks.
El callback original sobrescribe ese almacenamiento con los indicadores del atomic.
La siguiente utilización del frame podía entonces usar esos indicadores como una dirección de memoria.

El desensamblado del llamador original mantiene el frame en EDI y usa un almacenamiento separado, inicializado a cero, para los indicadores.
La corrección usa int32 atomicFlags = 0 y pasa &atomicFlags a ambos recorridos.
Esto restaura el contrato del ejecutable original. No es un cambio de comportamiento intencional del juego.

## Pruebas realizadas

El programa de pruebas extrae las tres funciones de un ejecutable cuyo hash debe coincidir exactamente:
callback de objeto, callback de frame y RemoveReplacementUpgrade.
Relocaliza siete operandos de dirección absoluta comprobados; conserva los desplazamientos relativos.
Ejecuta el código x86 en un proceso propio y lo compara con las funciones extraídas del Vehicle.cpp editado y compiladas con MSVC x86.

Las dependencias del motor se sustituyen por dobles controlados. Se comparan el orden y los argumentos de llamadas, la salida escrita en data, el retorno, el contador simulado de referencias y efectos simulados sobre atomic/clump/frame.

Resultados:
- 393.475 casos de callback aprobados.
- 12 casos del llamador aprobados.
- Todos los valores de indicadores de 16 bits, combinados con jerarquía 0/7/-1 y modelo presente/ausente.
- Los 255 tipos de objeto distintos de atomic, con data nulo.
- Cuatro valores adicionales que prueban bits altos de la salida de 32 bits.
- Compilación sin optimización y con /O2; advertencias de C++ tratadas como errores.
- Control negativo: eliminar la escritura de indicadores falla.
- Control negativo: restaurar el paso de &frameOfUpgrade produce acceso inválido (0xC0000005) en el proceso de prueba.
- La ejecución final con el código correcto vuelve a aprobar.

Estos son casos de prueba aislados, no sesiones de juego ni evidencia exhaustiva del motor RenderWare. Los dobles no prueban los efectos internos reales de destrucción, memoria, texturas o recorrido de listas del motor.

## Archivos y reproducción

- source/game_sa/Entity/Vehicle/Vehicle.cpp: candidato y llamador corregido.
- reversing/CVehicle/Vehicle.cpp.before: respaldo previo.
- original-callbacks.asm.txt y original-caller.asm.txt: desensamblado.
- differential.cpp: banco de comparación x86.
- validate.py: extracción, verificación de hash, compilación y ejecución.
- check_regressions.py: controles negativos y validación final optimizada.
- test-build/result.json y test-build/regressions.json: resultados.
- changes.patch: diferencias exactas respecto al respaldo.
- full-build.log: compilación completa y dependencias.
- final-build.log: recompilación final aprobada tras ajustar el registro.
- build-project.ps1: preparación de rutas locales y comando de compilación.
- manifest.json: hashes de la fuente final, el ASI y sus entradas.

Ejecutar desde esta carpeta, con Python instalado:

    python validate.py --exe "D:\Grand Theft Auto San Andreas\gta-sa.exe" --optimized
    python check_regressions.py "D:\Grand Theft Auto San Andreas\gta-sa.exe"

El script conoce la ubicación de Visual Studio 2026 de esta computadora. En otra máquina habría que ajustar esa ruta.

## Validación completada y límites

El callback ya está habilitado y el catálogo actualizado. El banco dentro del motor aprobó 60 pares de vehículos y tres casos adicionales, incluyendo componentes conservados/retirados, jerarquía y referencias; [resultado verificable](runtime/results.json). No está pendiente volver a habilitarlo.

Sigue fuera de este banco la cobertura exhaustiva de vehículos, la revisión visual y una sesión prolongada. Los ciclos finitos comparados no garantizan estabilidad general.

El 142/142 se refiere únicamente a los hooks registrados bajo CVehicle. En Vehicle.cpp aún hay funciones comentadas en el registro o redirecciones al original fuera de ese conteo. No se puede equiparar ese porcentaje con la reconstrucción de toda la clase ni de todo el subsistema de vehículos.


## Historial de preparación y compilación inicial

Se preparó Conan 2.32.0 en un entorno virtual local y una caché separada.
Se compilaron las dependencias declaradas por el proyecto con MSVC 19.51, x86 Debug y CMake 4.2.0.
El primer intento encontró CMake fuera del PATH; se corrigieron las rutas de las herramientas descargadas.
La compilación completa y la recompilación final terminaron con código 0.

El resultado diferencial conserva el hash de la fuente probada antes del cambio exclusivo de metadatos del hook.
Se verificó que los cuerpos de las tres funciones compiladas por la prueba son idénticos a los de la fuente final.
En esa primera etapa aislada, el callback quedó registrado con reversed=true y enabled=false; todavía no se había arrancado GTA ni instalado el ASI. Posteriormente se completó la validación dentro del motor descrita al principio y se habilitó el hook. Esa etapa inicial no representa el estado vigente.

## Desbloqueo posterior de conducción

CVehicle conserva 142/142 entradas registradas y ahora no tiene candados. ProcessDrivingAnims se corrigió y desbloqueó con 16.384 pares; otros 256 comprueban la ABI de GetVehicleAppearance. [Banco de conducción y límites](../UnlockDriving/README.md). Estas pruebas son adicionales y no sustituyen la evidencia del callback. La [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente) identifica el ASI instalado en la copia de pruebas.
