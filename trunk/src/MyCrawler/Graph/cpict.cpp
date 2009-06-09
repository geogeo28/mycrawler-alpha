#include "cpict.h"

#include <QPainter>
#include <QMouseEvent>

#include <stdlib.h>
#include <math.h>
#include <time.h>

bool CPict::showLinks = true;

//initialisation du générateur de nbres aleatoires
inline void randomize(){
  srand((time_t) time(NULL));
}
//générateur de nbres aléatoires entre 0 et n-1
inline int random(int n){
  return rand()%n;
}

CNode::CNode():fils(NULL),suivant(NULL){}
CNode::~CNode()
{
    if(fils)
    {
        fils->freeNode();
        delete fils;
        fils=NULL;
    }
}

float CNode::updateSize(double &x,double &y)
{
    float tx,ty;

    if(this->suivant)
    {
        float sz(0.0),tmpf(0.0);

        CNode *nxt(this);
        /*
        while(nxt)
        {
            tmpf+=nxt->size;
            tx+=nxt->p.x()*nxt->size;
            ty+=nxt->p.y()*nxt->size;

            nxt=nxt->suivant;
        }
        if(tmpf>0.00000000001){x=tx/tmpf;y=ty/tmpf;}
        nxt=this;
        */

        while(nxt)
        {
            tx=nxt->p.x()-x;
            ty=nxt->p.y()-y;
            tmpf=sqrtf(tx*tx+ty*ty)+nxt->size;
            if(tmpf>sz)sz=tmpf;
            nxt=nxt->suivant;
        }

        return sz;
    }
    else
    {
            tx=this->p.x()-x;
            ty=this->p.y()-y;
            return (sqrtf(tx*tx+ty*ty)+MARGECOEFBONUS*(this->size));
    }
}

void CNode::updateSizeRec()
{
    CNode *nxt=this;
    while(nxt)
    {
        if(nxt->fils)nxt->size=MARGE+nxt->fils->updateSizeRecPriv(nxt->p.rx(),nxt->p.ry());
        nxt=nxt->suivant;
    }
}

/*
float CNode::attractForceRec(double &x,double &y,float gravity)
{
    float tx(0.0),ty(0.0),sz(0.0),tmpf(0.0);

    CNode *nxt(this);
    while(nxt)
    {
        tmpf+=nxt->size;
        tx+=nxt->p.x()*nxt->size;
        ty+=nxt->p.y()*nxt->size;

        nxt=nxt->suivant;
    }
    if(tmpf>0.00000000001){x=tx/tmpf;y=ty/tmpf;}

    nxt=this;
    while(nxt)
    {
        if(nxt->fils)nxt->size=MARGE+nxt->fils->attractForceRec(nxt->p.rx(),nxt->p.ry(),gravity);
        else reinterpret_cast<CFNode*>(nxt)->attractGrav(gravity);
        tx=nxt->p.x()-x;
        ty=nxt->p.y()-y;
        tmpf=sqrtf(tx*tx+ty*ty)+nxt->size;
        if(tmpf>sz)sz=tmpf;
        nxt=nxt->suivant;
    }

    return sz;
}
*/
/*
void CNode::attractForce(float gravity)
{
    CNode *nxt=this;
    while(nxt)
    {
        if(nxt->fils)nxt->size=MARGE+nxt->fils->attractForceRec(nxt->p.rx(),nxt->p.ry(),gravity);
        nxt=nxt->suivant;
    }
}
*/

bool CNode::checkIsIn(CNode *nd)
{
    CNode *nxt(this);
    while(nxt)
    {
        if((nxt==nd)||((nxt->fils) && (nxt->fils->checkIsIn(nd))))return true;
        nxt=nxt->suivant;
    }
    return false;
}

SList *CNode::checkIsInAndApplyGrav(CNode *listOfBro,float gravity,SList *listLinks)
{
    SList *listLinksPtr,*listLinksLastPtr;
    SList tmpNxt;
    float vx,vy,hypo,dx,dy;
    unsigned int nbTm;
    tmpNxt.suivant=listLinks;
    dx=dy=0.0;
    while(listOfBro)
    {
        if(listOfBro->fils)tmpNxt.suivant=checkIsInAndApplyGrav(listOfBro->fils,gravity,tmpNxt.suivant);
        listLinksPtr=tmpNxt.suivant;
        listLinksLastPtr=&tmpNxt;
        nbTm=0;
        while(listLinksPtr)
        {
            if(listLinksPtr->fnd==listOfBro)
            {
                ++nbTm;
                listLinksLastPtr->suivant=listLinksPtr->suivant;
                delete listLinksPtr;
                listLinksPtr=listLinksLastPtr->suivant;
            }
            else listLinksPtr=(listLinksLastPtr=listLinksPtr)->suivant;
        }
        if(nbTm>0)
        {
            vx=listOfBro->p.x()-p.x();
            vy=listOfBro->p.y()-p.y();
            hypo=sqrtf(vx*vx+vy*vy);
            if(hypo>0.000000001)
            {
                dx+=(vx/hypo)*gravity*nbTm;
                dy+=(vy/hypo)*gravity*nbTm;
            }
        }
        listOfBro=listOfBro->suivant;
    }

    p.rx()+=dx;
    p.ry()+=dy;

    return tmpNxt.suivant;
}

