// [다음 라운드 ▶] 1번 클릭 = 장면 1개 재생 (값이 바뀐 노드의 시점) + 수렴 후 노드 클릭

#include "simulator.h"
#include "./ui_simulator.h"
#include <QMetaObject>

void MainWindow::on_next_clicked()
{
    if (need_topology()) return;

    // 수렴 후에 또 누르면 다음 할 일 안내
    if (converged) {
        guide("[안내] 이미 수렴해서 더 진행할 라운드가 없어요. 다른 노드를 클릭해 그 노드가 최종적으로 아는 길을 비교하거나, [링크 장애 설정]으로 링크를 끊어 보세요.\n처음부터 다시 하려면 [파일] → [초기화]로 모두 지우고 예제를 다시 불러오세요.");
        return;
    }

    // 새 라운드 시작: 라운드 전체를 먼저 계산해 두고, 노드별 장면으로 나눠서 보여줌
    if (!in_round) {
        bool changed = run_round();
        round_count++;

        if (!changed) {
            converged = 1;
            show_final(0);
            return;
        }
        in_round = true;
        scene_src = -1;
    }

    // 값이 바뀐 다음 노드를 찾아 장면 재생 (바뀐 칸이 없는 노드는 건너뜀)
    int src = scene_src + 1;
    while (src < current_n && !row_changed(src)) src++;
    scene_src = src;
    QString msg = draw_view(src);
    print_table(src);

    // 이번 장면 뒤에 더 재생할 노드가 남았는지 확인
    in_round = false;
    for (int i = src + 1; i < current_n; i++) {
        if (row_changed(i)) in_round = true;
    }

    // 안내창: 첫 줄 = 이번 장면 설명, 둘째 줄 = 다음에 할 일
    int total = 0, done = 0;
    for (int i = 0; i < current_n; i++) {
        if (row_changed(i)) {
            total++;
            if (i <= src) done++;
        }
    }

    QString second;
    if (in_round && round_count == 1 && failed_count == 0 && done == 1)
        second = "라운드는 모든 노드가 이웃의 값을 동시에 한 번씩 받아 계산하는 한 바퀴예요. 여기서는 값이 바뀐 노드만 차례로 보여줄게요. (굵은 검정 선 = 직접 아는 링크, 파란 점선 = 이웃을 거쳐 알게 된 길)";
    else if (in_round)
        second = "[다음 라운드 ▶]를 누르면 값이 바뀐 다음 노드의 시점으로 넘어가요.";
    else
        second = "이번 라운드의 마지막 장면이에요. [다음 라운드 ▶]를 누르면 다음 라운드가 시작돼요.";

    guide(QString("[라운드 %1 · %2의 시점 (%3/%4)] %5\n%6")
                                .arg(round_count).arg(names[src]).arg(done).arg(total).arg(msg).arg(second));
}

// 수렴한 뒤 node의 시점으로 최종 모습을 보여줌 (수렴 직후와 노드를 클릭했을 때 사용)
void MainWindow::show_final(int node)
{
    draw_view(node);        // 수렴했으면 바뀐 칸이 없어서 강조 없이 그 노드가 아는 모습만 나옴
    print_table(node);
    ui->detail->setPlainText("모든 노드의 값이 이전 라운드와 똑같아서 더 이상 바뀌지 않아요. (수렴 완료)");

    QString next_step = (failed_count > 0)
        ? "다른 노드를 클릭해 그 노드가 아는 길을 비교하거나, [링크 장애 복구]로 끊은 링크를 되살려 보세요."
        : "다른 노드를 클릭해 그 노드가 아는 길을 비교하거나, [링크 장애 설정]으로 링크를 끊어 보세요.";

    guide(QString("[수렴 완료 · %1라운드 · %2의 시점] 더 이상 바뀌는 값이 없어요. 모든 라우터가 최단 거리를 알게 됐고, 지금 화면이 %2가 최종적으로 아는 길이에요.\n%3 (처음부터는 [파일] → [초기화])")
                                .arg(round_count).arg(names[node]).arg(next_step));
}

// 노드를 클릭하면 호출됨 (Node::mousePressEvent)
// 화면을 다시 그리면 클릭된 노드 자신이 지워져서 프로그램이 죽으므로, 클릭 이벤트가 끝난 뒤에 change_root가 실행되게 예약만 함
void MainWindow::on_node_clicked(int index)
{
    QMetaObject::invokeMethod(this, "change_root", Qt::QueuedConnection, Q_ARG(int, index));
}

// 수렴한 뒤에만 클릭한 노드의 시점으로 최종 모습을 보여줌
void MainWindow::change_root(int index)
{
    if (!converged) {
        guide("[안내] 노드 클릭은 수렴(더 이상 값이 안 바뀜)한 뒤에 쓸 수 있어요. 지금은 상단 [다음 라운드 ▶]를 눌러 계속 진행해 주세요.");
        return;
    }

    show_final(index);
}
