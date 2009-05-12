#ifndef CEDGE_H
#define CEDGE_H
#include "Node.h"

class CEdge
{
public:
    CEdge(CNode* nodeLeft, CNode* nodeRight);

    QString toString() const { return p_nodeLeft->toString() + " " + p_nodeRight->toString(); }

 private :
    CNode* p_nodeLeft;
    CNode* p_nodeRight;

};

#endif // CEDGE_H
