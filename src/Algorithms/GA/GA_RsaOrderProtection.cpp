//
// Created by henrique on 12/03/23.
//


#include "../../../include/Algorithms/GA/GA_RsaOrderProtection.h"
#include "../../../include/Structure/Topology.h"
#include "../../../include/Algorithms/GA/IndividualRsaOrderProtection.h"
#include "../../../include/SimulationType/SimulationType.h"
#include "../../../include/ResourceAllocation/ResourceAlloc.h"
#include "../../../include/Data/Data.h"
#include "../../../include/GeneralClasses/Def.h"

GA_RsaOrderProtection::GA_RsaOrderProtection(SimulationType* simul)
        :GA_SO(simul), numNodes(0) {

}

GA_RsaOrderProtection::~GA_RsaOrderProtection() {

}

void GA_RsaOrderProtection::Initialize() {
    GA_SO::Initialize();
    this->SetNumNodes(this->GetSimul()->GetTopology()->GetNumNodes());
    this->intDistribution = std::uniform_int_distribution<int>(r_sa_MinHop, sa_r_LowHighSlotIndex);
}

void GA_RsaOrderProtection::InitializePopulation() {
    assert(this->selectedPopulation.empty() && this->totalPopulation.empty());

    for(unsigned int a = 0; a < this->GetNumberIndividuals(); a++){

        if(a == 0){
            this->selectedPopulation.push_back(std::make_shared
                                                       <IndividualRsaOrderProtection>(this, r_sa));
        }
        else if(a == 1){
            this->selectedPopulation.push_back(std::make_shared
                                                       <IndividualRsaOrderProtection>(this, sa_r));
        }
        else{
            this->selectedPopulation.push_back(std::make_shared
                                                       <IndividualRsaOrderProtection>(this));
        }
    }
}

void GA_RsaOrderProtection::CreateNewPopulation() {
    this->totalPopulation.clear();
    this->SetSumFitnessSelectedPop();
    this->Crossover();
    this->Mutation();
}

void GA_RsaOrderProtection::KeepInitialPopulation() {
    GA_SO::KeepInitialPopulation();
    std::shared_ptr<IndividualRsaOrderProtection> auxInd;

    for(auto it: this->selectedPopulation){
        auxInd = std::dynamic_pointer_cast<IndividualRsaOrderProtection>(it);
        this->initialPopulation.push_back(
                std::make_shared<IndividualRsaOrderProtection>(auxInd));
    }
    std::sort(this->initialPopulation.begin(), this->initialPopulation.end(),
              IndividualCompare());
}

void GA_RsaOrderProtection::SaveIndividuals() {
    GA_SO::SaveIndividuals();
    std::shared_ptr<IndividualRsaOrderProtection> auxInd;

    auxInd = std::dynamic_pointer_cast<IndividualRsaOrderProtection>
            (this->selectedPopulation.back());
    this->bestIndividuals.push_back(std::make_shared<IndividualRsaOrderProtection>(auxInd));
    auxInd = std::dynamic_pointer_cast<IndividualRsaOrderProtection>
            (this->selectedPopulation.front());
    this->worstIndividuals.push_back(std::make_shared<IndividualRsaOrderProtection>(auxInd));
}

unsigned int GA_RsaOrderProtection::GetNumNodes() const {
    return numNodes;
}

void GA_RsaOrderProtection::SetNumNodes(unsigned int numNodes) {
    assert(numNodes > 0);

    this->numNodes = numNodes;
}

ResAllocOrderProtection GA_RsaOrderProtection::GetIntDistribution() {
    return (ResAllocOrderProtection) intDistribution(this->random_generator);
}

void GA_RsaOrderProtection::ApplyIndividual(Individual* ind) {
    IndividualRsaOrderProtection* indInt = dynamic_cast<IndividualRsaOrderProtection*>(ind);
    this->GetSimul()->GetResourceAlloc()
            ->SetResourceAllocOrderProtection(indInt->GetGenes());
}

void GA_RsaOrderProtection::SetIndParameters(Individual* ind) {
    double blockProb = this->GetSimul()->GetData()->GetReqBP();
    IndividualRsaOrderProtection* indInt = dynamic_cast<IndividualRsaOrderProtection*>(ind);

    indInt->SetBlockProb(blockProb);
}

