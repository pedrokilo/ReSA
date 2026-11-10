<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 880 grupos / 9863 funciones reconstruidas; 9863 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn129A: catorce funciones de catorce clases (lote 129A)

Estado: **APROBADO dentro del juego** (08/11/2026, worktree `wv-1` sobre main 6b525edfd0 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn129a/20261108-165916-329195` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn129A-20261108-165916-329195`; `comparisons.jsonl.gz` de 27,8 MB). 14 x 16384 = **229376 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 igual (ST0 de 80 bits en la funcion 2) y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `0e776373e97a641db54f013624ebdd79c99946d36a6dfa7b78c47f89ba51a745`; del EXE: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Plantilla: `Fn124A` (`Gate`, `HookRoute`, falsos identicos de colaboradores con hook), con arenas nuevas (terciaria y de slots), regiones globales vigiladas (tabla 0x8AD914 y conversaciones 0x9691D8) y falsos de RenderWare, delete[] y vtable de eventos.

| # | Funcion (nombre real) | Dir. | Bytes | Que hace | Fuente | Listado decia |
|---|---|---|---|---|---|---|
| 0 | `CRGBA::Copy` | 0x463370 | 31 | copia 4 bytes uno a uno (el origen puede solaparse), devuelve this | `RGBA.cpp` | CRoadBlocks |
| 1 | `CEventEscalator::TakesPriorityOver` (V, ranura 11) | 0x4B25E0 | 32 | tipo del otro evento y luego el propio; AL = distintos | `Events/EventEscalator.cpp/.h` | CEventEscalator |
| 2 | `CAEPedAudioEntity::SurfaceVolumeDb` | 0x4DA510 | 33 | ST0 = 20 log10(fild(tabla[b*16]) * 0.01f) | `Audio/Entities/AEPedAudioEntity.cpp` (HandlePedHit la usa) | CAECollisionAudioEntity |
| 3 | `CVehicle::SetEngineOn` | 0x41BDD0 | 47 | bEngineOn = motor roto ? 0 : bit 0 del argumento | `Entity/Vehicle/Vehicle.cpp` | CGeneral |
| 4 | `CAEMP3BankLoader::GetLoopOffset` | 0x4E0380 | 48 | -1 sin inicializar; si no LoopStartOffset (slot int16, sonido uint16, sin comprobar; se quita el assert) | `Audio/Loaders/AEMP3BankLoader.cpp` | CAEMP3BankLoader |
| 5 | `CAccidentManager::Constructor` | 0x56CE30 | 51 | 16 entradas a cero (iterador CRT 0x821D30 y 0x56CDE0 reales) | `AccidentManager.cpp` | CWorld |
| 6 | `CAESmoothFadeThread::Constructor` | 0x4EE9A0 | 52 | campos +0..+0x818 (instancia global 0xB608D0) | `Audio/AESmoothFadeThread.cpp` | CAEScriptAudioEntity |
| 7 | `CColModel::Destructor` | 0x40F700 | 52 | libera datos de colision (0x416400 + 0x72F430, o 0x40F070 + delete) | `Collision/ColModel.cpp` | CCollisionData |
| 8 | `CPlane::SetHeadingMatrix` | 0x6C4130 | 54 | matriz de giro en Z con fsin/fcos | `Entity/Vehicle/Plane.cpp` | cBuoyancy |
| 9 | `CPools::PoolDestructorBody` | 0x4049C0 | 54 | cuerpo del destructor de CPool (delete[] 0x8213AE); `DestroyPool` lo reutiliza | `Pools.cpp` | CPedPool |
| 10 | `CConversations::EnableConversation` | 0x43AA40 | 55 | byte m_Enabled de la primera conversacion del ped | `Conversations/Conversations.cpp` | CConversations |
| 11 | `CShadowCamera::RecreateRaster` | 0x705480 | 57 | destruye y recrea el raster cuadrado de la camara | `ShadowCamera.cpp` | CShadowCamera |
| 12 | `CVector::Cross_OG` | 0x70F890 | 57 | producto vectorial x87; cada componente se guarda antes de la siguiente | `Core/Vector.cpp` | CSprite |
| 13 | `CCollisionData::Constructor` | 0x40F030 | 58 | ceros hasta +0x2F salvo los 5 bits altos del byte +7 | `Collision/CollisionData.cpp` | CColLine |

Ninguna tenia hook ni estaba en otro worktree; no se salto ninguna. Se enlazan con envolturas estaticas (`RH_ScopedNamedGlobalInstall`) para no tocar cabeceras comunes (solo `EventEscalator.h`). Hallazgos del banco: el ECX de `Cross_OG` es el resultado (this); en la x87 un NaN senalizante en el operando de memoria de `fmul` pierde frente a un QNaN del registro y con el mismo significando gana el de signo positivo (el oraculo lo modela). `0x4B25E0` pide primero el tipo del otro evento.

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, `negative-runs/`): ronda 1 (13 mutaciones, `20261108-161714-962617`): fallan 13 funciones; ronda 2 (14 mutaciones, `20261108-164708-306162`): fallan las 14 (incluida GetLoopOffset, que en una ronda previa con slot sin signo detuvo el banco por lectura fuera de la arena; esa mutacion queda como ronda 3). Fuentes restauradas con SHA256 identico y corrida final repetida con la sonda reconstruida.

## Bancos repetidos
Citan las direcciones: `Dependencies26A` (0x4E0380) y `DependenciesGarageSphere15` (0x40F030): ambos APROBADOS sin cambios (`resultados-dependencies26a/20261108-171342-450000`, `resultados-dependenciesgaragesphere15/20261108-171636-094900`).
