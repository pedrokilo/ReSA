/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Matrix.h"

class CPlaceable;

/**
 * Matriz transformable que además actúa como nodo de una lista doblemente enlazada.
 *
 * El juego mantiene estas matrices en pools y mueve los mismos objetos entre listas
 * de uso y de libres. m_pOwner relaciona la transformación con el CPlaceable que la
 * está utilizando; m_pPrev/m_pNext implementan el enlace intrusivo.
 */
class CMatrixLink : public CMatrix {
public:
    // Como el original (0x54F0C0): sin inicialización por valor de CMatrix; solo los miembros con inicializador
    CMatrixLink() {}
    CMatrixLink(float fScale) : CMatrix() { SetScale(fScale); }

public:
    CPlaceable*  m_pOwner; //!< Objeto que utiliza esta matriz; no implica ownership de CPlaceable.
    CMatrixLink* m_pPrev;  //!< Nodo anterior dentro de la lista intrusiva actual.
    CMatrixLink* m_pNext;  //!< Nodo siguiente dentro de la lista intrusiva actual.

public:
    static void InjectHooks();

    // Inserta otro nodo inmediatamente después de éste dentro de la lista actual.
    inline void Insert(CMatrixLink* pWhere);

    // Separa este nodo de la lista; la memoria continúa perteneciendo al pool.
    inline void Remove();
};

VALIDATE_SIZE(CMatrixLink, 0x54);
