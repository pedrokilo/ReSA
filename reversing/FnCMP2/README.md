<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnCMP2: segunda auditoria de comparaciones float con NaN (19/11/2026)

Continuacion de `reversing/FnCMP1`. Rama `cmp2` (worktree `D:\Original\wv-16`, desde `cmp1` = 9d70070b99). Solo analisis estatico del EXE (lectura, nunca se ejecuta) y parches en C++ puro.
NO compilado, NO validado, NO subido. El worktree es disperso (sparse): solo `source`, `docs`, `reversing/FnCMP1|FnCMP2|FnBIS1|validation-registry` (D: estaba al 98 %).

## Por que sale tanto

MSVC emite `fcom + fnstsw + test ah,MASK + jcc` y casi todas las formas dejan el NaN en un lado distinto del que fija IEEE para el operador de C++:
`a<=b` -> `test 0x41 + jne` (NaN entra), `a>=b` -> `test 5 + jp` (NaN entra), `a<b` -> `test 1 + jne/je` (NaN entra), `a>b` -> `test 0x41 + je/jp` (NaN sale). Que el C++ coincida depende del lado
y del orden de los operandos (`fld K; fcomp x` frente a `fld x; fcomp K`). Un NaN que el EXE sanea solo (recortes `x>=K?K:x`) o que cambia de rama es lo que en ProcessWheel llego al mundo.

## Metodo (`scan2.py`, regenerable; lee el EXE de `E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe`)

1. Por cada funcion de `source/game_sa/**/*.cpp` (sin RenderWare) con direccion EXE (`docs/hooks.csv`, `RH_Scoped*Install` o comentario `// 0x......`) se extraen TODAS las comparaciones `< > <= >= == !=`
   con algun operando float (tipos por heuristica: nombres float de las cabeceras, locales `float`, literales, `Magnitude/Dot/sqrt...`; `!(a OP b)` se reconoce). Con o sin literal.
2. Del EXE: cada `fcom/fcomp/fucom/ftst + fnstsw + test ah/and eax/sahf + jcc` (`fcmp.py` de FnCMP1) con el conjunto de casos {mayor, menor, igual, NaN} que salta y que sigue, y la constante si se ve
   (`fcomp [K]`, `fld K` justo antes, `fld1/fldz`, `ftst`; en FnCMP2 se corrigio que `fcomp st(1)` con la constante en st0 no la veia).
3. Cada comparacion C++ da su conjunto de casos verdaderos T (con `NaN` solo en `!=` y `!(..)`). Se busca una comparacion del EXE cuyo lado (salta o sigue) tenga la misma particion no-NaN que T (con o sin
   operandos intercambiados; con constante, el orden queda fijado por donde esta la constante); IGUAL si ademas el NaN cae del mismo lado, DIFERENTE si cae del contrario. Reglas de la tarea comprobadas a mano en `0x6A8E91`,
   `0x6D71C9`, `0x6B298A`, `0x6ADB8B`, `0x6B7DB3`, `0x6F4835`, `0x5FCA56`.
4. Emparejamiento: (a) comparaciones con literal: por constante y tipo de particion, sin orden; si el lado del NaN no tiene pareja y el EXE de ese cubo (constante + tipo) solo tiene el lado contrario
   ("disjunto" / "elem") la diferencia es segura aunque haya varias comparaciones; (b) con empate se usa ademas la alineacion por orden fuente/direccion dentro de cada constante (`orden`);
   (c) sin literal: LCS por orden con compatibilidad de particion (confianza baja; solo se parchea a mano si el cubo es 1/1 o 2/2 y la linea es clara).
5. Parche (`patch2.py`): complemento ordenado que no cambia nada sin NaN: `a >= b` -> `!(a < b)`, `a <= b` -> `!(a > b)`, `!(a < b)` -> `(a >= b)`, etc., con comentario `// NaN como el EXE (0xDIR): ...`.
   Seleccion automatica en `select2.py` / `select3.py` (sel1..4.csv son las listas aplicadas).
6. Prueba de recall: ejecutado sobre el arbol original (cff02cea81) vuelve a encontrar CarAI 302, GangWars 285/1090/1107/1149, ManageObject 2365, Train 1221/1230/1259, PlaneSpotter 2676 (esta ultima solo
   tras corregir que un ORDEN no robe la constante). No encuentra `FxEmitterBP::Render` ni `PopulationCouple` (sin direccion en el escaner).

## Cifras

Universo: 10 200 comparaciones float en 2 429 funciones de `game_sa`; 8 361 en 1 899 funciones con direccion EXE; 1 839 sin direccion (lambdas, ayudantes, `PopulationCouple`).
En las zonas de riesgo (fisica, vehiculos, peatones, tareas, camara, colision, streaming, poblacion, armas, bandas...) 5 780 comparaciones (4 482 IGUAL, 125 DIFERENTE, 10 FALSO_POSITIVO, 907 INDETERMINADO, 256 SIN_DIRECCION).

B (barrido completo, `auditoria2.csv`, ordenado por riesgo = zona*10 + escrituras de posicion/velocidad/matriz en la funcion):
IGUAL 6 433 | DIFERENTE 150 | FALSO_POSITIVO 16 | INDETERMINADO 1 762 | SIN_DIRECCION 1 839.
DIFERENTE: 131 parcheadas en 71 funciones (`bancos_a_repetir2.csv`); 19 sin parchear (18 en audio, HUD, nubes, espejos, plantas, Fx, interiores, tuberia: no llevan el NaN al mundo; 1 resto).
Los FALSO_POSITIVO son sitios donde el C++ ya trata el NaN a mano (`isnan`, `x != x`, p. ej. Boat 1112, Vehicle 2059, VehicleRecording, PointInPoly).

