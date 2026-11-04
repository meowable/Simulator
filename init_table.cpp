// C 원본(init_table.c)의 초기화 로직 유지 + 안내창 공통 함수

#include "simulator.h"
#include "./ui_simulator.h"

void MainWindow::init_table()
{
    for (int i = 0; i < current_n; i++) {
        names[i] = 'A' + i;

        for (int j = 0; j < current_n; j++) {
            prev[i][j] = table[i][j] = cost[i][j] = original_cost[i][j];
            next_hop[i][j] = (cost[i][j] != INF && i != j) ? j : -1;
        }
    }

    round_count = converged = failed_count = 0;
    in_round = false;
}

// 토폴로지가 없을 때 버튼을 누르면 안내창에 알려주고 true를 돌려줌
bool MainWindow::need_topology()
{
    if (current_n > 0) return false;

    guide("[안내] 아직 네트워크가 없어요. 위쪽 [파일] 메뉴에서 예제(3/4/7 노드)를 불러오거나,\n[직접 구상 - 노드 추가]로 직접 만들어 보세요.");
    return true;
}

// 토폴로지가 준비됐을 때 안내창 문구 (예제 불러오기, 파일 불러오기, 링크 연결 공통)
void MainWindow::guide_ready()
{
    ui->detail->setPlainText("[다음 라운드 ▶]를 누르면, 이곳에 값이 바뀐 이유(계산 과정)가 한 줄씩 나와요.");
    guide("[2단계 안내] 네트워크가 준비됐어요! 동그라미는 라우터, 선 위 숫자는 연결 비용이에요. 오른쪽 표는 각 라우터가 알고 있는 거리인데, 처음에는 바로 이웃한 곳만 알고 나머지는 ∞예요.\n상단 [다음 라운드 ▶]를 눌러 라우터끼리 정보를 주고받아 보세요.");
}

// 안내창 문구 바꾸기: 줄바꿈(\n)마다 문단을 나눠서 줄 사이에 간격을 줌
void MainWindow::guide(QString text)
{
    QStringList lines = text.toHtmlEscaped().split("\n");
    ui->guideLabel->setText("<p style='margin:0;'>" + lines.join("</p><p style='margin:10px 0 0 0;'>") + "</p>");
}
