// 메인 윈도우 생성자 및 그래픽 씬(Scene) 초기 설정

#include "simulator.h"
#include "./ui_simulator.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // UI 파일(simulator.ui)의 위젯들을 메인 윈도우에 연결
    ui->setupUi(this);

    // 토폴로지를 그릴 그래픽 씬(도화지) 생성 및 화면(show)에 장착
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, SCENE_W, SCENE_H);
    ui->show->setScene(scene);
    ui->show->setRenderHint(QPainter::Antialiasing);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 창 크기가 바뀌어도 도화지 전체가 한 화면에 꽉 차게 맞춤 (글씨도 같이 커지고 작아짐)
void MainWindow::showEvent(QShowEvent *e)
{
    QMainWindow::showEvent(e);
    ui->show->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    ui->show->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}
