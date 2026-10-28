<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CppInteriorProposal — dos conversiones probadas dentro del juego

Resultado vigente: corrida `20261010-234420-320168`, 6.144 pares completos, cero diferencias y ningún resultado nulo. Pila, registros, banderas de Ceiling, entorno numérico, memoria, rutas y estado restaurados; partidas y configuración intactas. Evidencia archivada en `runs/20261010-234420-320168/`. Las notas de propuesta siguientes describen la preparación anterior a la prueba.

La fuente C++ aprobada vive en `source/game_sa/Interior/InteriorCpp.cpp`. Se compila por separado con `/Zi` en Debug, sin el PCH y sin incluirse en Unity. `/ZI` añadía `SUB ESP,192` incluso a la función vacía y cambiaba EFLAGS: el banco rechazó ese intento. La unidad nueva evita modificar la configuración del resto de Interior_c. No se escondió ensamblador ni se llamó al cuerpo original para resolverlo.

El primer intento `20261010-233402-229088` también agotó el espacio de la copia de pruebas y dejó un log incompleto: no concede aprobación, aunque los casos terminados de Center coincidieran. Se preservaron binarios de pruebas antiguas fuera del repositorio para liberar espacio; la corrida final terminó y se auditó completa. El intento `20261010-233936-150266` se canceló y tampoco concede aprobación.

Lote mínimo: `Interior_c::Shop_FurnishCeiling` (0x59A130, un RET) y `Office_FurnishCenter` (0x599A30..0x599AE4, RET completo). La propuesta no contiene asm, naked, bytes ejecutables generados ni llamadas al cuerpo original del método. No se ha aplicado a producción, compilado ni ejecutado GTA.

## Fidelidad de la propuesta

`Shop_FurnishCeiling` tiene cuerpo vacío C++. El EXE no lee ni escribe memoria, no cambia FP y conserva todos los registros. Debe comprobarse la compilación efectiva en ejecución: C++ no garantiza por sí solo la preservación de registros volátiles de una función vacía.

En `Office_FurnishCenter`, 599A47..599A68 usa multiplicación firmada por 0x2AAAAAAB y ajuste de signo para dividir entre seis truncando hacia cero. 599A71..599A8E resta el múltiplo, corrige signo y desplaza: equivale a `(valor % 6) / 2` con truncamiento hacia cero, también para dimensiones menores que seis. Se conserva el EAX final explícitamente mediante `return insetY`; 599ACB recarga ese valor tras cada columna poblada. Los argumentos a DeskQuad son (-1, x, y, int8 firmado de this+0x791), en orden columna/fila. El resultado del colaborador se descarta, como el EXE. No hay operaciones flotantes en este cuerpo.

## Banco derivado y alcance

Derivado de `InteriorCommerce`, conserva el receptor completo, bytes, traza ordenada, globals y restauración de hooks/parches. Se limita a 2.048 pares Ceiling y 4.096 Center. Añade medición de ESP, EBX/ESI/EDI/EBP, FP108 y MXCSR, y validación independiente de casos únicos, payloads no nulos y oráculo entero.

DeskQuad está instrumentado en su entrada EXE y dirección fuente, igual que el banco histórico. Es prueba de conversión del cuerpo con contrato del colaborador, **no** evidencia de creación real de muebles, ni nueva validación transitiva, ni admisión en la cohorte. El oráculo verifica dimensión byte completa, intermediarios negativos, IDs int8 firmados, return exacto y cada llamada en orden. No se aplican tolerancias ni normalización de FP.

El banco conserva asm únicamente en la instrumentación ABI, como los bancos anteriores. El coordinador debe revisar rutas/bytes reales y la política de registros volátiles antes de aplicar y ejecutar. La propuesta es revisable, pero **NO READY para admisión**; falta validación dentro del juego. Ceiling registra y compara EAX/ECX/EDX crudos: EAX entrada 0x59A130, EDX semilla 0xC1234567 y ECX receptor idéntico. Exige EFLAGS completos iguales antes y después de CALL.

`source-proposal.patch` pasa `git apply --check` sobre el árbol actual. Los bancos históricos no fueron editados.

Preparación offline terminada para revisión/aplicación y banco serial del coordinador. La compilación y ejecución pendientes son requisitos para afirmar equivalencia. El constructor del banco configura exclusivamente el clon E: antes de cargar probe_build. El manifiesto incluye InjectHooksMain y la implementación reversiblehooks usada.
