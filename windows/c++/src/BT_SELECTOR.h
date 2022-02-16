#pragma once
#include "BT_NODE.h"

class BT_SELECTOR : public BT_NODE {

public:
    BT_SELECTOR(BT_NODE* parent, size_t childrenMaxCount);
    BT_SELECTOR(size_t childrenMaxCount);
    State Evaluate(void* data) final;
};