// 라우팅 테이블을 우측 QTableWidget에 출력
// src = -1 : 현재 값 전체 / src >= 0 : src의 시점 장면 (아직 안 지나간 노드 행은 라운드 시작 값 prev)

#include "simulator.h"
#include "./ui_simulator.h"
#include <QHeaderView>

void MainWindow::print_table(int src)
{
    ui->table->clear();
    ui->table->setRowCount(current_n);
    ui->table->setColumnCount(current_n);

    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList headers;

    for (int i = 0; i < current_n; i++) {
        headers << QString(names[i]);

        for (int j = 0; j < current_n; j++) {
            // 아직 시점이 지나가지 않은 노드(i > src)의 행은 라운드 시작 값을 보여줌
            int val = table[i][j];
            if (src != -1 && i > src) val = prev[i][j];

            QString text = (val == INF) ? "∞" : QString::number(val);

            QTableWidgetItem *item = new QTableWidgetItem(text);
            item->setTextAlignment(Qt::AlignCenter);

            if (i == src) item->setBackground(QColor(255, 255, 200));   // 시점 노드 행: 연한 노랑 띠

            if (i == src && table[i][j] != prev[i][j]) {                // 이번 장면에 바뀐 칸: 진한 노랑 + 경유 이웃
                item->setBackground(QColor(255, 200, 0));
                if (next_hop[i][j] != -1) item->setText(QString("%1 (%2)").arg(text).arg(names[next_hop[i][j]]));
            }

            ui->table->setItem(i, j, item);
        }
    }

    ui->table->setHorizontalHeaderLabels(headers);
    ui->table->setVerticalHeaderLabels(headers);
}
