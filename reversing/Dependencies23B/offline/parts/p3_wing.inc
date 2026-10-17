void FlcRouteWing(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    alignas(16) uint8 stk[0xB00] = {};                      // pila x86 del EXE (el marco [F+x] empieza en stk + 0x800)
    uint32 eax = 0, ecx = 0, edx = 0, ebx = (uint32)(uintptr_t)f.pad, esi = (uint32)(uintptr_t)f.v, edi = 0, ebp = 0;
    uint32 esp = (uint32)(uintptr_t)stk + 0x800;            // F
    uint32 fA = 0, fB = 0, fR = 0;
    M32(esp + 0x264) = (uint32)f.model; memcpy(stk + 0x800 + 0x268, &f.skid, 4); memcpy(stk + 0x800 + 0x26C, &f.upDown, 4);
    memcpy(stk + 0x800 + 0x270, &f.sideways, 4); memcpy(stk + 0x800 + 0x274, &f.accel, 4);
    M32(esp + 0x14) = ebx;                                  // mando del conductor
    memcpy(stk + 0x800 + 0x40, f.vel, 12);                  // velocidad + deriva del viento
    memcpy(stk + 0x800 + 0x50, f.com, 12);                  // centro de masas en el mundo
    M32(esp + 0x4C) = (uint32)(uintptr_t)&v.m_vecMoveSpeed;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8b1d: fld dword ptr [esp + 0x268]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d8b24: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8b2a: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d8b2c: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d8b5f;   // 6d8b2f: jp 0x6d8b5f
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8b31: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0x0u);   // 6d8b33: mov dword ptr [esp + 0x268], 0
    if (fR == 0) goto L_6d8b5f;   // 6d8b3e: je 0x6d8b5f
    ecx = ebx;   // 6d8b40: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringLeftRight();   // 6d8b42: call 0x53fb80
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8b47: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6d8b4a: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8b4e: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d8b52: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8b58: fstp dword ptr [esp + 0x268]
L_6d8b5f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8b5f: fld dword ptr [esp + 0x26c]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d8b66: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8b6c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d8b6e: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d8be3;   // 6d8b71: jp 0x6d8be3
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8b73: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x26c) = (uint32)(0x0u);   // 6d8b75: mov dword ptr [esp + 0x26c], 0
    if (fR == 0) goto L_6d8be3;   // 6d8b80: je 0x6d8be3
    ecx = ebx;   // 6d8b82: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringUpDown();   // 6d8b84: call 0x53fbd0
    eax = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8b89: movsx eax, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6d8b8c: mov dword ptr [esp + 0x14], eax
    ecx = ebx;   // 6d8b90: mov ecx, ebx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8b92: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d8b96: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8b9c: fstp dword ptr [esp + 0x26c]
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d8ba3: call 0x53fc10
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8ba8: movsx ecx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6d8bab: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8baf: fild dword ptr [esp + 0x14]
    fp.Fabs();   // 6d8bb3: fabs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d8bb5: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8bbb: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8bbd: test ah, 0x41
    if (fR != 0) goto L_6d8be3;   // 6d8bc0: jne 0x6d8be3
    ecx = ebx;   // 6d8bc2: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d8bc4: call 0x53fc10
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8bc9: movsx edx, ax
    fR = (uint32)(0 - edx); edx = fR;   // 6d8bcc: neg edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6d8bce: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8bd2: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d8bd6: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8bdc: fstp dword ptr [esp + 0x26c]
L_6d8be3:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8be3: fld dword ptr [esp + 0x26c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8bea: fld dword ptr [esp + 0x268]
    fp.Fpatan();   // 6d8bf1: fpatan 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8bf3: fstp dword ptr [esp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858624)));   // 6d8bf7: fld dword ptr [0x858624]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8bfd: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf8)), true);   // 6d8c01: fcomp dword ptr [0x86daf8]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c07: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c09: test ah, 0x41
    if (fR != 0) goto L_6d8c2a;   // 6d8c0c: jne 0x6d8c2a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c0e: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x859ab0)), true);   // 6d8c12: fcomp dword ptr [0x859ab0]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c18: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c1a: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d8c2a;   // 6d8c1d: jp 0x6d8c2a
    fp.FstpSt(0);   // 6d8c1f: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c21: fld dword ptr [esp + 0x10]
    goto L_6d8cb5;   // 6d8c25: jmp 0x6d8cb5
L_6d8c2a:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c2a: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x859ab0)), true);   // 6d8c2e: fcomp dword ptr [0x859ab0]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c34: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c36: test ah, 0x41
    if (fR != 0) goto L_6d8c5a;   // 6d8c39: jne 0x6d8c5a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c3b: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf4)), true);   // 6d8c3f: fcomp dword ptr [0x86daf4]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c45: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c47: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d8c5a;   // 6d8c4a: jp 0x6d8c5a
    fp.FstpSt(0);   // 6d8c4c: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c4e: fld dword ptr [esp + 0x10]
    fp.Fsub((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 - m   // 6d8c52: fsub dword ptr [0x858fe4]
    goto L_6d8cb5;   // 6d8c58: jmp 0x6d8cb5
L_6d8c5a:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c5a: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf4)), true);   // 6d8c5e: fcomp dword ptr [0x86daf4]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c64: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c66: test ah, 0x41
    if (fR != 0) goto L_6d8c79;   // 6d8c69: jne 0x6d8c79
    fp.FstpSt(0);   // 6d8c6b: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c6d: fld dword ptr [esp + 0x10]
    fp.Fsub((*(float*)(uintptr_t)(uint32)(0x858cb8)));   // ST0 = ST0 - m   // 6d8c71: fsub dword ptr [0x858cb8]
    goto L_6d8cb5;   // 6d8c77: jmp 0x6d8cb5
L_6d8c79:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c79: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf0)), true);   // 6d8c7d: fcomp dword ptr [0x86daf0]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c83: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c85: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d8c98;   // 6d8c88: jp 0x6d8c98
    fp.FstpSt(0);   // 6d8c8a: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c8c: fld dword ptr [esp + 0x10]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x858cb8)));   // ST0 = ST0 + m   // 6d8c90: fadd dword ptr [0x858cb8]
    goto L_6d8cb5;   // 6d8c96: jmp 0x6d8cb5
L_6d8c98:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c98: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf8)), true);   // 6d8c9c: fcomp dword ptr [0x86daf8]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8ca2: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8ca4: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8cbd;   // 6d8ca7: jp 0x6d8cbd
    fp.FstpSt(0);   // 6d8ca9: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8cab: fld dword ptr [esp + 0x10]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 + m   // 6d8caf: fadd dword ptr [0x858fe4]
L_6d8cb5:;
    fp.Fcos();   // 6d8cb5: fcos 
    fp.Fdivr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m / ST0   // 6d8cb7: fdivr dword ptr [0x858624]
