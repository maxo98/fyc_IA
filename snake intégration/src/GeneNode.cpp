// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneNode.h"

GeneNode::GeneNode(NODE_TYPE _type, Activation* _activationFunction, unsigned int _layer)
{
	type = _type;
	layer = _layer;
	activationFunction = _activationFunction;

	if (layer > 100)
	{
		int err;
	}
}

GeneNode::~GeneNode()
{
}
