#include <limits>
#include <cmath>

#include "StHFTriplet.h"
#include "StPicoEvent/StPicoPhysicalHelix.h"
#include "StarClassLibrary/SystemOfUnits.h"
#include "StPicoEvent/StPicoTrack.h"
#include "StTypeDefs.h"

ClassImp(StHFTriplet)

// _________________________________________________________
StHFTriplet::StHFTriplet(): mLorentzVector(StLorentzVectorF()), mDecayVertex(TVector3()),
  mPointingAngle(std::numeric_limits<float>::quiet_NaN()), mDecayLength(std::numeric_limits<float>::quiet_NaN()),
  mParticle1Dca(std::numeric_limits<float>::quiet_NaN()), mParticle2Dca(std::numeric_limits<float>::quiet_NaN()), 
  mParticle3Dca(std::numeric_limits<float>::quiet_NaN()),
  mParticle1Idx(std::numeric_limits<unsigned short>::max()), mParticle2Idx(std::numeric_limits<unsigned short>::max()), 
  mParticle3Idx(std::numeric_limits<unsigned short>::max()),
  mDcaDaughters12(std::numeric_limits<float>::max()),  mDcaDaughters23(std::numeric_limits<float>::max()),
  mDcaDaughters31(std::numeric_limits<float>::max()),
  mCosThetaStar(std::numeric_limits<float>::quiet_NaN()),
  mdV0Max(std::numeric_limits<float>::quiet_NaN()){//kvapil
}

