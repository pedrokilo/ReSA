# Alternativa Debug pendiente de inspección

La devolución constante C++ permanece igual; se habilita optimización solo alrededor de Get2dEffectStore bajo _DEBUG y después se restaura off, siguiendo el patrón presente en ClearInterestingVehicleList. Esto evita depender del cuerpo /Od con prólogo RTC de relleno que modifica ECX. No se usa inline asm, naked ni llamadas GAME.

La expectativa es MOV EAX,0xB4C2D8 seguido de RET, o una secuencia equivalente que no modifique ECX, EDX, registros no volátiles, flags ni x87. MOV/RET originales no cambian flags. C++ no garantiza esos efectos de registros o flags por sí solo: pragma optimize tampoco constituye prueba; hay que inspeccionar el objeto real con /ZI, /RTC y opciones efectivas antes de ejecutar el banco.

Si aparece SUB/ADD ESP, XOR, REP STOS, instrumentación de comprobación, llamadas de seguridad o aritmética, no admitir como equivalente total por conservar solo el retorno. El banco heredado detecta ECX/EDX/ESP y EAX pero no mide flags ni todos los efectos internos; esas diferencias exigen inspección o ampliar evidencia, no eliminar requisitos.

No aplicar source-proposal.patch y debug-optimized-proposal.patch uno después del otro sin revisar contexto: el segundo se ha generado contra la fuente disponible en el momento de esta auditoría (naked original). El archivo candidate-debug-optimized.cpp.txt contiene el resultado completo de la función.

No se ha compilado ni ejecutado GTA; instrumentos del banco permanecen intactos.
