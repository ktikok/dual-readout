from Gaudi.Configuration import *
from Configurables import ApplicationMgr
from GaudiKernel import SystemOfUnits as units

from Configurables import k4DataSvc
dataservice = k4DataSvc("EventDataSvc")

from Configurables import PythiaInterface
pythia8Tool = PythiaInterface("PythiaInterface",
  pythiacard = "PATH_TO_CARD",
  printPythiaStatistics = True
)

from Configurables import GaussSmearVertex
smearTool = GaussSmearVertex("VertexSmearingTool",
  xVertexSigma = 0.5*units.mm,
  yVertexSigma = 0.5*units.mm,
  zVertexSigma = 20.0*units.mm,
  tVertexSigma = 100.0*units.picosecond
)

from Configurables import HepMCToEDMConverter
hepmc2edm = HepMCToEDMConverter("Converter")

from Configurables import GenAlg
gen = GenAlg("Pythia8", SignalProvider=pythia8Tool, VertexSmearingTool=smearTool)

from Configurables import GenParticleFilter
genfilter = GenParticleFilter("GenParticleFilter")

from Configurables import GeoSvc
geoservice = GeoSvc(
  "GeoSvc",
  detectors = [
    'file:share/compact/DRcalo.xml'
  ]
)

from Configurables import SimG4Svc, SimG4FastSimPhysicsList, SimG4FastSimOpFiberRegion, SimG4OpticalPhysicsList
regionTool = SimG4FastSimOpFiberRegion("fastfiber")
opticalPhysicsTool = SimG4OpticalPhysicsList("opticalPhysics", fullphysics="SimG4FtfpBert")
physicslistTool = SimG4FastSimPhysicsList("Physics", fullphysics=opticalPhysicsTool)

from Configurables import SimG4DRcaloActions
actionTool = SimG4DRcaloActions("SimG4DRcaloActions")

from Configurables import SimG4ConstantMagneticFieldTool
magnetTool = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",
  FieldOn = True,
  FieldComponentZ = 2.* units.tesla,
  IntegratorStepper = "ClassicalRK4",
  MaximumStep = 10000.
)

# Name of the tool in GAUDI is "XX/YY" where XX is the tool class name and YY is the given name
geantservice = SimG4Svc("SimG4Svc",
  physicslist = physicslistTool,
  regions = ["SimG4FastSimOpFiberRegion/fastfiber"],
  actions = actionTool,
  magneticField = magnetTool
)

from Configurables import SimG4Alg, SimG4PrimariesFromEdmTool
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
edmConverter = SimG4PrimariesFromEdmTool("EdmConverter")
edmConverter.GenParticles.Path = "GenParticlesFiltered"

from Configurables import SimG4SaveDRcaloHits, SimG4SaveDRcaloMCTruth, SimG4SaveDRcaloTrajectory
saveDRcaloTool = SimG4SaveDRcaloHits("saveDRcaloTool", readoutNames = ["DRcaloSiPMreadout"])
saveMCTruthTool = SimG4SaveDRcaloMCTruth("saveMCTruthTool") # need SimG4DRcaloActions
saveDRcalTrajTool = SimG4SaveDRcaloTrajectory("saveDRcalTrajTool")

geantsim = SimG4Alg("SimG4Alg",
  outputs = [
    "SimG4SaveDRcaloHits/saveDRcaloTool",
    "SimG4SaveDRcaloMCTruth/saveMCTruthTool",
    "SimG4SaveDRcaloTrajectory/saveDRcalTrajTool"
  ],
  eventProvider = edmConverter
)

from Configurables import PodioOutput
podiooutput = PodioOutput("PodioOutput", filename = "sim.root")
podiooutput.outputCommands = ["keep *"]

from Configurables import RndmGenSvc, HepRndm__Engine_CLHEP__RanluxEngine_
rndmEngine = HepRndm__Engine_CLHEP__RanluxEngine_("RndmGenSvc.Engine",
  SetSingleton = True,
  UseTable = True,
  Column = 0, # 0 or 1
  Row = 123 # 0 to 214
)

rndmGenSvc = RndmGenSvc("RndmGenSvc",
  Engine = rndmEngine.name()
)

ApplicationMgr(
  TopAlg = [gen, hepmc2edm, genfilter, geantsim, podiooutput],
  EvtSel = 'NONE',
  EvtMax = 10,
  # order is important, as GeoSvc is needed by SimG4Svc
  ExtSvc = [rndmEngine, rndmGenSvc, dataservice, geoservice, geantservice]
)
