#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <array>
#include <vector>
//#include <random>
#include <fstream>

#define MCW MPI_COMM_WORLD

bool willBeInfected(int infectedCount){
    int chance = 10 * infectedCount;    //10% chance per person

    if(rand() % 100 < chance){
        return true;
    }else{
        return false;
    }
}

int getOddRank(int size){
    int number = 0;

    while(number % 2 == 0){
        number = rand() % size;
    }

    return number;
}

int main(int argc, char **argv){
    int rank, size;
    std::vector<int> data = {0, 0, 0, 0};
    bool isDayOver;
    int infectedCount = 0;
    std::vector<std::vector<int>> buildingCap;
    int messageFound;
    MPI_Request request;
    MPI_Status mystatus;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    //seed rand
    srand (time(NULL) + rank);

    std::vector<int> person1 = {rank,0,0,0}; // homeId, Status: 0 = isNormal 1 = isSick 2 =isImmune, stopCount, daysInfected
    std::vector<int> person2 = {rank,0,0,0};
    std::vector<int> person3 = {rank,0,0,0};
    std::vector<int> person4 = {rank,0,0,0};

    int numberOfPeople;
    int numberOfDays = 10;
    std::ofstream fout;

    if(rank == 0){
        fout.open("testing.txt");
    }

    while(numberOfDays > 0){
        isDayOver = false;
        std::cout << "NUMBER OF DAYS: " << numberOfDays << std::endl;
        if((rank % 2) == 0){
            if(rank == 0){
                person1[1] = 1;
            }
            //homes
            MPI_Send(&person1[0], 4, MPI_INT, getOddRank(size), 1, MCW);
            MPI_Send(&person2[0], 4, MPI_INT, getOddRank(size), 1, MCW);
            MPI_Send(&person3[0], 4, MPI_INT, getOddRank(size), 1, MCW);
            MPI_Send(&person4[0], 4, MPI_INT, getOddRank(size), 1, MCW);
            numberOfPeople = 0;

            std::cout << "sent people out" << std::endl;

            int returnerFlag = 0;
            while(!returnerFlag || numberOfPeople < 4){
                MPI_Iprobe( MPI_ANY_SOURCE , 2 , MCW , &returnerFlag, &mystatus);
                while(returnerFlag && numberOfPeople < 4){
                    std::cout << "line 74" << std::endl;
                    switch(numberOfPeople){
                        case 0:
                        MPI_Recv(&person1[0], 4, MPI_INT, MPI_ANY_SOURCE, 2, MCW, &mystatus);
                        break;

                        case 1:
                        MPI_Recv(&person2[0], 4, MPI_INT, MPI_ANY_SOURCE, 2, MCW, &mystatus);
                        break;

                        case 2:
                        MPI_Recv(&person3[0], 4, MPI_INT, MPI_ANY_SOURCE, 2, MCW, &mystatus);
                        break;

                        case 3:
                        MPI_Recv(&person4[0], 4, MPI_INT, MPI_ANY_SOURCE, 2, MCW, &mystatus);
                        break;
                    }
                    std::cout << "line 92" << std::endl;
                    numberOfPeople += 1;
                    MPI_Iprobe( MPI_ANY_SOURCE , 2 , MCW , &returnerFlag, &mystatus);
                }
            }

            int numNormal =  0;
            int numInfected = 0;
            int numImmune = 0;
            switch(person1[1]){
                case 0:
                numNormal += 1;
                break;

                case 1:
                numInfected += 1;
                break;

                case 2:
                numImmune += 1;
            }
            switch(person2[1]){
                case 0:
                numNormal += 1;
                break;

                case 1:
                numInfected += 1;
                break;

                case 2:
                numImmune += 1;
            }
            switch(person3[1]){
                case 0:
                numNormal += 1;
                break;

                case 1:
                numInfected += 1;
                break;

                case 2:
                numImmune += 1;
            }
            switch(person4[1]){
                case 0:
                numNormal += 1;
                break;

                case 1:
                numInfected += 1;
                break;

                case 2:
                numImmune += 1;
            }
            
            // check if each is infected or not
            if (person1[1] == 0){
                if(willBeInfected(numInfected)){
                    person1[1] = 1;
                    numNormal -= 1;
                    numInfected += 1;
                }
            }
            if (person2[1] == 0){
                if(willBeInfected(numInfected)){
                    person2[1] = 1;
                    numNormal -= 1;
                    numInfected += 1;
                }
            }
            if (person3[1] == 0){
                if(willBeInfected(numInfected)){
                    person3[1] = 1;
                    numNormal -= 1;
                    numInfected += 1;
                }
            }
            if (person4[1] == 0){
                if(willBeInfected(numInfected)){
                    person4[1] = 1;
                    numNormal -= 1;
                    numInfected += 1;
                }
            }

            if(rank == 0){
                int totalNormal = numNormal;
                int totalInfected = numInfected;
                int totalImmune = numImmune;

                int normal;
                int infected;
                int immune;

                MPI_Probe( MPI_ANY_SOURCE , 6 , MCW , &mystatus);
                int normFlag = 1;
                while(normFlag){
                    std::cout << "line 198" << std::endl;
                    MPI_Recv(&normal , 1 , MPI_INT , MPI_ANY_SOURCE , 6 , MCW , &mystatus);
                    totalNormal += normal;
                    MPI_Iprobe( MPI_ANY_SOURCE , 6 , MCW , &normFlag , &mystatus);
                }

                std::cout << "line 204" << std::endl;

                MPI_Probe( MPI_ANY_SOURCE , 7 , MCW , &mystatus);
                int infFlag = 1;
                while(infFlag){
                    std::cout << "line 209" << std::endl;
                    MPI_Recv(&infected , 1 , MPI_INT , MPI_ANY_SOURCE , 7 , MCW , &mystatus);
                    totalInfected += infected;
                    MPI_Iprobe( MPI_ANY_SOURCE , 7 , MCW , &infFlag , &mystatus);
                }

                std::cout << "line 215" << std::endl;

                MPI_Probe( MPI_ANY_SOURCE , 8 , MCW , &mystatus);
                int immFlag = 1;
                while(immFlag){
                    std::cout << "line 220" << std::endl;
                    MPI_Recv(&immune , 1 , MPI_INT , MPI_ANY_SOURCE , 8 , MCW , &mystatus);
                    totalImmune += immune;
                    MPI_Iprobe( MPI_ANY_SOURCE , 8 , MCW , &immFlag , &mystatus);
                }
                std::cout << "225" << std::endl;

                fout << "Day " << numberOfDays << ": " << std::endl;
                fout << "Number of non-infected people: " << totalNormal << std::endl;
                fout << "Number of infected people: " << totalInfected << std::endl;
                fout << "Number of immune people: " << totalImmune << std::endl;
                fout << std::endl;
                fout << std::endl;

            }
            else{
                std::cout << "line 236" << std::endl;
                MPI_Send( &numNormal , 1 , MPI_INT , 0 , 6 , MCW);
                MPI_Send( &numInfected , 1 , MPI_INT , 0 , 7 , MCW);
                MPI_Send( &numImmune , 1 , MPI_INT , 0 , 8 , MCW);
                std::cout << "line 240" << std::endl;
            }
        }
        else{
            //non-homes
            while(isDayOver != true){
                //check for end of day message
                int dayEndFlag;
                MPI_Iprobe(MPI_ANY_SOURCE, 5, MCW, &dayEndFlag, MPI_STATUS_IGNORE);
                if(dayEndFlag){
                    if(rank ==1){
                        std::cout << "line 251" << std::endl;
                    }
                    MPI_Recv(&data[0], 4, MPI_INT, MPI_ANY_SOURCE, 5, MCW, MPI_STATUS_IGNORE);
                    isDayOver = true;
                    break;
                }
//                std::cout << "line 263" << std::endl;

                //fill the building
                while(buildingCap.size() < 4) {
                    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, &messageFound, MPI_STATUS_IGNORE);
//                    std::cout << rank << " got Message status: " << messageFound << std::endl;

                    if (messageFound) {
                        if(rank ==1){
                            std::cout << "line 264" << std::endl;
                        }
                        MPI_Recv(&data[0], 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, MPI_STATUS_IGNORE);

                        if(rank ==1){
                            std::cout << "line 267" << std::endl;
                        }

                        if(data[1] == 1){
                            infectedCount += 1;
                        }

                        std::cout << "Adding TO QUEUE~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
                        buildingCap.push_back(data);
                        std::cout << "ADDED TO QUEUE Rank:" << rank << " holds " << buildingCap.size() << std::endl;
                    }else{
                        break;
                    }
                }


                //send a person from the building queue
                if(!buildingCap.empty()){
                    std::vector<int> tempPerson = {0, 0, 0, 0};
                    for(int i = 0; i < 4; ++i){
                        tempPerson[i] = buildingCap[0][i];
                    }
                    
                    buildingCap.erase(buildingCap.begin());

                    //calculate if tempPerson will be infected
                    if(tempPerson[1] == 0 and infectedCount > 0){
                        if(willBeInfected(infectedCount)){
                            if(tempPerson[1] == 0) {
                                tempPerson[1] = 1;
                            }
                        }
                    }
                    if(rank ==1){
                        std::cout << "line 301" << std::endl;
                    }
                    if(tempPerson[2] >= 3){
                        if(tempPerson[1] == 1){
                            infectedCount--;
                        }
                        //send home
                        MPI_Isend(&tempPerson[0], 4, MPI_INT, tempPerson[0], 0, MCW, &request);
                    }else{
                        if(tempPerson[1] == 1){
                            infectedCount--;
                        }
                        //send to another building
                        tempPerson[2] += 1;
                        MPI_Isend(&tempPerson[0], 4, MPI_INT, getOddRank(size), 0, MCW, &request);
                    }
                    if(rank ==1){
                        std::cout << "line 316" << std::endl;
                    }
                }
            }
        }

        std::cout << "Sending Kill code" << std::endl;
        if(rank == 0){
            for(int i = 1; i < size; i += 2){
                std::cout << "sending to: " << i << std::endl;
                MPI_Send(&data[0], 4, MPI_INT, i, 5, MCW);
            }
        }

        numberOfDays--;
        MPI_Barrier( MCW );
    }

    MPI_Finalize();

    return 0;
}
