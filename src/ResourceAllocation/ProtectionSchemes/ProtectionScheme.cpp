/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ProtectionScheme.cpp
 * Author: henrique
 * 
 * Created on October 26, 2019, 9:31 AM
 */

#include "../../../include/ResourceAllocation/ProtectionSchemes/ProtectionScheme.h"
#include "../../../include/Calls/Call.h"
#include "../../../include/Data/Parameters.h"
#include "../../../include/SimulationType/SimulationType.h"
#include "../../../include/Data/Data.h"
#include "math.h" 

ProtectionScheme::ProtectionScheme(ResourceDeviceAlloc* rsa)
:resDevAlloc(rsa), resources(rsa->GetResources()), topology(rsa->GetTopology()),
parameters(rsa->parameters), modulation(rsa->GetModulation()), 
routing(rsa->routing.get()), protectionScheme(this->protectionScheme),
numSchProtRoutes(2) {
    
}

ProtectionScheme::~ProtectionScheme() {
    
}

void ProtectionScheme::CalcBetaAverage(CallDevices* call) {
    double callBetaAverage;
 
    if(call->GetMultiCallVec().size() == 3){
        //getting bit rate of each route
        double BR0 = call->GetMultiCalls().at(0)->GetBitRate();
        double BR1 = call->GetMultiCalls().at(1)->GetBitRate();
        double BR2 = call->GetMultiCalls().at(2)->GetBitRate();
        double BRR = call->GetBitRate(); //Bit Rate requested
        double BRmin = call->GetBitRate()*(1-parameters->GetBeta());
        //getting topology number of links
        unsigned int L = topology->GetNumLinks();
        //getting number of links=hops of each route
        double NL0 = (call->GetMultiCalls().at(0)->GetRoute()->GetNumHops());
        double NL1 = (call->GetMultiCalls().at(1)->GetRoute()->GetNumHops());
        double NL2 = (call->GetMultiCalls().at(2)->GetRoute()->GetNumHops());
        double NLT = NL0+NL1+NL2;
        //getting beta resulting from failure of each route
        double betaR0 = 0;   //beta result due route 0 failure
        double betaR1 = 0;
        double betaR2 = 0;
        if(BRmin <= BR1+BR2 < BRR)
            betaR0 = (1 - ((BR1 + BR2) / BRR));
        else if(BR1+BR2 >= BRR)
            betaR0 = 0;
        if(BRmin <= BR0+BR2 < BRR)
            betaR1 = (1 - ((BR0 + BR2) / BRR));
        else if(BR0+BR2 >= BRR)
            betaR1 = 0;
        if(BRmin <= BR0+BR1 < BRR)
            betaR2 = (1 - ((BR1 + BR2) / BRR));
        else if(BR0+BR1 >= BRR)
            betaR2 = 0;
        //getting failure probability of each route
        double P0 = (1-pow(1-(1/L),NL0)); //failure probability of route 0
        double P1 = (1-pow(1-(1/L),NL1));
        double P2 = (1-pow(1-(1/L),NL1));

        //callBetaAverage = (betaR0*(NL0/NLT)) + (betaR1*(NL1/NLT)) + (betaR2*(NL2/NLT));

        callBetaAverage = ((betaR0*P0) + (betaR1*P1) + (betaR2*P2))/(P0 + P1 + P2);

        this->callBetaAverage.push_back(callBetaAverage);
        resDevAlloc->simulType->GetData()->SetSumCallsBetaAverage(callBetaAverage);
    }
    
    if(call->GetMultiCallVec().size() == 2){
        callBetaAverage = parameters->GetBeta();
        this->callBetaAverage.push_back(callBetaAverage);
        resDevAlloc->simulType->GetData()->SetSumCallsBetaAverage(callBetaAverage);
    }
}

void ProtectionScheme::CalcAlpha(CallDevices *call) {
    double callAlpha = 0;
    double BRT = 0;                     //Sum of routes bit rates
    double BRR = call->GetBitRate();    //bit rate requested

    for(auto it : call->GetMultiCallVec()){
        BRT += it->GetBitRate();
    }
    callAlpha = (BRT - BRR) / BRR;

    resDevAlloc->simulType->GetData()->SetSumCallsAlpha(callAlpha);
}


