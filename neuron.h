#ifndef NEURON
#define NEURON

#include <QList>

template<class T, int W, int H>
class Neuron
{
public:
    Neuron()
    {

    }

    QString name;

    T input[W][H];
    T output;
    T weight;
    T memory[W][H];
};

#endif // NEURON

