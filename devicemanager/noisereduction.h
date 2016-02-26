#ifndef NOISEREDUCTION_H
#define NOISEREDUCTION_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "channeldata.h"

#define  BLOCK_LEN      (20)
#define  BLOCK_LEN_50      (20)
#define  BLOCK_LEN_60      (25)
#define  XLEN           (50000)
#define  CANCEL_ALPHA   (0.3)



/*!
 *  This function sweeps the vector XVEC by block of BLOCK_LEN.
 *  With each block projects it over the matrix space MATRIX_M
 *  to coherently cancell out the content of the mentioned space
 *  on XVEC.
 *
 * \param xvec Incoming vector to be denoised
 *
 * \param block_len Each of the blocks to be projected
 *
 * \param offset Offset between blocks
 *
 * \param matrix_m Matrix to be used as projection
 *
 * \param yvec Denoised vector
 *
 * \param svec
 */
void cancel_pwl_first(double *xvec, uint32_t block_len, int offset, double **matrix_m, double *yvec, double *svec);

/*!
 *  This function sweepd the vector XVEC by block of BLOCK_LEN.
 *  With each block projects it over the matrix space MATRIX_M
 *  to coherently cancell out the content of the mentioned space
 *  on XVEC. The output is written in vector YVEC.
 *
 * \param xvec Incoming vector to be denoised
 *
 * \param block_len Length of each of the blocks to be projectedr
 *
 * \param matrix_m Projection matrix
 *
 * \param yvec Denoised vector
 *
 * \param svec
 */

void cancel_pwl_signal(double *xvec, uint32_t block_len, double **matrix_m, double *yvec, double *svec);


/*!
 *  This function projects the signal XVEC of lenghth XLEN
 *  over the matrices M50 and M60 of dimensions XLEN x XLEN.
 *  The function retruns 50 when the projection over M50
 *  has higher energy than the projection over M60 Otherwise it returns 60
 *
 * \param xvec Incoming vector to evaluate the powerline noise
 *
 * \param xlen Length of the incoming vector
 *
 * \param m50 50 Hz projection matrix
 *
 * \param m60 60 Hz projection matrix
 *
 * \param xlen50 length of the 50Hz projection matrix
 *
 * \param xlen60 length of the 60Hz projection matrix
 */
double detect_pwl_freq(double *xvec, uint32_t xlen, double **m50, double **m60, int xlen50, int xlen60);

/*!
 * Allocates memory for a Vector of dimensions 1 x VLEN of type doubl
 * returning a pointer to the vector
 *
 * \param vlen dimension of the vector
 */
double* CreateVector(uint32_t vlen);


/*!
 * Allocates memory for a Matrix of dimensions ROWS x COLS of typw doule
 * returning a pointer to the matrix
 *
 * \param rows number of rows
 *
 * \param cols number of columns
 */
double** CreateMatrix(uint32_t rows, uint32_t cols);


/*!
 * Free memory space for a Matrix of dimensions ROWS x COLS of type double
 *
 * \param rows number of rows
 *
 * \param cols number of columns
 */
void FreeMatrix(double **mat, uint32_t rows, uint32_t cols);


/*!
 * reads the file in FNAME and fills the contens of VECTOR of length VLEN.
 * VECTOR must be allocated before calling this function.
 *
 * \param fname name of the file
 *
 * \param xvec vector to be filled
 *
 * \param vlen length of the vector
 */
void read_vexctor_from_file(const char *fname, double *xvec, uint32_t vlen);

/*!
 *  Reads the file in FNAME and fills the contens of a square MATRIX
 *  of dimensions BLOCK_LEN x BLOCK_LEN. MATRIX must be allocated before calling this function.
 *
 * \param fname name of the file
 *
 * \param matrix matrix to be filled
 *
 * \param block_len length of the vector
 */
void read_matrix_from_file(const char *fname, double **matrix, uint32_t block_len);

/*!
 *  This function updates the value of the intermediate variable svec
 *
 * \param xvec Incoming vector to evaluate the powerline noise
 *
 * \param offset Offset between blocks
 *
 * \param block_len Length of each of the blocks to be projectedr
 *
 * \param matrix_m Projection matrix
 *
 * \param svec Svec value pointer to update the svec vector
 */
void update_svec(double *xvec, uint32_t offset, uint32_t block_len, double **matrix_m, double *svec);

/*!
 *  This function denoises the incoming sample
 *
 * \param yvec sample to be denoised
 *
 * \param svec denoising vector
 *
 * \param index of the vector to be substracted to denoise the sample
 *
 * \return denoised sample
 */
