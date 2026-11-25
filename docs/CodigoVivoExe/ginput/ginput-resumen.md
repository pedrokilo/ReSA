# GInputSA 1.11 (Silent): que modifica en el juego (resumen)

Catalogo completo en `ginput-modificaciones.csv` (171 filas), `ginput-direcciones.csv` (las 344 direcciones clasificadas), `ginput-estructuras.md`, `ginput-tablas.md`. Herramientas: `tools/` (Python con capstone). Analisis estatico, sin ejecutar nada.

## Mecanismo general
- DllMain (0x1000D4A0): lee `GInputSA.ini` (clave Enable) y comprueba la version del EXE; instala parches solo en 1.0 US/EU. Todo el parcheo estatico esta en una funcion (0x1000A740, ~200 regiones con VirtualProtect). No hay comprobacion de hash ni parches por version mas alla del tipo 0.
- Tipos de modificacion: 56 CALL reescritos (rel32), 44 JMP en la entrada de funciones del juego, 15 parches con CALL/JMP inyectado, 21 reescrituras de bytes, 7 NOP, 20 operandos redirigidos a variables propias, 1 parche en caliente, 1 hook de API (DefWindowProcA via operando de IAT), 1 escritura de datos del menu, 1 parche de la DLL xinput1_3 de SCP. No toca vtables del juego ni la IAT del EXE.
- Los originales se llaman por punteros guardados, resueltos leyendo operandos de instrucciones del EXE (`CPad::Pads`, `TheText`, `FrontEndMenuManager`...).

## Por caracteristica (filas del CSV)
- Lectura del mando: se reemplazan UpdatePads (CALL a ProcessPad/UpdateMouse/Update), se anula `CControllerState::Clear` en CPad::Update y `RsInputDeviceAttach`; XInputGetState/SetState (ordinales 2 y 3; NO usa el ordinal 100). Lectura en 0x10007550.
- Mapeo y Setups: ~113 filas. Cada getter de CPad (acelerar, freno, mirar, objetivo, arma, camara, radio, conversacion...) se sustituye y bifurca por CPad::Mode (0 = Setup 1, 1 = Setup 2). Southpaw/ejes/sensibilidad/zona muerta por pad. Detalle en tablas, seccion 2.
- Trucos con el mando: 1 hook (CGame::Process+0xD8 -> 0x10004950); convierte botones a letras, hash CRC32 y tabla de 90 hashes; avisa con GXT CHEAT1/CHEAT8.
- Iconos y GXT: CText::Get encadenado + tabla de 124 correcciones regex; CFont::ParseToken (3 CALL + GetStringWidth/ProcessCurrentString); GetGxtStringOfCommandKeys; CTxdStore/CSprite2d para los TXD de botones; PrintChar usa tabla/escala propias.
- Cambio teclado/mando: ConfigPad::SetActive (+0x7D), LoadSettings fuerza ControlMethod=0, 10 lecturas de m_bUseMouse3rdPerson redirigidas, comando 0xA4B invertido.
- Scripts: GetPadState por nombre de script (codigos 1-7), comandos 0x390/0x494/0x293 y dos del rango 1900-1999 (dance, lowgame, bdisply). No hay tabla de bytes SCM: lo anterior es codigo.
- Co-op: ReconcileTwoControllersInput sustituido (0x10002450); MapPadOneToPadTwo en UpdatePads/StopShaking.
- Vibracion: StopShaking/StopPadsShaking -> XInputSetState; INI Vibration -> FrontEndMenuManager+0x20; constructor ya no lo fuerza.
- Foco: unico hook de API, DefWindowProcA (WM_SETFOCUS/KILLFOCUS). Steam/Guide: solo en 0x10002450 y la INI (GuideLaunchesOverlay); strings de SteamClient/overlay sin analizar a fondo.
- SIXAXIS/SCP: direccion por giroscopio en GetSteering*, parche de la DLL de SCP (0x1000A600), opciones Sixaxis*.
- Menus: Start/Triangle/Circle cierran menu, parches Old/New conmutables (0x1000D2A0), PrintMap/AdditionalOptionInput, pantalla de controles sin texto original.

## Honestidad: que no se entendio del todo
- "Confirmado" = bytes parcheados verificados contra gta_sa.exe 1.0 US y reemplazo leido; "parcial" (63 filas) = campos y ramas por Setup extraidos automaticamente, cuerpo no leido linea a linea (descripcion fiable, detalle fino no). Solo 4 filas "por contexto".
- Sin decompilador disponible (REA no soporta x86 32 bits aqui): todo con capstone.
- Pendientes: cuerpo de 0x10007550 (lectura XInput, curvas, vibracion: solo estructura), curva exacta de sensibilidad, switch de GetPadState (0x1000609C), 0x10008E80 (tokens ~@NOMBRE~), motor regex de 0x10009680 y la rutina de Steam/Guide, significado exacto de varios offsets de CPed y de las escrituras del menu (+0x1FD2...), parches internos de GetHandBrake (probable codigo muerto).
- Falso positivo posible: algunos campos "TempMouse" son de otras estructuras.

## Orden de integracion recomendado
1. Trucos del mando (1 hook, autocontenido) y tabla GXT + CText::Get + ParseToken/iconos (ya en curso).
2. Getters de CPad por Setup 1 y luego Setup 2 (tabla seccion 2), con Southpaw/ejes.
3. Cambio teclado/mando (SetActive, 0xB6EC2E) y menus (cierre con Start/Triangulo, parches Old/New).
4. Lectura XInput con zonas muertas y curvas, vibracion, co-op y foco.
5. Scripts (GetPadState, dance/lowgame/bdisply).
6. Fuera de alcance: SIXAXIS/SCP, Steam/Guide.
