<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría estática independiente — FormationMemory8

Fuente aplicada; revisión de sólo lectura, sin build, juego ni cambios a instrumentos. Identidades exactas entre source y banco: CFormation::FindNearestFreeDestination69B1B0 y categoría Rs/global psGetMemoryFunctions745510. Los alias semánticos están declarados como tales.

## Fuente y ABI

Las 52 instrucciones normalizadas de Formation coinciden con52 del EXE69B1B0..69B230, preservando x87 FSQRT, comparación strict/unordered, primer empate y coste acumulado incluso sin destino (constante86D270). No hay CALL. Getter: MOV EAX,8D6228 y RET745515 completos, dirección real, sin invocar callbacks. Retornos naked explícitos.

Formation cdecl CVector por valor (tres floats) y float*total: banco coloca x/y/z/total y limpia16bytes; fuente lee ESP+10/14/18/1C después de sus tres pushes. Getter sin parámetros. ESP/EBP/EBX/ESI/EDI medidos; retorno EAX completo y coste por bits. Hooks instalados en CFormation y RsInjectHooks ya llamado; no familia extra.

## Cobertura y legalidad

CPointList13C consta de count,24 vectores,24flags; globals reales C1A318/C1A324/C1A43C coinciden con instrucciones. Count0..24, ninguno/todos/alternos/primer/último libre, puntos3D, exacto, empates y distancias fuera del límite. Conteos negativos se excluyen como estado inválido del count unsigned, sin inventar contratos. Un punto NaN quieto específico ejerce comparación unordered original sin colaborador ni indexado ilegal; los demás fixtures son finitos. No se fabrican actores/pools. Empate mode5 exige retorno0; máscaras con selección exigen destino libre; domain=-1 o índice dentro count. Resumen exige seleccionado/no seleccionado, vacío/full y empate; las rutas de máscara/unordered están representadas por casos reales aunque no tienen contador específico de resumen.

Getter64 pares suficientes para cuerpo sin ramas; puntero8D6228,16bytes readonly y4punteros callbacks en páginas ejecutables. No demuestra identidad semántica ni ABI de callbacks y no los invoca: sólo valida getter. Esos destinos siguen fuera del alcance.

Snapshot/restauración de CPointList completo, FNSAVE/FRSTOR y MXCSR; tabla se compara readonly. Cada función usa entradaEXE originalOFF y candidatoON, con RH registrado y ruta comprobada. No colaboradores del juego ni stubs. Guardas de coste y entradas se verifican antes de restore. Estado completo y dos rutas restaurados al final. No defecto bloqueante estático identificado; la admisión requiere run completo sin diferencias/nulos y restauración. README aún muestra propuesta sin aplicar: descripción editorial pendiente, no validación afirmada.