void GA_RsaOrderProtection::SetSelectedPopFitness() {
    double bestPb = Def::Max_Double;
    IndividualRsaOrderProtection* auxInd;

    //Find the best blocking probability of the selectedPop container
    for(auto it: this->selectedPopulation){
        auxInd = dynamic_cast<IndividualRsaOrderProtection*>(it.get());

        if(bestPb > auxInd->GetBlockProb())
            bestPb = auxInd->GetBlockProb();
    }

    for(auto it: this->selectedPopulation){
        auxInd = dynamic_cast<IndividualRsaOrderProtection*>(it.get());
        auxInd->SetFitness(1.0 / (bestPb + auxInd->GetBlockProb()));
    }
}

void GA_RsaOrderProtection::SetTotalPopFitness() {
    double bestPb = Def::Max_Double;
    IndividualRsaOrderProtection* auxInd;

    //Find the best blocking probability of the selectedPop container
    for(auto it: this->totalPopulation){
        auxInd = dynamic_cast<IndividualRsaOrderProtection*>(it.get());

        if(bestPb > auxInd->GetBlockProb())
            bestPb = auxInd->GetBlockProb();
    }

    for(auto it: this->totalPopulation){
        auxInd = dynamic_cast<IndividualRsaOrderProtection*>(it.get());
        auxInd->SetFitness(1.0 / (bestPb + auxInd->GetBlockProb()));
    }
}

void GA_RsaOrderProtection::Crossover() {
    assert(this->selectedPopulation.size() == this->GetNumberIndividuals());
    assert(this->totalPopulation.empty());
    IndividualRsaOrderProtection *auxInd1, *auxInd2;

    while(this->totalPopulation.size() < this->GetNumberIndividuals()){
        auxInd1 = dynamic_cast<IndividualRsaOrderProtection*>(this->RouletteIndividual());
        do{
            auxInd2 = dynamic_cast<IndividualRsaOrderProtection*>
            (this->RouletteIndividual());
        }while(auxInd1 == auxInd2);

        this->GenerateNewIndividuals(auxInd1, auxInd2);
    }
}

void GA_RsaOrderProtection::GenerateNewIndividuals(const IndividualRsaOrderProtection* const ind1,
                                         const IndividualRsaOrderProtection* const ind2) {
    //Put condition for choose  the crossover
    this->UniformCrossover(ind1, ind2);
}

void GA_RsaOrderProtection::UniformCrossover(const IndividualRsaOrderProtection* const ind1,
                                   const IndividualRsaOrderProtection* const ind2) {
    std::shared_ptr<IndividualRsaOrderProtection> newInd1 =
            std::make_shared<IndividualRsaOrderProtection>(this);
    std::shared_ptr<IndividualRsaOrderProtection> newInd2 =
            std::make_shared<IndividualRsaOrderProtection>(this);
    double auxProb;

    for(unsigned int a = 0; a < this->numNodes; a++){
        for(unsigned int b = 0; b < this->numNodes; b++){
            auxProb = this->GetProbDistribution();

            if(auxProb < this->GetProbCrossover()){
                newInd1->SetGene(a, b, ind1->GetGene(a, b));
                newInd2->SetGene(a, b, ind2->GetGene(a, b));
            }
            else{
                newInd1->SetGene(a, b, ind2->GetGene(a, b));
                newInd2->SetGene(a, b, ind1->GetGene(a, b));
            }
        }
    }

    this->totalPopulation.push_back(newInd1);
    this->totalPopulation.push_back(newInd2);
}

void GA_RsaOrderProtection::Mutation() {
    assert(this->totalPopulation.size() == this->GetNumberIndividuals());

    for(auto it: this->totalPopulation){
        this->MutateIndividual(dynamic_cast<IndividualRsaOrderProtection*>(it.get()));
    }

    this->totalPopulation.insert(this->totalPopulation.end(),
                                 this->selectedPopulation.begin(), this->selectedPopulation.end());

    this->selectedPopulation.clear();
}

void GA_RsaOrderProtection::MutateIndividual(IndividualRsaOrderProtection* const ind) {
    double auxProb;

    for(unsigned int a = 0; a < this->numNodes; a++){
        for(unsigned int b = 0; b < this->numNodes; b++){
            auxProb = this->GetProbDistribution();

            if(auxProb < this->GetProbMutation())
                ind->SetGene(a, b, !ind->GetGene(a, b));
        }
    }
}
