<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Grasa/músculo, impactos de arma y natación: validación aprobada

Las tres entradas están reconstruidas, validadas y admitidas: 0x55C470, 0x55CEB0 y 0x55C990. Corrida **20261010-210539-196204: 6144 pares**, 2048 por función, **cero diferencias y cero resultados nulos**, rutas/colaboradores y restauración correctos. source-proposal.patch conserva la propuesta histórica aplicada a Stats.cpp y sus RH; no debe reaplicarse. El parche se escribió UTF-8/LF y se aplicó sin opciones de whitespace. Los 42 archivos del manifest existen.

## Fuente original completa

| Función | Evidencia de instrucciones y salidas |
| --- | --- |
| UpdateFatAndMuscleStats 55C470 | FatMuscle.dis.txt, 123 instrucciones. 55C494..55C499 conserva test ah,5/jp del límite; 55C4BA/55C567 distinguen unordered al comparar grasa/pérdida. 55C5CA imul conserva producto bajo de dword; 55C5D4..55C5DE usa multiplicación unsigned y desplazamiento para dividir por diez. El segundo contador se trata independientemente. RET 55C63B/55C65A. |
| UpdateStatsWhenWeaponHit 55CEB0 | WeaponHit.dis.txt, 73 instrucciones. 55CEB8 usa GetSkillStatIndex; 55CEE5 usa IsCoopGameGoingOn. 55CF25 separa cambio de arma de acumulación, 55CF54..55CF60 usa comparación mayor estricta para mostrar la actualización. RET 55CF3D/55CF81/55CF93. |
| UpdateStatsWhenSwimming 55C990 | Swimming.dis.txt, 76 instrucciones. 55C991..55C9AF selecciona ritmo normal/rápido por bytes de los dos bool; 55C9B5 ejecuta grasa/músculo antes de stamina/lung. 55CA35..55CA37 omite únicamente lung cuando el primer bool es cero. RET 55CA95/55CAB4. |

Las **272 instrucciones** se cotejaron automáticamente contra los desensamblados completos, normalizando nombres de puentes, destinos y ceros de literales. Una particularidad del decodificador: en 55C5D4 muestra `mul eax,edx`, pero los bytes originales **F7 E2** codifican `mul edx` con EAX implícito; la propuesta usa la sintaxis válida de MSVC y conserva ambos resultados EDX:EAX. Se conservan sólo etiquetas referenciadas para evitar C4102 sin desactivar advertencias.

## ABI y dominio

Métodos static/cdecl: un dword para grasa y arma, dos slots de dword para bool de natación; los llamadores limpian 4/8 bytes. caller-weapon.dis.txt muestra 73B853 seguido de add esp,4. caller-swimming.dis.txt muestra 68A197/68A1A2 escribiendo sólo AL/CL con sete y empujando EAX/ECX en 68A1A5/68A1A6; por eso el banco conserva bits altos arbitrarios y compara los bool por el byte bajo, no exige slots canónicos de 0/1. La puerta compara EAX/ECX/EDX completos y ambos argumentos físicos, ESP y EBX/ESI/EDI/EBP.

En WeaponHit, 55CEE1 escribe el valor de habilidad en el ECX guardado en [esp+8], no en el argumento. ECX puede cambiar legítimamente al retornar: no se inventa una garantía de preservación. Los tres cuerpos conservan sus RET originales.

El dominio de arma cubre **cada ID 22..32**: GetSkillStatIndex devuelve 69..78, con TEC9 32 alias de slot 6. Fuera de ese intervalo devuelve -1, y el original usa movzx si/index FFFF para leer fuera del array: esos tipos quedan excluidos explícitamente. No se fuerza el caso inválido ni se añade una guarda nueva. Co-op se prueba como condición de ambos punteros a ped no nulos; el fixture usa el ped cargado como valor válido en ambos slots durante la llamada, sin crear ni ejecutar una sesión real de dos jugadores.

## Colaboradores y restauración

