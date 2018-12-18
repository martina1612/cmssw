#ifndef DataFormats_ME0RecHit_H
#define DataFormats_ME0RecHit_H

/** \class ME0RecHit
 *
 *  RecHit for ME0 
 *
 *  \author M. Maggi -- INFN Bari 
 */

#include "DataFormats/TrackingRecHit/interface/RecHit2DLocalPos.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"


class ME0RecHit : public RecHit2DLocalPos {
 public:

  ME0RecHit(const ME0DetId& me0Id,
	    int bx);

  /// Default constructor
  ME0RecHit();

  /// Constructor from a local position, me0Id and digi time.
  /// The 3-dimensional local error is defined as
  /// resolution (the cell resolution) for the coordinate being measured
  /// and 0 for the two other coordinates
  ME0RecHit(const ME0DetId& me0Id,
	    int bx,
	    const LocalPoint& pos);
  

  /// Constructor from a local position and error, me0Id and bx.
  ME0RecHit(const ME0DetId& me0Id,
	    int bx,
	    const LocalPoint& pos,
	    const LocalError& err);
  

  /// Constructor from a local position and error, me0Id, bx, frist strip of cluster and cluster size.
  ME0RecHit(const ME0DetId& me0Id,
	    int bx,
	    int firstStrip,
	    int clustSize,
	    const LocalPoint& pos,
	    const LocalError& err);
  
  /// Destructor
  virtual ~ME0RecHit();


  /// Return the 3-dimensional local position
  virtual LocalPoint localPosition() const {
    return theLocalPosition;
  }


  /// Return the 3-dimensional error on the local position
  virtual LocalError localPositionError() const {
    return theLocalError;
  }


  virtual ME0RecHit* clone() const;

  
  /// Access to component RecHits.
  /// No components rechits: it returns a null vector
  virtual std::vector<const TrackingRecHit*> recHits() const;


  /// Non-const access to component RecHits.
  /// No components rechits: it returns a null vector
  virtual std::vector<TrackingRecHit*> recHits();


  /// Set local position 
  void setPosition(LocalPoint pos) {
    theLocalPosition = pos;
  }

  
  /// Set local position error
  void setError(LocalError err) {
    theLocalError = err;
  }


  /// Set the local position and its error
  void setPositionAndError(LocalPoint pos, LocalError err) {
    theLocalPosition = pos;
    theLocalError = err;
  }
  

  /// Return the me0Id
  ME0DetId me0Id() const {
    return theME0Id;
  }

  int BunchX() const {
    return theBx;
  }

  int firstClusterStrip() const {
    return theFirstStrip;
  }

  int clusterSize() const {
    return theClusterSize;
  }

  float tof() const {	//added by Martina
    return theBx*25.0;	//
  }			//
  /// Comparison operator, based on the me0Id and the digi time
  bool operator==(const ME0RecHit& hit) const;

 private:
  ME0DetId theME0Id;
  int theBx;
  int theFirstStrip;
  int theClusterSize;
  // Position and error in the Local Ref. Frame of the ME0Layer
  LocalPoint theLocalPosition;
  LocalError theLocalError;

};
#endif

/// The ostream operator
std::ostream& operator<<(std::ostream& os, const ME0RecHit& hit);
