#ifndef FAIRGEOMATRIX_H
#define FAIRGEOMATRIX_H

#include "TObject.h"                    // for TObject

#include "FairGeoVector.h"              // for FairGeoVector

#include "Rtypes.h"                     // for Double_t, etc

/**
 * @author Ilse koenig
 */
class FairGeoMatrix : public TObject
{
  protected:
    Double_t fM[9];
  public:
    FairGeoMatrix(void);
    ~FairGeoMatrix(void);
    Double_t& operator()(Int_t i,Int_t j) { return fM[i*3+j]; }
    Double_t det(void);
    FairGeoVector operator*(FairGeoVector& v);
    FairGeoMatrix& operator/=(Double_t d);
    ClassDef(FairGeoMatrix,0)
};

#endif