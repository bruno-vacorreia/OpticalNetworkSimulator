//
// Created by henrique on 12/03/23.
//

#ifndef OPTICALNETWORKSIMULATOR_GA_RSAORDERPROTECTION_H
#define OPTICALNETWORKSIMULATOR_GA_RSAORDERPROTECTION_H

#include "GA_SO.h"

class SimulationType;
class IndividualRsaOrderProtection;

/**
 * @brief GA algorithm applied for RSA order.
 */
class GA_RsaOrderProtection : public GA_SO {
public:
    /**
     * @brief Default constructor for GA RSA order.
     * @param simul SimulationType that owns this GA RSA order.
     */
    GA_RsaOrderProtection(SimulationType* simul);
    /**
     * @brief Default destructor for GA RSA order.
     */
    virtual ~GA_RsaOrderProtection();

    /**
     * @brief Initialize the base class, set the number of nodes, that will
     * require to construct the vector of genes, and set the boolean
     * distribution.
     */
    void Initialize() override;
    /**
     * @brief Function to create the initial population in the selected
     * population container. This GA create individuals of the type
     * IndividualBool class.
     */
    void InitializePopulation() override;
    /**
     * @brief Call the functions of crossover and mutation for create a new
     * population.
     */
    void CreateNewPopulation() override;
    /**
     * @brief Keeps the selected population in the initial population container.
     */
    void KeepInitialPopulation() override;
    /**
     * @brief Function that saves the best and the worst individuals in their
     * respective containers.
     */
    void SaveIndividuals() override;

    /**
     * @brief Gets the number of nodes of the network.
     * @return Number of nodes.
     */
    unsigned int GetNumNodes() const;
    /**
     * @brief Sets the number of nodes of the network.
     * @param numNodes Number of nodes.
     */
    void SetNumNodes(unsigned int numNodes);
    /**
     * @brief Gets a randomly integer value.
     * @return Boolean value.
     */
    ResAllocOrderProtection GetIntDistribution();

    /**
     * @brief Apply the genes of a specified individual in the network RSA
     * order .
     * @param ind Specified individual.
     */
    void ApplyIndividual(Individual* ind) override;
    /**
     * @brief Set the individual parameters. For this GA, set the blocking
     * probability found and the fitness.
     * @param ind Specified individual.
     */
    void SetIndParameters(Individual* ind) override;
    /**
     * @brief Sets the fitness of all individuals in the selected population
     * container.
     */
    void SetSelectedPopFitness() override;
    /**
     * @brief Sets the fitness of all individuals in the total population
     * container.
     */
    void SetTotalPopFitness() override;
private:
    /**
     * @brief Function to create new individuals by crossover.
     * Uses the roulette to choose the parents.
     */
    void Crossover();
    /**
     * @brief Function to generate two new individuals of two specified
     * parents.
     * @param ind1 First parent.
     * @param ind2 Second parent.
     */
    void GenerateNewIndividuals(const IndividualRsaOrderProtection* const ind1,
                                const IndividualRsaOrderProtection* const ind2);
    /**
     * @brief One point crossover, in which two parents generate two new
     * individuals. A index of the genes is selected. One new individual will
     * receive the genes of the first parent from 0 to the selected index, and
     * of the second from that index to the end. The other will receive the
     * opposite genes structure.
     * @param ind1 First parent.
     * @param ind2 Second parent.
     */
    void OnePointCrossover(const IndividualRsaOrderProtection* const ind1,
                           const IndividualRsaOrderProtection* const ind2);
    /**
     * @brief Two point crossover, in which two parents generate two new
     * individuals. Two index of the genes is selected. One new individual will
     * receive the genes of the first parent from 0 to the selected first index
     * and the second index to the end. The other will receive the opposite
     * genes structure.
     * @param ind1 First parent.
     * @param ind2 Second parent.
     */
    void TwoPointCrossover(const IndividualRsaOrderProtection* const ind1,
                           const IndividualRsaOrderProtection* const ind2);
    /**
     * @brief Uniform crossover, in which two parents generate two new
     * individuals. the crossover is done by gene, in which the first new
     * individual has a crossover probability to receive the gene of the first
     * parent. The other new individual will receive the gene of the other
     * parent.
     * @param ind1 First parent.
     * @param ind2 Second parent.
     */
    void UniformCrossover(const IndividualRsaOrderProtection* const ind1,
                          const IndividualRsaOrderProtection* const ind2);
    /**
     * @brief Apply the mutation in all the new individuals created by the
     * crossover process. After that, this function add the possible parents,
     * in the selected population container, to the total population container.
     * The selected population is clean.
     */
    void Mutation();
    /**
     * @brief Apply the mutation in a specified individual. Each gene of this
     * individual has the mutation probability to be generated randomly.
     * @param ind Specified individual.
     */
    void MutateIndividual(IndividualRsaOrderProtection* const ind);
private:
    /**
     * @brief Number of nodes in the network, used to construct the gene vector.
     */
    unsigned int numNodes;
    /**
     * @brief Boolean distribution.
     */
    std::uniform_int_distribution<int> intDistribution;

};


#endif //OPTICALNETWORKSIMULATOR_GA_RSAORDERPROTECTION_H
