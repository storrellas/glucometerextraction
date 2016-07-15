#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H
#include <QMutex>

class Synchronizer
{
public:
    Synchronizer();

    ~Synchronizer();


    /*!
     * It computes the current offset taking into account the statistic of the previous instant offset
     */
    double computeFilteredOffset(double t);


    /*!
     * It computes the instant offset by knowing
     *     t0: the time when a request from NIC is sent,
     *     t1: the time when this request is received by NEC,
     *     t2: the time when the answer to the request is sent by NEC and
     *     t3: the time when the answer to the request is received by NIC
     */
    double computeOffset(double t0, double t1, double t2, double t3);

    /*!
     * From the statistic of the computed instant offset the drifft between the two clocks is computed
     */
    float getDriftClock(int * measuredTime);

    /*!
     * It removes any dependencies with previous instant offset computations
    */
    void reset();

private:
    int _fit(const double *x, const double *y, unsigned int n, double *b, double *a);

    double * _synchXBuffer;
    double * _synchYBuffer;
    unsigned int _synchIndx;
    QMutex _mutex;

};

#endif // SYNCHRONIZER_H