L_6d8cbd:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8cbd: fld dword ptr [esp + 0x268]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d8cc4: mov eax, dword ptr [esi + 0x14]
    fp.Arith('*', 0, 0, 1, false);   // 6d8cc7: fmul st(1)
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d8cc9: add eax, 0x10
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d8ccc: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d8cce: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d8cd1: mov eax, dword ptr [eax + 8]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8cd4: fstp dword ptr [esp + 0x268]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(ecx);   // 6d8cdb: mov dword ptr [esp + 0x28], ecx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // ST0 = ST0 * m   // 6d8cdf: fmul dword ptr [esp + 0x26c]
    ecx = esi;   // 6d8ce6: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d8ce8: mov dword ptr [esp + 0x2c], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(eax);   // 6d8cec: mov dword ptr [esp + 0x30], eax
    fp.Fchs();   // 6d8cf0: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8cf2: fstp dword ptr [esp + 0x26c]
    eax = (uint32)(uintptr_t)v.GetColModel();   // 6d8cf9: call 0x535300
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 4)));   // 6d8cfe: fld dword ptr [eax + 4]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x264));   // 6d8d01: mov edi, dword ptr [esp + 0x264]
    fA = edi; fB = 0x1u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8d08: cmp edi, 1
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d8d0b: fld dword ptr [esp + 0x28]
    fp.Arith('*', 0, 0, 1, false);   // 6d8d0f: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d8d11: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d8d15: fld dword ptr [esp + 0x2c]
    fp.Arith('*', 0, 0, 1, false);   // 6d8d19: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d8d1b: fstp dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d8d1f: fld dword ptr [esp + 0x30]
    fp.Arith('*', 0, 0, 1, false);   // 6d8d23: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d8d25: fstp dword ptr [esp + 0x24]
    fp.FstpSt(0);   // 6d8d29: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d8d2b: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // ST0 = ST0 * m   // 6d8d2f: fmul dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d8d33: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // ST0 = ST0 * m   // 6d8d37: fmul dword ptr [esp + 0x40]
    fp.Arith('+', 1, 1, 0, true);   // 6d8d3b: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d8d3d: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // ST0 = ST0 * m   // 6d8d41: fmul dword ptr [esp + 0x48]
    fp.Arith('+', 1, 1, 0, true);   // 6d8d45: faddp st(1)
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8d47: fst dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // ST0 = ST0 * m   // 6d8d4b: fmul dword ptr [esp + 0x10]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d8d4f: fstp dword ptr [esp + 0x18]
    if (fR != 0) goto L_6d8db7;   // 6d8d53: jne 0x6d8db7
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d8d55: mov ecx, dword ptr [0xb7cb5c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d8d5b: mov edx, dword ptr [esi + 0x8c]
    esp -= 4; M32(esp) = ecx;   // 6d8d61: push ecx
    esp -= 4; M32(esp) = edx;   // 6d8d62: push edx
    eax = (uint32)(esp + 0x3c);   // 6d8d63: lea eax, [esp + 0x3c]
    esp -= 4; M32(esp) = eax;   // 6d8d67: push eax
    ecx = (uint32)(esp + 0x194);   // 6d8d68: lea ecx, [esp + 0x194]
    esp -= 4; M32(esp) = ecx;   // 6d8d6f: push ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x44) = (uint32)(0x0u);   // 6d8d70: mov dword ptr [esp + 0x44], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x48) = (uint32)(0x0u);   // 6d8d78: mov dword ptr [esp + 0x48], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x4c) = (uint32)(0x3B83126Fu);   // 6d8d80: mov dword ptr [esp + 0x4c], 0x3b83126f
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d8d88: call 0x40fec0
    esp += 0xCu;   // 6d8d8d: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d8d90: push eax
    edx = (uint32)(esp + 0xd0);   // 6d8d91: lea edx, [esp + 0xd0]
    esp -= 4; M32(esp) = edx;   // 6d8d98: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d8d99: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d8d9e: mov edx, dword ptr [eax]
    ecx = esp;   // 6d8da0: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d8da2: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d8da4: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d8da7: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d8daa: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d8dad: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d8db0: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d8db2: call 0x5429f0
L_6d8db7:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8db7: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d8dbe: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8dc4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d8dc6: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d8e05;   // 6d8dc9: jp 0x6d8e05
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8dcb: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x274) = (uint32)(0x0u);   // 6d8dcd: mov dword ptr [esp + 0x274], 0
    if (fR == 0) goto L_6d8e05;   // 6d8dd8: je 0x6d8e05
    ecx = ebx;   // 6d8dda: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetAccelerate();   // 6d8ddc: call 0x5403f0
    ecx = ebx;   // 6d8de1: mov ecx, ebx
    ebp = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8de3: movsx ebp, ax
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetBrake();   // 6d8de6: call 0x540080
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8deb: movsx ecx, ax
    fA = ebp; fB = ecx; fR = (fA - fB) & 0xFFFFFFFFu; ebp = fR;   // 6d8dee: sub ebp, ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ebp);   // 6d8df0: mov dword ptr [esp + 0x14], ebp
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8df4: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859a3c)));   // ST0 = ST0 * m   // 6d8df8: fmul dword ptr [0x859a3c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8dfe: fstp dword ptr [esp + 0x274]
L_6d8e05:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x590));   // 6d8e05: mov ecx, dword ptr [esi + 0x590]
    fA = ecx; fB = ecx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8e0b: test ecx, ecx
    eax = 0x3F800000u;   // 6d8e0d: mov eax, 0x3f800000
    if (fR != 0) goto L_6d8efe;   // 6d8e12: jne 0x6d8efe
    ebx = (ebx & 0xFFFF0000u) | (((uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22))) & 0xFFFFu);   // 6d8e18: mov bx, word ptr [esi + 0x22]
    fA = (ebx & 0xFFFFu); fB = 0x21Bu; fR = (fA - fB) & 0xFFFFu;   // 6d8e1c: cmp bx, 0x21b
    if (fR == 0) goto L_6d8e6f;   // 6d8e21: je 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7d4)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e23: cmp dword ptr [esi + 0x7d4], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e29: jne 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7d8)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e2b: cmp dword ptr [esi + 0x7d8], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e31: jne 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7dc)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e33: cmp dword ptr [esi + 0x7dc], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e39: jne 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7e0)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e3b: cmp dword ptr [esi + 0x7e0], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e41: jne 0x6d8e6f
    fA = (ebx & 0xFFFFu); fB = 0x1CCu; fR = (fA - fB) & 0xFFFFu;   // 6d8e43: cmp bx, 0x1cc
    if (fR != 0) goto L_6d8efe;   // 6d8e48: jne 0x6d8efe
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x40));   // 6d8e4e: mov eax, dword ptr [esi + 0x40]
    fA = ((eax >> 8) & 0xFFu); fB = 0x1u; fR = (fA & fB) & 0xFFu;   // 6d8e51: test ah, 1
    if (fR == 0) goto L_6d8efe;   // 6d8e54: je 0x6d8efe
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8e5a: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858cc4)), true);   // 6d8e5e: fcomp dword ptr [0x858cc4]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8e64: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8e66: test ah, 0x41
    if (fR == 0) goto L_6d8efe;   // 6d8e69: je 0x6d8efe
