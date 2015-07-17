#ifndef NEURONFONTS_H
#define NEURONFONTS_H

#include <QtCore>
#include <QObject>
#include <QtSql>
#include "neuron.h"

#ifdef Q_OS_WIN
#include <QAxBindable>
#endif

#define MEMORY_TYPE quint16
#define IMG_SIZE 29
#define IMG_DATA_SIZE (IMG_SIZE * IMG_SIZE * sizeof(MEMORY_TYPE))
#define UNICODE_A 0x0410

class NeuronFonts : public QObject
#ifdef Q_OS_WIN
        , QAxBindable
#endif
{
    Q_OBJECT

    Q_CLASSINFO("ClassID", "{C096E987-A36E-447F-AEA3-2754B075AFEE}")
    Q_CLASSINFO("InterfaceID", "{5CD93C84-FD9A-41C2-B35B-8882E7263075}")
    Q_CLASSINFO("EventsID", "{6BDCC1F9-A7C6-4285-A512-E9E59CB9D296}")
    Q_CLASSINFO("RegisterObject", "yes")

    Q_PROPERTY(QString MemoryFile READ MemoryFile WRITE setMemoryFile)
public:
    explicit NeuronFonts(QObject *parent = 0);
    ~NeuronFonts();
    QString MemoryFile() const;

signals:
    void glyphReadedFromMemory(uint id);

public slots:
    void setMemoryFile(const QString &memfile);
    void openMemory();
    void recognize(const QPixmap &image, uint &neuronid);
    void recognize(const QString &fimage, uint &neuronid);
    void study(uint &neuronid);
    void getMemoryIcon(uint neuronid, QByteArray &out);
    int getMemoryValue(uint neuronid, uint x, uint y);
    void clearMemory(uint neuronid);

private:
    void start();
    QString _memory;
    QSqlDatabase _db;
    QList<Neuron<MEMORY_TYPE,29,29>*> neuro_web;

    int max_n;
};

#endif // NEURONFONTS_H
