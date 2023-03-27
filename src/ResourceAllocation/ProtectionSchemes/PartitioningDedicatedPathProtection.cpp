/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DedicatedPathProtection.cpp
 * Author: henrique
 * 
 * Created on October 26, 2019, 9:36 AM
 */

#include <vector>
#include <memory>
#include <tuple>

#include "../../../include/ResourceAllocation/ProtectionSchemes/PartitioningDedicatedPathProtection.h"
#include "../../../include/ResourceAllocation/SA.h"
#include "../../../include/ResourceAllocation/ResourceAlloc.h"
#include "../../../include/ResourceAllocation/Routing.h"
#include "../../../include/ResourceAllocation/Modulation.h"
#include "../../../include/Calls/Call.h"
#include "../../../include/Data/Parameters.h"
#include "../../../include/Calls/Traffic.h"
#include "../../../include/SimulationType/SimulationType.h"
#include "../../../include/Data/Data.h"
#include "../../../include/Data/InputOutput.h"
#include "../../../include/GeneralClasses/Def.h"
#include "math.h" 

PartitioningDedicatedPathProtection::PartitioningDedicatedPathProtection
(ResourceDeviceAlloc* rsa): ProtectionScheme(rsa),PDPPBitRateDistOptions(0),
PDPPBitRateNodePairsDist(0) {

}

PartitioningDedicatedPathProtection::~PartitioningDedicatedPathProtection() {

}

void PartitioningDedicatedPathProtection::CreateProtectionRoutes() {

    if(resDevAlloc->options->GetProtectionOption() == ProtectionPDPP || ProtectionPDPP_MultiP) {
        switch (this->routing->GetRoutingOption()) {
            case RoutingYEN:
                routing->ProtectionDisjointYEN();
                break;
            case RoutingDPGR:
                this->routing->DisjointPathGroupsRouting();
                break;
            default:
                std::cerr << "Invalid offline routing option" << std::endl;
                std::abort();
        }
    }
    else {
        switch (resDevAlloc->options->GetProtectionOption()) {
            case ProtectionPDPP_LowHighSlotIndex:
            case ProtectionPDPP_MinSumSlotIndex:
            case ProtectionPDPP_MinNumSlot:
            case ProtectionPDPP_MinHop:
            case ProtectionPDPP_MinLength:
            case ProtectionOPDPP_GA:
                this->routing->DisjointPathGroupsRouting();
                break;
            default:
                std::cerr << "Invalid offline routing option" << std::endl;
                std::abort();
        }
    }

    this->CreatePDPPBitRateOptions();
}

void PartitioningDedicatedPathProtection::CreatePDPPBitRateOptions() {
    assert(parameters->GetNumberPDPPprotectionRoutes() >= 2 &&
           parameters->GetNumberPDPPprotectionRoutes() <= 4);
    numSchProtRoutes = parameters->GetNumberPDPPprotectionRoutes();

    switch(resDevAlloc->options->GetProtectionOption()){
        case ProtectionPDPP_MinHop:
        case ProtectionPDPP_MinLength:
        case ProtectionPDPP_MinSumSlotIndex:
        case ProtectionPDPP_LowHighSlotIndex:
        case ProtectionPDPP_MinNumSlot:
        case ProtectionPDPP_MultiP:
        case ProtectionPDPP:
            LoadPDPPBitRateOptions();
            break;
        case ProtectionOPDPP_GA:
        case ProtectionHPDPP_GA:
            break;
        default:
            std::cerr << "Invalid Protection Option for PDPPBitRate option" << std::endl;
            std::abort();
    }
    LoadPDPPBitRateNodePairDist();
}

void PartitioningDedicatedPathProtection::LoadPDPPBitRateOptions() {
    std::vector<double> VecTraffic = resDevAlloc->traffic->GetVecTraffic();
    std::vector<double> auxBitRateOption;
    double partialBitRate;
    double beta = parameters->GetBeta();

    for(auto it : VecTraffic){
        partialBitRate = ceil (((1 - beta) * it) / (numSchProtRoutes -1));
        for(unsigned int a = 0; a < numSchProtRoutes; a++){
            auxBitRateOption.push_back(partialBitRate);
        }
        PDPPBitRateDistOptions.push_back(auxBitRateOption);
        auxBitRateOption.clear();
    }
}

void PartitioningDedicatedPathProtection::LoadPDPPBitRateNodePairDist() {
    unsigned int NumNodes = topology->GetNumNodes();
    PDPPBitRateNodePairsDist.resize(NumNodes * NumNodes);

    switch(resDevAlloc->options->GetProtectionOption()){
        case ProtectionPDPP_MinHop:
        case ProtectionPDPP_MinLength:
        case ProtectionPDPP_MinSumSlotIndex:
        case ProtectionPDPP_LowHighSlotIndex:
        case ProtectionPDPP_MinNumSlot:
        case ProtectionPDPP_MultiP:
        case ProtectionPDPP:
        case ProtectionHPDPP_GA:
            for(int a = 0; a < PDPPBitRateNodePairsDist.size(); a++){
                PDPPBitRateNodePairsDist.at(a) = PDPPBitRateDistOptions;
            }
            break;
        case ProtectionOPDPP_GA:
            this->SetPDPPBitRateNodePairDistGA();
            break;
        default:
            std::cerr << "Invalid Protection Option for DPPBitRateNodePairDist" << std::endl;
            std::abort();
    }
}

void PartitioningDedicatedPathProtection::ResourceAlloc(CallDevices* call) {

/*    if(resDevAlloc->options->GetProtectionOption() == ProtectionOPDPP_GA) {
        switch (this->routing->GetRoutingOption()) {
            case RoutingYEN:
                if (resDevAlloc->CheckResourceAllocOrder(call) == r_sa)
                    this->RoutingSpecPDPP(call);
                else
                    this->SpecRoutingPDPP(call);
                break;
            case RoutingDPGR:
                if (resDevAlloc->CheckResourceAllocOrder(call) == r_sa)
                    if (resDevAlloc->options->GetGaOption() == GaPDPPBO ||
                        resDevAlloc->options->GetProtectionOption() == ProtectionOPDPP_GA)
                        //this->ResourceAllocProtectionPDPP_MinNumSlot(call);
                        this->ResourceAllocPDPP_MultiP(call);
                    else
                        this->ResourceAllocPDPP_MultiP(call);
                else
                    this->SpecRoutingPDPP_DPGR(call);
                break;
            default:
                std::cerr << "Invalid offline routing option" << std::endl;
                std::abort();
        }
    }
    else if(resDevAlloc->options->GetProtectionOption() == ProtectionPDPP ) {
        switch (this->routing->GetRoutingOption()) {
            case RoutingYEN:
                if (resDevAlloc->CheckResourceAllocOrder(call) == r_sa)
                    this->RoutingSpecPDPP(call);
                else
                    this->SpecRoutingPDPP(call);
                break;
            case RoutingDPGR:
                if (resDevAlloc->CheckResourceAllocOrder(call) == r_sa)
                    if (resDevAlloc->options->GetGaOption() == GaPDPPBO ||
                        resDevAlloc->options->GetProtectionOption() == ProtectionOPDPP_GA)
                        //this->ResourceAllocProtectionPDPP_MinNumSlot(call);
                        this->ResourceAllocPDPP_MultiP(call);
                    else
                        this->ResourceAllocPDPP(call);
                        //this->RoutingSpecPDPP_DPGR(call);
                else
                    this->SpecRoutingPDPP_DPGR(call);
                break;
            default:
                std::cerr << "Invalid offline routing option" << std::endl;
                std::abort();
        }
    }
    else{
        switch (resDevAlloc->options->GetProtectionOption()) {
            case ProtectionPDPP_MultiP:
                this->ResourceAllocPDPP_MultiP(call);
                break;
            case ProtectionPDPP_MinHop:
                this->ResourceAllocPDPP_MultiP_MinHop(call);
                break;
            case ProtectionPDPP_MinLength:
                this->ResourceAllocPDPP_MultiP_MinLength(call);
                break;
            case ProtectionPDPP_MinSumSlotIndex:
                this->ResourceAllocPDPP_MinSumSlotIndexes(call);
                break;
            case ProtectionPDPP_LowHighSlotIndex:
                this->ResourceAllocPDPP_LowHighSlotIndex(call);
                break;
            case ProtectionPDPP_MinNumSlot:
                //this->ResourceAllocProtectionPDPP_MinNumSlot(call);
                this->ResourceAllocPDPP_MinNumSlot(call);
                break;
            default:
                std::cerr << "Invalid Protection option" << std::endl;
                std::abort();
        }
    }*/

    if (resDevAlloc->options->GetGaOption() == GaHPDPP ||
    resDevAlloc->options->GetGaOption() == GaPDPPBO ||
    resDevAlloc->options->GetProtectionOption() == ProtectionHPDPP_GA) {
        switch (resDevAlloc->CheckResourceAllocOrderProtection(call)) {
            case r_sa_MinHop:
                this->ResourceAllocPDPP_MultiP_MinHop(call);
                break;
            case r_sa_MinLength:
                this->ResourceAllocPDPP_MultiP_MinLength(call);
                break;
            case sa_r_MinSumSlotIndex:
                this->ResourceAllocPDPP_MinSumSlotIndexes(call);
                break;
            case sa_r_LowHighSlotIndex:
                this->ResourceAllocPDPP_LowHighSlotIndex(call);
                break;
            default:
                std::cerr << "Invalid Rsa Order Protection option" << std::endl;
                std::abort();
        }
    }
    else if (resDevAlloc->options->GetProtectionOption() != ProtectionDisable){
        switch (resDevAlloc->options->GetProtectionOption()) {
            case ProtectionPDPP:
                this->ResourceAllocPDPP(call);
                break;
            case ProtectionPDPP_MultiP:
            case ProtectionOPDPP_GA:
                this->ResourceAllocPDPP_MultiP(call);
                break;
            case ProtectionPDPP_MinHop:
                this->ResourceAllocPDPP_MultiP_MinHop(call);
                break;
            case ProtectionPDPP_MinLength:
                this->ResourceAllocPDPP_MultiP_MinLength(call);
                break;
            case ProtectionPDPP_MinSumSlotIndex:
                this->ResourceAllocPDPP_MinSumSlotIndexes(call);
                break;
            case ProtectionPDPP_LowHighSlotIndex:
                this->ResourceAllocPDPP_LowHighSlotIndex(call);
                break;
            case ProtectionPDPP_MinNumSlot:
                this->ResourceAllocPDPP_MinNumSlot(call);
                break;
            default:
                std::cerr << "Invalid Protection option" << std::endl;
                std::abort();
        }
    }
}

void PartitioningDedicatedPathProtection::ResourceAllocPDPP(CallDevices *call) {
    this->CreateProtectionCalls(call); //loading multiCall vector with protection calls

    unsigned int orN = call->GetOrNode()->GetNodeId();
    unsigned int deN = call->GetDeNode()->GetNodeId();
    unsigned int numNodes = this->topology->GetNumNodes();
    unsigned int nodePairIndex = orN * numNodes + deN;
    double callBitRate = call->GetBitRate();
    double beta = parameters->GetBeta();
    double partialBitRate = 0;

    if(numSchProtRoutes == 4) {
        //setting 4 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);
        std::shared_ptr<Call> callWork3 = callsVec.at(3);

        //trying to allocate with 4 routes
        if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
        .empty()) {
            for (auto &group4: resources->protectionAllRoutesGroups.at(
            nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                this->RSA_ProtectionCalls(call, group4, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
        else if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
        .empty()) {
            //Delete one call, recalculate Bit rate and try allocating with 3 routes
            callsVec.pop_back();
            partialBitRate = ceil(((1 - beta) * callBitRate) / (numSchProtRoutes - 2));
            callWork0->SetBitRate(partialBitRate);
            callWork1->SetBitRate(partialBitRate);
            callWork2->SetBitRate(partialBitRate);
            call->SetMultiCallVec(callsVec);
            for (auto &group3: resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
        else if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-4)
        .empty()) {
            //Delete two routes, recalculate Bit rate and try allocating with 2 routes
            for(int a = 0; a < numSchProtRoutes-2; a++){
                callsVec.pop_back();
            }
            partialBitRate = ceil(((1 - beta) * callBitRate) / (numSchProtRoutes - 3));
            callWork0->SetBitRate(partialBitRate);
            callWork1->SetBitRate(partialBitRate);
            call->SetMultiCallVec(callsVec);

            for (auto &group2: resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-4)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        } */
    }

    if(numSchProtRoutes == 3){
        //setting 3 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        //trying to allocate with 3 routes
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2).empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
        else if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
        .empty()){
            //Delete one partition, recalculate Bit rate and try allocating with 2 routes
            callsVec.pop_back();
            partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
            callWork0->SetBitRate(partialBitRate);
            callWork1->SetBitRate(partialBitRate);
            call->SetMultiCallVec(callsVec);

            for(auto& group2 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        }*/
    }

    if(numSchProtRoutes == 2){
        //setting 2 calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        //trying to allocate with 2 routes
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
        .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        } */
    }
}

void PartitioningDedicatedPathProtection::ResourceAllocPDPP_MultiP(CallDevices *call) {
    this->CreateProtectionCalls(call); //loading multiCall vector with protection calls

    unsigned int orN = call->GetOrNode()->GetNodeId();
    unsigned int deN = call->GetDeNode()->GetNodeId();
    unsigned int numNodes = this->topology->GetNumNodes();
    unsigned int nodePairIndex = orN * numNodes + deN;
    double callBitRate = call->GetBitRate();
    double beta = parameters->GetBeta();
    double partialBitRate = 0;

    if(numSchProtRoutes == 4) {
        //setting 4 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);
        std::shared_ptr<Call> callWork3 = callsVec.at(3);

        //trying to allocate with 4 routes
        if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
                .empty()) {
            for (auto &group4: resources->protectionAllRoutesGroups.at(
                    nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                this->RSA_ProtectionCalls(call, group4, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
        //Delete one call, recalculate Bit rate and try allocating with 3 routes
        callsVec.pop_back();
        partialBitRate = ceil(((1 - beta) * callBitRate) / (numSchProtRoutes - 2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        callWork2->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);
        if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
                .empty()) {
            for (auto &group3: resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes - 3)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if (call->GetStatus() == Accepted)
                    return;
            }
        }
        //Delete one call, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil(((1 - beta) * callBitRate) / (numSchProtRoutes - 3));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);
        if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-4)
                .empty()) {
            for (auto &group2: resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-4)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        }*/
    }

    if(numSchProtRoutes == 3){
        //setting 3 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        //trying to allocate with 3 routes
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
        .empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
        //Delete one partition, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
        .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        }*/
    }

    if(numSchProtRoutes == 2){
        //setting 2 calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        //trying to allocate with 2 routes
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
        .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        }*/
    }
}

