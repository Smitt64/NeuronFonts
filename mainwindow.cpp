#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql>
#include <QDir>
#include <QPixmap>
#include <math.h>
#include <QInputDialog>
#include <QFileDialog>
#include <QDebug>
#include <QPalette>
#include <QSysInfo>

#ifdef WIN_DWM
#include <QtWinExtras>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    nf = new NeuronFonts(this);
    connect(nf, SIGNAL(glyphReadedFromMemory(uint)), SLOT(glyphReadedFromMemory(uint)));
    nf->openMemory();

    clearMemoryAction = new QAction(tr("Forget the letter"), ui->listWidget);
    ui->listWidget->addAction(clearMemoryAction);
    ui->listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

#ifdef WIN_DWM
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_VISTA && QSysInfo::windowsVersion() <= QSysInfo::WV_WINDOWS10)
    {
        margin = QMargins(4, 4, 4, ui->statusBar->height());
        setAttribute(Qt::WA_NoSystemBackground);
        QtWin::enableBlurBehindWindow(this);
        QtWin::extendFrameIntoClientArea(this, margin);

        // uncomment to use non transparent background
        /*
         QPalette palette = ((QWidget*)this)->palette();
         centralWidget()->setStyleSheet(QString("QWidget#centralWidget {background: %1}").arg(palette.color(QPalette::Window).name()));
         */
    }
#endif

    connect(clearMemoryAction, SIGNAL(triggered(bool)), SLOT(onClearMemoryAction()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::glyphReadedFromMemory(uint id)
{
    ui->listWidget->addItem(QChar(UNICODE_A + id));
}

void MainWindow::onClearMemoryAction()
{
    QListWidgetItem *item = ui->listWidget->currentItem();

    if (!item)
        return;

    int memid = item->text().at(0).unicode() - UNICODE_A;

    ui->listWidget->clear();
    nf->clearMemory(memid);
    on_listWidget_itemDoubleClicked(item);
}

void MainWindow::on_pushButton_clicked()
{
    QString s = QFileDialog::getOpenFileName(this);

    if (s.isEmpty())
        return;

    QPixmap m;
    if (m.load(s))
    {
        m = m.scaled(128, 128, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->label->setPixmap(m);
        int max_n = -1;
        nf->recognize(s, max_n);

        if (max_n >= 0)
        {
            QString s = QInputDialog::getText(this, tr("I was wrong?"), tr("I think it is: %1").arg(QChar(UNICODE_A + max_n)), QLineEdit::Normal, QChar(UNICODE_A + max_n));

            if (s.isEmpty())
                return;

            QChar ch = s.at(0).toUpper();

            if (!ch.isLetter())
                return;

            if (ch.unicode() < UNICODE_A || ch.unicode() > UNICODE_A + 32)
                return;

            int newid = ch.unicode() - UNICODE_A;
            ui->listWidget->clear();
            nf->study(newid);
        }
    }
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QImage mapout(QSize(IMG_SIZE, IMG_SIZE), QImage::Format_RGB32);

    int newid = item->text().at(0).toUpper().unicode() - UNICODE_A;
    for (int x = 0; x < IMG_SIZE; x++)
    {
        for (int y = 0; y < IMG_SIZE; y++)
        {
            mapout.setPixel(x, y, qRgb(nf->getMemoryValue(newid, x, y), nf->getMemoryValue(newid, x, y), nf->getMemoryValue(newid, x, y)));
        }
    }
    mapout = mapout.scaled(128, 128);
    ui->label->setPixmap(QPixmap::fromImage(mapout));
}
