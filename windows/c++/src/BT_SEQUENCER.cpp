#include "BT_SEQUENCER.h"
#include <iostream>

BT_SEQUENCER::BT_SEQUENCER(BT_NODE* parent, size_t childrenMaxCount) : BT_NODE(parent, childrenMaxCount) {};

BT_SEQUENCER::BT_SEQUENCER(size_t childrenMaxCount) : BT_NODE(childrenMaxCount) {};

BT_NODE::State BT_SEQUENCER::Evaluate(void* data) {
    
    //std::cout << "sequencer evaluate" << std::endl;

    for (size_t childIdx = 0; childIdx < ChildrenCount; ++childIdx) {
        const BT_NODE::State childState = Children[childIdx]->Evaluate(data);
        if (childState != BT_NODE::State::SUCCESS) {
            return childState;
        }
    }
    return BT_NODE::State::SUCCESS;
}