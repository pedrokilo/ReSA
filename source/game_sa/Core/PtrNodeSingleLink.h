/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <PtrNode.h>

// Can't include <Pool.h> here, because it would cause circular dependency
namespace details {
void* CPtrNodeSingleLink__operator_new(size_t sz);
void  CPtrNodeSingleLink__operator_delete(void* data); // 0x552390 (lote 173A, banco Fn173A)
void __fastcall CPtrListSingleLink__FlushExe(void* list, void* edx); // 0x552400 (lote 131A, banco Fn131A)
};

template<typename TItemType>
class CPtrNodeSingleLink : public CPtrNode<TItemType, CPtrNodeSingleLink<TItemType>> {
public:
    using ItemType = TItemType;
    using RawItemType = std::remove_pointer_t<std::remove_cvref_t<ItemType>>;

public:
    static void* operator new(size_t sz) {
        return details::CPtrNodeSingleLink__operator_new(sz);
    }

    static void operator delete(void* data, size_t sz) {
        assert(sz == sizeof(CPtrNodeSingleLink<void*>));
        return details::CPtrNodeSingleLink__operator_delete(data);
    }

public:
    using CPtrNode<TItemType, CPtrNodeSingleLink<TItemType>>::CPtrNode;

    //void AddToList(CPtrListSingleLink* list) {
    //    m_next       = list->GetNode();
    //    list->m_Head = static_cast<CPtrNode*>(this);
    //}
};
VALIDATE_SIZE(CPtrNodeSingleLink<void*>, 0x8);
