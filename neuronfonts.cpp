#include "neuronfonts.h"
#include <QtCore>
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

NeuronFonts::NeuronFonts(QObject *parent) : QObject(parent)
{
    QDir memdir = QDir(QApplication::applicationDirPath());
    _memory = memdir.absoluteFilePath("memory.db");

    for (int i = 0; i < 32; i++)
    {
        neuro_web.push_back(new Neuron<MEMORY_TYPE,IMG_SIZE,IMG_SIZE>());
    }

    max_n = -1;
}

NeuronFonts::~NeuronFonts()
{
    for (int i = 0; i < 32; i++)
    {
        delete neuro_web[i];
    }
    neuro_web.clear();
    _db.close();
}

void NeuronFonts::setMemoryFile(const QString &memfile)
{
    _memory = memfile;
}

QString NeuronFonts::MemoryFile() const
{
    return _memory;
}

void NeuronFonts::openMemory()
{
    if (_db.isOpen())
    {
        _db.close();
    }

    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(_memory);

    if (_db.open())
    {
        if (!_db.tables().contains("memory"))
        {
            QSqlQuery q("CREATE TABLE memory (id  INTEGER NOT NULL,mem  BLOB NOT NULL,PRIMARY KEY (id ASC))", _db);
            q.exec();

            QByteArray data(IMG_DATA_SIZE, 0);

            MEMORY_TYPE k[IMG_SIZE][IMG_SIZE];
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
                QSqlQuery q2(_db);
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
    else
    {
        qDebug() << "Can't open memory file " << _memory;
    }
    start();
}

void NeuronFonts::start()
{
    for (int i = 0; i < neuro_web.size(); i++)
    {
        neuro_web[i]->weight = 0;
        neuro_web[i]->output = 50;
        neuro_web[i]->name = QString::number(i);

        QSqlQuery q(_db);
        q.prepare("select * from memory where id = ?");
        q.bindValue(0, i);

        if (q.exec())
        {
            q.next();
            memcpy(neuro_web[i]->memory, q.value(1).toByteArray().data(), IMG_DATA_SIZE);
            emit glyphReadedFromMemory(i);
        }
    }
}

void NeuronFonts::recognize(const QPixmap &image, int &neuronid)
{
    QImage map = image.toImage().scaled(IMG_SIZE, IMG_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    MEMORY_TYPE img[IMG_SIZE][IMG_SIZE];
    int max = 0;

    max_n = -1;
    memset(img, 0, sizeof(img));

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
            neuronid = max_n = i;
        }
    }
}

void NeuronFonts::recognizefile(const QString &fimage, int &neuronid)
{
    QPixmap map;

    if (map.load(fimage))
    {
        printf("recognizefile start");
        recognize(map, neuronid);
    }
    else
    {
        neuronid = -1;
    }
}

void NeuronFonts::study(int &neuronid)
{
    QSqlQuery q(_db);
    QByteArray data(IMG_DATA_SIZE, 0);
    data.setRawData((const char*)neuro_web[neuronid]->memory, IMG_DATA_SIZE);

    q.prepare("update memory set mem = :mem where id = :id");
    q.bindValue(":id", neuronid);
    q.bindValue(":mem", data);

    q.exec();

    start();
}

void NeuronFonts::clearMemory(uint neuronid)
{
    MEMORY_TYPE k[IMG_SIZE][IMG_SIZE];
    for (int x = 0; x < IMG_SIZE; x++)
    {
        for (int y = 0; y < IMG_SIZE; y++)
        {
            k[x][y] = 255;
        }
    }

    QSqlQuery q;
    QByteArray data(IMG_DATA_SIZE, 0);
    data.setRawData((const char*)k, IMG_DATA_SIZE);

    q.prepare("update memory set mem = :mem where id = :id");
    q.bindValue(":id", neuronid);
    q.bindValue(":mem", data);

    q.exec();

    start();
}

int NeuronFonts::getMemoryValue(uint neuronid, uint x, uint y)
{
    return neuro_web[neuronid]->memory[x][y];
}

void NeuronFonts::getMemoryIcon(uint neuronid, QByteArray &out)
{
    out.clear();
    out.resize(IMG_DATA_SIZE);

    out.setRawData((const char*)neuro_web[neuronid]->memory, IMG_DATA_SIZE);
}
