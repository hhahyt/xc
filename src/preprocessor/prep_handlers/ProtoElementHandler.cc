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
//ProtoElementHandler.cc

#include "ProtoElementHandler.h"
#include "create_elem.h"

#include "domain/mesh/element/truss_beam_column/truss/CorotTruss.h"
#include "domain/mesh/element/truss_beam_column/truss/CorotTrussSection.h"
#include "domain/mesh/element/truss_beam_column/truss/Truss.h"
#include "domain/mesh/element/truss_beam_column/truss/TrussSection.h"
#include "domain/mesh/element/truss_beam_column/truss/Spring.h"

#include "domain/mesh/element/truss_beam_column/beam2d/beam2d02.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d03.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d04.h"
#include "domain/mesh/element/truss_beam_column/beam3d/beam3d01.h"
#include "domain/mesh/element/truss_beam_column/beam3d/beam3d02.h"
#include "domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges2d.h"
#include "domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges3d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam2d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/UpdatedLagrangianBeam2D.h"

#include "domain/mesh/element/plane/fourNodeQuad/ConstantPressureVolumeQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/EnhancedQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/FourNodeQuad.h"
#include "domain/mesh/element/plane/triangle/Tri31.h"
#include "domain/mesh/element/plane/fourNodeQuad/NineNodeMixedQuad.h"
#include "domain/mesh/element/plane/UP-ucsd/FourNodeQuadUP.h"
#include "domain/mesh/element/plane/UP-ucsd/NineFourNodeQuadUP.h"

#include "domain/mesh/element/volumen/brick/BbarBrick.h"
#include "domain/mesh/element/volumen/brick/Brick.h"
#include "domain/mesh/element/volumen/8nbrick/EightNodeBrick.h"
#include "domain/mesh/element/volumen/20nbrick/TwentyNodeBrick.h"
#include "domain/mesh/element/volumen/20nbrick/Twenty_Node_Brick.h"
#include "domain/mesh/element/volumen/27nbrick/TwentySevenNodeBrick.h"
#include "domain/mesh/element/volumen/TotalLagrangianFD20NodeBrick/TotalLagrangianFD20NodeBrick.h"
#include "domain/mesh/element/volumen/upU/EightNodeBrick_u_p_U.h"
#include "domain/mesh/element/volumen/upU/TwentyNodeBrick_u_p_U.h"
#include "domain/mesh/element/volumen/UP-ucsd/BrickUP.h"
#include "domain/mesh/element/volumen/UP-ucsd/TwentyEightNodeBrickUP.h"

#include "domain/mesh/element/special/joint/BeamColumnJoint2d.h"
#include "domain/mesh/element/special/joint/BeamColumnJoint3d.h"
#include "domain/mesh/element/special/joint/Joint2D.h"
#include "domain/mesh/element/special/joint/Joint3D.h"

#include "domain/mesh/element/plane/shell/ShellMITC4.h"
#include "domain/mesh/element/plane/shell/CorotShellMITC4.h"
#include "domain/mesh/element/plane/shell/ShellNL.h"

#include "domain/mesh/element/zeroLength/ZeroLength.h"
#include "domain/mesh/element/zeroLength/ZeroLengthSection.h"
#include "domain/mesh/element/zeroLength/ZeroLengthContact2D.h"
#include "domain/mesh/element/zeroLength/ZeroLengthContact3D.h"

#include "preprocessor/Preprocessor.h"


//! @brief Default constructor.
XC::ProtoElementHandler::ProtoElementHandler(Preprocessor *preprocessor)
  : PrepHandler(preprocessor), nmb_mat("nil"), num_sec(3), dim_elem(0), nmb_transf("nil"), nmb_integ("Lobatto"), dir(0)
 {}

//! @brief Returns a reference to the material handler.
const XC::MaterialHandler &XC::ProtoElementHandler::get_material_handler(void) const
  { return getPreprocessor()->getMaterialHandler(); }

//! @brief Returns an iterator al material que se especifica en nmb_mat.
XC::MaterialHandler::const_iterator XC::ProtoElementHandler::get_iter_material(void) const
  { return getPreprocessor()->getMaterialHandler().find(nmb_mat); }

