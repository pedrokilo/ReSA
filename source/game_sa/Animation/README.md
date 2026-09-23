# Estructuras de animación

## CAnimBlendSequence

Representa los fotogramas de animación de un solo hueso.

## CAnimBlendAssociation

Representa una animación que se está reproduciendo en un conjunto de objetos RenderWare (`clump`), normalmente un peatón.

Los datos de secuencias y fotogramas se copian de `CAnimBlendHierarchy` a `CAnimBlendAssociation` cuando el conjunto solicita una animación. La instancia de `CAnimBlendAssociation` se destruye cuando el peatón o conjunto deja de reproducirla. En cambio, la jerarquía permanece en memoria hasta que `CStreaming` descarga el IFP para liberar espacio.

Un conjunto puede tener una o varias asociaciones. Normalmente hay una animación principal, pero también existen animaciones parciales que se reproducen junto a ella, como los gestos de las manos o fumar.

Si una animación mueve 15 huesos, hay 15 instancias de `CAnimBlendSequence` y una instancia de `CAnimBlendHierarchy` que representa la animación completa.

## CAnimBlendHierarchy

Es el objeto de animación. Contiene secuencias `CAnimBlendSequence`, cada una correspondiente a un hueso o nodo.

Sus datos se copian a `CAnimBlendAssociation` cuando un conjunto solicita la animación. Permanece en memoria hasta que `CStreaming` descarga el IFP para liberar espacio.

## CAnimBlendStaticAssociation

Almacena datos estáticos de animación cargados desde un conjunto de animación.

## CAnimBlendAssocGroup

Representa un grupo o bloque, como `ped` o `int_house`. También relaciona los valores `AnimationId` y los nombres de animación con sus datos.
