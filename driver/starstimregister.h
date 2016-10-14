#ifndef STARSTIMREGISTER_H
#define STARSTIMREGISTER_H

/*!
 * \class StarStimRegister
 *
 * \brief This class implements a generic icognos3G/StarStim configuration
 * register.
 */
class StarStimRegister
{
public:
    /*!
     * Defaut constructor.
     */
    StarStimRegister();

    /*!
     * It returns the value of the register.
     *
     * \return Register's value.
     */
    unsigned int value();

    /*!
     * It sets the value of the register.
     */
    void setValue (unsigned int value);

    /*!
     * It returns whether the value of the register was updated since the last
     * time it was accessed.
     *
     * \return True if the register was updated since the las access, false
     * otherwise.
     */
    bool updated ();

private:
    /*!
     * \property StarStimRegister::_updated
     *
     * Boolean that holds whether the register's value has been updated.
     */
    bool _updated;

    /*!
     * \property StarStimRegister::_value
     *
     * The value of the register.
     */
    unsigned int _value;
};

#endif // STARSTIMREGISTER_H