L_6d8e6f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8e6f: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8e76: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8e7c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8e7e: test ah, 0x41
    if (fR == 0) goto L_6d8efe;   // 6d8e81: je 0x6d8efe
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8e83: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8e8a: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8e90: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8e92: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8eee;   // 6d8e95: jp 0x6d8eee
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d8e97: mov edx, dword ptr [esp + 0x4c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d8e9b: mov eax, dword ptr [esi + 0x14]
    esp -= 4; M32(esp) = edx;   // 6d8e9e: push edx
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d8e9f: add eax, 0x10
    esp -= 4; M32(esp) = eax;   // 6d8ea2: push eax
    FlcVecDot(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4));   // 6d8ea3: call 0x40fdb0
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b38)), true);   // 6d8ea8: fcomp dword ptr [0x858b38]
    esp += 0x8u;   // 6d8eae: add esp, 8
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8eb1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8eb3: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8eee;   // 6d8eb6: jp 0x6d8eee
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8eb8: fld dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x871f88)));   // ST0 = ST0 * m   // 6d8ebc: fmul dword ptr [0x871f88]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // ST0 = m - ST0   // 6d8ec2: fsubr dword ptr [esp + 0x274]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8ec9: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d8ecf: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8ed1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8ed3: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8ee0;   // 6d8ed6: jp 0x6d8ee0
    fp.FstpSt(0);   // 6d8ed8: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8eda: fld dword ptr [0x858b50]
L_6d8ee0:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d8ee0: mov ecx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d8ee6: fmul dword ptr [ecx + 4]
    goto L_6d8fa6;   // 6d8ee9: jmp 0x6d8fa6
L_6d8eee:;
    *(uint32*)(uintptr_t)(uint32)(esp + 0x274) = (uint32)(0x0u);   // 6d8eee: mov dword ptr [esp + 0x274], 0
    goto L_6d8fad;   // 6d8ef9: jmp 0x6d8fad
L_6d8efe:;
    ebx = (ebx & 0xFFFF0000u) | (((uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22))) & 0xFFFFu);   // 6d8efe: mov bx, word ptr [esi + 0x22]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8f02: fld dword ptr [esp + 0x274]
    fA = (ebx & 0xFFFFu); fB = 0x21Bu; fR = (fA - fB) & 0xFFFFu;   // 6d8f09: cmp bx, 0x21b
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d8f0e: mov ecx, dword ptr [esi + 0x388]
    if (fR != 0) goto L_6d8f1b;   // 6d8f14: jne 0x6d8f1b
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d8f16: fmul dword ptr [ecx + 4]
    goto L_6d8f2a;   // 6d8f19: jmp 0x6d8f2a
L_6d8f1b:;
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = ST0 + m   // 6d8f1b: fadd dword ptr [0x858624]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d8f21: fmul dword ptr [ecx + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b8c)));   // ST0 = ST0 * m   // 6d8f24: fmul dword ptr [0x858b8c]
L_6d8f2a:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8f2a: fstp dword ptr [esp + 0x274]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8f31: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8f35: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f3b: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8f3d: test ah, 0x41
    if (fR != 0) goto L_6d8fad;   // 6d8f40: jne 0x6d8fad
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // 6d8f42: fld dword ptr [ecx + 8]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d8f45: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f4b: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8f4d: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8fad;   // 6d8f50: jp 0x6d8fad
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // 6d8f52: fld dword ptr [ecx + 8]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8f55: fcomp dword ptr [0x858b50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8f5b: fld dword ptr [esp + 0x10]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f5f: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x1u; fR = (fA & fB) & 0xFFu;   // 6d8f61: test ah, 1
    if (fR != 0) goto L_6d8f75;   // 6d8f64: jne 0x6d8f75
    fp.Fsub((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 - m   // 6d8f66: fsub dword ptr [ecx + 8]
    fp.FldSt(0);   // 6d8f69: fld st(0)
    fp.Arith('*', 1, 1, 0, true);   // 6d8f6b: fmulp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858f50)));   // ST0 = ST0 * m   // 6d8f6d: fmul dword ptr [0x858f50]
    goto L_6d8f82;   // 6d8f73: jmp 0x6d8f82
L_6d8f75:;
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 + m   // 6d8f75: fadd dword ptr [ecx + 8]
    fp.FldSt(0);   // 6d8f78: fld st(0)
    fp.Arith('*', 1, 1, 0, true);   // 6d8f7a: fmulp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b3c)));   // ST0 = ST0 * m   // 6d8f7c: fmul dword ptr [0x858b3c]
L_6d8f82:;
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d8f82: fsubr dword ptr [0x858624]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8f88: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d8f8e: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f90: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8f92: test ah, 0x41
    if (fR != 0) goto L_6d8f9f;   // 6d8f95: jne 0x6d8f9f
    fp.FstpSt(0);   // 6d8f97: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8f99: fld dword ptr [0x858b50]
L_6d8f9f:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // ST0 = ST0 * m   // 6d8f9f: fmul dword ptr [esp + 0x274]
L_6d8fa6:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fa6: fstp dword ptr [esp + 0x274]
L_6d8fad:;
    fA = edi; fB = 0x4u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8fad: cmp edi, 4
    if (fR != 0) goto L_6d8fc1;   // 6d8fb0: jne 0x6d8fc1
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fb2: fld dword ptr [esp + 0x274]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c24)));   // ST0 = ST0 * m   // 6d8fb9: fmul dword ptr [0x858c24]
    goto L_6d8fd3;   // 6d8fbf: jmp 0x6d8fd3
