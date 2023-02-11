#include "Species.h"
#include <algorithm>

Species::Species(Genome* first)
{
	Add(first);
}

void Species::computeScore()
{
	bool improved = false;
	maxFitness = 0;
	float totalFitness = 0;

	for (std::vector<Genome*>::iterator it = genomes.begin(); it != genomes.end(); ++it)
	{
		if (champ == nullptr || champ->getScore() < (*it)->getScore())
		{
			champ = (*it);
		}

		if ((*it)->getScore() < 0) continue;

		totalFitness += (*it)->getScore();

		if ((*it)->getScore() > maxFitness)
		{
			maxFitness = (*it)->getScore();

			if (maxFitness > maxFitnessEver)
			{
				maxFitnessEver = maxFitness;
				improved = true;
			}
		}
	}

	avgFitness = totalFitness / genomes.size();

	if (improved == true)
	{
		lastImprove = age;
	}
}

void Species::sort()
{
	std::sort(genomes.begin(), genomes.end(), genomeSortDesc);
}

void Species::Add(Genome* gen)
{
	genomes.push_back(gen);

	if (champ == nullptr || champ->getScore() < gen->getScore())
	{
		champ = gen;
	}
}

void Species::countOffspring(float& skim, const float& totalFitness, const int& popSize)//Uses the average fitness of the whole population 
{
	expectedOffspring = 0;
	int skimIntPart;  //The whole offspring in the skim

	for (std::vector<Genome*>::iterator it = genomes.begin(); it != genomes.end(); ++it)
	{
		float eoGen = (*it)->getSpeciesScore() / totalFitness * popSize;
		int eoIntPart = floor(eoGen);  //The floor of an organism's expected offspring
		float eoFracPart = fmod(eoGen, 1.0); //Expected offspring fractional part

		expectedOffspring += eoIntPart;
		skim += eoFracPart;

		//NOTE:  Some precision is lost by computer
		//       Must be remedied later
		if (skim >= 1.0) 
		{
			skimIntPart = floor(skim);
			expectedOffspring += skimIntPart;
			skim -= skimIntPart;
		}
	}
}

bool Species::remove(Genome* gen)
{
	for (std::vector<Genome*>::iterator it = genomes.begin(); it != genomes.end(); ++it)
	{
		if (*it == gen)
		{
			genomes.erase(it, std::next(it));

			//Species should not get empty before reproduction
			if (genomes.empty() == true)
			{
				genomes.push_back(champ);
			}

			return true;
		}
	}

	return false;
}