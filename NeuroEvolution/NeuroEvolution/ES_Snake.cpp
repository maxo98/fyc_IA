#include "ES_Snake.h"

#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
//#include <conio.h>
#include <time.h>
#include <iostream>
#include <algorithm> 

#define TAILLE_ECRAN 20

int launchESHypeneatTest()
{
    NeatParameters neatparam;

    //neatparam.activationFunctions.push_back(new thresholdActivation());
    neatparam.activationFunctions.push_back(new AbsActivation());
    neatparam.activationFunctions.push_back(new SinActivation());
    neatparam.activationFunctions.push_back(new HyperbolSecantActivation());
    neatparam.activationFunctions.push_back(new TanhActivation());

    neatparam.pbMutateLink = 0.05;// 0.05;
    neatparam.pbMutateNode = 0.03;//0.03;
    neatparam.pbWeight = 0.9;// 0.9;
    neatparam.pbToggleLink = 0.05;// 0.05;
    neatparam.weightMuteStrength = 1.3;// 2.5;
    neatparam.pbMutateActivation = 0.7;

    neatparam.disjointCoeff = 1.0;
    neatparam.excessCoeff = 1.0;
    neatparam.mutDiffCoeff = 0.4;
    neatparam.activationDiffCoeff = 1.0;

    neatparam.killRate = 0.2;

    neatparam.champFileSave = "champ";
    neatparam.avgFileSave = "avg";//Without extension type file
    neatparam.saveChampHistory = true;
    neatparam.saveAvgHistory = true;

    neatparam.pbMateMultipoint = 0.6;
    neatparam.pbMateSinglepoint = 0.0;
    neatparam.interspeciesMateRate = 0.001;
    neatparam.dropOffAge = 15;
    neatparam.ageSignificance = 1.0;
    neatparam.pbMutateOnly = 0.25;
    neatparam.pbMateOnly = 0.2;

    neatparam.speciationDistance = 2.0;


    neatparam.speciationDistanceMod = 0.3;
    neatparam.minExpectedSpecies = 6;
    neatparam.maxExpectedSpecies = 12;
    neatparam.adaptSpeciation = true;

    neatparam.keepChamp = true;
    neatparam.elistism = true;
    neatparam.rouletteMultiplier = 2.0;

    HyperneatParameters hyperneatParam;

    hyperneatParam.activationFunction = new LinearActivation();
    hyperneatParam.cppnInput = 5;
    //hyperneatParam.cppnInputFunction = distCppnInput;
    hyperneatParam.cppnInputFunction = biasCppnInput;
    hyperneatParam.cppnOutput = 1;
    hyperneatParam.nDimensions = 2;
    hyperneatParam.thresholdFunction = fixedThreshold;
    hyperneatParam.weightModifierFunction = substractWeight;

    float threshold = 0.3;

    float maxDist = 1;

    hyperneatParam.inputVariables.push_back(&maxDist);
    hyperneatParam.thresholdVariables.push_back(&threshold);
    hyperneatParam.weightVariables.push_back(&threshold);

    ES_Parameters esParam;

    esParam.bandThreshold = 2;
    esParam.width = 1;

    esParam.initialDepth = 1;
    esParam.maxDepth = 3;
    esParam.bandThreshold = 0.3;
    esParam.iterationLevel = 1;
    esParam.varianceThreshold = 2;
    esParam.allowRecurisvity = false;

    esParam.center.push_back(0);
    esParam.center.push_back(0);

    int popSize = 150;

    std::vector<float> pos;
    pos.resize(2);

    ES_Hyperneat esHyper(popSize, neatparam, hyperneatParam, esParam);

    pos[0] = 0;
    pos[1] = -0.5;
    esHyper.addInput(pos);

    pos[0] = 0;
    pos[1] = 0.5;
    esHyper.addInput(pos);

    pos[0] = -0.5;
    pos[1] = 0;
    esHyper.addInput(pos);

    pos[0] = 0.5;
    pos[1] = 0;
    esHyper.addInput(pos);





    //Set network output, up, down, left, right

    pos[0] = 0;
    pos[1] = -1;
    esHyper.addOutput(pos);

    pos[0] = 0;
    pos[1] = 1;
    esHyper.addOutput(pos);

    pos[0] = -1;
    pos[1] = 0;
    esHyper.addOutput(pos);

    pos[0] = 1;
    pos[1] = 0;
    esHyper.addOutput(pos);


    esHyper.initNetworks();
    esHyper.generateNetworks();

    esHypeneatTest(popSize, esHyper);

    esHyper.saveHistory();

    std::vector<float> output;

    NeuralNetwork network;
    esHyper.genomeToNetwork(*esHyper.getGoat(), network);

    snakeTest(&network, true);

    for (int i = 0; i < neatparam.activationFunctions.size(); i++)
    {
        delete neatparam.activationFunctions[i];
    }

    delete hyperneatParam.activationFunction;

    return 0;
}


