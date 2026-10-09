<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7478 con evidencia histórica, 0 con vigencia acreditada y 968 pendientes en 17 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dos funciones restantes: eventos y muerte por daño

Banco **preparado**, sin compilación ni ejecución. No altera fuentes funcionales ni el registro. [Manifest de preparación](preparation-manifest.json) distingue hashes observados de procedencia de una compilación real futura.

- `CInterestingEvents::InvalidateEvent`, S 0x00602960: 720 pares previstos. Ocho direcciones exactas de miembros, null y un objeto externo vivo, selección activa -1..7, referencia null/no-null y cuatro patrones de bytes. Modelo independiente: sólo el slot exacto cambia tiempo y referencia; selección activa sólo se borra si coincide. Stub de cleanup comprueba entity y dirección exacta de slot y deja la referencia para que el target la borre. Se comparan todos los bytes, trazas y modelo.
- `CPedDamageResponseCalculator::ComputeWillKillPed`, S 0x004B3210: 9216 pares previstos. CivMale/Player1/Player2, cinco booleanos independientes (invencibilidad, mega punch, force death, speak miembro y argumento), ocho armas 0/1/46/47/53/54/22/55, doce pares salud/daño finitos. Incluye resto 1 y ambos vecinos float, daño 5 y vecino inferior, resto 10 y vecino superior, suelo 5 y salud 0. Modelo numérico procede del dossier EXE y usa contextos nativos 0x159/0x158/0x165. Se comparan ped/calculator/response completos, sin normalizar bytes.

`CPed::IsPlayer` conserva su implementación original y observa el campo ped type de almacenamiento privado. Force-death y Say se parchean en ambas direcciones original/DLL con stubs explícitos: force devuelve la decisión elegida sin escribir objetos; Say comprueba identidad, contexto y los seis argumentos. El resultado no acreditará ComputeWillForceDeath, voz, actores, pools ni cadenas reales de referencias. Las entradas de fuerza/voz/cleanup y las dos banderas de cheats se restauran con comprobaciones completas de bytes y flags. Las pruebas no cubren NaN/infinito, solapamiento inválido ni llamadas concurrentes.

El banco exige un harness pregraphics aislado; no tiene guard de FindPlayerPed ni depende de CGameProcess. Toda falla y su salida completa deben conservarse. El analizador futuro debe exigir 9936 filas, conteos 720/9216, ambas gates, comparación con modelo, restauración, salida natural 0 y `passed=true` explícito. No hay autorización para registrar estos targets hasta el replay aprobado del coordinador.

Los dossiers REA originales se copiaron byte por byte a `artifacts/rea`; pseudocódigo y assembly se extrajeron sin análisis nuevo. InvalidateEvent recorre hasta ocho slots y retorna tras la coincidencia; KillPed cubre cuerpo 0x4B3210..0x4B33A1. Los hashes de las respuestas copiadas constan en el manifest. Los identificadores de evidencia enviados por el coordinador son `ev_2159d77b9c006d3b029153dddb44e6ab6b06835af30c43287fbc97362368b1be` y `ev_7f7b749383665fec1fc54ae5eb32053b519a0ec3623a36cb60df1877718ea3f3`, respectivamente.

## Harness y ejecución preparados

`build_probe.py` instala instrumentación temporal sólo al invocarlo el coordinador, captura ASI/PDB y manifest de fuentes/instrumentos y restaura WinMain y binarios normales en finally. `Preflight.hpp` compara los 52 bytes originales/candidatos de todos los hooks, tablas virtuales GTA/DLL de cada categoría virtual registrada mediante el mapa explícito, flags y 12 bytes de timer. Una categoría virtual desconocida rechaza el harness. No contiene parches CRT.

`run_local.py` audita por streaming el producto exacto de 9936 fixtures y pares únicos, resultados frente al modelo, trazas, gates, colaboradores y restauración. Retorna passed=false con diagnósticos en lugar de aprobar diferencias. El runner compartido corregido conserva ese veredicto y exige cierre natural 0; quedan archivados el log completo, harness, summary y manifest enriquecido. Nada de este apartado fue compilado ni ejecutado todavía.

Los logs de más de 90 MB se conservan íntegros localmente y se agregan como gzip determinístico sin pérdida (mtime=0), con SHA comprimido/descomprimido y verificación de roundtrip. Sólo ese archivo crudo en su carpeta de run queda ignorado por Git; los veredictos y demás datos no cambian.
