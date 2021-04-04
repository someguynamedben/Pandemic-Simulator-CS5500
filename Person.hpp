#ifndef PROJECT_PERSON_HPP
#define PROJECT_PERSON_HPP


class Person {
public:
    Person(bool isSick, int homeId);

    bool getIsImmune();
    bool getIsSick();
    int getHomeID();
    int m_stopCount;

private:
    int m_homeID;
    bool m_isSick;
    bool m_isImmune;
};


#endif //PROJECT_PERSON_HPP
