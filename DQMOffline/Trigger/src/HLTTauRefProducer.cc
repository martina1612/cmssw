#include "DQMOffline/Trigger/interface/HLTTauRefProducer.h"
#include "TLorentzVector.h"
// TAU includes
#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/TauReco/interface/PFTauDiscriminator.h"
#include "DataFormats/TauReco/interface/CaloTau.h"
#include "DataFormats/TauReco/interface/CaloTauDiscriminatorByIsolation.h"
// ELECTRON includes
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronIDAssociation.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronID.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/TrackReco/interface/Track.h"
// MUON includes
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "TLorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
//CaloTower includes
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/CaloTowers/interface/CaloTowerDefs.h"
#include "Math/GenVector/VectorUtil.h"

using namespace edm;
using namespace reco;
using namespace std;

HLTTauRefProducer::HLTTauRefProducer(const edm::ParameterSet& iConfig)
{
  //One Parameter Set per Collection
  {
    auto const& pfTau = iConfig.getUntrackedParameter<edm::ParameterSet>("PFTaus");
    PFTaus_ = consumes<reco::PFTauCollection>(pfTau.getUntrackedParameter<InputTag>("PFTauProducer"));
    auto discs = pfTau.getUntrackedParameter<vector<InputTag>>("PFTauDiscriminators");
    for (edm::InputTag& tag: discs) {
      PFTauDis_.push_back(consumes<reco::PFTauDiscriminator>(tag));
    }
    doPFTaus_ = pfTau.getUntrackedParameter<bool>("doPFTaus",false);
    ptMinPFTau_= pfTau.getUntrackedParameter<double>("ptMin",15.);
  }

  {
    auto const& electrons = iConfig.getUntrackedParameter<edm::ParameterSet>("Electrons");
    Electrons_ = consumes<reco::GsfElectronCollection>(electrons.getUntrackedParameter<InputTag>("ElectronCollection"));
    doElectrons_ = electrons.getUntrackedParameter<bool>("doElectrons",false);
    e_doID_ = electrons.getUntrackedParameter<bool>("doID",false);
    if(e_doID_) {
      e_idAssocProd_ = consumes<reco::ElectronIDAssociationCollection>(electrons.getUntrackedParameter<InputTag>("IdCollection"));
    }
    e_ctfTrackCollectionSrc_ = electrons.getUntrackedParameter<InputTag>("TrackCollection");
    e_ctfTrackCollection_ = consumes<reco::TrackCollection>(e_ctfTrackCollectionSrc_);
    ptMinElectron_= electrons.getUntrackedParameter<double>("ptMin",15.);
    e_doTrackIso_ = electrons.getUntrackedParameter<bool>("doTrackIso",false);
    e_trackMinPt_= electrons.getUntrackedParameter<double>("ptMinTrack",1.5);
    e_lipCut_= electrons.getUntrackedParameter<double>("lipMinTrack",1.5);
    e_minIsoDR_= electrons.getUntrackedParameter<double>("InnerConeDR",0.02);
    e_maxIsoDR_= electrons.getUntrackedParameter<double>("OuterConeDR",0.6);
    e_isoMaxSumPt_= electrons.getUntrackedParameter<double>("MaxIsoVar",0.02);
  }

  {
    auto const& muons = iConfig.getUntrackedParameter<edm::ParameterSet>("Muons");
    Muons_ = consumes<reco::MuonCollection>(muons.getUntrackedParameter<InputTag>("MuonCollection"));
    doMuons_ = muons.getUntrackedParameter<bool>("doMuons",false);
    ptMinMuon_= muons.getUntrackedParameter<double>("ptMin",15.);
  }

  {
    auto const& jets = iConfig.getUntrackedParameter<edm::ParameterSet>("Jets");
    Jets_ = consumes<reco::CaloJetCollection>(jets.getUntrackedParameter<InputTag>("JetCollection"));
    doJets_ = jets.getUntrackedParameter<bool>("doJets");
    ptMinJet_= jets.getUntrackedParameter<double>("etMin");
  }

  {
    auto const& towers = iConfig.getUntrackedParameter<edm::ParameterSet>("Towers");
    Towers_ = consumes<CaloTowerCollection>(towers.getUntrackedParameter<InputTag>("TowerCollection"));
    doTowers_ = towers.getUntrackedParameter<bool>("doTowers");
    ptMinTower_= towers.getUntrackedParameter<double>("etMin");
    towerIsol_= towers.getUntrackedParameter<double>("towerIsolation");
  }

  {
    auto const& photons = iConfig.getUntrackedParameter<edm::ParameterSet>("Photons");
    Photons_ = consumes<reco::PhotonCollection>(photons.getUntrackedParameter<InputTag>("PhotonCollection"));
    doPhotons_ = photons.getUntrackedParameter<bool>("doPhotons");
    ptMinPhoton_= photons.getUntrackedParameter<double>("etMin");
    photonEcalIso_= photons.getUntrackedParameter<double>("ECALIso");
  }

  {
    auto const& met = iConfig.getUntrackedParameter<edm::ParameterSet>("MET");
    MET_ = consumes<reco::CaloMETCollection>(met.getUntrackedParameter<InputTag>("METCollection"));
    doMET_ = met.getUntrackedParameter<bool>("doMET",false);
    ptMinMET_= met.getUntrackedParameter<double>("ptMin",15.);
  }

  etaMax_ = iConfig.getUntrackedParameter<double>("EtaMax",2.5);

  //recoCollections
  produces<LorentzVectorCollection>("PFTaus");
  produces<LorentzVectorCollection>("Electrons");
  produces<LorentzVectorCollection>("Muons");
  produces<LorentzVectorCollection>("Jets");
  produces<LorentzVectorCollection>("Photons");
  produces<LorentzVectorCollection>("Towers");
  produces<LorentzVectorCollection>("MET");

}