// _________________________________________________________
StHFTriplet::StHFTriplet(StHFTriplet const * t) : 
  mLorentzVector(t->mLorentzVector), mDecayVertex(t->mDecayVertex),
  mPointingAngle(t->mPointingAngle), mDecayLength(t->mDecayLength), 
  mParticle1Dca(t->mParticle1Dca), mParticle2Dca(t->mParticle2Dca), mParticle3Dca(t->mParticle3Dca),
  mParticle1Idx(t->mParticle1Idx), mParticle2Idx(t->mParticle2Idx), mParticle3Idx(t->mParticle3Idx),
  mDcaDaughters12(t->mDcaDaughters12),  mDcaDaughters23(t->mDcaDaughters23), mDcaDaughters31(t->mDcaDaughters31), 
  mCosThetaStar(t->mCosThetaStar), mdV0Max(t->mdV0Max){ //kvapil
}
//------------------------------------
StHFTriplet::StHFTriplet(StPicoTrack const * const particle1, StPicoTrack const * const particle2, StPicoTrack const * const particle3,
			 float p1MassHypo, float p2MassHypo, float p3MassHypo,
			 unsigned short const p1Idx, unsigned short const p2Idx, unsigned short const p3Idx,
			 TVector3 const & vtx, float const bField)  :
  mLorentzVector(TLorentzVector()), mDecayVertex(TVector3()),
  mPointingAngle(std::numeric_limits<float>::quiet_NaN()), mDecayLength(std::numeric_limits<float>::quiet_NaN()),
  mParticle1Dca(std::numeric_limits<float>::quiet_NaN()), mParticle2Dca(std::numeric_limits<float>::quiet_NaN()), 
  mParticle3Dca(std::numeric_limits<float>::quiet_NaN()),
  mParticle1Idx(p1Idx), mParticle2Idx(p2Idx),  mParticle3Idx(p3Idx),
  mDcaDaughters12(std::numeric_limits<float>::max()), mDcaDaughters23(std::numeric_limits<float>::max()),  
  mDcaDaughters31(std::numeric_limits<float>::max()),
  mCosThetaStar(std::numeric_limits<float>::min()),mdV0Max(std::numeric_limits<float>::quiet_NaN()) { //kvapil
  // -- Create triplet out of 3 tracks
  //     prefixes code:
  //      p1 means particle 1
  //      p2 means particle 2
  //      p3 means particle 3
  //      pair means particle1-particle2 pair||  particle2-particle3 pair ||  particle1-particle3 pair
  //      triplet particle1-particle2-particle3

  if ((!particle1 || !particle2 || !particle3) || 
      (particle1->id() == particle2->id() || particle1->id() == particle3->id() || particle2->id() == particle3->id())) {
    mParticle1Idx = std::numeric_limits<unsigned short>::max();
    mParticle2Idx = std::numeric_limits<unsigned short>::max();
    mParticle3Idx = std::numeric_limits<unsigned short>::max();
    return;
  }

	//-----------------------------------------------------------------------------------------	

    StPicoPhysicalHelix p1Helix = particle1->helix(bField);
    StPicoPhysicalHelix p2Helix = particle2->helix(bField);
    StPicoPhysicalHelix p3Helix = particle3->helix(bField);

    // move origins of helices to the primary vertex origin - needed lated in the code
    p1Helix.moveOrigin(p1Helix.pathLength(vtx));
    p2Helix.moveOrigin(p2Helix.pathLength(vtx));
    p3Helix.moveOrigin(p3Helix.pathLength(vtx));

    // use straight lines approximation to get point of DCA of kaon-pion pair
    TVector3 const p1Mom = p1Helix.momentum(bField * kilogauss);
    TVector3 const p2Mom = p2Helix.momentum(bField * kilogauss);
    TVector3 const p3Mom = p3Helix.momentum(bField * kilogauss);

    StPicoPhysicalHelix const p1StraightLine(p1Mom, p1Helix.origin(), 0, particle1->charge());
    StPicoPhysicalHelix const p2StraightLine(p2Mom, p2Helix.origin(), 0, particle2->charge());
    StPicoPhysicalHelix const p3StraightLine(p3Mom, p3Helix.origin(), 0, particle3->charge());


//-----------------------------------------------------------------------------------------
  
  pair<double, double> const ss12 = p1StraightLine.pathLengths(p2StraightLine);
    TVector3 const p1AtDcaToP2 = p1StraightLine.at(ss12.first);
    TVector3 const p2AtDcaToP1 = p2StraightLine.at(ss12.second);

  pair<double, double> const ss23 = p2StraightLine.pathLengths(p3StraightLine);
    TVector3 const p2AtDcaToP3 = p2StraightLine.at(ss23.first);
    TVector3 const p3AtDcaToP2 = p3StraightLine.at(ss23.second);
  
  pair<double, double> const ss31 = p3StraightLine.pathLengths(p1StraightLine);
    TVector3 const p3AtDcaToP1 = p3StraightLine.at(ss31.first);
    TVector3 const p1AtDcaToP3 = p1StraightLine.at(ss31.second);
  
  // -- calculate DCA of particle1 to particl2 at their DCA
  mDcaDaughters12 = (p1AtDcaToP2 - p2AtDcaToP1).Mag();
  
  // -- calculate DCA of particle2 to particl3 at their DCA
  mDcaDaughters23 = (p2AtDcaToP3 - p3AtDcaToP2).Mag();
  
  // -- calculate DCA of particle3 to particle1 at their DCA
  mDcaDaughters31 = (p3AtDcaToP1 - p1AtDcaToP3).Mag();
  
  // -- calculate decay vertex (secondary)
    TVector3 mDecayVertex = ( p1AtDcaToP2 + p2AtDcaToP1 + p2AtDcaToP3 + p3AtDcaToP2 + p3AtDcaToP1 + p1AtDcaToP3 ) / 6.0;
  
  //kvapil begin
  // Distance between v12 and v23
  float const v12 = (p1AtDcaToP2 + p2AtDcaToP1 - p2AtDcaToP3 - p3AtDcaToP2).Mag()/2.0;
  // Distance between v23 and v31
  float const v23 = (p2AtDcaToP3 + p3AtDcaToP2 - p3AtDcaToP1 - p1AtDcaToP3).Mag()/2.0;
  // Distance between v31 and v12
  float const v31 = (p3AtDcaToP1 + p1AtDcaToP3 - p1AtDcaToP2 - p2AtDcaToP1).Mag()/2.0;

  //maximum dist between reo v0's to be averaging
  float const max12 =  v12 > v23 ? v12 : v23 ;
  mdV0Max = max12 > v31 ? max12 : v31;

  //kvapil end


  // -- constructing mother daughter four momentum. Need helix (not straight line) for each daughter
  double const p1AtV0 = p1Helix.pathLength( mDecayVertex );
    TVector3 const p1MomAtDca = p1Helix.momentumAt(p1AtV0 ,  bField * kilogauss);

  double const p2AtV0 = p2Helix.pathLength( mDecayVertex );
    TVector3 const p2MomAtDca = p2Helix.momentumAt(p2AtV0 ,  bField * kilogauss);
  
  double const p3AtV0 = p3Helix.pathLength( mDecayVertex );
    TVector3 const p3MomAtDca = p3Helix.momentumAt(p3AtV0 ,  bField * kilogauss);

    TLorentzVector const p1FourMom(p1MomAtDca, sqrt(p1MomAtDca*p1MomAtDca+p1MassHypo*p1MassHypo));
    TLorentzVector const p2FourMom(p2MomAtDca, sqrt(p2MomAtDca*p2MomAtDca+p2MassHypo*p2MassHypo));
    TLorentzVector const p3FourMom(p3MomAtDca, sqrt(p3MomAtDca*p3MomAtDca+p3MassHypo*p3MassHypo));


    mLorentzVector = p1FourMom + p2FourMom + p3FourMom;
   
  // -- calculate cosThetaStar
  //    ->> Lomnitz: Need to rethink theta star
    TLorentzVector tripletFourMomReverse;
    tripletFourMomReverse.SetPxPyPzE(-mLorentzVector.Px(), -mLorentzVector.Py(), -mLorentzVector.Pz(), mLorentzVector.E());

    TLorentzVector p1FourMomStar = p1FourMom;
    TLorentzVector p2FourMomStar = p2FourMom;
    TLorentzVector p3FourMomStar = p3FourMom;

    TVector3 beta = tripletFourMomReverse.BoostVector();
    p1FourMomStar.Boost(beta);
    p2FourMomStar.Boost(beta);
    p3FourMomStar.Boost(beta);

    
    mCosThetaStar = cos(p2FourMomStar.Vect().Angle(mLorentzVector.Vect()));




  // -- calculate pointing angle and decay length
    TVector3 const vtxToV0 = mDecayVertex - vtx;
  mPointingAngle = vtxToV0.Angle(mLorentzVector.Vect());
  mDecayLength = vtxToV0.Mag();

 
  // --- calculate DCA of tracks to primary vertex
  mParticle1Dca = (p1Helix.origin() - vtx).Mag();
  mParticle2Dca = (p2Helix.origin() - vtx).Mag();
  mParticle3Dca = (p3Helix.origin() - vtx).Mag();

}

