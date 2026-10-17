// ============================================================================================================================================================
// CVehicle::FlyingControl (0x6D85F0, 10484 bytes). Reconstruccion en C++ con la maquina x87 de software FlcX87 (X87Math.h/.cpp): el EXE ejecuta el calculo en el x87 a 24 bits de
// precision y deja valores en los registros muertos, banderas acumuladas y codigos de condicion; FlcX87 reproduce cada instruccion con aritmetica entera y FlcX87::Commit() deja ese
// estado final en el hardware. Las secuencias que el EXE toma de funciones pequenas (operadores de CVector y CMatrix, _CIpow, _CIasin, _CIacos, __ftol2) estan en FlcVec* / FlcMat* /
// FlcCrt* (X87Math.cpp). Los colaboradores (CPhysical::ApplyTurnForce / ApplyMoveForce / GetSpeed, CPad, HeightAboveCeiling, GetColModel) son llamadas ordinarias.
//
// Rutas (flightModel, argumento 1; el EXE las elige con la tabla de bytes 0x6DAEF4 y la tabla de saltos 0x6DAEE4):
//   0           -> FlcRouteAirship   (0x6D8715..0x6D8B18)  [el juego no la usa]
//   1, 3, 4, 5  -> FlcRouteWing      (0x6D8B1D..0x6D986E)  1 = Baron/RC, 3 = avion y coche, 4 = sin uso, 5 = bote
//   2, 6, 8     -> FlcRouteRotor     (0x6D986E..0x6DA900)  6 = helicoptero e Hydra, 2 y 8 sin uso
//   7, > 8      -> solo prologo y cola
//   (todas)     -> cola comun FlcTail (0x6DA900..0x6DAED7)
// ============================================================================================================================================================
