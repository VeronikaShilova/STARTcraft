#pragma once
#include "BT_NODE.h"

class BT_ACTION : public BT_NODE {

public:
    typedef bool(*ACTION_CBK)(void* data);

protected:
    ACTION_CBK  ActionCBK;

public:
    BT_ACTION(BT_NODE* parent, ACTION_CBK evaluateCBK);

    State Evaluate(void* data) final;
};