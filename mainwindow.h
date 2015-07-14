#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <neuron.h>
#include <QDir>
#include <QListWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    void start(bool onlyclear = false);
    void recognize();
    Ui::MainWindow *ui;
    QDir memdir;
    QList<Neuron<int,29,29>*> neuro_web;

#if QT_VERSION >= 0x050000 && defined(Q_OS_WIN)
    QMargins margin;
#endif
};

#endif // MAINWINDOW_H