SList *CNode::applyGravity(CNode *listOfBro,float gravity,SList *listLinks,SList *fus)//fus est le première box de la liste !!!
{
    /*
    CNode *listOfBroPtr(listOfBro);
    QPointF svPf(p);
    while(listOfBroPtr)
    {
        if(listOfBroPtr!=this && listOfBroPtr->fils)
        {
            listLinks=checkIsInAndApplyGrav(listOfBroPtr->fils,gravity,listLinks);
            if(!listLinks)return NULL;
        }
        listOfBroPtr=listOfBroPtr->suivant;
    }
    SList *listLTmp;
    float vx,vy,hypo,dx(0.0),dy(0.0);
    while(listLinks)
    {
        listLinks=(listLTmp=listLinks)->suivant;
        listOfBroPtr=listOfBro;
        while(listOfBroPtr && listOfBroPtr!=listLTmp->fnd)listOfBroPtr=listOfBroPtr->suivant;
        if(listOfBroPtr)
        {
            vx=listOfBroPtr->p.x()-p.x();
            vy=listOfBroPtr->p.y()-p.y();
            hypo=sqrtf(vx*vx+vy*vy);
            if(hypo>0.000000001)
            {
                dx+=(vx/hypo)*gravity;
                dy+=(vy/hypo)*gravity;
            }
            delete listLTmp;
        }
        else
        {
            listLTmp->suivant=fus;
            fus=listLTmp;
        }
    }

    if(fils)fils->repercutDelta(p-svPf);
    return fus;
    */
    SList *listLTmp;
    float vx,vy,hypo,dx(0.0),dy(0.0);
    while(listLinks)
    {
        listLinks=(listLTmp=listLinks)->suivant;
        if(listOfBro->checkIsIn(listLTmp->fnd))
        {
            vx=listLTmp->fnd->p.x()-p.x();
            vy=listLTmp->fnd->p.y()-p.y();
            hypo=sqrtf(vx*vx+vy*vy);
            if(hypo>0.000000001)
            {
                dx+=(vx/hypo)*gravity;
                dy+=(vy/hypo)*gravity;
            }
            delete listLTmp;
        }
        else
        {
            listLTmp->suivant=fus;
            fus=listLTmp;
        }
    }
    p.rx()+=dx;
    p.ry()+=dy;
    if(fils)fils->repercutDelta(QPointF(dx,dy));
    return fus;
}

void CNode::calculNewSize(float ammort)
{
    CNode *ptr(this);

    do
    {
        if(ptr->fils)ptr->fils->calculNewSize(ammort);
        else reinterpret_cast<CFNode*>(ptr)->calculNewSize();
        ptr=ptr->suivant;
    }
    while(ptr);
}

void CFNode::calculNewSize(float ammort)
{
    SList *ptr(lstLink);
    unsigned int nbLnks(1);
    while(ptr){++nbLnks;ptr=ptr->suivant;}

    float scr(ammort*(size/nbLnks));
    ptr=lstLink;
    while(ptr)
    {
        if(!(ptr->fnd->fils))reinterpret_cast<CFNode*>(ptr->fnd)->newSize+=scr;
        ptr=ptr->suivant;
    }
}

void CNode::updateFSize()
{
    CNode *ptr(this);

    do
    {
        if(ptr->fils)ptr->fils->updateFSize();
        else
        {
            ptr->size=reinterpret_cast<CFNode*>(ptr)->newSize;
            reinterpret_cast<CFNode*>(ptr)->newSize=60.0;
        }
        ptr=ptr->suivant;
    }
    while(ptr);
}

CFNode *CNode::addNewNode(const QString &addr,bool *isNew)
{
    QStringList spltList;
    CFNode *res(NULL);//new CFNode()

    if(addr.left(7).compare("http://",Qt::CaseInsensitive)==0)spltList=(addr.right(addr.length()-7)).split('/',QString::SkipEmptyParts);
    else if(addr.left(8).compare("https://",Qt::CaseInsensitive)==0)spltList=(addr.right(addr.length()-8)).split('/',QString::SkipEmptyParts);
    else spltList=addr.split('/',QString::KeepEmptyParts);

    if(spltList.isEmpty() || spltList.length()>8)return NULL;

    QStringList::iterator it,itP(spltList.begin()),ed(spltList.end());

    CNode *cur(this),*tmpPtrNode,*oldCr(NULL);

    while(itP!=ed)
    {
        it=itP;
        ++itP;
        //printf("%s\n",it->toStdString().c_str());
        if(itP==ed)
        {
            if(cur)res=cur->getFNode(*it);
            if(!cur || !res)
            {
                res=new CFNode();
                res->name=*it;
                if(oldCr)res->p=oldCr->p+QPointF(rand()%80-40,rand()%80-40);
                else res->p=QPointF(rand()%80-40,rand()%80-40);
                res->size=60.0;
                if(cur)
                {
                    res->suivant=cur->suivant;
                    cur->suivant=res;
                }
                else
                {
                    oldCr->fils=res;
                    cur=res;
                }
                if(isNew)*isNew=true;
            }
            else if(isNew)*isNew=false;
        }
        else if(!(it->isEmpty()))
        {
            if(cur)tmpPtrNode=cur->getNode(*it);
            if(!cur || !tmpPtrNode)
            {
                tmpPtrNode=new CNode();
                tmpPtrNode->name=*it;
                if(oldCr)tmpPtrNode->p=oldCr->p+QPointF(rand()%80-40,rand()%80-40);
                else tmpPtrNode->p=QPointF(rand()%80-40,rand()%80-40);
                tmpPtrNode->size=65.0;
                if(cur)
                {
                    tmpPtrNode->suivant=cur->suivant;
                    cur->suivant=tmpPtrNode;
                }
                else
                {
                    oldCr->fils=tmpPtrNode;
                    cur=tmpPtrNode;
                }
            }
            oldCr=tmpPtrNode;
            cur=tmpPtrNode->fils;
        }
    }

    return res;
}

