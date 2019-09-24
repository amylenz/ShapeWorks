/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleRegionDomain.h,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef _ParticleRegionDomain_h
#define _ParticleRegionDomain_h

#include <itkDataObject.h>
#include "ParticleDomain.h"
#include <itkPoint.h>
#include <itkWeakPointer.h>
#include <itkExceptionObject.h>


/** \class ParticleRegionDomain
 *  \brief 
 */
template <unsigned int VDimension=3>
class ParticleRegionDomain : public ParticleDomain<VDimension>
{
public:
  /** Standard class typedefs */
  typedef ParticleRegionDomain Self;
  typedef ParticleDomain<VDimension> Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleRegionDomain, ParticleDomain);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Point type used to store particle locations. */
  typedef itk::Point<double, VDimension> PointType;

  /** Apply any constraints to the given point location.  This method may, for
      example, implement boundary conditions or restrict points to lie on a
      surface.  This class will throw an exception if the point is outside of
      the region.  To specify other behaviors, create a subclass and override
      this method. */
  virtual bool ApplyConstraints(PointType &p) const
  {
    bool flagged = false;
    for (unsigned int i = 0; i < VDimension; i++)
      {
            if ( p[i] < m_LowerBound[i])
              {
              flagged = true;
              p[i] = m_LowerBound[i];
              }
            else if ( p[i] > m_UpperBound[i] )
              {
              flagged = true;
              p[i] = m_UpperBound[i];
              }
      }
        return flagged;
//    return false;
  }

  /** Set the lower/upper bound of the bounded region. */
  itkSetMacro(LowerBound, PointType);
  itkSetMacro(UpperBound, PointType);
  virtual const PointType &GetUpperBound() const
  { return m_UpperBound; }
  virtual const PointType &GetLowerBound() const
  { return m_LowerBound; }
  
  /** Specify the lower and upper bounds (1st and 2nd parameters, respectively)
      of the region. */
  void SetRegion(const PointType &l, const PointType &u)
  {
    this->SetLowerBound(l);
    this->SetUpperBound(u);
  }
  
protected:
  ParticleRegionDomain() {}
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << "LowerBound = " << m_LowerBound << std::endl;
    os << "UpperBound = " << m_UpperBound << std::endl;
  }
  virtual ~ParticleRegionDomain() {};
  
private:
  ParticleRegionDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  PointType m_LowerBound;
  PointType m_UpperBound;

};




#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleRegionDomain+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "ParticleRegionDomain.txx"
#endif

#endif