bool esHypeneatTest(int popSize, ES_Hyperneat& esHyper)
{
    std::vector<float> fitness;

    fitness.resize(popSize);

    for (int i3 = 0; i3 < 100; i3++)
    {
        std::cout << std::endl << "gen " << i3 << std::endl;

        for (int i = 0; i < popSize; i++)
        {
            fitness[i] = 0;
        }

        std::vector<std::thread> threads;
        unsigned int cpus = std::thread::hardware_concurrency();

        float totalWorkload = popSize;
        float workload = totalWorkload / cpus;
        float restWorkload = 0;
        int currentWorkload = totalWorkload;
        int startIndex = 0;
        int count = 0;

#ifdef MULTITHREAD
        while (workload < 1)
        {
            cpus--;
            workload = totalWorkload / cpus;
        }

        currentWorkload = floor(workload);
        float workloadFrac = fmod(workload, 1.0f);
        restWorkload = workloadFrac;

        while (cpus > threads.size() + 1)
        {
            threads.push_back(std::thread(snakeEvaluate, startIndex, currentWorkload + floor(restWorkload), std::ref(fitness), std::ref(esHyper)));

            count += currentWorkload + floor(restWorkload);

            startIndex += currentWorkload + floor(restWorkload);

            restWorkload -= floor(restWorkload);
            restWorkload += workloadFrac;
        }

        while (restWorkload > 0)
        {
            restWorkload--;
            currentWorkload++;
        }
#endif //MULTITHREAD

        count += currentWorkload;

        while (count > totalWorkload)
        {
            currentWorkload--;
            count--;
        }

        snakeEvaluate(startIndex, currentWorkload, fitness, esHyper);

        for (int i = 0; i < threads.size(); i++)
        {
            threads[i].join();
        }

        esHyper.setScore(fitness);

        esHyper.evolve();
    }

    std::cout << "done" << std::endl;

    return false;
}

void snakeEvaluate(int startIndex, int currentWorkload, std::vector<float>& fitness, ES_Hyperneat& esHyper)
{
    for (int i = startIndex; i < (startIndex + currentWorkload); i++)
    {
        fitness[i] = snakeTest(esHyper.getNeuralNetwork(i), false);
    }
}


