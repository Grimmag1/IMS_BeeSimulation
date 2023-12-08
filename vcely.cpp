/*
TODO
    - fix choosing of empty/flower from vector of empty/flower
*/

#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <getopt.h>

#define RADIUS_OF_HIVE              400     // in meters
#define MAX_TIME_OUTSIDE_OF_HIVE    25      // in minutes
#define NUM_OF_FLOWERS_TO_VISIT     80
#define PROB_OF_GOING_TO_FLOWER     0.7
#define PROB_OF_GOING_TO_EMPTY      0.2
#define PROB_OF_GOING_BACK 1-PROB_OF_GOING_TO_EMPTY-PROB_OF_GOING_TO_FLOWER
#define POLLUTING_TIME              10      // in seconds

#define LLUI long long unsigned int

using namespace std;

double calculateDistanceToHive(vector<int> * beePosition){
    int size = RADIUS_OF_HIVE*4;
    int a = abs((*beePosition)[0] - size);
    int b = abs((*beePosition)[1] - size);
    return (double)sqrt(a*a + b*b);
}





int main(int argc, char** argv)
{
    srand (static_cast <unsigned> (time(0)));

    double outsideTemperature = 13.0;
    
    string location = "Lindens";
    /*PARSING*/
    int option_index;
    int c;
    char* temperature = nullptr;
    struct option long_options[] =
    {
        {"location", optional_argument, NULL, 'l'},
        {"temperature", required_argument, NULL, 't'},
        {0, 0, 0, 0}
    };
    while ((c = getopt_long(argc, argv, "t:l:", long_options,&option_index)) != -1)
    {
        switch (c)
        {
            /*file*/
            case 't':
            {
                temperature = optarg;
                //cout << temperature << endl;
                break;
            }
            /*port*/
            case 'l':
            {
                location = optarg;
                //cout << location << endl;
                break;
            }
            case '?':
            {
                cerr << "UNKOWN OPTION. Usage: ./bees -t {temperature} -l {location}" << endl;
                cerr << "Location list: [Lindens,Mixed,Gardens,Rapeseed_oil,Poppy,Sunflower], Default: Lindens" << endl;
                exit(-1);
            }case 'h':
            {
                cerr << "Usage: ./bees -t {temperature} -l {location}" << endl;
                cerr << "Location list: [Lindens,Mixed,Gardens,Rapeseed_oil,Poppy,Sunflower], Default: Lindens" << endl;
                exit(-1);
            }
            default:
            {
                cerr << "UKNOWN OPTION. Usage: ./bees -t {temperature} -l {location}" << endl;
                cerr << "Location list: [Lindens,Mixed,Gardens,Rapeseed_oil,Poppy,Sunflower], Default location: Lindens, Default temperature: 15°C" << endl;
                exit(-1);
            }
        }
    }
    double chanceOfFlower;
    if (location == "Lindens") {
        chanceOfFlower = 0.7;
    } else if (location == "Mixed") {
        chanceOfFlower = 0.2;
    } else if (location == "Gardens") {
        chanceOfFlower = 0.6;
    } else if (location == "Rapeseed_oil") {
        chanceOfFlower = 0.9;
    } else if (location == "Poppy") {
        chanceOfFlower = 0.4;
    } else if (location == "Sunflower") {
        chanceOfFlower = 0.8;
    } else { //Lindens
        cerr << "WRONG Location input." << endl;
        exit(-1);
    }
    if (temperature){
        outsideTemperature = stoi(temperature);
        //cout << outsideTemperature << endl;
    } else {
        outsideTemperature = 15;
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
    LLUI areaOfHive = (RADIUS_OF_HIVE * 8) + 1;
    vector<vector<int>> hiveArray(areaOfHive, vector<int>(areaOfHive, 0));
    vector<int> beePosition(2, RADIUS_OF_HIVE*4);
    vector<int> oldPosition(2, RADIUS_OF_HIVE*4);
    int visitedFlowersCount = 0;
    double maxTimeOutsideOfHive = MAX_TIME_OUTSIDE_OF_HIVE * 60;
    double realTimeOutsideOfHive = 0;
    double timeToReturnToHive = 0;
    double travelTimeBetweenPlaces = 0.25 / realSpeedOfBee;

    float probability;
    for (LLUI i = 0; i<areaOfHive; i++){
        for (LLUI j = 0; j<areaOfHive; j++){
            probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            if (probability < chanceOfFlower){
                hiveArray[i][j] = 1;
            }
            else {
                hiveArray[i][j] = 0;
            }
        }
    }

    hiveArray[(LLUI)(beePosition[0])][(LLUI)(beePosition[1])] = -1;

    vector<vector<int>> indexArray = {{-1, -1}, {-1, 0}, {-1, 1},
                                                {0, -1} ,          {0, 1},
                                                {1, -1} , {1, 0} , {1, 1} };
    vector<int> chosenSurrounding = {0, 0};

    double individualFlowerChance;
    double individualEmptyChance;
    int chosenFlower;
    int chosenEmpty;
    int countOfFlowers = 0;
    int countOfEmpty = 0;
    int countOfBacks = 0;
    LLUI beeSurroundingSize = 8;

    while ((visitedFlowersCount < NUM_OF_FLOWERS_TO_VISIT) && (realTimeOutsideOfHive + timeToReturnToHive <= maxTimeOutsideOfHive)){
        realSpeedOfBee = (maxSpeedOfBee - (visitedFlowersCount * (maxSpeedOfBee / (NUM_OF_FLOWERS_TO_VISIT *2))));
        travelTimeBetweenPlaces = 0.25 / realSpeedOfBee;
        realTimeOutsideOfHive += travelTimeBetweenPlaces;
        
        timeToReturnToHive = calculateDistanceToHive(&beePosition) / realSpeedOfBee;

        vector<int> beeSurrounding = {      hiveArray[(LLUI)(beePosition[0]) - 1][(LLUI)(beePosition[1]) - 1],
                                            hiveArray[(LLUI)(beePosition[0]) - 1][(LLUI)(beePosition[1]) - 0],
                                            hiveArray[(LLUI)(beePosition[0]) - 1][(LLUI)(beePosition[1]) + 1],
                                            hiveArray[(LLUI)(beePosition[0]) - 0][(LLUI)(beePosition[1]) - 1],
                                            hiveArray[(LLUI)(beePosition[0]) - 0][(LLUI)(beePosition[1]) + 1],
                                            hiveArray[(LLUI)(beePosition[0]) + 1][(LLUI)(beePosition[1]) - 1],
                                            hiveArray[(LLUI)(beePosition[0]) + 1][(LLUI)(beePosition[1]) - 0],
                                            hiveArray[(LLUI)(beePosition[0]) + 1][(LLUI)(beePosition[1]) + 1] };

        countOfFlowers = 0;
        countOfEmpty = 0;
        countOfBacks = 0;
        for (LLUI i = 0; i<8; i++){
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
                individualEmptyChance = 1.0 / (double)countOfEmpty;
                probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                for (int i = 0; i < countOfEmpty+1; i++){
                    if (probability < (individualEmptyChance * (i+1.0))){
                        chosenEmpty = i;
                        break;
                    }
                }

                vector<vector<int>> indexes;
                for (LLUI i = 0; i < beeSurroundingSize; i++){
                    if (beeSurrounding[i] == 0){
                        indexes.push_back(indexArray[i]);
                    }
                }
                chosenSurrounding[0] = indexes[(LLUI)chosenEmpty][0];
                chosenSurrounding[1] = indexes[(LLUI)chosenEmpty][1];
            }
            else if (countOfEmpty == 0){
                visitedFlowersCount++;
                realTimeOutsideOfHive += POLLUTING_TIME;
    
                individualFlowerChance = 1.0 / (double)(countOfFlowers);
                probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                for (int i = 0; i < countOfFlowers+1; i++){
                    if (probability < (individualFlowerChance * (i+1.0))){
                        chosenFlower = i;
                        break;
                    }
                }

                vector<vector<int>> indexes;
                for (LLUI i = 0; i < beeSurroundingSize; i++){
                    if (beeSurrounding[i] == 1){
                        indexes.push_back(indexArray[i]);
                    }
                }
                chosenSurrounding[0] = indexes[(LLUI)chosenFlower][0];
                chosenSurrounding[1] = indexes[(LLUI)chosenFlower][1];
            }
            else if (probability < PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_BACK/2){
                visitedFlowersCount++;
                realTimeOutsideOfHive += POLLUTING_TIME;
    
                individualFlowerChance = 1.0 / (double)(countOfFlowers);
                probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                for (int i = 0; i < countOfFlowers+1; i++){
                    if (probability < (individualFlowerChance * (i+1.0))){
                        chosenFlower = i;
                        break;
                    }
                }

                vector<vector<int>> indexes;
                for (LLUI i = 0; i < beeSurroundingSize; i++){
                    if (beeSurrounding[i] == 1){
                        indexes.push_back(indexArray[i]);
                    }
                }
                chosenSurrounding[0] = indexes[(LLUI)chosenFlower][0];
                chosenSurrounding[1] = indexes[(LLUI)chosenFlower][1];
            }
            else { // (probability <= PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY + PROB_OF_GOING_BACK)
                individualEmptyChance = 1.0 / (double)countOfEmpty;
                probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                for (int i = 0; i < countOfEmpty+1; i++){
                    if (probability < (individualEmptyChance * (i+1.0))){
                        chosenEmpty = i;
                        break;
                    }
                }

                vector<vector<int>> indexes;
                for (LLUI i = 0; i < beeSurroundingSize; i++){
                    if (beeSurrounding[i] == 0){
                        indexes.push_back(indexArray[i]);
                    }
                }
                chosenSurrounding[0] = indexes[(LLUI)chosenEmpty][0];
                chosenSurrounding[1] = indexes[(LLUI)chosenEmpty][1];
            }

        }

        else if ((countOfFlowers != 0) && (countOfEmpty != 0)){

            if (probability < PROB_OF_GOING_TO_FLOWER){
                visitedFlowersCount++;
                realTimeOutsideOfHive += POLLUTING_TIME;
    
                individualFlowerChance = 1.0 / (double)(countOfFlowers);
                probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                for (int i = 0; i < countOfFlowers+1; i++){
                    if (probability < (individualFlowerChance * (i+1.0))){
                        chosenFlower = i;
                        break;
                    }
                }

                vector<vector<int>> indexes;
                for (LLUI i = 0; i < beeSurroundingSize; i++){
                    if (beeSurrounding[i] == 1){
                        indexes.push_back(indexArray[i]);
                    }
                }
                chosenSurrounding[0] = indexes[(LLUI)chosenFlower][0];
                chosenSurrounding[1] = indexes[(LLUI)chosenFlower][1];

            }
            else if (probability < PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY){
                individualEmptyChance = 1.0 / (double)countOfEmpty;
                probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                for (int i = 0; i < countOfEmpty+1; i++){
                    if (probability < (individualEmptyChance * (i+1.0))){
                        chosenEmpty = i;
                        break;
                    }
                }

                vector<vector<int>> indexes;
                for (LLUI i = 0; i < beeSurroundingSize; i++){
                    if (beeSurrounding[i] == 0){
                        indexes.push_back(indexArray[i]);
                    }
                }
                chosenSurrounding[0] = indexes[(LLUI)chosenEmpty][0];
                chosenSurrounding[1] = indexes[(LLUI)chosenEmpty][1];
            }
            else { // (probability <= PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY + PROB_OF_GOING_BACK)
                vector<vector<int>> indexes;
                for (LLUI i = 0; i < beeSurroundingSize; i++){
                    if (beeSurrounding[i] == -1){
                        indexes.push_back(indexArray[i]);
                    }
                }
                chosenSurrounding[0] = indexes[0][0];
                chosenSurrounding[1] = indexes[0][1];
            }

        }

        else if (countOfFlowers == 0){
            
            if (probability < PROB_OF_GOING_TO_EMPTY + PROB_OF_GOING_TO_FLOWER/2){
                individualEmptyChance = 1.0 / (double)countOfEmpty;
                probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                for (int i = 0; i < countOfEmpty+1; i++){
                    if (probability < (individualEmptyChance * (i+1.0))){
                        chosenEmpty = i;
                        break;
                    }
                }

                vector<vector<int>> indexes;
                for (LLUI i = 0; i < beeSurroundingSize; i++){
                    if (beeSurrounding[i] == 0){
                        indexes.push_back(indexArray[i]);
                    }
                }
                chosenSurrounding[0] = indexes[(LLUI)chosenEmpty][0];
                chosenSurrounding[1] = indexes[(LLUI)chosenEmpty][1];
            }
            else { // (probability <= PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY + PROB_OF_GOING_BACK)
                vector<vector<int>> indexes;
                for (LLUI i = 0; i < beeSurroundingSize; i++){
                    if (beeSurrounding[i] == -1){
                        indexes.push_back(indexArray[i]);
                    }
                }
                chosenSurrounding[0] = indexes[0][0];
                chosenSurrounding[1] = indexes[0][1];
            }

        }

        else if (countOfEmpty == 0){

            if (probability < PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY/2){
                visitedFlowersCount++;
                realTimeOutsideOfHive += POLLUTING_TIME;
    
                individualFlowerChance = 1.0 / (double)(countOfFlowers);
                probability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                for (int i = 0; i < countOfFlowers+1; i++){
                    if (probability < (individualFlowerChance * (i+1.0))){
                        chosenFlower = i;
                        break;
                    }
                }

                vector<vector<int>> indexes;
                for (LLUI i = 0; i < beeSurroundingSize; i++){
                    if (beeSurrounding[i] == 1){
                        indexes.push_back(indexArray[i]);
                    }
                }
                chosenSurrounding[0] = indexes[(LLUI)chosenFlower][0];
                chosenSurrounding[1] = indexes[(LLUI)chosenFlower][1];
            }
            else { // (probability <= PROB_OF_GOING_TO_FLOWER + PROB_OF_GOING_TO_EMPTY + PROB_OF_GOING_BACK)
                vector<vector<int>> indexes;
                for (LLUI i = 0; i < beeSurroundingSize; i++){
                    if (beeSurrounding[i] == -1){
                        indexes.push_back(indexArray[i]);
                    }
                }
                chosenSurrounding[0] = indexes[0][0];
                chosenSurrounding[1] = indexes[0][1];
            }
        }

        hiveArray[(LLUI)(oldPosition[0])][(LLUI)(oldPosition[1])] = 0;
        hiveArray[(LLUI)(beePosition[0])][(LLUI)(beePosition[1])] = -1;
        oldPosition[0] = beePosition[0];
        oldPosition[1] = beePosition[1];
        beePosition[0] = beePosition[0] + chosenSurrounding[0];
        beePosition[1] = beePosition[1] + chosenSurrounding[1];
    }
    cout << "Number of Visited Flowers: " << visitedFlowersCount << endl;
    cout << "Time spent outside of the hive: " << (realTimeOutsideOfHive + timeToReturnToHive) / 60.0 << " minutes" << endl;
    return 0;
}