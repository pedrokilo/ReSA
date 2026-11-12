#pragma once

/**
 * Clase de red (Winsock) del EXE sin nombre conocido: una base abstracta (vtable 0x866DB0, cinco ranuras) y una clase derivada (vtable 0x866DC4) cuyos metodos
 * (0x59CC70..0x59CD10) abren un socket TCP y se conectan con las funciones de WS2_32. Codigo muerto del juego (sin llamadores en el EXE salvo las vtables).
 * Nombre provisional: de momento solo se reconstruyen el cuerpo del destructor de la base y el destructor con borrado de la derivada (lote 178A, sin validar).
 */
class CWinsockLink {
public:
    static void InjectHooks();
};
