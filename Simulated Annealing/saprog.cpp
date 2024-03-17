#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <math.h>
using namespace std;

int noOfCells;
int noOfConnections;
vector< vector<int> > connectivityMatrix;

void ConnectivityMatrix(ifstream &benchMarkFile);
int RandomIndex(vector<int> *currentPartition);
int GainModify(int elementIndexA,int elementIndexB,vector<int> *initialCellVector);
void InitialSol(int cellCount,vector<int> *initialCellVector);
bool acceptMove(int gain, int temperature);
int SAalgorithm(vector<int> *initialCellVector);
int GetCutset(vector<int> *initialCellVector);

int main(int argumentCount,char *fileName[]){
    vector<int> initialCellVector;
    vector<int> partition_A;
    vector<int> partition_B;
    int cutSet = 0;

    ifstream benchMarkFile(fileName[1]);

    ofstream outputFile(fileName[2]);
    unsigned seed = time(0);
    clock_t initialTime, finalTime;
    initialTime = clock();
    srand(seed);
	outputFile<<"seed: "<<seed<<endl;
    
    if(benchMarkFile.is_open()){
        ConnectivityMatrix(benchMarkFile);  // formulate connectivity matrix
        InitialSol(noOfCells,&initialCellVector);   // random partition for first time
        cutSet = SAalgorithm(&initialCellVector);   // run SA algorithm
	outputFile<<"cutsetSize: "<<cutSet<<endl;
        outputFile<<"final Partitions:"<<endl;
        int initialCellVectorSize = initialCellVector.size();
        outputFile<<"partition A:"<<endl;
        for(int i=0; i<initialCellVectorSize;i++){
            if(initialCellVector.at(i)==0){
                outputFile<<i+1<<" ";
            }
        }
        outputFile<<endl<<"partition B:"<<endl;
        for(int i=0; i<initialCellVectorSize;i++){
            if(initialCellVector.at(i)==1){
                outputFile<<i+1<<" ";
            }
        }
        outputFile<<endl;
        finalTime = clock();
        outputFile<<"Execution Time: "<<(double)(finalTime-initialTime)/CLOCKS_PER_SEC<<" seconds." << endl;


    }else{
        cout<<"File could not be opened.\n";
    }
  return 0;
}

void ConnectivityMatrix(ifstream &benchMarkFile){
    int node1,node2;
    benchMarkFile>>noOfCells;
    benchMarkFile>>noOfConnections;
    for(int i=0;i<noOfCells;i++){
        connectivityMatrix.push_back(vector<int>());
    }
    while(benchMarkFile >> node1 >> node2){
        connectivityMatrix[node1-1].push_back(node2-1);
        connectivityMatrix[node2-1].push_back(node1-1);
    }
}

void InitialSol(int cellCount,vector<int> *initialCellVector){
    for(int i=1;i<=cellCount/2;i++){    // dividing the cells in two parts
		initialCellVector->push_back(0);    // assigning 0 to half the cells
		initialCellVector->push_back(1);    // assigning 1 to half the cells
	}
	random_shuffle(initialCellVector->begin(), initialCellVector->end());   // randomising the assigned 0's & 1's
}

