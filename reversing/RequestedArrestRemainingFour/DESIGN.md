<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ArrestPed: cuatro entradas pendientes

Banco preparado, todavía sin compilar ni ejecutar. Ninguna función recibe acreditación por este documento.

| Identidad CSV preservada | Entrada PE exacta | Pares |
| --- | --- | ---: |
| Constructor S 0x0068B990 | 0x0068B990 | 128 |
| Destructor S 0x0068BA00 | 0x0068BA00 | 256 |
| MakeAbortable V 0 | vtable 0x8709A8, slot 6: 0x0068BA60 | 384 |
| CreateFirstSubTask V 0 | vtable 0x8709A8, slot 9: 0x006907A0 | 2048 |

El EXE canónico tiene SHA-256 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b. Los dossiers de constructor y destructor proceden de la campaña REA. Las consultas nuevas de REA a las dos virtuales devolvieron invalid_request; se conserva la respuesta, sin atribuir pseudocódigo inexistente. Bytes PE y dumpbin documentan los cuerpos y los once slots originales. CreateFirstSubTask termina en 0x6908BD; MakeAbortable es el tail jump de ocho bytes que delega a slot 6 del child.

Constructor cruza 64 poisons y target null/privado. Comprueba que sólo se inicializan parent, subtask, referencia y vehículo; conserva byte 0xC y campos 0x14..0x1F. Destructor cruza 64 poisons, referencia null/privada y child null/privado: cleanup precede scalar delete con flags 1 y el terminal nativo CTask 0x86D48C debe corresponder al terminal DLL CTask exacto. Los vptr crudos se registran y verifican por rol; se comparan separadamente los otros 32 bytes, sin sustituirlos ni afirmar equivalencia global de las tablas.

MakeAbortable cruza 16 poisons, prioridades 0/1/2, evento null/privado, retorno false/true y actor null/privado. Child siempre no nulo con vtable privada y handler ABI explícito. El modelo exige un único delegate con argumentos exactos y retorno AL. Actor null sólo acredita passthrough del contrato del fixture; no acredita una tarea real.

CreateFirstSubTask cruza cuatro poisons, ocho escenarios, 16 controles y cuatro slots de arma activos: target null, a pie, bike, quad, boat, upright, upside y side. Controles independientes melee/shotgun/pistol y resultado delegado null/privado. El modelo reconstruye ramas, escritura del byte 0xC, llamadas y argumentos desde las instrucciones canónicas. Captura GiveWeapon(22,10,false), SetCurrentWeapon y ambas consultas de disponibilidad cuando corresponden. CreateSubTask se intercepta; sus constructores y colocación real no están acreditados aquí.

Nueve gates interceptan ambos cuerpos cuando existen: RegisterReference, CleanUpOldReference, CreateSubTask, IsUpsideDown, IsOnItsSide, IsTypeMelee, DoWeHaveWeaponAvailable, SetCurrentWeapon y GiveWeapon. Cada entrada propia conserva 52 bytes; la región escrita sigue siendo exactamente cinco. Las arenas de ped, target, vehicle, child, evento y salida deben permanecer intactas. El harness pregraphics preserva flags, 17692 rangos de entradas/tablas, timer y control FP, y exige salida natural 0 y restauración completa de instalación/configuración/saves.

No cubre actores construidos, animación, pools ni ejecución real de delegates, eventos interpretados por subtareas reales, prioridades fuera de 0..2, target en vehículo con puntero nulo ni integración de juego. Los helpers sólo aportan trazas controladas. CSV con direcciones V=0 permanece intacto; futuras aprobaciones deben resolver exactamente estas cuatro identidades y mantener dependency_review=null hasta revisión adicional.