void PartitioningDedicatedPathProtection::ResourceAllocPDPP_MultiP_MinHop(CallDevices *call) {
    this->CreateProtectionCalls(call); //loading multiCall vector with protection calls

    unsigned int orN = call->GetOrNode()->GetNodeId();
    unsigned int deN = call->GetDeNode()->GetNodeId();
    unsigned int numNodes = this->topology->GetNumNodes();
    unsigned int nodePairIndex = orN * numNodes + deN;
    double callBitRate = call->GetBitRate();
    double beta = parameters->GetBeta();
    double partialBitRate = 0;

    if(numSchProtRoutes == 4) {
        //setting 4 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);
        std::shared_ptr<Call> callWork3 = callsVec.at(3);

        //trying to allocate with 4 routes
        if (!resources->protectionAllRoutesGroupsHops.at(nodePairIndex).at(numSchProtRoutes-2)
                .empty()) {
            for (auto &group4: resources->protectionAllRoutesGroupsHops.at(
                    nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                this->RSA_ProtectionCalls(call, group4, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
        //Delete one call, recalculate Bit rate and try allocating with 3 routes
        callsVec.pop_back();
        partialBitRate = ceil(((1 - beta) * callBitRate) / (numSchProtRoutes - 2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        callWork2->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);
        if (!resources->protectionAllRoutesGroupsHops.at(nodePairIndex).at(numSchProtRoutes-3)
                .empty()) {
            for (auto &group3: resources->protectionAllRoutesGroupsHops
                    .at(nodePairIndex).at(numSchProtRoutes - 3)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if (call->GetStatus() == Accepted)
                    return;
            }
        }
        //Delete one call, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil(((1 - beta) * callBitRate) / (numSchProtRoutes - 3));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);
        if (!resources->protectionAllRoutesGroupsHops.at(nodePairIndex).at(numSchProtRoutes-4)
                .empty()) {
            for (auto &group2: resources->protectionAllRoutesGroupsHops
                    .at(nodePairIndex).at(numSchProtRoutes-4)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        }*/
    }

    if(numSchProtRoutes == 3){
        //setting 3 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        //trying to allocate with 3 routes
        if(!resources->protectionAllRoutesGroupsHops.at(nodePairIndex).at(numSchProtRoutes-2)
                .empty()){
            for(auto& group3 : resources->protectionAllRoutesGroupsHops
                    .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
        //Delete one partition, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        if(!resources->protectionAllRoutesGroupsHops.at(nodePairIndex).at(numSchProtRoutes-3)
                .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroupsHops
                    .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        }*/
    }

    if(numSchProtRoutes == 2){
        //setting 2 calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        //trying to allocate with 2 routes
        if(!resources->protectionAllRoutesGroupsHops.at(nodePairIndex).at(numSchProtRoutes-2)
                .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroupsHops
                    .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        }*/
    }
}

void PartitioningDedicatedPathProtection::ResourceAllocPDPP_MultiP_MinLength(CallDevices *call) {
    this->CreateProtectionCalls(call); //loading multiCall vector with protection calls

    unsigned int orN = call->GetOrNode()->GetNodeId();
    unsigned int deN = call->GetDeNode()->GetNodeId();
    unsigned int numNodes = this->topology->GetNumNodes();
    unsigned int nodePairIndex = orN * numNodes + deN;
    double callBitRate = call->GetBitRate();
    double beta = parameters->GetBeta();
    double partialBitRate = 0;

    if(numSchProtRoutes == 4) {
        //setting 4 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);
        std::shared_ptr<Call> callWork3 = callsVec.at(3);

        //trying to allocate with 4 routes
        if (!resources->protectionAllRoutesGroupsLength.at(nodePairIndex).at(numSchProtRoutes-2)
        .empty()) {
            for (auto &group4: resources->protectionAllRoutesGroupsLength
            .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                this->RSA_ProtectionCalls(call, group4, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
        //Delete one call, recalculate Bit rate and try allocating with 3 routes
        callsVec.pop_back();
        partialBitRate = ceil(((1 - beta) * callBitRate) / (numSchProtRoutes - 2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        callWork2->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);
        if (!resources->protectionAllRoutesGroupsLength.at(nodePairIndex).at(numSchProtRoutes-3)
                .empty()) {
            for (auto &group3: resources->protectionAllRoutesGroupsLength
                    .at(nodePairIndex).at(numSchProtRoutes - 3)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if (call->GetStatus() == Accepted)
                    return;
            }
        }
        //Delete one call, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil(((1 - beta) * callBitRate) / (numSchProtRoutes - 3));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);
        if (!resources->protectionAllRoutesGroupsLength.at(nodePairIndex).at(numSchProtRoutes-4)
                .empty()) {
            for (auto &group2: resources->protectionAllRoutesGroupsLength
                    .at(nodePairIndex).at(numSchProtRoutes-4)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        }*/
    }

    if(numSchProtRoutes == 3){
        //setting 3 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        //trying to allocate with 3 routes
        if(!resources->protectionAllRoutesGroupsLength.at(nodePairIndex).at(numSchProtRoutes-2)
                .empty()){
            for(auto& group3 : resources->protectionAllRoutesGroupsLength
                    .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
        //Delete one partition, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        if(!resources->protectionAllRoutesGroupsLength.at(nodePairIndex).at(numSchProtRoutes-3)
                .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroupsLength
                    .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        }*/
    }

    if(numSchProtRoutes == 2){
        //setting 2 calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        //trying to allocate with 2 routes
        if(!resources->protectionAllRoutesGroupsLength.at(nodePairIndex).at(numSchProtRoutes-2)
                .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroupsLength
                    .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
        }
/*        else {
            //Delete one route, recalculate Bitrate and try allocating without protection
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
                callWork0->SetRoute(route);
                this->modulation->DefineBestModulation(call);
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(route);
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                    return;
                }
            }
        }*/
    }
}

void PartitioningDedicatedPathProtection::ResourceAllocPDPP_MinSumSlotIndexes(CallDevices *call) {
    this->CreateProtectionCalls(call); //loading multiCall vector with protection calls
    resDevAlloc->options->SetSpecAllOption(SpecAllFF); //setting FirstFit option

    unsigned int orN = call->GetOrNode()->GetNodeId();
    unsigned int deN = call->GetDeNode()->GetNodeId();
    unsigned int numNodes = this->topology->GetNumNodes();
    unsigned int nodePairIndex = orN * numNodes + deN;

    int sumSlotIndexesG = 0; // variable to store the sum of the lowest slot indexes from one group
    std::vector<int> auxSumSlotIndexesGroupsVec; // vector to store sum of lowest slots indexes
    std::vector<std::vector<std::shared_ptr<Route>>> auxRouteGroupsVec;

    double callBitRate = call->GetBitRate();
    double beta = parameters->GetBeta();
    double partialBitRate =0;

    if(numSchProtRoutes == 4){
        //setting 4 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);
        std::shared_ptr<Call> callWork3 = callsVec.at(3);

        //computing the total number of required slots from each group for current call
        if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
                .empty()) {
            for (auto &group4: resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes - 2)) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //summation of the first slot indexes available in each route of current group
                for (const auto &partition: callsVec) {
                    sumSlotIndexesG += partition->GetFirstSlot();
                }
                auxSumSlotIndexesGroupsVec.push_back(sumSlotIndexesG);
                auxRouteGroupsVec.push_back(group4);
                sumSlotIndexesG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxSumSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxSumSlotIndexesGroupsVec[gj] = auxSumSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying tp allocate with 4 routes from ordered vector
            for (auto &group4: auxRouteGroupsVec) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                this->RSA_ProtectionCalls(call, group4, callWork0);
                if (call->GetStatus() == Accepted)
                    return;
            }
            auxSumSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        //Delete one route, recalculate Bit rate and try allocating with 3 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
                .empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //summation of the first slot indexes available in each route of current group
                for (const auto &partition: callsVec) {
                    sumSlotIndexesG += partition->GetFirstSlot();
                }
                auxSumSlotIndexesGroupsVec.push_back(sumSlotIndexesG);
                auxRouteGroupsVec.push_back(group3);
                sumSlotIndexesG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxSumSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxSumSlotIndexesGroupsVec[gj] = auxSumSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 3 routes from ordered vector
            for(auto& group3 : auxRouteGroupsVec) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxSumSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        //Delete one route, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-3));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-4)
                .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-4)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //summation of the first slot indexes available in each route of current group
                for (const auto &partition: callsVec) {
                    sumSlotIndexesG += partition->GetFirstSlot();
                }
                auxSumSlotIndexesGroupsVec.push_back(sumSlotIndexesG);
                auxRouteGroupsVec.push_back(group2);
                sumSlotIndexesG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxSumSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxSumSlotIndexesGroupsVec[gj] = auxSumSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 2 routes from ordered vector
            for(auto& group2 : auxRouteGroupsVec) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxSumSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }

    if(numSchProtRoutes == 3){
        //setting 3 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
        .empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //summation of the first slot indexes available in each route of current group
                for (const auto &partition: callsVec) {
                    sumSlotIndexesG += partition->GetFirstSlot();
                }
                auxSumSlotIndexesGroupsVec.push_back(sumSlotIndexesG);
                auxRouteGroupsVec.push_back(group3);
                sumSlotIndexesG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxSumSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxSumSlotIndexesGroupsVec[gj] = auxSumSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 3 routes from ordered vector
            for(auto& group3 : auxRouteGroupsVec) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxSumSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        //Delete one route, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
        .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //summation of the first slot indexes available in each route of current group
                for (const auto &partition: callsVec) {
                    sumSlotIndexesG += partition->GetFirstSlot();
                }
                auxSumSlotIndexesGroupsVec.push_back(sumSlotIndexesG);
                auxRouteGroupsVec.push_back(group2);
                sumSlotIndexesG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxSumSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxSumSlotIndexesGroupsVec[gj] = auxSumSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 2 routes from oredered vector
            for(auto& group2 : auxRouteGroupsVec) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxSumSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }

    if(numSchProtRoutes == 2){
        //setting 2 calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
        .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //summation of the first slot indexes available in each route of current group
                for (const auto &partition: callsVec) {
                    sumSlotIndexesG += partition->GetFirstSlot();
                }
                auxSumSlotIndexesGroupsVec.push_back(sumSlotIndexesG);
                auxRouteGroupsVec.push_back(group2);
                sumSlotIndexesG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxSumSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxSumSlotIndexesGroupsVec[gj] = auxSumSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 2 routes from ordered vector
            for(auto& group2 : auxRouteGroupsVec) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxSumSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }
}

void PartitioningDedicatedPathProtection::ResourceAllocPDPP_LowHighSlotIndex(CallDevices *call) {
    this->CreateProtectionCalls(call); //loading multiCall vector with protection calls
    resDevAlloc->options->SetSpecAllOption(SpecAllFF); //setting FirstFit option for SA function

    unsigned int orN = call->GetOrNode()->GetNodeId();
    unsigned int deN = call->GetDeNode()->GetNodeId();
    unsigned int numNodes = this->topology->GetNumNodes();
    unsigned int nodePairIndex = orN * numNodes + deN;

    int maxSlotIndexG = 0;
    int highSlotIndexG = 0; // variable to store the highest slot index from one group
    std::vector<int> auxHighSlotIndexesGroupsVec; // vector to store sum the highest slots indexes
    std::vector<std::vector<std::shared_ptr<Route>>> auxRouteGroupsVec;

    double callBitRate = call->GetBitRate();
    double beta = parameters->GetBeta();
    double partialBitRate =0;

    if(numSchProtRoutes == 4){
        //setting 4 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);
        std::shared_ptr<Call> callWork3 = callsVec.at(3);

        //computing the total number of required slots from each group for current call
        if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
                .empty()) {
            for (auto &group4: resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes - 2)) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //obtaining the highest slot index among the routes of current group
                for (const auto &partition: callsVec) {
                    maxSlotIndexG = partition->GetFirstSlot();
                    if(highSlotIndexG < maxSlotIndexG)
                        highSlotIndexG = maxSlotIndexG;
                }
                auxHighSlotIndexesGroupsVec.push_back(highSlotIndexG);
                auxRouteGroupsVec.push_back(group4);
                highSlotIndexG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxHighSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxHighSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxHighSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxHighSlotIndexesGroupsVec[gj] = auxHighSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxHighSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 4 routes from ordered vector
            for (auto &group4: auxRouteGroupsVec) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                this->RSA_ProtectionCalls(call, group4, callWork0);
                if (call->GetStatus() == Accepted)
                    return;
            }
            auxHighSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        //Delete one route, recalculate Bit rate and try allocating with 3 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
                .empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //obtaining the highest slot index among the routes of current group
                for (const auto &partition: callsVec) {
                    maxSlotIndexG = partition->GetFirstSlot();
                    if(highSlotIndexG < maxSlotIndexG)
                        highSlotIndexG = maxSlotIndexG;
                }
                auxHighSlotIndexesGroupsVec.push_back(highSlotIndexG);
                auxRouteGroupsVec.push_back(group3);
                highSlotIndexG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxHighSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxHighSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxHighSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxHighSlotIndexesGroupsVec[gj] = auxHighSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxHighSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 3 routes from ordered vector
            for(auto& group3 : auxRouteGroupsVec) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxHighSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        //Delete one route, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-3));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-4)
                .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-4)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //obtaining the highest slot index among the routes of current group
                for (const auto &partition: callsVec) {
                    maxSlotIndexG = partition->GetFirstSlot();
                    if(highSlotIndexG < maxSlotIndexG)
                        highSlotIndexG = maxSlotIndexG;
                }
                auxHighSlotIndexesGroupsVec.push_back(highSlotIndexG);
                auxRouteGroupsVec.push_back(group2);
                highSlotIndexG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxHighSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxHighSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxHighSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxHighSlotIndexesGroupsVec[gj] = auxHighSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxHighSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 2 routes from ordered vector
            for(auto& group2 : auxRouteGroupsVec) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxHighSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }

    if(numSchProtRoutes == 3){
        //setting 3 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
        .empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //obtaining the highest slot index among the routes of current group
                for (const auto &partition: callsVec) {
                    maxSlotIndexG = partition->GetFirstSlot();
                    if(highSlotIndexG < maxSlotIndexG)
                        highSlotIndexG = maxSlotIndexG;
                }
                auxHighSlotIndexesGroupsVec.push_back(highSlotIndexG);
                auxRouteGroupsVec.push_back(group3);
                highSlotIndexG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxHighSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxHighSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxHighSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxHighSlotIndexesGroupsVec[gj] = auxHighSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxHighSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 3 routes from ordered vector
            for(auto& group3 : auxRouteGroupsVec) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxHighSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        //Delete one route, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
                .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //obtaining the highest slot index among the routes of current group
                for (const auto &partition: callsVec) {
                    maxSlotIndexG = partition->GetFirstSlot();
                    if(highSlotIndexG < maxSlotIndexG)
                        highSlotIndexG = maxSlotIndexG;
                }
                auxHighSlotIndexesGroupsVec.push_back(highSlotIndexG);
                auxRouteGroupsVec.push_back(group2);
                highSlotIndexG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxHighSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxHighSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxHighSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxHighSlotIndexesGroupsVec[gj] = auxHighSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxHighSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 2 routes from ordered vector
            for(auto& group2 : auxRouteGroupsVec) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxHighSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }

    if(numSchProtRoutes == 2){
        //setting 2 calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
                .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //getting the first slot index available in each route
                this->resDevAlloc->specAlloc->SpecAllocation(call);
                //obtaining the highest slot index among the routes of current group
                for (const auto &partition: callsVec) {
                    maxSlotIndexG = partition->GetFirstSlot();
                    if(highSlotIndexG < maxSlotIndexG)
                        highSlotIndexG = maxSlotIndexG;
                }
                auxHighSlotIndexesGroupsVec.push_back(highSlotIndexG);
                auxRouteGroupsVec.push_back(group2);
                highSlotIndexG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxHighSlotIndexesGroupsVec.size(); gi++) {
                int Ci = auxHighSlotIndexesGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxHighSlotIndexesGroupsVec[gj - 1]; gj--) {
                    auxHighSlotIndexesGroupsVec[gj] = auxHighSlotIndexesGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxHighSlotIndexesGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 2 routes from ordered vector
            for(auto& group2 : auxRouteGroupsVec) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxHighSlotIndexesGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }
}

void PartitioningDedicatedPathProtection::ResourceAllocPDPP_MinNumSlot(CallDevices *call) {
    this->CreateProtectionCalls(call); //loading multiCall vector with protection calls

    unsigned int orN = call->GetOrNode()->GetNodeId();
    unsigned int deN = call->GetDeNode()->GetNodeId();
    unsigned int numNodes = this->topology->GetNumNodes();
    unsigned int nodePairIndex = orN * numNodes + deN;

    int sumNumSlotsG = 0; // variable to store the sum of slots demanded from one group
    std::vector<int> auxSumNumSlotsGroupsVec; //vector to store slots demand of each group
    std::vector<std::vector<std::shared_ptr<Route>>> auxRouteGroupsVec;

    double callBitRate = call->GetBitRate();
    double beta = parameters->GetBeta();
    double partialBitRate =0;

    if(numSchProtRoutes == 4){
        //setting 4 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);
        std::shared_ptr<Call> callWork3 = callsVec.at(3);

        //computing the total number of required slots from each group for current call
        if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
        .empty()) {
            for (auto &group4: resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes - 2)) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //summation of number of slots required by the routes
                for (const auto &partition: callsVec) {
                    sumNumSlotsG += partition->GetTotalNumSlots();
                }
                auxSumNumSlotsGroupsVec.push_back(sumNumSlotsG);
                auxRouteGroupsVec.push_back(group4);
                sumNumSlotsG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumNumSlotsGroupsVec.size(); gi++) {
                int Ci = auxSumNumSlotsGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumNumSlotsGroupsVec[gj - 1]; gj--) {
                    auxSumNumSlotsGroupsVec[gj] = auxSumNumSlotsGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumNumSlotsGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 4 routes from ordered vector
            for (auto &group4: auxRouteGroupsVec) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                this->RSA_ProtectionCalls(call, group4, callWork0);
                if (call->GetStatus() == Accepted)
                    return;
            }
            auxSumNumSlotsGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        //Delete one route, recalculate Bit rate and try allocating with 3 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
        .empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //summation of number of slots required by the routes
                for (const auto &partition: callsVec) {
                    sumNumSlotsG += partition->GetTotalNumSlots();
                }
                auxSumNumSlotsGroupsVec.push_back(sumNumSlotsG);
                auxRouteGroupsVec.push_back(group3);
                sumNumSlotsG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumNumSlotsGroupsVec.size(); gi++) {
                int Ci = auxSumNumSlotsGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumNumSlotsGroupsVec[gj - 1]; gj--) {
                    auxSumNumSlotsGroupsVec[gj] = auxSumNumSlotsGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumNumSlotsGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 3 routes from ordered vector
            for(auto& group3 : auxRouteGroupsVec) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxSumNumSlotsGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        //Delete one route, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-3));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-4)
        .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-4)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //summation of number of slots required by the routes
                for (const auto &partition: callsVec) {
                    sumNumSlotsG += partition->GetTotalNumSlots();
                }
                auxSumNumSlotsGroupsVec.push_back(sumNumSlotsG);
                auxRouteGroupsVec.push_back(group2);
                sumNumSlotsG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumNumSlotsGroupsVec.size(); gi++) {
                int Ci = auxSumNumSlotsGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumNumSlotsGroupsVec[gj - 1]; gj--) {
                    auxSumNumSlotsGroupsVec[gj] = auxSumNumSlotsGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumNumSlotsGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 2 routes from ordered vector
            for(auto& group2 : auxRouteGroupsVec) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxSumNumSlotsGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }

    if(numSchProtRoutes == 3){
        //setting 3 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
        .empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //summation of number of slots required by the routes
                for (const auto &partition: callsVec) {
                    sumNumSlotsG += partition->GetTotalNumSlots();
                }
                auxSumNumSlotsGroupsVec.push_back(sumNumSlotsG);
                auxRouteGroupsVec.push_back(group3);
                sumNumSlotsG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumNumSlotsGroupsVec.size(); gi++) {
                int Ci = auxSumNumSlotsGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumNumSlotsGroupsVec[gj - 1]; gj--) {
                    auxSumNumSlotsGroupsVec[gj] = auxSumNumSlotsGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumNumSlotsGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 3 routes from ordered vector
            for(auto& group3 : auxRouteGroupsVec) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                this->RSA_ProtectionCalls(call, group3, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxSumNumSlotsGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        //Delete one route, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
        .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and getting num of slots required by the routes
                this->modulation->DefineBestModulation(call);
                //summation of number of slots required by the routes
                for (const auto &partition: callsVec) {
                    sumNumSlotsG += partition->GetTotalNumSlots();
                }
                auxSumNumSlotsGroupsVec.push_back(sumNumSlotsG);
                auxRouteGroupsVec.push_back(group2);
                sumNumSlotsG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumNumSlotsGroupsVec.size(); gi++) {
                int Ci = auxSumNumSlotsGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumNumSlotsGroupsVec[gj - 1]; gj--) {
                    auxSumNumSlotsGroupsVec[gj] = auxSumNumSlotsGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumNumSlotsGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 2 routes from ordered vector
            for(auto& group2 : auxRouteGroupsVec) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxSumNumSlotsGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }

    if(numSchProtRoutes == 2){
        //setting 2 calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
        .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
            .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 3 routes
                for (const auto &partition: callsVec) {
                    sumNumSlotsG += partition->GetTotalNumSlots();
                }
                auxSumNumSlotsGroupsVec.push_back(sumNumSlotsG);
                auxRouteGroupsVec.push_back(group2);
                sumNumSlotsG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxSumNumSlotsGroupsVec.size(); gi++) {
                int Ci = auxSumNumSlotsGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxSumNumSlotsGroupsVec[gj - 1]; gj--) {
                    auxSumNumSlotsGroupsVec[gj] = auxSumNumSlotsGroupsVec[gj - 1];
                    auxRouteGroupsVec[gj] = auxRouteGroupsVec[gj - 1];
                }
                auxSumNumSlotsGroupsVec[gj] = Ci;
                auxRouteGroupsVec[gj] = Ri;
            }
            //trying to allocate with 2 routes from ordered vector
            for(auto& group2 : auxRouteGroupsVec) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                this->RSA_ProtectionCalls(call, group2, callWork0);
                if(call->GetStatus() == Accepted)
                    return;
            }
            auxSumNumSlotsGroupsVec.clear();
            auxRouteGroupsVec.clear();
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }
}




void PartitioningDedicatedPathProtection::RoutingSpecPDPP(CallDevices* call) {
    if(numSchProtRoutes == 3){
        this->routing->RoutingCall(call); //loading trialRoutes and trialprotRoutes
        unsigned int numRoutes = call->GetNumRoutes();

        this->CreateProtectionCalls(call); //loading multiCall with protection calls

        //seting 3 protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        //try allocating with 3 routes
        for(unsigned int k = 0; k < numRoutes; k++){
            callWork0->SetRoute(call->GetRoute(k));
            callWork0->SetModulation(FixedModulation);
            std::deque<std::shared_ptr<Route>> ProtRoutes = call->GetProtRoutes(k);

            ProtRoutes.erase(std::remove(std::begin(ProtRoutes), std::end(ProtRoutes), nullptr),
                             std::end(ProtRoutes));
            unsigned int sizeProtRoutes = ProtRoutes.size();

            if(sizeProtRoutes >= 2){  //if to skip case which it is no routes enough
                for(unsigned int kd0 = 0; kd0 < sizeProtRoutes; kd0++) {

                    if(call->GetProtRoute(k , kd0)){  //if to avoid null route pointer
                        callWork1->SetRoute(call->GetProtRoute(k, kd0));
                        callWork1->SetModulation(FixedModulation);    

                        for(unsigned int kd1 = 0; kd1 < sizeProtRoutes; kd1++) {

                            if(kd0 == kd1)
                                continue;
                            callWork2->SetRoute(call->GetProtRoute(k, kd1));
                            callWork2->SetModulation(FixedModulation);

                            //calculate number of slots for the vector of calls
                            this->modulation->SetModulationParam(call);
                            //check if the number of slots are available in the 3 routes
                            this->resDevAlloc->specAlloc->SpecAllocation(call);

                            if(topology->IsValidLigthPath(call)){
                                call->SetRoute(call->GetRoute(k));
                                call->SetModulation(FixedModulation);
                                call->SetFirstSlot(callWork0->GetFirstSlot());
                                call->SetLastSlot(callWork0->GetLastSlot());
                                call->ClearTrialRoutes();
                                call->ClearTrialProtRoutes();
                                call->SetStatus(Accepted);
                                resDevAlloc->simulType->GetData()->SetProtectedCalls();
                                CalcBetaAverage(call);
                                CalcAlpha(call);
                                return;           
                            }
                        }
                    }
                }
            }
        }

        //Delete one route, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        double callBitRate = call->GetBitRate();
        double beta = parameters->GetBeta();
        double partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);
        
        for(unsigned int k = 0; k < numRoutes; k++){
        callWork0->SetRoute(call->GetRoute(k));
        callWork0->SetModulation(FixedModulation);
        unsigned int sizeProtRoutes = call->GetProtRoutes(k).size();
        
            for(unsigned int kd = 0; kd < sizeProtRoutes; kd++) {

                if(call->GetProtRoute(k , kd)){  //if to avoid null route pointer
                    callWork1->SetRoute(call->GetProtRoute(k, kd));
                    callWork1->SetModulation(FixedModulation);

                    //calculate number of slots for the vector of calls (multiCall)
                    this->modulation->SetModulationParam(call);

                    this->resDevAlloc->specAlloc->SpecAllocation(call);

                    if(topology->IsValidLigthPath(call)){
                        call->SetRoute(k);
                        call->SetModulation(FixedModulation);
                        call->SetFirstSlot(callWork0->GetFirstSlot());
                        call->SetLastSlot(callWork0->GetLastSlot());
                        call->ClearTrialRoutes();
                        call->ClearTrialProtRoutes();
                        call->SetStatus(Accepted);
                        resDevAlloc->simulType->GetData()->SetProtectedCalls();
                        CalcBetaAverage(call);
                        CalcAlpha(call);
                        return;
                    }
                }
            }
        }
        /*//Delete one route again, recalculate Bit rate and try allocating just 1
        //route (without protection)
        callsVec.pop_back();
        callWork0->SetBitRate(call->GetBitRate());
        call->SetMultiCallVec(callsVec);
     
        for(unsigned int k = 0; k < numRoutes; k++){
            callWork0->SetRoute(call->GetRoute(k));
            callWork0->SetModulation(FixedModulation);
            this->modulation->SetModulationParam(call);
            this->resDevAlloc->specAlloc->SpecAllocation(call);

            if(topology->IsValidLigthPath(call)){
                call->SetRoute(call->GetRoute(k));
                call->SetModulation(FixedModulation);
                call->SetFirstSlot(callWork0->GetFirstSlot());
                call->SetLastSlot(callWork0->GetLastSlot());
                call->ClearTrialRoutes();
                call->ClearTrialProtRoutes();
                call->SetStatus(Accepted);
                resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                return;
            }
        }*/
    }
    
    if(numSchProtRoutes == 2){
        this->routing->RoutingCall(call); //loading trialRoutes and trialprotRoutes
        unsigned int numRoutes = call->GetNumRoutes();

        this->CreateProtectionCalls(call); //loading multiCall with calls

        //setting 2 protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        //try allocating with 2 routes
        for(unsigned int k = 0; k < numRoutes; k++){
        callWork0->SetRoute(call->GetRoute(k));
        callWork0->SetModulation(FixedModulation);
        unsigned int sizeProtRoutes = call->GetProtRoutes(k).size();
        
            for(unsigned int kd = 0; kd < sizeProtRoutes; kd++) {

                if(call->GetProtRoute(k , kd)){  //if to avoid null route pointer
                    callWork1->SetRoute(call->GetProtRoute(k, kd));
                    callWork1->SetModulation(FixedModulation);

                    //calculate number of slots for the vector of calls (multiCall)
                    this->modulation->SetModulationParam(call);

                    this->resDevAlloc->specAlloc->SpecAllocation(call);

                    if(topology->IsValidLigthPath(call)){
                        call->SetRoute(k);
                        call->SetModulation(FixedModulation);
                        call->SetFirstSlot(callWork0->GetFirstSlot());
                        call->SetLastSlot(callWork0->GetLastSlot());
                        call->ClearTrialRoutes();
                        call->ClearTrialProtRoutes();
                        call->SetStatus(Accepted);
                        resDevAlloc->simulType->GetData()->SetProtectedCalls();
                        CalcBetaAverage(call);
                        CalcAlpha(call);
                        return;
                    }
                }
            }
        }
        /*//Delete one route and try allocating just 1 route (without protection)
        callsVec.pop_back();
        callWork0->SetBitRate(call->GetBitRate());
        call->SetMultiCallVec(callsVec);
     
        for(unsigned int k = 0; k < numRoutes; k++){
            callWork0->SetRoute(call->GetRoute(k));
            callWork0->SetModulation(FixedModulation);
            this->modulation->SetModulationParam(call);
            this->resDevAlloc->specAlloc->SpecAllocation(call);

            if(topology->IsValidLigthPath(call)){
                call->SetRoute(call->GetRoute(k));
                call->SetModulation(FixedModulation);
                call->SetFirstSlot(callWork0->GetFirstSlot());
                call->SetLastSlot(callWork0->GetLastSlot());
                call->ClearTrialRoutes();
                call->ClearTrialProtRoutes();
                call->SetStatus(Accepted);
                resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                return;
            }
        }*/
    }
}

void PartitioningDedicatedPathProtection::RoutingSpecPDPP_DPGR(CallDevices *call) {

    if(numSchProtRoutes == 4) {
        this->CreateProtectionCalls(call); //loading multiCall vector with protection calls

        //setting 4 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);
        std::shared_ptr<Call> callWork3 = callsVec.at(3);

        unsigned int orN = call->GetOrNode()->GetNodeId();
        unsigned int deN = call->GetDeNode()->GetNodeId();
        unsigned int numNodes = this->topology->GetNumNodes();
        unsigned int nodePairIndex = orN * numNodes + deN;
        double callBitRate = call->GetBitRate();
        double beta = parameters->GetBeta();
        double partialBitRate = 0;

        //trying to allocate with 4 routes
        if (!resources->protectionAllRoutesGroups.at(nodePairIndex).front().empty()) {
            for (auto &group4: resources->protectionAllRoutesGroups.at(
                    nodePairIndex).front()) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 4 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group4.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        } else if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(
                1).empty()) {
            //Delete one route, recalculate Bit rate and try allocating with 3 routes
            callsVec.pop_back();
            partialBitRate = ceil(((1 - beta) * callBitRate) / (numSchProtRoutes - 2));
            callWork0->SetBitRate(partialBitRate);
            callWork1->SetBitRate(partialBitRate);
            callWork2->SetBitRate(partialBitRate);
            call->SetMultiCallVec(callsVec);
            for (auto &group3: resources->protectionAllRoutesGroups.at(nodePairIndex).at(
                    1)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 3 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group3.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }
        else if (!resources->protectionAllRoutesGroups.at(nodePairIndex).back().empty()) {
            //Delete one route, recalculate Bit rate and try allocating with 2 routes
            callsVec.pop_back();
            partialBitRate = ceil(((1 - beta) * callBitRate) / (numSchProtRoutes - 3));
            callWork0->SetBitRate(partialBitRate);
            callWork1->SetBitRate(partialBitRate);
            call->SetMultiCallVec(callsVec);

            for (auto &group2: resources->protectionAllRoutesGroups.at(
                    nodePairIndex).back()) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 2 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group2.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }

//        else {
//            //Delete one route again, recalculate Bit rate and try allocating just 1 route
//            // (without protection)
//            callsVec.pop_back();
//            callWork0->SetBitRate(call->GetBitRate());
//            call->SetMultiCallVec(callsVec);
//
//            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
//                callWork0->SetRoute(route);
//                //callWork0->SetModulation(FixedModulation);
//                //this->modulation->SetModulationParam(call);
//                this->modulation->DefineBestModulation(call);
//                this->resDevAlloc->specAlloc->SpecAllocation(call);
//
//                if (topology->IsValidLigthPath(call)) {
//                    call->SetRoute(route);
//                    call->SetModulation(callWork0->GetModulation());
//                    call->SetFirstSlot(callWork0->GetFirstSlot());
//                    call->SetLastSlot(callWork0->GetLastSlot());
//                    call->SetStatus(Accepted);
//                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
//                    return;
//                }
//            }
//        }
    }

    if(numSchProtRoutes == 3){
        this->CreateProtectionCalls(call); //loading multiCall vector with protection calls

        //setting 3 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        unsigned int orN = call->GetOrNode()->GetNodeId();
        unsigned int deN = call->GetDeNode()->GetNodeId();
        unsigned int numNodes = this->topology->GetNumNodes();
        unsigned int nodePairIndex = orN * numNodes + deN;

        //trying to allocate with 3 routes
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(1).empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups.at(nodePairIndex).at(1)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 3 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group3.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }
        else if(!resources->protectionAllRoutesGroups.at(nodePairIndex).back().empty()){
            //Delete one partition, recalculate Bit rate and try allocating with 2 routes
            callsVec.pop_back();
            double callBitRate = call->GetBitRate();
            double beta = parameters->GetBeta();
            double partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
            callWork0->SetBitRate(partialBitRate);
            callWork1->SetBitRate(partialBitRate);
            call->SetMultiCallVec(callsVec);
            for(auto& group2 : resources->protectionAllRoutesGroups.at(nodePairIndex).back()) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 2 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group2.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }
//        else {
//            //Delete one route again, recalculate Bit rate and try allocating just 1
//            //route (without protection)
//            callsVec.pop_back();
//            callWork0->SetBitRate(call->GetBitRate());
//            call->SetMultiCallVec(callsVec);
//
//            for (auto &route: resources->allRoutes.at(nodePairIndex)) {
//                callWork0->SetRoute(route);
//                //callWork0->SetModulation(FixedModulation);
//                //this->modulation->SetModulationParam(call);
//                this->modulation->DefineBestModulation(call);
//                this->resDevAlloc->specAlloc->SpecAllocation(call);
//
//                if (topology->IsValidLigthPath(call)) {
//                    call->SetRoute(route);
//                    call->SetModulation(callWork0->GetModulation());
//                    call->SetFirstSlot(callWork0->GetFirstSlot());
//                    call->SetLastSlot(callWork0->GetLastSlot());
//                    call->SetStatus(Accepted);
//                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
//                    return;
//                }
//            }
//        }
    }

    if(numSchProtRoutes == 2){
        this->CreateProtectionCalls(call); //loading multiCall vector with calls

        //setting 2 calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        unsigned int orN = call->GetOrNode()->GetNodeId();
        unsigned int deN = call->GetDeNode()->GetNodeId();
        unsigned int numNodes = this->topology->GetNumNodes();
        unsigned int nodePairIndex = orN * numNodes + deN;

        //trying to allocate with 2 routes
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).back().empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups.at(nodePairIndex).back()) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 2 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group2.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }
        /* //Delete one route, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }
}

void PartitioningDedicatedPathProtection::RoutingSpecPDPP_DPGR_MultiP(CallDevices* call) {

    if(numSchProtRoutes == 4){
        this->CreateProtectionCalls(call); //loading multiCall vector with protection calls

        //setting 4 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);
        std::shared_ptr<Call> callWork3 = callsVec.at(3);

        unsigned int orN = call->GetOrNode()->GetNodeId();
        unsigned int deN = call->GetDeNode()->GetNodeId();
        unsigned int numNodes = this->topology->GetNumNodes();
        unsigned int nodePairIndex = orN * numNodes + deN;

        //trying allocate with 4 routes
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).front().empty()){
            for(auto& group4 : resources->protectionAllRoutesGroups.at(nodePairIndex).front()) {
                callWork0->SetRoute(group4.at(0));
                callWork1->SetRoute(group4.at(1));
                callWork2->SetRoute(group4.at(2));
                callWork3->SetRoute(group4.at(3));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 4 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group4.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }
        //Delete one route, recalculate Bit rate and try allocating with 3 routes
        callsVec.pop_back();
        double callBitRate = call->GetBitRate();
        double beta = parameters->GetBeta();
        double partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        callWork2->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(1).empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups.at(nodePairIndex).at(1)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 3 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group3.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }
        //Delete one route, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        callBitRate = call->GetBitRate();
        beta = parameters->GetBeta();
        partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-3));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).back().empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups.at(nodePairIndex).back()) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 2 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group2.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }

    if(numSchProtRoutes == 3){
        this->CreateProtectionCalls(call); //loading multiCall vector with protection calls

        //setting 3 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        unsigned int orN = call->GetOrNode()->GetNodeId();
        unsigned int deN = call->GetDeNode()->GetNodeId();
        unsigned int numNodes = this->topology->GetNumNodes();
        unsigned int nodePairIndex = orN * numNodes + deN;

        //trying allocate with 3 routes
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(1).empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups.at(nodePairIndex).at(1)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 3 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group3.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }

        //Delete one route, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        double callBitRate = call->GetBitRate();
        double beta = parameters->GetBeta();
        double partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).back().empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups.at(nodePairIndex).back()) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 2 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group2.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }
       /* //Delete one route again, recalculate Bit rate and try allocating just 1
        //route (without protection)
        callsVec.pop_back();
        callWork0->SetBitRate(call->GetBitRate());
        call->SetMultiCallVec(callsVec);

        for(auto& route : resources->allRoutes.at(nodePairIndex)){
            callWork0->SetRoute(route);
            //callWork0->SetModulation(FixedModulation);
            //this->modulation->SetModulationParam(call);
            this->modulation->DefineBestModulation(call);
            this->resDevAlloc->specAlloc->SpecAllocation(call);

            if(topology->IsValidLigthPath(call)){
                call->SetRoute(route);
                call->SetModulation(callWork0->GetModulation());
                call->SetFirstSlot(callWork0->GetFirstSlot());
                call->SetLastSlot(callWork0->GetLastSlot());
                call->SetStatus(Accepted);
                resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                return;
            }
        }*/
    }

    if(numSchProtRoutes == 2){
        this->CreateProtectionCalls(call); //loading multiCall vector with calls

        //setting 2 calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        unsigned int orN = call->GetOrNode()->GetNodeId();
        unsigned int deN = call->GetDeNode()->GetNodeId();
        unsigned int numNodes = this->topology->GetNumNodes();
        unsigned int nodePairIndex = orN * numNodes + deN;

        //trying to allocate with 2 routes
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).back().empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups.at(nodePairIndex).back()) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 2 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group2.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }
}

