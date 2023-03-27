//
// Created by henrique on 12/03/23.
//


#include "../../../include/Algorithms/GA/IndividualRsaOrderProtection.h"
#include "../../../include/Algorithms/GA/GA_RsaOrderProtection.h"

IndividualRsaOrderProtection::IndividualRsaOrderProtection(GA_RsaOrderProtection* ga)
        :Individual(ga), ga(ga), genes(0), blockProb(0.0) {
    const unsigned int numNodes = this->ga->GetNumNodes();

    for(unsigned int a = 0; a < numNodes*numNodes; a++){
        this->genes.push_back(this->ga->GetIntDistribution());
    }
}

IndividualRsaOrderProtection::IndividualRsaOrderProtection(GA_RsaOrderProtection* ga, ResAllocOrderProtection gene)
        :Individual(ga), ga(ga), genes(0), blockProb(0.0) {
    const unsigned int numNodes = this->ga->GetNumNodes();

    for(unsigned int a = 0; a < numNodes*numNodes; a++){
        this->genes.push_back(gene);
    }
}

IndividualRsaOrderProtection::IndividualRsaOrderProtection(
        const std::shared_ptr<const IndividualRsaOrderProtection>& orig)
        :Individual(orig), ga(orig->ga), genes(orig->genes),
         blockProb(orig->blockProb) {

}

IndividualRsaOrderProtection::~IndividualRsaOrderProtection() {

}

ResAllocOrderProtection IndividualRsaOrderProtection::GetGene(unsigned int orNode,
                                      unsigned int deNode) const {
    return this->genes.at(orNode*this->ga->GetNumNodes()+deNode);
}

std::vector<ResAllocOrderProtection> IndividualRsaOrderProtection::GetGenes() const {
    return genes;
}

void IndividualRsaOrderProtection::SetGene(unsigned int orNode, unsigned int deNode,
                                           ResAllocOrderProtection value) {
    this->genes.at(orNode*this->ga->GetNumNodes()+deNode) = value;
}

double IndividualRsaOrderProtection::GetBlockProb() const {
    return blockProb;
}

void IndividualRsaOrderProtection::SetBlockProb(double blockProb) {
    assert(this->GetCount() < this->ga->GetMaxNumSimulation());
    assert(blockProb >= 0.0);
    this->SetCount(this->GetCount()+1);

    if(this->GetCount() == 1)
        this->blockProb = blockProb;
    this->blockProb = (this->blockProb + blockProb) / 2.0;
}

double IndividualRsaOrderProtection::GetMainParameter() {
    return this->GetBlockProb();
}

double IndividualRsaOrderProtection::GetSecondParameter() {
    return this->GetMainParameter();
}

void IndividualRsaOrderProtection::Save(std::ostream &bestInd) {

}

