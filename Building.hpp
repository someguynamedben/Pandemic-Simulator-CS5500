#ifndef PROJECT_BUILDING_HPP
#define PROJECT_BUILDING_HPP


#include <vector>

class Building {
public:
    Building(bool isAHome);

    bool isFull();
    int getID();
    bool getIsAHome();

private:
    std::vector<Person> m_capacityArray[4];
    bool m_isAHome;
    int m_buildingID;
};


#endif //PROJECT_BUILDING_HPP
