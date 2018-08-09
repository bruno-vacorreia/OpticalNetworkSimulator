/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SimulationType.cpp
 * Author: bruno
 * 
 * Created on August 2, 2018, 3:50 PM
 */

#include "../../include/SimulationType/SimulationType.h"

#include "../../include/Data/Parameters.h"
#include "../../include/Data/Options.h"
#include "../../include/Data/Data.h"
#include "../../include/Data/InputOutput.h"
#include "../../include/Structure/Topology.h"

SimulationType::SimulationType(unsigned int simulIndex)
:simulationIndex(simulIndex),
parameters(std::make_shared<Parameters> (this)), 
options(std::make_shared<Options> (this)), 
data(boost::make_unique<Data>(this)), 
topology(std::make_shared<Topology> (this)),
inputOutput(boost::make_unique<InputOutput>(this)){
    
}

SimulationType::~SimulationType() {
    
}

void SimulationType::LoadFile() {
    
}

Parameters* SimulationType::GetParameters() const {
    return parameters;
}

void SimulationType::SetParameters(std::shared_ptr<Parameters> parameters) {
    this->parameters = parameters;
}

Options* SimulationType::GetOptions() const {
    return options;
}

void SimulationType::SetOptions(std::shared_ptr<Options> options) {
    this->options = options;
}

Data* SimulationType::GetData() const {
    return data;
}

void SimulationType::SetData(std::unique_ptr<Data> data) {
    this->data = data;
}

Topology* SimulationType::GetTopology() const {
    return topology;
}

void SimulationType::SetTopology(std::shared_ptr<Topology> topology) {
    this->topology = topology;
}

InputOutput* SimulationType::GetInputOutput() const {
    return inputOutput;
}

void SimulationType::SetInputOutput(std::unique_ptr<InputOutput> inputOutput) {
    this->inputOutput = inputOutput;
}
