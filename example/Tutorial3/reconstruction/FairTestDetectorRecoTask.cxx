#include "FairTestDetectorRecoTask.h"

#include "FairTestDetectorHit.h"
#include "FairTestDetectorDigi.h"

#include "FairRootManager.h"

#include "TMath.h"

#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
FairTestDetectorRecoTask::FairTestDetectorRecoTask()
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
FairTestDetectorRecoTask::FairTestDetectorRecoTask(Int_t verbose)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
FairTestDetectorRecoTask::~FairTestDetectorRecoTask()
{
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus FairTestDetectorRecoTask::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- FairTestDetectorRecoTask::Init: " ///todo replace with logger!
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fDigiArray = (TClonesArray*) ioman->GetObject("FairTestDetectorDigi");
  if (!fDigiArray) {
    std::cout << "-W- FairTestDetectorRecoTask::Init: "
              << "No Point array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("FairTestDetectorHit");
  ioman->Register("FairTestDetectorHit", "FairTestDetector", fHitArray, kTRUE);

  return kSUCCESS;
}


// -----   Public method Exec   --------------------------------------------
void FairTestDetectorRecoTask::Exec(Option_t* opt)
{

  fHitArray->Delete();

  // fill the map

  for(int ipnt = 0; ipnt < fDigiArray->GetEntries(); ipnt++) {
    FairTestDetectorDigi* digi = (FairTestDetectorDigi*) fDigiArray->At(ipnt);
    if(!digi) { continue; }


    Double_t timestamp = digi->GetTimeStamp();
    TVector3 pos(digi->GetX()+0.5, digi->GetY()+0.5, digi->GetZ()+0.5);
    TVector3 dpos(1/TMath::Sqrt(12), 1/TMath::Sqrt(12), 1/TMath::Sqrt(12));

    FairTestDetectorHit* hit = new ((*fHitArray)[ipnt]) FairTestDetectorHit(-1, -1, pos, dpos);
    hit->SetTimeStamp(digi->GetTimeStamp());
    hit->SetTimeStampError(digi->GetTimeStampError());
    hit->SetLink(FairLink("FairTestDetectorDigi", ipnt));
  }
}
// -------------------------------------------------------------------------


ClassImp(FairTestDetectorRecoTask)