void HLTTauRefProducer::produce(edm::StreamID, edm::Event& iEvent, edm::EventSetup const&) const
{
  if(doPFTaus_)
    doPFTaus(iEvent);
  if(doElectrons_)
    doElectrons(iEvent);
  if(doMuons_)
    doMuons(iEvent);
  if(doJets_)
    doJets(iEvent);
  if(doPhotons_)
    doPhotons(iEvent);
  if(doTowers_)
    doTowers(iEvent);
  if(doMET_)
    doMET(iEvent);
}

void
HLTTauRefProducer::doPFTaus(edm::Event& iEvent) const
{
  auto product_PFTaus = make_unique<LorentzVectorCollection>();

  edm::Handle<PFTauCollection> pftaus;
  if (iEvent.getByToken(PFTaus_,pftaus)) {
    for (unsigned int i=0; i<pftaus->size(); ++i) {
      auto const& pftau = (*pftaus)[i];
      if (pftau.pt()>ptMinPFTau_&&fabs(pftau.eta())<etaMax_) {
        reco::PFTauRef thePFTau {pftaus,i};
        bool passAll {true};
        for (edm::EDGetTokenT<reco::PFTauDiscriminator> const& token: PFTauDis_) {
          edm::Handle<reco::PFTauDiscriminator> pftaudis;
          if (iEvent.getByToken(token, pftaudis)) {
            if ((*pftaudis)[thePFTau] < 0.5) {
              passAll = false;
              break;
            }
          }
          else{
            passAll = false;
            break;
          }
        }
        if (passAll) {
          product_PFTaus->emplace_back(pftau.px(), pftau.py(), pftau.pz(), pftau.energy());
        }
      }
    }
  }
  iEvent.put(move(product_PFTaus),"PFTaus");
}


void
HLTTauRefProducer::doElectrons(edm::Event& iEvent) const
{
  auto product_Electrons = make_unique<LorentzVectorCollection>();

  edm::Handle<reco::ElectronIDAssociationCollection> pEleID;
  bool doID {e_doID_};
  if (doID) {//UGLY HACK UNTIL GET ELETRON ID WORKING IN 210
    if (!iEvent.getByToken(e_idAssocProd_, pEleID)) {
      edm::LogInfo("") << "Error! Can't get electronIDAssocProducer by label. ";
      doID = false;
    }
  }

  edm::Handle<reco::TrackCollection> pCtfTracks;
  if (!iEvent.getByToken(e_ctfTrackCollection_, pCtfTracks)) {
    edm::LogInfo("") << "Error! Can't get " << e_ctfTrackCollectionSrc_.label() << " by label. ";
    iEvent.put(move(product_Electrons),"Electrons");
    return;
  }

  edm::Handle<GsfElectronCollection> electrons;
  if (iEvent.getByToken(Electrons_,electrons)) {
    for (size_t i=0 ; i<electrons->size(); ++i) {
      edm::Ref<reco::GsfElectronCollection> electronRef (electrons,i);
      bool idDec {false};
      if (doID) {
        reco::ElectronIDAssociationCollection::const_iterator tagIDAssocItr;
        tagIDAssocItr = pEleID->find(electronRef);
        const reco::ElectronIDRef& id_tag = tagIDAssocItr->val;
        idDec=id_tag->cutBasedDecision();
      }
      else {
        idDec=true;
      }
      auto const& electron = (*electrons)[i];
      if (electron.pt()>ptMinElectron_&&fabs(electron.eta())<etaMax_&&idDec) {
        if (e_doTrackIso_) {
          double sum_of_pt_ele {};
          for (auto const& tr : *pCtfTracks) {
            double const lip {electron.gsfTrack()->dz() - tr.dz()};
            if (tr.pt() > e_trackMinPt_ && fabs(lip) < e_lipCut_) {
              double dphi {fabs(tr.phi()-electron.trackMomentumAtVtx().phi())};
              if (dphi>acos(-1.)) {
                dphi=2*acos(-1.)-dphi;
              }
              double const deta {fabs(tr.eta()-electron.trackMomentumAtVtx().eta())};
              double const dr_ctf_ele {sqrt(deta*deta+dphi*dphi)};
              if((dr_ctf_ele>e_minIsoDR_) && (dr_ctf_ele<e_maxIsoDR_)){
                double const cft_pt_2 {tr.pt()*tr.pt()};
                sum_of_pt_ele += cft_pt_2;
              }
            }
          }
          double const isolation_value_ele {sum_of_pt_ele/(electron.trackMomentumAtVtx().Rho()*electron.trackMomentumAtVtx().Rho())};
          if (isolation_value_ele<e_isoMaxSumPt_) {
            product_Electrons->emplace_back(electron.px(),electron.py(),electron.pz(),electron.energy());
          }
        }
        else {
          product_Electrons->emplace_back(electron.px(),electron.py(),electron.pz(),electron.energy());
        }
      }
    }
  }
  iEvent.put(move(product_Electrons),"Electrons");
}

