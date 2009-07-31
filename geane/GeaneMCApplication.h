
#ifndef GEANE_MC_APPLICATION_H
#define GEANE_MC_APPLICATION_H

#include "TVirtualMCApplication.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include<iostream>

class GeaneMCApplication : public TVirtualMCApplication
{

  public:
    GeaneMCApplication();

    virtual ~GeaneMCApplication(){;}
    void InitMC();
    /** Construct user geometry */
    virtual void          ConstructGeometry(){
      gGeoManager->CloseGeometry();   // close geometry
      gMC->SetRootGeometry();  
      _abort();
    }
    virtual void          Field(const Double_t* x, Double_t* b) const;
    virtual void          FinishEvent(){_abort();}
    virtual void          FinishPrimary(){_abort();}
    void                  FinishRun(){_abort();}
    virtual void          GeneratePrimaries(){_abort();}
    virtual void          InitGeometry(){_abort();}
    virtual void          PostTrack(){_abort();}
    virtual void          PreTrack(){_abort();}
    virtual void          BeginEvent(){_abort();}
    virtual void          BeginPrimary(){_abort();}
    virtual void          Stepping(){_abort();}
private:
    void _abort(){std::cout << "GeaneMCApplication aborting" << std::endl;}// throw;}
public:
    ClassDef(GeaneMCApplication,1)  //Interface to MonteCarlo application
};

// inline functions


#endif