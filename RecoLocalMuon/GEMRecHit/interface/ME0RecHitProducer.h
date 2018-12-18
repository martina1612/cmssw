#ifndef RecoLocalMuon_ME0RecHit_ME0RecHitProducer_h
#define RecoLocalMuon_ME0RecHit_ME0RecHitProducer_h

/** \class ME0RecHitProducer
 *  Module for ME0RecHit production. 
 *  
 *  \author M. Maggim -- INFN Bari
 */


#include <memory>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <cstdlib>
#include <bitset>
#include <map>

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"
#include "DataFormats/GEMDigi/interface/ME0DigiCollection.h"
#include "DataFormats/GEMDigi/interface/ME0PadDigiCollection.h"

// #include "CondFormats/GEMObjects/interface/GEMMaskedStrips.h"
// #include "CondFormats/DataRecord/interface/GEMMaskedStripsRcd.h"
// #include "CondFormats/GEMObjects/interface/GEMDeadStrips.h"
// #include "CondFormats/DataRecord/interface/GEMDeadStripsRcd.h"

//#include "GEMEtaPartitionMask.h"


namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}

class ME0RecHitBaseAlgo;

class ME0RecHitProducer : public edm::stream::EDProducer<> {

public:
  /// Constructor
  ME0RecHitProducer(const edm::ParameterSet& config);

  /// Destructor
  virtual ~ME0RecHitProducer();

  // Method that access the EventSetup for each run
  virtual void beginRun(const edm::Run&, const edm::EventSetup& ) override;

  /// The method which produces the rechits
  virtual void produce(edm::Event& event, const edm::EventSetup& setup) override;

private:

  // The token to be used to retrieve GEM digis from the event
  edm::EDGetTokenT<ME0DigiCollection> theME0DigiToken; // 20022018

  // The reconstruction algorithm
  ME0RecHitBaseAlgo *theAlgo;
  //   static std::string theAlgoName;

  // GEMMaskedStrips* GEMMaskedStripsObj;
  // Object with mask-strips-vector for all the GEM Detectors

  // GEMDeadStrips* GEMDeadStripsObj;
  // Object with dead-strips-vector for all the GEM Detectors

  // std::string maskSource;
  // std::string deadSource;

  // std::vector<GEMMaskedStrips::MaskItem> MaskVec;
  // std::vector<GEMDeadStrips::DeadItem> DeadVec;

};

#endif