L_6d8fc1:;
    fA = edi; fB = 0x5u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8fc1: cmp edi, 5
    if (fR != 0) goto L_6d8fda;   // 6d8fc4: jne 0x6d8fda
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fc6: fld dword ptr [esp + 0x274]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b1c)));   // ST0 = ST0 * m   // 6d8fcd: fmul dword ptr [0x858b1c]
L_6d8fd3:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fd3: fstp dword ptr [esp + 0x274]
L_6d8fda:;
    fA = (ebx & 0xFFFFu); fB = 0x208u; fR = (fA - fB) & 0xFFFFu;   // 6d8fda: cmp bx, 0x208
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d8fdf: mov edi, dword ptr [esi + 0x14]
    edx = (uint32)(edi + 0x10);   // 6d8fe2: lea edx, [edi + 0x10]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx));   // 6d8fe5: mov eax, dword ptr [edx]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 4));   // 6d8fe7: mov ecx, dword ptr [edx + 4]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 8));   // 6d8fea: mov edx, dword ptr [edx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(eax);   // 6d8fed: mov dword ptr [esp + 0x28], eax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(ecx);   // 6d8ff1: mov dword ptr [esp + 0x2c], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(edx);   // 6d8ff5: mov dword ptr [esp + 0x30], edx
    if (fR != 0) goto L_6d9084;   // 6d8ff9: jne 0x6d9084
    eax = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x86c));   // 6d8fff: movzx eax, word ptr [esi + 0x86c]
    ecx = (uint32)(int32_t)(int16_t)((uint32)(*(uint16*)(uintptr_t)(uint32)(0x8d33c4)));   // 6d9006: movsx ecx, word ptr [0x8d33c4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6d900d: mov dword ptr [esp + 0x14], eax
    edx = (uint32)(edi + 0x20);   // 6d9011: lea edx, [edi + 0x20]
    esp -= 4; M32(esp) = edx;   // 6d9014: push edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9015: fild dword ptr [esp + 0x18]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6d9019: mov dword ptr [esp + 0x18], ecx
    esp -= 4; M32(esp) = ecx;   // 6d901d: push ecx
    eax = (uint32)(esp + 0x250);   // 6d901e: lea eax, [esp + 0x250]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 * m   // 6d9025: fmul dword ptr [0x858fe4]
    fp.FidivI((*(int*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d902b: fidiv dword ptr [esp + 0x1c]
    fp.FldSt(0);   // 6d902f: fld st(0)
    fp.Fcos();   // 6d9031: fcos 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9033: fstp dword ptr [esp + 0x1c]
    fp.Fsin();   // 6d9037: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9039: fstp dword ptr [esp]
    esp -= 4; M32(esp) = eax;   // 6d903c: push eax
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d903d: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x20));   // 6d9042: mov ecx, dword ptr [esp + 0x20]
    esp += 0xCu;   // 6d9046: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9049: push eax
    fA = edi; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d904a: add edi, 0x10
    esp -= 4; M32(esp) = edi;   // 6d904d: push edi
    esp -= 4; M32(esp) = ecx;   // 6d904e: push ecx
    edx = (uint32)(esp + 0xec);   // 6d904f: lea edx, [esp + 0xec]
    esp -= 4; M32(esp) = edx;   // 6d9056: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9057: call 0x40fe90
    esp += 0xCu;   // 6d905c: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d905f: push eax
    eax = (uint32)(esp + 0x1a8);   // 6d9060: lea eax, [esp + 0x1a8]
    esp -= 4; M32(esp) = eax;   // 6d9067: push eax
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9068: call 0x40fe30
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d906d: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d906f: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9072: mov eax, dword ptr [eax + 8]
    esp += 0xCu;   // 6d9075: add esp, 0xc
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(ecx);   // 6d9078: mov dword ptr [esp + 0x28], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d907c: mov dword ptr [esp + 0x2c], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(eax);   // 6d9080: mov dword ptr [esp + 0x30], eax
L_6d9084:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9084: fld dword ptr [esp + 0x28]
    esp -= 0xCu;   // 6d9088: sub esp, 0xc
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d908b: fmul dword ptr [0x863984]
    ecx = esp;   // 6d9091: mov ecx, esp
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9093: fld dword ptr [esp + 0x38]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9097: fmul dword ptr [0x863984]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d909d: fld dword ptr [esp + 0x3c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d90a1: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d90a7: fstp dword ptr [esp + 0x48]
    fp.Fxch(1);   // 6d90ab: fxch st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x280)));   // ST0 = ST0 * m   // 6d90ad: fmul dword ptr [esp + 0x280]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d90b4: fstp dword ptr [esp + 0x34]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x280)));   // ST0 = ST0 * m   // 6d90b8: fmul dword ptr [esp + 0x280]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d90bf: fld dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x280)));   // ST0 = ST0 * m   // 6d90c3: fmul dword ptr [esp + 0x280]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // 6d90ca: fld dword ptr [esi + 0x8c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d90d0: fld dword ptr [esp + 0x34]
    fp.Arith('*', 0, 0, 1, false);   // 6d90d4: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d90d6: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6d90da: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6d90dc: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d90de: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6d90e2: fmul st(1)
    fp.Fxch(1);   // 6d90e4: fxch st(1)
    fp.FstpSt(0);   // 6d90e6: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d90e8: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d90ec: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d90f2: fstp dword ptr [esp + 0x34]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6d90f6: mov edx, dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d90fa: fld dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d90fe: mov dword ptr [ecx], edx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9100: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9106: fstp dword ptr [esp + 0x38]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6d910a: mov eax, dword ptr [esp + 0x38]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d910e: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6d9114: mov dword ptr [ecx + 4], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d9117: fstp dword ptr [esp + 0x3c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6d911b: mov edx, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d911f: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6d9122: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9124: call 0x5429f0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9129: fld dword ptr [esp + 0x48]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d912d: mov eax, dword ptr [esi + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 * m   // 6d9130: fmul dword ptr [eax + 8]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9133: mov ecx, dword ptr [eax]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9135: fld dword ptr [esp + 0x44]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9139: mov edx, dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6d913c: fmul dword ptr [eax + 4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(ecx);   // 6d913f: mov dword ptr [esp + 0x28], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9143: mov ecx, dword ptr [esi + 0x388]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d9149: mov dword ptr [esp + 0x2c], edx
    fp.Arith('+', 1, 1, 0, true);   // 6d914d: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d914f: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6d9153: fmul dword ptr [eax]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9155: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(eax);   // 6d9158: mov dword ptr [esp + 0x30], eax
    fp.Arith('+', 1, 1, 0, true);   // 6d915c: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d915e: fmul dword ptr [0x858c1c]
    fp.FldSt(0);   // 6d9164: fld st(0)
    fp.Fabs();   // 6d9166: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x14)));   // ST0 = ST0 * m   // 6d9168: fmul dword ptr [ecx + 0x14]
    fp.Arith('*', 1, 1, 0, true);   // 6d916b: fmulp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d916d: fld dword ptr [esp + 0x28]
    fp.Arith('*', 0, 0, 1, false);   // 6d9171: fmul st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9173: fld dword ptr [esp + 0x2c]
    fp.Arith('*', 0, 0, 2, false);   // 6d9177: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9179: fstp dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d917d: fld dword ptr [esp + 0x30]
    fp.Arith('*', 0, 0, 2, false);   // 6d9181: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9183: fstp dword ptr [esp + 0x30]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9187: mov edx, dword ptr [esi + 0x8c]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6d918d: mov dword ptr [esp + 0x14], edx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6d9191: fmul dword ptr [esp + 0x14]
    esp -= 0xCu;   // 6d9195: sub esp, 0xc
    eax = esp;   // 6d9198: mov eax, esp
    fp.FstpSt(1);   // 6d919a: fstp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d919c: fld dword ptr [esp + 0x38]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6d91a0: fmul dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d91a4: fld dword ptr [esp + 0x3c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6d91a8: fmul dword ptr [esp + 0x20]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d91ac: fstp dword ptr [esp + 0x70]
    fp.Fxch(1);   // 6d91b0: fxch st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d91b2: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d91b8: fstp dword ptr [esp + 0x40]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d91bc: mov ecx, dword ptr [esp + 0x40]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6d91c0: mov dword ptr [eax], ecx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d91c2: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d91c8: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d91cc: fld dword ptr [esp + 0x70]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d91d0: mov edx, dword ptr [esp + 0x44]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d91d4: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6d91da: mov dword ptr [eax + 4], edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d91dd: fstp dword ptr [esp + 0x48]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d91e1: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d91e5: mov dword ptr [eax + 8], ecx
    ecx = esi;   // 6d91e8: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d91ea: call 0x5429f0
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d91ef: mov edi, dword ptr [esi + 0x14]
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x1c));   // 6d91f2: mov ebx, dword ptr [esp + 0x1c]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x20));   // 6d91f6: mov ebp, dword ptr [esp + 0x20]
    edx = edi;   // 6d91fa: mov edx, edi
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx));   // 6d91fc: mov eax, dword ptr [edx]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 4));   // 6d91fe: mov ecx, dword ptr [edx + 4]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 8));   // 6d9201: mov edx, dword ptr [edx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(eax);   // 6d9204: mov dword ptr [esp + 0x28], eax
    esp -= 0xCu;   // 6d9208: sub esp, 0xc
    eax = esp;   // 6d920b: mov eax, esp
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ebx);   // 6d920d: mov dword ptr [eax], ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x38) = (uint32)(ecx);   // 6d920f: mov dword ptr [esp + 0x38], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x30));   // 6d9213: mov ecx, dword ptr [esp + 0x30]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x3c) = (uint32)(edx);   // 6d9217: mov dword ptr [esp + 0x3c], edx
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(ebp);   // 6d921b: mov dword ptr [eax + 4], ebp
    edx = (uint32)(esp + 0x104);   // 6d921e: lea edx, [esp + 0x104]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d9225: mov dword ptr [eax + 8], ecx
    esp -= 4; M32(esp) = edx;   // 6d9228: push edx
    ecx = esi;   // 6d9229: mov ecx, esi
    { CVector* out_ = (CVector*)(uintptr_t)M32(esp + 0); *out_ = v.GetSpeed(CVector{ (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)), (*(float*)(uintptr_t)(esp + 12)) }); eax = (uint32)(uintptr_t)out_; esp += 0x10; }   // 6d922b: call 0x542ce0
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 8)));   // 6d9230: fld dword ptr [eax + 8]
    fA = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22)); fB = 0x21Bu; fR = (fA - fB) & 0xFFFFu;   // 6d9233: cmp word ptr [esi + 0x22], 0x21b
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9239: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 4)));   // 6d923c: fld dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d923f: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9242: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax)));   // 6d9244: fld dword ptr [eax]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9246: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9248: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d924a: fmul dword ptr [0x858c1c]
    if (fR != 0) goto L_6d92b6;   // 6d9250: jne 0x6d92b6
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9252: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x274)), true);   // 6d9256: fcomp dword ptr [esp + 0x274]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d925d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d925f: test ah, 0x41
    if (fR != 0) goto L_6d926a;   // 6d9262: jne 0x6d926a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9264: fld dword ptr [esp + 0x10]
    goto L_6d9271;   // 6d9268: jmp 0x6d9271
