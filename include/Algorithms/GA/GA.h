/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GA.h
 * Author: BrunoVinicius
 *
 * Created on February 27, 2019, 10:07 PM
 */

#ifndef GA_H
#define GA_H

class SimulationType;

#include <vector>
#include <memory>
#include <cassert>
#include <random>
#include <algorithm>
#include <iostream>

#include "Individual.h"

/**
 * @brief Generic genetic algorithm class. This class contain the similar 
 * parameters, such as number of generations, number of individuals, etc, 
 * to all specific genetic algorithms.
 */
class GA {
    
    friend std::ostream& operator<<(std::ostream& ostream, const GA* ga);

public:
    /**
     * @brief Default constructor of a GA algorithm.
     * @param simul Simulation that own this algorithm.
     */
    GA(SimulationType* simul);
    /**
     * @brief Default destructor of a GA algorithm.
     */
    virtual ~GA();
    
    /**
     * @brief Initialize the GA algorithm with the probability distribution.
     */
    virtual void Initialize();
    /**
     * @brief Initialize the population with random individuals, created only 
     * in the derived class. 
     */
    virtual void InitializePopulation() = 0;
    /**
     * @brief Function to create the new population based on the select 
     * population.
     */
    virtual void CreateNewPopulation() = 0;
    /**
     * @brief Keeps the selected population in the initial population container.
     */
    virtual void KeepInitialPopulation() = 0;
    /**
     * @brief Selects the best population among the total population generated
     * by crossover and mutation. The selection is made first, selecting a 
     * specified number of best individuals, and for last, choosing randomly
     * the rest of the selected population.
     */
    virtual void SelectPopulation() = 0;
    /**
     * @brief Function that saves the best and the worst individuals in their
     * respective containers.
     */
    virtual void SaveIndividuals() = 0;
    
    /**
     * @brief Gets the number of generation of this GA algorithm.
     * @return Number of generations.
     */
    const unsigned int GetNumberGenerations() const;
    /**
     * @brief Gets the number of individuals of this GA algorithm.
     * @return Number of individuals.
     */
    const unsigned int GetNumberIndividuals() const;
    /**
     * @brief Gets the crossover probability of this GA algorithm.
     * @return Crossover probability.
     */
    const double GetProbCrossover() const;
    /**
     * @brief Gets the mutation probability of this GA algorithm.
     * @return Mutation probability.
     */
    const double GetProbMutation() const;
    /**
     * @brief Gets the maximum number of simulations per individual.
     * @return Maximum number of simulations.
     */
    const unsigned int GetMaxNumSimulation() const;

    /**
     * @brief Return the probability generated by the probDistribution.
     * @return Random probability.
     */
    double GetProbDistribution();
    /**
     * @brief Gets the simulation that owns the GA algorithm.
     * @return SimulationType pointer.
     */
    SimulationType* GetSimul() const;
    /**
     * @brief Gets the actual generation of this GA algorithm.
     * @return Actual generation.
     */
    unsigned int GetActualGeneration() const;
    /**
     * @brief Sets the actual generation of this GA algorithm.
     * @param actualGeneration Actual generation.
     */
    void SetActualGeneration(unsigned int actualGeneration);
    
    /**
     * @brief Runs the simulation for the selected population of the GA.
     */
    virtual void RunSelectPop() = 0;
    /**
     * @brief @brief Runs the simulation for the total population of the GA.
     */
    virtual void RunTotalPop() = 0;
    /**
     * @brief Check and run the simulation if there is any individual, of total
     * population, with less simulation than the minimum required.
     */
    virtual void CheckMinSimul() = 0;
private:
    /**
     * @brief Pointer to a SimulationType object that owns this algorithm.
     */
    SimulationType* simul;
    /**
     * @brief Total number of individuals.
     */
    const unsigned int numberIndividuals;
    /**
     * @brief Total number of generations.
     */
    const unsigned int numberGenerations;
    /**
     * @brief Crossover probability.
     */
    const double probCrossover;
    /**
     * @brief Mutation probability.
     */
    const double probMutation;
    /**
     * @brief Actual generation of this GA algorithm.
     */
    unsigned int actualGeneration;
    /**
     * @brief Number of maximum simulation each individual will do.
     */
    const unsigned int maxNumSimulation;
    
    /**
     * @brief Probability distribution used in this GA algorithm for crossover
     * and mutation.
     */
    std::uniform_real_distribution<double> probDistribution;
    
public:
    /**
     * @brief Random generator.
     */
    static std::default_random_engine random_generator;
};

#endif /* GA_H */

