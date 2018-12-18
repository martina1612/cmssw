/** \file
 *
 *  \author M. Maggi -- INFN Bari
*/

#include "ME0RecHitProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/GEMDigi/interface/ME0DigiCollection.h"
#include "DataFormats/GEMDigi/interface/ME0PadDigiCollection.h"

#include "Geometry/GEMGeometry/interface/ME0EtaPartition.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"
#include "DataFormats/GEMRecHit/interface/ME0RecHit.h"

#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitBaseAlgo.h"
#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitAlgoFactory.h"
#include "DataFormats/GEMRecHit/interface/ME0RecHitCollection.h"

// #include "CondFormats/GEMObjects/interface/GEMMaskedStrips.h"
// #include "CondFormats/DataRecord/interface/GEMMaskedStripsRcd.h"
// #include "CondFormats/GEMObjects/interface/GEMDeadStrips.h"
// #include "CondFormats/DataRecord/interface/GEMDeadStripsRcd.h"

#include <string>


using namespace edm;
using namespace std;

ME0RecHitProducer::ME0RecHitProducer(const ParameterSet& config){

  // Set verbose output

  produces<ME0RecHitCollection>();

  theME0DigiToken = consumes<ME0PadDigiCollection>(config.getParameter<edm::InputTag>("me0DigiLabel")); //20022018  

  // Get the concrete reconstruction algo from the factory

  string theAlgoName = config.getParameter<string>("recAlgo");
  theAlgo = ME0RecHitAlgoFactory::get()->create(theAlgoName,
						config.getParameter<ParameterSet>("recAlgoConfig"));

  // Get masked- and dead-strip information

  /* GEMMaskedStripsObj = new GEMMaskedStrips();

  GEMDeadStripsObj = new GEMDeadStrips();

  maskSource = config.getParameter<std::string>("maskSource");

  if (maskSource == "File") {
    edm::FileInPath fp = config.getParameter<edm::FileInPath>("maskvecfile");
    std::ifstream inputFile(fp.fullPath().c_str(), std::ios::in);
    if ( !inputFile ) {
      std::cerr << "Masked Strips File cannot not be opened" << std::endl;
      exit(1);
    }
    while ( inputFile.good() ) {
      GEMMaskedStrips::MaskItem Item;
      inputFile >> Item.rawId >> Item.strip;
      if ( inputFile.good() ) MaskVec.push_back(Item);
    }
    inputFile.close();
  }

  deadSource = config.getParameter<std::string>("deadSource");

  if (deadSource == "File") {
    edm::FileInPath fp = config.getParameter<edm::FileInPath>("deadvecfile");
    std::ifstream inputFile(fp.fullPath().c_str(), std::ios::in);
    if ( !inputFile ) {
      std::cerr << "Dead Strips File cannot not be opened" << std::endl;
      exit(1);
    }
    while ( inputFile.good() ) {
      GEMDeadStrips::DeadItem Item;
      inputFile >> Item.rawId >> Item.strip;
      if ( inputFile.good() ) DeadVec.push_back(Item);
    }
    inputFile.close();
  }
  */
}


ME0RecHitProducer::~ME0RecHitProducer(){

  delete theAlgo;
  // delete GEMMaskedStripsObj;
  // delete GEMDeadStripsObj;

}



void ME0RecHitProducer::beginRun(const edm::Run& r, const edm::EventSetup& setup){

  // Getting the masked-strip information
  /*
  if ( maskSource == "EventSetup" ) {
    edm::ESHandle<GEMMaskedStrips> readoutMaskedStrips;
    setup.get<GEMMaskedStripsRcd>().get(readoutMaskedStrips);
    const GEMMaskedStrips* tmp_obj = readoutMaskedStrips.product();
    GEMMaskedStripsObj->MaskVec = tmp_obj->MaskVec;
    delete tmp_obj;
  }
  else if ( maskSource == "File" ) {
    std::vector<GEMMaskedStrips::MaskItem>::iterator posVec;
    for ( posVec = MaskVec.begin(); posVec != MaskVec.end(); ++posVec ) {
      GEMMaskedStrips::MaskItem Item; 
      Item.rawId = (*posVec).rawId;
      Item.strip = (*posVec).strip;
      GEMMaskedStripsObj->MaskVec.push_back(Item);
    }
  }
  */
  // Getting the dead-strip information
  /*
  if ( deadSource == "EventSetup" ) {
    edm::ESHandle<GEMDeadStrips> readoutDeadStrips;
    setup.get<GEMDeadStripsRcd>().get(readoutDeadStrips);
    const GEMDeadStrips* tmp_obj = readoutDeadStrips.product();
    GEMDeadStripsObj->DeadVec = tmp_obj->DeadVec;
    delete tmp_obj;
  }
  else if ( deadSource == "File" ) {
    std::vector<GEMDeadStrips::DeadItem>::iterator posVec;
    for ( posVec = DeadVec.begin(); posVec != DeadVec.end(); ++posVec ) {
      GEMDeadStrips::DeadItem Item;
      Item.rawId = (*posVec).rawId;
      Item.strip = (*posVec).strip;
      GEMDeadStripsObj->DeadVec.push_back(Item);
    }
  }
  */
}



void ME0RecHitProducer::produce(Event& event, const EventSetup& setup) {

  // Get the GEM Geometry

  ESHandle<ME0Geometry> me0Geom;
  setup.get<MuonGeometryRecord>().get(me0Geom);

  // Get the digis from the event

  Handle<ME0PadDigiCollection> digis; // 20022018 
  event.getByToken(theME0DigiToken,digis);

  // Pass the EventSetup to the algo

  theAlgo->setES(setup);

  // Create the pointer to the collection which will store the rechits

  auto recHitCollection = std::make_unique<ME0RecHitCollection>();

  // Iterate through all digi collections ordered by LayerId   

  ME0PadDigiCollection::DigiRangeIterator me0dgIt; // 20022018
  for (me0dgIt = digis->begin(); me0dgIt != digis->end();
       ++me0dgIt){
       
    // The layerId
    const ME0DetId& me0Id = (*me0dgIt).first;
    								
    // Get the GeomDet from the setup
    const ME0EtaPartition* roll = me0Geom->etaPartition(me0Id);

    // Get the iterators over the digis associated with this LayerId
    const ME0PadDigiCollection::Range& range = (*me0dgIt).second; //20022018


    // Getting the roll mask, that includes dead strips, for the given GEMDet
    EtaPartitionMask mask;
    /*
    int rawId = gemId.rawId();
    int Size = GEMMaskedStripsObj->MaskVec.size();
    for (int i = 0; i < Size; i++ ) {
      if ( GEMMaskedStripsObj->MaskVec[i].rawId == rawId ) {
	int bit = GEMMaskedStripsObj->MaskVec[i].strip;
	mask.set(bit-1);
      }
    }

    Size = GEMDeadStripsObj->DeadVec.size();
    for (int i = 0; i < Size; i++ ) {
      if ( GEMDeadStripsObj->DeadVec[i].rawId == rawId ) {
	int bit = GEMDeadStripsObj->DeadVec[i].strip;
	mask.set(bit-1);
      }
    }
    */
    // Call the reconstruction algorithm    

    OwnVector<ME0RecHit> recHits =
      theAlgo->reconstruct(*roll, me0Id, range, mask);
    
    if(recHits.size() > 0) //FIXME: is it really needed?
      recHitCollection->put(me0Id, recHits.begin(), recHits.end());
  }

  event.put(std::move(recHitCollection));

}

