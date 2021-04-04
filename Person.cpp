#include "Person.hpp"

Person::Person(bool isSick, int homeID) {
    m_isSick = isSick;
    m_homeID = homeID;
    m_stopCount = 0;
}

bool Person::getIsImmune() {
    return m_isImmune;
}

bool Person::getIsSick() {
    return m_isSick;
}

int Person::getHomeID() {
    return m_homeID;
}