A (los 162 `POSIBLE_DIFERENTE_NAN` de FnCMP1, `auditoria2_A.csv`): DIFERENTE 101 (87 parcheadas, 14 fuera de zona), IGUAL 29 (4 ya confirmadas en FnCMP1; Automobile 969/3373/5102, Boat 1106, Physical 2528, CarCtrl 971/4694/4919
resultaron IGUAL: son recortes con la constante en st0, el NaN se conserva igual que en el EXE), FALSO_POSITIVO 7, INDETERMINADO 25.

## DIFERENTE (parcheadas, por zona)

* Vehiculos: `CAutomobile::VehicleDamage` (3625/3627), `ProcessBuoyancy` (5802), `ProcessControlInputs` (2345/2346/2353, a mano con el desensamblado de 0x6ADB8B..0x6ADBC4: con NaN el EXE va a marcha atras/frena),
  `CBoat::ProcessControl/ProcessControlInputs/BlowUpCar/AddWakePoint`, `CBike::ProcessAI/ProcessRiderAnims/ProcessControlInputs`, `CBmx::PreRender`, `CTrain::ProcessControl` (1259, 1596) y `DoTrainGenerationAndRemoval`,
  `CVehicle::ProcessWheel` (3870/3876: con speed NaN el EXE normaliza, el C++ ponia direction.x=1), `ProcessDrivingAnims`, `AddDamagedVehicleParticles`, `AddWheelDirtAndWater`, `DoBoatSplashes`, `AddExhaustParticles`, `SpecialEntityCalcCollisionSteps`.
* Peatones/tareas: `CPed::KillPedWithCar/OurPedCanSeeThisEntity/DoFootLanded/PlayFootSteps`, `CPlayerPed::EvaluateNeighbouringTarget`, `CPedGeometryAnalyser` (2), `CPedGroupPlacer::PlaceFormationGroup`, `CPedDamageResponseCalculator::ComputeWillKillPed`,
  `CEmergencyPed::InRange`, `CEventDeadPed::AffectsPed`, tareas ArrestPed, EnterCar, FollowLeaderInFormation, KillCriminal, KillPedOnFootArmed, Duck, GangDriveBy, GoToPointFine, PlayerOnFoot, SitIdle, Swim, LineUpPedWithCar.
* Camara: `CCam::Process`, `Process_Cam_TwoPlayer_Separate_Cars`, `Process_FollowPedWithMouse`, `Process_FollowPed_SA`, `CCamera::TryToStartNewCamMode/AvoidTheGeometry`, `CameraControlPedCycle`, `CIdleCam::IsTargetValid`.
* Mundo: `CStreaming::DeleteRwObjectsBehindCamera` (4) y `Update`, `CCarGenerator` (2), `CGangWars::CreateAttackWave/PickZoneToAttack`, `CPlayerInfo::Process/FindClosestCarSectorList`, `CPopulation::ManageObject`, `CProjectileInfo::Update`,
  `CRoadBlocks::GenerateRoadBlocks`, `CRope::Update`, `CTimer::UpdateVariables`, `CDoor::IsFullyOpen`, `CGarages::StopCarFromBlowingUp`, `CWeapon` (DoTankDoomAiming, PickTargetForHeatSeekingMissile, FireInstantHit), `CVector2D::Normalise`.

Pendiente de decision: `Door.cpp:260` decia "como el original: NaN devuelve true" pero `0x6F4835` (`fcompp; test ah,0x41; jp -> return 0`) devuelve false con NaN; se parcheo segun el desensamblado (revisar con su banco).
`ProcessWheel` 3870/3876 hace que con NaN el C++ copie el comportamiento del EXE (propaga) en lugar de sanearlo: si el banco de BIS1 depende del saneado, revertir esas dos.

## Limites

* 1 839 comparaciones en funciones sin direccion y 1 762 INDETERMINADO (sin comparacion x87 libre de la misma particion: enteros mal clasificados como float por la heuristica, comparaciones inlinadas o ya consumidas,
  constantes con `fld` lejano de la comparacion). Sin ancla (parte B sin literal) la confianza es baja: solo se parchearon 14 casos de ORDEN con cubo 1/1 o 2/2.
* No se cubren `std::min/max/clamp/abs`, ternarios dentro de ayudantes inline, `fcomi`, comparaciones de `double` en SSE, ni `Cam`/`Weapon`/`Tasks` con mas de 8 comparaciones iguales por constante sin disjuncion (quedan INDETERMINADO).
* La orientacion del NaN se calcula sobre la comparacion emparejada; un emparejamiento erroneo en cubos con varias comparaciones iguales puede mover un parche a la hermana (la suma de diferencias no empeora). Los parches
  de cubo 1/1 o "disjunto/elem" son los de confianza alta; los de `orden` (CarGenerator 114, Bike 623/686/690, Bmx 558/560, ProcessWheel 3870/3876, KillCriminal 654) son de confianza media.
* Tras compilar hay que repetir los bancos de `bancos_a_repetir2.csv` (66 de 71 tienen banco en `reversing/validation-registry`; los bancos actuales no incluyen entradas NaN en esos operandos).

## Archivos

`scan2.py` (escaner), `patch2.py` (aplica parches), `select2.py`/`select3.py` (selecciones), `final2.py` (CSV finales), `fcmp.py`/`exedis.py` (de FnCMP1), `pre_head2.csv` (escaneo antes de los parches, rama cmp1),
`post2.csv` (despues), `auditoria2.csv`, `auditoria2_A.csv`, `bancos_a_repetir2.csv`.
