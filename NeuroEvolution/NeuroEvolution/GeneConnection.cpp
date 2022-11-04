// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneConnection.h"

GeneConnection::GeneConnection()
{
	innovation = -1;
	nodeA = -1;
	nodeB = -1;
}

GeneConnection::GeneConnection(unsigned int _innovation, unsigned int _nodeA, unsigned int _nodeB)
{
	innovation = _innovation;
	nodeA = _nodeA;
	nodeB = _nodeB;
}

GeneConnection::~GeneConnection()
{

}
