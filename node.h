#ifndef NODE_H
#define NODE_H

// 화면에 그려지는 라우터 노드(원) 그래픽 아이템 클래스

#include <QGraphicsEllipseItem>
#include <QPen>
#include "simulator.h"

class Node : public QGraphicsEllipseItem
{
public:
    Node(qreal x, qreal y, qreal r, int idx, MainWindow *win)
        : QGraphicsEllipseItem(-r, -r, r * 2, r * 2)
        , index(idx)
        , mainWindow(win)
    {
        // 중심이 (x, y)가 되도록 배치하고 흰색 배경에 검은색 테두리 설정
        setPos(x, y);
        setBrush(Qt::white);
        setPen(QPen(Qt::black, 2));
    }

protected:
    // 노드를 마우스로 클릭했을 때 메인 윈도우로 노드 번호(index) 전달
    void mousePressEvent(QGraphicsSceneMouseEvent *) override
    {
        mainWindow->on_node_clicked(index);
    }

private:
    int index;              // 노드 인덱스 (0: A, 1: B...)
    MainWindow *mainWindow; // 클릭 이벤트를 전달할 메인 윈도우 포인터
};

#endif // NODE_H