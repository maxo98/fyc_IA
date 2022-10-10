// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneNode.h"

GeneNode::GeneNode(NODE_TYPE _type, ActivationFunction _activationFunction, int _layer)
{
	type = _type;
	layer = _layer;
	activationFunction = _activationFunction;
}

GeneNode::~GeneNode()
{
}
