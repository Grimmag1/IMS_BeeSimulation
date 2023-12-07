/*
TODO
    - user input
    - fix choosing of empty/flower from vector of empty/flower
*/

#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>

#define RADIUS_OF_HIVE              400     // in meters
#define MAX_TIME_OUTSIDE_OF_HIVE    25      // in minutes
#define NUM_OF_FLOWERS_TO_VISIT     80
#define PROB_OF_GOING_TO_FLOWER     0.7
#define PROB_OF_GOING_TO_EMPTY      0.2
#define PROB_OF_GOING_BACK 1-PROB_OF_GOING_TO_EMPTY-PROB_OF_GOING_TO_FLOWER
#define POLLUTING_TIME              10      // in seconds

using namespace std;

double calculateDistanceToHive(std::vector<long long unsigned int> * beePosition){
    int size = RADIUS_OF_HIVE*4;
    int a = abs((int)(*beePosition)[0] - size);
    int b = abs((int)(*beePosition)[1] - size);
    return (double)sqrt(a*a + b*b);
}

std::vector<int> findSurroundingWhenFlower (long long unsigned int * countOfFlowers, std::vector<int> * beeSurrounding, std::vector<std::vector<int>> * indexArray) {
    srand (static_cast <unsigned> (time(0)));
    
    double individualFlowerChance = 1.0 / (double)(*countOfFlowers);
    double probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    long long unsigned int chosenFlower;
    for (long long unsigned int i = 0; i < (*countOfFlowers)+1; i++){
        if (probability < individualFlowerChance * ((double)i+1.0)){
            chosenFlower = i;
            break;
        }
    }

    std::vector<std::vector<int>> indexes;
    for (long long unsigned int i = 0; i < (*beeSurrounding).size() + 1; i++){
        if ((*beeSurrounding)[i] == 1){
            indexes.push_back((*indexArray)[i]);
        }
    }
    return indexes[chosenFlower];
}

std::vector<int> findSurroundingWhenEmpty (long long unsigned int * countOfEmpty, std::vector<int> * beeSurrounding, std::vector<std::vector<int>> * indexArray) {
    srand (static_cast <unsigned> (time(0)));
    
    double individualEmptyChance = 1.0 / (double)(*countOfEmpty);
    double probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    long long unsigned int chosenEmpty;
    for (long long unsigned int i = 0; i < (*countOfEmpty)+1; i++){
        if (probability < individualEmptyChance * ((double)i+1.0)){
            chosenEmpty = i;
            break;
        }
    }

    std::vector<std::vector<int>> indexes;
    for (long long unsigned int i = 0; i < (*beeSurrounding).size() + 1; i++){
        if ((*beeSurrounding)[i] == 0){
            indexes.push_back((*indexArray)[i]);
        }
    }
    return indexes[chosenEmpty];
}

std::vector<int> findSurroundingWhenBack (std::vector<int> * beeSurrounding, std::vector<std::vector<int>> * indexArray){
    std::vector<std::vector<int>> indexes;
    for (long long unsigned int i = 0; i < (*beeSurrounding).size() + 1; i++){
        if ((*beeSurrounding)[i] == -1){
            indexes.push_back((*indexArray)[i]);
        }
    }
    return indexes[0];
}

