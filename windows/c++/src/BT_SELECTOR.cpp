#include"BT_NODE.cpp"

class BT_SELECTOR : public BT_NODE {

public:
    BT_SELECTOR(BT_NODE* parent, size_t childrenMaxCount) : BT_NODE(parent, childrenMaxCount) {};
    BT_SELECTOR(size_t childrenMaxCount) : BT_NODE(childrenMaxCount) {};
    State Evaluate(void* data) {
        for (size_t childIdx = 0; childIdx < ChildrenCount; ++childIdx) {
            BT_NODE::State  childState = Children[childIdx]->Evaluate(data);
            if (childState == BT_NODE::State::SUCCESS) {
                return BT_NODE::State::SUCCESS;
            }
            if (childState == BT_NODE::State::RUNNING) {
                return BT_NODE::State::RUNNING;
            }
        }
        return BT_NODE::State::FAILURE;
    }

};