CNode *CNode::getNode(const QString &name)
{
    CNode *cur(this);
    while(cur && (!cur->fils || (name.compare(cur->name,Qt::CaseInsensitive)!=0)))cur=cur->suivant;
    return cur;
}

CFNode *CNode::getFNode(const QString &name)
{
    CNode *cur(this);
    while(cur && (cur->fils || (name.compare(cur->name,Qt::CaseInsensitive)!=0)))cur=cur->suivant;
    return reinterpret_cast<CFNode*>(cur);
}



SList *CFNode::applyGravity(CNode *listOfBro,float gravity,SList *fus)//fus est le première box de la liste !!!
{
    SList *listLTmp;
    float vx,vy,hypo;
    SList *listLinks(lstLink);
    while(listLinks)
    {
        if(listOfBro->checkIsIn(listLinks->fnd))
        {
            vx=listLinks->fnd->p.x()-p.x();
            vy=listLinks->fnd->p.y()-p.y();
            hypo=sqrtf(vx*vx+vy*vy);
            if(hypo>0.000000001)
            {
                p.rx()+=(vx/hypo)*gravity;
                p.ry()+=(vy/hypo)*gravity;
            }
        }
        else
        {
            listLTmp = new SList;
            listLTmp->fnd=listLinks->fnd;
            listLTmp->suivant=fus;
            fus=listLTmp;
        }
        listLinks=listLinks->suivant;
    }
    return fus;
}

SList *CNode::attractForceRec2(float gravity,const QPointF &pereP)
{
    SList *listPtr(NULL),*listTmp;

    QPointF moy(0.0,0.0);
    /*
    QPointF tmp;
    float hypo;
    */
    //double nb(0.0);
    int nb(0);

    CNode *nxt(this);
    while(nxt)
    {
        if(nxt->fils)
        {
            listTmp=nxt->fils->attractForceRec2(gravity,nxt->p);
            listPtr=nxt->applyGravity(this,gravity,listTmp,listPtr);
        }
        else listPtr=reinterpret_cast<CFNode*>(nxt)->applyGravity(this,gravity,listPtr);

        /*
        moy-=nxt->p*nxt->size;
        nb+=nxt->size;
        */
        /*
        tmp=nxt->p-pereP;
        hypo=tmp.x()*tmp.x()+tmp.y()*tmp.y();
        if(hypo>0.00000000000000001)moy-=(tmp/sqrtf(hypo))*nxt->size;
        */

        moy-=nxt->p;
        ++nb;

        nxt=nxt->suivant;
    }

    if(nb>0.0)
    {
        moy/=(double)nb;
        moy+=pereP;
        this->repercutDelta(moy);
    }

    return listPtr;
}

void CNode::attractForce2(float gravity)
{
    SList *listPtr;

    QPointF moy(0.0,0.0);
    int nb(0);

    CNode *nxt(this);
    while(nxt)
    {
        if(nxt->fils)
        {
            listPtr=nxt->fils->attractForceRec2(gravity,nxt->p);
            listPtr=nxt->applyGravity(this,gravity,listPtr,NULL);
            if(listPtr)SList::freeSList(listPtr);
        }
        else
        {
            listPtr=reinterpret_cast<CFNode*>(nxt)->applyGravity(this,gravity,NULL);
            if(listPtr)SList::freeSList(listPtr);
        }

        moy-=nxt->p;
        ++nb;

        nxt=nxt->suivant;
    }

    if(nb>0.0)
    {
        moy/=(double)nb;
        this->repercutDelta(moy);
    }
}

