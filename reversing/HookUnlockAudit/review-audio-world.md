<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revision estatica de locks audio/render/world (10/10/2026)

Alcance: 105 filas locked=1 del catalogo. Hay **8 locks explicitos actuales** y 97 registros de fuente sin lock explicito. No se ha ejecutado GTA en esta subtarea; ninguna fila acredita seguridad nueva. Inventario por hook, motivo exacto, ubicacion fuente, evidencia historica y plan: [CSV](review-audio-world.csv).

## Ocho locks reales

- CAEVehicleAudioEntity::ProcessPlayerVehicleEngine 0x4FBB10, ProcessVehicle 0x501E10, Service 0x502280: comentarios fuente advierten crashes aleatorios al desactivar. VehAudioFinal4 conmuta todos los hooks conjuntamente con fixtures/stubs; evidencia de equivalencia102/20024 no cubre las ocho mezclas ni concurrencia/audio activo. Mantener hasta probar individualmente y conjunto, ambos sentidos, en gameplay motor/radio/subir/bajar/cambio vehiculo; registrar referencias, requests, hilo, hashes y restauracion.
- CEntryExit::GetPositionRelativeToOutsideWorld 0x43EA00: candidato prioritario. EXE no escribe EDX; ret4 en 0x43EA84/0x43EA8C. Fuente wrapper naked conserva EDX porque manager0x43F150 lo usa como contador. Simple::Switch OFF restaura EXE y reemplaza primeros5bytes del wrapper por JMP EXE antes de pushEDX; evita doublepop. Exige banco ON/OFF/ON desde EXE/simboloDLL con ESP/EDX centinelas y caller manager real, enlaces y pisos; luego teleport/interior gameplay con restauracion. No autoriza retirar lock por analisis estatico.
- CPostEffects::ImmediateModeRenderStatesStore 0x700CC0: comentario dice EAX overwritten unhooked. EXE0x700CC0 mov eax,[00C97B24], diez calls driverGet[engine+24], ret0x700D62; EAX es caller-saved y cambia tambien en EXE. No asumir fallo arreglado: probar binding DLLRwRenderStateGet, callbacks, diez valores, ESP y Store/ReStore sobre driver vivo, despues frames efectos activos. Fuente manipula ms_imf y debe restaurarse completamente.

- Game::Render2dStuff0x53E230, Idle0x53E920, FrontendIdle0x53E770: source/app/app_game.cpp65/70/71 tienen locks reales. Render2dStuff advierte bloqueo del juego sin hook. Idle conserva guarda ForegroundApp NOTSA; comentario describe politica antigua de menu F7. Su banco requiere punto de conmutacion fuera del frame en pila y transiciones menu/partida/foreground con render vivo. No prueba solamente cuerpo aislado.

## Discrepancia historica

Audio radio/script, camaras, agua, sombras y HUD tienen filas catalogadas como locked pero registro actual sin opcionlocked; HookInstallOptions en ReversibleHooks.h106 usa locked=!reversed y reversed=true por defecto. Confirmar Locked real exportando inventario ASI construido desde fuente actual; corregir catalogo a runtime no constituye desbloqueo nuevo. No reutilizar fechas/hashes antiguos como certificacion de conmutacion nueva.

## Restricciones

Cambios de codigo5bytes no atomicos: conmutar en punto de frame controlado sin caller Audio concurrente en funcion afectada. Restaurar bytes, estado hook/lock, world, RW/audio, partidas e INI. Si no es posible controlar hilo sin afectar comportamiento real, conservar lock. Stubbed pair equivalence no demuestra integracion render/audio ni estabilidad general. Para cada candidato conservar original/candidate no nulos, mezclas, destinos reales, diferencias0, estado restaurado y escenarios gameplay; reportar limites.
