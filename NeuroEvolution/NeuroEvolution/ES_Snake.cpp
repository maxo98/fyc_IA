#include "ES_Snake.h"

#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
//#include <conio.h>
#include <time.h>
#include <iostream>


#define TAILLE_ECRAN 20
#define TAILLE_SNAKE 11 //nombre de direction diff�rente pouvant etre enregistrer + 1

int launchESHypeneatTest()
{
    NeatParameters neatparam;

    //neatparam.activationFunctions.push_back(new thresholdActivation());
    neatparam.activationFunctions.push_back(new sigmoidActivation());
    neatparam.activationFunctions.push_back(new sinActivation());
    neatparam.activationFunctions.push_back(new gaussianActivation());
    neatparam.activationFunctions.push_back(new absActivation());

    neatparam.pbMutateLink = 0.15;// 0.05;
    neatparam.pbMutateNode = 0.1;//0.03;
    //neatparam.pbWeightShift = 0.7;
    //neatparam.pbWeightRandom = 0.2;
    neatparam.pbWeight = 0.9;// 0.9;
    neatparam.pbToggleLink = 0.05;// 0.05;
    //neatparam.weightShiftStrength = 2.5;
    //neatparam.weightRandomStrength = 2.5;
    neatparam.weightMuteStrength = 3.0;// 2.5;
    neatparam.pbMutateActivation = 0.9;

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

    hyperneatParam.activationFunction = new linearActivation();
    hyperneatParam.cppnInput = 5;
    hyperneatParam.cppnInputFunction = biasCppnInput;
    hyperneatParam.cppnOutput = 1;
    hyperneatParam.nDimensions = 2;
    hyperneatParam.thresholdFunction = fixedThreshold;
    hyperneatParam.weightModifierFunction = substractWeight;

    float threshold = 0.5;

    hyperneatParam.thresholdVariables.push_back(&threshold);
    hyperneatParam.weightVariables.push_back(&threshold);

    ES_Parameters esParam;

    esParam.bandThreshold = 2;
    esParam.width = 10;

    esParam.initialDepth = 2;
    esParam.maxDepth = 4;
    esParam.bandThreshold = 0.3;
    esParam.iterationLevel = 1;
    esParam.varianceThreshold = 2;
    esParam.allowRecurisvity = false;

    esParam.center.push_back(10);
    esParam.center.push_back(10);

    int popSize = 150;

    std::vector<float> pos;
    pos.resize(2);

    int count = 0;

    ES_Hyperneat esHyper(popSize, neatparam, hyperneatParam, esParam);

    for (int i = 1; i <= 20; i++)
    {
        pos[0] = i;

        for (int j = 1; j <= 20; j++)
        {
            pos[1] = j;

            esHyper.addInput(pos);       
            count++;
        }
    }

    //Set network output, up, down, left, right

    pos[0] = 10;
    pos[1] = 0;
    esHyper.addOutput(pos);

    pos[0] = 10;
    pos[1] = 19;
    esHyper.addOutput(pos);

    pos[0] = 0;
    pos[1] = 10;
    esHyper.addOutput(pos);

    pos[0] = 19;
    pos[1] = 10;
    esHyper.addOutput(pos);

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
    std::cout << startIndex << " " << currentWorkload << std::endl;
    for (int i = startIndex; i < (startIndex + currentWorkload); i++)
    {
        fitness[i] = snakeTest(esHyper.getNeuralNetwork(i), false);
    }
}