float CNode::updateSizeRecPriv(double &x,double &y)
{
    float tx,ty;

    if(this->suivant)
    {
        float sz(0.0),tmpf(0.0);

        CNode *nxt(this);
        /*
        while(nxt)
        {
            tmpf+=nxt->size;
            tx+=nxt->p.x()*nxt->size;
            ty+=nxt->p.y()*nxt->size;

            nxt=nxt->suivant;
        }
        if(tmpf>0.00000000001){x=tx/tmpf;y=ty/tmpf;}
        nxt=this;
        */

        while(nxt)
        {
            if(nxt->fils)nxt->size=MARGE+nxt->fils->updateSizeRecPriv(nxt->p.rx(),nxt->p.ry());
            tx=nxt->p.x()-x;
            ty=nxt->p.y()-y;
            tmpf=sqrtf(tx*tx+ty*ty)+nxt->size;
            if(tmpf>sz)sz=tmpf;
            nxt=nxt->suivant;
        }
        return sz;
    }
    else
    {
            if(this->fils)this->size=MARGE+this->fils->updateSizeRecPriv(this->p.rx(),this->p.ry());
            tx=this->p.x()-x;
            ty=this->p.y()-y;
            return (sqrtf(tx*tx+ty*ty)+MARGECOEFBONUS*(this->size));
    }
}

void CNode::freeNode()
{
    CNode *nxt=suivant;
    CNode *tmp;
    while(nxt)
    {
        tmp=nxt;
        nxt=nxt->suivant;
        //if(tmp->fils){tmp->fils->freeNode();delete tmp->fils;tmp->fils=NULL;}
        delete tmp;
    }
    suivant=NULL;
}

void CNode::draw(float oldPx,float oldPy,GLint objOpGlID,matrix &mat, float scl)
{
    //srand((time_t)rand());
    //QBrush qb(QColor(rand()%256,rand()%256,rand()%256,255),Qt::SolidPattern);

    float r,g,b;
    srand((time_t) this);
    r=(rand()%256)/255.0;
    g=(rand()%256)/255.0;
    b=(rand()%256)/255.0;
    CNode *nxt(this);
    while(nxt)
    {
        //pnt->setBrush(qb);
        glColor3f(r,g,b);
        mat.c.c14=oldPx+(nxt->p.x())*scl;
        mat.c.c24=oldPy+(nxt->p.y())*scl;
        mat.c.c11=mat.c.c22=nxt->size*scl;
        //pnt->drawEllipse(tmpP,nxt->size*scl,nxt->size*scl);
        glLoadIdentity();
        glMultMatrixf(mat.m);
        glCallList(objOpGlID);
        if(nxt->fils)nxt->fils->draw(oldPx,oldPy,objOpGlID,mat,scl);
        nxt=nxt->suivant;
    }
}

void CNode::setRandLink(CNode *root)
{
    CNode *nxt(this);
    while(nxt)
    {
        if(nxt->fils)nxt->fils->setRandLink(root);
                else if(!reinterpret_cast<CFNode*>(nxt)->lstLink)reinterpret_cast<CFNode*>(nxt)->lstLink=makeAleatList(NBCNMAXRND,root);
        nxt=nxt->suivant;
    }
}

void CNode::drawLinks(QPointF &p,float scl)
{
    if (CPict::showLinks == false) {
      return;
    }

    //glColor3f(0.0,0.0,0.0);
    CNode *nxt(this);
    while(nxt)
    {
        if(nxt->fils)nxt->fils->drawLinks(p,scl);
        else reinterpret_cast<CFNode*>(nxt)->comDrawLinks(p,scl);
        nxt=nxt->suivant;
    }
}

CNode *CNode::selectedNode(QPointF const &ps)
{
    CNode *nxt(this);
    CNode *res(NULL);
    CNode *tmp;

    float opx(ps.x()),opy(ps.y());
    float px,py;

    while(nxt)
    {
        px=opx-nxt->p.x();
        py=opy-nxt->p.y();

        if((px*px+py*py)<= nxt->size*nxt->size)
        {
            if(nxt->fils)
            {
                tmp=nxt->fils->selectedNode(ps);//-nxt->p
                if(tmp)res=tmp;
                else res=nxt;
            }
            else res=nxt;
        }
        nxt=nxt->suivant;
    }

    return res;
}

void CNode::repercutDelta(const QPointF &dlt)
{
    CNode *nxt(this);
    while(nxt)
    {
        if(nxt->fils)nxt->fils->repercutDelta(dlt);
        nxt->p+=dlt;
        nxt=nxt->suivant;
    }
}

void CNode::repercutDeltaX(float dltX)
{
    CNode *nxt(this);
    while(nxt)
    {
        if(nxt->fils)nxt->fils->repercutDeltaX(dltX);
        nxt->p.rx()+=dltX;
        nxt=nxt->suivant;
    }
}

void CNode::repercutDeltaY(float dltY)
{
    CNode *nxt(this);
    while(nxt)
    {
        if(nxt->fils)nxt->fils->repercutDeltaY(dltY);
        nxt->p.ry()+=dltY;
        nxt=nxt->suivant;
    }
}