void PartitioningDedicatedPathProtection::SpecRoutingPDPP(CallDevices* call) {
    if(numSchProtRoutes == 3){
        this->routing->RoutingCall(call); //loading trialRoutes and trialprotRoutes

        this->CreateProtectionCalls(call); //loading multiCall vector with calls

        //seting 3 protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        //call->RepeatModulation();
        unsigned int numRoutes = call->GetNumRoutes();
        call->SetCore(0);
        bool allocCallWork0Found = false;
        bool allocCallWork1Found = false;
        bool allocCallWork2Found = false;
        unsigned int auxSlot0;
        unsigned int auxSlot1;
        unsigned int auxSlot2;
        const unsigned int topNumSlots = topology->GetNumSlots();
        std::vector<unsigned int> possibleSlots(0);
        possibleSlots = this->resDevAlloc->specAlloc->SpecAllocation();

        //slot loop for callWork0
        for (unsigned int s0 = 0; s0 < possibleSlots.size(); s0++) {
            auxSlot0 = possibleSlots.at(s0);

            //try allocating with 3 routes
            for (unsigned int k = 0; k < numRoutes; k++) {
                callWork0->SetRoute(call->GetRoute(k));
                callWork0->SetModulation(FixedModulation);

                //getting protection routes to use in next loop (FOR)
                std::deque<std::shared_ptr<Route>> ProtRoutes = call->GetProtRoutes(k);
                ProtRoutes.erase(std::remove(std::begin(ProtRoutes),
                          std::end(ProtRoutes), nullptr), std::end(ProtRoutes));
                unsigned int sizeProtRoutes = ProtRoutes.size();

                //calculate number of slots for current of call
                this->modulation->SetModulationParam(callWork0.get());

                if (auxSlot0 + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                    continue;
                //checking if callWork0 number of slots are available in its route
                if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot0,
                                     auxSlot0 + callWork0->GetNumberSlots() - 1)) {
                    callWork0->SetFirstSlot(auxSlot0);
                    callWork0->SetLastSlot(auxSlot0 + callWork0->GetNumberSlots() - 1);
                    callWork0->SetCore(0);

                    if (sizeProtRoutes >= 2) {  //if to skip case which it is no routes enough

                        //slot loop for callWork1
                        for (unsigned int s1 = 0; s1 < possibleSlots.size(); s1++) {
                                auxSlot1 = possibleSlots.at(s1);
                            for (unsigned int kd0 = 0; kd0 < sizeProtRoutes; kd0++) {
                                if (call->GetProtRoute(k, kd0)) {  //if to avoid null route pointer
                                    callWork1->SetRoute(call->GetProtRoute(k, kd0));
                                    callWork1->SetModulation(FixedModulation);

                                    this->modulation->SetModulationParam(callWork1.get());
                                    if (auxSlot1 + callWork1->GetNumberSlots() - 1 >= topNumSlots)
                                        continue;
                                    //checking if callWork1 slots are available in its route
                                    if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(), auxSlot1,
                                            auxSlot1 + callWork1->GetNumberSlots() - 1)) {
                                        callWork1->SetFirstSlot(auxSlot1);
                                        callWork1->SetLastSlot(auxSlot1 + callWork1->GetNumberSlots() -1);
                                        callWork1->SetCore(0);

                                        //slot loop for callWork2
                                        for (unsigned int s2 = 0; s2 < possibleSlots.size(); s2++) {auxSlot2 = possibleSlots.at(s2);

                                            for (unsigned int kd1 = 0; kd1 < sizeProtRoutes; kd1++) {
                                                if (kd0 == kd1)
                                                    continue;
                                                callWork2->SetRoute(call->GetProtRoute(k, kd1));
                                                callWork2->SetModulation(FixedModulation);

                                                this->modulation->SetModulationParam(callWork2.get());

                                                if (auxSlot2 + callWork2->GetNumberSlots() - 1 >= topNumSlots)
                                                    continue;
                                                //checking if callWork2 slots are available in its route
                                                if (this->resDevAlloc->CheckSlotsDisp(callWork2->GetRoute(), auxSlot2,
                                                        auxSlot2 + callWork2->GetNumberSlots() - 1)) {
                                                    callWork2->SetFirstSlot(auxSlot2);
                                                    callWork2->SetLastSlot(auxSlot2 +
                                                                        callWork2->GetNumberSlots() - 1);
                                                    callWork2->SetCore(0);

                                                    call->SetRoute(call->GetRoute(k));
                                                    call->SetModulation(FixedModulation);
                                                    call->SetFirstSlot(callWork0->GetFirstSlot());
                                                    call->SetLastSlot(callWork0->GetLastSlot());
                                                    call->ClearTrialRoutes();
                                                    call->ClearTrialProtRoutes();
                                                    call->SetStatus(Accepted);
                                                    //increment proCalls counter
                                                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                                                    CalcBetaAverage(call);
                                                    CalcAlpha(call);
                                                    allocCallWork0Found = true;
                                                    allocCallWork1Found = true;
                                                    allocCallWork2Found = true;
                                                    break;
                                                }
                                            }
                                            if(allocCallWork2Found)
                                                break;
                                        }
                                    }
                                }
                                if (allocCallWork1Found)
                                    break;
                            }
                            if (allocCallWork1Found)
                                break;
                        }
                    }
                }
                if(allocCallWork0Found)
                    break;
            }
            if(allocCallWork0Found)
                break;
        }

        if(allocCallWork2Found == false) {
            //Delete one route, recalculate Bit rate and try allocating with 2 routes
            callsVec.pop_back();
            double callBitRate = call->GetBitRate();
            double beta = parameters->GetBeta();
            double partialBitRate = ceil(
                    ((1 - beta) * callBitRate) / (numSchProtRoutes - 2));
            callWork0->SetBitRate(partialBitRate);
            callWork1->SetBitRate(partialBitRate);
            call->SetMultiCallVec(callsVec);

            //slot loop for callWork0
            for (unsigned int s0 = 0; s0 < possibleSlots.size(); s0++) {
                auxSlot0 = possibleSlots.at(s0);

                for (unsigned int k = 0; k < numRoutes; k++) {
                    callWork0->SetRoute(call->GetRoute(k));
                    callWork0->SetModulation(FixedModulation);

                    //getting protection routes to use in next loop (FOR)
                    std::deque <std::shared_ptr<Route>> ProtRoutes = call->GetProtRoutes(k);
                    ProtRoutes.erase(std::remove(std::begin(ProtRoutes),
                                std::end(ProtRoutes), nullptr),std::end(ProtRoutes));
                    unsigned int sizeProtRoutes = ProtRoutes.size();

                    //calculate number of slots for current of call
                    this->modulation->SetModulationParam(callWork0.get());

                    if (auxSlot0 + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                        continue;
                    //checking if callWork0 number of slots are available in its route
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot0,
                                           auxSlot0 + callWork0->GetNumberSlots() -1)) {
                        callWork0->SetFirstSlot(auxSlot0);
                        callWork0->SetLastSlot(auxSlot0 + callWork0->GetNumberSlots() - 1);
                        callWork0->SetCore(0);

                        if (sizeProtRoutes >= 1) {  //if to skip case which it is no routes enough

                            //slot loop for callWork1
                            for (unsigned int s1 = 0; s1 < possibleSlots.size(); s1++) {
                                auxSlot1 = possibleSlots.at(s1);
                                for (unsigned int kd0 = 0; kd0 < sizeProtRoutes; kd0++) {
                                    if (call->GetProtRoute(k,
                                                           kd0)) {  //if to avoid null route pointer
                                        callWork1->SetRoute(call->GetProtRoute(k, kd0));
                                        callWork1->SetModulation(FixedModulation);

                                        this->modulation->SetModulationParam(callWork1.get());

                                        if (auxSlot1 + callWork1->GetNumberSlots() - 1 >= topNumSlots)
                                            continue;
                                        //checking if callWork1 slots are available in its route
                                        if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(), auxSlot1,
                                                auxSlot1 + callWork1->GetNumberSlots() -1)) {
                                            callWork1->SetFirstSlot(auxSlot1);
                                            callWork1->SetLastSlot(auxSlot1 +callWork1->GetNumberSlots() - 1);
                                            callWork1->SetCore(0);

                                            call->SetRoute(call->GetRoute(k));
                                            call->SetModulation(FixedModulation);
                                            call->SetFirstSlot(callWork0->GetFirstSlot());
                                            call->SetLastSlot(callWork0->GetLastSlot());
                                            call->ClearTrialRoutes();
                                            call->ClearTrialProtRoutes();
                                            call->SetStatus(Accepted);
                                            resDevAlloc->simulType->GetData()->SetProtectedCalls(); //increment proCalls counter
                                            CalcBetaAverage(call);
                                            CalcAlpha(call);
                                            allocCallWork0Found = true;
                                            allocCallWork1Found = true;
                                            break;
                                        }
                                    }
                                }
                                if (allocCallWork1Found)
                                    break;
                            }
                        }
                    }
                    if (allocCallWork0Found)
                        break;
                }
                if (allocCallWork0Found)
                    break;
            }
        }
        /*if (allocCallWork0Found == false) {
            //Delete one route and try allocating just 1 route (without protection)
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (unsigned int s0 = 0; s0 < possibleSlots.size(); s0++) {
                auxSlot0 = possibleSlots.at(s0);

                for (unsigned int k = 0; k < numRoutes; k++) {
                    callWork0->SetRoute(call->GetRoute(k));
                    callWork0->SetModulation(FixedModulation);

                    //getting protection routes to use in next loop (FOR)
                    std::deque <std::shared_ptr<Route>> ProtRoutes = call->GetProtRoutes(k);
                    ProtRoutes.erase(std::remove(std::begin(ProtRoutes),
                                                 std::end(ProtRoutes), nullptr),
                                     std::end(ProtRoutes));
                    unsigned int sizeProtRoutes = ProtRoutes.size();

                    //calculate number of slots for current of call
                    this->modulation->SetModulationParam(callWork0.get());

                    if (auxSlot0 + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                        continue;
                    //checking if callWork0 number of slots are available in its route
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot0,
                                                          auxSlot0 +
                                                          callWork0->GetNumberSlots() - 1)) {
                        callWork0->SetFirstSlot(auxSlot0);
                        callWork0->SetLastSlot(auxSlot0 + callWork0->GetNumberSlots() - 1);
                        callWork0->SetCore(0);

                        call->SetRoute(call->GetRoute(k));
                        call->SetModulation(FixedModulation);
                        call->SetFirstSlot(callWork0->GetFirstSlot());
                        call->SetLastSlot(callWork0->GetLastSlot());
                        call->ClearTrialRoutes();
                        call->ClearTrialProtRoutes();
                        call->SetStatus(Accepted);
                        resDevAlloc->simulType->GetData()->SetNonProtectedCalls(); //increment proCalls counter
                        CalcBetaAverage(call);
                        CalcAlpha(call);
                        allocCallWork0Found = true;
                        break;
                    }
                }
                if (allocCallWork0Found)
                    break;
            }
        }*/
    }

    if(numSchProtRoutes == 2) {
        this->routing->RoutingCall(call); //loading trialRoutes and trialprotRoutes

        this->CreateProtectionCalls(call); //loading multiCall vector with calls

        //seting 2 protection calls to allocation
        std::vector <std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr <Call> callWork0 = callsVec.at(0);
        std::shared_ptr <Call> callWork1 = callsVec.at(1);

        //call->RepeatModulation();
        unsigned int numRoutes = call->GetNumRoutes();
        call->SetCore(0);
        bool allocCallWork0Found = false;
        bool allocCallWork1Found = false;

        const unsigned int topNumSlots = topology->GetNumSlots();
        std::vector<unsigned int> possibleSlots(0);
        possibleSlots = this->resDevAlloc->specAlloc->SpecAllocation();
        unsigned int auxSlot0;
        unsigned int auxSlot1;

        //slot loop for callWork0
        for (unsigned int s0 = 0; s0 < possibleSlots.size(); s0++) {
            auxSlot0 = possibleSlots.at(s0);

            for (unsigned int k = 0; k < numRoutes; k++) {
                callWork0->SetRoute(call->GetRoute(k));
                callWork0->SetModulation(FixedModulation);

                //getting protection routes to use in next loop (FOR)
                std::deque <std::shared_ptr<Route>> ProtRoutes = call->GetProtRoutes(k);
                ProtRoutes.erase(std::remove(std::begin(ProtRoutes),
                                             std::end(ProtRoutes), nullptr),
                                 std::end(ProtRoutes));
                unsigned int sizeProtRoutes = ProtRoutes.size();

                //calculate number of slots for current of call
                this->modulation->SetModulationParam(callWork0.get());

                if (auxSlot0 + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                    continue;
                //checking if callWork0 number of slots are available in its route
                if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot0,
                                                      auxSlot0 +
                                                      callWork0->GetNumberSlots() - 1)) {
                    callWork0->SetFirstSlot(auxSlot0);
                    callWork0->SetLastSlot(auxSlot0 + callWork0->GetNumberSlots() - 1);
                    callWork0->SetCore(0);

                    if (sizeProtRoutes >= 1) {  //if to skip case which it is no routes enough

                        //slot loop for callWork1
                        for (unsigned int s1 = 0; s1 < possibleSlots.size(); s1++) {
                            auxSlot1 = possibleSlots.at(s1);
                            for (unsigned int kd0 = 0; kd0 < sizeProtRoutes; kd0++) {
                                if (call->GetProtRoute(k, kd0)) {  //if to avoid null route pointer
                                    callWork1->SetRoute(call->GetProtRoute(k, kd0));
                                    callWork1->SetModulation(FixedModulation);

                                    this->modulation->SetModulationParam(callWork1.get());

                                    if (auxSlot1 + callWork1->GetNumberSlots() - 1 >=
                                        topNumSlots)
                                        continue;
                                    //checking if callWork1 slots are available in its route
                                    if (this->resDevAlloc->CheckSlotsDisp(
                                            callWork1->GetRoute(), auxSlot1,
                                            auxSlot1 + callWork1->GetNumberSlots() - 1)) {
                                        callWork1->SetFirstSlot(auxSlot1);
                                        callWork1->SetLastSlot(
                                                auxSlot1 + callWork1->GetNumberSlots() -
                                                1);
                                        callWork1->SetCore(0);

                                        call->SetRoute(call->GetRoute(k));
                                        call->SetModulation(FixedModulation);
                                        call->SetFirstSlot(callWork0->GetFirstSlot());
                                        call->SetLastSlot(callWork0->GetLastSlot());
                                        call->ClearTrialRoutes();
                                        call->ClearTrialProtRoutes();
                                        call->SetStatus(Accepted);
                                        resDevAlloc->simulType->GetData()->SetProtectedCalls(); //increment proCalls counter
                                        CalcBetaAverage(call);
                                        CalcAlpha(call);
                                        allocCallWork0Found = true;
                                        allocCallWork1Found = true;
                                        break;
                                    }
                                }
                            }
                            if (allocCallWork1Found)
                                break;
                        }
                    }
                }
                if (allocCallWork0Found)
                    break;
            }
            if (allocCallWork0Found)
                break;
        }

        /*if (allocCallWork0Found == false) {
            //Delete one route and try allocating just 1 route (without protection)
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (unsigned int s0 = 0; s0 < possibleSlots.size(); s0++) {
                auxSlot0 = possibleSlots.at(s0);

                for (unsigned int k = 0; k < numRoutes; k++) {
                    callWork0->SetRoute(call->GetRoute(k));
                    callWork0->SetModulation(FixedModulation);

                    //getting protection routes to use in next loop (FOR)
                    std::deque <std::shared_ptr<Route>> ProtRoutes = call->GetProtRoutes(k);
                    ProtRoutes.erase(std::remove(std::begin(ProtRoutes),
                                                 std::end(ProtRoutes), nullptr),
                                     std::end(ProtRoutes));
                    unsigned int sizeProtRoutes = ProtRoutes.size();

                    //calculate number of slots for current of call
                    this->modulation->SetModulationParam(callWork0.get());

                    if (auxSlot0 + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                        continue;
                    //checking if callWork0 number of slots are available in its route
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot0,
                                                          auxSlot0 +
                                                          callWork0->GetNumberSlots() - 1)) {
                        callWork0->SetFirstSlot(auxSlot0);
                        callWork0->SetLastSlot(auxSlot0 + callWork0->GetNumberSlots() - 1);
                        callWork0->SetCore(0);

                        call->SetRoute(call->GetRoute(k));
                        call->SetModulation(FixedModulation);
                        call->SetFirstSlot(callWork0->GetFirstSlot());
                        call->SetLastSlot(callWork0->GetLastSlot());
                        call->ClearTrialRoutes();
                        call->ClearTrialProtRoutes();
                        call->SetStatus(Accepted);
                        resDevAlloc->simulType->GetData()->SetNonProtectedCalls(); //increment proCalls counter
                        CalcBetaAverage(call);
                        CalcAlpha(call);
                        allocCallWork0Found = true;
                        break;
                    }
                }
                if (allocCallWork0Found)
                    break;
            }
        }*/
    }
}