L_6d926a:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d926a: fld dword ptr [esp + 0x274]
L_6d9271:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9271: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9275: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d927b: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d927d: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d92a3;   // 6d9280: jp 0x6d92a3
    fp.FstpSt(0);   // 6d9282: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9284: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x274)), true);   // 6d9288: fcomp dword ptr [esp + 0x274]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d928f: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9291: test ah, 5
    if (FlcParityEven(fR)) goto L_6d929c;   // 6d9294: jp 0x6d929c
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9296: fld dword ptr [esp + 0x10]
    goto L_6d92a3;   // 6d929a: jmp 0x6d92a3
L_6d929c:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d929c: fld dword ptr [esp + 0x274]
L_6d92a3:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d92a3: mov eax, dword ptr [esi + 0x388]
    fp.FldSt(1);   // 6d92a9: fld st(1)
    fp.Fabs();   // 6d92ab: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 * m   // 6d92ad: fmul dword ptr [eax + 0x10]
    fp.Arith('*', 0, 0, 2, false);   // 6d92b0: fmul st(2)
    fp.Fxch(1);   // 6d92b2: fxch st(1)
    goto L_6d92c9;   // 6d92b4: jmp 0x6d92c9
L_6d92b6:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d92b6: mov eax, dword ptr [esi + 0x388]
    fp.FldSt(0);   // 6d92bc: fld st(0)
    fp.Fabs();   // 6d92be: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 * m   // 6d92c0: fmul dword ptr [eax + 0x10]
    fp.Arith('*', 0, 0, 1, false);   // 6d92c3: fmul st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d92c5: fld dword ptr [esp + 0x10]
L_6d92c9:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0xc)));   // ST0 = ST0 * m   // 6d92c9: fmul dword ptr [eax + 0xc]
    esp -= 4; M32(esp) = ecx;   // 6d92cc: push ecx
    ecx = (uint32)(esp + 0x2c);   // 6d92cd: lea ecx, [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // ST0 = ST0 * m   // 6d92d1: fmul dword ptr [esp + 0x26c]
    fp.Arith('+', 1, 1, 0, true);   // 6d92d8: faddp st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d92da: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d92e0: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d92e6: fstp dword ptr [esp]
    fp.FstpSt(0);   // 6d92e9: fstp st(0)
    FlcVecScale(fp, (float*)(uintptr_t)ecx, (*(float*)(uintptr_t)(esp + 0))); eax = ecx; esp += 4;   // 6d92eb: call 0x40fef0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d92f0: fld dword ptr [esp + 0x50]
    esp -= 0xCu;   // 6d92f4: sub esp, 0xc
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // ST0 = ST0 + m   // 6d92f7: fadd dword ptr [esp + 0x28]
    eax = esp;   // 6d92fb: mov eax, esp
    esp -= 0xCu;   // 6d92fd: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6d9300: fstp dword ptr [esp + 0x4c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d9304: mov ecx, dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6d9308: fld dword ptr [esp + 0x6c]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6d930c: mov dword ptr [eax], ecx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // ST0 = ST0 + m   // 6d930e: fadd dword ptr [esp + 0x38]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d9312: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d9316: fld dword ptr [esp + 0x70]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6d931a: mov edx, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // ST0 = ST0 + m   // 6d931e: fadd dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6d9322: mov dword ptr [eax + 4], edx
    edx = esp;   // 6d9325: mov edx, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d9327: fstp dword ptr [esp + 0x54]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6d932b: mov ecx, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d932f: mov dword ptr [eax + 8], ecx
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d9332: mov eax, dword ptr [esp + 0x40]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9336: mov ecx, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6d933a: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d933c: mov eax, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d9340: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6d9343: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9345: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d9348: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6d934d: fld dword ptr [esp + 0x270]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9354: mov edi, dword ptr [esi + 0x14]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d9357: fcomp dword ptr [0x871f8c]
    fp.Fld(CTimer::ms_fTimeStep);   // 6d935d: fld dword ptr [0xb7cb5c]
    ecx = edi;   // 6d9363: mov ecx, edi
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx));   // 6d9365: mov edx, dword ptr [ecx]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 4));   // 6d9367: mov eax, dword ptr [ecx + 4]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 8));   // 6d936a: mov ecx, dword ptr [ecx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(eax);   // 6d936d: mov dword ptr [esp + 0x2c], eax
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9371: fnstsw ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(ecx);   // 6d9373: mov dword ptr [esp + 0x30], ecx
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d9377: test ah, 0x44
    esp -= 4; M32(esp) = ecx;   // 6d937a: push ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d937b: mov dword ptr [esp + 0x2c], edx
    ecx = (uint32)(esp + 0x2c);   // 6d937f: lea ecx, [esp + 0x2c]
    if (FlcParityEven(fR)) goto L_6d939b;   // 6d9383: jp 0x6d939b
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9385: mov edx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edx + 0x18)));   // ST0 = ST0 * m   // 6d938b: fmul dword ptr [edx + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6d938e: fmul dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // ST0 = ST0 * m   // 6d9392: fmul dword ptr [esp + 0x26c]
    goto L_6d93af;   // 6d9399: jmp 0x6d93af
