<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Alternativa Debug pendiente de inspección

La devolución constante C++ permanece igual; se habilita optimización solo alrededor de Get2dEffectStore bajo _DEBUG y después se restaura off, siguiendo el patrón presente en ClearInterestingVehicleList. Esto evita depender del cuerpo /Od con prólogo RTC de relleno que modifica ECX. No se usa inline asm, naked ni llamadas GAME.

La expectativa es MOV EAX,0xB4C2D8 seguido de RET, o una secuencia equivalente que no modifique ECX, EDX, registros no volátiles, flags ni x87. MOV/RET originales no cambian flags. C++ no garantiza esos efectos de registros o flags por sí solo: pragma optimize tampoco constituye prueba; hay que inspeccionar el objeto real con /ZI, /RTC y opciones efectivas antes de ejecutar el banco.

Si aparece SUB/ADD ESP, XOR, REP STOS, instrumentación de comprobación, llamadas de seguridad o aritmética, no admitir como equivalente total por conservar solo el retorno. El banco heredado detecta ECX/EDX/ESP y EAX pero no mide flags ni todos los efectos internos; esas diferencias exigen inspección o ampliar evidencia, no eliminar requisitos.

No aplicar source-proposal.patch y debug-optimized-proposal.patch uno después del otro sin revisar contexto: el segundo se ha generado contra la fuente disponible en el momento de esta auditoría (naked original). El archivo candidate-debug-optimized.cpp.txt contiene el resultado completo de la función.

No se ha compilado ni ejecutado GTA; instrumentos del banco permanecen intactos.