//! @brief Returns a pointer to material que se especifica en nmb_mat.
const XC::Material *XC::ProtoElementHandler::get_ptr_material(void) const
  {
    Material *retval= nullptr;
    MaterialHandler::const_iterator imat= get_iter_material();
    if(imat!= get_material_handler().end())
      retval= imat->second;
    else 
      std::cerr << "ProtoElementHandler - material: '" 
                << nmb_mat << "' not found.\n";
    return retval;
  }

//! @brief Returns a reference to de integrator handler.
const XC::BeamIntegratorHandler &XC::ProtoElementHandler::get_beam_integrator_handler(void) const
  { return getPreprocessor()->getBeamIntegratorHandler(); }

//! @brief Returns an iterator al integrator que se especifica en nmb_integ.
XC::BeamIntegratorHandler::const_iterator XC::ProtoElementHandler::get_iter_beam_integrator(void) const
  { return getPreprocessor()->getBeamIntegratorHandler().find(nmb_integ); }


//! @brief Returns a pointer to integrator que se especifica en nmb_integ.
const XC::BeamIntegration *XC::ProtoElementHandler::get_ptr_beam_integrator(void) const
  {
    BeamIntegration *retval= nullptr;
    BeamIntegratorHandler::const_iterator iInteg= get_iter_beam_integrator();
    if(iInteg!= get_beam_integrator_handler().end())
      retval= iInteg->second;
    else
      if(verbosity>0)
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; integrator named: '" 
                  << nmb_integ << "' not found.\n";
    return retval;
  }

//! @brief Returns a reference to the coordinate transformation handler.
const XC::TransfCooHandler &XC::ProtoElementHandler::get_transf_coo_handler(void) const
  { return getPreprocessor()->getTransfCooHandler(); }

//! @brief Returns an iterator to the coordinate transformation with the name being passed as parameter.
XC::TransfCooHandler::const_iterator XC::ProtoElementHandler::get_iter_transf_coo(void) const
  { return getPreprocessor()->getTransfCooHandler().find(nmb_transf); }

//! @brief Returns a pointer to the coordinate transformation with the name being passed as parameter (nullptr if not found).
const XC::CrdTransf *XC::ProtoElementHandler::get_ptr_transf_coo(void) const
  {
    CrdTransf *retval= nullptr;
    TransfCooHandler::const_iterator itrf= get_iter_transf_coo();
    if(itrf!= this->get_transf_coo_handler().end())
      retval= itrf->second;
    else
      if(verbosity>0)
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; coordinate transformation named: '" 
                  << nmb_transf << "' not found.\n";
    return retval;
  }

//! @brief Warning messages about deprecated element type names.
void deprecatedElementNameMsg(const std::string &errHeader, const std::string &oldCmd,const std::string &newCmd)
  {
     std::clog << errHeader
               << "; type name: '" << oldCmd
	       << "' is deprecated use '"
               << newCmd << "' instead."
	       << std::endl;
  }

//! @brief Warning messages about materials that are not valid
//! for an element type.
void materialNotSuitableMsg(const std::string &errHeader, const std::string &matName, const std::string &elemType)
  {
    std::cerr << "Error in "
              << errHeader << "; material: '"
              << matName << "' is not suitable for "
	      << elemType << " elements." << std::endl;
  }

