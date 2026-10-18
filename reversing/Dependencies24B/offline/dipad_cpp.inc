// 0x7485C0 - _InputInitialiseJoys: borra los dos registros de pad, enumera con 0x7477C0 y, para cada pad presente, lee VID/PID.
//
// Reconstruccion exacta del EXE (sin ensamblador). Detalles que el EXE tiene y que se conservan:
//  - Solo se borran wDeviceID y los tres indicadores de cada pad: wVendorID/wProductID (y el relleno del byte +7) NO se tocan.
//  - La enumeracion es el 0x7477C0 del EXE (EnumDevices con 0x747020 y diPadSetRanges 0x746D80); su resultado se ignora.
//  - GetCapabilities se llama con dwSize = 0x2C y su resultado se ignora; ControlsManager.InitDefaultControlConfigJoyPad recibe
//    la constante 0x20, no dwButtons.
//  - HRESULT de GetProperty ignorado: si falla (o no escribe dwData) el EXE guarda los restos de pila que habia en el DIPROPDWORD,
//    y el pad 2 reutiliza el mismo DIPROPDWORD que el pad 1 (cabecera y dwData); si no hay pad 1, su cabecera tambien es resto de
//    pila. Esos cinco dwords estan en [R-0x40, R-0x2C) siendo R la direccion de retorno del EXE: se copian al entrar, antes de que
//    el marco de la DLL pueda pisarlos (diPadInit va sin comprobaciones de ejecucion y con un marco de pocos bytes).
//  - psGlobal se vuelve a leer exactamente donde lo hace el EXE (tras GetCapabilities y tras InitDefaultControlConfigJoyPad); el
//    puntero del pad 1 se vuelve a leer antes de cada llamada.
//  - Ningun llamador lee EAX/ECX/EDX al volver (0x748813 los sobrescribe), por eso no se reproduce el residuo de registros.
static_assert(sizeof(DIDEVCAPS) == 0x2C && sizeof(DIPROPDWORD) == 0x14 && sizeof(DIPROPHEADER) == 0x10);
static inline uint32 s_diPadEntryResidue[5]; // dwSize, dwHeaderSize, dwObj, dwHow, dwData

__declspec(noinline) static void diPadInitBody() {
    AllValidWinJoys.ClearJoyInfo(0);
    AllValidWinJoys.ClearJoyInfo(1);

    plugin::Call<0x7477C0>();

    DIPROPDWORD prop;
    prop.diph.dwSize       = s_diPadEntryResidue[0];
    prop.diph.dwHeaderSize = s_diPadEntryResidue[1];
    prop.diph.dwObj        = s_diPadEntryResidue[2];
    prop.diph.dwHow        = s_diPadEntryResidue[3];
    prop.dwData            = s_diPadEntryResidue[4];

    psGlobalType* ps = RsGlobal.ps;
    if (ps->diDevice1) {
        AllValidWinJoys.JoyStickNum[0].bJoyAttachedToPort = true;

        DIDEVCAPS caps;
        caps.dwSize = sizeof(DIDEVCAPS);
        ps->diDevice1->GetCapabilities(&caps);

        ps = RsGlobal.ps;
        prop.diph.dwSize       = sizeof(DIPROPDWORD);
        prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        prop.diph.dwObj        = 0;
        prop.diph.dwHow        = DIPH_DEVICE;
        ps->diDevice1->GetProperty(DIPROP_VIDPID, &prop.diph);

        AllValidWinJoys.JoyStickNum[0].wVendorID  = LOWORD(prop.dwData);
        AllValidWinJoys.JoyStickNum[0].wProductID = HIWORD(prop.dwData);

        ControlsManager.InitDefaultControlConfigJoyPad(0x20);

        ps = RsGlobal.ps;
    }

    if (ps->diDevice2) {
        ps->diDevice2->GetProperty(DIPROP_VIDPID, &prop.diph);

        AllValidWinJoys.JoyStickNum[1].wVendorID  = LOWORD(prop.dwData);
        AllValidWinJoys.JoyStickNum[1].wProductID = HIWORD(prop.dwData);
        AllValidWinJoys.JoyStickNum[1].bJoyAttachedToPort = true;
    }
}

#pragma runtime_checks("", off)
void diPadInit() {
    // Debe ser lo primero: lee el residuo de pila del llamador del EXE (ver arriba) antes de que nada reutilice esa zona.
    const auto* r = reinterpret_cast<const uint32*>(static_cast<const uint8*>(_AddressOfReturnAddress()) - 0x40);
    s_diPadEntryResidue[0] = r[0];
    s_diPadEntryResidue[1] = r[1];
    s_diPadEntryResidue[2] = r[2];
    s_diPadEntryResidue[3] = r[3];
    s_diPadEntryResidue[4] = r[4];
    diPadInitBody();
}
#pragma runtime_checks("", restore)
