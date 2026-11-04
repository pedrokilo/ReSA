/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CNodeAddress {
public:
    uint16 m_wAreaId{ (uint16)-1 };
    uint16 m_wNodeId{ (uint16)-1 };

    constexpr CNodeAddress() = default;
    constexpr CNodeAddress(uint16 areaId, uint16 nodeId) : m_wAreaId(areaId), m_wNodeId(nodeId) {}

    static void InjectHooks();

    bool operator==(CNodeAddress const&) const; // 0x420920
    bool operator!=(CNodeAddress const&) const; // 0x420950
    bool operator<(CNodeAddress const&) const;  // 0x420980, orden lexicografico sin signo (area y luego nodo)

    void ResetAreaId() { m_wAreaId = UINT16_MAX; }
    void ResetNodeId() { m_wNodeId = UINT16_MAX; }

    [[nodiscard]] bool IsAreaValid() const { return m_wAreaId != (uint16)-1; }
    [[nodiscard]] bool IsValid() const { return IsAreaValid() && m_wNodeId != UINT16_MAX; }

    operator bool() const { return IsValid(); }
};
VALIDATE_SIZE(CNodeAddress, 0x4);
