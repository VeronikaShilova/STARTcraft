#include "BT_LEAF.h"
#include <assert.h>

BT_LEAF::BT_LEAF(BT_NODE* parent, EVALUATE_CBK evaluateCBK) : BT_NODE(parent, 0), EvaluateCBK(evaluateCBK) {}

BT_LEAF::State BT_LEAF::Evaluate(void* data) {
    assert(EvaluateCBK);
    return EvaluateCBK(data);
}