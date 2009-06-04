/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphwidget.h"
#include "edge.h"
#include "node.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QWheelEvent>

#include <math.h>

#include "Debug/Exception.h"

GraphWidget::GraphWidget(const MCUrlInfo& urlInfo, int depth, QWidget* parent)
    : QGraphicsView(parent),
      timerId(0)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200, -200, 400, 400);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);

    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    //setWindowTitle(tr("Elastic Nodes"));

    createGraphHierarchy_(urlInfo, depth);
}

void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}

void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    /*case Qt::Key_Up:
        centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        centerNode->moveBy(20, 0);
        break;*/
    case Qt::Key_Plus:
        scaleView(qreal(1.2));
        break;
    case Qt::Key_Minus:
        scaleView(1 / qreal(1.2));
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        foreach (QGraphicsItem *item, scene()->items()) {
            if (qgraphicsitem_cast<Node *>(item))
                item->setPos(randPos_(width()), randPos_(height()));
        }
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach (Node *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (Node *node, nodes) {
        if (node->advance())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    /*Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
	painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
	painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersect(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);*/

    // Text
    /*QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
                    sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr("Click and drag the nodes around, and zoom with the mouse "
                       "wheel or the '+' and '-' keys"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);*/
}

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();

    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);

    /*qreal w = width() * scaleFactor;
    qreal h = height() * scaleFactor;

    scene()->setSceneRect(-w, -h, w, h);*/
}

void GraphWidget::resizeEvent(QResizeEvent* event) {
  qreal w = width();
  qreal h = height();

  scene()->setSceneRect(-w / 2, -h / 2, w, h);

  QGraphicsView::resizeEvent(event);
}

void GraphWidget::createNodesHierarchy_(const MCUrlInfo& parent, QMap<QByteArray, Node*>& nodes, int currentDepth, int maxDepth) {
  Assert(parent.isValid());

  if (currentDepth > maxDepth) {
    return;
  }

  Node* pNodeParent = nodes.value(parent.hash());
  if (pNodeParent == NULL) {
    pNodeParent = new Node(this);
    pNodeParent->setPos(randPos_(width()), randPos_(height()));

    nodes.insert(parent.hash(), pNodeParent);
    scene()->addItem(pNodeParent);
  }

  QListIterator<MCUrlInfo> it(parent.successors());
  while (it.hasNext()) {
    MCUrlInfo successor = it.next();

    // Previously analyzed
    Node* pNodeSuccessor = nodes.value(successor.hash());
    if (pNodeSuccessor != NULL) {
      scene()->addItem(new Edge(pNodeParent, pNodeSuccessor));
      continue;
    }
    // Create node
    else {
      pNodeSuccessor = new Node(this);
      pNodeSuccessor->setPos(pNodeParent->x() + randPos_(15), pNodeParent->y() + randPos_(15));
      nodes.insert(successor.hash(), pNodeSuccessor);

      scene()->addItem(pNodeSuccessor);
      scene()->addItem(new Edge(pNodeParent, pNodeSuccessor));

      createNodesHierarchy_(successor, nodes, currentDepth + 1, maxDepth);
    }
  }
}

void GraphWidget::createGraphHierarchy_(const MCUrlInfo& urlInfo, int depth) {
  QMap<QByteArray, Node*> nodes;

  Node* pNode = new Node(this);
  pNode->setPos(0, 0);

  nodes.insert(urlInfo.hash(), pNode);
  scene()->addItem(pNode);

  createNodesHierarchy_(urlInfo, nodes, 0, depth);
}

qreal GraphWidget::randPos_(qreal n) {
  return ((qreal)(qrand() % (int)n * 2)) - n;
}
