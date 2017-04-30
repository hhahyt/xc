//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//ComputePivots.h
//Computes the pivots for a bending plane.


#ifndef COMPUTEPIVOTS_H
#define COMPUTEPIVOTS_H

#include "xc_utils/src/geom/sis_ref/Ref3d3d.h"

namespace XC {

class PivotsUltimateStrains;
class DqFibras;
class StoFibras;
class Fiber;

//! @ingroup MATSCCDiagInt
//
//! @brief Given a bending plane, computes the "pivots" position
//! on the section.
class ComputePivots: public Ref3d3d
  {
    const PivotsUltimateStrains &agot_pivots; //!< Ultimate strains at pivots.
    const StoFibras &fibras;//!< Section fibers.
    const DqFibras &fibrasC; //!< Concrete fibers (or another material that works in compression).
    const DqFibras &fibrasS; //!< Steel fibers (or another material that works in tension). 
  public:
    ComputePivots(const PivotsUltimateStrains &ap,const StoFibras &fs,const DqFibras &fsC,const DqFibras &fsS,const double &theta);
    const Fiber *getFiberCMinY(void) const;
    const Fiber *getFiberSMinY(void) const;
    const Fiber *getFiberCMaxY(void) const;
    Pos3d GetPuntoD(void) const;
    Pos3d calcPositionPivotA(void) const;
    Pos3d calcPositionPivotB(void) const;
    Pos3d calcPositionPivotC(void) const;
  };

} // end of XC namespace

#endif