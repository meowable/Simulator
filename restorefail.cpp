// 장애 링크 복구 및 안내창 연동

#include "simulator.h"
#include "./ui_simulator.h"
#include <QInputDialog>

void MainWindow::on_restoref_clicked()
{
    if (need_topology()) return;

    if (failed_count == 0) {
        guide("[안내] 지금 끊어진 링크가 없어서 복구할 게 없어요. 수렴이 끝났다면 [링크 장애 설정]으로 먼저 링크를 끊어 보세요.");
        return;
    }

    QStringList items;
    for (int i = 0; i < failed_count; i++) {
        items << QString("%1 - %2 (원래 비용 %3)")
                     .arg(names[failed_links[i][0]])
                     .arg(names[failed_links[i][1]])
                     .arg(failed_links[i][2]);
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "링크 장애 복구", "복구할 링크 선택", items, 0, false, &ok);
    if (!ok) return;

    int sel = items.indexOf(selected);
    int u = failed_links[sel][0];
    int v = failed_links[sel][1];
    int c = failed_links[sel][2];

    cost[u][v] = cost[v][u] = c;

    if (table[u][v] > c) {
        table[u][v] = c;
        next_hop[u][v] = v;
    }
    if (table[v][u] > c) {
        table[v][u] = c;
        next_hop[v][u] = u;
    }

    for (int i = sel; i < failed_count - 1; i++) {
        failed_links[i][0] = failed_links[i + 1][0];
        failed_links[i][1] = failed_links[i + 1][1];
        failed_links[i][2] = failed_links[i + 1][2];
    }
    failed_count--;

    converged = round_count = 0;
    in_round = false;
    draw_topology();

    ui->detail->clear();
    guide(
        QString("[장애 복구 완료 (%1-%2)] 링크가 비용 %3으로 되살아났어요! 라우터들이 더 짧은 길이 생긴 걸 알게 되는 과정을 볼 차례예요.\n[다음 라운드 ▶]를 눌러 경로가 회복되는 과정을 확인하세요.")
            .arg(names[u]).arg(names[v]).arg(c)
        );
}