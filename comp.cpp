// 초기 라우팅 테이블과 현재 라우팅 테이블 비교 팝업

#include "simulator.h"
#include "./ui_simulator.h"
#include <QDialog>
#include <QHBoxLayout>
#include <QHeaderView>

void MainWindow::on_comp_clicked()
{
    if (need_topology()) return;

    // 표 2개를 가로로 나란히 놓을 팝업창
    QDialog dlg(this);
    dlg.resize(650, 340);
    QHBoxLayout *layout = new QHBoxLayout(&dlg);

    // k = 0: 초기 표(왼쪽), k = 1: 현재 표(오른쪽)
    for (int k = 0; k < 2; k++) {
        QTableWidget *t = new QTableWidget(current_n, current_n);
        t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        t->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QStringList headers;

        for (int i = 0; i < current_n; i++) {
            headers << QString(names[i]);

            for (int j = 0; j < current_n; j++) {
                int val = (k == 0) ? original_cost[i][j] : table[i][j];

                QTableWidgetItem *item = new QTableWidgetItem((val == INF) ? "∞" : QString::number(val));

                // 현재 표에서 초기 값과 달라진 칸은 노란색으로 강조
                if (k == 1 && val != original_cost[i][j]) item->setBackground(Qt::yellow);
                t->setItem(i, j, item);
            }
        }

        t->setHorizontalHeaderLabels(headers);
        t->setVerticalHeaderLabels(headers);
        layout->addWidget(t);
    }

    dlg.exec();
}
