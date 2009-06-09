#ifndef CPICT_H
#define CPICT_H

#include <QGLWidget>
#include <QList>

#include <QMutex>

#define MARGE 10.0
#define MARGEBONUS 200.0
#define MARGECOEFBONUS 1.25
#define MARGEREPPELMIN 2.0
#define MARGEREPPELMAX 4.0
#define NBCNMAXRND 3

//Type matrice qui peut être envoyée à openGL
struct Cmatrix
{
 float c11,c21,c31,c41;
 float c12,c22,c32,c42;
 float c13,c23,c33,c43;
 float c14,c24,c34,c44;
};

//Ce type de matrice est utilisable par OpenGL
union matrix
{
 Cmatrix c;
 float m[16];
};

class QPainter;
class CFNode;

struct SList
{
    CFNode *fnd;
    SList *suivant;

    static void freeSList(SList *listPtr);
};

class CNode
{
    friend class CFNode;
    public:
        QPointF p;
        float size;
        CNode *fils;
        CNode *suivant;

        QString name;

    private:
        float updateSizeRecPriv(double &x,double &y);
        //float attractForceRec(double &x,double &y,float gravity);
        SList *applyGravity(CNode *listOfBro,float gravity,SList *listLinks,SList *fus);//fus est le première box de la liste !!!
        SList *attractForceRec2(float gravity,const QPointF &pereP);
        void centerForceRec(float strCon,float strCen,const QPointF &pereP);

        bool checkIsIn(CNode *nd);
        SList *checkIsInAndApplyGrav(CNode *listOfBro,float gravity,SList *listLinks);

    public:
        CNode();
        virtual ~CNode();

        float updateSize(double &x,double &y);
        void updateSizeRec();

        void freeNode();

        void draw(float oldPx,float oldPy,GLint objOpGlID,matrix &mat, float scl);
        void drawLinks(QPointF &p,float scl);

        CNode *selectedNode(QPointF const &ps);

        void reppel();
        void reppel(float repForce,float degrad);

        void repercutDelta(const QPointF &dlt);
        void repercutDeltaX(float dltX);
        void repercutDeltaY(float dltY);

        CFNode *getRandomNode();

        void setRandLink(CNode *root);

        static SList *makeAleatList(unsigned int numMax,CNode *root);

        //void attractForce(float gravity);
        void attractForce2(float gravity);
        void centerForce(float strBaseCon,float strCon,float strCen);

        CFNode *addNewNode(const QString &addr,bool *isNew=NULL);
        CNode *getNode(const QString &name);
        CFNode *getFNode(const QString &name);

        void calculNewSize(float ammort=1.0);
        void updateFSize();
};

class CFNode : public CNode
{
    friend class CNode;

    private:
        //void attractGrav(float gravity);
        SList *applyGravity(CNode *listOfBro,float gravity,SList *fus);//fus est le première box de la liste !!!

    private:
        float newSize;

    public:
        SList *lstLink;

    public:
        CFNode();
        virtual ~CFNode();

        virtual void comDrawLinks(QPointF &p,float scl);
        bool addLink(CFNode *lk);

        void calculNewSize(float ammort=1.0);
};

class QMouseEvent;
class QWheelEvent;
class CPict : public QGLWidget
{
    Q_OBJECT

private:
    CNode *root;
    QPointF p;
    QPointF oldMous;
    QPointF zoomSvP;
    CNode *selectedNode;
    CNode *fllowedNode;
    float scl;

    GLint objOpGlID;

    CNode *initAleatGraphRec(float px=0.0,float py=0.0,int prof=3);

    void calculBase();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent *event);

protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int width,int height);

public slots:
    void calculate();

public:
    CPict(QWidget *parent=NULL);
    virtual ~CPict();

    void paintEvent(QPaintEvent *pnt);

    void initAleatGraph();

    CFNode *addNewNode(const QString &addr,bool *isNew=NULL);

    static bool showLinks;
};

Q_DECLARE_METATYPE(CFNode*);

#endif // CPICT_H
