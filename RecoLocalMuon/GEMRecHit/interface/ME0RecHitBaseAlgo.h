#ifndef RecoLocalMuon_ME0RecHitBaseAlgo_H
#define RecoLocalMuon_ME0RecHitBaseAlgo_H

/** \class ME0RecHitBaseAlgo
 *  Abstract algorithmic class to compute Rec Hit
 *  form a ME0 digi
 *
 *  \author M. Maggi -- INFN Bari
 */


#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/GeometrySurface/interface/LocalError.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GEMDigi/interface/ME0DigiCollection.h"
#include "DataFormats/GEMDigi/interface/ME0PadDigiCollection.h"
#include "DataFormats/GEMRecHit/interface/ME0RecHit.h"
#include "DataFormats/Common/interface/OwnVector.h"

#include "RecoLocalMuon/GEMRecHit/src/GEMEtaPartitionMask.h"
#include "RecoLocalMuon/GEMRecHit/src/GEMMaskReClusterizer.h"

class RecHitCluster;
class ME0EtaPartition;
class ME0DetId;

namespace edm {
  class ParameterSet;
  class EventSetup;
}


class ME0RecHitBaseAlgo {

 public:
  
  /// Constructor
  ME0RecHitBaseAlgo(const edm::ParameterSet& config);

  /// Destructor
  virtual ~ME0RecHitBaseAlgo();  

  /// Pass the Event Setup to the algo at each event
  virtual void setES(const edm::EventSetup& setup) = 0;

  /// Build all hits in the range associated to the gemId, at the 1st step.
  virtual edm::OwnVector<ME0RecHit> reconstruct(const ME0EtaPartition& roll,
						const ME0DetId& gemId,
						const ME0PadDigiCollection::Range& digiRange, //20022018
                                                const EtaPartitionMask& mask);

  /// standard local recHit computation
  virtual bool compute(const ME0EtaPartition& roll,
                       const RecHitCluster& cl,
                       LocalPoint& Point,
                       LocalError& error) const = 0;


  /// local recHit computation accounting for track direction and 
  /// absolute position
  virtual bool compute(const ME0EtaPartition& roll,
		       const RecHitCluster& cl,
                       const float& angle,
                       const GlobalPoint& globPos, 
                       LocalPoint& Point,
                       LocalError& error) const = 0;
};
#endif
