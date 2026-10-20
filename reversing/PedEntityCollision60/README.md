# Banco PedEntityCollision60: CPed::ProcessEntityCollision (0x5E2530)

**Estado: FUENTE C++ ESCRITA Y REVISADA CONTRA EL EXE (sin compilar). BANCO NO CONSTRUIDO.** Esta carpeta sólo contiene `source.patch` y este README. No hay Core.hpp / Fixtures.hpp / oráculo / run_local.py: la función (4.342 bytes, 1.180 instrucciones, ~20 colaboradores, objetos con destructor y SEH) es demasiado grande para construir además un banco par a par fiable sin poder compilar ni ejecutar. Nada de lo de aquí está validado contra el juego.

## Fuente

`source/game_sa/Entity/Ped/Ped.cpp` (worktree D:\Original\wc-4, base HEAD 26c881718): sustituye la envoltura `plugin::CallMethodAndReturn<..., 0x5E2530>` por la reconstrucción en C++, añade dos ayudantes en espacio anónimo (`PecCopyColPoint` = `CColPoint::operator=` del EXE 0x40FC80 sin relleno 0xC/0x1C/0x26; `PecDamageEvent` = secuencia CEventDamage + AffectsPed + CPedDamageResponseCalculator + ComputeDamageResponse + CEventGroup::Add), siete `#include` (EventDamage, PedDamageResponseCalculator, TaskSimpleUseGun, TempColModels, Camera, Collision, SurfaceInfos_c) y activa el hook `RH_ScopedVMTInstall(ProcessEntityCollision, 0x5E2530)` (virtual: entrada 22 de la vtable de CPed, 0x86C3B0). `source.patch` = `git diff HEAD` de ese único archivo; `git apply --check` contra `git show HEAD:...Ped.cpp` en un directorio temporal: OK.

## Mapa de la función (desensamblado 0x5E2530..0x5E3626, `ret 8`)

1. Prólogo SEH; `colmodel` del ped = `ms_modelInfoPtrs[(int16)idx]->+0x14`, datos `+0x2C`. Con `bTestForBlockedPositions && usesCollision` (0x474 bit15): si es jugador con cámara en modo 0x35 y arma con `bAimWithArm`, guarda `GetHeading()` y hace `SetHeading(atan2(-front.x, front.y))` (fpatan); en cualquier caso cambia al colmodel temporal 0x968E20/0x968E4C. Si `!usesCollision && !(physFlags&0x10000)` devuelve 0.
2. Si no hay flags 0x19000 / attachedTo / entidad es ped: arma las líneas de suelo (línea 0 start.z=0, end.z=-1 (+ gravedad×dt si bWasStanding); con `bCheckColAboveHead` línea 1 y radio/caja del colmodel), si no, `numLines=0`, radio 1.0, caja z -1.0..0.95.
3. `CCollision::ProcessColModels(matPed, colPed, matEnt, colEnt, colPoints, líneas[2], maxTouch[2], flag)` (flag = bIsStuck && (edificio || phys&4)).
4. Rama de suelo (línea 0 toca): brillo de superficie, entidad de contacto (RegisterReference), techo (field_588), corrección de Z y normal, field_578, superficie de contacto / pendiente; luego daño por caída o golpe (`WEAPON_FALL`, pieza torso) y segunda pasada de ProcessColModels sin líneas si el jugador subió.
5. Filtro de puntos (brazos/medio bloqueados, normal de tren), restauración del rumbo, empuje horizontal contra estáticos, punto de contacto extra, `AddCollisionRecord` en ambos sentidos y `m_bHasHitWall`.

## Revisión propia contra el EXE (fase 2)

Se anotó el desensamblado con el desplazamiento de pila real de cada instrucción (script en el scratchpad; marco: 0xF0 + 3 push + push ebx) para identificar cada variable local y se contrastó bloque a bloque. Discrepancias halladas y corregidas durante la escritura (todas de lectura de pila, ninguna en la versión entregada):
- los `[esp+X]` anteriores y posteriores a `push edi`/`push ebx` difieren en 4/8 bytes: `maxTouch` está en k54/k58, `colPoints de línea` desde k60 (no k5C), `startZ` en k5C, `numCPs` en k18, normal "mejor" en k40..k48 (no k44), rumbo guardado en k4C;
- `ProcessColModels` recibe `(matPed, colPed, matEnt, colEnt, colPoints, líneas, maxTouch, bool)`: la dirección de `maxTouch` es k54 y la de las líneas k60 (la 4.ª pila por la derecha), no al revés;
- `CTaskSimpleUseGun +0x30` es `m_WeaponInfo` (no la animación) y el bit comprobado de `CWeaponInfo +0x18` es `bAimWithArm` (bit 1);
- las comparaciones `test ah,5; jp` equivalen a "no menor" y `test ah,41h; jne` a "no mayor" (incluido NaN), y `test ah,44h; jnp` sólo salta con igualdad exacta (normalización de la normal contra estáticos);
- el filtro de puntos desplaza con `CColPoint::operator=` del EXE (no copia 0xC, 0x1C, 0x26) y repite el mismo índice tras cada borrado;
- `bOnBoat` y `bTryingToReachDryLand` se limpian con `0xFFFFFF7D`; `bHeadStuckInCollision` (bit 21 de 0x470) y `bHitSteepSlope` (bit 5) se activan juntos con `0x200020`.

