// 시뮬레이터 초기화: 토폴로지를 모두 지우고 1단계 안내로 돌아감

#include "simulator.h"
#include "./ui_simulator.h"

void MainWindow::on_action_reset_triggered()
{
    current_n = 0;
    round_count = converged = failed_count = 0;
    in_round = false;

    draw_topology();
    ui->detail->setPlainText("");
    guide("[1단계 안내] 모두 지웠어요. 위쪽 [파일] 메뉴를 눌러 예제(3/4/7 노드)를 불러오거나, [직접 구상 - 노드 추가]로 직접 네트워크를 만들어 보세요.");
}