L_6d939b:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d939b: mov eax, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 * m   // 6d93a1: fmul dword ptr [eax + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6d93a4: fmul dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // ST0 = ST0 * m   // 6d93a8: fmul dword ptr [esp + 0x274]
L_6d93af:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d93af: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d93b5: fstp dword ptr [esp]
    FlcVecScale(fp, (float*)(uintptr_t)ecx, (*(float*)(uintptr_t)(esp + 0))); eax = ecx; esp += 4;   // 6d93b8: call 0x40fef0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d93bd: fld dword ptr [esp + 0x50]
    esp -= 0xCu;   // 6d93c1: sub esp, 0xc
    fp.Fadd((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 + m   // 6d93c4: fadd dword ptr [edi + 0x20]
    ecx = esp;   // 6d93c7: mov ecx, esp
    esp -= 0xCu;   // 6d93c9: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6d93cc: fstp dword ptr [esp + 0x4c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d93d0: mov edx, dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6d93d4: fld dword ptr [esp + 0x6c]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d93d8: mov dword ptr [ecx], edx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(edi + 0x24)));   // ST0 = ST0 + m   // 6d93da: fadd dword ptr [edi + 0x24]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d93dd: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d93e1: fld dword ptr [esp + 0x70]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6d93e5: mov eax, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(edi + 0x28)));   // ST0 = ST0 + m   // 6d93e9: fadd dword ptr [edi + 0x28]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6d93ec: mov dword ptr [ecx + 4], eax
    eax = esp;   // 6d93ef: mov eax, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d93f1: fstp dword ptr [esp + 0x54]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6d93f5: mov edx, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d93f9: mov dword ptr [ecx + 8], edx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d93fc: mov ecx, dword ptr [esp + 0x40]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9400: mov edx, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6d9404: mov dword ptr [eax], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d9406: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6d940a: mov dword ptr [eax + 4], edx
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d940d: mov dword ptr [eax + 8], ecx
    ecx = esi;   // 6d9410: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d9412: call 0x542a50
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9417: mov eax, dword ptr [esi + 0x14]
    edx = (uint32)(esp + 0x74);   // 6d941a: lea edx, [esp + 0x74]
    esp -= 4; M32(esp) = edx;   // 6d941e: push edx
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d941f: add eax, 0x10
    esp -= 4; M32(esp) = eax;   // 6d9422: push eax
    eax = (uint32)(esp + 0x208);   // 6d9423: lea eax, [esp + 0x208]
    esp -= 4; M32(esp) = eax;   // 6d942a: push eax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x80) = (uint32)(0x0u);   // 6d942b: mov dword ptr [esp + 0x80], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x84) = (uint32)(0x0u);   // 6d9436: mov dword ptr [esp + 0x84], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x88) = (uint32)(0x3F800000u);   // 6d9441: mov dword ptr [esp + 0x88], 0x3f800000
    FlcVecCross(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d944c: call 0x59c730
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858624)));   // 6d9451: fld dword ptr [0x858624]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9457: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9459: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d945c: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x34) = (uint32)(ecx);   // 6d945f: mov dword ptr [esp + 0x34], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9463: mov ecx, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x28)));   // 6d9466: fld dword ptr [ecx + 0x28]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x3c) = (uint32)(eax);   // 6d9469: mov dword ptr [esp + 0x3c], eax
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d946d: fcomp dword ptr [0x858b50]
    esp += 0xCu;   // 6d9473: add esp, 0xc
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d9476: mov dword ptr [esp + 0x2c], edx
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d947a: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d947c: test ah, 0x41
    if (fR == 0) goto L_6d94ab;   // 6d947f: je 0x6d94ab
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9481: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d9485: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d948b: fstp dword ptr [esp + 0x28]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d948f: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d9493: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9499: fstp dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d949d: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d94a1: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d94a7: fstp dword ptr [esp + 0x30]
L_6d94ab:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // 6d94ab: fld dword ptr [ecx + 8]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d94ae: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d94b4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d94b6: test ah, 0x41
    if (fR != 0) goto L_6d94c3;   // 6d94b9: jne 0x6d94c3
    fp.FstpSt(0);   // 6d94bb: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // 6d94bd: fld dword ptr [0x858c1c]
