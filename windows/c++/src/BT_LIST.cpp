#include "BT_LIST.h"

BT_LIST::BT_LIST(BT_NODE* parent, size_t childrenMaxCount) : BT_NODE(parent, childrenMaxCount) {};

BT_LIST::BT_LIST(size_t childrenMaxCount) : BT_NODE(childrenMaxCount) {};

BT_NODE::State BT_LIST::Evaluate(void* data) {
    for (size_t childIdx = 0; childIdx < ChildrenCount; ++childIdx) {
        Children[childIdx]->Evaluate(data);
    }
    return BT_NODE::State::SUCCESS;
}