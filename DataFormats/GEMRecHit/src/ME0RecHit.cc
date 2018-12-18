/*
 *  See header file for a description of this class.
 *
 *  \author M. Maggi -- INFN Bari
 */


#include "DataFormats/GEMRecHit/interface/ME0RecHit.h"


ME0RecHit::ME0RecHit(const ME0DetId& me0Id, int bx) :  RecHit2DLocalPos(me0Id),
  theME0Id(me0Id), theBx(bx),theFirstStrip(99),theClusterSize(99), theLocalPosition(), theLocalError() 
{
}

ME0RecHit::ME0RecHit() :  RecHit2DLocalPos(),
  theME0Id(), theBx(99),theFirstStrip(99),theClusterSize(99), theLocalPosition(), theLocalError() 
{
}


ME0RecHit::ME0RecHit(const ME0DetId& me0Id, int bx, const LocalPoint& pos) :  RecHit2DLocalPos(me0Id),
  theME0Id(me0Id), theBx(bx), theFirstStrip(99),theClusterSize(99), theLocalPosition(pos) 
{
  float stripResolution = 3.0 ; //cm  this sould be taken from trimmed cluster size times strip size 
                                 //    taken out from geometry service i.e. topology
  theLocalError =
    LocalError(stripResolution*stripResolution, 0., 0.); //FIXME: is it really needed?
}



// Constructor from a local position and error, wireId and digi time.
ME0RecHit::ME0RecHit(const ME0DetId& me0Id,
		     int bx,
		     const LocalPoint& pos,
		     const LocalError& err) :  RecHit2DLocalPos(me0Id),
  theME0Id(me0Id), theBx(bx),theFirstStrip(99), theClusterSize(99), theLocalPosition(pos), theLocalError(err) 
{
}


// Constructor from a local position and error, wireId, bx and cluster size.
ME0RecHit::ME0RecHit(const ME0DetId& me0Id,
		     int bx,
		     int firstStrip,
		     int clustSize,
		     const LocalPoint& pos,
		     const LocalError& err) :  RecHit2DLocalPos(me0Id),
  theME0Id(me0Id), theBx(bx),theFirstStrip(firstStrip), theClusterSize(clustSize), theLocalPosition(pos), theLocalError(err) 
{
}




// Destructor
ME0RecHit::~ME0RecHit()
{
}



ME0RecHit * ME0RecHit::clone() const {
  return new ME0RecHit(*this);
}


// Access to component RecHits.
// No components rechits: it returns a null vector
std::vector<const TrackingRecHit*> ME0RecHit::recHits() const {
  std::vector<const TrackingRecHit*> nullvector;
  return nullvector; 
}



// Non-const access to component RecHits.
// No components rechits: it returns a null vector
std::vector<TrackingRecHit*> ME0RecHit::recHits() {
  std::vector<TrackingRecHit*> nullvector;
  return nullvector; 
}


// Comparison operator, based on the wireId and the digi time
bool ME0RecHit::operator==(const ME0RecHit& hit) const {
  return this->geographicalId() == hit.geographicalId(); 
}


// The ostream operator
std::ostream& operator<<(std::ostream& os, const ME0RecHit& hit) {
  os << "pos: " << hit.localPosition().x() ; 
  os << " +/- " << sqrt(hit.localPositionError().xx()) ;
  return os;
}