void PartitioningDedicatedPathProtection::SpecRoutingPDPP_DPGR(CallDevices* call) {

    if(numSchProtRoutes == 4) {
        this->CreateProtectionCalls(call); //loading multiCall vector with calls

        //seting 4 protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);
        std::shared_ptr<Call> callWork3 = callsVec.at(3);

        call->SetCore(0);
        unsigned int auxSlot;
        unsigned int sumFirstSlots = 0;
        const unsigned int topNumSlots = topology->GetNumSlots();
        std::vector<unsigned int> possibleSlots(0);
        std::vector<int> firstSlotIndexesSum(0);
        std::vector<std::vector<int>> firstSlotIndexes(0);
        possibleSlots = this->resDevAlloc->specAlloc->SpecAllocation();
        unsigned int orN = call->GetOrNode()->GetNodeId();
        unsigned int deN = call->GetDeNode()->GetNodeId();
        unsigned int numNodes = this->topology->GetNumNodes();
        unsigned int nodePairIndex = orN * numNodes + deN;
        bool callAllocated = false;
        unsigned int groupIndex = 0;

        //trying allocate call with 4 routes
        if (!resources->protectionAllRoutesGroups.at(nodePairIndex).front().empty()) {
            unsigned int numGroups = resources->protectionAllRoutesGroups.at(
                    nodePairIndex).front().size();
            firstSlotIndexes.resize(numGroups);
            firstSlotIndexesSum.resize(numGroups);
            //computing the first slot indexes available of each group for current call and its sum
            for (auto &group4: resources->protectionAllRoutesGroups.at(
                    nodePairIndex).front()) {
                bool allocCallWork0Found = false;
                bool allocCallWork1Found = false;
                bool allocCallWork2Found = false;
                bool allocCallWork3Found = false;
                sumFirstSlots = 0;
                callWork0->SetRoute(group4.at(0));
                this->modulation->DefineBestModulation(callWork0.get());
                for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                    auxSlot = possibleSlots.at(s);
                    if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                        break;
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot,
                                                          auxSlot +
                                                          callWork0->GetNumberSlots() -
                                                          1)) {
                        firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                        sumFirstSlots = auxSlot;
                        allocCallWork0Found = true;
                        break;
                    }
                }
                if (allocCallWork0Found == true) {
                    callWork1->SetRoute(group4.at(1));
                    this->modulation->DefineBestModulation(callWork1.get());
                    for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                        auxSlot = possibleSlots.at(s);
                        if (auxSlot + callWork1->GetNumberSlots() - 1 >= topNumSlots)
                            break;
                        if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(),
                                                              auxSlot, auxSlot +
                                                                       callWork1->GetNumberSlots() -
                                                                       1)) {
                            firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                            sumFirstSlots += auxSlot;
                            allocCallWork1Found = true;
                            break;
                        }
                    }
                }
                if (allocCallWork1Found == true) {
                    callWork2->SetRoute(group4.at(2));
                    this->modulation->DefineBestModulation(callWork2.get());
                    for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                        auxSlot = possibleSlots.at(s);
                        if (auxSlot + callWork2->GetNumberSlots() - 1 >= topNumSlots)
                            break;
                        if (this->resDevAlloc->CheckSlotsDisp(callWork2->GetRoute(),
                                                              auxSlot, auxSlot +
                                                                       callWork2->GetNumberSlots() -
                                                                       1)) {
                            firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                            sumFirstSlots += auxSlot;
                            allocCallWork2Found = true;
                            break;
                        }
                    }
                }
                if (allocCallWork2Found == true) {
                    callWork3->SetRoute(group4.at(3));
                    this->modulation->DefineBestModulation(callWork3.get());
                    for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                        auxSlot = possibleSlots.at(s);
                        if (auxSlot + callWork3->GetNumberSlots() - 1 >= topNumSlots)
                            break;
                        if (this->resDevAlloc->CheckSlotsDisp(callWork3->GetRoute(),
                                                              auxSlot, auxSlot +
                                                                       callWork3->GetNumberSlots() -
                                                                       1)) {
                            firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                            sumFirstSlots += auxSlot;
                            firstSlotIndexesSum.at(groupIndex) = sumFirstSlots;
                            allocCallWork3Found = true;
                            break;
                        }
                    }
                }
                if (allocCallWork3Found == false) {
                    firstSlotIndexesSum.at(groupIndex) = Def::Max_Int;
                }
                groupIndex++;
            }

            //allocating call using minimum slot index group and minimum number of hops
            //int minElementIndex = std::min_element(firstSlotIndexesSum.begin(),
            //           firstSlotIndexesSum.end()) -firstSlotIndexesSum.begin();
            int minSlotIndexSum = *std::min_element(firstSlotIndexesSum.begin(),
                                                    firstSlotIndexesSum.end());
            unsigned int counterIndex = 0;
            //unsigned int numHopSum = 0;
            //std::pair<unsigned, unsigned> minSlotIndex;
            //std::vector<std::pair<unsigned ,unsigned >> minSlotIndexVec;
            for (auto index: firstSlotIndexesSum) {
                if (index == minSlotIndexSum && index != Def::Max_Int) {
                    callWork0->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).front().at(counterIndex).at(0));
                    this->modulation->DefineBestModulation(callWork0.get());
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(),
                                                          firstSlotIndexes.at(
                                                                  counterIndex).at(0),
                                                          firstSlotIndexes.at(
                                                                  counterIndex).at(0) +
                                                          callWork0->GetNumberSlots() -
                                                          1)) {
                        callWork0->SetFirstSlot(firstSlotIndexes.at(counterIndex).at(0));
                        callWork0->SetLastSlot(firstSlotIndexes.at(counterIndex).at(0) +
                                               callWork0->GetNumberSlots() - 1);
                        callWork0->SetCore(0);
                    }
                    callWork1->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).front().at(counterIndex).at(1));
                    this->modulation->DefineBestModulation(callWork1.get());
                    if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(),
                                                          firstSlotIndexes.at(
                                                                  counterIndex).at(1),
                                                          firstSlotIndexes.at(
                                                                  counterIndex).at(1) +
                                                          callWork1->GetNumberSlots() -
                                                          1)) {
                        callWork1->SetFirstSlot(firstSlotIndexes.at(counterIndex).at(1));
                        callWork1->SetLastSlot(firstSlotIndexes.at(counterIndex).at(1) +
                                               callWork1->GetNumberSlots() - 1);
                        callWork1->SetCore(0);
                    }
                    callWork2->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).front().at(counterIndex).at(2));
                    this->modulation->DefineBestModulation(callWork2.get());
                    if (this->resDevAlloc->CheckSlotsDisp(callWork2->GetRoute(),
                                                          firstSlotIndexes.at(
                                                                  counterIndex).at(2),
                                                          firstSlotIndexes.at(
                                                                  counterIndex).at(2) +
                                                          callWork2->GetNumberSlots() -
                                                          1)) {
                        callWork2->SetFirstSlot(firstSlotIndexes.at(counterIndex).at(2));
                        callWork2->SetLastSlot(firstSlotIndexes.at(counterIndex).at(2) +
                                               callWork2->GetNumberSlots() - 1);
                        callWork2->SetCore(0);
                    }
                    callWork3->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).front().at(counterIndex).at(3));
                    this->modulation->DefineBestModulation(callWork3.get());
                    if (this->resDevAlloc->CheckSlotsDisp(callWork3->GetRoute(),
                                                          firstSlotIndexes.at(
                                                                  counterIndex).at(3),
                                                          firstSlotIndexes.at(
                                                                  counterIndex).at(3) +
                                                          callWork3->GetNumberSlots() -
                                                          1)) {
                        callWork3->SetFirstSlot(firstSlotIndexes.at(counterIndex).at(3));
                        callWork3->SetLastSlot(firstSlotIndexes.at(counterIndex).at(3) +
                                               callWork3->GetNumberSlots() - 1);
                        callWork3->SetCore(0);
                    }
                    call->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).front().at(counterIndex).at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    //increment proCalls counter
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    callAllocated = true;
                    break;
                }
                counterIndex++;
            }
        }

        if (callAllocated == false) {
            //Delete one route, recalculate Bit rate and try allocating with 3 routes
            callsVec.pop_back();
            double callBitRate = call->GetBitRate();
            double beta = parameters->GetBeta();
            double partialBitRate = ceil(
                    ((1 - beta) * callBitRate) / (numSchProtRoutes - 2));
            callWork0->SetBitRate(partialBitRate);
            callWork1->SetBitRate(partialBitRate);
            callWork2->SetBitRate(partialBitRate);
            call->SetMultiCallVec(callsVec);
            groupIndex = 0;

            //trying allocate call with 3 routes
            if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(1).empty()) {
                unsigned int numGroups = resources->protectionAllRoutesGroups.at(
                        nodePairIndex).at(1).size();
                firstSlotIndexes.resize(numGroups);
                firstSlotIndexesSum.resize(numGroups);
                //computing the first slot indexes available of each group for current call and its sum
                for (auto &group3: resources->protectionAllRoutesGroups.at(
                        nodePairIndex).at(1)) {
                    bool allocCallWork0Found = false;
                    bool allocCallWork1Found = false;
                    bool allocCallWork2Found = false;
                    sumFirstSlots = 0;
                    callWork0->SetRoute(group3.at(0));
                    this->modulation->DefineBestModulation(callWork0.get());
                    for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                        auxSlot = possibleSlots.at(s);
                        if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                            break;
                        if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(),
                                                              auxSlot,
                                                              auxSlot +
                                                              callWork0->GetNumberSlots() -
                                                              1)) {
                            firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                            sumFirstSlots = auxSlot;
                            allocCallWork0Found = true;
                            break;
                        }
                    }
                    if (allocCallWork0Found == true) {
                        callWork1->SetRoute(group3.at(1));
                        this->modulation->DefineBestModulation(callWork1.get());
                        for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                            auxSlot = possibleSlots.at(s);
                            if (auxSlot + callWork1->GetNumberSlots() - 1 >= topNumSlots)
                                break;
                            if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(),
                                                                  auxSlot, auxSlot +
                                                                           callWork1->GetNumberSlots() -
                                                                           1)) {
                                firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                                sumFirstSlots += auxSlot;
                                allocCallWork1Found = true;
                                break;
                            }
                        }
                    }
                    if (allocCallWork1Found == true) {
                        callWork2->SetRoute(group3.at(2));
                        this->modulation->DefineBestModulation(callWork2.get());
                        for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                            auxSlot = possibleSlots.at(s);
                            if (auxSlot + callWork2->GetNumberSlots() - 1 >= topNumSlots)
                                break;
                            if (this->resDevAlloc->CheckSlotsDisp(callWork2->GetRoute(),
                                                                  auxSlot, auxSlot +
                                                                           callWork2->GetNumberSlots() -
                                                                           1)) {
                                firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                                sumFirstSlots += auxSlot;
                                firstSlotIndexesSum.at(groupIndex) = sumFirstSlots;
                                allocCallWork2Found = true;
                                break;
                            }
                        }
                    }
                    if (allocCallWork2Found == false) {
                        firstSlotIndexesSum.at(groupIndex) = Def::Max_Int;
                    }
                    groupIndex++;
                }

                //allocating call using minimum slot index group and minimum number of hops
                //int minElementIndex = std::min_element(firstSlotIndexesSum.begin(),
                //           firstSlotIndexesSum.end()) -firstSlotIndexesSum.begin();
                int minSlotIndexSum = *std::min_element(firstSlotIndexesSum.begin(),
                                                        firstSlotIndexesSum.end());
                unsigned int counterIndex = 0;
                //unsigned int numHopSum = 0;
                //std::pair<unsigned, unsigned> minSlotIndex;
                //std::vector<std::pair<unsigned ,unsigned >> minSlotIndexVec;
                for (auto index: firstSlotIndexesSum) {
                    if (index == minSlotIndexSum && index != Def::Max_Int) {
                        callWork0->SetRoute(resources->protectionAllRoutesGroups.at(
                                nodePairIndex).at(1).at(counterIndex).at(0));
                        this->modulation->DefineBestModulation(callWork0.get());
                        if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(),
                                                              firstSlotIndexes.at(
                                                                      counterIndex).at(0),
                                                              firstSlotIndexes.at(
                                                                      counterIndex).at(
                                                                      0) +
                                                              callWork0->GetNumberSlots() -
                                                              1)) {
                            callWork0->SetFirstSlot(
                                    firstSlotIndexes.at(counterIndex).at(0));
                            callWork0->SetLastSlot(
                                    firstSlotIndexes.at(counterIndex).at(0) +
                                    callWork0->GetNumberSlots() - 1);
                            callWork0->SetCore(0);
                        }
                        callWork1->SetRoute(resources->protectionAllRoutesGroups.at(
                                nodePairIndex).at(1).at(counterIndex).at(1));
                        this->modulation->DefineBestModulation(callWork1.get());
                        if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(),
                                                              firstSlotIndexes.at(
                                                                      counterIndex).at(1),
                                                              firstSlotIndexes.at(
                                                                      counterIndex).at(
                                                                      1) +
                                                              callWork1->GetNumberSlots() -
                                                              1)) {
                            callWork1->SetFirstSlot(
                                    firstSlotIndexes.at(counterIndex).at(1));
                            callWork1->SetLastSlot(
                                    firstSlotIndexes.at(counterIndex).at(1) +
                                    callWork1->GetNumberSlots() - 1);
                            callWork1->SetCore(0);
                        }
                        callWork2->SetRoute(resources->protectionAllRoutesGroups.at(
                                nodePairIndex).at(1).at(counterIndex).at(2));
                        this->modulation->DefineBestModulation(callWork2.get());
                        if (this->resDevAlloc->CheckSlotsDisp(callWork2->GetRoute(),
                                                              firstSlotIndexes.at(
                                                                      counterIndex).at(2),
                                                              firstSlotIndexes.at(
                                                                      counterIndex).at(
                                                                      2) +
                                                              callWork2->GetNumberSlots() -
                                                              1)) {
                            callWork2->SetFirstSlot(
                                    firstSlotIndexes.at(counterIndex).at(2));
                            callWork2->SetLastSlot(
                                    firstSlotIndexes.at(counterIndex).at(2) +
                                    callWork2->GetNumberSlots() - 1);
                            callWork2->SetCore(0);
                        }

                        call->SetRoute(resources->protectionAllRoutesGroups.at(
                                nodePairIndex).at(1).at(counterIndex).at(0));
                        call->SetModulation(callWork0->GetModulation());
                        call->SetFirstSlot(callWork0->GetFirstSlot());
                        call->SetLastSlot(callWork0->GetLastSlot());
                        call->SetStatus(Accepted);
                        //increment proCalls counter
                        resDevAlloc->simulType->GetData()->SetProtectedCalls();
                        CalcBetaAverage(call);
                        CalcAlpha(call);
                        callAllocated = true;
                        break;
                    }
                    counterIndex++;
                }
            }

            if (callAllocated == false) {
                //Delete one route, recalculate Bit rate and try allocating with 2 routes
                callsVec.pop_back();
                double callBitRate = call->GetBitRate();
                double beta = parameters->GetBeta();
                double partialBitRate = ceil(
                        ((1 - beta) * callBitRate) / (numSchProtRoutes - 3));
                callWork0->SetBitRate(partialBitRate);
                callWork1->SetBitRate(partialBitRate);
                call->SetMultiCallVec(callsVec);
                groupIndex = 0;

                //trying allocate call with 2 routes
                if (!resources->protectionAllRoutesGroups.at(
                        nodePairIndex).back().empty()) {
                    unsigned int numGroups = resources->protectionAllRoutesGroups.at(
                            nodePairIndex).back().size();
                    firstSlotIndexesSum.clear();
                    firstSlotIndexes.clear();
                    firstSlotIndexes.resize(numGroups);
                    firstSlotIndexesSum.resize(numGroups);
                    //computing the first slot indexes available of each group for current call and its sum
                    for (auto &group2: resources->protectionAllRoutesGroups.at(
                            nodePairIndex).back()) {
                        bool allocCallWork0Found = false;
                        bool allocCallWork1Found = false;
                        sumFirstSlots = 0;
                        callWork0->SetRoute(group2.at(0));
                        this->modulation->DefineBestModulation(callWork0.get());
                        for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                            auxSlot = possibleSlots.at(s);
                            if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                                break;
                            if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(),
                                                                  auxSlot, auxSlot +
                                                                           callWork0->GetNumberSlots() -
                                                                           1)) {
                                firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                                sumFirstSlots = auxSlot;
                                allocCallWork0Found = true;
                                break;
                            }
                        }
                        if (allocCallWork0Found == true) {
                            callWork1->SetRoute(group2.at(1));
                            this->modulation->DefineBestModulation(callWork1.get());
                            for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                                auxSlot = possibleSlots.at(s);
                                if (auxSlot + callWork1->GetNumberSlots() - 1 >=
                                    topNumSlots)
                                    break;
                                if (this->resDevAlloc->CheckSlotsDisp(
                                        callWork1->GetRoute(),
                                        auxSlot,
                                        auxSlot + callWork1->GetNumberSlots() - 1)) {
                                    firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                                    sumFirstSlots += auxSlot;
                                    firstSlotIndexesSum.at(groupIndex) = sumFirstSlots;
                                    allocCallWork1Found = true;
                                    break;
                                }
                            }
                        }
                        if (allocCallWork1Found == false) {
                            firstSlotIndexesSum.at(groupIndex) = Def::Max_Int;
                        }
                        groupIndex++;
                    }

                    //allocating call using minimum slot index group and minimum number of hops
                    //int minElementIndex = std::min_element(firstSlotIndexesSum.begin(),
                    //            firstSlotIndexesSum.end()) -firstSlotIndexesSum.begin();
                    int minSlotIndexSum = *std::min_element(firstSlotIndexesSum.begin(),
                                                            firstSlotIndexesSum.end());
                    unsigned int counterIndex = 0;
                    //unsigned int numHopSum = 0;
                    //std::pair<unsigned, unsigned> minSlotIndex;
                    //std::vector<std::pair<unsigned ,unsigned >> minSlotIndexVec;
                    for (auto index: firstSlotIndexesSum) {
                        if (index == minSlotIndexSum && index != Def::Max_Int) {
                            callWork0->SetRoute(resources->protectionAllRoutesGroups.at(
                                    nodePairIndex).back().at(counterIndex).at(0));
                            //callWork0->SetModulation(FixedModulation);
                            //this->modulation->SetModulationParam(callWork0.get());
                            this->modulation->DefineBestModulation(callWork0.get());
                            if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(),
                                                                  firstSlotIndexes.at(
                                                                          counterIndex).at(
                                                                          0),
                                                                  firstSlotIndexes.at(
                                                                          counterIndex).at(
                                                                          0) +
                                                                  callWork0->GetNumberSlots() -
                                                                  1)) {
                                callWork0->SetFirstSlot(
                                        firstSlotIndexes.at(counterIndex).at(0));
                                callWork0->SetLastSlot(
                                        firstSlotIndexes.at(counterIndex).at(0) +
                                        callWork0->GetNumberSlots() - 1);
                                callWork0->SetCore(0);
                            }
                            callWork1->SetRoute(resources->protectionAllRoutesGroups.at(
                                    nodePairIndex).back().at(counterIndex).at(1));
                            //callWork1->SetModulation(FixedModulation);
                            //this->modulation->SetModulationParam(callWork1.get());
                            this->modulation->DefineBestModulation(callWork1.get());
                            if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(),
                                                                  firstSlotIndexes.at(
                                                                          counterIndex).at(
                                                                          1),
                                                                  firstSlotIndexes.at(
                                                                          counterIndex).at(
                                                                          1) +
                                                                  callWork1->GetNumberSlots() -
                                                                  1)) {
                                callWork1->SetFirstSlot(
                                        firstSlotIndexes.at(counterIndex).at(1));
                                callWork1->SetLastSlot(
                                        firstSlotIndexes.at(counterIndex).at(1) +
                                        callWork1->GetNumberSlots() - 1);
                                callWork1->SetCore(0);
                            }
                            call->SetRoute(resources->protectionAllRoutesGroups.at(
                                    nodePairIndex).back().at(counterIndex).at(0));
                            call->SetModulation(callWork0->GetModulation());
                            call->SetFirstSlot(callWork0->GetFirstSlot());
                            call->SetLastSlot(callWork0->GetLastSlot());
                            call->SetStatus(Accepted);
                            //increment proCalls counter
                            resDevAlloc->simulType->GetData()->SetProtectedCalls();
                            CalcBetaAverage(call);
                            CalcAlpha(call);
                            callAllocated = true;
                            break;
                        }
                        counterIndex++;
                    }
                }
            }
            /*  if(callAllocated == false) {
                  //Delete one route and try allocating just 1 route (without protection)
                  callsVec.pop_back();
                  callWork0->SetBitRate(call->GetBitRate());
                  call->SetMultiCallVec(callsVec);

                  for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                      auxSlot = possibleSlots.at(s);
                      for(auto& route : resources->allRoutes.at(nodePairIndex)){
                          callWork0->SetRoute(route);
                          //callWork0->SetModulation(FixedModulation);

                          //calculate number of slots for current of call
                          //this->modulation->SetModulationParam(callWork0.get());
                          this->modulation->DefineBestModulation(callWork0.get());

                          if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                              continue;
                          //checking if callWork0 number of slots are available in its route
                          if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot,
                                                                auxSlot +
                                                                callWork0->GetNumberSlots() - 1)) {
                              callWork0->SetFirstSlot(auxSlot);
                              callWork0->SetLastSlot(auxSlot + callWork0->GetNumberSlots() - 1);
                              callWork0->SetCore(0);

                              call->SetRoute(route);
                              call->SetModulation(callWork0->GetModulation());
                              call->SetFirstSlot(callWork0->GetFirstSlot());
                              call->SetLastSlot(callWork0->GetLastSlot());
                              call->SetStatus(Accepted);
                              //increment proCalls counter
                              resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                              CalcBetaAverage(call);
                              CalcAlpha(call);
                              callAllocated = true;
                              break;
                          }
                      }
                      if(callAllocated == true){
                          break;
                      }
                  }
              }*/
        }
    }

    if(numSchProtRoutes == 3) {
        this->CreateProtectionCalls(call); //loading multiCall vector with calls

        //seting 3 protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        call->SetCore(0);
        unsigned int auxSlot;
        unsigned int sumFirstSlots = 0;
        const unsigned int topNumSlots = topology->GetNumSlots();
        std::vector<unsigned int> possibleSlots(0);
        std::vector<int> firstSlotIndexesSum(0);
        std::vector<std::vector<int>> firstSlotIndexes(0);
        possibleSlots = this->resDevAlloc->specAlloc->SpecAllocation();
        unsigned int orN = call->GetOrNode()->GetNodeId();
        unsigned int deN = call->GetDeNode()->GetNodeId();
        unsigned int numNodes = this->topology->GetNumNodes();
        unsigned int nodePairIndex = orN * numNodes + deN;
        bool callAllocated = false;
        unsigned int groupIndex = 0;

        //trying allocate call with 3 routes
        if (!resources->protectionAllRoutesGroups.at(nodePairIndex).at(1).empty()) {
            unsigned int numGroups = resources->protectionAllRoutesGroups.at(
                    nodePairIndex).at(1).size();
            firstSlotIndexes.resize(numGroups);
            firstSlotIndexesSum.resize(numGroups);
            //computing the first slot indexes available of each group for current call and its sum
            for (auto &group3: resources->protectionAllRoutesGroups.at(
                    nodePairIndex).at(1)) {
                bool allocCallWork0Found = false;
                bool allocCallWork1Found = false;
                bool allocCallWork2Found = false;
                sumFirstSlots = 0;
                callWork0->SetRoute(group3.at(0));
                this->modulation->DefineBestModulation(callWork0.get());
                for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                    auxSlot = possibleSlots.at(s);
                    if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                        break;
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot,
                             auxSlot +callWork0->GetNumberSlots() -1)) {
                        firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                        sumFirstSlots = auxSlot;
                        allocCallWork0Found = true;
                        break;
                    }
                }
                if (allocCallWork0Found == true) {
                    callWork1->SetRoute(group3.at(1));
                    this->modulation->DefineBestModulation(callWork1.get());
                    for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                        auxSlot = possibleSlots.at(s);
                        if (auxSlot + callWork1->GetNumberSlots() - 1 >= topNumSlots)
                            break;
                        if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(),
                          auxSlot,auxSlot +callWork1->GetNumberSlots() -1)) {
                            firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                            sumFirstSlots += auxSlot;
                            allocCallWork1Found = true;
                            break;
                        }
                    }
                }
                if (allocCallWork1Found == true) {
                    callWork2->SetRoute(group3.at(2));
                    this->modulation->DefineBestModulation(callWork2.get());
                    for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                        auxSlot = possibleSlots.at(s);
                        if (auxSlot + callWork2->GetNumberSlots() - 1 >= topNumSlots)
                            break;
                        if (this->resDevAlloc->CheckSlotsDisp(callWork2->GetRoute(),
                            auxSlot, auxSlot +callWork2->GetNumberSlots() -1)) {
                            firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                            sumFirstSlots += auxSlot;
                            firstSlotIndexesSum.at(groupIndex) = sumFirstSlots;
                            allocCallWork2Found = true;
                            break;
                        }
                    }
                }
                if (allocCallWork2Found == false) {
                    firstSlotIndexesSum.at(groupIndex) = Def::Max_Int;
                }
                groupIndex++;
            }

            //allocating call using minimum slot index group and minimum number of hops
            //int minElementIndex = std::min_element(firstSlotIndexesSum.begin(),
             //           firstSlotIndexesSum.end()) -firstSlotIndexesSum.begin();
            int minSlotIndexSum = *std::min_element(firstSlotIndexesSum.begin(),
                                                    firstSlotIndexesSum.end());
            unsigned int counterIndex = 0;
            //unsigned int numHopSum = 0;
            //std::pair<unsigned, unsigned> minSlotIndex;
            //std::vector<std::pair<unsigned ,unsigned >> minSlotIndexVec;
            for (auto index: firstSlotIndexesSum) {
                if (index == minSlotIndexSum && index != Def::Max_Int) {
                    callWork0->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).at(1).at(counterIndex).at(0));
                    this->modulation->DefineBestModulation(callWork0.get());
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(),
                     firstSlotIndexes.at(counterIndex).at(0),
                     firstSlotIndexes.at(counterIndex).at(0) +callWork0->GetNumberSlots() -1)) {
                        callWork0->SetFirstSlot(firstSlotIndexes.at(counterIndex).at(0));
                        callWork0->SetLastSlot(firstSlotIndexes.at(counterIndex).at(0) +
                                               callWork0->GetNumberSlots() - 1);
                        callWork0->SetCore(0);
                    }
                    callWork1->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).at(1).at(counterIndex).at(1));
                    this->modulation->DefineBestModulation(callWork1.get());
                    if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(),
                    firstSlotIndexes.at(counterIndex).at(1),
                    firstSlotIndexes.at(counterIndex).at(1) +callWork1->GetNumberSlots() -1)) {
                        callWork1->SetFirstSlot(firstSlotIndexes.at(counterIndex).at(1));
                        callWork1->SetLastSlot(firstSlotIndexes.at(counterIndex).at(1) +
                                               callWork1->GetNumberSlots() - 1);
                        callWork1->SetCore(0);
                    }
                    callWork2->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).at(1).at(counterIndex).at(2));
                    this->modulation->DefineBestModulation(callWork2.get());
                    if (this->resDevAlloc->CheckSlotsDisp(callWork2->GetRoute(),
                     firstSlotIndexes.at(counterIndex).at(2),
                     firstSlotIndexes.at(counterIndex).at(2) +callWork2->GetNumberSlots() -1)) {
                        callWork2->SetFirstSlot(firstSlotIndexes.at(counterIndex).at(2));
                        callWork2->SetLastSlot(firstSlotIndexes.at(counterIndex).at(2) +
                                               callWork2->GetNumberSlots() - 1);
                        callWork2->SetCore(0);
                    }

                    call->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).at(1).at(counterIndex).at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    //increment proCalls counter
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    callAllocated = true;
                    break;
                }
                counterIndex++;
            }
        }
        if(callAllocated == false) {
            //Delete one route, recalculate Bit rate and try allocating with 2 routes
            callsVec.pop_back();
            double callBitRate = call->GetBitRate();
            double beta = parameters->GetBeta();
            double partialBitRate = ceil(
                    ((1 - beta) * callBitRate) / (numSchProtRoutes - 2));
            callWork0->SetBitRate(partialBitRate);
            callWork1->SetBitRate(partialBitRate);
            call->SetMultiCallVec(callsVec);
            groupIndex = 0;

            //trying allocate call with 2 routes
            if (!resources->protectionAllRoutesGroups.at(nodePairIndex).back().empty()) {
                unsigned int numGroups = resources->protectionAllRoutesGroups.at(
                        nodePairIndex).back().size();
                firstSlotIndexesSum.clear();
                firstSlotIndexes.clear();
                firstSlotIndexes.resize(numGroups);
                firstSlotIndexesSum.resize(numGroups);
                //computing the first slot indexes available of each group for current call and its sum
                for (auto &group2: resources->protectionAllRoutesGroups.at(
                        nodePairIndex).back()) {
                    bool allocCallWork0Found = false;
                    bool allocCallWork1Found = false;
                    sumFirstSlots = 0;
                    callWork0->SetRoute(group2.at(0));
                    this->modulation->DefineBestModulation(callWork0.get());
                    for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                        auxSlot = possibleSlots.at(s);
                        if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                            break;
                        if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(),
                         auxSlot,auxSlot +callWork0->GetNumberSlots() -1)) {
                            firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                            sumFirstSlots = auxSlot;
                            allocCallWork0Found = true;
                            break;
                        }
                    }
                    if (allocCallWork0Found == true) {
                        callWork1->SetRoute(group2.at(1));
                        this->modulation->DefineBestModulation(callWork1.get());
                        for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                            auxSlot = possibleSlots.at(s);
                            if (auxSlot + callWork1->GetNumberSlots() - 1 >= topNumSlots)
                                break;
                            if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(),
                             auxSlot,auxSlot +callWork1->GetNumberSlots() -1)) {
                                firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                                sumFirstSlots += auxSlot;
                                firstSlotIndexesSum.at(groupIndex) = sumFirstSlots;
                                allocCallWork1Found = true;
                                break;
                            }
                        }
                    }
                    if (allocCallWork1Found == false) {
                        firstSlotIndexesSum.at(groupIndex) = Def::Max_Int;
                    }
                    groupIndex++;
                }

                //allocating call using minimum slot index group and minimum number of hops
                //int minElementIndex = std::min_element(firstSlotIndexesSum.begin(),
                //            firstSlotIndexesSum.end()) -firstSlotIndexesSum.begin();
                int minSlotIndexSum = *std::min_element(firstSlotIndexesSum.begin(),
                                                        firstSlotIndexesSum.end());
                unsigned int counterIndex = 0;
                //unsigned int numHopSum = 0;
                //std::pair<unsigned, unsigned> minSlotIndex;
                //std::vector<std::pair<unsigned ,unsigned >> minSlotIndexVec;
                for (auto index: firstSlotIndexesSum) {
                    if (index == minSlotIndexSum && index != Def::Max_Int) {
                        callWork0->SetRoute(resources->protectionAllRoutesGroups.at(
                                nodePairIndex).back().at(counterIndex).at(0));
                        //callWork0->SetModulation(FixedModulation);
                        //this->modulation->SetModulationParam(callWork0.get());
                        this->modulation->DefineBestModulation(callWork0.get());
                        if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(),
                          firstSlotIndexes.at(counterIndex).at(0),
                           firstSlotIndexes.at(counterIndex).at(0)+callWork0->GetNumberSlots() -1)) {
                            callWork0->SetFirstSlot(
                                    firstSlotIndexes.at(counterIndex).at(0));
                            callWork0->SetLastSlot(
                                    firstSlotIndexes.at(counterIndex).at(0) +
                                    callWork0->GetNumberSlots() - 1);
                            callWork0->SetCore(0);
                        }
                        callWork1->SetRoute(resources->protectionAllRoutesGroups.at(
                                nodePairIndex).back().at(counterIndex).at(1));
                        //callWork1->SetModulation(FixedModulation);
                        //this->modulation->SetModulationParam(callWork1.get());
                        this->modulation->DefineBestModulation(callWork1.get());
                        if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(),
                         firstSlotIndexes.at(counterIndex).at(1),
                          firstSlotIndexes.at(counterIndex).at(1) +callWork1->GetNumberSlots() -1)) {
                            callWork1->SetFirstSlot(
                                    firstSlotIndexes.at(counterIndex).at(1));
                            callWork1->SetLastSlot(
                                    firstSlotIndexes.at(counterIndex).at(1) +
                                    callWork1->GetNumberSlots() - 1);
                            callWork1->SetCore(0);
                        }
                        call->SetRoute(resources->protectionAllRoutesGroups.at(
                                nodePairIndex).back().at(counterIndex).at(0));
                        call->SetModulation(callWork0->GetModulation());
                        call->SetFirstSlot(callWork0->GetFirstSlot());
                        call->SetLastSlot(callWork0->GetLastSlot());
                        call->SetStatus(Accepted);
                        //increment proCalls counter
                        resDevAlloc->simulType->GetData()->SetProtectedCalls();
                        CalcBetaAverage(call);
                        CalcAlpha(call);
                        callAllocated = true;
                        break;
                    }
                    counterIndex++;
                }
            }
        }
      /*  if(callAllocated == false) {
            //Delete one route and try allocating just 1 route (without protection)
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                auxSlot = possibleSlots.at(s);
                for(auto& route : resources->allRoutes.at(nodePairIndex)){
                    callWork0->SetRoute(route);
                    //callWork0->SetModulation(FixedModulation);

                    //calculate number of slots for current of call
                    //this->modulation->SetModulationParam(callWork0.get());
                    this->modulation->DefineBestModulation(callWork0.get());

                    if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                        continue;
                    //checking if callWork0 number of slots are available in its route
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot,
                                                          auxSlot +
                                                          callWork0->GetNumberSlots() - 1)) {
                        callWork0->SetFirstSlot(auxSlot);
                        callWork0->SetLastSlot(auxSlot + callWork0->GetNumberSlots() - 1);
                        callWork0->SetCore(0);

                        call->SetRoute(route);
                        call->SetModulation(callWork0->GetModulation());
                        call->SetFirstSlot(callWork0->GetFirstSlot());
                        call->SetLastSlot(callWork0->GetLastSlot());
                        call->SetStatus(Accepted);
                        //increment proCalls counter
                        resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                        CalcBetaAverage(call);
                        CalcAlpha(call);
                        callAllocated = true;
                        break;
                    }
                }
                if(callAllocated == true){
                    break;
                }
            }
        }*/
    }

    if(numSchProtRoutes == 2){
        this->CreateProtectionCalls(call); //loading multiCall vector with calls

        //seting 2 protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        call->SetCore(0);
        unsigned int auxSlot;
        unsigned int sumFirstSlots = 0;
        const unsigned int topNumSlots = topology->GetNumSlots();
        std::vector<unsigned int> possibleSlots(0);
        std::vector<int> firstSlotIndexesSum(0);
        std::vector<std::vector<int>> firstSlotIndexes(0);
        possibleSlots = this->resDevAlloc->specAlloc->SpecAllocation();
        unsigned int orN = call->GetOrNode()->GetNodeId();
        unsigned int deN = call->GetDeNode()->GetNodeId();
        unsigned int numNodes = this->topology->GetNumNodes();
        unsigned int nodePairIndex = orN * numNodes + deN;
        bool callAllocated = false;
        unsigned int groupIndex = 0;

        //trying allocate call with 2 routes
        if (!resources->protectionAllRoutesGroups.at(nodePairIndex).back().empty()) {
            unsigned int numGroups = resources->protectionAllRoutesGroups.at(
                    nodePairIndex).back().size();
            firstSlotIndexesSum.clear();
            firstSlotIndexes.clear();
            firstSlotIndexes.resize(numGroups);
            firstSlotIndexesSum.resize(numGroups);
            //computing the first slot indexes available of each group for current call and its sum
            for (auto &group2: resources->protectionAllRoutesGroups.at(nodePairIndex).back()) {
//                if(groupIndex == parameters->GetNumberRoutes())
//                    break;
                bool allocCallWork0Found = false;
                bool allocCallWork1Found = false;
                sumFirstSlots = 0;
                callWork0->SetRoute(group2.at(0));
                callWork0->SetModulation(FixedModulation);
                this->modulation->SetModulationParam(callWork0.get());
                for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                    auxSlot = possibleSlots.at(s);
                    if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                        break;
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(),
                                                          auxSlot,auxSlot +callWork0->GetNumberSlots() -1)) {
                        firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                        sumFirstSlots = auxSlot;
                        allocCallWork0Found = true;
                        break;
                    }
                }
                if (allocCallWork0Found == true) {
                    callWork1->SetRoute(group2.at(1));
                    callWork1->SetModulation(FixedModulation);
                    this->modulation->SetModulationParam(callWork1.get());
                    for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                        auxSlot = possibleSlots.at(s);
                        if (auxSlot + callWork1->GetNumberSlots() - 1 >= topNumSlots)
                            break;
                        if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(),
                                                              auxSlot,auxSlot +callWork1->GetNumberSlots() -1)) {
                            firstSlotIndexes.at(groupIndex).push_back(auxSlot);
                            sumFirstSlots += auxSlot;
                            firstSlotIndexesSum.at(groupIndex) = sumFirstSlots;
                            allocCallWork1Found = true;
                            break;
                        }
                    }
                }
                if (allocCallWork1Found == false) {
                    firstSlotIndexesSum.at(groupIndex) = Def::Max_Int;
                }
                groupIndex++;
            }

            //allocating call using minimum slot index group and minimum number of hops
            //int minElementIndex = std::min_element(firstSlotIndexesSum.begin(),
            //            firstSlotIndexesSum.end()) -firstSlotIndexesSum.begin();
            int minSlotIndexSum = *std::min_element(firstSlotIndexesSum.begin(),
                                                    firstSlotIndexesSum.end());
            unsigned int counterIndex = 0;
            //unsigned int numHopSum = 0;
            //std::pair<unsigned, unsigned> minSlotIndex;
            //std::vector<std::pair<unsigned ,unsigned >> minSlotIndexVec;
            for (auto index: firstSlotIndexesSum) {
                if (index == minSlotIndexSum && index != Def::Max_Int) {
                    callWork0->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).back().at(counterIndex).at(0));
                    callWork0->SetModulation(FixedModulation);
                    this->modulation->SetModulationParam(callWork0.get());
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(),
                                                          firstSlotIndexes.at(counterIndex).at(0),
                                                          firstSlotIndexes.at(counterIndex).at(0)+callWork0->GetNumberSlots() -1)) {
                        callWork0->SetFirstSlot(
                                firstSlotIndexes.at(counterIndex).at(0));
                        callWork0->SetLastSlot(
                                firstSlotIndexes.at(counterIndex).at(0) +
                                callWork0->GetNumberSlots() - 1);
                        callWork0->SetCore(0);
                    }
                    callWork1->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).back().at(counterIndex).at(1));
                    callWork1->SetModulation(FixedModulation);
                    this->modulation->SetModulationParam(callWork1.get());
                    if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(),
                                                          firstSlotIndexes.at(counterIndex).at(1),
                                                          firstSlotIndexes.at(counterIndex).at(1) +callWork1->GetNumberSlots() -1)) {
                        callWork1->SetFirstSlot(
                                firstSlotIndexes.at(counterIndex).at(1));
                        callWork1->SetLastSlot(
                                firstSlotIndexes.at(counterIndex).at(1) +
                                callWork1->GetNumberSlots() - 1);
                        callWork1->SetCore(0);
                    }
                    call->SetRoute(resources->protectionAllRoutesGroups.at(
                            nodePairIndex).back().at(counterIndex).at(0));
                    call->SetModulation(FixedModulation);
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    //increment proCalls counter
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    callAllocated = true;
                    break;
                }
                counterIndex++;
            }
        }
        /*  if(callAllocated == false) {
            //Delete one route and try allocating just 1 route (without protection)
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

            for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                auxSlot = possibleSlots.at(s);
                for(auto& route : resources->allRoutes.at(nodePairIndex)){
                    callWork0->SetRoute(route);
                    callWork0->SetModulation(FixedModulation);

                    //calculate number of slots for current of call
                    this->modulation->SetModulationParam(callWork0.get());

                    if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                        continue;
                    //checking if callWork0 number of slots are available in its route
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot,
                                                          auxSlot +
                                                          callWork0->GetNumberSlots() - 1)) {
                        callWork0->SetFirstSlot(auxSlot);
                        callWork0->SetLastSlot(auxSlot + callWork0->GetNumberSlots() - 1);
                        callWork0->SetCore(0);

                        call->SetRoute(route);
                        call->SetModulation(FixedModulation);
                        call->SetFirstSlot(callWork0->GetFirstSlot());
                        call->SetLastSlot(callWork0->GetLastSlot());
                        call->SetStatus(Accepted);
                        //increment proCalls counter
                        resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                        CalcBetaAverage(call);
                        CalcAlpha(call);
                        callAllocated = true;
                        break;
                    }
                }
                if(callAllocated == true){
                    break;
                }
            }
        }*/
    }
}

