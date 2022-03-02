#include "BT_ACTION.h"
#include <assert.h>

BT_ACTION::BT_ACTION(BT_NODE* parent, ACTION_CBK ActionCBK) : BT_NODE(parent, 0), ActionCBK(ActionCBK) {}

BT_ACTION::State BT_ACTION::Evaluate(void* data) {
    //std::cout << "action evaluate" << std::endl;

    bool actionComplete = ActionCBK(data);
    if (actionComplete) return SUCCESS;
    else return FAILURE;
}