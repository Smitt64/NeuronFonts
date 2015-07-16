#include "neuronfonts.h"
#include <QtCore>
#include <QDebug>

NeuronFonts::NeuronFonts(QObject *parent) : QObject(parent)
{
    QDir memdir = QDir::current();
    _memory = memdir.absoluteFilePath("memory.db");

    for (int i = 0; i < 32; i++)
    {
        neuro_web.push_back(new Neuron<MEMORY_TYPE,IMG_SIZE,IMG_SIZE>());
    }
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
}
