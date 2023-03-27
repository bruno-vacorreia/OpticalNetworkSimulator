/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ResourceDeviceAlloc.h
 * Author: brunovacorreia
 *
 * Created on May 27, 2019, 10:41 PM
 */

#ifndef RESOURCEDEVICEALLOC_H
#define RESOURCEDEVICEALLOC_H

#include "ResourceAlloc.h"
#include "../Data/Options.h"
#include "Modulation.h"

class Call;
class CallDevices;
class RegeneratorAssignment;
class ProtectionScheme;

/**
 * @brief Class responsible to apply the resource allocation with devices.
 */
class ResourceDeviceAlloc : public ResourceAlloc {
public:
    /**
     * @brief Standard constructor for a resource allocation with devices use.
     * @param simulType Simulation that owns this object.
     */
    ResourceDeviceAlloc(SimulationType *simulType);
    /**
     * @brief Standard destructor for a resource allocation wiith devices.
     */
    virtual ~ResourceDeviceAlloc();
    /**
     * @brief Load the ResourceAlloc.
     */
    void Load() override;
    
    void AdditionalSettings() override;

    /**
     * @brief Choose the type of resource allocation for a call, based on the 
     * option chosen.
     * @param call Call request to apply resource allocation.
     */
    void ResourAlloc(Call* call) override;
    /**
     * @brief Function to apply the resource allocation with virtualized 
     * regeneration option. First apply the routing, than try one possible
     * allocation combination of route with regeneration option. The order
     * is decided based on the metric chosen.
     * @param call Call request the function will try to allocate.
     */
    void RoutingOffVirtRegSpecAlloc(CallDevices* call);
    
    void RoutingOnVirtRegSpecAlloc(CallDevices* call);
    
    void RoutingTransponderSpecAlloc(CallDevices* call);
    
    RegeneratorAssignment* GetRegeneratorAssignment() const;

    ProtectionScheme* GetProtectionScheme() const;
    /**
     * @brief Set the container that indicate the RSA order (R-SA Minhop, R-SA MinLength
     * SA-R MinSumSlotIndex or SA-R LowHighSlotIndex) for
     * each node pair in the network of the output of the first PDPP (protection) simulation.
     */
    void SetResourceAllocOrderProtectionGA();
    /**
     * @brief Check if this ResourceAlloc will apply R-SA or SA-R, depending
     * on the order vector.
     * @param call Call request.
     * @return 1 or 2 if will apply R-SA Minhop or MinLength or 3 or 4 if will apply SA-R
     * MinSumSlotIndex or LowHighslotIndex.
     */
    int CheckResourceAllocOrderProtection(Call* call);


private:
    /**
     * @brief Function to check the OSNR for a call request with devices.
     * @param call Call to be analise.
     * @return True if the OSNR is acceptable.
     */
    bool CheckOSNR(CallDevices* call);
    void CreateRegeneratorAssignment();
    void CreateProtectionScheme();
    void CreateRsaOrderProtection();
    
private:    
    /**
     * @brief Pointer to the Regenerator assignment object of this simulation.
     */
    std::shared_ptr<RegeneratorAssignment> regAssAlgorithm;  
    /**
     * @brief Pointer to the protection scheme object of this simulation.
     */
    std::shared_ptr<ProtectionScheme> protScheme;
    
};

#endif /* RESOURCEDEVICEALLOC_H */

