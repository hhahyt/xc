# -*- coding: utf-8 -*-
'''Verification of the quasi-Winkler model.'''
# Verification of the quasi-Winkler model according to
#    the article of Gabriel E. Barrientos
#    «Vigas sobre fundaciones bi-paramétricas del tipo Cuasi-Winker»
#    Departamento de Ingeniería Mecánica. Universidad del Bío-Bío.
#    Casilla 5-C. Concepción. Chile.

from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from model.geometry import geom_utils
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

numDiv= 30
L= 5500
F= 10000
E= 5200
I= 342990
beta= 3*math.pi/L
k= pow(beta,4)*4*E*I
h= pow(12*I,1/3.0)
A= h # Suponemos width unidad

# Problem type
prb= xc.ProblemaEF()
preprocessor=  prb.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Define materials
kY= typical_materials.defElastNoTensMaterial(preprocessor, "kY",k*L/numDiv)
nodes.newSeedNode()
# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)


seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultTransformation= "lin"
seedElemLoader.defaultMaterial= "scc"
seedElemLoader.defaultTag= 1 #Tag for next element.
beam2d= seedElemLoader.newElement("ElasticBeam2d",xc.ID([0,0]))
beam2d.h= h


points= preprocessor.getCad.getPoints
pt1= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPntIDPos3d(2,geom.Pos3d(L/2,0.0,0.0))
pt3= points.newPntIDPos3d(3,geom.Pos3d(L,0.0,0.0))

lines= preprocessor.getCad.getLines
lines.defaultTag= 1
l1= lines.newLine(1,2)
l1.nDiv= int(numDiv/2)


lines.defaultTag= 2
l2= lines.newLine(2,3)
l2.nDiv= int(numDiv/2)



l1.genMesh(xc.meshDir.I)
l2.genMesh(xc.meshDir.I)

fixedNodeId= 0
idCentralNode= 0




# Fix end nodes.
constraints= preprocessor.getConstraintLoader
modelSpace.fixNode00F(pt1.getTagNode)
modelSpace.fixNode00F(pt3.getTagNode)

# Apoyamos los interiores.
l1InteriorNodes= geom_utils.LineWrapper(l1).getInteriorNodes()
for n in l1InteriorNodes:
  fixedNodeId, idElem= modelSpace.setUniaxialBearing2D(n.tag,"kY",[0,1])

l1LastNode= geom_utils.LineWrapper(l1).getLastNode()
idCentralNode= l1LastNode.tag
fixedNodeId, idElem= modelSpace.setUniaxialBearing2D(l1LastNode.tag,"kY",[0,1])

l2InteriorNodes= geom_utils.LineWrapper(l2).getInteriorNodes()
for n in l2InteriorNodes:
  fixedNodeId, idElem= modelSpace.setUniaxialBearing2D(n.tag,"kY",[0,1])


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
lp0.newNodalLoad(idCentralNode,xc.Vector([0,-F,0]))

#We add the load case to domain.
casos.addToDomain("0")



# Solution procedure
solution= predefined_solutions.SolutionProcedure()
solution.convergenceTestTol= 1.0e-2
analysis= solution.simpleNewtonRaphsonBandGen(prb)
predefined_solutions.resuelveComb(preprocessor, "0",analysis,10)

Lmedios= L/2
EIbeta3= F/(E*I*pow(beta,3))

yAdimCentroVano= l1LastNode.getDisp[1]/EIbeta3
yAdimMaxima= -1e6

for n in l2.getNodes():
  yAdimMaxima= max(yAdimMaxima,n.getDisp[1]/EIbeta3)
  ''' 
  print "  node: ",n.tag," x= ",(n.get3dCoo[0]-Lmedios)/L
  print " dy= ",n.getDisp[1]/EIbeta3
  '''


''' Adimensional values for the minimum deflection/span ratio at midspan
and the maximum one near the support was measured on the figure 2 of the
article «Beams on quasi-Winkler foundations» of M. N. Pavlovic 
y S. Tsikkos. '''
yAdimCentroVanoTeor= -0.1254
yAdimMaximaTeor= 0.0229
ratio1= beta*L-3*math.pi
ratio2= pow(beta,4)-k/(4*E*I)
ratio3= abs((yAdimCentroVano-yAdimCentroVanoTeor)/yAdimCentroVanoTeor)
ratio4= abs(yAdimMaxima-yAdimMaximaTeor)/yAdimMaximaTeor

''' 
print "yAdimCentroVanoTeor= ",yAdimCentroVanoTeor
print "yAdimCentroVano= ",yAdimCentroVano
print "yAdimMaximaTeor= ",yAdimMaximaTeor
print "yAdimMaxima= ",yAdimMaxima
print "beta= ",beta*1000,"e-3n"
print "k= ",k*100,"e-2n"
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-15) & (ratio3<0.03) & (ratio4<0.09)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
