#ifndef __REMOLLBEAMTARGET_HH
#define __REMOLLBEAMTARGET_HH

#include "remolltypes.hh"
#include "remollglobs.hh"
#include "remollVertex.hh"

#include "G4ThreeVector.hh"
#include <vector>

/*!
     Class that contains information on 
     the beam and target.  It needs to be
     aware of and consistant with what is
     in the geometry.

     This is responsible for:
         Rastering, arbitrary beam angle
	 Sampling along the target
	 Pre-vertex multiple scattering
	 External radiative effects
	 Luminosity calculations
 
     This is implemented in the singleton model

*/

class G4GenericMessenger;
class G4VPhysicalVolume;
class G4Material;
class remollMultScatt;

class remollBeamTarget {

    private:
        // Static geometry objects
	static G4String fActiveTargetMotherName;
	static G4String fActiveTargetVolumeName;
	static size_t fActiveTargetMother;
	static size_t fActiveTargetVolume;
        static std::vector<std::pair<G4VPhysicalVolume*,G4String>> fTargetMothers;
        static std::vector<std::vector<std::pair<G4VPhysicalVolume*,G4String>>> fTargetVolumes;

        // Effective lengths are weighted by density (i.e. in 1/cm^2)
        static G4double fTotalTargetEffectiveLength;
        static G4double fActiveTargetEffectiveLength;
        // Positions are in physical distances (i.e. in cm)
        static G4double fMotherTargetAbsolutePosition;
        static G4double fActiveTargetRelativePosition;

    public:
        static void UpdateInfo();

    public:
        // Static geometry functions
        static void ResetTargetVolumes() {
          fTargetVolumes.clear();
          fTargetMothers.clear();
        }
        static void AddMotherVolume(G4VPhysicalVolume *v, const G4String& tag) {
          fTargetMothers.push_back(std::make_pair(v,tag));
          fTargetVolumes.resize(fTargetMothers.size());
          fActiveTargetMother = fTargetMothers.size() - 1;
        }
        static void AddTargetVolume(G4VPhysicalVolume *v, const G4String& tag) {
          fTargetVolumes[fActiveTargetMother].push_back(std::make_pair(v,tag));
        }
        static std::vector<std::pair<G4VPhysicalVolume*,G4String>> GetTargetVolumes() {
          return fTargetVolumes[fActiveTargetMother];
        }

        void SetActiveTargetMother(G4String name);
        void SetActiveTargetVolume(G4String name);

        void PrintTargetInfo();

    public:
        remollBeamTarget();
	virtual ~remollBeamTarget();

	G4double GetEffLumin(SamplingType_t);

	remollVertex SampleVertex(SamplingType_t);

	G4double fBeamEnergy;
	G4double fBeamCurrent;
	G4double fBeamPolarization;

	remollMultScatt *fMS;

	bool fAlreadyWarned;
        bool fAlreadyWarned_LH2;

    private:
	G4GenericMessenger* fMessenger;
	G4GenericMessenger* fMessengerTarget;

	G4Material *fDefaultMat;

    public:
	// Base position, angle *sampled* info
	G4ThreeVector fVer, fDir;
	G4double fSampledEnergy;
	G4double fRadiationLength;
	G4double fTravelledLength;
        G4double fEffectiveMaterialLength;

        G4double fEnergyCut;

	// Base position/angle sampling info
        G4bool fOldRaster;
	G4double fRasterX, fRasterY;
	G4double fX0, fY0;
	G4double fTh0, fPh0;
	G4double fdTh, fdPh, fCorrTh, fCorrPh;

    public:
        remollBeamTarget_t GetBeamTargetIO() const {
          remollBeamTarget_t bm;
          bm.x = fVer.x();
          bm.y = fVer.y();
          bm.z = fVer.z();
          bm.dx = fDir.x();
          bm.dy = fDir.y();
          bm.dz = fDir.z();
          bm.th = fDir.theta();
          bm.ph = fDir.phi();
          return bm;
        }
};


#endif//__REMOLLBEAMTARGET_HH

