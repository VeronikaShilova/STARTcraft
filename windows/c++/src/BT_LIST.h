#pragma once
#include "BT_NODE.h"

class BT_LIST : public BT_NODE {

public:
    BT_LIST(BT_NODE* parent, size_t childrenMaxCount);
    BT_LIST(size_t childrenMaxCount);
    State Evaluate(void* data) final;
};
