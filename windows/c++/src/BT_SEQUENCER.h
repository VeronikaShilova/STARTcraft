#pragma once
#include "BT_NODE.h"

class BT_SEQUENCER : public BT_NODE {

public:
    BT_SEQUENCER(BT_NODE* parent, size_t childrenMaxCount);
    BT_SEQUENCER(size_t childrenMaxCount);
    State Evaluate(void* data) final;
};