L_6d94c3:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x18)));   // 6d94c3: fld dword ptr [ecx + 0x18]
    esp -= 4; M32(esp) = ecx;   // 6d94c6: push ecx
    fp.Fabs();   // 6d94c7: fabs 
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d94c9: fsubr dword ptr [0x858624]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d94cf: fld dword ptr [esp + 0x34]
    edx = (uint32)(esp + 0x114);   // 6d94d3: lea edx, [esp + 0x114]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6d94da: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d94dd: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d94e1: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d94e4: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d94e6: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6d94ea: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6d94ec: faddp st(1)
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d94ee: fsubr dword ptr [0x858624]
    fp.Arith('*', 0, 0, 2, false);   // 6d94f4: fmul st(2)
    fp.Arith('*', 1, 1, 0, true);   // 6d94f6: fmulp st(1)
    fp.FstpSt(1);   // 6d94f8: fstp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d94fa: fld dword ptr [esp + 0x54]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 0x20)));   // ST0 = ST0 + m   // 6d94fe: fadd dword ptr [ecx + 0x20]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9501: fstp dword ptr [esp + 0x38]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x58)));   // 6d9505: fld dword ptr [esp + 0x58]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 0x24)));   // ST0 = ST0 + m   // 6d9509: fadd dword ptr [ecx + 0x24]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d950c: fstp dword ptr [esp + 0x3c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x5c)));   // 6d9510: fld dword ptr [esp + 0x5c]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 0x28)));   // ST0 = ST0 + m   // 6d9514: fadd dword ptr [ecx + 0x28]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9517: mov ecx, dword ptr [esi + 0x388]
    esp -= 4; M32(esp) = ecx;   // 6d951d: push ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d951e: fstp dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x1c)));   // ST0 = ST0 * m   // 6d9522: fmul dword ptr [ecx + 0x1c]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9525: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d952b: fmul dword ptr [esi + 0x90]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b8c)));   // ST0 = ST0 * m   // 6d9531: fmul dword ptr [0x858b8c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9537: fstp dword ptr [esp]
    esp -= 4; M32(esp) = edx;   // 6d953a: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d953b: call 0x40fe90
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d9540: mov edx, dword ptr [esp + 0x40]
    ecx = esp;   // 6d9544: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d9546: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9548: mov edx, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d954c: mov dword ptr [ecx + 4], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d954f: mov edx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d9553: mov dword ptr [ecx + 8], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9556: mov edx, dword ptr [eax]
    esp -= 0xCu;   // 6d9558: sub esp, 0xc
    ecx = esp;   // 6d955b: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d955d: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d955f: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9562: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d9565: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d9568: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d956b: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d956d: call 0x542a50
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9572: mov edi, dword ptr [esi + 0x14]
    ecx = (uint32)(edi + 0x20);   // 6d9575: lea ecx, [edi + 0x20]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx));   // 6d9578: mov edx, dword ptr [ecx]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 4));   // 6d957a: mov eax, dword ptr [ecx + 4]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 8));   // 6d957d: mov ecx, dword ptr [ecx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(edx);   // 6d9580: mov dword ptr [esp + 0x28], edx
    esp -= 0xCu;   // 6d9584: sub esp, 0xc
    edx = esp;   // 6d9587: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(esp + 0x3c) = (uint32)(ecx);   // 6d9589: mov dword ptr [esp + 0x3c], ecx
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ebx);   // 6d958d: mov dword ptr [edx], ebx
    ecx = (uint32)(esp + 0x1c4);   // 6d958f: lea ecx, [esp + 0x1c4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x38) = (uint32)(eax);   // 6d9596: mov dword ptr [esp + 0x38], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x30));   // 6d959a: mov eax, dword ptr [esp + 0x30]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ebp);   // 6d959e: mov dword ptr [edx + 4], ebp
    esp -= 4; M32(esp) = ecx;   // 6d95a1: push ecx
    ecx = esi;   // 6d95a2: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d95a4: mov dword ptr [edx + 8], eax
    { CVector* out_ = (CVector*)(uintptr_t)M32(esp + 0); *out_ = v.GetSpeed(CVector{ (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)), (*(float*)(uintptr_t)(esp + 12)) }); eax = (uint32)(uintptr_t)out_; esp += 0x10; }   // 6d95a7: call 0x542ce0
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 8)));   // 6d95ac: fld dword ptr [eax + 8]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x28)));   // ST0 = ST0 * m   // 6d95af: fmul dword ptr [edi + 0x28]
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 4)));   // 6d95b2: fld dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x24)));   // ST0 = ST0 * m   // 6d95b5: fmul dword ptr [edi + 0x24]
    fp.Arith('+', 1, 1, 0, true);   // 6d95b8: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax)));   // 6d95ba: fld dword ptr [eax]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d95bc: mov eax, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 * m   // 6d95c2: fmul dword ptr [edi + 0x20]
    esp -= 0xCu;   // 6d95c5: sub esp, 0xc
    edx = esp;   // 6d95c8: mov edx, esp
    esp -= 0xCu;   // 6d95ca: sub esp, 0xc
    fp.Arith('+', 1, 1, 0, true);   // 6d95cd: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d95cf: fmul dword ptr [0x858c1c]
    fp.FldSt(0);   // 6d95d5: fld st(0)
    fp.Fabs();   // 6d95d7: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x24)));   // ST0 = ST0 * m   // 6d95d9: fmul dword ptr [eax + 0x24]
    fp.Arith('*', 1, 1, 0, true);   // 6d95dc: fmulp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d95de: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x20)));   // ST0 = ST0 * m   // 6d95e2: fmul dword ptr [eax + 0x20]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // ST0 = ST0 * m   // 6d95e5: fmul dword ptr [esp + 0x284]
    fp.Arith('+', 1, 1, 0, true);   // 6d95ec: faddp st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d95ee: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d95f4: fmul dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d95fa: fld dword ptr [esp + 0x40]
    fp.Arith('*', 0, 0, 1, false);   // 6d95fe: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d9600: fstp dword ptr [esp + 0x40]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9604: fld dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6d9608: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d960a: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d960e: fld dword ptr [esp + 0x48]
    fp.Arith('*', 0, 0, 1, false);   // 6d9612: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9614: fstp dword ptr [esp + 0x48]
    fp.FstpSt(0);   // 6d9618: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x68)));   // 6d961a: fld dword ptr [esp + 0x68]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // ST0 = ST0 + m   // 6d961e: fadd dword ptr [esp + 0x34]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6d9622: fstp dword ptr [esp + 0x4c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d9626: mov eax, dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6d962a: fld dword ptr [esp + 0x6c]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6d962e: mov dword ptr [edx], eax
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // ST0 = ST0 + m   // 6d9630: fadd dword ptr [esp + 0x38]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d9634: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d9638: fld dword ptr [esp + 0x70]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6d963c: mov ecx, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // ST0 = ST0 + m   // 6d9640: fadd dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d9644: mov dword ptr [edx + 4], ecx
    ecx = esp;   // 6d9647: mov ecx, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d9649: fstp dword ptr [esp + 0x54]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6d964d: mov eax, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9651: mov dword ptr [edx + 8], eax
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d9654: mov edx, dword ptr [esp + 0x40]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9658: mov eax, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d965c: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d965e: mov edx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6d9662: mov dword ptr [ecx + 4], eax
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d9665: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6d9668: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d966a: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d966f: fld dword ptr [esp + 0x48]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9673: mov edi, dword ptr [esi + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x28)));   // ST0 = ST0 * m   // 6d9676: fmul dword ptr [edi + 0x28]
    ecx = (uint32)(esp + 0x40);   // 6d9679: lea ecx, [esp + 0x40]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d967d: fld dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x24)));   // ST0 = ST0 * m   // 6d9681: fmul dword ptr [edi + 0x24]
    fp.Arith('+', 1, 1, 0, true);   // 6d9684: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d9686: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 * m   // 6d968a: fmul dword ptr [edi + 0x20]
    fp.Arith('+', 1, 1, 0, true);   // 6d968d: faddp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d968f: fstp dword ptr [esp + 0x14]
    FlcVecMagnitude(fp, (float*)(uintptr_t)ecx);   // 6d9693: call 0x4082c0
    esp -= 4; M32(esp) = ecx;   // 6d9698: push ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9699: fstp dword ptr [esp]
    esp -= 4; M32(esp) = 0x3C23D70Au;   // 6d969c: push 0x3c23d70a
    FlcFloatMax(fp, (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)));   // 6d96a1: call 0x420800
    fp.Fdivr((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // ST0 = m / ST0   // 6d96a6: fdivr dword ptr [esp + 0x1c]
    esp += 0x4u;   // 6d96aa: add esp, 4
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d96ad: fstp dword ptr [esp]
    esp -= 4; M32(esp) = 0x3F800000u;   // 6d96b0: push 0x3f800000
    FlcFloatMin(fp, (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)));   // 6d96b5: call 0x404330
    esp += 0x4u;   // 6d96ba: add esp, 4
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d96bd: fstp dword ptr [esp]
    esp -= 4; M32(esp) = 0xBF800000u;   // 6d96c0: push 0xbf800000
    FlcFloatMax(fp, (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)));   // 6d96c5: call 0x420800
    esp += 0x8u;   // 6d96ca: add esp, 8
    FlcCrtAsin(fp);   // 6d96cd: call 0x821e70
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x594));   // 6d96d2: mov ebx, dword ptr [esi + 0x594]
    fA = ebx; fB = 0x4u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d96d8: cmp ebx, 4
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d96db: fmul dword ptr [0x858c1c]
    if (fR != 0) goto L_6d970d;   // 6d96e1: jne 0x6d970d
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871d98)), false);   // 6d96e3: fcom dword ptr [0x871d98]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d96e9: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d96eb: test ah, 0x41
    if (fR != 0) goto L_6d970d;   // 6d96ee: jne 0x6d970d
    fp.Fld(CTimer::ms_fTimeStep);   // 6d96f0: fld dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b38)));   // ST0 = ST0 * m   // 6d96f6: fmul dword ptr [0x858b38]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c4c)));   // ST0 = ST0 * m   // 6d96fc: fmul dword ptr [0x858c4c]
    eax = (uint32)FlcCrtFtol2(fp);   // 6d9702: call 0x821b40
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x9a0)); fB = eax; fR = (fA + fB) & 0xFFFFFFFFu; *(uint32*)(uintptr_t)(uint32)(esi + 0x9a0) = (uint32)(fR);   // 6d9707: add dword ptr [esi + 0x9a0], eax
L_6d970d:;
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x264));   // 6d970d: mov edx, dword ptr [esp + 0x264]
    fA = edx; fB = 0x1u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d9714: cmp edx, 1
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9717: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x28)));   // 6d971d: fld dword ptr [ecx + 0x28]
    if (fR != 0) goto L_6d974f;   // 6d9720: jne 0x6d974f
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8d3670)));   // 6d9722: fld dword ptr [0x8d3670]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9728: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d972e: fstp dword ptr [esp + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9732: fld dword ptr [esp + 0x18]
    fp.Arith('*', 0, 0, 1, false);   // 6d9736: fmul st(1)
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x14)), true);   // 6d9738: fcomp dword ptr [esp + 0x14]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d973c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d973e: test ah, 0x41
    if (fR != 0) goto L_6d976a;   // 6d9741: jne 0x6d976a
    fp.FstpSt(0);   // 6d9743: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d9745: fld dword ptr [esp + 0x14]
    fp.Fdiv((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 / m   // 6d9749: fdiv dword ptr [esp + 0x18]
    goto L_6d976a;   // 6d974d: jmp 0x6d976a
L_6d974f:;
    fA = ebx; fB = 0x4u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d974f: cmp ebx, 4
    if (fR != 0) goto L_6d976a;   // 6d9752: jne 0x6d976a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x9cc)));   // 6d9754: fld dword ptr [esi + 0x9cc]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d975a: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9760: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9762: test ah, 5
    if (FlcParityEven(fR)) goto L_6d976a;   // 6d9765: jp 0x6d976a
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x34)));   // ST0 = ST0 * m   // 6d9767: fmul dword ptr [ecx + 0x34]
L_6d976a:;
    fp.Fxch(1);   // 6d976a: fxch st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x2c)));   // ST0 = ST0 * m   // 6d976c: fmul dword ptr [ecx + 0x2c]
    fp.Arith('+', 0, 0, 1, false);   // 6d976f: fadd st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // ST0 = ST0 * m   // 6d9771: fmul dword ptr [esi + 0x8c]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9777: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 * m   // 6d977d: fmul dword ptr [esp + 0x18]
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d9781: fst dword ptr [esp + 0x14]
    fp.FstpSt(1);   // 6d9785: fstp st(1)
    fp.Fld(CTimer::ms_fTimeStep);   // 6d9787: fld dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // ST0 = ST0 * m   // 6d978d: fmul dword ptr [esi + 0x8c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9793: fmul dword ptr [0x863984]
    fp.Fxch(1);   // 6d9799: fxch st(1)
    fp.Fcompp();   // 6d979b: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d979d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d979f: test ah, 0x41
    if (fR != 0) goto L_6d983a;   // 6d97a2: jne 0x6d983a
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d97a8: test edi, edi
    eax = (uint32)(edi + 0x30);   // 6d97aa: lea eax, [edi + 0x30]
    if (fR != 0) goto L_6d97b2;   // 6d97ad: jne 0x6d97b2
    eax = (uint32)(esi + 4);   // 6d97af: lea eax, [esi + 4]
