#include "trigger.h"

const QString Trigger::TriggerStartTag = "<TRIGGER>";
const QString Trigger::TriggerEndTag = "</TRIGGER>";

Trigger::Trigger (int code, qint64 timestamp) :
        _code(code),
        _timestamp(timestamp)
{    
}

int Trigger::getCode ()
{
    return _code;
}

qint64 Trigger::getTimestamp ()
{
    return _timestamp;
}
