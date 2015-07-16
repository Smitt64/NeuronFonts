#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <neuron.h>
#include <QDir>
#include <QListWidget>
#include "neuronfonts.h"

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
    void onClearMemoryAction();

    void glyphReadedFromMemory(uint id);

private:
    Ui::MainWindow *ui;
    NeuronFonts *nf;

    QAction *clearMemoryAction;
#ifdef WIN_DWM
    QMargins margin;
#endif
};

#endif // MAINWINDOW_H
