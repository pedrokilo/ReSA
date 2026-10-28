<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Arnes local GangDriveBy (fuera del juego)

Verificacion previa a `Dependencies21D`; no sustituye al banco ni acredita validacion. Ejecuta los bytes reales del EXE (imagen mapeada en un proceso hijo de 32 bits, colaboradores falsos identicos en EXE y DLL,
x87 a 24 bits) frente al texto de `TaskSimpleGangDriveBy.cpp` (tipos falsos de `h/mock.h`) y `X87Math.cpp` compilado con las opciones del proyecto.
Antes de cada llamada la pila x87 se carga con un residuo distinto por registro (cuatro FSW iniciales); despues se compara FNSAVE completo: FSW, FTW, TOP y los diez bytes de cada ST, ademas de memoria, traza, errno, ESP, no volatiles y SEH.

`python build.py 100000 -1 3` (casos, funcion -1 todas / 0 PlayerTarget / 1 AITarget / 2 AimGun / 3 Constructor, semilla).