void CNode::reppel()
{
    CNode *nxtI(this),*nxtJ;

    while(nxtI)
    {
        if(nxtI->fils)
        {
            nxtI->fils->reppel();
            nxtI->size=MARGE+nxtI->fils->updateSize(nxtI->p.rx(),nxtI->p.ry());
        }
        nxtI=nxtI->suivant;
    }

    nxtI=this;
    qreal ix,iy,ir;
    qreal dx,dy;
    qreal dist,sumR;
    QPointF tmpDlt;

    while(nxtI)
    {
        ix=nxtI->p.x();
        iy=nxtI->p.y();
        ir=nxtI->size;
        nxtJ=nxtI->suivant;
        while(nxtJ)
        {
            dx=nxtJ->p.x()-ix;
            dy=nxtJ->p.y()-iy;
            dist=dx*dx+dy*dy;
            if(dist<0.000001)
            {
                sumR=(ir+nxtJ->size+MARGEREPPELMAX)*0.5;
                nxtJ->p.rx()+=sumR;
                if(nxtJ->fils)nxtJ->fils->repercutDeltaX(sumR);
                ix-=sumR;
            }
            else
            {
                sumR=ir+nxtJ->size+MARGEREPPELMIN;
                if(dist<sumR*sumR)
                {
                    sumR+=(MARGEREPPELMAX-MARGEREPPELMIN);
                    dist=sqrt(dist);
                    sumR-=dist;
                    sumR/=dist;
                    sumR*=0.6;
                    dx*=sumR;dy*=sumR;
                    //nxtJ->p.rx()+=dx;
                    //nxtJ->p.ry()+=dy;
                    tmpDlt.setX(dx);
                    tmpDlt.setY(dy);
                    nxtJ->p+=tmpDlt;
                    ix-=dx;
                    iy-=dy;
                    if(nxtJ->fils)nxtJ->fils->repercutDelta(tmpDlt);
                }
            }
            nxtJ=nxtJ->suivant;
        }
        if(nxtI->p.x()!=ix || nxtI->p.y()!=iy)
        {
            if(nxtI->fils)
            {
                tmpDlt.setX(ix-nxtI->p.x());
                tmpDlt.setY(iy-nxtI->p.y());
                nxtI->fils->repercutDelta(tmpDlt);
            }
            nxtI->p.setX(ix);
            nxtI->p.setY(iy);
        }
        nxtI=nxtI->suivant;
    }
}

void CNode::reppel(float repForce,float degrad)
{
    CNode *nxtI(this),*nxtJ;

    while(nxtI)
    {
        if(nxtI->fils)
        {
            nxtI->fils->reppel(repForce*degrad,degrad);
            nxtI->size=MARGE+nxtI->fils->updateSize(nxtI->p.rx(),nxtI->p.ry());
        }
        nxtI=nxtI->suivant;
    }

    nxtI=this;
    qreal ix,iy,ir;
    qreal dx,dy;
    qreal dist,sumR;
    QPointF tmpDlt;

    while(nxtI)
    {
        ix=nxtI->p.x();
        iy=nxtI->p.y();
        ir=nxtI->size;
        nxtJ=nxtI->suivant;
        while(nxtJ)
        {
            dx=nxtJ->p.x()-ix;
            dy=nxtJ->p.y()-iy;
            dist=dx*dx+dy*dy;
            if(dist<0.000001)
            {
                sumR=(ir+nxtJ->size+MARGEREPPELMAX)*0.5;
                if(rand()%2)
                {
                    nxtJ->p.rx()+=sumR;
                    if(nxtJ->fils)nxtJ->fils->repercutDeltaX(sumR);
                    ix-=sumR;
                }
                else
                {
                    nxtJ->p.ry()+=sumR;
                    if(nxtJ->fils)nxtJ->fils->repercutDeltaY(sumR);
                    iy-=sumR;
                }
            }
            else
            {
                sumR=ir+nxtJ->size+MARGEREPPELMIN;

                dist=sqrt(dist);

                if(dist<sumR)
                {
                    qreal tSumR(sumR+(MARGEREPPELMAX-MARGEREPPELMIN));
                    tSumR-=dist;
                    tSumR/=dist;
                    if(tSumR<0.01)tSumR=0.01;
                    tSumR*=0.5;
                    tmpDlt.setX(dx*tSumR);
                    tmpDlt.setY(dy*tSumR);
                    nxtJ->p+=tmpDlt;
                    ix-=tmpDlt.x();
                    iy-=tmpDlt.y();
                    if(nxtJ->fils)nxtJ->fils->repercutDelta(tmpDlt);
                }

                {
                    //sumR+=(MARGEREPPELMAX-MARGEREPPELMIN);
                    if((dist-sumR)<0.01)dist=sumR+0.01;
                    dist=(repForce/(dist*(dist-sumR)))*sumR;
                    if(dist<0.0)dist=0.0;
                    dx*=dist;dy*=dist;
                    if(dx<-0.0001 || 0.0001<dx || dy<-0.0001 || 0.0001<dy)
                    {
                        if(dx>10.0)dx=10.0;
                        else if(dx<-10.0)dx=-10.0;
                        if(dy>10.0)dy=10.0;
                        else if(dy<-10.0)dy=-10.0;
                        
                        ix-=dx;
                        iy-=dy;
                        tmpDlt.setX(dx);
                        tmpDlt.setY(dy);
                        nxtJ->p+=tmpDlt;
                        nxtJ->repercutDelta(tmpDlt);
                    }
                }
            }
            nxtJ=nxtJ->suivant;
        }
        if(nxtI->p.x()!=ix || nxtI->p.y()!=iy)
        {
            if(nxtI->fils)
            {
                tmpDlt.setX(ix-nxtI->p.x());
                tmpDlt.setY(iy-nxtI->p.y());
                nxtI->fils->repercutDelta(tmpDlt);
            }
            nxtI->p.setX(ix);
            nxtI->p.setY(iy);
        }
        nxtI=nxtI->suivant;
    }
}

