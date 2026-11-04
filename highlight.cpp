// 화면 그리기: 원래 지도 위에, src 노드가 아는 길을 겹쳐 그리고 이번 라운드에 바뀐 내용을 표시 (src = -1 이면 구성 화면)

#include "simulator.h"
#include "./ui_simulator.h"
#include "node.h"
#include <QPen>
#include <QLineF>
#include <QPolygonF>
#include <QPainterPath>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>

// 노드를 노란색으로 칠하고, 노드 위에 작은 이름표("시점" / "새로 앎" / "값 바뀜" / "끊김")를 붙임
void MainWindow::mark_node(int idx, QString text)
{
    nodeItems[idx]->setBrush(Qt::yellow);

    QGraphicsTextItem *tag = scene->addText(text, scene_font(13, true));
    tag->setPos(nodeItems[idx]->x() - tag->boundingRect().width() / 2, nodeItems[idx]->y() - NODE_R - tag->boundingRect().height());
    tag->setZValue(3);
}

// 바뀐 노드 옆에 "왜 이 값인지"를 보여주는 결과 배지를 붙임
void MainWindow::add_badge(int idx, QString html)
{
    QGraphicsTextItem *text = scene->addText("", scene_font(14));
    text->setHtml(html);
    text->setZValue(13);

    qreal w = text->boundingRect().width();
    qreal h = text->boundingRect().height();

    // 우 → 좌 → 아래 → 위 순서로, 다른 노드와 안 겹치는 자리를 찾아서 놓음
    QPointF c = nodeItems[idx]->pos();
    qreal gap = NODE_R + 6;
    QPointF cand[4] = { QPointF(c.x() + gap, c.y() - h / 2 - 8), QPointF(c.x() - gap - w, c.y() - h / 2 - 8),
                        QPointF(c.x() - w / 2, c.y() + gap), QPointF(c.x() - w / 2, c.y() - gap - h) };
    QPointF best = cand[0];
    for (int k = 3; k >= 0; k--) {
        QRectF r(cand[k], QSizeF(w, h));
        bool ok = QRectF(5, 5, SCENE_W - 10, SCENE_H - 10).contains(r);
        for (int n = 0; n < current_n; n++) {
            if (n != idx && nodeItems[n]->sceneBoundingRect().intersects(r)) ok = false;
        }
        if (ok) best = cand[k];
    }
    text->setPos(best);

    QGraphicsRectItem *box = scene->addRect(text->sceneBoundingRect(), QPen(QColor(255, 200, 0), 3), QBrush(QColor(255, 250, 205)));
    box->setZValue(12);
}

// 토폴로지 구성 화면: 시점 노드 없이 노드와 링크를 원래 모양 그대로 그리고 표도 같이 갱신
void MainWindow::draw_topology()
{
    draw_view(-1);
    print_table();
}