int snakeTest(NeuralNetwork* network, bool display)
{
    std::vector<float> scoreArray;

    scoreArray.resize(10);

    float fruitScore = sqrt(pow(TAILLE_ECRAN, 2) * 2);

    for(int test = 0; test < 10; test++)
    {
        if (display == true)
        {
            std::cout << "new snake\n";
        }

        std::pair<int, int> fruit;
        char screen[TAILLE_ECRAN][TAILLE_ECRAN];

        std::deque<std::pair<int, int>> snake;

        int direction = 2, score = 0, input = 0, vie = 1, cpt, found = 0, mange = 0;

        for (int i = 0; i < TAILLE_ECRAN; i++)//Initialisation de l'�cran "� vide"
        {
            for (cpt = 0; cpt < TAILLE_ECRAN; cpt++)
            {
                screen[i][cpt] = 0;
            }
        }

        //Initialisation du snake
        for (int i = 0; i < 3; i++)
        {
            screen[5+i][4] = -1;
            snake.push_back(std::pair<int, int>(5 + i, 4));
        }

        do//Apparition du premier fruit
        {
            fruit.first = rand() % (TAILLE_ECRAN);
            fruit.second = rand() % (TAILLE_ECRAN);
            if (screen[fruit.first][fruit.second] != -1)//on s'assure qu'il napparait pas sur le snake
            {
                screen[fruit.first][fruit.second] = 5;
                found = 1;
            }
        } while (found == 0);

        int timer = 0;

        std::vector<float> networkInput, output;
        int countChangeDir = 0;

        networkInput.resize(4);//8);

        //START MAIN GAME LOOP
        do {
            timer++;

            if (display == true)
            {
                std::cout << snake.back() << " " << fruit << std::endl;
            }

            networkInput[0] = sqrt(pow(TAILLE_ECRAN, 2) * 2)  - sqrt(pow(snake.back().first - fruit.first - 0.5, 2) + pow(snake.back().second - fruit.second, 2));
            networkInput[1] = sqrt(pow(TAILLE_ECRAN, 2) * 2) - sqrt(pow(snake.back().first - fruit.first + 0.5, 2) + pow(snake.back().second - fruit.second, 2));
            networkInput[2] = sqrt(pow(TAILLE_ECRAN, 2) * 2) - sqrt(pow(snake.back().first - fruit.first, 2) + pow(snake.back().second - fruit.second - 0.5, 2));
            networkInput[3] = sqrt(pow(TAILLE_ECRAN, 2) * 2) - sqrt(pow(snake.back().first - fruit.first, 2) + pow(snake.back().second - fruit.second + 0.5, 2));


            //std::pair<int, int> pos;



            //        if (pos.first == -1 || pos.second == -1 || pos.first == TAILLE_ECRAN || pos.second == TAILLE_ECRAN || 
            //            (pos.first > -1 && pos.first < TAILLE_ECRAN && pos.second > -1 && pos.second < TAILLE_ECRAN && screen[pos.first][pos.second] == -1))
            //        {
            //            float dist = sqrt(pow(TAILLE_ECRAN, 2) * 2) - sqrt(pow(snake.back().first - pos.first - 0.7, 2) + pow(snake.back().second - pos.second, 2));

            //            if (dist > networkInput[4]) networkInput[4] = dist;

            //            dist = sqrt(pow(TAILLE_ECRAN, 2) * 2) - sqrt(pow(snake.back().first - pos.first + 0.7, 2) + pow(snake.back().second - pos.second, 2));

            //            if (dist > networkInput[5]) networkInput[5] = dist;

            //            dist = sqrt(pow(TAILLE_ECRAN, 2) * 2) - sqrt(pow(snake.back().first - pos.first, 2) + pow(snake.back().second - pos.second - 0.7, 2));

            //            if (dist > networkInput[6]) networkInput[6] = dist;

            //            dist = sqrt(pow(TAILLE_ECRAN, 2) * 2) - sqrt(pow(snake.back().first - pos.first, 2) + pow(snake.back().second - pos.second + 0.7, 2));

            //            if (dist > networkInput[7]) networkInput[7] = dist;
            //        }
            //    }
            //}

            if (display == true)
            {
                std::cout << networkInput << std::endl;
            }

            network->compute(networkInput, output);

            //up, down, left, right
            int directionIndex = 0;
            float outputScore = output[0];

            for (int i = 1; i < output.size(); i++)
            {
                if (output[i] > outputScore)
                {
                    directionIndex = i;
                    outputScore = output[i];
                }
            }

            //Do not change direction by default
            if (outputScore == 0)
            {
                directionIndex = -1;
            }

            switch (directionIndex)//lecture de l'entr� pour savoir quel direction prendre
            {
            case 0:
                if (display == true) std::cout << "UP\n";
                direction = 8;
                countChangeDir++;
                break;
            case 1:
                if (display == true) std::cout << "DOWN\n";
                direction = 2;
                countChangeDir++;
                break;
            case 2:
                if (display == true) std::cout << "LEFT\n";
                direction = 4;
                countChangeDir++;
                break;
            case 3:
                if (display == true) std::cout << "RIGHT\n";
                direction = 6;
                countChangeDir++;
                break;
            }

            input = 0;//reset de l'input

            switch (direction)//lecture de l'entr� pour savoir quel direction prendre
            {
            case 8:
                snake.push_back(std::pair<int, int>(snake.back().first - 1, snake.back().second));
                break;
            case 2:
                snake.push_back(std::pair<int, int>(snake.back().first + 1, snake.back().second));
                break;
            case 6:
                snake.push_back(std::pair<int, int>(snake.back().first, snake.back().second + 1));
                break;
            case 4:
                snake.push_back(std::pair<int, int>(snake.back().first, snake.back().second - 1));
                break;
            }

            if (screen[snake.back().first][snake.back().second] == 5)//Si le serpent mange un fruit
            {
                do//apparition d'un autre autre fruit
                {
                    fruit.first = rand() % (TAILLE_ECRAN);
                    fruit.second = rand() % (TAILLE_ECRAN);
                    if (screen[fruit.first][fruit.second] != -1 && abs(snake.back().first - fruit.first) > 1 && abs(snake.back().second - fruit.second))
                    {
                        screen[fruit.first][fruit.second] = 5;
                        mange = 1;
                    }
                } while (mange == 0);

                score++;
            }

            if ((snake.back().first >= TAILLE_ECRAN) || (snake.back().first < 0) || (snake.back().second >= TAILLE_ECRAN) || (snake.back().second < 0)
                || (screen[snake.back().first][snake.back().second] == -1))//si le serpent sort de l'�cran ou se mange lui meme
            {
                vie = 0;// Alors il meurt
            }
            else {
                screen[snake.back().first][snake.back().second] = -1;//placement du nouveau morceau du snake

                if (mange == 0)//Si le serpent n'est pas en train de manger
                {

                    screen[snake[0].first][snake[0].second] = 0;//effacement du dernier morceau du serpent
                    snake.pop_front();
                }
                else {//Si le serpent est en train de manger alors on n'efface pas de bout su serpent pour cette frame

                    mange = 0;
                }

                if (display == true)
                {
                    //system("cls");//Nettoyage juste avant d'afficher une nouvelle frame
                    printf("Score : %d\n", score);
                    for (int i = 0; i < TAILLE_ECRAN; i++)//Affichage
                    {
                        for (cpt = 0; cpt < TAILLE_ECRAN; cpt++)
                        {
                            switch (screen[i][cpt])
                            {
                            case 0:
                                printf("_");
                                break;
                            case -1:
                                printf("o");
                                break;
                            case 5:
                                printf("X");
                                break;
                                /*case 5 :
                                    printf("5");
                                    break;*/
                            }
                        }
                        printf("\n");
                    }
                }

                /*Sleep(500);
                if (_kbhit() != 0)//Detecte si on appuie sur le clavier
                {
                    input = _getch();//Lecture de la derniere touche appuy�
                    fflush(stdin);
                }*/

                scoreArray[test] += fruitScore - sqrt(pow(snake.back().first - fruit.first, 2) + pow(snake.back().second - fruit.second, 2)) + fruitScore * score;
            }

        } while (vie >= 1 && score < 10 && timer < 200 && timer < ((TAILLE_ECRAN + 5) * (score + 1)));
        //END MAIN GAME LOOP

    }





    std::sort(scoreArray.begin(), scoreArray.end());

    return (scoreArray[2] + scoreArray[4]);
}
