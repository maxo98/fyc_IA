#pragma once

#include "Genome.h"

class Species
{
public:
	Species(Genome* first);

	void Add(Genome*);
	void computeScore();
	void countOffspring(float& skim, const float& avgFitness);
	bool remove(Genome* gen);

	inline std::vector<Genome*>* getSpecies() { return &genomes; };

	inline float getAvgFitness() const { return avgFitness; };
	inline float getMaxFitness() const { return maxFitness; };

	inline int getExpectedOffspring() { return expectedOffspring; };
	inline void setExpectedOffspring(int value) { expectedOffspring = value; };
	inline void incrementExpectedOffspring() { ++expectedOffspring; };

	inline Genome* getChamp() { return champ; };
	inline void setChamp(Genome* value) { champ = value; };

	int age = 1; //The age of the Species 
	bool obliterate = false;  //Allows killing off in competitive coevolution stagnation
	int lastImprove = 0;  //If this is too long ago, the Species will goes extinct

private:
	std::vector<Genome*> genomes;
	Genome* champ = nullptr;

	float avgFitness = 0; //The average fitness of the Species
	float maxFitness = 0; //Max fitness of the Species
	float maxFitnessEver = 0; //The max it ever had
	int expectedOffspring = 0;
	bool novel = false;
	bool checked;
};