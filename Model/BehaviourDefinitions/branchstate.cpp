#include "branchstate.h"
#include "condition.h"

BranchState::BranchState():Branch(NK_BRANCH_STATE){
}

bool BranchState::canAdoptChild(Node *child)
{
    return Branch::canAdoptChild(child);
}

bool BranchState::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Branch::canAcceptEdge(edgeKind, dst);
}
