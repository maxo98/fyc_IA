#include "BackpropTest.h"
#include "Neat.h"

void backpropTest()
{
	NeuralNetwork network;

	Activation* sigmoid = new SigmoidActivation();

	std::vector<Activation*> arrActiv;
	arrActiv.push_back(sigmoid);

	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> allConn;

	Genome gen(2, 1, arrActiv);

	gen.addHiddenNode(sigmoid, 1);

	gen.addConnection(0, 3, allConn);
	gen.addConnection(1, 3, allConn);
	gen.addConnection(3, 2, allConn);

	Neat::genomeToNetwork(gen, network);

	std::vector<float> input, output, tmp;

	input.push_back(-1);
	input.push_back(-1);
	output.push_back(1);

	network.compute(input, tmp);

	std::cout << tmp << std::endl;

	for (int i = 0; i < 1000; i++)
	{
		network.backprop(input, output, 0.2);

		network.compute(input, tmp);

		std::cout << tmp << std::endl;
	}

	network.compute(input, tmp);

	std::cout << tmp << std::endl;

	network.applyBackprop(gen);

	Neat::genomeToNetwork(gen, network);

	network.compute(input, tmp);

	std::cout << tmp << std::endl;
}