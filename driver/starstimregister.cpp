#include "icognosregister.h"

StarStimRegister::StarStimRegister() : _updated(false), _value(0)
{
}

unsigned int StarStimRegister::value()
{
    _updated = false;
    return _value;
}

void StarStimRegister::setValue (unsigned int value)
{
    _value = value;
    _updated = true;
}

bool StarStimRegister::updated ()
{
    return _updated;
}
