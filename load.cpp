// 토폴로지 파일 불러오기 (외부 파일 / 예제 3, 4, 7 노드)

#include "simulator.h"
#include "./ui_simulator.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

// 파일을 읽어서 화면에 띄움 (첫 줄: 노드 개수, 다음 줄부터: "A B 5" 형식의 링크)
void MainWindow::open_topology(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        guide("[안내] 파일을 열 수 없어요. (예제 파일은 프로젝트 폴더 안의 esave 폴더에 있어야 해요)");
        return;
    }

    QTextStream in(&file);
    int n = in.readLine().toInt();
    if (n < 1 || n > MAX_N) {
        guide(QString("[안내] 올바른 토폴로지 파일이 아니에요. 첫 줄에 노드 개수(1~%1)가 있어야 해요.").arg(MAX_N));
        return;
    }
    current_n = n;

    for (int i = 0; i < current_n; i++) {
        for (int j = 0; j < current_n; j++) {
            original_cost[i][j] = (i == j) ? 0 : INF;
        }
    }

    while (!in.atEnd()) {
        QStringList parts = in.readLine().split(' ', Qt::SkipEmptyParts);   // 띄어쓰기가 여러 칸이어도 괜찮게
        if (parts.size() != 3) continue;

        int i = parts[0].at(0).toLatin1() - 'A';
        int j = parts[1].at(0).toLatin1() - 'A';
        int c = parts[2].toInt();
        if (i < 0 || i >= current_n || j < 0 || j >= current_n || i == j || c <= 0) continue;

        original_cost[i][j] = original_cost[j][i] = c;
    }

    init_table();
    draw_topology();
    guide_ready();
}

void MainWindow::on_action_load_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "파일 불러오기", find_path("save"), "*.txt");
    if (fileName != "") open_topology(fileName);
}

void MainWindow::on_action_e1_triggered() { open_topology(find_path("esave/3node.txt")); }
void MainWindow::on_action_e2_triggered() { open_topology(find_path("esave/4node.txt")); }
void MainWindow::on_action_e3_triggered() { open_topology(find_path("esave/7node.txt")); }
