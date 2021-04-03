#include "Building.hpp"

Building::Building(bool isAHome) {
    m_isAHome = isAHome;
}

bool Building::isFull() {
    if(m_capacityArray.size() >= 4){
        return true;
    }else{
        return false;
    }
}

int Building::getID() {
    return m_buildingID;
}

bool Building::getIsAHome() {
    return m_isAHome;
}