//! @brief Process the comands used to define
//! the elements of the finite element model. It interprets
//! the following commands (if the identifier (tag) is not
//! specified a default value will be assigned):
//!
//! - Truss: Defines a truss element (Truss). 
//! - TrussSection: Defines a truss element with section type material.
//! - CorotTruss: Defines a truss element with corotational formulation. 
//! - CorotTrussSection: Defines a truss element with corotational formulation and section type material.
//! - Beam2d_02: Defines a beam-column element (beam2d02) for plane problems. 
//! - Beam2d_03: Defines a beam-column element (beam2d03) for plane problems. 
//! - Beam2d_04: Defines a beam-column element (beam2d04) for plane problems.
//! - Beam3d_01: Defines a beam-column element (beam3d01) for 3D problems.
//! - Beam3d_02: Defines a beam-column element (beam2d02) for 3D problems.
//! - ElasticBeam2d: Defines a beam-column element (ElasticBeam2d) for plane problems.
//! - ElasticBeam3d: Defines a beam-column element (ElasticBeam3d) for 3D problems.
//! - ForceBeamColumn_2d: Defines a beam-column element (ForceBeamColumn2d) no lineal,
//!   for plane problems.
//! - ForceBeamColumn_3d: Defines a beam-column element (ForceBeamColumn3d) no lineal,
//!   for 3D problems.
//! - ShellMITC4: Defines a shell element (ShellMITC4),
//! - ShellNL: Defines a shell element (ShellNL),
//!   for shell problems.
//! - FourNodeQuad: Defines a four node quad (FourNodeQuad),
//! - Tri31: Defines a three node triangle with just a Gauss point (Tri31),
//!   for plane problems.
//! - Brick: Defines an eight node hexahedron (Brick),
//!   para solid analysis.
//! - ZeroLength: Defines a zero length element (ZeroLength).
//! - ZeroLengthSection: Defines a zero length element with section type material (ZeroLengthSection).
XC::Element *XC::ProtoElementHandler::create_element(const std::string &cmd,int tag_elem)
  {
    Element *retval= nullptr;
    const std::string errHeader= getClassName() + "::" + __FUNCTION__;
    if((cmd == "truss")||(cmd=="Truss"))
      {
	if(cmd=="truss")
	  deprecatedElementNameMsg(errHeader,cmd,"Truss");
        retval= new_element_dim_gen_mat<Truss>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((cmd == "truss_section")||(cmd == "TrussSection"))
      {
	if(cmd=="truss_section")
	  deprecatedElementNameMsg(errHeader,cmd,"TrussSection");
        retval=  new_element_dim_gen_mat<TrussSection>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((cmd == "corot_truss")||(cmd == "CorotTruss"))
      {
	if(cmd=="corot_truss")
	  deprecatedElementNameMsg(errHeader,cmd,"CorotTruss");
        retval=  new_element_dim_gen_mat<CorotTruss>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((cmd == "corot_truss_section")||(cmd == "CorotTrussSection"))
      {
	if(cmd=="corot_truss_section")
	  deprecatedElementNameMsg(errHeader,cmd,"CorotTrussSection");
        retval=  new_element_dim_gen_mat<CorotTrussSection>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((cmd == "spring")||(cmd == "Spring"))
      {
	if(cmd=="spring")
	  deprecatedElementNameMsg(errHeader,cmd,"Spring");
        retval=  new_element_dim_gen_mat<Spring>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((cmd == "beam2d_02")||(cmd == "beam2d02"))
      {
	if(cmd=="beam2d_02")
	  deprecatedElementNameMsg(errHeader,cmd,"beam2d02");
        retval=  new_element<beam2d02>(tag_elem);
      }
    else if((cmd == "beam2d_03")||(cmd == "beam2d03"))
      {
	if(cmd=="beam2d_03")
	  deprecatedElementNameMsg(errHeader,cmd,"beam2d03");
        retval=  new_element<beam2d03>(tag_elem);
      }
    else if((cmd == "beam2d_04")||(cmd == "beam2d04"))
      {
	if(cmd=="beam2d_04")
	  deprecatedElementNameMsg(errHeader,cmd,"beam2d04");
        retval=  new_element<beam2d04>(tag_elem);
      }
    else if((cmd == "beam3d_01")||(cmd == "beam3d01"))
      {
	if(cmd=="beam3d_01")
	  deprecatedElementNameMsg(errHeader,cmd,"beam3d01");
        retval=  new_element<beam3d01>(tag_elem);
      }
    else if((cmd == "beam3d_02")||(cmd == "beam3d02"))
      {
	if(cmd=="beam3d_02")
	  deprecatedElementNameMsg(errHeader,cmd,"beam3d02");
        retval=  new_element<beam3d02>(tag_elem);
      }
    else if((cmd == "elastic_beam_2d")||(cmd == "ElasticBeam2d"))
      {
	if(cmd=="elastic_beam_2d")
	  deprecatedElementNameMsg(errHeader,cmd,"ElasticBeam2d");
        retval=  new_element_mat_crd<ElasticBeam2d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if((cmd == "elastic_beam_3d")||(cmd == "ElasticBeam3d"))
      {
	if(cmd=="elastic_beam_3d")
	  deprecatedElementNameMsg(errHeader,cmd,"ElasticBeam3d");
        retval=  new_element_mat_crd<ElasticBeam3d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if((cmd == "beam_with_hinges_2d")||(cmd == "BeamWithHinges2d"))
      {
	if(cmd=="beam_with_hinges_2d")
	  deprecatedElementNameMsg(errHeader,cmd,"BeamWithHinges2d");
        retval=  new_element_gen_mat_crd<BeamWithHinges2d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if((cmd == "beam_with_hinges_3d")||(cmd == "BeamWithHinges3d"))
      {
	if(cmd=="beam_with_hinges_3d")
	  deprecatedElementNameMsg(errHeader,cmd,"BeamWithHinges3d");
        retval=  new_element_gen_mat_crd<BeamWithHinges3d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if((cmd == "disp_beam_column_2d")||(cmd == "DispBeamColumn2d"))
      {
	if(cmd=="disp_beam_column_2d")
	  deprecatedElementNameMsg(errHeader,cmd,"DispBeamColumn2d");
        retval=  new_element_dim_gen_mat_crd<DispBeamColumn2d>(tag_elem, dim_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if((cmd == "disp_beam_column_3d")||(cmd == "DispBeamColumn3d"))
      {
	if(cmd=="disp_beam_column_3d")
	  deprecatedElementNameMsg(errHeader,cmd,"DispBeamColumn3d");
        retval=  new_element_dim_gen_mat_crd<DispBeamColumn3d>(tag_elem, dim_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if((cmd == "nl_beam_column_2d")||(cmd == "NLBeamColumn2d"))
      {
	if(cmd=="nl_beam_column_2d")
	  deprecatedElementNameMsg(errHeader,cmd,"NLBeamColumn2d");
        retval=  new_element_ns_gen_mat_crd<NLBeamColumn2d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo());
      }
    else if((cmd == "nl_beam_column_3d")||(cmd == "NLBeamColumn3d"))
      {
	if(cmd=="nl_beam_column_3d")
	  deprecatedElementNameMsg(errHeader,cmd,"NLBeamColumn3d");
        retval=  new_element_ns_gen_mat_crd<NLBeamColumn3d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo());
      }
    else if((cmd == "force_beam_column_2d")||(cmd == "ForceBeamColumn2d"))
      {
	if(cmd=="force_beam_column_2d")
	  deprecatedElementNameMsg(errHeader,cmd,"ForceBeamColumn2d");
        retval=  new_element_ns_gen_mat_crd_integ<ForceBeamColumn2d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo(),get_ptr_beam_integrator());
      }
    else if((cmd == "force_beam_column_3d")||(cmd == "ForceBeamColumn3d"))
      {
	if(cmd=="force_beam_column_3d")
	  deprecatedElementNameMsg(errHeader,cmd,"ForceBeamColumn3d");
        retval=  new_element_ns_gen_mat_crd_integ<ForceBeamColumn3d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo(),get_ptr_beam_integrator());
      }
    else if((cmd == "zero_length")||(cmd == "ZeroLength"))
      {
	if(cmd=="zero_length")
	  deprecatedElementNameMsg(errHeader,cmd,"ZeroLength");
        retval=  new_element_dim_gen_mat_dir<ZeroLength>(tag_elem, dim_elem, get_ptr_material(),dir);
      }
    else if((cmd == "zero_length_contact_2d")||(cmd == "ZeroLengthContact2D"))
      {
	if(cmd=="zero_length_contact_2d")
	  deprecatedElementNameMsg(errHeader,cmd,"ZeroLengthContact2D");
        retval=  new_element<ZeroLengthContact2D>(tag_elem);
      }
    else if((cmd == "zero_length_contact_3d")||(cmd == "ZeroLengthContact3D"))
      {
	if(cmd=="zero_length_contact_3d")
	  deprecatedElementNameMsg(errHeader,cmd,"ZeroLengthContact3D");
        retval=  new_element<ZeroLengthContact3D>(tag_elem);
      }
    else if((cmd == "zero_length_section")||(cmd == "ZeroLengthSection"))
      {
	if(cmd=="zero_length_section")
	  deprecatedElementNameMsg(errHeader,cmd,"ZeroLengthSection");
        retval=  new_element_dim_gen_mat<ZeroLengthSection>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((cmd == "shell_mitc4")||(cmd == "ShellMITC4"))
      {
	if(cmd=="shell_mitc4")
	  deprecatedElementNameMsg(errHeader,cmd,"ShellMITC4");
        retval= new_element_mat<ShellMITC4,SectionForceDeformation>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,nmb_mat,cmd);
      }
    else if((cmd == "corot_shell_mitc4")||(cmd == "CorotShellMITC4"))
      {
	if(cmd=="corot_shell_mitc4")
	  deprecatedElementNameMsg(errHeader,cmd,"CorotShellMITC4");
        retval= new_element_mat<CorotShellMITC4,SectionForceDeformation>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,nmb_mat,cmd);
      }
    else if((cmd == "shell_nl")||(cmd == "ShellNL"))
      {
	if(cmd=="shell_nl")
	  deprecatedElementNameMsg(errHeader,cmd,"ShellNL");
        retval= new_element_mat<ShellNL,SectionForceDeformation>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,nmb_mat,cmd);
      }
    else if((cmd == "quad4n")||(cmd == "FourNodeQuad"))
      {
	if(cmd=="quad4n")
	  deprecatedElementNameMsg(errHeader,cmd,"FourNodeQuad");
        retval= new_element_mat<FourNodeQuad,NDMaterial>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,nmb_mat,cmd);
      }
    else if((cmd == "tri31")||(cmd == "Tri31"))
      {
	if(cmd=="tri31")
	  deprecatedElementNameMsg(errHeader,cmd,"Tri31");
        retval= new_element_mat<Tri31,NDMaterial>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,nmb_mat,cmd);
      }
    else if((cmd == "brick")||(cmd == "Brick"))
      {
	if(cmd=="brick")
	  deprecatedElementNameMsg(errHeader,cmd,"Brick");
        retval= new_element_mat<Brick,NDMaterial>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,nmb_mat,cmd);
      }
    else
      std::cerr << errHeader
		<< "; element type: " << cmd << " unknown."
		<< std::endl;
    return retval;
  }

//! @brief Create a new element.
//! @param type: type of element. Available types:'Truss','TrussSection','CorotTruss','CorotTrussSection','Spring', 'Beam2d02', 'Beam2d03',  'Beam2d04', 'Beam3d01', 'Beam3d02', 'ElasticBeam2d', 'ElasticBeam3d', 'BeamWithHinges2d', 'BeamWithHinges3d', 'NlBeamColumn2d', 'NlBeamColumn3d','ForceBeamColumn2d', 'ForceBeamColumn3d', 'ShellMitc4', ' shellNl', 'Quad4n', 'Tri31', 'Brick', 'ZeroLength', 'ZeroLengthContact2d', 'ZeroLengthContact3d', 'ZeroLengthSection'.
//! @param iNodes: nodes ID, e.g. xc.ID([1,2]) to create a linear element from node 1 to node 2.
XC::Element *XC::ProtoElementHandler::newElement(const std::string &type,const ID &iNodes)
  {
    const int tag_elem= getDefaultTag();
    Element *retval= getPreprocessor()->getDomain()->getElement(tag_elem);
    if(!retval) //It doesn't already exists.
      {
        retval= create_element(type,tag_elem);
        if(retval)
          {
            retval->setIdNodes(iNodes);
            add(retval);
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; ERROR the element: "
                << tag_elem << " already exists.\n";
    return retval;
  }

//! @brief Sets the default material name for new elements.
void XC::ProtoElementHandler::setDefaultMaterial(const std::string &nmb)
  { nmb_mat= nmb; }

//! @brief Returns the default material name for new elements.
const std::string &XC::ProtoElementHandler::getDefaultMaterial(void) const
  { return nmb_mat; }

//! @brief Default number of sections for new elements.
void XC::ProtoElementHandler::setNumSections(const int &ns)
  { num_sec= ns; }

//! @brief Returns the default number of sections for new elements.
int XC::ProtoElementHandler::getNumSections(void) const
  { return num_sec; }

//! @brief Sets the default dimension (0D,1D,2D or 3D) for new elements.
void XC::ProtoElementHandler::setDimElem(const int &dim)
  { dim_elem= dim; }

//! @brief Returns the default dimension (0D,1D,2D or 3D) for new elements.
int XC::ProtoElementHandler::getDimElem(void) const
  { return dim_elem; }

//! @brief Sets the name of the default coordinate transformation for new elements.
void XC::ProtoElementHandler::setDefaultTransf(const std::string &nmb)
  { nmb_transf= nmb; }

//! @brief Returns the name of the default coordinate transformation for new elements.
const std::string &XC::ProtoElementHandler::getDefaultTransf(void) const
  { return nmb_transf; }

//! @brief Sets the name of the default integrator for new elements.
void XC::ProtoElementHandler::setDefaultIntegrator(const std::string &nmb)
  { nmb_integ= nmb; }

//! @brief Returns the name of the default integrator for new elements.
const std::string &XC::ProtoElementHandler::getDefaultIntegrator(void) const
  { return nmb_integ; }

