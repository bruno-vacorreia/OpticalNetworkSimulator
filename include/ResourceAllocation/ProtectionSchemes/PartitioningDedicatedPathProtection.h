/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PartitioningDedicatedPathProtection.h
 * Author: henrique
 *
 * Created on August 26, 2020, 4:44 PM
 */

#ifndef PARTITIONINGDEDICATEDPATHPROTECTION_H
#define PARTITIONINGDEDICATEDPATHPROTECTION_H

class CallDevices;

#include "ProtectionScheme.h"


class PartitioningDedicatedPathProtection : public ProtectionScheme {
public:
        
    PartitioningDedicatedPathProtection(ResourceDeviceAlloc* rsa);
    virtual ~PartitioningDedicatedPathProtection();
    
      /**
     * @brief Function which creates protection disjoint routes for each 
     * working route defined by routing algorithm and stored in allRoutes vector.
     */
    void CreateProtectionRoutes() override;
      /**
     * @brief Function which create a container of calls for working and backup
     * lightpaths.
     * 
     */
    void CreateProtectionCalls(CallDevices* call) override;
    /**
    * @brief Function which perform RSA for the call works (partitions) according with
    * PDPP scheme defined in options.
    * @param Call vector which contain PDPPs call works (partitions).
    */
    void ResourceAlloc(CallDevices* call) override;
    /**
    * @brief Function which performs RSA considering all groups of disjoint routes ordered
    * according link cost type defined in options and provided by offline DPGR routing
    * (fixed-alternate) according with PDPP scheme.
    * Only if there is not a set of groups of P disjoint routes, a set of P-1 is used
    * (Original PDPP).
    * @param call Call request the function will try to allocate.
    */
    void ResourceAllocPDPP(CallDevices* call);
    /**
    * @brief Function which performs RSA considering all groups of disjoint routes ordered
    * according link cost type defined in options, provided by offline DPGR routing
    * (fixed-alternate) according with PDPP scheme.
    * If there is not a set of groups of P disjoint routes or or the allocation was not
    * possible in one of them, a set of P-1 is used (Multi-P concept).
    * @param call Call request the function will try to allocate.
    */
    void ResourceAllocPDPP_MultiP(CallDevices* call);
    /**
    * @brief Function which performs RSA considering all groups of disjoint routes ordered
    * by sum of hops of its disjoint routes, provided by offline DPGR routing
    * (fixed-alternate) according with PDPP scheme.
    * If there is not a set of groups of P disjoint routes or or the allocation was not
     * possible in one of them, a set of P-1 is used (Multi-P concept).
    * @param call Call request the function will try to allocate.
    */
    void ResourceAllocPDPP_MultiP_MinHop(CallDevices* call);
    /**
    * @brief Function which performs RSA considering all groups of disjoint routes ordered
    * by sum of length of its disjoint routes, provided by offline DPGR routing
    * (fixed-alternate) according with PDPP scheme.
    * If there is not a set of groups of P disjoint routes or or the allocation was not
     * possible in one of them, a set of P-1 is used (Multi-P concept).
    * @param call Call request the function will try to allocate.
    */
    void ResourceAllocPDPP_MultiP_MinLength(CallDevices* call);
    /**
    * @brief Function which performs RSA for multipath protected Calls by PDPP scheme using
    * offline DPGR routing. This function analyses each group of link-disfoint paths and
    * allocate the partitions in the one which demands a minimum number of slots.
    * @param call Call request the function will try to allocate.
    */
    void ResourceAllocPDPP_MultiP_MinNumSlot(CallDevices* call);




