#include "HyperBackprop.h"

void testHyperBackprop()
{
    Activation* tanh = new TanhActivation();

	NeatParameters neatparam;

    neatparam.activationFunctions.push_back(tanh);

    HyperneatParameters hyperneatParam;

    hyperneatParam.activationFunction = new LinearActivation();
    hyperneatParam.cppnInput = 5;
    hyperneatParam.cppnInputFunction = biasCppnInput;
    hyperneatParam.cppnOutput = 1;
    hyperneatParam.nDimensions = 2;
    hyperneatParam.thresholdFunction = noThreshold;
    hyperneatParam.weightModifierFunction = noChangeWeight;
    hyperneatParam.inverseWeightModifierFunction = noChangeWeight;

    std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> allConn;

    std::vector<Activation*> arrActiv;
    arrActiv.push_back(tanh);

    Genome gen(5, 1, arrActiv);

    std::vector<int> layerSize;
    layerSize.push_back(5);

    for (int layer = 1; layer < 3; layer++)
    {
        layerSize.push_back(3);

        for (int i = 0; i < 3; i++)
        {
            gen.addHiddenNode(tanh, layer);
        }
    }

    layerSize.push_back(1);

    int offsetIn = 0;
    int offsetOut = hyperneatParam.cppnOutput;

    for (int layer = 1; layer < layerSize.size(); layer++)
    {
        offsetOut += layerSize[layer - 1];

        if (layer == (layerSize.size() - 1))
        {
            offsetOut = hyperneatParam.cppnInput;
        }

        for (int i = 0; i < layerSize[layer-1]; i++)
        {
            for (int cpt = 0; cpt < layerSize[layer]; cpt++)
            {
                gen.addConnection(offsetIn + i, offsetOut + cpt, allConn);
            }
        }

        offsetIn += layerSize[layer - 1];

        if (layer == 1)
        {
            offsetIn += hyperneatParam.cppnOutput;
        }
    }

    std::vector<Genome> genVec;

    genVec.push_back(gen);

    Hyperneat hyper(1, neatparam, hyperneatParam, genVec);

    std::vector<float> pos, inputs, outputs, expectedOutputs;
    pos.resize(2);


    for (int i = 1; i < 11; i++)
    {
        pos[0] = i;
        for (int cpt = 1; cpt < 11; cpt++)
        {
            pos[1] = cpt;

            hyper.addInput(pos);
            hyper.addOutput(pos);
            inputs.push_back(1);
            expectedOutputs.push_back(-1);
        }
    }

    hyper.initNetworks();

    hyper.generateNetworks();

    hyper.getNeuralNetwork(0)->compute(inputs, outputs);

    std::cout << outputs << std::endl;

    for (int i = 0; i < 100; i++)
    {
        hyper.backprop(inputs, expectedOutputs, 0.01);
    }

    hyper.applyBackprop();

    hyper.getNeuralNetwork(0)->compute(inputs, outputs);

    std::cout << outputs << std::endl;
}