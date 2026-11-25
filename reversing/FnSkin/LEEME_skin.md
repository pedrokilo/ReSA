# Vigilancia del fallo "la textura de CJ se expande" (rama fix-skin-expansion)

Herramientas (todas SIN efecto sobre el juego salvo que se activen):

* **En el juego** (`source/game_sa/RenderWare/rw/SkinVigilancia.*`): `GTASA_SKIN_VIGILANCIA=1` (el exe `gta_sa_standalone_vigila.exe` la trae activa por defecto; `=0` la apaga).
  Registra en `logs/skin.log` (tope 6 MB): anomalias de matrices de hueso antes de subirlas al shader (CJ y peatones), de la matriz local de cada nodo de la jerarquia
  (`RpHAnimHierarchyUpdateMatrices`, con el keyframe interpolado crudo y su puntero), de la entidad y los frames de cada vehiculo (`CVehicle::PreRender`), y un LATIDO de 1 s
  con posicion, vehiculo, velocidad, giro, volante, inclinacion e ids de animacion de CJ. `GTASA_SKIN_VIGILANCIA_PRUEBA=1` inyecta un NaN a proposito para probar la deteccion.
  Capturas del backbuffer: `GTASA_SKIN_CAPTURAS=1` (EXPERIMENTAL, apagado: con MSAA 4x la resolucion en proceso no esta verificada).
* **Grabador/reproductor de acciones** (`source/game_sa/AccionesGrabador.*`, enganchado en `CPad::UpdatePads`, funciona en standalone y en la ASI): `GTASA_ACCIONES=1` graba
  `logs/acciones.jsonl` (solo cambios de botones/sticks/raton con tiempos de juego y de pared + contexto cada segundo); `GTASA_REPLAY=<archivo>` lo reproduce
  (`GTASA_REPLAY_SPEED=<factor>`, `GTASA_REPLAY_SEMILLA=0` para no restaurar la semilla). Con la vigilancia, cada anomalia guarda `logs/acciones_evento_NN.jsonl` (ultimos ~30 s).
* **Fuera del juego** (Python, sin tocar el juego): `vigilante_skin.py` (dejarlo abierto mientras se juega: lee matrices de hueso del bufer de skin y de la jerarquia de CJ,
  captura la ventana a ~100 fps en anillo de 3 s y guarda 3 s antes/despues de cada evento), `sonda_skin.py` + `ejecutar_skin.ps1` (pruebas guiadas: bici/moto/coche/pie/ruta),
  `skinwatch.py` (biblioteca comun).

Notas: el exe de vigilancia se compila con `SkinVigilanciaActivaPorDefecto.h` (archivo local, NO versionado) que define `GTASA_SKIN_VIGILANCIA_POR_DEFECTO`.
