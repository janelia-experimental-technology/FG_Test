#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Util.h"
#include <QMainWindow>
#include <QMap>
#include <QVector>

namespace Ui {
class MainWindow;
}

class FakeFrameGenerator;
class Recorder;
class Dialog;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *) override;

private slots:

private:
    void setupToolBar();
    void updateToolBar();
    void updateStatusMessage();
    QMap<QString, QAction *> tbActs;
    Throttler updateStatusMessageThrottled;
    Ui::MainWindow *ui;
    FakeFrameGenerator *fgen = nullptr;

    enum StatusString { FPS1 = 0, FPS2, FPS3, FrameNum, Dropped, FrameNumRec, MBPerSec, Recording, NStatus };
    QVector<QString> statusStrings = QVector<QString>(NStatus);

    Recorder *rec = nullptr;

    // tmp dialog for "Please Wait..."
    QDialog *dlg_tmp = nullptr;
    void kill_dlg();
    void show_dlg(const QString &);
    QTimer *blinkenTimer = nullptr;
    int blink = 0;
};

#endif // MAINWINDOW_H