/*
void CNode::centerForceRec(float str,const QPointF &pereP)
{
    QPointF frc;

    CNode *nxt(this);
    while(nxt)
    {
        frc=str*(pereP-nxt->p);
        nxt->p+=frc;
        if(nxt->fils)
        {
            nxt->fils->repercutDelta(frc);
            nxt->fils->centerForceRec(str,nxt->p);
        }
        nxt=nxt->suivant;
    }
}

void CNode::centerForce(float strBase,float str)
{
    QPointF frc;

    CNode *nxt(this);
    while(nxt)
    {
        frc=-strBase*(nxt->p);
        nxt->p+=frc;
        if(nxt->fils)
        {
            nxt->fils->repercutDelta(frc);
            nxt->fils->centerForceRec(str,nxt->p);
        }
        nxt=nxt->suivant;
    }
}
*/

void CNode::centerForceRec(float strCon,float strCen,const QPointF &pereP)
{
    CNode *nxt(this->suivant);
    QPointF cPt;

    {
        qreal fMinX(p.x()-size),fMinY(p.y()-size);
        qreal fMaxX(p.x()+size),fMaxY(p.y()+size);
        qreal tx,ty;
        float tSize;

        while(nxt)
        {
            tSize=nxt->size;
            tx=nxt->p.x()-tSize;
            ty=nxt->p.y()-tSize;
            if(tx<fMinX)fMinX=tx;
            if(ty<fMinY)fMinY=ty;
            tx=nxt->p.x()+tSize;
            ty=nxt->p.y()+tSize;
            if(tx>fMaxX)fMaxX=tx;
            if(ty>fMaxY)fMaxY=ty;
            nxt=nxt->suivant;
        }

        cPt.setX((fMinX+fMaxX)*0.5);
        cPt.setY((fMinY+fMaxY)*0.5);
    }
    QPointF fCentri((pereP-cPt)*strCen);

    QPointF frc;

    nxt=this;
    while(nxt)
    {
        frc=fCentri+strCon*(cPt-nxt->p);
        nxt->p+=frc;
        if(nxt->fils)
        {
            nxt->fils->repercutDelta(frc);
            nxt->fils->centerForceRec(strCon,strCen,nxt->p);
        }
        nxt=nxt->suivant;
    }
}

void CNode::centerForce(float strBaseCon,float strCon,float strCen)
{
    CNode *nxt(this->suivant);
    QPointF cPt;

    {
        qreal fMinX(p.x()-size),fMinY(p.y()-size);
        qreal fMaxX(p.x()+size),fMaxY(p.y()+size);
        qreal tx,ty;
        float tSize;

        while(nxt)
        {
            tSize=nxt->size;
            tx=nxt->p.x()-tSize;
            ty=nxt->p.y()-tSize;
            if(tx<fMinX)fMinX=tx;
            if(ty<fMinY)fMinY=ty;
            tx=nxt->p.x()+tSize;
            ty=nxt->p.y()+tSize;
            if(tx>fMaxX)fMaxX=tx;
            if(ty>fMaxY)fMaxY=ty;
            nxt=nxt->suivant;
        }

        cPt.setX((fMinX+fMaxX)*0.5);
        cPt.setY((fMinY+fMaxY)*0.5);
    }
    //QPointF fCentri(-cPt);

    QPointF frc;

    nxt=this;
    while(nxt)
    {
        frc=strBaseCon*(cPt-nxt->p)-cPt;
        nxt->p+=frc;
        if(nxt->fils)
        {
            nxt->fils->repercutDelta(frc);
            nxt->fils->centerForceRec(strCon,strCen,nxt->p);
        }
        nxt=nxt->suivant;
    }
}

SList *CNode::makeAleatList(unsigned int numMax,CNode *root)
{
	numMax=rand()%numMax;
	if(numMax==0)return NULL;

	SList *res,*last;
	res=last=new SList;
	last->fnd=root->getRandomNode();
	last->suivant=NULL;
	--numMax;
	
	while(numMax>0)
	{
		last->suivant=new SList;
		last=last->suivant;
		last->fnd=root->getRandomNode();
		last->suivant=NULL;
		--numMax;
	}
	
	return res;
}

CFNode *CNode::getRandomNode()
{
	CNode *ptrT(this);
	unsigned int count(0);
	
	while(ptrT)
	{
		++count;
        ptrT=ptrT->suivant;
	}
	
	count=rand()%count;
	ptrT=this;
	while(count>0)
	{
		--count;
        ptrT=ptrT->suivant;
	}
	
    if(ptrT->fils)return (ptrT->fils->getRandomNode());
	else return reinterpret_cast<CFNode*>(ptrT);
}