double denoise_sample(double *yvec, double *svec, int index);



/*!
 * \class NoiseReduction noisereduction.h
 *
 * \brief This class performs the power line noise cancelling denoising the incoming signal
 * getting rid of th power line noise at 50 or 60Hz.
 */

class NoiseReduction : public QObject
{
    Q_OBJECT

public:

    /*!
     * Default constructor
     */
    NoiseReduction ();

    /*!
     * Default destructor
     */
    virtual ~NoiseReduction ();


    enum State {
        EVALUATING_NOISE_FREQUENCY,
        DENOISING
    };

    uint32_t ind;

    /*!
     * \property NoiseReduction:matrix_m50
     *
     * Matrix to project the received signl onto the 50Hz power-line signal
     */
    double   **matrix_m50;

    double   **outMatrix;

    /*!
     * \property NoiseReduction:matrix_m60
     *
     * Matrix to project the received signl onto the 60Hz power-line signal
     */
    double   **matrix_m60;

    /*!
     * \property NoiseReduction:matrix_m
     *
     * Matrix to project the received signal onto the 50 or 60Hz power-line signal previously detected
     */
    double   **matrix_m;

    /*!
     * \property NoiseReduction:xvec
     *
     * Vector to store the input signal to be processed
     */
    double   *xvec;

    /*!
     * \property NoiseReduction:yvec
     *
     * Vector to store the output signal after signal cancellation
     */
    double   *yvec;

    /*!
     * \property NoiseReduction:svec
     *
     * Vector to store the estimation of the interference signal
     */
    double   *svec;

    /*!
     * \property NoiseReduction:fvec
     *
     * Vector to store the estimation of the interference signal
     */
    double   *fvec;

    /*!
     * \property NoiseReduction:pwl_freq
     *
     * Power line noise frequency detected
     */
    double   pwl_freq;

    /*!
     * \property NoiseReduction:_delay
     *
     * Delay in samples at startUp to evaluate if the noise frequency is 50 or 60 Hz
     */
    int _delay;

    /*!
     * \property NoiseReduction:_noiseFrequency
     *
     * Noise Frequency Stimation in Hz(50 or 60 Hz)
     */
    int _noiseFrequency;


    /*!
     * \property NoiseReduction:_numOfChannels
     *
     * Number of channels of the device currently connected
     */
    int _numOfChannels;


    /*!
     * \property NoiseReduction:_noiseFrequency
     *
     * Length of the trasposition matrix used
     */
    int _blockLength;


    /*!
     * \property NoiseReduction:_noiseState
     *
     * Current state of the denoising procedure with two possible states: Detecting pwl frequency or Denoising
     */
    State _noiseState;


    /*!
     * \property NoiseReduction:_noiseReductionEnabled
     *
     * boolean set to true if the denoising procedure wants to be applied and to false otherwise.
     */
    bool _noiseReductionEnabled;

    /*!
     *  This method denoises the incoming sample
     *
     * \param data ChannelData class containing the incoming sample
     *
     * \return ChannelData class containing the incoming sample denoised if _noiseReductionEnabled is set to true
     *
     */
    ChannelData denoiseSample(ChannelData data);

    /*!
     *  Gets wether the denoising state is denoising or evaluating the noise frequency
     *
     * \return true if the current state is Denoising, false otherwise
     *
     */
    bool isDenoising();

    /*!
     *  Sets the number of channels of the current device connected
     *
     * \param channels number of channels of the current device connected
     *
     */
    void setNumOfChannels(int channels);

    /*!
     *  Enables the noise cancelling procedure
     */
    void enableLineNoiseCancelling();

    /*!
     *  Disables the noise cancelling procedure
     */
    void disableLineNoiseCancelling();

signals:

    /*!
     *  This signal is emitted to deliver to the following stages
     * the data sample after applying the denoising procedure
     *
     * \param data ChannelData class containing the denoised sample
     *
     */
    void DenoisedData(ChannelData data);

public slots:



private slots:

    /*!
     * This slot receives the raw data sample before being denoised
     *
     * \param data ChannelData class containing sample to be denoised
     *
     * \param  numOfChannels number of channels of the
     *
     */
     void onNewData(ChannelData data, int numOfChannels);

private:

private:

    /*!
     * \property NoiseReduction:_Buffer
     *
     * FIFO Buffer where the data stored and where the denoising algorrithm is applied
     */
    QVector <double> _Buffer[32];

    /*!
     * \property NoiseReduction:_BufferDenoise
     *
     * Buffer where the denoised data is stored
     */
    QVector <double> _BufferDenoised[32];

};

#endif // NOISEREDUCTION_H
