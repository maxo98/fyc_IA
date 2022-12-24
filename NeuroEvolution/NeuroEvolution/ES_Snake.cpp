#include "ES_Snake.h"

#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
//#include <conio.h>
#include <time.h>
#include <iostream>
#include <algorithm> 
#include <time.h>
#include <iomanip>
#include "ThreadPool.h"


//#define HELP
//#define ES_HYPER
#define TROLLOLOL

#define TAILLE_ECRAN 20

int launchESHypeneatTest()
{
    NeatParameters neatparam;

#ifndef TROLLOLOL
    //neatparam.activationFunctions.push_back(new thresholdActivation());
    neatparam.activationFunctions.push_back(new AbsActivation());
    neatparam.activationFunctions.push_back(new SinActivation());
    neatparam.activationFunctions.push_back(new HyperbolSecantActivation());
    neatparam.activationFunctions.push_back(new TanhActivation());
    neatparam.activationFunctions.push_back(new LinearActivation());
#endif // !TROLLOLOL


#ifdef TROLLOLOL
    neatparam.activationFunctions.push_back(new SquareActivation());
    neatparam.activationFunctions.push_back(new SquareRootActivation());
#endif // TROLLOLOL

    neatparam.pbMutateLink = 0.05;// 0.05;
    neatparam.pbMutateNode = 0.03;//0.03;
    neatparam.pbWeight = 0.9;// 0.9;
    neatparam.pbToggleLink = 0.01;// 0.05;
    neatparam.weightMuteStrength = 0.5;// 2.5;
    neatparam.pbMutateActivation = 0.7;

    neatparam.disjointCoeff = 1.0;
    neatparam.excessCoeff = 1.0;
    neatparam.mutDiffCoeff = 0.4;
    neatparam.activationDiffCoeff = 1.0;
    neatparam.weightCoeff = 0;

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

    neatparam.speciationDistance = 3;


    neatparam.speciationDistanceMod = 0.3;
    neatparam.minExpectedSpecies = 15;
    neatparam.maxExpectedSpecies = 40;
    neatparam.adaptSpeciation = false;

    neatparam.keepChamp = true;
    neatparam.elistism = true;
    neatparam.rouletteMultiplier = 2.0;

    HyperneatParameters hyperneatParam;

    hyperneatParam.activationFunction = new LinearActivation();
    hyperneatParam.cppnInput = 4;

#ifdef HELP
    hyperneatParam.cppnInputFunction = invDistCppnInput;
#endif // HELP

#ifndef HELP
    hyperneatParam.cppnInputFunction = basicCppnInput;
#endif // !HELP

    hyperneatParam.cppnOutput = 1;
    hyperneatParam.nDimensions = 2;
    hyperneatParam.thresholdFunction = noThreshold; 
    //hyperneatParam.weightModifierFunction = noChangeWeight;
    hyperneatParam.weightModifierFunction = absWeight;


    float threshold = 0.5;

    float maxDist = 4;

    hyperneatParam.inputVariables.push_back(&maxDist);
    hyperneatParam.thresholdVariables.push_back(&threshold);
    hyperneatParam.weightVariables.push_back(&threshold);

    ES_Parameters esParam;

    esParam.width = 1;

    esParam.initialDepth = 1;
    esParam.maxDepth = 3;
    esParam.bandThreshold = 0.3;
    esParam.iterationLevel = 1;
    esParam.varianceThreshold = 2;
    esParam.allowRecurisvity = false;

    esParam.center.push_back(0);
    esParam.center.push_back(0);

    int popSize = 300;

    std::vector<float> pos;
    pos.resize(2);

#ifndef ES_HYPER
    Hyperneat esHyper(popSize, neatparam, hyperneatParam);
#endif // !ES_HYPER

#ifdef ES_HYPER
    ES_Hyperneat esHyper(popSize, neatparam, hyperneatParam, esParam);
#endif // ES_HYPER


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
    pos[1] = -0.5;
    esHyper.addOutput(pos);

    pos[0] = 0;
    pos[1] = 0.5;
    esHyper.addOutput(pos);

    pos[0] = -0.5;
    pos[1] = 0;
    esHyper.addOutput(pos);

    pos[0] = 0.5;
    pos[1] = 0;
    esHyper.addOutput(pos);


    esHyper.initNetworks();
    esHyper.generateNetworks();

    esHypeneatTest(popSize, esHyper);

    esHyper.saveHistory();

    std::vector<float> output;

    NeuralNetwork network;
    esHyper.genomeToNetwork(*esHyper.getGoat(), network);

    bool aaa;

    snakeTest(&network, true, aaa);

    esHyper.getGoat()->saveCurrentGenome();

    for (int i = 0; i < neatparam.activationFunctions.size(); i++)
    {
        delete neatparam.activationFunctions[i];
    }

    delete hyperneatParam.activationFunction;

    return 0;
}


