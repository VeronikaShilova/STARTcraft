#pragma once

class BT_NODE {

public:
     enum State {
        RUNNING,
        SUCCESS,
        FAILURE
      };

//protected:
     BT_NODE** Children;
     size_t    ChildrenCount;
     size_t    ChildrenMaxCount;
     BT_NODE*  Parent;

private:
    void InitChildrenArray(size_t childrenMaxCount);

public:
    BT_NODE() = delete;
    BT_NODE(size_t childrenMaxCount);
    BT_NODE(BT_NODE* parent, size_t childrenMaxCount);
    virtual ~BT_NODE();
    virtual State Evaluate(void* data) = 0;
    size_t AddChild(BT_NODE* child) noexcept;
};
