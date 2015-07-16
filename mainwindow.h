#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <neuron.h>
#include <QDir>
#include <QListWidget>

namespace Ui {
class MainWindow;
}

#define MEMORY_TYPE quint16

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void onClearMemoryAction();

private:
    void start(bool onlyclear = false);
    void recognize();
    Ui::MainWindow *ui;
    QDir memdir;
    QList<Neuron<MEMORY_TYPE,29,29>*> neuro_web;

    QAction *clearMemoryAction;
#ifdef WIN_DWM
    QMargins margin;
#endif
};

#endif // MAINWINDOW_H
