<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 05/11/2026 (Ciudad de México):** 837 grupos / 9153 funciones reconstruidas; 9153 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Player82B (lote 82B)

`CPlayerPed::Compute3rdPersonMouseTarget` (EXE 0x60B650, thiscall, ECX = jugador, un `bool` de pila, `ret 4`, void; 0x426 bytes medidos con `dis.sh`). Hook `S`.

Con el raton en tercera persona (`CCamera::m_bUseMouse3rdPerson`, byte 0xB6EC2E) traza una linea desde el origen de la camara hasta `origen + frente * alcance` (alcance = `m_fTargetRange` de la estructura
del arma activa) buscando solo peatones (`ProcessLineOfSight` con `pIgnoreEntity = jugador` y `bIncludeBikers = true`) y se queda con el primer peaton vivo distinto del jugador. Si el argumento es distinto de
cero (`meleeWeapon`) la linea sale de `CCamera::Find3rdPersonCamTargetVector`; si no se calcula aqui (fuente de la camara activa, desplazada `P - Q * dot` cuando `Q . (P - jugador) < 0`).
Con peaton: registra la referencia en `m_p3rdPersonMouseTarget` (+0x79C; antes `CleanUpOldReference` del anterior), fija el plazo +0x7A0 = reloj + 1000 y, solo con arma de fuego (tipo de disparo con `GetWeaponInfo(tipo, STD)` distinto de
cuerpo a cuerpo) con el bit 0 de las banderas, peaton a la vista (`IsInSeeingRange` con la posicion del jugador) y sin una tarea `TASK_COMPLEX_REACT_TO_GUN_AIMED_AT` (0x259) en las tres primeras
ranuras primarias, dice la frase 0xB0 (salvo con el arma 0x17) y avisa con un `CEventGunAimedAt`: al grupo del peaton (`CEventGroupEvent` + `CPedGroupIntelligence::AddEvent`, salvo si ya estan en el mismo grupo)
o a su `CEventGroup`. Sin raton o sin peaton, el objetivo anterior caduca (`+0x7A0 < reloj`, comparacion sin signo) sin tocar su referencia.

## Reconstruccion
`PlayerPed.cpp` (el metodo) y `X87Math.cpp` (`Player82BAimLineCpp`, la aritmetica x87 a 24 bits de la linea sin vector de camara; usa `Player82ARound24` del lote 82A). El producto escalar
`(Qy*dy + Qz*dz) + Qx*dx` y los productos `Q*dot`, `Qx*r`, `Qy*r` se quedan en la pila x87 sin guardar (exponente ampliado); se guardan como float las tres componentes de la fuente ajustada,
`Qz*r` (que se relee de la pila de llamadas) y el objetivo. El indice de la ranura de arma se lee con signo (`movsx`) en cada uso.

## Banco
- 16.384 pares (16 familias de 1.024 casos): flujo completo, raton apagado o con valor distinto de 0/1, ruta con vector de camara, ruta calculada con producto escalar negativo, bordes del signo (0, -0, minimos),
  trazado (negativo, golpe nulo / el propio jugador / otro peaton / muerto), objetivo anterior (nulo, mismo, otros), cadena de armas (cuerpo a cuerpo, bit 0, ranuras entre -20 y 20, arma 0x17), rango de vision y tareas
  pendientes, grupos, valores no finitos y especiales, exponentes variados, jugador con y sin matriz, camaras 0..2, plazo y reloj (bordes de 1000 ms y de 2^32).
- Falsos identicos en ambos lados (parche de entrada del EXE y del cuerpo de la DLL; todos con hook salvo `CEvent::operator new`): GetWeaponSkill 0x5E6580, GetWeaponInfo 0x743C60 (una estructura distinta por llamada),
  Find3rdPersonCamTargetVector 0x514970, ProcessLineOfSight 0x56BA00, ResetLineTestOptions 0x5631C0, IsAlive 0x5E0170, CleanUpOldReference 0x571A00, RegisterReference 0x571B70, IsInSeeingRange 0x600C60, Say 0x5EFFE0,
  GetPedsGroup 0x5F7E80, AreInSameGroup 0x5F7F40, CEvent::operator new 0x4B5620 (sin hook: la entrada del EXE sigue siendo codigo original; entrega una ranura real del pool de eventos), AddEvent 0x5F7470 y CEventGroup::Add 0x4AB420.
  Registran sus argumentos en orden (punteros como etiquetas de arena; las ranuras de la pila como `0xFFFFFFFE`) y, AddEvent y Add, los campos escritos por los constructores de los eventos.
- Corren de verdad en ambos lados: constructores y destructores de `CEventGunAimedAt` (0x4B0700/0x4B07B0) y `CEventGroupEvent` (0x4ADFD0/0x4AE070), `CEvent::operator delete` y el borrado del pool de eventos (el destructor virtual del EXE lo
  inlinea; por eso el evento reservado es una ranura real del pool, cuya cabecera y bytes de estado se restauran tras cada lado). La tabla virtual de las tareas es falsa (48 ranuras; la 4 devuelve `GetTaskType` del caso).
- Oraculo independiente `fixture_oracle.py` (modelo desde el desensamblado): cada lado de cada par coincide con el.

## Fuera de los fixtures / dominio
- NaN: la x87 y SSE propagan cargas utiles distintas; todo float observado se canonicaliza.
- `GetWeaponSkill` devuelve 0..3 en EAX (el EXE real devuelve un entero pequeno; los bits altos basura no son dominio: la DLL los trunca a su tipo `eWeaponSkill`).
- Camaras activas distintas de 0..2, ranuras de arma fuera de -20..20 y peatones con inteligencia o matriz nulas (el EXE se caeria) no se prueban.
- Suma con diferencia de exponentes mayor que 29 bits: la DLL la hace en `double` y redondea a 24 bits (doble redondeo posible en teoria; sin diferencias en el banco).
- EAX/ECX/EDX de salida no se comparan (nadie los lee).

Repetir: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Player82B`. Evidencia: [validation-summary.json](validation-summary.json).
