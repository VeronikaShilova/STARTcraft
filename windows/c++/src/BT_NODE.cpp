#include<assert.h>

class BT_NODE {

public:
    enum State {
        RUNNING,
        SUCCESS,
        FAILURE
    };

protected:
    BT_NODE** Children;
    size_t    ChildrenCount;
    size_t    ChildrenMaxCount;
    BT_NODE* Parent;

    ~BT_NODE() {
        delete[] Children;
    }

    BT_NODE(size_t childrenMaxCount) : Parent(0), ChildrenMaxCount(childrenMaxCount), ChildrenCount(0) {
        InitChildrenArray(childrenMaxCount);
    }

    BT_NODE(BT_NODE* parent, size_t childrenMaxCount) : Parent(parent), ChildrenMaxCount(childrenMaxCount), ChildrenCount(0) {
        InitChildrenArray(childrenMaxCount);
        if (Parent)
            Parent->AddChild(this);
    }

private:
    BT_NODE() {}

    void InitChildrenArray(size_t childrenMaxCount) {
        Children = new BT_NODE * [ChildrenMaxCount];
    }

public:
    virtual State Evaluate(void* data) = 0;

    size_t AddChild(BT_NODE* child) {
        assert(ChildrenCount < ChildrenMaxCount);
        Children[ChildrenCount++] = child;
        return ChildrenCount;
    }

};