void PartitioningDedicatedPathProtection::SpecRoutingSameSlotPDPP(CallDevices* call) {
    if(numSchProtRoutes == 3){
        this->routing->RoutingCall(call); //loading trialRoutes and trialprotRoutes
        unsigned int numRoutes = call->GetNumRoutes();

        this->CreateProtectionCalls(call); //loading multiCall vector with calls

        //seting 3 protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        //call->RepeatModulation();
        call->SetCore(0);
        bool allocCallWork0Found = false;
        bool allocCallWork1Found = false;
        bool allocCallWork2Found = false;
        const unsigned int topNumSlots = topology->GetNumSlots();
        std::vector<unsigned int> possibleSlots(0);
        possibleSlots = this->resDevAlloc->specAlloc->SpecAllocation();
        unsigned int auxSlot;

        for (unsigned int s = 0; s < possibleSlots.size(); s++) {
            auxSlot = possibleSlots.at(s);

            //try allocating with 3 routes
            for (unsigned int k = 0; k < numRoutes; k++) {
                callWork0->SetRoute(call->GetRoute(k));
                callWork0->SetModulation(FixedModulation);

                //getting protection routes to use in next loop (FOR)
                std::deque<std::shared_ptr<Route>> ProtRoutes = call->GetProtRoutes(k);
                ProtRoutes.erase(std::remove(std::begin(ProtRoutes),
                       std::end(ProtRoutes), nullptr), std::end(ProtRoutes));
                unsigned int sizeProtRoutes = ProtRoutes.size();

                //calculate number of slots for current of call
                this->modulation->SetModulationParam(callWork0.get());

                if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                    continue;
                //checking if callWork0 number of slots are available in its route
                if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot,
                                         auxSlot + callWork0->GetNumberSlots() - 1)) {
                    callWork0->SetFirstSlot(auxSlot);
                    callWork0->SetLastSlot(auxSlot + callWork0->GetNumberSlots() - 1);
                    callWork0->SetCore(0);

                    if (sizeProtRoutes >= 2) {  //if to skip case which it is no routes enough
                        for (unsigned int kd0 = 0; kd0 < sizeProtRoutes; kd0++) {
                            if (call->GetProtRoute(k, kd0)) {  //if to avoid null route pointer
                                callWork1->SetRoute(call->GetProtRoute(k, kd0));
                                callWork1->SetModulation(FixedModulation);

                                this->modulation->SetModulationParam(callWork1.get());

                                if (auxSlot + callWork1->GetNumberSlots() - 1 >= topNumSlots)
                                    continue;
                                //checking if callWork1 slots are available in its route
                                if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(), auxSlot,
                                                 auxSlot + callWork1->GetNumberSlots() - 1)) {
                                    callWork1->SetFirstSlot(auxSlot);
                                    callWork1->SetLastSlot(auxSlot + callWork1->GetNumberSlots() - 1);
                                    callWork1->SetCore(0);

                                    for (unsigned int kd1 = 0; kd1 < sizeProtRoutes; kd1++) {
                                        if (kd0 == kd1)
                                            continue;
                                        callWork2->SetRoute(call->GetProtRoute(k, kd1));
                                        callWork2->SetModulation(FixedModulation);

                                        this->modulation->SetModulationParam(callWork2.get());

                                        if (auxSlot + callWork2->GetNumberSlots() - 1 >= topNumSlots)
                                            continue;
                                        //checking if callWork2 slots are available in its route
                                        if (this->resDevAlloc->CheckSlotsDisp(callWork2->GetRoute(), auxSlot,
                                                    auxSlot + callWork2->GetNumberSlots() - 1)) {
                                            callWork2->SetFirstSlot(auxSlot);
                                            callWork2->SetLastSlot(auxSlot + callWork2->GetNumberSlots() - 1);
                                            callWork2->SetCore(0);

                                            call->SetRoute(call->GetRoute(k));
                                            call->SetModulation(FixedModulation);
                                            call->SetFirstSlot(callWork0->GetFirstSlot());
                                            call->SetLastSlot(callWork0->GetLastSlot());
                                            call->ClearTrialRoutes();
                                            call->ClearTrialProtRoutes();
                                            call->SetStatus(Accepted);
                                            resDevAlloc->simulType->GetData()->SetProtectedCalls(); //increment proCalls counter
                                            CalcBetaAverage(call);
                                            CalcAlpha(call);
                                            allocCallWork0Found = true;
                                            allocCallWork1Found = true;
                                            allocCallWork2Found = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            if(allocCallWork1Found)
                                break;
                        }
                    }
                }
                if(allocCallWork0Found)
                    break;
            }
            if(allocCallWork2Found)
                break;
        }

        if(allocCallWork2Found == false) {
            //Delete one route, recalculate Bit rate and try allocating with 2 routes
            callsVec.pop_back();
            double callBitRate = call->GetBitRate();
            double beta = parameters->GetBeta();
            double partialBitRate = ceil(
                    ((1 - beta) * callBitRate) / (numSchProtRoutes - 2));
            callWork0->SetBitRate(partialBitRate);
            callWork1->SetBitRate(partialBitRate);
            call->SetMultiCallVec(callsVec);

            //slot loop for callWork0
            for (unsigned int s = 0; s < possibleSlots.size(); s++) {
                auxSlot = possibleSlots.at(s);

                for (unsigned int k = 0; k < numRoutes; k++) {
                    callWork0->SetRoute(call->GetRoute(k));
                    callWork0->SetModulation(FixedModulation);

                    //getting protection routes to use in next loop (FOR)
                    std::deque <std::shared_ptr<Route>> ProtRoutes = call->GetProtRoutes(k);
                    ProtRoutes.erase(std::remove(std::begin(ProtRoutes),
                                     std::end(ProtRoutes), nullptr),std::end(ProtRoutes));
                    unsigned int sizeProtRoutes = ProtRoutes.size();

                    //calculate number of slots for current of call
                    this->modulation->SetModulationParam(callWork0.get());

                    if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                        continue;
                    //checking if callWork0 number of slots are available in its route
                    if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot,
                                                     auxSlot + callWork0->GetNumberSlots() -1)) {
                        callWork0->SetFirstSlot(auxSlot);
                        callWork0->SetLastSlot(auxSlot + callWork0->GetNumberSlots() - 1);
                        callWork0->SetCore(0);

                        if (sizeProtRoutes >= 1) {  //if to skip case which it is no routes enough

                            for (unsigned int kd0 = 0; kd0 < sizeProtRoutes; kd0++) {
                                if (call->GetProtRoute(k, kd0)) {  //if to avoid null route pointer
                                    callWork1->SetRoute(call->GetProtRoute(k, kd0));
                                    callWork1->SetModulation(FixedModulation);

                                    this->modulation->SetModulationParam(callWork1.get());

                                    if (auxSlot + callWork1->GetNumberSlots() - 1 >= topNumSlots)
                                        continue;
                                    //checking if callWork1 slots are available in its route
                                    if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(), auxSlot,
                                            auxSlot + callWork1->GetNumberSlots() - 1)) {
                                        callWork1->SetFirstSlot(auxSlot);
                                        callWork1->SetLastSlot(
                                                auxSlot + callWork1->GetNumberSlots() - 1);
                                        callWork1->SetCore(0);

                                        call->SetRoute(call->GetRoute(k));
                                        call->SetModulation(FixedModulation);
                                        call->SetFirstSlot(callWork0->GetFirstSlot());
                                        call->SetLastSlot(callWork0->GetLastSlot());
                                        call->ClearTrialRoutes();
                                        call->ClearTrialProtRoutes();
                                        call->SetStatus(Accepted);
                                        resDevAlloc->simulType->GetData()->SetProtectedCalls(); //increment proCalls counter
                                        CalcBetaAverage(call);
                                        CalcAlpha(call);
                                        allocCallWork0Found = true;
                                        allocCallWork1Found = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if (allocCallWork0Found)
                        break;
                }
                if (allocCallWork1Found)
                    break;
            }
        }
        /*if(allocCallWork1Found == false)
        //Delete one route and try allocating just 1 route (without protection)
            callsVec.pop_back();
            callWork0->SetBitRate(call->GetBitRate());
            call->SetMultiCallVec(callsVec);

        for (unsigned int s = 0; s < possibleSlots.size(); s++) {
            auxSlot = possibleSlots.at(s);
            for (unsigned int k = 0; k < numRoutes; k++) {
                callWork0->SetRoute(call->GetRoute(k));
                callWork0->SetModulation(FixedModulation);

                //calculate number of slots for current of call
                this->modulation->SetModulationParam(callWork0.get());

                if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                    continue;
                //checking if callWork0 number of slots are available in its route
                if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot,
                                                      auxSlot +
                                                      callWork0->GetNumberSlots() -
                                                      1)) {
                    callWork0->SetFirstSlot(auxSlot);
                    callWork0->SetLastSlot(auxSlot + callWork0->GetNumberSlots() - 1);
                    callWork0->SetCore(0);

                    call->SetRoute(call->GetRoute(k));
                    call->SetModulation(FixedModulation);
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->ClearTrialRoutes();
                    call->ClearTrialProtRoutes();
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetNonProtectedCalls(); //increment proCalls counter
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    break;
                }
            }
        }*/
    }

    if(numSchProtRoutes == 2){
        this->routing->RoutingCall(call); //loading trialRoutes and trialprotRoutes
        unsigned int numRoutes = call->GetNumRoutes();

        this->CreateProtectionCalls(call); //loading multiCall vector with calls

        //seting 2 protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        //call->RepeatModulation();
        call->SetCore(0);
        bool allocCallWork0Found = false;
        bool allocCallWork1Found = false;

        const unsigned int topNumSlots = topology->GetNumSlots();
        std::vector<unsigned int> possibleSlots(0);
        possibleSlots = this->resDevAlloc->specAlloc->SpecAllocation();
        unsigned int auxSlot;

        //try allocation with 2 routes
        for (unsigned int s = 0; s < possibleSlots.size(); s++) {
            auxSlot = possibleSlots.at(s);

            for (unsigned int k = 0; k < numRoutes; k++) {
                callWork0->SetRoute(call->GetRoute(k));
                callWork0->SetModulation(FixedModulation);

                //getting protection routes to use in next loop (FOR)
                std::deque <std::shared_ptr<Route>> ProtRoutes = call->GetProtRoutes(k);
                ProtRoutes.erase(std::remove(std::begin(ProtRoutes),
                          std::end(ProtRoutes), nullptr),std::end(ProtRoutes));
                unsigned int sizeProtRoutes = ProtRoutes.size();

                //calculate number of slots for current of call
                this->modulation->SetModulationParam(callWork0.get());

                if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                    continue;
                //checking if callWork0 number of slots are available in its route
                if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot,
                                   auxSlot +callWork0->GetNumberSlots() -1)) {
                    callWork0->SetFirstSlot(auxSlot);
                    callWork0->SetLastSlot(auxSlot + callWork0->GetNumberSlots() - 1);
                    callWork0->SetCore(0);

                    if (sizeProtRoutes >= 1) {  //if to skip case which it is no routes enough
                        for (unsigned int kd0 = 0; kd0 < sizeProtRoutes; kd0++) {
                            if (call->GetProtRoute(k, kd0)) {  //if to avoid null route pointer
                                callWork1->SetRoute(call->GetProtRoute(k, kd0));
                                callWork1->SetModulation(FixedModulation);

                                this->modulation->SetModulationParam(callWork1.get());

                                if (auxSlot + callWork1->GetNumberSlots() - 1 >= topNumSlots)
                                    continue;
                                //checking if callWork1 slots are available in its route
                                if (this->resDevAlloc->CheckSlotsDisp(callWork1->GetRoute(), auxSlot,
                                        auxSlot + callWork1->GetNumberSlots() - 1)) {
                                    callWork1->SetFirstSlot(auxSlot);
                                    callWork1->SetLastSlot(auxSlot + callWork1->GetNumberSlots() - 1);
                                    callWork1->SetCore(0);

                                    call->SetRoute(call->GetRoute(k));
                                    call->SetModulation(FixedModulation);
                                    call->SetFirstSlot(callWork0->GetFirstSlot());
                                    call->SetLastSlot(callWork0->GetLastSlot());
                                    call->ClearTrialRoutes();
                                    call->ClearTrialProtRoutes();
                                    call->SetStatus(Accepted);
                                    resDevAlloc->simulType->GetData()->SetProtectedCalls(); //increment proCalls counter
                                    CalcBetaAverage(call);
                                    CalcAlpha(call);
                                    allocCallWork0Found = true;
                                    allocCallWork1Found = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (allocCallWork0Found)
                    break;
            }
            if (allocCallWork1Found)
                break;
        }
    }
    /*if(allocCallWork1Found == false)
    //Delete one route and try allocating just 1 route (without protection)
        callsVec.pop_back();
        callWork0->SetBitRate(call->GetBitRate());
        call->SetMultiCallVec(callsVec);

    for (unsigned int s = 0; s < possibleSlots.size(); s++) {
        auxSlot = possibleSlots.at(s);
        for (unsigned int k = 0; k < numRoutes; k++) {
            callWork0->SetRoute(call->GetRoute(k));
            callWork0->SetModulation(FixedModulation);

            //calculate number of slots for current of call
            this->modulation->SetModulationParam(callWork0.get());

            if (auxSlot + callWork0->GetNumberSlots() - 1 >= topNumSlots)
                continue;
            //checking if callWork0 number of slots are available in its route
            if (this->resDevAlloc->CheckSlotsDisp(callWork0->GetRoute(), auxSlot,
                                                  auxSlot +
                                                  callWork0->GetNumberSlots() -
                                                  1)) {
                callWork0->SetFirstSlot(auxSlot);
                callWork0->SetLastSlot(auxSlot + callWork0->GetNumberSlots() - 1);
                callWork0->SetCore(0);

                call->SetRoute(call->GetRoute(k));
                call->SetModulation(FixedModulation);
                call->SetFirstSlot(callWork0->GetFirstSlot());
                call->SetLastSlot(callWork0->GetLastSlot());
                call->ClearTrialRoutes();
                call->ClearTrialProtRoutes();
                call->SetStatus(Accepted);
                resDevAlloc->simulType->GetData()->SetNonProtectedCalls(); //increment proCalls counter
                CalcBetaAverage(call);
                CalcAlpha(call);
                break;
            }
        }
    }*/
}

void PartitioningDedicatedPathProtection::ResourceAllocProtectionPDPP_MinNumSlot(CallDevices *call) {

    if(numSchProtRoutes == 3){
        this->CreateProtectionCalls(call); //loading multiCall vector with protection calls

        unsigned int orN = call->GetOrNode()->GetNodeId();
        unsigned int deN = call->GetDeNode()->GetNodeId();
        unsigned int numNodes = this->topology->GetNumNodes();
        unsigned int nodePairIndex = orN * numNodes + deN;

        //setting 3 partitioned protection calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);
        std::shared_ptr<Call> callWork2 = callsVec.at(2);

        int numTotalSlotG = 0;
        std::vector<int> auxTotalSlotGroupsVec;
        std::vector<std::vector<std::shared_ptr<Route>>> auxTotalRouteGroupsVec;

        //computing the total number of required slots from each group for current call
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
                .empty()){
            for(auto& group3 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 3 routes
                for (const auto &partition: callsVec) {
                    numTotalSlotG += partition->GetTotalNumSlots();
                }
                auxTotalSlotGroupsVec.push_back(numTotalSlotG);
                auxTotalRouteGroupsVec.push_back(group3);
                numTotalSlotG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxTotalSlotGroupsVec.size(); gi++) {
                int Ci = auxTotalSlotGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxTotalRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxTotalSlotGroupsVec[gj - 1]; gj--) {
                    auxTotalSlotGroupsVec[gj] = auxTotalSlotGroupsVec[gj - 1];
                    auxTotalRouteGroupsVec[gj] = auxTotalRouteGroupsVec[gj - 1];
                }
                auxTotalSlotGroupsVec[gj] = Ci;
                auxTotalRouteGroupsVec[gj] = Ri;
            }
            //updating the set of groups in ProtectionAllRoutesGroups with ordered groups
            resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
                    = auxTotalRouteGroupsVec;
            auxTotalSlotGroupsVec.clear();
            auxTotalRouteGroupsVec.clear();

            //trying allocate with 3 routes
            for(auto& group3 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group3.at(0));
                callWork1->SetRoute(group3.at(1));
                callWork2->SetRoute(group3.at(2));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 3 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group3.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }

        //Delete one route, recalculate Bit rate and try allocating with 2 routes
        callsVec.pop_back();
        double callBitRate = call->GetBitRate();
        double beta = parameters->GetBeta();
        double partialBitRate = ceil (((1 - beta) * callBitRate) / (numSchProtRoutes-2));
        callWork0->SetBitRate(partialBitRate);
        callWork1->SetBitRate(partialBitRate);
        call->SetMultiCallVec(callsVec);

        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
                .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 3 routes
                for (const auto &partition: callsVec) {
                    numTotalSlotG += partition->GetTotalNumSlots();
                }
                auxTotalSlotGroupsVec.push_back(numTotalSlotG);
                auxTotalRouteGroupsVec.push_back(group2);
                numTotalSlotG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxTotalSlotGroupsVec.size(); gi++) {
                int Ci = auxTotalSlotGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxTotalRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxTotalSlotGroupsVec[gj - 1]; gj--) {
                    auxTotalSlotGroupsVec[gj] = auxTotalSlotGroupsVec[gj - 1];
                    auxTotalRouteGroupsVec[gj] = auxTotalRouteGroupsVec[gj - 1];
                }
                auxTotalSlotGroupsVec[gj] = Ci;
                auxTotalRouteGroupsVec[gj] = Ri;
            }
            //updating the set of groups in ProtectionAllRoutesGroups with ordered groups
            resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-3)
                    = auxTotalRouteGroupsVec;
            auxTotalSlotGroupsVec.clear();
            auxTotalRouteGroupsVec.clear();

            //trying allocate with 2 routes
            for(auto& group2 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-3)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 2 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group2.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }

    if(numSchProtRoutes == 2){
        this->CreateProtectionCalls(call); //loading multiCall vector with calls

        //setting 2 calls to allocation
        std::vector<std::shared_ptr<Call>> callsVec = call->GetMultiCallVec();
        std::shared_ptr<Call> callWork0 = callsVec.at(0);
        std::shared_ptr<Call> callWork1 = callsVec.at(1);

        unsigned int orN = call->GetOrNode()->GetNodeId();
        unsigned int deN = call->GetDeNode()->GetNodeId();
        unsigned int numNodes = this->topology->GetNumNodes();
        unsigned int nodePairIndex = orN * numNodes + deN;

        int numTotalSlotG = 0;
        std::vector<int> auxTotalSlotGroupsVec;
        std::vector<std::vector<std::shared_ptr<Route>>> auxTotalRouteGroupsVec;

        //trying to allocate with 2 routes
        if(!resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
                .empty()){
            for(auto& group2 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 3 routes
                for (const auto &partition: callsVec) {
                    numTotalSlotG += partition->GetTotalNumSlots();
                }
                auxTotalSlotGroupsVec.push_back(numTotalSlotG);
                auxTotalRouteGroupsVec.push_back(group2);
                numTotalSlotG = 0;
            }
            //ordering groups in aux vectors by total number of required slots
            for (int gi = 1; gi < auxTotalSlotGroupsVec.size(); gi++) {
                int Ci = auxTotalSlotGroupsVec[gi];
                std::vector<std::shared_ptr<Route>> Ri = auxTotalRouteGroupsVec[gi];
                int gj;
                for (gj = gi; gj > 0 && Ci < auxTotalSlotGroupsVec[gj - 1]; gj--) {
                    auxTotalSlotGroupsVec[gj] = auxTotalSlotGroupsVec[gj - 1];
                    auxTotalRouteGroupsVec[gj] = auxTotalRouteGroupsVec[gj - 1];
                }
                auxTotalSlotGroupsVec[gj] = Ci;
                auxTotalRouteGroupsVec[gj] = Ri;
            }
            //updating the set of groups in ProtectionAllRoutesGroups with ordered groups
            resources->protectionAllRoutesGroups.at(nodePairIndex).at(numSchProtRoutes-2)
                    = auxTotalRouteGroupsVec;
            auxTotalSlotGroupsVec.clear();
            auxTotalRouteGroupsVec.clear();

            //trying to allocate with 2 routes
            for(auto& group2 : resources->protectionAllRoutesGroups
                    .at(nodePairIndex).at(numSchProtRoutes-2)) {
                callWork0->SetRoute(group2.at(0));
                callWork1->SetRoute(group2.at(1));

                //defining modulation format and number of slots for the vector of calls
                this->modulation->DefineBestModulation(call);
                //check if the number of slots are available in the 2 routes
                this->resDevAlloc->specAlloc->SpecAllocation(call);

                if (topology->IsValidLigthPath(call)) {
                    call->SetRoute(group2.at(0));
                    call->SetModulation(callWork0->GetModulation());
                    call->SetFirstSlot(callWork0->GetFirstSlot());
                    call->SetLastSlot(callWork0->GetLastSlot());
                    call->SetStatus(Accepted);
                    resDevAlloc->simulType->GetData()->SetProtectedCalls();
                    CalcBetaAverage(call);
                    CalcAlpha(call);
                    return;
                }
            }
        }
        /* //Delete one route again, recalculate Bit rate and try allocating just 1
         //route (without protection)
         callsVec.pop_back();
         callWork0->SetBitRate(call->GetBitRate());
         call->SetMultiCallVec(callsVec);

         for(auto& route : resources->allRoutes.at(nodePairIndex)){
             callWork0->SetRoute(route);
             //callWork0->SetModulation(FixedModulation);
             //this->modulation->SetModulationParam(call);
             this->modulation->DefineBestModulation(call);
             this->resDevAlloc->specAlloc->SpecAllocation(call);

             if(topology->IsValidLigthPath(call)){
                 call->SetRoute(route);
                 call->SetModulation(callWork0->GetModulation());
                 call->SetFirstSlot(callWork0->GetFirstSlot());
                 call->SetLastSlot(callWork0->GetLastSlot());
                 call->SetStatus(Accepted);
                 resDevAlloc->simulType->GetData()->SetNonProtectedCalls();
                 return;
             }
         }*/
    }
}