int SAalgorithm(vector<int> *initialCellVector){
    int StartTemperature = 40000;  // Assigning T
    int StopTemperature = 10;
    int temperature;
    int gain;
    int elementIndexA =0;
    int elementIndexB =0;
    int tempForSwap;
    int StepsForIteration = 500000;
    int cutSet =0;
    bool accept = false;
    temperature = StartTemperature;
    cutSet = GetCutset(initialCellVector);
    while(temperature >=StopTemperature){
        for(int i=0;i<StepsForIteration;i++){
            elementIndexA = RandomIndex(initialCellVector);     //Taking random index from A
            elementIndexB = RandomIndex(initialCellVector);     //Taking random index from B
            while(initialCellVector->at(elementIndexA) == initialCellVector->at(elementIndexB)){
                elementIndexB = RandomIndex(initialCellVector);
            }
            gain = GainModify(elementIndexA,elementIndexB,initialCellVector);   //Obtain new gain
            accept = acceptMove(gain,temperature);  //Check whether to accept the move or not
            if(accept){
                tempForSwap = initialCellVector->at(elementIndexA);
                initialCellVector->at(elementIndexA) = initialCellVector->at(elementIndexB);
                initialCellVector->at(elementIndexB) = tempForSwap; // swaping A and B
                cutSet = cutSet-gain;   //updating Cutset
            }
       //      cout<<" iteration:"<<i<<" cutset:"<<cutSet<<endl;
       
	    }
        temperature = 0.99*temperature;
        cout<<"Temperature:"<<temperature<<" CutSet:"<<cutSet<<endl;
    }
    return cutSet;
}

int GainModify(int elementIndexA,int elementIndexB,vector<int> *initialCellVector){ //New gain after swap
    int Ea = 0;
    int Eb = 0;
    int Ia = 0;
    int Ib = 0;
    int Cab = 0;
    int Da = 0;
    int Db = 0;
    int Gab =0;
    int elementIndexARowSize = 0;
    int elementIndexBRowSize = 0;
    elementIndexARowSize = connectivityMatrix[elementIndexA].size();
    elementIndexBRowSize = connectivityMatrix[elementIndexB].size();
    for(int i=0;i<elementIndexARowSize;i++){
        if(initialCellVector->at(elementIndexA)==initialCellVector->at(connectivityMatrix[elementIndexA][i])){
            Ia += 1;
        }else{
            Ea += 1;
        }
    }
    for(int i=0;i<elementIndexBRowSize;i++){
        if(initialCellVector->at(elementIndexB)==initialCellVector->at(connectivityMatrix[elementIndexB][i])){
            Ib += 1;
        }else{
            Eb += 1;
        }
    }
    if(elementIndexARowSize<elementIndexBRowSize){
        for(int i=0;i<elementIndexARowSize;i++){
            if(connectivityMatrix[elementIndexA][i] == elementIndexB){
                Cab += 1;
            }
        }
    }else{
        for(int i=0;i<elementIndexBRowSize;i++){
            if(connectivityMatrix[elementIndexB][i] == elementIndexA){
                Cab += 1;
            }
        }
    }
    Da = Ea-Ia;
    Db = Eb-Ib;
    Gab = Da+Db-2*Cab;
    return Gab;
}


int RandomIndex(vector<int> *currentPartition){     //Generating ramdom index to select for swap
	int randomElementIndex = ((double)rand()*(currentPartition->size()-1))/(double)RAND_MAX;
	return randomElementIndex;
}

int GetCutset(vector<int> *initialCellVector){
    int cutSetSize = 0;
    int elementOfPartition_A;
    int elementOfPartition_B;
    int initialCellVectorSize = initialCellVector->size();
    int rowSize = 0;
    vector<int> tempRowVector;
    for(int i=0;i<initialCellVectorSize;i++){
        elementOfPartition_A = initialCellVector->at(i);
        tempRowVector = connectivityMatrix.at(i);
        rowSize = tempRowVector.size();
        for(int j=0;j<rowSize;j++){
            elementOfPartition_B = initialCellVector->at(tempRowVector.at(j));
            if(elementOfPartition_A!=elementOfPartition_B){
                cutSetSize = cutSetSize+1;
            }
        }
    }
    return cutSetSize/2;
}

bool acceptMove(int gain, int temperature){     // check whether or not to accept the move
	double boltz;
	double random;
	double exponent;
	double kT;
	const double k = 1.380658e-23;
	if(gain<0){
		kT = k*temperature;
		exponent = gain/kT;
		boltz = exp(exponent);
		random = ((double)rand())/(double)RAND_MAX;
		if(random<boltz)
            return true;
	 	return false;
	}
	return true;
}