void SList::freeSList(SList *listPtr)
{
    SList *tmp;
    while(listPtr)
    {
        listPtr=(tmp=listPtr)->suivant;
        delete tmp;
    }
}

CFNode::CFNode():CNode(),newSize(60.0),lstLink(NULL){}
CFNode::~CFNode()
{
    SList *tmp;
    while(lstLink)
    {
        tmp=lstLink;
        lstLink=lstLink->suivant;
        delete tmp;
    }
}

void CFNode::comDrawLinks(QPointF &p,float scl)
{
    SList *ptrL(lstLink);
    while(ptrL)
    {
        //srand((time_t)this);
        //glColor3f((rand()%256)/255.0,(rand()%256)/255.0,(rand()%256)/255.0);
        glColor3f(1.0,0.0,0.0);
        glVertex2f(p.x()+this->p.x()*scl,p.y()+this->p.y()*scl);
        //srand((time_t)ptrL->fnd);
        //glColor3f((rand()%256)/255.0,(rand()%256)/255.0,(rand()%256)/255.0);
        glColor3f(0.0,0.0,1.0);
        glVertex2f(p.x()+ptrL->fnd->p.x()*scl,p.y()+ptrL->fnd->p.y()*scl);
        ptrL=ptrL->suivant;
    }
}

bool CFNode::addLink(CFNode *lk)
{
    SList *ptrL(lstLink);
    while(ptrL && ptrL->fnd!=lk)ptrL=ptrL->suivant;
    if(ptrL)return false;

    SList *ptrTT(new SList);
    ptrTT->fnd=lk;
    ptrTT->suivant=lstLink;
    lstLink=ptrTT;

    return true;
}

/*
void CFNode::attractGrav(float gravity)
{
    SList *ptrLst(lstLink);

    float px(p.x());
    float py(p.y());
    float nx,ny,hypo;

    while(ptrLst)
    {
        if(ptrLst->fnd)
        {
            nx=ptrLst->fnd->p.x()-px;
            ny=ptrLst->fnd->p.x()-py;
            hypo=sqrtf(nx*nx+ny*ny);
            if(hypo>0.0000001)
            {
                px+=(nx/hypo)*gravity;
                py+=(ny/hypo)*gravity;
            }
        }
        ptrLst=ptrLst->suivant;
    }

    p.setX(px);
    p.setY(py);
}
*/
CPict::CPict(QWidget *parent):QGLWidget(QGLFormat(QGL::SampleBuffers),parent),root(NULL),p(250.0,250.0),selectedNode(NULL),fllowedNode(NULL),scl(0.5)
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setAutoFillBackground(false);
    setMouseTracking(true);
}
CPict::~CPict(){if(root){root->freeNode();delete root;root=NULL;}}

CNode *CPict::initAleatGraphRec(float px,float py,int prof)
{
    if(prof<0)return NULL;

    int nbSuivant;
    nbSuivant=1+prof/2+rand()%(prof+1);
	
    CNode *res;
    CNode *last;
    last=res=((prof==0)?new CFNode:new CNode);
    last->p.setX(px-10+rand()%21);
    last->p.setY(py-10+rand()%21);
    last->fils=initAleatGraphRec(last->p.x(),last->p.y(),prof-1);
    last->size=5+rand()%10;
    last->suivant=NULL;
    while(nbSuivant>0)
    {
        last->suivant=((prof==0)?new CFNode:new CNode);
        last=last->suivant;
        last->p.setX(px-10+rand()%21);
        last->p.setY(py-10+rand()%21);
        last->fils=initAleatGraphRec(last->p.x(),last->p.y(),prof-1);
        last->size=5+rand()%10;
        last->suivant=NULL;

        --nbSuivant;
    }

    return res;
}

void CPict::initAleatGraph()
{
    randomize();
    if(root){root->freeNode();delete root;}
    root=initAleatGraphRec(0.0,0.0,3);
    root->setRandLink(root);
    root->updateSizeRec();
}

void CPict::paintEvent(QPaintEvent *pev){updateGL();}

void CPict::mousePressEvent(QMouseEvent *event)
{
    oldMous=event->posF();
    Qt::MouseButtons msb(event->buttons());

    if(msb & Qt::LeftButton)
    {
        selectedNode=root->selectedNode((oldMous-p)/scl);
    }
    else if(msb & Qt::RightButton)
    {
        //zoomSvP=oldMous;
        fllowedNode=NULL;
    }
    else if(msb & Qt::MidButton)
    {
        fllowedNode=root->selectedNode((oldMous-p)/scl);
    }
    QGLWidget::mouseReleaseEvent(event);
}

