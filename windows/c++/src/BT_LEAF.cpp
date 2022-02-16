#include"BT_NODE.cpp"

class BT_LEAF : public BT_NODE {

public:
    typedef BT_NODE::State(*EVALUATE_CBK)(void* data);

protected:
    EVALUATE_CBK  EvaluateCBK;

public:
    BT_LEAF(BT_NODE* parent, EVALUATE_CBK evaluateCBK) : EvaluateCBK(evaluateCBK), BT_NODE(parent, 0) {}
    State Evaluate(void* data) {
        assert(EvaluateCBK);
        return EvaluateCBK(data);
    }
};