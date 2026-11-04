// 직접 구상 - 노드 추가 및 링크 연결 (안내창 연동 포함)

#include "simulator.h"
#include "./ui_simulator.h"
#include <QInputDialog>

void MainWindow::on_action_add_triggered()
{
    if (current_n >= MAX_N) {
        guide(QString("[안내] 노드는 최대 %1개까지 추가할 수 있어요. [파일] → [직접 구상 - 링크 연결]로 선을 이어 보세요.").arg(MAX_N));
        return;
    }

    for (int i = 0; i < current_n; i++) {
        original_cost[current_n][i] = INF;
        original_cost[i][current_n] = INF;
    }
    original_cost[current_n][current_n] = 0;

    current_n++;

    init_table();
    draw_topology();

    ui->detail->setPlainText("");
    guide(
        QString("[토폴로지 구성 중] %1 노드를 추가했어요 (현재 %2개). 아직 연결선이 없어요.\n[파일] → [직접 구상 - 링크 연결]로 노드끼리 선을 이어 보세요.")
            .arg(names[current_n - 1]).arg(current_n)
        );
}

void MainWindow::on_action_link_triggered()
{
    if (current_n < 2) {
        guide("[안내] 링크를 연결하려면 노드가 2개 이상 필요해요. [파일] → [직접 구상 - 노드 추가]로 먼저 노드를 추가해 주세요.");
        return;
    }

    QString input = QInputDialog::getText(this, "링크 연결", "노드1 노드2 비용 (예: A B 5)");
    if (input.length() < 5) return;

    int u = input.at(0).toLatin1() - 'A';
    int v = input.at(2).toLatin1() - 'A';
    int w = input.mid(4).toInt();

    if (u < 0 || u >= current_n || v < 0 || v >= current_n || u == v || w <= 0) {
        guide(QString("[안내] 입력이 올바르지 않아요. 현재 노드는 %1~%2이고, 'A B 5'처럼 \"노드1 노드2 비용\"을 대문자와 양수로 입력해 주세요.")
                                    .arg(names[0]).arg(names[current_n - 1]));
        return;
    }

    original_cost[u][v] = original_cost[v][u] = w;

    init_table();
    draw_topology();

    ui->detail->setPlainText("");
    guide(
        QString("[링크 연결 완료] %1-%2 링크를 비용 %3으로 연결했어요. 더 연결하려면 [파일] → [직접 구상 - 링크 연결]을 다시 누르세요.\n준비가 끝났다면 상단 [다음 라운드 ▶]를 눌러 거리 벡터 갱신을 시작하세요.")
            .arg(names[u]).arg(names[v]).arg(w)
        );
}
