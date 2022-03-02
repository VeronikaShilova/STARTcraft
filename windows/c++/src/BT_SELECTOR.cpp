#include "BT_SELECTOR.h"

BT_SELECTOR::BT_SELECTOR(BT_NODE* parent, size_t childrenMaxCount) : BT_NODE(parent, childrenMaxCount) {};

BT_SELECTOR::BT_SELECTOR(size_t childrenMaxCount) : BT_NODE(childrenMaxCount) {};

BT_NODE::State BT_SELECTOR::Evaluate(void* data) {
    //std::cout << "selector evaluate" << std::endl;
    for (size_t childIdx = 0; childIdx < ChildrenCount; ++childIdx) {
        const BT_NODE::State childState = Children[childIdx]->Evaluate(data);
        if (childState != BT_NODE::State::FAILURE) {
            return childState;
        }
    }
    return BT_NODE::State::FAILURE;
}