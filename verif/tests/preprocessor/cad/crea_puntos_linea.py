# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# nverborrea= 0
NumDiv= 4
CooMax= NumDiv
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor

puntos= preprocessor.getCad.getPoints
pt1= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt2= puntos.newPntIDPos3d(2,geom.Pos3d(CooMax,CooMax,CooMax))
puntos.defaultTag= 100

lineas= preprocessor.getCad.getLines
lineas.defaultTag= 1
l1= lineas.newLine(1,2)
l1.nDiv= NumDiv
l1.divide()

cumple= True
pos= puntos.get(101).getPos 
cumple= (abs(pos.x-1.0)<1e-5) & (cumple) 
cumple= (abs(pos.y-1.0)<1e-5) & (cumple) 
cumple= (abs(pos.z-1.0)<1e-5) & (cumple) 
pos= puntos.get(102).getPos 
cumple= (abs(pos.x-2.0)<1e-5) & (cumple) 
cumple= (abs(pos.y-2.0)<1e-5) & (cumple) 
cumple= (abs(pos.z-2.0)<1e-5) & (cumple) 
pos= puntos.get(103).getPos 
cumple= (abs(pos.x-3.0)<1e-5) & (cumple) 
cumple= (abs(pos.y-3.0)<1e-5) & (cumple) 
cumple= (abs(pos.z-3.0)<1e-5) & (cumple) 
pos= puntos.get(104).getPos 
cumple= (abs(pos.x-4.0)<1e-5) & (cumple) 
cumple= (abs(pos.y-4.0)<1e-5) & (cumple) 
cumple= (abs(pos.z-4.0)<1e-5) & (cumple) 


import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if cumple:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
