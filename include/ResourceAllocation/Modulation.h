/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Modulation.h
 * Author: BrunoVinicius
 *
 * Created on November 28, 2018, 1:56 PM
 */

#ifndef MODULATION_H
#define MODULATION_H

class ResourceAlloc;
class Call;

#include <boost/assign/list_of.hpp>
#include <boost/unordered_map.hpp>

/**
 * @brief Numerate the types of modulation allowed in this simulator.
 */
enum TypeModulation{
    InvalidModulation,
    QAM_4,
    QAM_8,
    QAM_16,
    QAM_32,
    QAM_64,
    FirstModulation = QAM_4,
    LastModulation = QAM_64,
    FixedModulation = QAM_8
};

/**
 * @brief Class responsible to calculate the call requests parameters based in
 * its modulation format.
 */
class Modulation {

public:
    /**
     * @brief Default constructor of a modulation object.
     * @param resourAlloc Resource allocation object pointer.
     * @param slotBand Slot bandwidth size, used to calculate the parameters.
     */
    Modulation(ResourceAlloc* resourAlloc, double slotBand);
    /**
     * @brief Default destructor of a modulation object.
     */
    virtual ~Modulation();
    
    /**
     * @brief Calculates the physical layer parameters of a call request a apply
     * the values to it.
     * @param call Call request to be analyzed.
     */
    void SetModulationParam(Call* call);
    
    void SetModulationParam(std::vector<Call*> calls);
    /**
     * @brief Function to calculate the spectral bandwidth of a optical signal, 
     * based on the modulation format and the bit rate.
     * @param M Number of bits used to compose the modulation variations.
     * @param Rbps Bit rate to transmit.
     * @return Spectral bandwidth needed to transmit.
     */
    double BandwidthQAM(unsigned int M, double Rbps);
    
    double GetOSNRQAM(unsigned int M, double Rbps);
    
    double GetSNRbQAM(unsigned int M);
    
    double GetsnrbQAM(unsigned int M);
    /**
     * @brief Check if the simulation is EON, based on the slot bandwidth.
     * @return True if it is an EON simulation.
     */
    bool isEON() const;
    /**
     * @brief Gets the possible slots set that the call requests can use.
     * @param traffic Container with all possible traffics.
     * @return Container of possible slots.
     */
    std::vector<unsigned int> GetPossibleSlots(std::vector<double> traffic);
private:
    /**
     * @brief Gets the possible slots set that the call requests can use with
     * a fixed modulation format allowed.
     * @param traffic Container with all possible traffics.
     * @return Container of possible slots.
     */
    std::vector<unsigned int> GetPossibleSlotsFixedMod(std::vector<double>&
                                                       traffic);
    /**
     * @brief Gets the possible slots set that the call requests can use for
     * with all modulation format allowed.
     * @param traffic Container with all possible traffics.
     * @return Container of possible slots.
     */
    std::vector<unsigned int> GetPossibleSlotsVariableMod(std::vector<double>&
                                                          traffic);
private:
    /**
     * @brief ResouseAlloc object that owns the modulation object.
     */
    ResourceAlloc* resourAlloc;
    /**
     * @brief Spectral slot bandwidth used in this simulation.
     */
    double slotBandwidth;
    /**
     * @brief Bit error rate used in this simulation.
     */
    const double BER;
    /**
     * @brief Number of polarizations used in the signal transmmissions.
     */
    const unsigned int polarization;
    
    const double rollOff;
    /**
     * @brief Map composed by the modulation format options and its respective
     * amount of bits to represent the signal.
     */
    static const boost::unordered_map<TypeModulation, 
                                      unsigned int> mapModulation;
};

#endif /* MODULATION_H */

