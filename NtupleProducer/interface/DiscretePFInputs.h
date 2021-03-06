#ifndef FASTPUPPI_NTUPLERPRODUCER_DISCRETEPFINPUTS_H
#define FASTPUPPI_NTUPLERPRODUCER_DISCRETEPFINPUTS_H

#include <cstdint>
#include <cstdlib>
#include <cmath>

namespace l1tpf_int { 

  struct CaloCluster {
      int16_t  hwPt;   
      int16_t  hwEmPt;   
      int16_t  hwPtErr;   
      int16_t  hwEta;   
      int16_t  hwPhi;   
      uint16_t hwFlags;
      bool     isEM, used;
      static constexpr float PT_SCALE = 4.0;     // quantize in units of 0.25 GeV (can be changed)
      static constexpr float ETAPHI_FACTOR = 4;  // size of an ecal crystal in phi in integer units (our choice)
      static constexpr float ETAPHI_SCALE = ETAPHI_FACTOR*(180./M_PI);  // M_PI/180 is the size of an ECal crystal; we make a grid that is 4 times that size
      static constexpr int16_t PHI_WRAP = 360*ETAPHI_FACTOR;            // what is 3.14 in integer
      // sorting
      bool operator<(const CaloCluster &other) const { return hwPt > other.hwPt; }
      // filling from floating point
      void fill(float pt, float emPt, float ptErr, float eta, float phi, bool em, unsigned int flags) {
          hwPt  = round(pt  * CaloCluster::PT_SCALE);
          hwEmPt  = round(emPt  * CaloCluster::PT_SCALE);
          hwPtErr = round(ptErr  * CaloCluster::PT_SCALE);
          hwEta = round(eta * CaloCluster::ETAPHI_SCALE);
          hwPhi = int16_t(round(phi * CaloCluster::ETAPHI_SCALE)) % CaloCluster::PHI_WRAP;
          isEM  = em;
          used = false;
          hwFlags = flags;
      }
      float floatPt() const { return float(hwPt) / CaloCluster::PT_SCALE; }
      float floatEmPt() const { return float(hwEmPt) / CaloCluster::PT_SCALE; }
      float floatPtErr() const { return float(hwPtErr) / CaloCluster::PT_SCALE; }
      static float minFloatPt() { return float(1.0) / CaloCluster::PT_SCALE; }
      float floatEta() const { return float(hwEta) / CaloCluster::ETAPHI_SCALE; }
      float floatPhi() const { return float(hwPhi) / CaloCluster::ETAPHI_SCALE; }
      void  setFloatPt(float pt) { hwPt  = round(pt  * CaloCluster::PT_SCALE); }
      void  setFloatEmPt(float emPt) { hwEmPt  = round(emPt  * CaloCluster::PT_SCALE); }
  };

  // https://twiki.cern.ch/twiki/bin/view/CMS/L1TriggerPhase2InterfaceSpecifications
  struct InputTrack {
      uint16_t hwInvpt;
      int32_t  hwVtxEta;
      int32_t  hwVtxPhi;
      bool     hwCharge;
      int16_t  hwZ0;
      uint16_t hwChi2, hwStubs;
      uint16_t hwFlags;
      static constexpr float INVPT_SCALE   = 2E4;    // 1%/pt @ 100 GeV is 2 bits 
      static constexpr float VTX_PHI_SCALE = 1/2.5E-6; // 5 micro rad is 2 bits
      static constexpr float VTX_ETA_SCALE = 1/1E-5;   // no idea, but assume it's somewhat worse than phi
      static constexpr float Z0_SCALE      = 20;     // 1mm is 2 bits
      static constexpr int32_t VTX_ETA_1p3 = 1.3 * InputTrack::VTX_ETA_SCALE;
      // filling from floating point
      void fillInput(float pt, float eta, float phi, int charge, float dz, unsigned int flags) {
          hwInvpt  = round(1/pt  * InputTrack::INVPT_SCALE);
          hwVtxEta = round(eta * InputTrack::VTX_ETA_SCALE);
          hwVtxPhi = round(phi * InputTrack::VTX_PHI_SCALE);
          hwCharge = (charge > 0);
          hwZ0     = round(dz  * InputTrack::Z0_SCALE);
          hwFlags = flags;
      }
      float floatVtxPt() const { return 1/(float(hwInvpt) / InputTrack::INVPT_SCALE); }
      float floatVtxEta() const { return float(hwVtxEta) / InputTrack::VTX_ETA_SCALE; }
      float floatVtxPhi() const { return float(hwVtxPhi) / InputTrack::VTX_PHI_SCALE; }
      float floatDZ()     const { return float(hwZ0) / InputTrack::Z0_SCALE; }
      int intCharge()     const { return hwCharge ? +1 : -1; }
  };

