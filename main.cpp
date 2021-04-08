#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <unistd.h>
#include <stdlib.h>
#include <array>
#include <vector>
#include <random>
#include <fstream>

#define MCW MPI_COMM_WORLD

bool willBeInfected(int infectedCount){
    int chance = 25 * infectedCount;    //10% chance per person

    if(rand() % 100 < chance){
        return true;
    }else{
        return false;
    }
}

int getOddRank(int rank, int size){
    int number = 0;

    while(number % 2 == 0 || number == rank){
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
    int numberOfDays = 100;
    std::ofstream fout;

    if(rank == 0){
        person1[1] = 1;
        fout.open("testing.txt");
    }

    while(numberOfDays > 0){
        isDayOver = false;
        if((rank % 2) == 0){
            //homes
            MPI_Send(&person1[0], 4, MPI_INT, getOddRank(rank, size), 1, MCW);
            MPI_Send(&person2[0], 4, MPI_INT, getOddRank(rank, size), 1, MCW);
            MPI_Send(&person3[0], 4, MPI_INT, getOddRank(rank, size), 1, MCW);
            MPI_Send(&person4[0], 4, MPI_INT, getOddRank(rank, size), 1, MCW);
            numberOfPeople = 0;
            
            while(numberOfPeople < 4){
                MPI_Probe( MPI_ANY_SOURCE , 2 , MCW , &mystatus);
                switch(numberOfPeople){
                    case 0:
                    MPI_Recv(&person1[0], 4, MPI_INT, MPI_ANY_SOURCE, 2, MCW, &mystatus);
                    numberOfPeople += 1;
                    break;

                    case 1:
                    MPI_Recv(&person2[0], 4, MPI_INT, MPI_ANY_SOURCE, 2, MCW, &mystatus);
                    numberOfPeople += 1;
                    break;

                    case 2:
                    MPI_Recv(&person3[0], 4, MPI_INT, MPI_ANY_SOURCE, 2, MCW, &mystatus);
                    numberOfPeople += 1;
                    break;

                    case 3:
                    MPI_Recv(&person4[0], 4, MPI_INT, MPI_ANY_SOURCE, 2, MCW, &mystatus);
                    numberOfPeople += 1;
                    break;
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
            
            // check if each person gets infected or not
            if (person1[1] == 0 && numInfected > 0){
                if(willBeInfected(numInfected)){
                    person1[1] = 1;
                    numNormal -= 1;
                    numInfected += 1;
                }
            }
            if (person2[1] == 0 && numInfected > 0){
                if(willBeInfected(numInfected)){
                    person2[1] = 1;
                    numNormal -= 1;
                    numInfected += 1;
                }
            }
            if (person3[1] == 0 && numInfected > 0){
                if(willBeInfected(numInfected)){
                    person3[1] = 1;
                    numNormal -= 1;
                    numInfected += 1;
                }
            }
            if (person4[1] == 0 && numInfected > 0){
                if(willBeInfected(numInfected)){
                    person4[1] = 1;
                    numNormal -= 1;
                    numInfected += 1;
                }
            }

            // check if each person becomes immune after being sick for 3 days
            if (person1[1] == 1 && person1[3] < 4){
                person1[3] += 1;
                if(person1[3] >= 3){
                    person1[1] = 2;
                    numInfected -= 1;
                    numImmune += 1;
                }
            }
            if (person2[1] == 1 && person2[3] < 3){
                person2[3] += 1;
                if(person2[3] >= 3){
                    person2[1] = 2;
                    numInfected -= 1;
                    numImmune += 1;
                }
            }
            if (person3[1] == 1 && person3[3] < 3){
                person3[3] += 1;
                if(person3[3] >= 3){
                    person3[1] = 2;
                    numInfected -= 1;
                    numImmune += 1;
                }
            }
            if (person4[1] == 1 && person4[3] < 3){
                person4[3] += 1;
                if(person4[3] >= 3){
                    person4[1] = 2;
                    numInfected -= 1;
                    numImmune += 1;
                }
            }



            if(rank == 0){
                int totalNormal = numNormal;
                int totalInfected = numInfected;
                int totalImmune = numImmune;

                int normal;
                int infected;
                int immune;

                
                for (int i = 1; i < (size/2); ++i){
                    MPI_Probe( MPI_ANY_SOURCE , 6 , MCW , &mystatus);
                    MPI_Recv(&normal , 1 , MPI_INT , MPI_ANY_SOURCE , 6 , MCW , &mystatus);
                    totalNormal += normal;
                }

                for(int i = 1; i < (size/2); ++i){
                    MPI_Probe( MPI_ANY_SOURCE , 7 , MCW , &mystatus);
                    MPI_Recv(&infected , 1 , MPI_INT , MPI_ANY_SOURCE , 7 , MCW , &mystatus);
                    totalInfected += infected;
                }

                for(int i = 1; i < (size/2); ++i){
                    MPI_Probe( MPI_ANY_SOURCE , 8 , MCW , &mystatus);
                    MPI_Recv(&immune , 1 , MPI_INT , MPI_ANY_SOURCE , 8 , MCW , &mystatus);
                    totalImmune += immune;
                }

                fout << "Day " << numberOfDays << ": " << std::endl;
                fout << "Number of non-infected people: " << totalNormal << std::endl;
                fout << "Number of infected people: " << totalInfected << std::endl;
                fout << "Number of immune people: " << totalImmune << std::endl;
                fout << std::endl;
                fout << std::endl;

                std::cout << "Wrote day " << numberOfDays << "'s results to file" << std::endl;

            }
            else{
                MPI_Send( &numNormal , 1 , MPI_INT , 0 , 6 , MCW);
                MPI_Send( &numInfected , 1 , MPI_INT , 0 , 7 , MCW);
                MPI_Send( &numImmune , 1 , MPI_INT , 0 , 8 , MCW);
            }
        }
        else{
            //non-homes
            while(!isDayOver){
                //check for end of day message
                int dayEndFlag;
                MPI_Iprobe(MPI_ANY_SOURCE, 5, MCW, &dayEndFlag, MPI_STATUS_IGNORE);
                if(dayEndFlag){
                    MPI_Recv(&data[0], 4, MPI_INT, MPI_ANY_SOURCE, 5, MCW, MPI_STATUS_IGNORE);
                    isDayOver = true;
                    break;
                }

                //fill the building
                while(buildingCap.size() < 4) {
                    sleep(0.25);
                    MPI_Iprobe(MPI_ANY_SOURCE, 1, MCW, &messageFound, MPI_STATUS_IGNORE);

                    if (messageFound) {
                        MPI_Recv(&data[0], 4, MPI_INT, MPI_ANY_SOURCE, 1, MCW, MPI_STATUS_IGNORE);

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
                    std::vector<int> tempPerson ;
                    for(int i = 0; i < 4; ++i){
                        tempPerson.push_back(buildingCap[0][i]);
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

                    if(tempPerson[2] >= 3){
                        if(tempPerson[1] == 1){
                            infectedCount--;
                        }

                        //send home
                        MPI_Isend(&tempPerson[0], 4, MPI_INT, tempPerson[0], 2, MCW, &request);
                    }else{
                        if(tempPerson[1] == 1){
                            infectedCount--;
                        }
                        //send to another building
                        tempPerson[2] += 1;
                        MPI_Isend(&tempPerson[0], 4, MPI_INT, getOddRank(rank, size), 1, MCW, &request);
                    }
                }
            }
        }

        if(rank == 0){
            for(int i = 1; i < size; i += 2){
                MPI_Send(&data[0], 4, MPI_INT, i, 5, MCW);
            }
        }
        numberOfDays--;
        MPI_Barrier( MCW );
    }
    if(!rank){
        fout.close();
    }

    MPI_Finalize();

    return 0;
}
