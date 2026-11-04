#include "StdInc.h"

#include "NodeAddress.h"

void CNodeAddress::InjectHooks() {
    RH_ScopedClass(CNodeAddress);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(operator==, 0x420920);
    RH_ScopedInstall(operator!=, 0x420950);
    RH_ScopedInstall(operator<, 0x420980);
}

// 0x420920
bool CNodeAddress::operator==(CNodeAddress const& other) const {
    return m_wAreaId == other.m_wAreaId && m_wNodeId == other.m_wNodeId;
}

// 0x420950
bool CNodeAddress::operator!=(CNodeAddress const& other) const {
    return m_wAreaId != other.m_wAreaId || m_wNodeId != other.m_wNodeId;
}

// 0x420980
bool CNodeAddress::operator<(CNodeAddress const& other) const {
    if (m_wAreaId != other.m_wAreaId) {
        return m_wAreaId < other.m_wAreaId;
    }
    return m_wNodeId < other.m_wNodeId;
}
