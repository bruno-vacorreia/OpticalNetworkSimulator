//
// Created by henrique on 12/03/23.
//

#ifndef OPTICALNETWORKSIMULATOR_INDIVIDUALRSAORDERPROTECTION_H
#define OPTICALNETWORKSIMULATOR_INDIVIDUALRSAORDERPROTECTION_H



#include <vector>
#include <iostream>

class GA_RsaOrderProtection;

#include "Individual.h"
#include "../../GeneralClasses/Def.h"

/**
 * @brief Specific individual for RSA order, composed by boolean genes.
 */
class IndividualRsaOrderProtection : public Individual {
public:
    /**
     * @brief Default constructor for this specified individual.
     * @param ga GA_RsaOrder pointer that owns this individual.
     */
    IndividualRsaOrderProtection(GA_RsaOrderProtection* ga);
    /**
     * @brief Constructor for this individual, that receives a predetermined
     * gene to apply on it.
     * @param ga GA_RsaOrder pointer that owns this individual.
     * @param gene Gene value for the individual.
     */
    IndividualRsaOrderProtection(GA_RsaOrderProtection* ga, ResAllocOrderProtection gene);
    /**
     * @brief Copy constructor for this individual.
     * @param orig Original individual to be copied.
     */
    IndividualRsaOrderProtection(const std::shared_ptr<const IndividualRsaOrderProtection>& orig);
    /**
     * @brief Default destructor of this individual.
     */
    virtual ~IndividualRsaOrderProtection();

    /**
     * @brief Gets the gene of  this individual, for a specified node pair,
     * used to find the container position.
     * @param orNode Source node, used to find the container position.
     * @param deNode Destination node, used to find the container position.
     * @return Gene, in boolean value, of this individual.
     */
    ResAllocOrderProtection GetGene(unsigned int orNode, unsigned int deNode) const;
    /**
     * @brief Gets the container of boolean values, genetical representation
     * of this individual.
     * @return Genes of this individual.
     */
    std::vector<ResAllocOrderProtection> GetGenes() const;
    /**
     * @brief Sets the gene of  this individual, for a specified node pair,
     * used to find the container position.
     * @param orNode Source node, used to find the container position.
     * @param deNode Destination node, used to find the container position.
     * @param value Gene, in boolean value, of this individual.
     */
    void SetGene(unsigned int orNode, unsigned int deNode, ResAllocOrderProtection value);
    /**
     * @brief Gets the blocking probability of this individual.
     * @return Blocking probability.
     */
    double GetBlockProb() const;
    /**
     * @brief Sets the blocking probability of this individual.
     * @param blockProb Blocking probability.
     */
    void SetBlockProb(double blockProb);
    /**
     * @brief Gets the main parameter of this individual.
     * @return First parameter.
     */
    double GetMainParameter() override;
    /**
     * @brief Gets the second parameter of this individual.
     * @return Second parameter.
     */
    double GetSecondParameter() override;

    void Save(std::ostream& bestInd) override;
private:
    /**
     * @brief GA_RsaOrder that owns this individual.
     */
    GA_RsaOrderProtection* ga;
    /**
     * @brief Genes of this individual, with boolean values.
     */
    std::vector<ResAllocOrderProtection> genes;
    /**
     * @brief Blocking probability of this individual.
     */
    double blockProb;
};

#endif //OPTICALNETWORKSIMULATOR_INDIVIDUALRSAORDERPROTECTION_H