void
HLTTauRefProducer::doMuons(edm::Event& iEvent) const
{
  auto product_Muons = make_unique<LorentzVectorCollection>();

  edm::Handle<MuonCollection> muons;
  if (iEvent.getByToken(Muons_,muons)) {
    for (auto const& muon : *muons) {
      if (muon.pt()>ptMinMuon_ && fabs(muon.eta())<etaMax_) {
        product_Muons->emplace_back(muon.px(),muon.py(),muon.pz(),muon.energy());
      }
    }
  }
  iEvent.put(move(product_Muons),"Muons");
}


void
HLTTauRefProducer::doJets(edm::Event& iEvent) const
{
  auto product_Jets = make_unique<LorentzVectorCollection>();

  edm::Handle<CaloJetCollection> jets;
  if (iEvent.getByToken(Jets_,jets)) {
    for (auto const& jet : *jets) {
      if (jet.et()>ptMinJet_ && fabs(jet.eta())<etaMax_) {
        product_Jets->emplace_back(jet.px(),jet.py(),jet.pz(),jet.energy());
      }
    }
  }
  iEvent.put(move(product_Jets),"Jets");
}

void
HLTTauRefProducer::doTowers(edm::Event& iEvent) const
{
  auto product_Towers = make_unique<LorentzVectorCollection>();

  edm::Handle<CaloTowerCollection> towers;
  if (iEvent.getByToken(Towers_,towers)) {
    for (auto const& tower1 : *towers) {
      if (tower1.pt()>ptMinTower_ && fabs(tower1.eta())<etaMax_) {
        //calculate isolation
        double isolET {};
        for (auto const& tower2 : *towers) {
          if (ROOT::Math::VectorUtil::DeltaR(tower1.p4(),tower2.p4())<0.5) {
            isolET+=tower2.pt();
          }
          isolET-=tower1.pt();
        }
        if (isolET<towerIsol_) {
          product_Towers->emplace_back(tower1.px(),tower1.py(),tower1.pz(),tower1.energy());
        }
      }
    }
  }
  iEvent.put(move(product_Towers),"Towers");
}


void
HLTTauRefProducer::doPhotons(edm::Event& iEvent) const
{
  auto product_Gammas = make_unique<LorentzVectorCollection>();

  edm::Handle<PhotonCollection> photons;
  if (iEvent.getByToken(Photons_,photons)) {
    for (auto const& photon : *photons) {
      if (photon.ecalRecHitSumEtConeDR04()<photonEcalIso_ &&
          photon.et()>ptMinPhoton_ && fabs(photon.eta())<etaMax_) {
        product_Gammas->emplace_back(photon.px(),photon.py(),photon.pz(),photon.energy());
      }
    }
  }
  iEvent.put(move(product_Gammas),"Photons");
}

void
HLTTauRefProducer::doMET(edm::Event& iEvent) const
{
  auto product_MET = make_unique<LorentzVectorCollection>();

  edm::Handle<reco::CaloMETCollection> met;
  if(iEvent.getByToken(MET_,met) && !met->empty()){
    auto const& metMom = met->front().p4();
    product_MET->emplace_back(metMom.Px(), metMom.Py(), 0, metMom.Pt());
  }
  iEvent.put(move(product_MET),"MET");
}
