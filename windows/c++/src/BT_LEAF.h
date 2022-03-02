#pragma once
#include "BT_NODE.h"

class BT_LEAF : public BT_NODE {

public:
    typedef BT_NODE::State(*EVALUATE_CBK)(void* data);

protected:
    EVALUATE_CBK  EvaluateCBK;

public:
    BT_LEAF(BT_NODE* parent, EVALUATE_CBK evaluateCBK);
    
    State Evaluate(void* data) final;
};