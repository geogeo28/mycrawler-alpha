#include <iostream>
#include <string>
#include "Node.h"

CNode::CNode(const QString nodeName,
               const int coorX,
               const int coorY)
  : m_sName(nodeName), m_xNode(coorX), m_yNode(coorY)
{

}
void CNode::displayCoordonnees(){
    std::cout << "x : " << this->coordonneeX() << "y : " << this->coordonneeY() << std::endl ;

}
CNode::~CNode(){


}