L_6d97b2:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d97b2: mov eax, dword ptr [eax + 8]
    esp -= 4; M32(esp) = edx;   // 6d97b5: push edx
    esp -= 4; M32(esp) = eax;   // 6d97b6: push eax
    ecx = esi;   // 6d97b7: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d97b9: call 0x6d2600
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d97be: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d97c4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d97c6: test ah, 0x41
    if (fR != 0) goto L_6d983a;   // 6d97c9: jne 0x6d983a
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d97cb: test edi, edi
    eax = (uint32)(edi + 0x30);   // 6d97cd: lea eax, [edi + 0x30]
    if (fR != 0) goto L_6d97d5;   // 6d97d0: jne 0x6d97d5
    eax = (uint32)(esi + 4);   // 6d97d2: lea eax, [esi + 4]
L_6d97d5:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d97d5: mov ecx, dword ptr [eax + 8]
    esp -= 4; M32(esp) = edx;   // 6d97d8: push edx
    esp -= 4; M32(esp) = ecx;   // 6d97d9: push ecx
    ecx = esi;   // 6d97da: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d97dc: call 0x6d2600
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b38)));   // ST0 = ST0 * m   // 6d97e1: fmul dword ptr [0x858b38]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d97e7: fsubr dword ptr [0x858624]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d97ed: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d97f3: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d97f5: test ah, 5
    if (FlcParityEven(fR)) goto L_6d9802;   // 6d97f8: jp 0x6d9802
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d97fa: fld dword ptr [0x858b50]
    goto L_6d9824;   // 6d9800: jmp 0x6d9824
L_6d9802:;
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9802: test edi, edi
    eax = (uint32)(edi + 0x30);   // 6d9804: lea eax, [edi + 0x30]
    if (fR != 0) goto L_6d980c;   // 6d9807: jne 0x6d980c
    eax = (uint32)(esi + 4);   // 6d9809: lea eax, [esi + 4]
L_6d980c:;
    esp -= 4; M32(esp) = edx;   // 6d980c: push edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d980d: mov edx, dword ptr [eax + 8]
    esp -= 4; M32(esp) = edx;   // 6d9810: push edx
    ecx = esi;   // 6d9811: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d9813: call 0x6d2600
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b38)));   // ST0 = ST0 * m   // 6d9818: fmul dword ptr [0x858b38]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d981e: fsubr dword ptr [0x858624]
L_6d9824:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // ST0 = ST0 * m   // 6d9824: fmul dword ptr [esi + 0x8c]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d982a: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9830: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d9836: fstp dword ptr [esp + 0x14]
L_6d983a:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x14));   // 6d983a: mov eax, dword ptr [esp + 0x14]
    fA = edi; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d983e: add edi, 0x20
    esp -= 4; M32(esp) = edi;   // 6d9841: push edi
    esp -= 4; M32(esp) = eax;   // 6d9842: push eax
    ecx = (uint32)(esp + 0x94);   // 6d9843: lea ecx, [esp + 0x94]
    esp -= 4; M32(esp) = ecx;   // 6d984a: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d984b: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9850: mov ecx, dword ptr [eax]
    edx = esp;   // 6d9852: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6d9854: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9856: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9859: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d985c: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6d985f: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9861: mov dword ptr [edx + 8], eax
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9864: call 0x5429f0
    return;   // 6d9869: jmp 0x6da900
}
