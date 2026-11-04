// C 원본 토폴로지 파일 저장 (save_topology.c 기반)

#include "simulator.h"
#include "./ui_simulator.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

void MainWindow::on_action_save_triggered()
{
    // 저장할 토폴로지가 없으면 안내 후 중단
    if (need_topology()) return;

    // 파일 저장 창 열기 (취소 시 중단)
    QString fileName = QFileDialog::getSaveFileName(this, "저장하기", find_path("save"), "*.txt");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QTextStream out(&file);

    // 첫 줄에 노드 개수 기록
    out << current_n << "\n";

    // 양방향 중복 저장을 막기 위해 한쪽 방향(i < j)만 원본 비용(original_cost) 기준으로 기록
    for (int i = 0; i < current_n; i++) {
        for (int j = i + 1; j < current_n; j++) {
            if (original_cost[i][j] != INF) {
                out << names[i] << " " << names[j] << " " << original_cost[i][j] << "\n";
            }
        }
    }

    guide("[저장 완료] 현재 네트워크를 파일로 저장했어요. 다음에 [파일] → [파일 불러오기]로 다시 열 수 있어요.");
}