QString MainWindow::draw_view(int src)
{
    QColor purple(123, 31, 162);
    QColor gray(130, 130, 130);
    QColor blue(30, 100, 220);

    scene->clear();

    // 1. 실제 링크 전체: src가 직접 아는 링크는 굵은 검정, 나머지는 흐린 회색, 끊어진 링크는 빨강
    for (int i = 0; i < current_n; i++) {
        for (int j = i + 1; j < current_n; j++) {
            if (original_cost[i][j] == INF) continue;

            bool isFailed = (cost[i][j] == INF);
            bool known = (src == -1 || i == src || j == src);
            QColor color = known ? QColor(Qt::black) : QColor(200, 200, 200);
            if (isFailed) color = QColor(Qt::red);

            scene->addLine(QLineF(map_pos[i], map_pos[j]), QPen(color, known ? 4 : 2))->setZValue(-3);

            // 비용 숫자는 선 한가운데에 흰 배경을 깔아서 읽기 쉽게
            QString text = isFailed ? QString("∞ (%1)").arg(original_cost[i][j]) : QString::number(cost[i][j]);
            QGraphicsTextItem *label = scene->addText(text, scene_font(14, known));
            label->setDefaultTextColor(isFailed ? QColor(Qt::red) : (known ? QColor(Qt::black) : QColor(150, 150, 150)));
            QPointF mid = (map_pos[i] + map_pos[j]) / 2;
            label->setPos(mid.x() - label->boundingRect().width() / 2, mid.y() - label->boundingRect().height() / 2);
            label->setZValue(2);
            scene->addRect(label->sceneBoundingRect(), QPen(Qt::NoPen), QBrush(Qt::white))->setZValue(1);
        }
    }

    // 2. 이웃을 거쳐 알게 된 곳: src에서 그 노드까지 파란 점선 (실제 링크와 구분되게 살짝 휘어서 그림)
    for (int j = 0; j < current_n; j++) {
        if (src == -1 || j == src || table[src][j] == INF || next_hop[src][j] == j) continue;

        QLineF line(map_pos[src], map_pos[j]);
        QPointF d = (line.p2() - line.p1()) / line.length();
        QPointF bend = (line.p1() + line.p2()) / 2 + QPointF(-d.y(), d.x()) * line.length() * 0.15;

        QPainterPath path(map_pos[src]);
        path.quadTo(bend, map_pos[j]);
        scene->addPath(path, QPen(blue, 3, Qt::DashLine))->setZValue(-2);
    }

    // 3. 노드 그리기 (src가 아는 값은 노드 아래에 표시, 아직 모르는 노드는 흐린 점선 원)
    for (int j = 0; j < current_n; j++) {
        Node *node = new Node(map_pos[j].x(), map_pos[j].y(), NODE_R, j, this);
        scene->addItem(node);
        nodeItems[j] = node;

        QGraphicsTextItem *nameLabel = new QGraphicsTextItem(QString(names[j]), node);
        nameLabel->setFont(scene_font(18, true));
        nameLabel->setPos(-nameLabel->boundingRect().width() / 2, -nameLabel->boundingRect().height() / 2);

        if (j == src || src == -1) continue;

        QString value = "∞";
        if (table[src][j] == INF) {
            node->setBrush(QColor(235, 235, 235));
            node->setPen(QPen(Qt::gray, 2, Qt::DashLine));
        } else if (next_hop[src][j] == j) {
            value = QString::number(table[src][j]);
        } else {
            value = QString("%1 (%2 경유)").arg(table[src][j]).arg(names[next_hop[src][j]]);
        }
        QGraphicsTextItem *text = scene->addText(value, scene_font(12, true));
        text->setPos(map_pos[j].x() - text->boundingRect().width() / 2, map_pos[j].y() + NODE_R);
        text->setZValue(3);
    }

    if (src == -1) return "";       // 구성 화면은 여기까지

    // 4. 이번 라운드에 바뀐 곳 표시와 설명 (채택/탈락한 이웃은 아래 5단계에서 그림)
    bool win_flag[MAX_N]  = {false};    // 채택된 이웃
    bool lose_flag[MAX_N] = {false};    // 탈락한 이웃
    QString vec[MAX_N];                 // 이웃이 알려준 값 (예: "C까지 5, E까지 4")

    mark_node(src, "시점");
    QString msg = explain(src, win_flag, lose_flag, vec);
    if (msg == "") return "";

    // 5. 이웃별 표시 (채택 = 보라 실선 + 화살표 + 진한 말풍선 / 탈락 = 회색 실선 + 흐린 말풍선)
    for (int w = 0; w < current_n; w++) {
        if (!win_flag[w] && !lose_flag[w]) continue;

        QColor color = win_flag[w] ? purple : gray;
        QLineF line(map_pos[w], map_pos[src]);

        if (win_flag[w]) {
            nodeItems[w]->setPen(QPen(purple, 5));
            scene->addLine(line, QPen(purple, 5))->setZValue(-1);

            // 이웃(w) → 시점(src) 방향 화살표 (시점 노드 바로 앞에 삼각형)
            QPointF d = (line.p2() - line.p1()) / line.length();
            QPointF n(-d.y(), d.x());
            QPointF tip = line.p2() - d * (NODE_R + 2);
            QPolygonF arrow;
            arrow << tip << (tip - d * 18 + n * 8) << (tip - d * 18 - n * 8);
            scene->addPolygon(arrow, QPen(purple), QBrush(purple))->setZValue(1);
        } else {
            scene->addLine(line, QPen(QColor(190, 190, 190), 5))->setZValue(-1);
        }

        if (vec[w] != "") {
            QGraphicsTextItem *text = scene->addText(QString("%1(나와 비용 %2)의 벡터 중 %3").arg(names[w]).arg(cost[src][w]).arg(vec[w]), scene_font(13));
            text->setDefaultTextColor(color);
            text->setZValue(11);

            // 말풍선은 시점 노드에서 먼 쪽(이웃 노드의 바깥쪽)에 놓아서 다른 노드를 가리지 않게 함
            qreal bw = text->boundingRect().width();
            qreal bh = text->boundingRect().height();
            qreal bx = qBound(5.0, map_pos[w].x() - bw / 2, SCENE_W - 5 - bw);
            qreal by = map_pos[w].y() + NODE_R + 28;
            if (map_pos[w].y() < map_pos[src].y()) by = map_pos[w].y() - NODE_R - 6 - bh;
            text->setPos(bx, by);

            scene->addRect(text->sceneBoundingRect(), QPen(color, 2), QBrush(Qt::white))->setZValue(10);
        }
    }

    return msg;
}

