// 프로그램 진입점 (C 원본의 while(1) 루프를 Qt 이벤트 루프로 대체)

#include "simulator.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Qt GUI 애플리케이션 객체 생성
    QApplication a(argc, argv);

    // 메인 윈도우 생성 및 초기화
    MainWindow w;

    // 화면에 띄우기
    w.show();

    // C 원본의 무한 루프를 대신하여 이벤트(클릭 등) 대기
    return QApplication::exec();
}