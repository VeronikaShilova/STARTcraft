#pragma once
#include "BT_NODE.h"

class BT_CONDITION : public BT_NODE {

public:
    typedef bool(*EVALUATE_CBK)(void* data);

protected:
    EVALUATE_CBK  EvaluateCBK;

public:
    BT_CONDITION(BT_NODE* parent, EVALUATE_CBK evaluateCBK);
    
    State Evaluate(void* data) final;
};