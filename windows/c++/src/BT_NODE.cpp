#include <assert.h>
#include "BT_NODE.h"

BT_NODE::BT_NODE(size_t childrenMaxCount)
  : Parent(0), ChildrenMaxCount(childrenMaxCount), ChildrenCount(0) {
    InitChildrenArray(childrenMaxCount);
}

BT_NODE::BT_NODE(BT_NODE* parent, size_t childrenMaxCount)
  : Parent(parent), ChildrenMaxCount(childrenMaxCount), ChildrenCount(0) {
    InitChildrenArray(childrenMaxCount);
    if (Parent) {
        Parent->AddChild(this);
    }
}

void BT_NODE::InitChildrenArray(size_t childrenMaxCount) {
    Children = new BT_NODE* [ChildrenMaxCount];
}

BT_NODE::~BT_NODE() {
    /*
    for (size_t childIdx = 0; childIdx < ChildrenCount; ++childIdx) {
        delete Children[childIdx];
    }
    */
    delete [] Children;
}

size_t BT_NODE::AddChild(BT_NODE* child) noexcept {
    assert(ChildrenCount < ChildrenMaxCount);
    Children[ChildrenCount] = child;
    return ++ChildrenCount;
}