int main(/*int argc, char** argv*/)
{
    srand (static_cast <unsigned> (time(0)));

    double outsideTemperature = 13.0;
    
    string location = "Gardens";
    /*
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            string arg = argv[i];
            if (arg == "-t" || arg == "--temperature") {
                if (i + 1 < argc) {
                    try {
                        outsideTemperature = stod(argv[i + 1]);
                    } catch (const exception& e) {
                        cout << "Value you input is not a number." << endl;
                        return 0;
                    }
                }
            } else if (arg == "-l" || arg == "--location") {
                if (i + 1 < argc) {
                    location = argv[i + 1];
                }
            }
        }
    }*/

    double chanceOfFlower;
    if (location == "Mixed") {
        chanceOfFlower = 0.2;
    } else if (location == "Gardens") {
        chanceOfFlower = 0.6;
    } else if (location == "Rapeseed oil") {
        chanceOfFlower = 0.9;
    } else if (location == "Poppy") {
        chanceOfFlower = 0.4;
    } else if (location == "Sunflower") {
        chanceOfFlower = 0.8;
    }

    double maxSpeedOfBee;
    if (outsideTemperature < 6.0){
        cout << "Too cold. Bees wont fly" << endl;
        return 0;
    }
    else if (outsideTemperature > 60.0){
        cout << "Too hot. Bees wont fly" << endl;
        return 0;
    }
    else if (outsideTemperature > 15.0){
        maxSpeedOfBee = 8;
    }
    else {  // outsideTemperature ∈ <6, 15>
        maxSpeedOfBee = outsideTemperature * (8.0/15.0);
    }

    double realSpeedOfBee = maxSpeedOfBee;
    long long unsigned int areaOfHive = (RADIUS_OF_HIVE * 8) + 1;
    std::vector<std::vector<int>> hiveArray(areaOfHive, std::vector<int>(areaOfHive, 0));
    std::vector<long long unsigned int> beePosition(2, RADIUS_OF_HIVE*4);
    std::vector<long long unsigned int> oldPosition(2, RADIUS_OF_HIVE*4);
    int visitedFlowersCount = 0;
    double maxTimeOutsideOfHive = MAX_TIME_OUTSIDE_OF_HIVE * 60;
    double realTimeOutsideOfHive = 0;
    double timeToReturnToHive = 0;
    double travelTimeBetweenPlaces = 0.25 / realSpeedOfBee;

    float probability;
    for (long long unsigned int i = 0; i<areaOfHive; i++){
        for (long long unsigned int j = 0; j<areaOfHive; j++){
            probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            if (probability < chanceOfFlower){
                hiveArray[i][j] = 1;
            }
            else {
                hiveArray[i][j] = 0;
            }
        }
    }

    hiveArray[beePosition[0]][beePosition[1]] = -1;

    std::vector<std::vector<int>> indexArray = {{-1, -1}, {-1, 0}, {-1, 1},
                                                {0, -1} ,          {0, 1},
                                                {1, -1} , {1, 0} , {1, 1} };
    std::vector<int> chosenSurrounding = {0, 0};
    

    while ((visitedFlowersCount < NUM_OF_FLOWERS_TO_VISIT) && (realTimeOutsideOfHive + timeToReturnToHive <= maxTimeOutsideOfHive)){
        realSpeedOfBee = (maxSpeedOfBee - (visitedFlowersCount * (maxSpeedOfBee / (NUM_OF_FLOWERS_TO_VISIT *2))));
        travelTimeBetweenPlaces = 0.25 / realSpeedOfBee;
        realTimeOutsideOfHive += travelTimeBetweenPlaces;
        
        timeToReturnToHive = calculateDistanceToHive(&beePosition) / realSpeedOfBee;

        std::vector<int> beeSurrounding = { hiveArray[beePosition[0] - 1][beePosition[1] - 1],
                                            hiveArray[beePosition[0] - 1][beePosition[1] - 0],
                                            hiveArray[beePosition[0] - 1][beePosition[1] + 1],
                                            hiveArray[beePosition[0] - 0][beePosition[1] - 1],
                                            hiveArray[beePosition[0] - 0][beePosition[1] + 1],
                                            hiveArray[beePosition[0] + 1][beePosition[1] - 1],
                                            hiveArray[beePosition[0] + 1][beePosition[1] - 0],
                                            hiveArray[beePosition[0] + 1][beePosition[1] + 1] };

        long long unsigned int countOfFlowers = 0;
        long long unsigned int countOfEmpty = 0;
        long long unsigned int countOfBacks = 0;
        for (long long unsigned int i = 0; i<8; i++){
            switch (beeSurrounding[i]){
                case -1:
                    countOfBacks++;
                    break;
                case 0:
                    countOfEmpty++;
                    break;
                case 1:
                    countOfFlowers++;
                    break;
            }
        }

        probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        
        if (countOfBacks == 0){

            if (countOfFlowers == 0){
                chosenSurrounding = findSurroundingWhenEmpty(&countOfEmpty,&beeSurrounding,&indexArray);
            }
            else if (countOfEmpty == 0){
                visitedFlowersCount++;
                realTimeOutsideOfHive += POLLUTING_TIME;
                chosenSurrounding = findSurroundingWhenFlower(&countOfFlowers,&beeSurrounding,&indexArray);
            }
            else if (probability < PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_BACK/2){
                visitedFlowersCount++;
                realTimeOutsideOfHive += POLLUTING_TIME;
                chosenSurrounding = findSurroundingWhenFlower(&countOfFlowers,&beeSurrounding,&indexArray);
            }
            else { // (probability <= PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY + PROB_OF_GOING_BACK)
                chosenSurrounding = findSurroundingWhenEmpty(&countOfEmpty,&beeSurrounding,&indexArray);
            }

        }

        else if ((countOfFlowers != 0) && (countOfEmpty != 0)){

            if (probability < PROB_OF_GOING_TO_FLOWER){
                visitedFlowersCount++;
                realTimeOutsideOfHive += POLLUTING_TIME;
                chosenSurrounding = findSurroundingWhenFlower(&countOfFlowers,&beeSurrounding,&indexArray);
            }
            else if (probability < PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY){
                chosenSurrounding = findSurroundingWhenEmpty(&countOfEmpty,&beeSurrounding,&indexArray);
            }
            else { // (probability <= PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY + PROB_OF_GOING_BACK)
                chosenSurrounding = findSurroundingWhenBack(&beeSurrounding,&indexArray);
            }

        }

        else if (countOfFlowers == 0){
            
            if (probability < PROB_OF_GOING_TO_EMPTY + PROB_OF_GOING_TO_FLOWER/2){
                chosenSurrounding = findSurroundingWhenEmpty(&countOfEmpty,&beeSurrounding,&indexArray);
            }
            else { // (probability <= PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY + PROB_OF_GOING_BACK)
                chosenSurrounding = findSurroundingWhenBack(&beeSurrounding,&indexArray);
            }

        }

        else if (countOfEmpty == 0){

            if (probability < PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY/2){
                visitedFlowersCount++;
                realTimeOutsideOfHive += POLLUTING_TIME;
                chosenSurrounding = findSurroundingWhenFlower(&countOfFlowers,&beeSurrounding,&indexArray);
            }
            else { // (probability <= PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY + PROB_OF_GOING_BACK)
                chosenSurrounding = findSurroundingWhenBack(&beeSurrounding,&indexArray);
            }
        }

        hiveArray[oldPosition[0]][oldPosition[1]] = 0;
        hiveArray[beePosition[0]][beePosition[1]] = -1;
        oldPosition[0] = beePosition[0];
        oldPosition[1] = beePosition[1];
        beePosition[0] = beePosition[0] + (long long unsigned int)chosenSurrounding[(int)0];
        beePosition[1] = beePosition[1] + (long long unsigned int)chosenSurrounding[(int)1];
    }
    cout << "Number of Visited Flowers: " << visitedFlowersCount << endl;
    cout << "Time spent outside of the hive: " << (realTimeOutsideOfHive + timeToReturnToHive) / 60.0 << " minutes" << endl;
    return 0;
}