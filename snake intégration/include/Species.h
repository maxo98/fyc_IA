#pragma once

#include "Genome.h"

class Species
{
public:
	Species(Genome* first);

	void Add(Genome*);
	void computeScore();
	void countOffspring(float& skim, const float& totalFitness, const int& popSize);
	bool remove(Genome* gen);
	void sort();

	inline std::vector<Genome*>* getSpecies() { return &genomes; };

	inline float getAvgFitness() const { return avgFitness; };
	inline float getMaxFitness() const { return maxFitness; };

	inline unsigned int getExpectedOffspring() { return expectedOffspring; };
	inline void setExpectedOffspring(unsigned int value) { expectedOffspring = value; };
	inline void incrementExpectedOffspring() { ++expectedOffspring; };
	inline void decrementExpectedOffspring() { --expectedOffspring; };

	inline Genome* getChamp() { return champ; };
	inline void setChamp(Genome* value) { champ = value; };
	inline bool getNovel() { return novel; }
	inline void setNovel(bool value) { novel = value; }
	inline void setPopChamp(bool value) { popChamp = value; }
	inline bool isPopChamp() { return popChamp; }

	int age = 1; //The age of the Species 
	bool obliterate = false;  //Allows killing off in competitive coevolution stagnation
	int lastImprove = 0;  //If this is too long ago, the Species will goes extinct

private:
	std::vector<Genome*> genomes;
	Genome* champ = nullptr;

	float avgFitness = 0; //The average fitness of the Species
	float maxFitness = 0; //Max fitness of the Species
	float maxFitnessEver = 0; //The max it ever had
	unsigned int expectedOffspring = 0;
	bool novel = true;
	bool popChamp = false;
};