  struct PropagatedTrack : public InputTrack {
      int16_t  hwPt;
      int16_t  hwPtErr;
      int16_t  hwCaloPtErr;
      int16_t  hwEta; // at calo
      int16_t  hwPhi; // at calo
      bool     muonLink;
      bool     used; // note: this flag is not used in the default PF, but is used in alternative algos
      bool     fromPV;
      // sorting
      bool operator<(const PropagatedTrack &other) const { return hwPt > other.hwPt; }
      void fillPropagated(float pt, float ptErr, float caloPtErr, float eta, float phi, unsigned int flags) {
          hwPt  = round(pt  * CaloCluster::PT_SCALE);
          hwPtErr = round(ptErr  * CaloCluster::PT_SCALE);
          hwCaloPtErr = round(caloPtErr  * CaloCluster::PT_SCALE);
          hwEta = round(eta * CaloCluster::ETAPHI_SCALE);
          hwPhi = int16_t(round(phi * CaloCluster::ETAPHI_SCALE)) % CaloCluster::PHI_WRAP;
          muonLink = false;
          used = false;
      }
      float floatPt() const { return float(hwPt) / CaloCluster::PT_SCALE; }
      float floatPtErr() const { return float(hwPtErr) / CaloCluster::PT_SCALE; }
      float floatCaloPtErr() const { return float(hwCaloPtErr) / CaloCluster::PT_SCALE; }
      float floatEta() const { return float(hwEta) / CaloCluster::ETAPHI_SCALE; }
      float floatPhi() const { return float(hwPhi) / CaloCluster::ETAPHI_SCALE; }
  };

  struct Muon {
      int16_t hwPt;   
      int16_t  hwEta;   // at calo
      int16_t  hwPhi;   // at calo
      uint16_t hwFlags;
      bool     hwCharge;
      // sorting
      bool operator<(const Muon &other) const { return hwPt > other.hwPt; }
      void fill(float pt, float eta, float phi, int charge, unsigned int flags) {
          // we assume we use the same discrete ieta, iphi grid for all particles 
          hwPt  = round(pt  * CaloCluster::PT_SCALE);
          hwEta = round(eta * CaloCluster::ETAPHI_SCALE);
          hwPhi = int16_t(round(phi * CaloCluster::ETAPHI_SCALE)) % CaloCluster::PHI_WRAP;
          hwCharge = (charge > 0);
          hwFlags = flags;
      }
      float floatPt() const { return float(hwPt) / CaloCluster::PT_SCALE; }
      float floatEta() const { return float(hwEta) / CaloCluster::ETAPHI_SCALE; }
      float floatPhi() const { return float(hwPhi) / CaloCluster::ETAPHI_SCALE; }
      int intCharge()     const { return hwCharge ? +1 : -1; }
  };

  struct PFParticle {
      int16_t         hwPt;   
      int16_t         hwEta;  // at calo face 
      int16_t         hwPhi;   
      uint8_t         hwId; // CH=0, EL=1, NH=2, GAMMA=3, MU=4 
      int16_t         hwVtxEta;  // propagate back to Vtx for charged particles (if useful?)
      int16_t         hwVtxPhi;   
      uint16_t        hwFlags;
      CaloCluster     cluster;
      PropagatedTrack track;
      bool            chargedPV;
      uint16_t        hwPuppiWeight;
      uint16_t        hwStatus; // for debugging
      static constexpr float PUPPI_SCALE = 100;
      // sorting
      bool operator<(const PFParticle &other) const { return hwPt > other.hwPt; }
      float floatPt() const { return float(hwPt) / CaloCluster::PT_SCALE; }
      float floatEta() const { return float(hwEta) / CaloCluster::ETAPHI_SCALE; }
      float floatPhi() const { return float(hwPhi) / CaloCluster::ETAPHI_SCALE; }
      float floatVtxEta() const { return (track.hwPt > 0 ? track.floatVtxEta() : float(hwVtxEta) / CaloCluster::ETAPHI_SCALE); }
      float floatVtxPhi() const { return (track.hwPt > 0 ? track.floatVtxPhi() : float(hwVtxPhi) / CaloCluster::ETAPHI_SCALE); }
      float floatDZ() const { return float(track.hwZ0) / InputTrack::Z0_SCALE; }
      float floatPuppiW() const { return float(hwPuppiWeight) / PUPPI_SCALE; }
      int intCharge()     const { return (track.hwPt > 0 ? track.intCharge() : 0); }
      void setPuppiW(float w) {
            hwPuppiWeight = std::round(w * PUPPI_SCALE);
      }
      void  setFloatPt(float pt) { hwPt  = round(pt  * CaloCluster::PT_SCALE); }
  };

} // namespace
#endif
