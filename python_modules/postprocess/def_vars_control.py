# -*- coding: utf-8 -*-
'''
   THIS PROPERTIES MUST BE REPLACED BY THE CLASSES 
   DEFINED IN control_vars.py 
   THIS FILE MUST DISSAPEAR.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from miscUtils import LogMessages as lmsg
import xc_base
import geom
import xc



def defVarControlMov(obj, code):
  if(not obj.hasProp('span')):
    lmsg.warning('span property not defined for: '+str(obj.tag) + ' object.')
  obj.setProp(code+'Max',0.0)
  obj.setProp('Comb'+code+'Max',"")
  obj.setProp(code+'Min',0.0)
  obj.setProp('Comb'+code+'Min',"")

def defVarsControlMovs(nodes,flags):
  tags= []
  for n in nodes:
    tags.append(n.tag)
    for f in flags:
      defVarControlMov(n,f)
  return tags

# Deprecated.
# def defVarsControlMovU(nodes):
#   return defVarsControlMovs(nodes,{'U'})

# def defVarsControlMovV(nodes):
#   return defVarsControlMovs(nodes,{'V'})

# def defVarsControlMovW(nodes):
#   return defVarsControlMovs(nodes,{'W'})

# def defVarsControlMovUV(nodes):
#   return defVarsControlMovs(nodes,{'U','V'})
# def defVarsControlMovUVW(nodes):
#   return defVarsControlMovs(nodes,{'U','V','W'})

# def defVarsControlMovUVWRXRYRZ(nodes):
#   return defVarsControlMovs(nodes,{'U','V','W','RX','RY','RZ'})

def defVarsControlMovModulus(nodes):
  tags= []
  for n in nodes:
    if(not n.hasProp('span')):
      lmsg.warning('span property not defined for node: '+str(n.tag) + '.')
    tags.append(n.tag)
    n.setProp("dispMax",0.0)
    n.setProp("CombDispMax","")
  return tags


def defVarsControlTensRegElastico2d(elems):
  for e in elems:
    e.setProp("Sg",0)
    e.setProp("SgMax",0)
    e.setProp("SgMin",0)
    e.setProp("NCP",0)
    e.setProp("MzCP",0)
    e.setProp("FCTN",0)
    e.setProp("FCTNCP",0)
    e.setProp("HIPCPTN","")
    e.setProp("Tau",0)
    e.setProp("TauMax",0)
    e.setProp("VyCP",0)
    e.setProp("FCV",0)
    e.setProp("FCVCP",0)
    e.setProp("HIPCPV","")

def defVarsControlTensRegElastico3d(elems):
  for e in elems:
    e.setProp("Sg",0)
    e.setProp("SgMax",0)
    e.setProp("SgMin",0)
    e.setProp("NCP",0)
    e.setProp("MyCP",0)
    e.setProp("MzCP",0)
    e.setProp("FCTN",0)
    e.setProp("FCTNCP",0)
    e.setProp("HIPCPTN","")
    e.setProp("Tau",0)
    e.setProp("TauMax",0)
    e.setProp("VyCP",0)
    e.setProp("VzCP",0)
    e.setProp("FCV",0)
    e.setProp("FCVCP",0)
    e.setProp("HIPCPV","")

def defVarsEnvelopeInternalForcesBeamElems(elems):
  '''Defines properties to store extreme values of internal forces.'''
  for e in elems:
    # [back node value, front node value]
    e.setProp('N+',[-6.023e23,-6.023e23]) #Positive axial force envelope
    e.setProp('N-',[6.023e23,6.023e23]) #Negative axial force envelope
    e.setProp('Mz+',[-6.023e23,-6.023e23]) #Positive bending moment envelope
    e.setProp('Mz-',[6.023e23,6.023e23]) #Negative bending moment envelope
    e.setProp('My+',[-6.023e23,-6.023e23]) #Positive bending moment envelope
    e.setProp('My-',[6.023e23,6.023e23]) #Negative bending moment envelope
    e.setProp('Vy+',[-6.023e23,-6.023e23]) #Positive y shear envelope
    e.setProp('Vy-',[6.023e23,6.023e23]) #Negative y shear  envelope
    e.setProp('Vz+',[-6.023e23,-6.023e23]) #Positive y shear envelope
    e.setProp('Vz-',[6.023e23,6.023e23]) #Negative y shear  envelope
    e.setProp('T+',[-6.023e23,-6.023e23]) #Positive torque envelope
    e.setProp('T-',[6.023e23,6.023e23]) #Negative torque envelope

def updateEnvelopeInternalForcesBeamElem2D(beamElem2D):
  '''Update values for extreme values of internal forces in 2D elements.'''
  beamElem2D.getResistingForce()
  N1= beamElem2D.getN1
  M1= beamElem2D.getM1
  V1= beamElem2D.getV1
  N2= beamElem2D.getN2
  M2= beamElem2D.getM2
  V2= beamElem2D.getV2
  maxN= beamElem2D.getProp('N+') # [back node value, front node value]
  maxM= beamElem2D.getProp('Mz+')
  maxV= beamElem2D.getProp('Vy+')
  minN= beamElem2D.getProp('N-')
  minM= beamElem2D.getProp('Mz-')
  minV= beamElem2D.getProp('Vy-')
  if(N1>maxN[0]):
    maxN[0]= N1
  if(N1<minN[0]):
    minN[0]= N1
  if(M1>maxM[0]):
    maxM[0]= M1
  if(M1<minM[0]):
    minM[0]= M1
  if(V1>maxV[0]):
    maxV[0]= V1
  if(V1<minV[0]):
    minV[0]= V1
  if(N2>maxN[1]):
    maxN[1]= N2
  if(N2<minN[1]):
    minN[1]= N2
  if(M2>maxM[1]):
    maxM[1]= M2
  if(M2<minM[1]):
    minM[1]= M2
  if(V2>maxV[1]):
    maxV[1]= V2
  if(V2<minV[1]):
    minV[1]= V2
  beamElem2D.setProp('N+',maxN)
  beamElem2D.setProp('Mz+',maxM)
  beamElem2D.setProp('Vy+',maxV)
  beamElem2D.setProp('N-',minN)
  beamElem2D.setProp('Mz-',minM)
  beamElem2D.setProp('Vy-',minV)


def updateEnvelopeInternalForcesBeamElem(beamElem):
  '''Update values for extreme values of internal forces.'''
  beamElem.getResistingForce()
  N1= beamElem.getN1
  My1= beamElem.getMy1
  Mz1= beamElem.getMz1
  Vy1= beamElem.getVy1
  N2= beamElem.getN2
  My2= beamElem.getMy2
  Mz2= beamElem.getMz2
  Vy2= beamElem.getVy2
  maxN= beamElem.getProp('N+') # [back node value, front node value]
  maxMy= beamElem.getProp('My+')
  maxMz= beamElem.getProp('Mz+')
  maxVy= beamElem.getProp('Vy+')
  maxVz= beamElem.getProp('Vz+')
  minT= beamElem.getProp('T+')
  minN= beamElem.getProp('N-')
  minMy= beamElem.getProp('My-')
  minMz= beamElem.getProp('Mz-')
  minVy= beamElem.getProp('Vy-')
  minVz= beamElem.getProp('Vz-')
  minT= beamElem.getProp('T-')
  if(N1>maxN[0]):
    maxN[0]= N1
  if(N1<minN[0]):
    minN[0]= N1
  if(My1>maxMy[0]):
    maxMy[0]= My1
  if(My1<minMy[0]):
    minMy[0]= My1
  if(Mz1>maxMz[0]):
    maxMz[0]= Mz1
  if(Mz1<minMz[0]):
    minMz[0]= Mz1
  if(Vy1>maxVy[0]):
    maxVy[0]= Vy1
  if(Vy1<minVy[0]):
    minVy[0]= Vy1
  if(N2>maxN[1]):
    maxN[1]= N2
  if(N2<minN[1]):
    minN[1]= N2
  if(My2>maxMy[1]):
    maxMy[1]= My2
  if(My2<minMy[1]):
    minMy[1]= My2
  if(Mz2>maxMz[1]):
    maxMz[1]= Mz2
  if(Mz2<minMz[1]):
    minMz[1]= Mz2
  if(Vy2>maxVy[1]):
    maxVy[1]= Vy2
  if(Vy2<minVy[1]):
    minVy[1]= Vy2
  beamElem.setProp('N+',maxN)
  beamElem.setProp('My+',maxMy)
  beamElem.setProp('Mz+',maxMz)
  beamElem.setProp('Vy+',maxVy)
  beamElem.setProp('Vz+',maxVz)
  beamElem.setProp('T+',minT)
  beamElem.setProp('N-',minN)
  beamElem.setProp('My-',minMy)
  beamElem.setProp('Mz-',minMz)
  beamElem.setProp('Vy-',minVy)
  beamElem.setProp('Vz-',minVz)
  beamElem.setProp('T-',minT)
