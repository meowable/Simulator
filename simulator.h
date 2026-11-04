#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QFont>

#define MAX_N 10
#define INF 9999

#define SCENE_W 900     // 도화지 크기 (화면 크기에 맞춰 통째로 확대/축소됨)
#define SCENE_H 600
#define NODE_R 28       // 노드(원) 반지름

class Node;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 노드 10개의 고정 좌표 (도화지 900 x 600 기준)
inline const QPointF map_pos[MAX_N] = {
    {100, 180}, {330, 100}, {580, 150},
    {130, 420}, {370, 470}, {600, 390},
    {800, 260},
    {240, 300}, {460, 290}, {560, 545}
};

// 도화지 위 글씨 폰트 만들기 (크기, 굵기)
inline QFont scene_font(int size, bool bold = false)
{
    QFont f;
    f.setPointSize(size);
    f.setBold(bold);
    return f;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void on_node_clicked(int index);

protected:
    void showEvent(QShowEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private slots:
    // 상단 메뉴바 액션 슬롯 (load.cpp, save.cpp, edit.cpp, reset.cpp, menu.cpp)
    void on_action_e1_triggered();
    void on_action_e2_triggered();
    void on_action_e3_triggered();
    void on_action_load_triggered();
    void on_action_save_triggered();
    void on_action_add_triggered();
    void on_action_link_triggered();
    void on_action_reset_triggered();
    void on_action_end_triggered();
    void on_action_help_triggered();
    void on_action_th_triggered();

    // 상단 조작 버튼 (.ui에 같은 이름의 버튼이 있는 것만 on_이름_clicked 형태로 둠)
    void on_help_clicked();
    void on_th_clicked();
    void on_next_clicked();
    void on_comp_clicked();
    void on_setf_clicked();
    void on_restoref_clicked();

    // 노드 클릭 처리 (next.cpp) - 클릭 이벤트가 끝난 뒤에 실행되도록 on_node_clicked가 예약함
    void change_root(int index);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    // C 원본 전역 배열 구조 유지
    int current_n = 0;
    int round_count = 0;
    int converged = 0;
    int failed_count = 0;

    char names[MAX_N];
    int original_cost[MAX_N][MAX_N];
    int cost[MAX_N][MAX_N];
    int table[MAX_N][MAX_N];
    int prev[MAX_N][MAX_N];         // 라운드 시작 시점의 값 (이웃에게서는 항상 이 값만 읽음)
    int next_hop[MAX_N][MAX_N];
    int failed_links[MAX_N * MAX_N][3];

    Node *nodeItems[MAX_N];

    // 장면 재생 관리
    bool in_round = false;      // 한 라운드의 장면을 재생하는 중인가
    int scene_src = -1;         // 지금까지 재생한 마지막 시점 노드

    // 시뮬레이션 (init_table.cpp, run_round.cpp, next.cpp)
    void init_table();
    bool run_round();
    bool row_changed(int i);
    bool need_topology();
    void guide(QString text);
    void guide_ready();
    void show_final(int node);

    // 화면 그리기 (print_table.cpp, highlight.cpp)
    void draw_topology();                   // 구성 화면 (시점 노드 없음)
    void print_table( int src = -1 );
    QString draw_view( int src );           // src 노드의 시점으로 그리고, 안내창에 쓸 한 줄 설명을 돌려줌 (-1 = 구성 화면)
    QString explain( int src, bool win_flag[], bool lose_flag[], QString vec[] );    // 바뀐 곳 표시 + 설명 만들기
    void mark_node( int idx, QString text );
    void add_badge( int idx, QString html );

    // 파일 (load.cpp, menu.cpp)
    void open_topology(QString path);
    QString find_path(QString name);
    void open_pdf(QString name);
};

#endif // SIMULATOR_H