Puntos **no** contrastados por ejecución (riesgos): (a) no se compiló nada: puede haber errores de tipos/ámbito (p. ej. nombres de miembros como `m_nFlags` de CWeaponInfo, `m_damageResponse` de CEventDamage, `lighting.value`); (b) el orden de evaluación x87 se reprodujo con float de 24 bits expresión a expresión; no hay `_ftol2` ni redondeos especiales, pero el comportamiento de NaN/inf sólo se razonó (comparaciones); (c) `lineCPs` se inicializa a cero en C++ (en el EXE es basura de pila): sólo se leen tras `maxTouch < 1.0`, que implica que ProcessColModels las escribió; (d) el 8.º argumento de ProcessColModels es un dword con basura en el EXE (sólo se escribe el byte bajo) y la DLL pasa un bool; (e) los flags se escriben con campos de bits (RMW de otro ancho que el dword del EXE, mismo resultado salvo carreras); (f) `CVector::Normalise` (0x59C910), `SetHeading` (0x43E0C0), `GetHeading` (0x441DB0), `CMatrix::SetRotateZOnly` (0x59B020), `GetLocalDirection` (0x5DEF60), `RpAnimBlendClumpGetAssociation` (0x4D68B0), `AddCollisionRecord` (0x543490), `RegisterReference` (0x571B70), `IsSteepSlope`/`IsSoftLanding`, `AllocateMatrix`/`UpdateMatrix`, `ProcessColModels`, el constructor de CEventDamage y el calculador ya están en C++: se llaman en C++ (su cuerpo con el hook encendido no se ha comprobado aquí).

## Llamadores y registros de salida

Sólo llamadas virtuales `call [reg+0x58]` en CPhysical::ProcessCollision (0x546934, 0x54BD2D, 0x54C95F, 0x54CF19, 0x54D114): leen EAX (número de puntos); en las cinco ventanas de 24 bytes posteriores ECX/EDX se reescriben antes de leerse o no se leen. No hay llamadores directos (`residue_scan.py` sin resultados). El revisor debe confirmarlo a mano; el banco, si se construye, debe registrar EAX/ECX/EDX y exigir sólo EAX.

## Qué faltaría para el banco (diseño, no implementado)

- Entradas: ped falso (≥0x7A4 bytes con arena, colmodel/datos propios para el modelo, matriz propia, flags 0x1C/0x40/0x46C/0x470/0x474/0x478, velocidades, field_588, tipo de ped, estado), entidad falsa (tipos 0..5, vehículo barco/tren con 0x590/0x594, física con 0x40, velocidad), `colPoints[32]` previos con guardas, cámara activa/modo y tarea de arma.
- Colaboradores a sustituir por falsos idénticos en ambos lados (parche de entrada en la dirección del EXE **y** en el cuerpo de la DLL con hook encendido): ProcessColModels 0x4185C0 (devuelve puntos y escribe líneas/maxTouch del caso), GetColModel 0x535300, GetTaskUseGun 0x600F70, GetHeading/SetHeading 0x441DB0/0x43E0C0, AllocateMatrix/UpdateMatrix 0x54F560/0x54F1B0, RegisterReference 0x571B70, IsSteepSlope/IsSoftLanding 0x55E770/0x55E690, RpAnimBlendClumpGetAssociation 0x4D68B0, GetLocalDirection 0x5DEF60, CEventDamage 0x4AD830 / AffectsPed 0x4B35A0, calculador 0x4AD3F0 / 0x4B5AC0, CEventGroup::Add 0x4AB420, AddCollisionRecord 0x543490, Normalise 0x59C910, SetRotateZOnly 0x59B020, CColPoint::operator= 0x40FC80 (real).
- Compuertas: ABI estándar (`ret 8`), x87 normalizado a 24 bits, memoria completa del ped/entidad/colpoints/colmodel/líneas con guardas, trazas de colaboradores, estado global restaurado (colmodel temporal 0x968E20 y datos de 0x968E4C, matrices).
- Fixtures ≥4096 con: combinaciones de flags, ramas de suelo/barco/techo/pared, NaN/inf en normales y velocidades, ids de superficie 0..255, piezas 0..8, 0..32 puntos, cámara en modo 0x35 y no.
- Plantillas: ModifySpeed27 (Gate, x87, Watch), FileLoaderLoad2dEffect27 (falsos con parche en EXE y DLL), CTaskSimpleJetPack (hooks reversibles).

## Comandos

Ninguno todavía. Para aplicar la fuente: `git apply reversing/PedEntityCollision60/source.patch` (o usar el worktree D:\Original\wc-4). Después compilar y, si se construye el banco, correrlo en serie.