bool esHypeneatTest(int popSize, Hyperneat& esHyper)
{
    std::vector<float> fitness;

    fitness.resize(popSize);

    bool validated = false;

    for (int i3 = 0; i3 < 100 && validated == false; i3++)
    {
        std::cout << std::endl << "gen " << i3 << std::endl;

        for (int i = 0; i < popSize; i++)
        {
            fitness[i] = 0;
        }

        int threads = 1;
        ThreadPool* pool = ThreadPool::getInstance();
        unsigned int cpus = std::thread::hardware_concurrency();

        float totalWorkload = popSize;
        float workload = totalWorkload / cpus;
        float restWorkload = 0;
        int currentWorkload = totalWorkload;
        int startIndex = 0;
        int count = 0;

        std::deque<std::atomic<bool>> tickets;

#ifdef MULTITHREAD
        while (workload < 1)
        {
            cpus--;
            workload = totalWorkload / cpus;
        }

        while (cpus > threads)
        {
            currentWorkload = floor(workload);
            float workloadFrac = fmod(workload, 1.0f);
            restWorkload = workloadFrac;

            tickets.emplace_back(false);
            pool->queueJob(snakeEvaluate, startIndex, currentWorkload + floor(restWorkload), std::ref(fitness), std::ref(esHyper), std::ref(validated), &tickets.back());
            ++threads;

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

        snakeEvaluate(startIndex, currentWorkload, fitness, esHyper, validated);

        for (std::deque<std::atomic<bool>>::iterator itTicket = tickets.begin(); itTicket != tickets.end(); ++itTicket)
        {
            itTicket->wait(false);
        }

        esHyper.setScore(fitness);

        esHyper.evolve();
    }

    std::cout << "done" << std::endl;

    return false;
}

void snakeEvaluate(int startIndex, int currentWorkload, std::vector<float>& fitness, Hyperneat& esHyper, bool& validated, std::atomic<bool>* ticket)
{
    for (int i = startIndex; i < (startIndex + currentWorkload); i++)
    {
        bool tmp = false;

        fitness[i] = snakeTest(esHyper.getNeuralNetwork(i), false, tmp);

        if (tmp == true)
        {
            validated = true;
        }
    }

    if (ticket != nullptr)
    {
        (*ticket) = true;
        ticket->notify_one();
    }
}


int snakeTest(NeuralNetwork* network, bool display, bool& validated)
{
    std::vector<float> scoreArray;
    int wins = 0;

    scoreArray.resize(20, 0);

    float fruitScore = sqrt(pow(TAILLE_ECRAN, 2) * 2);

    for(int test = 0; (test < 20 && display == false) || (test < 1 && display == true); test++)
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
            screen[TAILLE_ECRAN/2 + i - 1][TAILLE_ECRAN / 2] = -1;
            snake.push_back(std::pair<int, int>(TAILLE_ECRAN / 2 + i - 1, TAILLE_ECRAN / 2));
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

        networkInput.resize(8, 0);

        //START MAIN GAME LOOP
        do {
            timer++;

            if (display == true)
            {
                std::cout << snake.back() << " " << fruit << std::endl;
            }

            networkInput[0] = (pow(TAILLE_ECRAN, 2) * 2)  - (pow(snake.back().first - fruit.first - 0.5, 2) + pow(snake.back().second - fruit.second, 2));
            networkInput[1] = (pow(TAILLE_ECRAN, 2) * 2) - (pow(snake.back().first - fruit.first + 0.5, 2) + pow(snake.back().second - fruit.second, 2));
            networkInput[2] = (pow(TAILLE_ECRAN, 2) * 2) - (pow(snake.back().first - fruit.first, 2) + pow(snake.back().second - fruit.second - 0.5, 2));
            networkInput[3] = (pow(TAILLE_ECRAN, 2) * 2) - (pow(snake.back().first - fruit.first, 2) + pow(snake.back().second - fruit.second + 0.5, 2));

            for (int n = 0; n < 4; n++)
            {
                networkInput[n + 4] = 0;
            }

            float result = 0;
            
            std::pair<int, int> pos;

            int debug = 0;
            int aa = 0;
            int bb = 0;

            for (int i = -1; i <= TAILLE_ECRAN; i++)
            {
                pos.first = snake.back().first - i;

                for (int j = -1; j <= TAILLE_ECRAN; j++)
                {
                    pos.second = snake.back().second - j;

                    if (i == -1 || j == -1 || i == TAILLE_ECRAN || j == TAILLE_ECRAN ||
                        (i > -1 && i < TAILLE_ECRAN && j > -1 && j < TAILLE_ECRAN && screen[i][j] == -1
                            && (pos.first != 0 || pos.second != 0)))
                    {
                        
                        float dist = -((pow(TAILLE_ECRAN, 2) * 2) - (pow(float(pos.first) - 0.5f, 2) + pow(pos.second, 2)));

                        if (dist < networkInput[4]) networkInput[4] = dist;

                        dist = -((pow(TAILLE_ECRAN, 2) * 2) - (pow(float(pos.first) + 0.5f, 2) + pow(pos.second, 2)));

                        if (dist < networkInput[5]) networkInput[5] = dist;

                        dist = -((pow(TAILLE_ECRAN, 2) * 2) - (pow(pos.first, 2) + pow(float(pos.second) - 0.5f, 2)));

                        if (dist < networkInput[6]) networkInput[6] = dist;

                        dist = -((pow(TAILLE_ECRAN, 2) * 2) - (pow(pos.first, 2) + pow(float(pos.second) + 0.5f, 2)));

                        if (dist < networkInput[7]) networkInput[7] = dist;
                    }
                }
            }

            if (display == true)
            {
                std::cout << "netInput " << networkInput << std::endl;
            }

            network->compute(networkInput, output);

            //up, down, left, right
            int directionIndex = 0;
            float outputScore = output[0];

            if (display == true)
            {
                std::cout << 0 << " " << output[0] << std::endl;
            }

            for (int i = 1; i < output.size(); i++)
            {
                if (output[i] > outputScore)
                {
                    directionIndex = i;
                    outputScore = output[i];
                }

                if (display == true)
                {
                    std::cout << i << " " << output[i] << std::endl;
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
            case 8://UP
                snake.push_back(std::pair<int, int>(snake.back().first - 1, snake.back().second));
                break;
            case 2://Down
                snake.push_back(std::pair<int, int>(snake.back().first + 1, snake.back().second));
                break;
            case 4://Left
                snake.push_back(std::pair<int, int>(snake.back().first, snake.back().second - 1));
                break;
            case 6://Right
                snake.push_back(std::pair<int, int>(snake.back().first, snake.back().second + 1));
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

                if (sqrt(pow(TAILLE_ECRAN, 2) * 2) - sqrt(pow(snake.back().first - fruit.first - 0.5, 2) + pow(snake.back().second - fruit.second, 2))
                    < sqrt(pow(TAILLE_ECRAN, 2) * 2) - sqrt(pow(snake[snake.size() - 2].first - fruit.first - 0.5, 2) + pow(snake[snake.size() - 2].second - fruit.second, 2)))
                {
                    scoreArray[test] += 0.25;
                }
                else {
                    scoreArray[test] -= 0.25;
                }
            }

        } while (vie >= 1 && score < 10 && timer < 200 && timer < ((TAILLE_ECRAN + 5) * (score + 1)));
        //END MAIN GAME LOOP

        scoreArray[test] += score * TAILLE_ECRAN * 4 + 1;

        if (countChangeDir == 0)
        {
            scoreArray[test] = 0;
        }

        if (score == 10)
        {
            wins++;
        }
    }

    if (wins == 20)
    {
        validated = true;
    }

    float finalScore = 0;

    for (int i = 0; i < scoreArray.size(); i++)
    {
        finalScore += scoreArray[i];
    }

    finalScore /= scoreArray.size();

    return finalScore;
}