int snakeTest(NeuralNetwork* network, bool display)
{
    char screen[TAILLE_ECRAN][TAILLE_ECRAN];

    char lastDir[TAILLE_SNAKE+1][2] = { {2,0},{0,4} };


    //tete1 = position y de la tete du snake, tete2 = position x de la tete du snake
    //queue1 = coordon�s y de la case � reset queue2 = coordon�s x de la case � reset
    int direction = 2, score = 0, input = 0, vie = 1, tete1 = 14, tete2 = 9, queue1 = 10, queue2 = 9, cpt, found = 0, decremente, r1, r2, mange = 0, snake = 4;
    //srand(time(NULL));//G�n�ration seed al�atoire


    for (int i = 0; i < TAILLE_ECRAN; i++)//Initialisation de l'�cran "� vide"
    {
        for (cpt = 0; cpt < TAILLE_ECRAN; cpt++)
        {
            screen[i][cpt] = 1;
        }
    }

    for (int i = 2; i < TAILLE_SNAKE; i++)//Initialisation de lastDir
    {
        for (cpt = 0; cpt < 2; cpt++)
        {
            lastDir[i][cpt] = 0;
        }
    }

    screen[10][9] = 0;//Initialisation du snake
    screen[11][9] = 0;
    screen[12][9] = 0;
    screen[13][9] = 0;
    screen[14][9] = 0;

    do//Apparition du premier fruit
    {
        r1 = rand() % (TAILLE_ECRAN);
        r2 = rand() % (TAILLE_ECRAN);
        if (screen[r1][r2] != 0)//on s'assure qu'il napparait pas sur le snake
        {
            screen[r1][r2] = 2;
            found = 1;
        }
    } while (found == 0);


    int timer = 0;

    std::cout << "new snake\n";

    std::vector<float> networkInput, output;

    //START MAIN GAME LOOP
    do {
        timer++;

        

        for (int i = 0; i < TAILLE_ECRAN; i++)
        {
            for (int i2 = 0; i2 < TAILLE_ECRAN; i2++)
            {
                //Use a special input for the head
                if (i != tete1 || i2 != tete2)
                {
                    networkInput.push_back(screen[i][i2]);
                }
                else {
                    networkInput.push_back(5);
                }
                
            }
        }

        network->compute(networkInput, output);

        //up, down, left, right
        int directionIndex = 0;
        float outputScore = output[0];

        //std::cout << output[0] << std::endl;

        for (int i = 1; i < output.size(); i++)
        {
            //std::cout << output[i] << std::endl;

            if (output[i] > outputScore)
            {
                directionIndex = i;
                outputScore = output[i];
                std::cout << i << std::endl;
            }
        }

        std::cout << "chose " << directionIndex << std::endl;

        //std::cout << std::endl;

        switch (directionIndex)//lecture de l'entr� pour savoir quel direction prendre
        {
        case 0:
            std::cout << "UP\n";
            if (direction == 8)
            {
                input = 0;
            }
            else {
                direction = 8;
            }

            break;
        case 1:
            std::cout << "DOWN\n";
            if (direction == 2)
            {
                input = 0;
            }
            else {
                direction = 2;
                std::cout << "SET DOWN\n";
            }
            break;
        case 3:
            std::cout << "RIGHT\n";
            if (direction == 6)
            {
                input = 0;
            }
            else {
                direction = 6;
            }
            break;
        case 2:
            std::cout << "LEFT\n";
            if (direction == 4)
            {
                input = 0;
            }
            else {
                direction = 4;
            }
            break;
        }

        if (input != 0)// si il y a eue une entr�
        {
            found = 0;
            int i = 1;
            do
            {
                if (lastDir[i][0] == 0)//Alors on cherche une place
                {
                    found = 1;
                }
                else {
                    i++;
                }

            } while (found == 0);
            lastDir[i][0] = direction;//pour enregistrer cette direction
            input = 0;//reset de l'input
        }

        switch (direction)//D�placement de la tete suivant la direction
        {
        case 8:
            tete1 -= 1;
            break;
        case 2:
            tete1 += 1;
            break;
        case 4:
            tete2 -= 1;
            break;
        case 6:
            tete2 += 1;
            break;
        }

        if (screen[tete1][tete2] == 2)//Si le serpent mange un fruit
        {
            do//apparition d'un autre autre fruit
            {
                r1 = rand() % (TAILLE_ECRAN);
                r2 = rand() % (TAILLE_ECRAN);
                if (screen[r1][r2] != 0)
                {
                    screen[r1][r2] = 2;
                    mange = 1;
                }
            } while (mange == 0);

            score++;
            snake++;//Augmentation de la taille du serpent
        }

        if ((tete1 > TAILLE_ECRAN) || (tete1 < 0) || (tete2 > TAILLE_ECRAN) || (tete2 < 0) || (screen[tete1][tete2] == 0))//si le serpent sort de l'�cran ou se mange lui meme
        {
            vie = 0;// Alors il meurt
        }
        else {
            screen[tete1][tete2] = 0;//placement du nouveau morceau du snake
        }

        if (mange == 0)//Si le serpent n'est pas en train de manger
        {

            screen[queue1][queue2] = 1;//effacement du dernier morceau du serpent
            switch (lastDir[0][0])//nouvelle position � effacer
            {
            case 8:
                queue1 -= 1;
                break;
            case 2:
                queue1 += 1;
                break;
            case 4:
                queue2 -= 1;
                break;
            case 6:
                queue2 += 1;
                break;
            }
            //screen[queue1][queue2] = 5;
            decremente = 0;
            found = 0;
            int i = 1;

            do
            {
                if (lastDir[i][0] == 0)
                {
                    if (lastDir[i][1] < snake)
                    {
                        lastDir[i][1]++;//nombre de frame pass� depuis le dernier changement direction, maximum = snake
                    }
                    found = 1;//quand on a trouv� partie du tableau qui n'a pas
                    //direction on lincr�mente et sort de la boucle
                }
                else {

                    if (decremente == 0)//si on trouve une partie du tableau qui a une direction
                    {//et que l'on a pas encore d�cr�menter alors on d�crmente le timer de cellle-ci
                        lastDir[i][1] -= 1;
                        decremente = 1;
                    }
                    i++;


                }
            } while (found == 0);//un fois qu'on a trouv� une partie du tableau sans direction alors on sort

        }
        else {//Si le serpent est en train de manger alors on n'efface pas de bout su serpent pour cette frame

            mange = 0;
            int i = snake + 2;//On part de la fin du tableau
            found = 0;

            do
            {
                if (lastDir[i][0] != 0)//On cherche le dernier changement de direction
                {
                    found = 1;
                    lastDir[i + 1][1]++;//et on augmente de 1 le nombre de frame �coul� depuis celui d'avant
                }

                i--;
            } while (found == 0);
            //lastDir[1][1]++;
        }

        if ((lastDir[1][1] == 0) && (lastDir[1][0] != 0))//Si le 1er timer est � 0 tout est d�cal�
        {
            for (int i = 1; i < TAILLE_SNAKE; i++)
            {

                lastDir[i - 1][0] = lastDir[i][0];
                lastDir[i - 1][1] = lastDir[i][1];
            }

            lastDir[TAILLE_SNAKE][1] = 0;//Et on reset la fin du tableau
            lastDir[TAILLE_SNAKE][0] = 0;
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
                    case 1:
                        printf("_");
                        break;
                    case 0:
                        printf("o");
                        break;
                    case 2:
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

        //Affichage pour debuggage
        /*printf("lastdir[0][0] = %d\n", lastDir[0][0]);
        printf("lastdir[0][1] = %d\n", lastDir[0][1]);
        printf("\nlastdir[1][0] = %d\n", lastDir[1][0]);
        printf("lastdir[1][1] = %d\n", lastDir[1][1]);
        printf("\nlastdir[2][0] = %d\n", lastDir[2][0]);
        printf("lastdir[2][1] = %d\n", lastDir[2][1]);
        printf("\nlastdir[3][0] = %d\n", lastDir[3][0]);
        printf("lastdir[3][1] = %d\n", lastDir[3][1]);
        printf("\nlastdir[4][0] = %d\n", lastDir[4][0]);
        printf("lastdir[4][1] = %d\n", lastDir[4][1]);
        printf("\nlastdir[5][0] = %d\n", lastDir[5][0]);
        printf("lastdir[5][1] = %d\n", lastDir[5][1]);
        printf("\nqueuex = %d\n", queue1);
        printf("queuey = %d\n", queue2);*/

        /*Sleep(500);
        if (_kbhit() != 0)//Detecte si on appuie sur le clavier
        {
            input = _getch();//Lecture de la derniere touche appuy�
            fflush(stdin);
        }*/

        networkInput.clear();
        output.clear();

    } while (vie >= 1 && score < 10 && timer < 200);
    //END MAIN GAME LOOP

    if (score < 10)
    {
        return score * 10 + timer;
    }
    else {
        return score * 100 + 200;
    }
    
}
