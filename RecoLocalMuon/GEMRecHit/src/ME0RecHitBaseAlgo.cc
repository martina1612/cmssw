/*
 *  See header file for a description of this class.
 *
 *  \author M. Maggi -- INFN Bari
 */


#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitBaseAlgo.h"
//#include "RecoLocalMuon/GEMRecHit/src/GEMClusterContainer.h"
#include "RecoLocalMuon/GEMRecHit/src/GEMCluster.h"
#include "RecoLocalMuon/GEMRecHit/src/GEMClusterizer.h"
#include "RecoLocalMuon/GEMRecHit/src/GEMMaskReClusterizer.h"

#include "Geometry/GEMGeometry/interface/ME0EtaPartition.h"
#include "DataFormats/GEMDigi/interface/ME0DigiCollection.h"
#include "DataFormats/GEMDigi/interface/ME0PadDigiCollection.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "RecoLocalMuon/GEMRecHit/src/Clusterizer.h"

ME0RecHitBaseAlgo::ME0RecHitBaseAlgo(const edm::ParameterSet& config) {
  //  theSync = GEMTTrigSyncFactory::get()->create(config.getParameter<string>("tTrigMode"),
  //config.getParameter<ParameterSet>("tTrigModeConfig"));
}

ME0RecHitBaseAlgo::~ME0RecHitBaseAlgo(){}


// Build all hits in the range associated to the layerId, at the 1st step.
edm::OwnVector<ME0RecHit> ME0RecHitBaseAlgo::reconstruct(const ME0EtaPartition& roll,
							 const ME0DetId& gemId,
							 const ME0DigiCollection::Range& digiRange, // 20022018
                                                         const EtaPartitionMask& mask) {
  edm::OwnVector<ME0RecHit> result; 

  Clusterizer<ME0DigiCollection::Range> Gclizer; //20022018
  RecHitClusterContainer cls;
  Gclizer.clusterize(digiRange, cls);
//  GEMClusterizer clizer;
//  GEMClusterContainer tcls = clizer.doAction(digiRange);
//  GEMMaskReClusterizer mrclizer;
//  GEMClusterContainer cls = mrclizer.doAction(gemId,tcls,mask);

  for (RecHitClusterContainer::const_iterator cl = cls.begin();
       cl != cls.end(); cl++){
    
    LocalError tmpErr;
    LocalPoint point;
    // Call the compute method
    bool OK = this->compute(roll, *cl, point, tmpErr);
    if (!OK) continue;

    // Build a new pair of 1D rechit 
    int firstClustStrip= cl->firstStrip();
    int clusterSize=cl->clusterSize(); 
    ME0RecHit*  recHit = new ME0RecHit(gemId,cl->bx(),firstClustStrip,clusterSize,point,tmpErr);

    result.push_back(recHit);
  }
  return result;
}
