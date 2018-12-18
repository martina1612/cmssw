#ifndef DataFormats_GEMDigi_ME0PreRecoObject_h
#define DataFormats_GEMDigi_ME0PreRecoObject_h

/** \class ME0PreRecoObject
 *
 * Pre Reco Object for ME0
 *  
 * \author Martina Ressegotti
 *
 */

#include <cstdint>
#include <iosfwd>

class ME0PreRecoObject{

public:
//  explicit ME0PreRecoObject (float x, float y, float ex, float ey, float corr, float tof);
  explicit ME0PreRecoObject (float x, float y, float ex, float ey, float corr, float tof, int pdgid, int prompt);
  ME0PreRecoObject ();

  bool operator==(const ME0PreRecoObject& digi) const;
  bool operator!=(const ME0PreRecoObject& digi) const;
  bool operator<(const ME0PreRecoObject& digi) const;

  float x() const { return x_; }
  float y() const { return y_; }
  float ex() const { return ex_; }
  float ey() const { return ey_; }
  float corr() const { return corr_; }
  float tof() const { return tof_;}
  // coding mc-truth
  int pdgid() const { return pdgid_;}
  int prompt() const { return prompt_;}
  void print() const;

private:
  float x_;
  float y_;
  float ex_;
  float ey_;
  float corr_;
  float tof_;
  // coding mc-truth
  int pdgid_;
  int prompt_;
};

std::ostream & operator<<(std::ostream & o, const ME0PreRecoObject& digi);

#endif

