#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <array>
#include <vector>

#define MCW MPI_COMM_WORLD

bool willBeInfected(int infectedCount){
    int chance = 10 * infectedCount;    //10% chance per person

    if(rand() % 100 < chance){
        return true;
    }else{
        return false;
    }
}

int main(int argc, char **argv){
    int rank, size;
    int data[4];
    bool isDayOver = false;
    int numBuildings = 10;
//    int buildingCap = 0;
    int infectedCount = 0;
    std::vector<std::array<int, 4>> buildingCap;
    int messageFound;
    MPI_Request request;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    //seed rand
    srand (time(NULL) + rank);

    if((rank % 2) == 0){
        //homes
    }else{
        //non-homes
        while(isDayOver != true){
            //check for end of day message
            MPI_Iprobe(MPI_ANY_SOURCE, 5, MCW, &messageFound, MPI_STATUS_IGNORE);

            if(messageFound){
                MPI_Recv(data, 1, MPI_INT, MPI_ANY_SOURCE, 5, MCW, MPI_STATUS_IGNORE);
                isDayOver = true;
            }

            //fill the building
            while(buildingCap.size() < 4) {
                MPI_Iprobe(MPI_ANY_SOURCE, !5, MCW, &messageFound, MPI_STATUS_IGNORE);

                if (messageFound) {
                    MPI_Recv(data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE); // TODO: update variable

                    if(data[1] == 1){
                        infectedCount += 1;
                    }

                    buildingCap.push_back(data);
                }else{
                    break;
                }
            }

            //send a person from the building queue
            if(!buildingCap.empty()){
                int temp[4];
                for(int i = 0; i < 4; ++i){
                    temp[i] = buildingCap[0][i];
                }

                buildingCap.erase(buildingCap.begin());

                //calculate if person will be infected
                if(temp[1] == 0 and infectedCount > 0){
                    if(willBeInfected(infectedCount)){
                        temp[1] = 1;
                    }
                }

                if(temp[2] >= 3){
                    //send home
                    MPI_Isend(&temp, 1, MPI_INT, temp[0], 0, MCW, &request);
                }else{
                    //send to another building
                    MPI_Isend(&temp, 1, MPI_INT, ((2 * rand()) % size) + 1, 0, MCW, &request);
                }
            }
        }
    }

    MPI_Finalize();

    return 0;
}