// 이번 라운드에 src의 값이 바뀐 곳마다 이름표/배지를 붙이고, 계산 과정을 해설창에 적고, 안내창에 쓸 한 줄 설명을 돌려줌
// (바뀐 곳이 없으면 ""). 이웃이 알려준 값은 vec에, 채택/탈락한 이웃은 win_flag/lose_flag에 채워서 돌려줌
QString MainWindow::explain(int src, bool win_flag[], bool lose_flag[], QString vec[])
{
    QString detail = "";
    QString newL = "", shortL = "", otherL = "";    // 안내 문장용: 새로 알게 된 / 더 짧아진 / 비용이 바뀐 도착지

    for (int j = 0; j < current_n; j++) {
        if (table[src][j] == prev[src][j]) continue;

        int win = next_hop[src][j];
        QString newStr = (table[src][j] == INF) ? "∞" : QString::number(table[src][j]);
        QString cands = "";     // 후보 목록 (∞는 생략)
        QString badge = "";     // 바뀐 노드 옆 배지 내용
        int cnt = 0;            // 후보 개수

        for (int w = 0; w < current_n; w++) {
            if (w == src || cost[src][w] == INF || prev[w][j] == INF) continue;

            int sum = cost[src][w] + prev[w][j];
            if (cands != "") cands += ", ";
            cands += QString("%1 경유 %2+%3=%4").arg(names[w]).arg(cost[src][w]).arg(prev[w][j]).arg(sum);
            cnt++;
            if (w == win) badge += QString("<b style='color:#7b1fa2'>%1 경유 %2 ← 선택</b><br>").arg(names[w]).arg(sum);
            else          badge += QString("<span style='color:#828282'>%1 경유 %2 탈락</span><br>").arg(names[w]).arg(sum);

            // 이웃 자신(0)은 말풍선에서 생략
            if (w != j) {
                if (vec[w] != "") vec[w] += ", ";
                vec[w] += QString("%1까지 %2").arg(names[j]).arg(prev[w][j]);
            }

            if (w == win) win_flag[w] = true;
            else          lose_flag[w] = true;
        }

        // 안내 문장용 분류와 이름표
        QString *list = &otherL;
        QString tag = "값 바뀜";
        if (prev[src][j] == INF && table[src][j] != INF) {
            list = &newL;
            tag = "새로 앎";
        } else if (table[src][j] < prev[src][j]) {
            list = &shortL;
        }
        if (table[src][j] == INF) tag = "끊김";
        if (*list != "") *list += ", ";
        *list += QString(names[j]);
        mark_node(j, tag);

        // 후보가 둘 이상일 때만 비교 배지를 띄움 (하나뿐이면 노드 아래 글씨와 똑같아서 생략)
        if (cnt >= 2) add_badge(j, badge);

        if (win == -1)
            detail += QString("%1까지: 경로가 끊겨서 ∞\n").arg(names[j]);
        else
            detail += QString("%1까지: %2 → %3 채택(%4)\n").arg(names[j]).arg(cands).arg(newStr).arg(names[win]);
    }

    if (detail == "") {
        ui->detail->setPlainText(QString("이번 라운드에는 %1의 값이 바뀐 곳이 없어요.").arg(names[src]));
        return "";
    }
    ui->detail->setPlainText(detail + "\n나머지 목적지는 기존 값이 같거나 더 작아서 변화 없음");

    // 안내창에 쓸 한 줄 설명 만들기
    QString who = "";
    for (int w = 0; w < current_n; w++) {
        if (!win_flag[w] && !lose_flag[w]) continue;
        if (who != "") who += ", ";
        who += QString(names[w]);
    }
    QString msg = QString("%1는 ").arg(names[src]);
    if (who != "") msg += QString("이웃 %1가 알려준 정보로 ").arg(who);
    QStringList parts;
    if (newL != "")   parts << QString("%1까지 가는 길을 새로 알게 됐어요.").arg(newL);
    if (shortL != "") parts << QString("%1까지는 더 짧은 길을 찾았어요.").arg(shortL);
    if (otherL != "") parts << QString("%1까지 가는 길의 비용이 늘었거나 끊겼어요.").arg(otherL);
    msg += parts.join(" ");

    return msg;
}
