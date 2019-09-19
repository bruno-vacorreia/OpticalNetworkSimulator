/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ParticlePSO_SCRA.h
 * Author: brunovacorreia
 *
 * Created on September 16, 2019, 2:15 PM
 */

#ifndef PARTICLEPSO_SCRA_H
#define PARTICLEPSO_SCRA_H

#include "ParticlePSO.h"

class ResourceDeviceAlloc;
class SCRA;

class ParticlePSO_SCRA : public ParticlePSO {
public:
    
    ParticlePSO_SCRA(PSO* pso, Data* data, ResourceAlloc* resAlloc);
    
    ParticlePSO_SCRA(const std::shared_ptr<const ParticlePSO_SCRA>& orig);

    virtual ~ParticlePSO_SCRA();
    
    void CalculateFitness() override;
private:
    
    void ApplyCoefficients();
private:
    
    ResourceDeviceAlloc* resDevAlloc;
    
    SCRA* scra;
};

#endif /* PARTICLEPSO_SCRA_H */
