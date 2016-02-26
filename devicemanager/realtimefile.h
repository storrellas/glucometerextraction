#ifndef REALTIMEFILE_H
#define REALTIMEFILE_H

#include <QFile>
#include <QTextStream>


/*!
 * \class RealTimeFile realtimefile.h
 *
 * \brief This class interfaces the file that has the signals to be sent to
 * StarStim to perform the user defined stimulation in real time.
 */
class RealTimeFile
{
public:
    /*!
     * Constructor.
     *
     * \param fileName Name of the file that contains the real time
     * signal.
     */
    RealTimeFile(QString fileName = "");

    /*!
     * Destructor.
     */
    ~RealTimeFile();

    /*!
     * This function sets the name of the file that contains the real time
     * signal
     *
     * \param fileName The file name
     */
    void setFileName (QString fileName);

    /*!
     * this function read the specified number of samples from the text file
     * and placed them on the provided buffer in binary form
     *
     * \param numberOfSamples Number of samples to be read
     *
     * \param buffer Pointer to the buffer where the samples will be placed.
     * The caller shall provide space enough to store the samples
     *
     * \param size Output parameter where is reported the number of byte
     * written to the buffer. A negative number indicates that the reported
     * samples are the last ones of the file
     *
     * \param channelInfo Output parameter that contains the information
     * regarding the channels that are present in the samples
     *
     * \param isLast Pointer to boolean that is set with true value when the
     * read samples are the last ones from the file.
     *
     * \return The number of read samples. It might be less than the asked ones
     * if the end of the file is reached. A negative number is returned if some
     * error happens while reading from the file (-1) or the format of file is
     * not the expected one (-2)
     */
    int readSamples(int numberOfSamples, unsigned char * buffer, int * size,
                                  unsigned char * channelInfo, bool * isLast);
private:
    /*!
     * \property realTimeFile::_fileWithRealTimeSignal
     *
     * Handle of the file that has the real time stimulation signal
     */
    QFile _fileWithRealTimeSignal;

    /*!
     * \property realTimeFile::_textStream
     *
     * Interface for reading the text from the file
     */
    QTextStream _textStream;
};

#endif // REALTIMEFILE_H
