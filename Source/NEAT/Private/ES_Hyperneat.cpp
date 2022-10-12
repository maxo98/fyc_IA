// Fill out your copyright notice in the Description page of Project Settings.


#include "ES_Hyperneat.h"

ES_Hyperneat::ES_Hyperneat(unsigned int _populationSize, NeatParameters _neatParamters, HyperneatParameters _hyperparam, unsigned int _nDecomposition):
	Hyperneat(_populationSize, _neatParamters, _hyperparam)
{
	nDecomposition = _nDecomposition;
}

ES_Hyperneat::~ES_Hyperneat()
{
}

void ES_Hyperneat::generateNetworks()
{

}