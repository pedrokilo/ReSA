<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente — DependenciesTask10

Lectura directa de todos los registros del gzip admitido `20261010-211257-052931`, sin ejecutar el banco ni cambiar instrumentación.

- 6144 pares únicos (0..6143), tres funciones con casos 0..2047 y 2048 perfiles por función: 256 máscaras × cuatro modos de miembros × presencia/ausencia de tareas.
- Todos los original/candidate son diccionarios no nulos, estrictamente iguales y equal/accepted=true; cero diferencias.
- 159744 gates verdaderos: ESP, no volátiles, SEH, RH OFF/ON, colaboradores, referencias, contrato, guardas, recursos, pool original intacto, estado restaurado, FP y arenas privadas.
- Verificación independiente del primer grupo libre/retorno AddGroup, actividad final (incluidos flags activos 0x80 de perfiles con tareas), índices SCM y todos los slots de miembros/inteligencia definidos por Flush. Cada observación incluye 5792 bytes de grupos, ocho flags, 16 bytes de índices y 16 estados de pool privado.
- Las tareas restantes coinciden con 8 menos los grupos vaciados; 49152 observaciones de rutas virtuales reales tienen módulo y gate cierto. Los doce colaboradores compartidos tienen rutas verificadas.
- Pool original capacidad 500: vigilancia de 64000 bytes de almacenamiento y 500 de bitmap, además de cabecera/puntero. Gate cierto por lado y al final; nunca se escribe sobre esos bytes para esconder efectos.
- Registro terminal completo: 6144 pares, restauración, colaboradores, x87/MXCSR y pool original ciertos. El registro de admisión confirma partidas/configuración y rutas RH.
- Huellas actuales: 32/32 fuentes y 3/3 instrumentos coinciden con la admisión.

Log SHA-256: `99f8aba128bc48a25992148c699baff964eb3273801bd494711beff3482da23f`. ASI SHA-256: `e481a394f1f46ed8ad6f61a5abdd71a3cde42ded8de9d18687aa1b51c1261aed`. Gzip SHA-256: `4de2e2eb3dfb3c2a12ec62dd9d80e53f33ad15b9330611d42ab25b0f7e5c7a93`.

Límites: jugador real, nodos CReference y pool privado reales, con constructores/destructores y registro/limpieza originales o rutas RH compartidas actuales. Cadena del jugador, nodos originales y arenas se restituyen antes de bombear mensajes. No se certifica independencia transitiva, metadatos internos del heap ni la rama no-jugador de RemoveMember. El intento previo rechazado por espacio no cuenta como prueba.
