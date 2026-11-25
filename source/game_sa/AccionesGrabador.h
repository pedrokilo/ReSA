#pragma once
// Grabador y reproductor de ACCIONES del jugador a nivel de EVENTOS (no por fotograma): registra solo cambios (botones, sticks con umbral/histeresis a <= 20 Hz, raton) con la marca
// del reloj de juego y del de pared, mas un contexto por segundo (posicion, velocidad, rumbo, vehiculo, animaciones, camara). Funciona en el standalone y en la ASI sobre el EXE original
// porque se engancha en CPad::UpdatePads (codigo del proyecto), no en el exe.
// Activacion (SIN efecto por defecto): GTASA_ACCIONES=1 graba logs/acciones.jsonl (0 la apaga siempre; el exe de vigilancia, compilado con SkinVigilanciaActivaPorDefecto.h, la tiene
// activa); GTASA_REPLAY=<archivo.jsonl> reproduce un archivo (GTASA_REPLAY_SPEED=<factor>, 1 por defecto: acelera o frena el guion; GTASA_REPLAY_SEMILLA=0 no restaura la semilla de rand).
// Formato JSONL: primera linea {"k":"cab","ver":1,...}; despues {"k":"pad","t":juego_ms,"w":pared_ms,"f":fotograma,"n":"<campo de CControllerState>","v":valor},
// {"k":"raton","t":..,"dx":..,"dy":..,"b":mascara}, {"k":"ctx",...}, {"k":"accion","t":..,"n":"subir_vehiculo|bajar_vehiculo|arma|acelerar|frenar|girar|saltar|esprintar",...}.
void AccionesGrabadorFotograma();   // al final de CPad::UpdatePads
void AccionesGrabadorVolcarEvento();// copia los ultimos ~30 s a logs/acciones_evento_NN.jsonl (lo llama la vigilancia al detectar una anomalia)
