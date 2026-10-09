<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7193 con evidencia histórica, 0 con vigencia acreditada y 1253 pendientes en 23 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Vigencia del constructor de animación por nombre

Revisión del 23 de septiembre de 2026. Entrada: `CTaskSimpleRunNamedAnim,Constructor-Anim,0x0061A990,S`. [Resultado de la nueva ejecución](namedanim-validation.json). [Prueba anterior y alcance](../FallenPeds/README.md).

## Dependencias examinadas

| Ruta | Papel en la construcción |
| --- | --- |
| TaskSimpleRunNamedAnim.cpp/.h | Adaptador thiscall, argumentos, disposición de 0x64 bytes, flags, cadenas, desplazamiento y jerarquía |
| CTaskSimpleAnim → CTaskSimple → CTask | Inicialización de flags, animación nula y padre nulo; no se ejecutan ProcessPed ni Clone durante la construcción |
| CTaskTimer y CVector | Inicialización de los miembros del temporizador y del desplazamiento; el constructor no consulta CTimer |
| strcpy_s | Copias de cadenas por CRT; el banco usa nombres válidos que caben, no entradas nulas ni desbordamientos |
| CAnimManager::GetAnimationBlock(const char*) | Recorre las tablas de bloques; comparación de nombres mediante ci_string_view y _strnicmp en Windows |
| CAnimManager::GetAnimation(const char*, block) → CKeyGen::GetUppercaseKey → GetAnimation(hash, block) | Hash CRC con tabla constexpr y búsqueda de jerarquía por hash en el bloque |
| AnimManager.h, AnimBlock.h y AnimBlendHierarchy.h | Tipos y tablas residentes en memoria del host; el banco usa el bloque ped y un bloque inexistente |

Se revisaron cuerpos, constructores base, inicializadores y helpers inline. El destructor del fixture sale por `m_pAnim == nullptr`. La ruta registrada es la del constructor con estos inputs; los callbacks y las virtuales posteriores de la tarea no se certifican con esta revisión. En particular, ProcessPed continúa siendo original y sus 128 escenarios son una comprobación relacionada, no otra función candidata acreditada.

El historial entre 4516c03 y 11fd691 solo cambia dos archivos de `source`: el constructor y IKChain. Aun así, el manifiesto histórico no fijaba todas las dependencias de compilación. Para evitar atribuirle una cobertura que no registraba se recompiló una sonda desde el árbol actual y se repitió el banco dentro del motor.

## Nueva evidencia

Ejecución `resultados-namedanim-init/20260923-164058-543160`, sin exclusiones de hooks. **8.192 pares del constructor**, 128 de ProcessPed original y 512 de OffsetPedPosition: 8.832 en total, cero diferencias, error de posición máximo cero. Se comprobaron entradas independientes, las 256 × 16 × 2 combinaciones del constructor y contadores original/candidato 0/1. No se suman como nueva cobertura única respecto al banco anterior.

Sonda ASI: `7902d9c275ac7c7c00ecfab8790b8c01aac9e95bd59c384edc8f93187f6e3874`. Original Compact: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Log: `5f4339fcacb0e8fd60ce1034eb01396f3a7f108e7fd1254a5e110f239061a8e8`.

La compilación empleó Debug x86, Visual Studio 18 2026, MSVC 14.51.36231, plataforma Win32 y toolset v145. La caché de configuración y manifiestos están archivados en `artifacts/20260923-164036`; los binarios extensos permanecen ignorados. El resumen registra su hash. La acreditación corresponde a este entorno y alcance: no se extrapola a otro compilador, biblioteca externa, datos o configuración.

## Caducidad de la acreditación

Se capturaron antes de compilar las huellas completas de `source`, `cmake` y `conan`, además de configuración versionada, fuentes del banco y ejecutores. Se verificó su igualdad después de restaurar. El registro conserva esas huellas y el hash de este informe. Añadir, cambiar o retirar un archivo de esos árboles retira la acreditación al regenerar el panel, incluso si el cambio fuera inocuo: es deliberadamente conservador.

La huella abarca todas las fuentes del repositorio para no olvidar una dependencia transitiva del constructor. No afirma que todo ese código esté probado. Las bibliotecas y herramientas externas están limitadas al entorno de ejecución documentado; no se vigila su instalación mediante el contador portable del repositorio. Un nuevo entorno requiere revisar o repetir el banco antes de trasladar la acreditación.

Se restauraron fuentes normales, ASI/PDB en bin/Debug y copia de pruebas, ambos INI y sondas históricas predeterminadas. Partidas conservadas por hash. ASI normal sigue `ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f`. El proceso propio terminó automáticamente. No se tocó la instalación personal, no se forzó la ranura incompatible y no se cambió el CSV.

## Decisión

**Vigencia acreditada para esta entrada y el alcance del banco**, con 8.192 pares del constructor. No acredita toda la clase, todas las animaciones, estabilidad general ni independencia del host. La barra de dependencia de funciones originales permanece sin medir. Los otros dos registros no cambian de estado.
