<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11695 con evidencia histórica y 746 pendientes de validación en 32 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn222A: 16 funciones del EXE (lote 222A)

Estado: **APROBADO dentro del juego** (17/11/2026, worktree `wv-5` sobre `ce4513dd9a`, sin cherry-pick; plantilla `Fn219A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn222a/20261117-130915-052632` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn222A-20261117-130915-052632`, `comparisons.jsonl.gz`). 16 x 16384 = **262144 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia (24 bits) y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `a1e8295e34d3e2ba617ba1552880c3487e771134ca49fd25d98b171a26ad0cfc`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El lote ya estaba integrado con hooks y `validated=0` (`reversing/PendienteValidar/222A.md`). Faltaba el banco; el banco encontro **dos defectos de la fuente**, corregidos (abajo). Sin x87 en ninguna de las 16 funciones (el informe mencionaba x87 solo en otras partes del bloque): la compuerta x87 comprueba que quede vacia con la palabra de control a 24 bits.

| # | Funcion | Entrada | Tipo |
|---|---|---|---|
| 0 | `CProtectionStub::Empty` (stdcall con un argumento, vacia) | 0x4032A4 | S |
| 1-2 | `CProtectionStub::BlockEncrypt` / `BlockDecrypt` (cifrado de bloque de 8 bytes) | 0x403638, 0x4036C1 | S |
| 3 | `CBuilding::Constructor` (cuerpo) | 0x403E00 | S |
| 4-5 | `CAEAudioEntity::Destructor-6` / `Destructor-7` (copias del cuerpo del destructor) | 0x447000, 0x447050 | S |
| 6 | `CAutoPilot::SetCarMissionUnlessCrashAndBurn` | 0x463490 | S |
| 7 | `CDecisionMaker::SetDefault` | 0x4634D0 | S |
| 8 | `CEntity::IsInvisibleOrRenderDamaged` | 0x46A2F0 | S |
| 9 | `CTheScripts::ScriptTextPairSet` | 0x46A340 | S |
| 10 | `CEntity::Add()` (virtual, hueco 2) | 0x533020 | V |
| 11-12 | `CPtrNodeSingleLinkPool::AddItem` / `RemoveItem` | 0x5335E0, 0x533610 | S |
| 13-14 | `CPtrNodeDoubleLinkPool::AddItem` / `RemoveItem` | 0x533670, 0x5336B0 | S |
| 15 | `CEntity::ScalarDeletingDestructor` | 0x536C20 | S |

Excluidas: ninguna. Saltadas (ya documentadas en `PendienteValidar/222A.md`, no se pueden escribir en C++ puro): las 16 rutinas restantes del bloque de proteccion del EXE (0x402A36, 0x402DC5, 0x4032C5, 0x40335F, 0x403379, 0x4033B2, 0x4033F5, 0x40374C, 0x403902, 0x403B03, 0x403B7B, 0x403B7D, 0x403C14, 0x403C70, 0x403C88, 0x403D07).

## Defectos corregidos por el banco
- **`BlockEncrypt` y `BlockDecrypt`** (`ProtectionStub.cpp`): la primera version leia todo el bloque y la tabla antes de escribir. El EXE lee el bloque de memoria al empezar cada ronda, escribe B y A a lo largo de ella y lee la tabla entre medias (S[1] tras escribir B+4; S[3+2i] tras escribir B+4; en el descifrado S[24-2i], S[1] y S[0] tras escribir A). Con la tabla solapada con el bloque el resultado cambia: el primer intento del banco (2048 casos por funcion) dio 674 y 701 pares distintos de 2048. Ahora los accesos son volatiles y van en el orden del desensamblado.
- **`CEntity::Add()`** (`Entity.cpp`): `Add(GetBoundRect())` hacia que el compilador reutilizara la vtable de la primera llamada (por ser `GetBoundRect` const), mientras que el EXE relee `[esi]` antes de llamar a `[edx+4]`. El banco cambia la vtable del objeto desde el falso de GetBoundRect: 1025 de 2048 pares distintos (el EXE llamaba a la vtable nueva y la DLL a la vieja). Ahora se llama por la vtable con dos lecturas volatiles del puntero y el rectangulo es un bloque de 4 floats sin copiar (sin x87).
El resto de las fuentes del lote no necesito cambios.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40, familias 0..15 (posicion del objeto x relleno). Casos: cifrado con la tabla en B (alineada o no), solapada con el bloque en A (cualquier desplazamiento de -0x66 a +7) o detras; mision con valores 0x38/0x39/0x3A/0x3B y argumento con ruido; las cuatro combinaciones de los bits 7 y 9; cadenas de 0 a 0x30 bytes de origen externo, delante del destino o leyendo lo que escribio la primera copia (desbordan sobre la segunda); `Add()` con vtables falsas de 24 huecos en las arenas (los huecos distintos de 1 y 9 anotan que se llamaron por error) y cambio de vtable a mitad; nodos nuevos que se solapan con la propia lista; listas de 0 a 6 nodos con elementos repetidos, ausentes o nulos y anteriores incoherentes (nulos o a basura de C); destructor con borrado con ruido en los bits altos del BYTE. Colaboradores: falso identico en la direccion del EXE y en el cuerpo de la DLL (`CEntity::CEntity`, `CAESound::~CAESound`, `CDecision::SetDefault`, operator new/delete de nodos, `CEntity::~CEntity`); `operator delete` del CRT solo en la direccion del EXE. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/Fn222A/chain222.sh` (humo de 2048 casos, corrida principal y rondas en una sola sesion del candado). `python -I fixture_selfcheck.py` pasa los 262144 casos por el oraculo sin juego con cobertura de ramas >= 50 por marca.

## Prueba negativa
`mutate.py` (17 mutaciones por ronda que cubren las 16 funciones, condicionadas a `this` o un puntero de argumento desalineado a 4 bytes, que el juego real no produce; sin cambios de convencion de llamada). **Las 16 funciones fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261117-133056-818548` (10975 de 32768 pares con diferencias) y ronda 2 `20261117-133743-921342` (10477 de 32768); el lado EXE coincide con el oraculo en todos los pares. Incidencias de la primera tanda (registradas en `chain.out`): las mutaciones de los destructores de audio no compilaban (`plugin::CallMethod` exige un puntero a clase: la tanda corrio sin mutar y se descarto); una mutacion de `ScriptTextPairSet` (segunda cadena un byte mas alla) rompe el invariante de terminacion de las copias solapadas y una de `Add()` llamaba a un hueco con otra firma (pila desbalanceada): ambas colgaban el juego y se sustituyeron por mutaciones seguras. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas).

## Fuera de los fixtures
Pool de nodos lleno (el EXE escribe en la direccion 4 u 8 y revienta), `this` nulo o crudo, listas con enlaces a memoria no mapeada o con ciclos, cadenas sin terminador, tablas de cifrado fuera de las arenas y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x403E00 -> `Fn159A`, `Misc8`; 0x533020 -> `Physical56`, `UnlockPhysical`, `RequestedGeometryWrappersFive` (**repetir**: la fuente de `CEntity::Add()` cambio); 0x5336B0 -> `CEntB`. El resto de direcciones no aparece citado.