    /**
     * @brief Function which performs Routing-Spectrum RSA ordering for all Partitioned
     * routes according with PDPP scheme for offline YEN routing and no same set of slots
     * between routes.
     * @param call Call request the function will try to allocate.
     */
    void RoutingSpecPDPP(CallDevices* call);
    /**
    * @brief Function which performs Routing-Spectrum RSA ordering for all groups
    * of disjoint routes provided by offline DPGR routing (fixed-alternate) according
    * with PDPP scheme. If the allocation has not succed using a set of groups of P
    * disjoint routes, a set of P-1 is not tryed (Original PDPP).
    * @param call Call request the function will try to allocate.
    */
    void RoutingSpecPDPP_DPGR(CallDevices* call);
    /**
    * @brief Function which performs Routing-Spectrum RSA ordering for all groups
     * of disjoint routes provided by offline DPGR routing (fixed-alternate) according
     * with PDPP scheme. If the allocation has not succed using a set of groups of P
     * disjoint routes, a set of P-1 is tryed (Multi-P).
    * @param call Call request the function will try to allocate.
    */
    void RoutingSpecPDPP_DPGR_MultiP(CallDevices* call);
    /**
    * @brief Function which performs Spectrum-Routing RSA ordering for all Partitioned
    * routes according with PDPP scheme for offline YEN routing and no same set of slots
     * between routes route.
    * @param call Call request the function will try to allocate.
    */
    void SpecRoutingPDPP(CallDevices* call);
    /**
    * @brief Function which performs Spectrum-Routing RSA ordering for all Partitioned
    * routes according with PDPP scheme for offline DPGR routing (fixed-alternate groups
     * of disjoint routes) and no same set of slots between routes.
    * @param call Call request the function will try to allocate.
    */
    void SpecRoutingPDPP_DPGR(CallDevices* call);
    /**
    * @brief Function which performs Spectrum-Routing RSA ordering for all Partitioned
    * routes according with PDPP scheme for offline routing and same set of slots
     * between routes route.
    * @param call Call request the function will try to allocate.
    */
    void SpecRoutingSameSlotPDPP(CallDevices* call);
    /**
    * @brief Function which performs RSA for multipath protected Calls by PDPP scheme using
    * offline DPGR routing. This function analyses each group of link-disfoint paths and
    * allocate the partitions in the one which demands a minimum number of slots.
    * @param call Call request the function will try to allocate.
    */
    void ResourceAllocProtectionPDPP_MinNumSlot(CallDevices* call);
    /**
    * @brief Function which performs RSA for all Partitioned routes according with PDPP
     * scheme for fixed-alternate groups of disjoint routes routing (DPGR) which the groups
     * are ordered by minimal sum of its lower slot index available to allocate each
     * partition in each route.
    * @param call Call request the function will try to allocate.
    */
    void ResourceAllocProtectionPDPP_MinSumSlotIndex(CallDevices* call);
    /**
    * @brief Function which performs RSA for all Partitioned routes according with PDPP
    * scheme for fixed-alternate groups of disjoint routes routing (DPGR) which the groups
    * are ordered by lower slot index among the maximum slot indexes available to allocate
     * each partition in each route..
    * @param call Call request the function will try to allocate.
    */
    void ResourceAllocProtectionPDPP_MinMaxSlotIndex(CallDevices* call);
    /**
     * @brief Function which compute a partial bit rate distribution for all 
      * source-destination pair and for all incoming traffic demand possibilities.
     */
    void LoadPDPPBitRateNodePairDist();
    /**
     * @brief Function which calculates the partial bit rate for each 
     * partitioned routes based in incoming traffic demand. A vector 
     * PDPPBitRateDistOptions is loaded with equally distribution between routes. 
     */
    void LoadPDPPBitRateOptions();
    /**
     * @brief Function which create a partial bit rate distribution according on scheme
     * (equally for PDPP and not equally for GA_PDPPBO) for each 
     * partitioned routes based in incoming traffic demand. A vector 
     * PDPPBitRateNodePairDist is loaded. 
     */
    void CreatePDPPBitRateOptions();
    /**
     * @brief Function which get the PDPPBitRateNodePairDist container. 
     */
    std::vector<std::vector<std::vector<double>>> GetPDPPBitRateNodePairsDist() const;
    /**
     * @brief Function which set the PDPPBitRateNodePairDist container. 
     */
    void SetPDPPBitRateNodePairsDist(std::vector<std::vector<std::vector<double>>>
    PDPPBitRateNodePairsDist);
    /**
    * @brief Set the container that indicate the PDPP Bit rate distribution option for
    * each node pair in the network of the output of the first simulation.
    */
    void SetPDPPBitRateNodePairDistGA();
    /**
    * @brief Function which performs RSA for all PDPP partitions according with the
    * number of routes defined in parameters.
    *  * @param call Call request, .
    */
    void RSA_ProtectionCalls(CallDevices *call, std::vector<std::shared_ptr<Route>> group,
                             std::shared_ptr<Call> firstPartition);
      
private:
    /**
     * @brief Vector to store the partial bit rate distribution options for each
     * possible incoming traffic demand. The 1ª dimension is the incoming traffic
     * demands possibilities and the 2ª dimension is the bit rate distribution
     * options for the protection routes.
     */
    std::vector<std::vector<double>> PDPPBitRateDistOptions;
     /**
     * @brief Vector to store the partial bit rate distribution for all 
     * source-destination pair. The 1ª dimension is the node pair index, the 
     * 2ª dimension is the traffic demands possibilities and 3ª dimension is the 
     * bit rate distribution defined for the protection routes.
     */
    std::vector<std::vector<std::vector<double>>> PDPPBitRateNodePairsDist;
         
};

#endif /* PARTITIONINGDEDICATEDPATHPROTECTION_H */