void CPict::mouseMoveEvent(QMouseEvent * event)
{
    Qt::MouseButtons msb(event->buttons());
    zoomSvP=event->posF();

    if(msb)
    {
        QPointF pt(event->posF()-oldMous);
        oldMous=event->posF();

        if(msb & Qt::LeftButton)
        {
            if(selectedNode)
            {
                pt/=scl;
                selectedNode->p+=pt;
                if(selectedNode->fils)selectedNode->fils->repercutDelta(pt);
                root->updateSizeRec();
                /*
                QPointF dlt((p+event->posF())/scl-(selectedNode->p));
                selectedNode->p+=dlt;
                if(selectedNode->fils)selectedNode->fils->repercutDelta(dlt);
                */
            }
            else
            {
                fllowedNode=NULL;
                p+=pt;
            }
        }

        /*
        if(msb & Qt::MidButton)
        {
        }
        */

        if(msb & Qt::RightButton)
        {
            /*
            float oldScl(scl);
            scl-=pt.y()*0.01*scl;if(scl==0.0)scl=0.001;
            //p=(p-QPointF(width()/2,height()/2))*(scl/oldScl)+QPointF(width()/2,height()/2);
            p=(p-zoomSvP)*(scl/oldScl)+zoomSvP;
            */
            p+=pt;
        }

        if((msb & Qt::LeftButton && selectedNode))calculBase();
        repaint();
    }
    QGLWidget::mouseReleaseEvent(event);
}

void CPict::mouseReleaseEvent(QMouseEvent *event){QGLWidget::mouseReleaseEvent(event);}

void CPict::mouseDoubleClickEvent(QMouseEvent* event) {
  CPict::showLinks = !CPict::showLinks;
  QGLWidget::mouseDoubleClickEvent(event);
}

void CPict::wheelEvent(QWheelEvent *event)
{
    if(fllowedNode)
    {
        scl+=(event->delta())*0.002*scl;if(scl==0.0)scl=0.001;
        p=QPointF(width()*0.5,height()*0.5)-fllowedNode->p*scl;
    }
    else
    {
        QPointF zoomSvP(event->pos());
        float oldScl(scl);
        scl+=(event->delta())*0.002*scl;if(scl==0.0)scl=0.001;
        //p=(p-QPointF(width()/2,height()/2))*(scl/oldScl)+QPointF(width()/2,height()/2);
        p=(p-zoomSvP)*(scl/oldScl)+zoomSvP;
    }

    repaint();
            
    QGLWidget::wheelEvent(event);
}

void CPict::initializeGL ()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(0.0,0.0,0.0);

    //glDepthFunc(GL_LESS);
    //glEnable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
    glDisable(GL_FOG);

    glDisable(GL_LIGHTING);

    objOpGlID=glGenLists(1);
    glNewList(objOpGlID, GL_COMPILE);
    float theta;
    glBegin(GL_TRIANGLE_FAN);
    for(theta=0;theta<2*M_PI;theta+=M_PI/16)glVertex2f(cosf(theta),sinf(theta));
    glEnd();
    glEndList();
}
void CPict::calculBase()
{
    if(root)
    {
        root->reppel(0.0,0.0);
        root->updateSizeRec();

        if(fllowedNode)p=QPointF(width()*0.5,height()*0.5)-fllowedNode->p*scl;
    }
}
void CPict::calculate()
{
    if(root)
  {
        //root->attractForce(10.0);
        root->calculNewSize();
        root->updateFSize();
        root->attractForce2(0.8);
        root->centerForce(0.001,0.1,0.5);
        root->reppel(20.0,0.95);
        root->updateSizeRec();

        if(fllowedNode)p=QPointF(width()*0.5,height()*0.5)-fllowedNode->p*scl;
    }
}

void CPict::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glLoadIdentity();
    //glTranslatef(0.0,0.0,0.0);

    matrix mat;
    mat.c.c44=mat.c.c33=1.0;
    mat.c.c12=mat.c.c13=mat.c.c21=mat.c.c23=mat.c.c31=mat.c.c32=mat.c.c34=mat.c.c41=mat.c.c42=mat.c.c43=0.0;
    //mat.c.c11=mat.c.c22=scl;
    //mat.c.c14=p.x();
    //mat.c.c24=p.y();
    if(root)root->draw(p.x(),p.y(),objOpGlID,mat,scl);
    glLoadIdentity();
    glBegin(GL_LINES);
    if(root)root->drawLinks(p,scl);
    glEnd();

    if(root)
    {
        CNode *snd(root->selectedNode((zoomSvP-p)/scl));
        if(snd)
        {
            QPainter p(this);
            p.drawText(zoomSvP,snd->name);
            p.end();
        }
    }

}
void CPict::resizeGL(int width,int height)
{
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(1.0,1.0,-1.0,1.0,-1.0,1.0);
    //glOrtho(0.0,width,height,0.0,0.1,-1.0);
    gluOrtho2D(0.0,width,height,0.0);
    glMatrixMode(GL_MODELVIEW);
    //updateGL();
}

CFNode *CPict::addNewNode(const QString &addr,bool *isNew)
{
    CFNode *res(NULL);

    if(root)res=root->addNewNode(addr,isNew);
    else
    {
        CFNode *tmpPtrRt(new CFNode());
        tmpPtrRt->name="/";
        //tmpPtrRt->size=30;
        res=tmpPtrRt->addNewNode(addr,isNew);
        root=tmpPtrRt->suivant;
        delete tmpPtrRt;
    }
    return res;
}
