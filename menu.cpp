// 종료, 도움말 PDF 열기, 프로젝트 폴더의 파일 찾기

#include "simulator.h"
#include "./ui_simulator.h"
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>

void MainWindow::on_action_end_triggered()  { close(); }
void MainWindow::on_action_help_triggered() { open_pdf("help.pdf"); }
void MainWindow::on_action_th_triggered()   { open_pdf("theory.pdf"); }
void MainWindow::on_help_clicked()          { open_pdf("help.pdf"); }
void MainWindow::on_th_clicked()            { open_pdf("theory.pdf"); }

// 프로그램을 어디서 실행하든 프로젝트 폴더의 파일(esave, save, help.pdf...)을 찾아 줌
QString MainWindow::find_path(QString name)
{
    // 1. 현재 작업 폴더에서 먼저 찾기
    if (QFileInfo::exists(name)) return QFileInfo(name).absoluteFilePath();

    // 2. 실행 파일 위치에서 위 폴더로 올라가며 찾기
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 8; i++) {
        if (dir.exists(name)) return dir.absoluteFilePath(name);
        if (!dir.cdUp()) break;
    }
    return "";
}

// PDF 열기 (상대경로는 맥에서 안 열려서 찾은 전체 경로로 연다)
void MainWindow::open_pdf(QString name)
{
    QString path = find_path(name);

    if (path == "") {
        guide(QString("[안내] %1 파일을 찾을 수 없어요. 프로젝트 폴더 안에 %1 파일이 있는지 확인해 주세요.").arg(name));
        return;
    }
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path)))
        guide(QString("[안내] %1 파일을 열 수 없어요. PDF를 볼 수 있는 프로그램이 설치되어 있는지 확인해 주세요.").arg(name));
}
