#include "StdInc.h"

#include "RunningScript.h"

// Rama vivo-scra (sin validar): despachador original de los comandos de script 0..99 (ranura 0 de s_OriginalCommandHandlerTable, 0x465E60). thiscall con el numero de comando en la pila (`ret 4`)
// y AL = 0 (OR_CONTINUE) o 0xFF (OR_INTERRUPT: comando sin caso, destino 0x466C3A). Tabla de saltos 0x466C50 indexada por el comando (sin tabla de bytes); el EXE la envuelve en un marco SEH que
// aqui no hace falta. Como en RunningScriptCommands300To499.cpp, solo llevan caso los comandos que el EXE resuelve aqui y que NO tienen manejador propio en el C++ (docs/ComandosScript.csv, estado
// original o no_implementado con caso en la tabla de saltos). En esta centena solo queda COMMAND_LINE; todo lo demas va a ProcessHandledCommand (manejador propio, o OR_INTERRUPT como el EXE).
OpcodeResult CRunningScript::ProcessCommands0To99(int32 command) {
    switch (command) {
    case COMMAND_LINE: // 82, 0x4668BE: lee y descarta sus seis parametros (CollectParameters 0x464080) y devuelve 0
        CollectParameters(6);
        return OR_CONTINUE;
    default:
        return ProcessHandledCommand(command);
    }
}
