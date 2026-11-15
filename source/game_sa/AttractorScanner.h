/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskTimer.h"

class C2dEffect;
struct C2dEffectPedAttractor;
class CPed;
class CEntity;

class CAttractorScanner {
public:
    char       field_0;
    char       _pad[3];
    CTaskTimer field_4;
    C2dEffect* m_pEffectInUse;
    int32      field_14;
    int32      field_18[10];
    int32      field_40[10];
    int32      field_68[10];

    static void InjectHooks();
    /**
     * Pone a cero los dos primeros vectores y deja las distancias en 25 (las posiciones 4 y 7, al cuadrado de 0x86C52C).
     * Devuelve EDX:EAX tal como los deja el EXE (0x5FFF90), que sus llamadores y el banco comprueban: EAX = &field_68[10]
     * y EDX = 10. Los llamadores en C++ pueden ignorar el valor.
     */
    uint64 Clear();

    /**
     * 0x6060A0: busca atractores (efectos 2D) alrededor del peaton; sin efecto util, en vehiculo o con la tarea de usar un efecto no hace nada.
     * Los exploradores de sectores y de efectos, y el resto de colaboradores, siguen por direccion del EXE.
     */
    void ScanForAttractors(CPed& ped);

    /**
     * 0x6034B0 (lote 198A, sin validar): recorre una lista de punteros de un sector (los dos primeros dwords de cada nodo son el elemento y el siguiente: vale para las listas de enlace simple de edificios y las de doble enlace de objetos) y pasa a
     * 0x5FFFD0 (por direccion del EXE) cada efecto 2D de tipo atractor de los modelos de sus entidades. Cada llamada es a su propia lista: `list` apunta al objeto de la lista (primer dword = primer nodo).
     */
    void ScanEntityList(const void* list, CPed* ped);

    /**
     * 0x603570 (lote 198A, sin validar; cdecl, un argumento de pila, SEH): devuelve el peaton mas cercano (distancia al cuadrado estrictamente menor, empezando en FLT_MAX) que pueda usar el efecto de script `effect`: no esta usando ya un efecto
     * (tarea activa distinta de TASK_COMPLEX_USE_EFFECT), el efecto admite su modelo o su tipo de peaton (lista de usuarios de CScripted2dEffects) y CPedAttractorManager::IsApproachable lo acepta. Nulo si ninguno.
     */
    static CPed* FindClosestPedForEffect(C2dEffectPedAttractor* effect);

    /**
     * 0x5FFFD0 (lote 202A, sin validar; thiscall, tres argumentos de pila, `ret 0Ch`, 429 bytes con SEH que solo destruye la matriz local): evalua un efecto 2D de atractor (de una entidad, o de script con entidad nula) como candidato para el peaton: segun la lluvia
     * (CWeather::Rain frente a 0.2) solo vale el tipo 4 (refugio) con lluvia y cualquier otro sin ella; la distancia al cuadrado entre el peaton y la posicion del efecto en el mundo debe ser estrictamente menor que la guardada para su tipo (field_68[tipo]); los
     * efectos de script (tipo 7) ademas respetan su radio al cuadrado (CScripted2dEffects::ms_radii; radio negativo = sin limite); y CPedAttractorManager::HasEmptySlot e IsApproachable lo aceptan. Si lo aceptan guarda entidad, distancia y efecto en field_18, field_68 y field_40
     * (indice = tipo, sin comprobar el rango).
     */
    void ScanEffect(C2dEffectPedAttractor* effect, CEntity* entity, CPed* ped);

    /**
     * 0x600180 (lote 202A, sin validar; thiscall, dos argumentos de pila, `ret 8`, 364 bytes): deja en *effect y *entity el mejor candidato guardado (ambos a nulo si no hay): si hay entidad guardada para el tipo 4 (refugio) gana esa; si no, el de menor distancia
     * al cuadrado estricta (empezando en FLT_MAX) entre los diez tipos que tengan efecto no nulo, recorridos del 0 al 9 (los empates los gana el primero).
     */
    void GetBestEffect(C2dEffectPedAttractor** effect, CEntity** entity);
};

VALIDATE_SIZE(CAttractorScanner, 0x90);
