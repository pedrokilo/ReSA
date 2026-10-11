# Segunda conversión C++ de CarCtrl

`CCarCtrl::ClearInterestingVehicleList`, entrada original `0x423F00`, se convierte a dos escrituras `volatile` de punteros nulos en `0x969084` y `0x969088`. El EXE ejecuta XOR EAX, dos MOV y RET; conserva ECX y EDX y no modifica los tiempos.

Dos intentos con el arreglo global fallaron las 256 pruebas de preservación de registros aunque la memoria coincidía. Se rechazaron las corridas `20261010-231117-853952` y `20261010-231333-521897`. El objeto generado usaba ECX y EDX para acceder al arreglo; las escrituras absolutas evitan esos accesos. No se retiró ninguna comprobación del banco.

La corrida final `20261010-231937-429823` pasó las tres funciones y 4.352 pares: ClearInterestingVehicleList 256, RegisterVehicleOfInterest 2.048 y FindSequenceElement 2.048. Cero diferencias, ningún original/candidate nulo, rutas verificadas, estado restaurado, partidas intactas y configuración restaurada. La auditoría y los resultados comprimidos están en `runs/20261010-231937-429823/`.

La preservación de registros queda demostrada para la compilación probada. C++ no garantiza por sí solo conservar registros volátiles; cambios de compilador o configuración requieren repetir el banco. Esta revalidación no añade funciones a la cohorte de dependencias.
