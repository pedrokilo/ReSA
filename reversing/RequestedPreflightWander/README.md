<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Preflight de GetWanderType sin inicialización gráfica

2026-10-08. Sonda compilada; todavía no ejecutada por este agente. ASI `3ae5526495a6c6d228c31b5046a1718ca7898a4ce9e47c05a0f72bb075c60df1`. ASI/PDB, hashes de fuentes e instrumentos y registro de compilación en `artifacts/`.

El constructor de sonda modifica temporalmente `NOTSA_WinMain(HINSTANCE,HINSTANCE,LPSTR,INT)` con la firma WINAPI real. Después de construir su guardia de cierre del registro y antes de SystemParametersInfo, llama al preflight y devuelve0. No inicializa RenderWare, gráficos ni input, y no acredita WinMain ni el arranque del juego. WinMain permanece enganchado: el runner no envía unhook ni selector histórico.

El wrapper guarda primero todos los flags/candados y las entradas EXE/DLL; después desactiva todos los hooks salvo WinMain. Invoca sin cambios el banco congelado `RequestedParallelReplay/WanderType/LiveComparison.hpp`:1280pares de getter, cinco rutas, bytes exactos y vtables. Restaura todos los hooks y verifica entradas y flags. El getter es puro; sus objetos están en pila, con subtask nulo y timer de inicialización local, sin pool ni globales RW. No se acredita construcción ni destrucción como funciones independientes.

El analizador exige tanto la aprobación íntegra del getter como las cuatro etapas del harness, restauración efectiva y cierre natural del proceso con código0. El runner común usa `stop_before_analysis=False`, `wait_for_exit=True`, menú-2 y ningún argumento unhook. Preserva logs, manifest y fallos anteriores. La ejecución queda coordinada por root.

Compilación Debug con `_CL_=/MP6` y un único proceso exterior. Captura ASI/PDB y manifiesto antes de restaurar WinMain.cpp y los binarios previos en finally. Se verificaron hashes congelados y restauración exacta. Los bancos de otros agentes se compilan en serie.
