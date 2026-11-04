// 거리 벡터 라우팅 한 라운드 계산 (C 원본 run_round 기반)

#include "simulator.h"

bool MainWindow::run_round()
{
    // 라운드 시작 값을 prev에 보관 (동시 갱신: 이웃 값은 항상 prev에서만 읽음)
    for (int i = 0; i < current_n; i++) {
        for (int j = 0; j < current_n; j++) {
            prev[i][j] = table[i][j];
        }
    }

    for (int i = 0; i < current_n; i++) {
        // 1단계: 기존 경유지(next_hop) 경로의 비용이 늘어났거나 끊어졌는지 먼저 반영 (교재 323쪽 규칙)
        for (int j = 0; j < current_n; j++) {
            int nh = next_hop[i][j];
            if (nh == -1) continue; // 설정된 경로가 없거나 자기 자신(i == j)이면 건너뜀

            int new_dist = (cost[i][nh] == INF || prev[nh][j] == INF) ? INF : cost[i][nh] + prev[nh][j];

            if (new_dist != table[i][j]) {
                table[i][j] = new_dist;
                if (new_dist == INF) next_hop[i][j] = -1;
            }
        }

        // 2단계: 이웃 노드(w)를 거쳐가는 더 짧은 경로가 있는지 확인 (교재 표 8.1 벨만-포드 방정식)
        for (int w = 0; w < current_n; w++) {
            if (cost[i][w] == INF || i == w) continue;

            for (int j = 0; j < current_n; j++) {
                if (prev[w][j] == INF) continue;

                int new_dist = cost[i][w] + prev[w][j];

                if (new_dist < table[i][j]) {
                    table[i][j]    = new_dist;
                    next_hop[i][j] = w;
                }
            }
        }
    }

    // 라운드가 끝났을 때 시작값(prev)과 달라진 칸이 하나라도 있으면 true
    for (int i = 0; i < current_n; i++) {
        if (row_changed(i)) return true;
    }
    return false;
}

// i번 노드의 행에서 이번 라운드에 값이 바뀐 칸이 있는지
bool MainWindow::row_changed(int i)
{
    for (int j = 0; j < current_n; j++) {
        if (table[i][j] != prev[i][j]) return true;
    }
    return false;
}
