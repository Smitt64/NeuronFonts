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

#define IMG_SIZE 29
#define IMG_DATA_SIZE (IMG_SIZE * IMG_SIZE * sizeof(int))
#define UNICODE_A 0x0410

#if QT_VERSION >= 0x050000
#include <QtWinExtras>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    memdir = QDir::current();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(memdir.absoluteFilePath("memory.db"));
    if (db.open())
    {
        if (!db.tables().contains("memory"))
        {
            QSqlQuery q("CREATE TABLE memory (id  INTEGER NOT NULL,mem  BLOB NOT NULL,PRIMARY KEY (id ASC))");

            QByteArray data(IMG_DATA_SIZE, 0);

            int k[29][29];
            for (int x = 0; x < IMG_SIZE; x++)
            {
                for (int y = 0; y < IMG_SIZE; y++)
                {
                    k[x][y] = 255;
                }
            }
            data.setRawData((const char*)k, IMG_DATA_SIZE);
            for (int i = 0; i < 32; i++)
            {
                QSqlQuery q2;
                q2.prepare("INSERT INTO memory VALUES (:id ,:mem)");
                q2.bindValue(":id", i);
                q2.bindValue(":mem", data);

                if (!q2.exec())
                {
                    qDebug() << q2.lastError().text();
                }
            }
        }
    }

    for (int i = 0; i < 32; i++)
    {
        neuro_web.push_back(new Neuron<int,IMG_SIZE,IMG_SIZE>());
    }

    start();

#if QT_VERSION >= 0x050000 && defined(Q_OS_WIN)
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_VISTA && QSysInfo::windowsVersion() <= QSysInfo::WV_WINDOWS10)
    {
        QPalette palette = ((QWidget*)this)->palette();
        margin = QMargins(4, 4, 4, ui->statusBar->height());
        setAttribute(Qt::WA_NoSystemBackground);
        QtWin::enableBlurBehindWindow(this);
        QtWin::extendFrameIntoClientArea(this, margin);
    }
    //centralWidget()->setStyleSheet(QString("QWidget#centralWidget {background: %1}").arg(palette.color(QPalette::Window).name()));
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start(bool onlyclear)
{
    ui->listWidget->clear();
    for (int i = 0; i < neuro_web.size(); i++)
    {
        neuro_web[i]->weight = 0;
        neuro_web[i]->output = 50;
        neuro_web[i]->name = QString::number(i);


        ui->listWidget->addItem(QChar(UNICODE_A + i));

        if (!onlyclear)
        {
            QSqlQuery q;
            q.prepare("select * from memory where id = ?");
            q.bindValue(0, i);

            if (q.exec())
            {
                q.next();
                memcpy(neuro_web[i]->memory, q.value(1).toByteArray().data(), IMG_DATA_SIZE);
            }
        }
    }
}

void MainWindow::recognize()
{
    int img[IMG_SIZE][IMG_SIZE];
    int max = 0;
    int max_n;
    max_n = 0;
    memset(img, 0, sizeof(img));

    QImage map = ui->label->pixmap()->toImage().scaled(IMG_SIZE, IMG_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    for (int x = 0; x < IMG_SIZE; x++)
    {
        for (int y = 0; y < IMG_SIZE; y++)
        {
            QRgb c = map.pixel(x, y);
            img[x][y] = (qRed(c) + qGreen(c) + qBlue(c)) / 3;
        }
    }

    for (int i = 0; i < neuro_web.size(); i++)
    {
        for (int x = 0; x < IMG_SIZE; x++)
        {
            for (int y = 0; y < IMG_SIZE; y++)
            {
                neuro_web[i]->input[x][y] = img[x][y];
            }
        }
    }

    for (int i = 0; i < neuro_web.size(); i++)
    {
        for (int x = 0; x < IMG_SIZE; x++)
        {
            for (int y = 0; y < IMG_SIZE; y++)
            {
                int n = neuro_web[i]->memory[x][y];
                int m = neuro_web[i]->input[x][y];

                if ((abs(m - n) < 120))
                {
                    if (m < 250)
                        neuro_web[i]->weight = neuro_web[i]->weight + 1;
                }
                if (m != 0)
                {
                    if (m < 250)
                    {
                        n = (n + (n + m) / 2) / 2;
                    }
                    neuro_web[i]->memory[x][y] = n;
                }
                else if (n != 0)
                {
                    if (m < 250)
                    {
                        n = (n + (n + m) / 2) / 2;
                    }
                }
                neuro_web[i]->memory[x][y] = n;
            }
        }

        if (neuro_web[i]->weight > max)
        {
            max = neuro_web[i]->weight;
            max_n = i;
        }
    }

    QString s = QInputDialog::getText(this, tr("I was wrong?"), tr("I think it is: %1").arg(QChar(UNICODE_A + max_n)), QLineEdit::Normal, QChar(UNICODE_A + max_n));

    if (s.isEmpty())
        return;

    QChar ch = s.at(0).toUpper();

    if (!ch.isLetter())
        return;

    if (ch.unicode() < UNICODE_A || ch.unicode() > UNICODE_A + 32)
        return;

    int newid = ch.unicode() - UNICODE_A;

    QSqlQuery q;
    QByteArray data(IMG_DATA_SIZE, 0);
    data.setRawData((const char*)neuro_web[newid]->memory, IMG_DATA_SIZE);

    q.prepare("update memory set mem = :mem where id = :id");
    q.bindValue(":id", newid);
    q.bindValue(":mem", data);

    q.exec();

    start();
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
        recognize();
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
            mapout.setPixel(x, y, qRgb(neuro_web[newid]->memory[x][y], neuro_web[newid]->memory[x][y], neuro_web[newid]->memory[x][y]));
        }
    }
    mapout = mapout.scaled(128, 128);
    ui->label->setPixmap(QPixmap::fromImage(mapout));
}
