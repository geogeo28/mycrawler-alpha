#include "Graph.h"
#include "Node.h"
#include "Edge.h"
#include "Debug/Logger.h"

MCGraph::MCGraph()
{
    CNode* p1 = addNode(CNode("1"));
    addNode(CNode("2"), p1);
    addNode(CNode("3"),p1);
    CNode* p4 = addNode(CNode("4"),p1);
    addNode(CNode("5"),p1);
    CNode* p6 = addNode(CNode("6"));
    addEdge(p6,p6);
    addEdge(p6,p4);
}
MCGraph::~MCGraph(){
    qDeleteAll(m_lstNodes);
    qDeleteAll(m_lstEdges);

}

CNode* MCGraph::addNode(const CNode& node, CNode* preview) {
    CNode* nodeGraph =  new CNode(node); //construction par copie crée un node et mis dans le tas
    m_lstNodes << nodeGraph;
    if (preview!=NULL)
        addEdge(preview,nodeGraph);
    return nodeGraph;
}

CEdge* MCGraph::addEdge(CNode* nodeLeft, CNode* nodeRight){
    CEdge* edge = new CEdge(nodeLeft,nodeRight);
     m_lstEdges << edge;
     return edge;
}

void MCGraph::display() const{
    foreach (const CEdge* e, m_lstEdges) {
      ILogger::Trace() << e->toString();
    }
}
