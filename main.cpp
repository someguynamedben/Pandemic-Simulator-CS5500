#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <random>
#include <fstream>

#define MCW MPI_COMM_WORLD

using namespace std;

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
    int data;
    MPI_Status mystatus;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    srand(rank);

    int person1[3] = {rank,0,0,0}; // homeid, Status: 0 = isNormal 1 = isSick 2 =isImmune, stopCount, daysInfected
    int person2[3] = {rank,0,0,0};
    int person3[3] = {rank,0,0,0};
    int person4[3] = {rank,0,0,0};

    int numberOfPeople;
    int numberOfDays = 100;

    if(rank == 0){
        ofstream fout;
        fout.open("testing.ppm");
    }

    while(numberOfDays > 0){
        if((rank % 2) == 0){
            //homes
            int dest =  1 + ((2 * rand()) % size);
            MPI_Send(person1, 4, MPI_INT, dest, 1, MCW);
            dest =  1 + ((2 * rand()) % size); 
            MPI_Send(person2, 4, MPI_INT, dest, 2, MCW);
            dest =  1 + ((2 * rand()) % size);
            MPI_Send(person3, 4, MPI_INT, dest, 3, MCW);
            dest =  1 + ((2 * rand()) % size);
            MPI_Send(person4, 4, MPI_INT, dest, 4, MCW);
            numberOfPeople = 0;
            
            int returnerFlag = 0;
            while(!returnerFlag){
                MPI_IProbe( MPI_ANY_SOURCE , 0 , MCW , &returnerFlag, &mystatus);
                while(returnerFlag){
                    switch(numberOfPeople){
                        case 0:
                        MPI_Recv(person1, 4, MPI_INT, MPI_ANY_TAG, MCW, &mystatus);
                        break;

                        case 1:
                        MPI_Recv(person2, 4, MPI_INT, MPI_ANY_TAG, MCW, &mystatus);
                        break;

                        case 2:
                        MPI_Recv(person3, 4, MPI_INT, MPI_ANY_TAG, MCW, &mystatus);
                        break;

                        case 3:
                        MPI_Recv(person4, 4, MPI_INT, MPI_ANY_TAG, MCW, &mystatus);
                        break;
                    }
                    numberOfPeople += 1;
                    MPI_IProbe( MPI_ANY_SOURCE , 0 , MCW , &returnerFlag, &mystatus);
                }
                returnerFlag = 1;
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
                    person1[1] == 1;
                    numNormal -= 1;
                    numInfected += 1;
                }
            }
            if (person2[1] == 0){
                if(willBeInfected(numInfected)){
                    person2[1] == 1;
                    numNormal -= 1;
                    numInfected += 1;
                }
            }
            if (person3[1] == 0){
                if(willBeInfected(numInfected)){
                    person3[1] == 1;
                    numNormal -= 1;
                    numInfected += 1;
                }
            }
            if (person4[1] == 0){
                if(willBeInfected(numInfected)){
                    person4[1] == 1;
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

                int normFlag;
                MPI_Iprobe( MPI_ANY_SOURCE , 6 , MCW , &normFlag , &mystatus);
                while(normFlag){
                    MPI_Recv(&normal , 1 , MPI_INT , MPI_ANY_SOURCE , 6 , MCW , &mystatus);
                    totalNormal += normal;
                    MPI_Iprobe( MPI_ANY_SOURCE , 6 , MCW , &normFlag , &mystatus);
                }

                int infFlag;
                MPI_Iprobe( MPI_ANY_SOURCE , 7 , MCW , &infFlag , &mystatus);
                while(infFlag){
                    MPI_Recv(&infected , 1 , MPI_INT , MPI_ANY_SOURCE , 7 , MCW , &mystatus);
                    totalInfected += infected;
                    MPI_Iprobe( MPI_ANY_SOURCE , 7 , MCW , &infFlag , &mystatus);
                }

                int immFlag;
                MPI_Iprobe( MPI_ANY_SOURCE , 8 , MCW , &immFlag , &mystatus);
                while(immFlag){
                    MPI_Recv(&immune , 1 , MPI_INT , MPI_ANY_SOURCE , 8 , MCW , &mystatus);
                    totalImmune += immune;
                    MPI_Iprobe( MPI_ANY_SOURCE , 8 , MCW , &immFlag , &mystatus);
                }

                fout << "Day " << numberOfDays << ": " << endl;
                fout << "Number of non-infected people: " << totalNormal << endl;
                fout << "Number of infected people: " << totalInfected << endl;
                fout << "Number of immune people: " << totalImmune << endl;
                fout << endl;
                fout << endl;

            }
            else{
                MPI_Send( &numNormal , 1 , MPI_INT , 0 , 6 , MCW);
                MPI_Send( &numInfected , 1 , MPI_INT , 0 , 7 , MCW);
                MPI_Send( &numImmune , 1 , MPI_INT , 0 , 8 , MCW);
            }
        }
        else{
            //non-homes

        }

        MPI_Barrier( MCW );
    }

    MPI_Finalize();

    return 0;
}