void PartitioningDedicatedPathProtection::CreateProtectionCalls(CallDevices* call) {   
    unsigned int orN = call->GetOrNode()->GetNodeId();
    unsigned int deN = call->GetDeNode()->GetNodeId();
    unsigned int numNodes = this->topology->GetNumNodes();
    unsigned int nodePairIndex = orN * numNodes + deN;
    call->GetMultiCalls().clear();
    std::vector<double> VecTraffic = resDevAlloc->traffic->GetVecTraffic();
    double callBitRate = call->GetBitRate();
    unsigned int trafficIndex;
    
    for(unsigned int a = 0; a < VecTraffic.size(); a++){
        if(callBitRate == VecTraffic.at(a))
            trafficIndex = a;
    }  
    
    std::shared_ptr<Call> auxCall;
    std::vector<std::shared_ptr<Call>> auxVec(0);
        
    for(unsigned int a = 0; a < numSchProtRoutes; a++){
        auxCall = std::make_shared<Call>(call->GetOrNode(),                     
        call->GetDeNode(), PDPPBitRateNodePairsDist.at(nodePairIndex).at
        (trafficIndex).at(a), call->GetDeactivationTime());             
        auxVec.push_back(auxCall);
    }
    call->SetMultiCallVec(auxVec);
}

std::vector<std::vector<std::vector<double>>> PartitioningDedicatedPathProtection::
GetPDPPBitRateNodePairsDist() const {
    return PDPPBitRateNodePairsDist;

}

