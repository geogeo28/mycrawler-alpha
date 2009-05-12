#ifndef MCGRAPH_H
#define MCGRAPH_H
#include <QList>

class CNode;
class CEdge;

class MCGraph
{
public:
    MCGraph();
    CNode* addNode(const CNode& node, CNode* preview = NULL);
    CEdge* addEdge(CNode* nodeLeft, CNode* nodeRight);
    void display() const;
private:
    QList<CNode*> m_lstNodes;
    QList<CEdge*> m_lstEdges;
};

#endif // MCGRAPH_H
