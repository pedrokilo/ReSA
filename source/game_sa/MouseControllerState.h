#pragma once

class CMouseControllerState {
public:
    // DirectInput stores button masks (0 or 0x80), not canonical C++ bools.
    uint8     isMouseLeftButtonPressed{};   // LMB
    uint8     isMouseRightButtonPressed{};  // RMB
    uint8     isMouseMiddleButtonPressed{}; // MMB
    bool      isMouseWheelMovedUp{};        // Wheel up
    bool      isMouseWheelMovedDown{};      // Wheel down
    uint8     isMouseFirstXPressed{};       // BMX1
    uint8     isMouseSecondXPressed{};      // BMX2
    float     m_fWheelMoved{};              // Wheel movement
    CVector2D m_AmountMoved{};              // Mouse movement

public:
    static void InjectHooks();

    CMouseControllerState();
    CMouseControllerState* Constructor();

    void Clear();
    [[nodiscard]] bool CheckForInput() const;
    [[nodiscard]] auto GetAmountMouseMoved() const { return m_AmountMoved; }
};
VALIDATE_SIZE(CMouseControllerState, 0x14);
