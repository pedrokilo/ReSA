# Pruebas y evidencias de reconstrucción

Esta carpeta sí se utiliza. Ejecutar `PROBAR.cmd` desde la raíz para elegir una prueba; `PROBAR.cmd --list` muestra los casos disponibles. No se necesita un CMD por función.

- `run.py` y `cases.json`: menú y catálogo común.
- `CVehicle`, `C_PcSave`, `CPlayerPed`, `CPed`, `CWeaponEffects`: pruebas repetibles y evidencia de las funciones trabajadas.
- `RpAnimBlend`: actualización de animaciones, validada mediante 652 comparaciones dentro del motor.
- `runtime_runner.py`: ejecución, restauración y registro compartidos; utilizado inicialmente por RpAnimBlend.
- `MEJORAS_DEL_PROCESO.md`: método de trabajo y criterios de validación.

La caché local `CVehicle/conan-cache` sigue referenciada por CMake y las dependencias. No es una carpeta abandonada y no se debe borrar mientras el build dependa de ella. Los binarios, registros extensos y cachés quedan fuera del commit mediante las reglas de Git existentes.

Se eliminaron los siete lanzadores específicos de la raíz y tres copias temporales de fuentes ya restauradas. No se eliminaron informes de pruebas ni código necesario para repetirlas.