IncrementStat55C180, DisplayScriptStatUpdateMessage55B980, GetSkillStatIndex743CD0 e IsCoopGameGoingOn441390 deben estar registrados, reconstruidos y ON compartidos por ambos lados. El observador comprueba flags y bytes esperados de RH, E9, símbolo y módulo. Los demás colaboradores conservan su modo inicial, incluido ftol821B40 CRT original. No hay stubs, objetos falsos con virtuales ni sustitución de texto/estadísticas.

FatMuscle se prueba como objetivo OFF/ON. Durante el objetivo Swimming permanece **ON y locked en ambos lados**, con una segunda ruta observada que demuestra cuerpo/símbolo/entrada del ASI y registra su metadata antes y después; se restauran flags y código al acabar Swimming. Esa colaboración compartida no sustituye la comparación propia de FatMuscle ni implica independencia transitiva de HUD/texto/CRT.

Se preparan y comparan **17 bloques crudos**: estadísticas B78200..B79537, ayuda completa, gString/gString2, ambos PlayerInfo, JustOutOfFrontEnd, flags enumerados, búfer de fallback y paso de tiempo. El snapshot se amplía respecto de Wave9 para incluir **B79534**, indicador modificado por grasa/músculo. Los lados restauran todo antes de publicar; el final comprueba los flags/código originales de los tres objetivos y colaboradores. x87 completo de 108 bytes y MXCSR se guardan/restauran exactamente. El runner verifica partidas y configuraciones; no se guarda sesión.

## Cobertura y puertas

2048 casos por función: 16 perfiles × 128 variantes, sin afirmar todo el producto cartesiano. Se serializan los inputs efectivos completos, incluidos límites, contadores unsigned, NaN/infinitos, pérdidas, habilidad, último slot, co-op y bits de bool. Se comparan todos los bloques crudos y registros, contadores/resultados explícitos y restauración; no se elimina salida para pasar.

El analizador calcula independientemente los efectos definidos de contadores con redondeo x87 PC24, truncación, producto bajo de dword/división unsigned y wrap. Exige rutas de grasa y salud maxima grant/acumulacion, grasa positiva/no positiva/unordered, indicadores 1/otros, pérdida positiva/otros y resultado positivo/clamp; WeaponHit co-op/habilidad/cambio/mostrar/acumular; Swimming ritmo normal/rápido, stamina grant/acumular y lung omitido/grant/acumular. Exige todas las armas y los cuatro bool en inputs efectivos, 6144 pares únicos, cero nulos y gates/rutas/restauración correctos. complete_paths deriva de inputs serializados y oráculos de contadores, **no de tracing de ramas por instrucción**.

Enumeración estática prevista: Fat grant1056/acum992, salud maxima grant864/acum1184; Weapon co-op1024/habilidad408/cambio308/mostrar232/acum76; Swimming normal512/rápido1536, stamina grant960/acum1088, lung omitido1024/grant464/acum560. Las demás ramas requeridas también aparecen; son previsiones, no ejecución real.

Sólo el coordinador aplica y prueba, después de completar Wave9:

`bash reversing/_handoff/bench.sh DependenciesVehicleStats10 3`

## Evidencia final

[Registro completo](runs/20261010-210539-196204/comparisons.jsonl.gz), [resumen](runs/20261010-210539-196204/summary.json), [aprobación](runs/20261010-210539-196204/approval-audit.json), [resultado](runs/20261010-210539-196204/RESULTADO.txt) y [auditoría independiente](audit-runtime.md).

SHA-256 del registro descomprimido: `92b924ac1a782c07b1ff53ab839931f26c5df4262ab02b02e0243491877c9399`.

SHA-256 del ASI ejecutado: `7bfe88b1b786c4497d5e604dd288c85d9460e35cdf8d58795686e718dfc1b7be`.

El runner informa partidas sin cambios y configuraciones restauradas. La auditoría independiente confirmó todos los pares, sus efectos de contador y dwords del estado crudo, y la colaboración FatMuscle DLL real en ambos lados de natación. No se reconstruyen LoadConditions, ciclismo, otras armas ni funciones de librería fuera de esta tanda.

El segundo contador B7950C es m_MaxHealthCounter; se varian tambien su incremento y valor de salud maxima. No es un contador de musculo.
