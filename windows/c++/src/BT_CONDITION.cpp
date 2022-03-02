#include "BT_CONDITION.h"
#include <assert.h>

BT_CONDITION::BT_CONDITION(BT_NODE* parent, EVALUATE_CBK evaluateCBK) : BT_NODE(parent, 0), EvaluateCBK(evaluateCBK) {}

BT_CONDITION::State BT_CONDITION::Evaluate(void* data) {
    bool condition = EvaluateCBK(data);
    if (condition) return SUCCESS;
    else return FAILURE;
}