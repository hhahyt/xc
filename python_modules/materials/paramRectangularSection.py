# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (A_OO) "
__copyright__= "Copyright 2016, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com "

import math
import scipy.interpolate
import sys
from materials import typical_materials
from materials import sectionProperties as sp
import geom

# Puntos que definen el valor de alpha en función de h/b
#   ver libro hormigón Jiménez Montoya 14a. edición página 405
x= [1,1.25,1.5,2,3,4,6,10,10000]
y= [0.14,0.171,0.196,0.229,0.263,0.281,0.299,0.313,1.0/3.0]
tablaAlphaTorsion= scipy.interpolate.interp1d(x,y)


def getAlphaTorsion(b,h):
  ''':returns: alpha coefficient of a rectangular sections, as a function of  h/b
  Reference: concrete book Jiménez Montoya 14a. edition page 405

  :param b:            cross-section width (parallel to local z-axis)
  :param h:            cross-section height (parallel to local y-axis)
  '''
  retval= 0.0
  if b<h:
    retval= tablaAlphaTorsion(h/b)
  else:
    retval= tablaAlphaTorsion(b/h)
  return retval

# Puntos que definen el valor de beta en función de h/b
#   ver libro hormigón Jiménez Montoya 14a. edición página 405
x= [1,1.25,1.5,2,3,4,6,8,10,10000]
y= [0.208, 0.221, 0.231, 0.246, 0.267, 0.282, 0.299, 0.307, 0.313, 1.0/3]
tablaBetaTorsion=  scipy.interpolate.interp1d(x,y)

def getBetaTorsion( b, h):
  ''':returns: beta coefficient of a rectangular sections, as a function of  h/b
  Reference: concrete book Jiménez Montoya 14a. edition page 405

  :param b:            cross-section width (parallel to local z-axis)
  :param h:            cross-section height (parallel to local y-axis)
  '''
  retval= 0.0
  if b<h:
    retval= tablaBetaTorsion(h/b)
  else:
    retval= tablaBetaTorsion(b/h)
  return retval

def getJTorsion( b, h):
  ''':returns: torsional moment of inertia of a rectangular section
  Reference: concrete book Jiménez Montoya 14a. edition page 405

  :param b:            cross-section width (parallel to local z-axis)
  :param h:            cross-section height (parallel to local y-axis)
  '''
  alphaJT= getAlphaTorsion(b,h)
  retval= 0.0
  if b<h:
    retval= alphaJT*pow(b,3)*h
  else:
    retval= alphaJT*b*pow(b,3)
  return retval

class RectangularSection(sp.sectionProperties):
  '''Rectangular section geometric parameters
  
  :ivar name:         name identifying the section
  :ivar b:            cross-section width (parallel to local z-axis)
  :ivar h:            cross-section height (parallel to local y-axis)
  '''
  def __init__(self,name,b,h):
    super(RectangularSection,self).__init__(name)
    self.b= b
    self.h= h
  def A(self):
    '''Return cross-sectional area of the section'''
    return self.b*self.h
  def Iy(self):
    '''Return second moment of area about the local y-axis'''
    return 1/12.0*self.h*self.b**3
  def iy(self):
    '''Return the radious of gyration of the section around
       the axis parallel to Z that passes through section centroid.
    '''
    return math.sqrt(self.Iy()/self.A())
  def Iz(self):
    '''Return second moment of area about the local z-axis'''
    return 1/12.0*self.b*self.h**3
  def iz(self):
    '''Return the radious of gyration of the section around
       the axis parallel to Z that passes through section centroid.
    '''
    return math.sqrt(self.Iz()/self.A())
  def J(self):
    '''Return torsional moment of inertia of the section'''
    return getJTorsion(self.b,self.h)
  def Wyel(self):
    '''Return section modulus with respect to local y-axis'''
    return self.Iy()/(self.b/2.0)
  def Wzel(self):
    '''Return section modulus with respect to local z-axis'''
    return self.Iz()/(self.h/2.0)
  def alphaY(self):
    '''Return shear shape factor with respect to local y-axis'''
    return 5.0/6.0 #Coeficiente de distorsión, ver libro E. Oñate pág. 122.
  def alphaZ(self):
    '''Return shear shape factor with respect to local z-axis'''
    return self.alphaY()
  def getYieldMomentY(self,fy):
    '''Return section yield moment.

       :param fy: material yield stress.
    '''
    return 2*fy/self.b*self.Iy()
  def getPlasticSectionModulusY(self):
    '''Returns the plastic section modulus.

       Computes the plastic section modulus assuming that plastic neutral 
       axis passes through section centroid (which is true whenever the 
       rectangular section is homogeneous).
    '''
    return (self.b*self.h/2.0)*self.b/4.0
  def getPlasticMomentY(self,fy):
    '''Return section plastic moment.

       Computes the plastic moment of the section assuming that plastic
       neutral axis passes through section centroid (which is true
       whenever the rectangular section is homogeneous).
    '''
    return 2*self.getPlasticSectionModulusY()*fy
  def getYieldMomentZ(self,fy):
    '''Return section yield moment.

       :param fy: material yield stress.
    '''
    return 2*fy/self.h*self.Iz()
  def getPlasticSectionModulusZ(self):
    '''Returns the plastic section modulus.

       Computes the plastic section modulus assuming that plastic neutral 
       axis passes through section centroid (which is true whenever the 
       rectangular section is homogeneous).
    '''
    return (self.b*self.h/2.0)*self.h/4.0
  def getPlasticMomentZ(self,fy):
    '''Return section plastic moment.

       Computes the plastic moment of the section assuming that plastic
       neutral axis passes through section centroid (which is true
       whenever the rectangular section is homogeneous).
    '''
    return 2*self.getPlasticSectionModulusZ()*fy
  def getRegion(self,gm,nmbMat):
    '''generation of a quadrilateral region from the section 
    geometry (sizes and number of divisions for the cells)
    made of the specified material
   
    :param gm: object of type section_geometry
    :param nmbMat: name of the material (string)
    '''
    regiones= gm.getRegions
    reg= regiones.newQuadRegion(nmbMat)
    reg.nDivIJ= self.nDivIJ
    reg.nDivJK= self.nDivJK
    reg.pMin= geom.Pos2d(-self.h/2.0,-self.b/2.0)
    reg.pMax= geom.Pos2d(self.h/2.0,self.b/2.0)
    return reg
