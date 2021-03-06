# -*- coding: utf-8 -*-
# Home made test
KX= 1000 # Spring constant
KY= 2000 # Spring constant
FX= 1 # Magnitude of force
FY= 2 
FZ= 3

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Problem type
# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(1,1,1)

    
# Define materials
kx= typical_materials.defElasticMaterial(preprocessor, "kx",KX)
ky= typical_materials.defElasticMaterial(preprocessor, "ky",KY)


fixedNode, newElement= modelSpace.setBearingOnXYRigZ(nod.tag,["kx","ky"])

# Constraints
constraints= preprocessor.getBoundaryCondHandler

#
spc= constraints.newSPConstraint(nod.tag,3,0.0) # nod1 Rx= 0,Ry= 0 and Rz= 0
spc= constraints.newSPConstraint(nod.tag,4,0.0)
spc= constraints.newSPConstraint(nod.tag,5,0.0)


# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(nod.tag,xc.Vector([FX,FY,FZ,0,0,0]))
#We add the load case to domain.
lPatterns.addToDomain("0")


# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)


nodes.calculateNodalReactions(False,1e-7)

deltax= nod.getDisp[0]
deltay= nod.getDisp[1]
deltaz= nod.getDisp[2] 
RX= fixedNode.getReaction[0]
RY= fixedNode.getReaction[1] 
RZ= fixedNode.getReaction[2] 


ratio1= (FX+RX)/FX
ratio2= (FX-KX*deltax)/FX
ratio3= (FY+RY)/FY
ratio4= (FY-KY*deltay)/FY
ratio5= (FZ+RZ)/FZ
ratio6= deltaz

''' 
print "RX= ",RX
print "dx= ",deltax
print "RY= ",RY
print "dy= ",deltay
print "RZ= ",RZ
print "dz= ",deltaz
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
print "ratio5= ",(ratio5)
print "ratio6= ",(ratio6)
   '''
  
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5)  & (abs(ratio5)<1e-5) & (abs(ratio6)<1e-5) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
