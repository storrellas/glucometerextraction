#ifndef TRIGGER_H
#define TRIGGER_H

#include <QtCore>

/*!
 * \class Trigger trigger.h
 *
 * \brief This class encapsulates the trigger information
 */
class Trigger
{
public:
    const static QString TriggerStartTag;
    const static QString TriggerEndTag;
    /*!
     * The constructor.
     *
     * \param code The trigger code.
     *
     * \param timestamp The timestamp when the trigger was received. It is like
     * the POSIX time_t variable (milliseconds since 1970-01-01T00:00:00
     * Universal Coordinated Time) but expresssed in milliseconds instead.
     */
    Trigger(int code = 0, qint64 timestamp = 0);

    /*!
     * It gets the trigger's timestamp.
     *
     * \return The timestamp expressed as the number of milliseconds since
     * 1970-01-01T00:00:00 Universal Coordinated Time.
     */
    qint64 getTimestamp ();

    /*!
     * It gets the trigger's code.
     *
     * \return The trigger's code.
     */
    int getCode ();

private:
    int _code; ///< It keeps the trigger's code.

    qint64 _timestamp; ///< It keeps the trigger's timestamp.
};

#endif // TRIGGER_H
