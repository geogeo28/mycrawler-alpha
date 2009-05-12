#ifndef CNODES_H
#define CNODES_H
#include <QString>


class CNode
{
public:
    CNode(const QString nameNode = QString(), int coorX = 0, int coorY = 0);
    ~CNode();
    QString nameNode()const { return m_sName; }
    int coordonneeX() const { return m_xNode; }
    int coordonneeY() const { return m_yNode; }
    void displayCoordonnees();
    QString toString() const { return nameNode(); }

private :
    QString m_sName;
    int m_xNode; //Coordonnee x du Node
    int m_yNode; // Coordonne y du Node

};

#endif // CNODES_H