void PartitioningDedicatedPathProtection::SetPDPPBitRateNodePairsDist
(std::vector<std::vector<std::vector<double>>> PDPPBitRateNodePairsDist) {
    this->PDPPBitRateNodePairsDist = PDPPBitRateNodePairsDist;
}

void PartitioningDedicatedPathProtection::SetPDPPBitRateNodePairDistGA(){
    std::ifstream auxIfstream;
    double auxBR = 0;
    unsigned int numNodes = this->topology->GetNumNodes();
    this->resDevAlloc->GetSimulType()->GetInputOutput()->
    LoadPDPPBitRateNodePairsDistFirstSimul(auxIfstream);
    unsigned int numTraffic = resDevAlloc->GetSimulType()->GetTraffic()->
    GetVecTraffic().size();
    unsigned int numPDPPRoutes = resDevAlloc->GetSimulType()->GetParameters()->
    GetNumberPDPPprotectionRoutes();
    std::vector<std::vector<std::vector<double>>> auxPDPPBitRateNodePairsDist;
    auxPDPPBitRateNodePairsDist.resize(numNodes*numNodes);

    for(unsigned int a = 0; a < numNodes*numNodes; a++){
        auxPDPPBitRateNodePairsDist.at(a).resize(numTraffic);
        for(unsigned int trIndex = 0; trIndex < numTraffic; trIndex++) {
            for(unsigned int numRoutes = 0; numRoutes < numPDPPRoutes; numRoutes++){
                auxIfstream >> auxBR;
                auxPDPPBitRateNodePairsDist.at(a).at(trIndex).push_back(auxBR);
            }
        }
    }
    this->SetPDPPBitRateNodePairsDist(auxPDPPBitRateNodePairsDist);
}

void PartitioningDedicatedPathProtection::RSA_ProtectionCalls(CallDevices *call,
std::vector<std::shared_ptr<Route>> group, std::shared_ptr<Call> firstPartition) {

    //defining modulation format and number of slots for the vector of calls
    this->modulation->DefineBestModulation(call);
    //check if the number of slots are available in the 3 routes
    this->resDevAlloc->specAlloc->SpecAllocation(call);

    if (topology->IsValidLigthPath(call)) {
        call->SetRoute(group.at(0));
        call->SetModulation(firstPartition->GetModulation());
        call->SetFirstSlot(firstPartition->GetFirstSlot());
        call->SetLastSlot(firstPartition->GetLastSlot());
        call->SetStatus(Accepted);
        resDevAlloc->simulType->GetData()->SetProtectedCalls();
        CalcBetaAverage(call);
        CalcAlpha(call);
    }
}
























