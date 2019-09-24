/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleImageDomainWithGradients.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef _ParticleImageDomainWithGradients_h
#define _ParticleImageDomainWithGradients_h

#include <itkImage.h>
#include <itkImageDuplicator.h>
#include "ParticleImageDomain.h"
#include <itkVectorLinearInterpolateImageFunction.h>
#include <itkGradientImageFilter.h>
#include <itkFixedArray.h>


/** \class ParticleImageDomainWithGradients
 *
 * An image domain that extends ParticleImageDomainWithGradients with image
 * gradient information.  Gradient values are interpolated with the
 * SampleGradients(point) method.
 *
 * \sa ParticleImageDomain
 * \sa ParticleClipRegionDomain
 * \sa ParticleDomain
 */
template <class T, unsigned int VDimension=3>
class ParticleImageDomainWithGradients : public ParticleImageDomain<T, VDimension>
{
public:
  /** Standard class typedefs */
  typedef ParticleImageDomainWithGradients Self;
  typedef ParticleImageDomain<T, VDimension> Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self>  ConstWeakPointer;

    /** Point type of the domain (not necessarily of the image). */
  typedef typename Superclass::PointType PointType;
  
  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::ScalarInterpolatorType ScalarInterpolatorType;
  typedef itk::GradientImageFilter<ImageType> GradientImageFilterType;
  typedef typename GradientImageFilterType::OutputImageType GradientImageType;
  typedef itk::VectorLinearInterpolateImageFunction<GradientImageType, typename PointType::CoordRepType>
  GradientInterpolatorType;

  typedef itk::FixedArray<T, 3> VectorType;
  typedef vnl_vector_fixed<T, 3> VnlVectorType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleImageDomainWithGradients, ParticleImageDomain);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Set/Get the itk::Image specifying the particle domain.  The set method
      modifies the parent class LowerBound and UpperBound. */
  void SetImage(ImageType *I)
  {
    Superclass::SetImage(I);
 
    // Compute gradient image and set up gradient interpolation.
    typename GradientImageFilterType::Pointer filter = GradientImageFilterType::New();
    filter->SetInput(I);
    filter->SetUseImageSpacingOn();
    filter->Update();
    m_GradientImage = filter->GetOutput();
    
    m_GradientInterpolator->SetInputImage(m_GradientImage);
  }
  itkGetObjectMacro(GradientImage, GradientImageType);


  /** Sample the image at a point.  This method performs no bounds checking.
      To check bounds, use IsInsideBuffer.  SampleGradientsVnl returns a vnl
      vector of length VDimension instead of an itk::CovariantVector
      (itk::FixedArray). */
  inline VectorType SampleGradient(const PointType &p) const
  {
      if(this->IsInsideBuffer(p))
        return  m_GradientInterpolator->Evaluate(p);
      else {
          itkExceptionMacro("Gradient queried for a Point, " << p << ", outside the given image domain." );
         VectorType g(1.0e-5);
          return g;
      }
  }
  inline VnlVectorType SampleGradientVnl(const PointType &p) const
  { return VnlVectorType( this->SampleGradient(p).GetDataPointer() ); }
  inline VnlVectorType SampleNormalVnl(const PointType &p, T epsilon = 1.0e-5) const
  {
    VnlVectorType grad = this->SampleGradientVnl(p);
    grad = grad.normalize();
//    double q = 1.0 / (grad.magnitude() + epsilon);
//    for (unsigned int i = 0; i < VDimension; i++) { grad[i] *= q;
        /* PRATEEP */
        // reverse normal directions
//        grad[i] *= -1.0;
//    }
    return grad;
  }
  
  /** Allow public access to the scalar interpolator. */
  itkGetObjectMacro(GradientInterpolator, GradientInterpolatorType);


  /** This method is called by an optimizer after a call to Evaluate and may be
      used to apply any constraints the resulting vector, such as a projection
      to the surface tangent plane. Returns true if the gradient was modified.*/
  virtual bool ApplyVectorConstraints(vnl_vector_fixed<double, VDimension> &gradE,
                                      const PointType &pos,
                                      double maxtimestep) const
    
  {
    if (this->m_ConstraintsEnabled == true)
      {
      const double epsilon = 1.0e-10;
      
      double dotprod = 0.0;  
      VnlVectorType normal =  this->SampleNormalVnl(pos, epsilon);
      for (unsigned int i = 0; i < VDimension; i++) {   dotprod  += normal[i] * gradE[i]; }
      for (unsigned int i = 0; i < VDimension; i++) {   gradE[i] -= normal[i] * dotprod; }
     
      return true;
      }
    else return false;
  }

  /** Used when a domain is fixed. */
  void DeleteImages()
  {
    Superclass::DeleteImages();
    m_GradientImage = 0;
    m_GradientInterpolator = 0;
  }
  
protected:
  ParticleImageDomainWithGradients()
  {
    m_GradientInterpolator = GradientInterpolatorType::New();
  }

  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent << "m_GradientImage = " << m_GradientImage << std::endl;
    os << indent << "m_GradientInterpolator = " << m_GradientInterpolator << std::endl;
  }
  virtual ~ParticleImageDomainWithGradients() {};
  
private:
  ParticleImageDomainWithGradients(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename GradientImageType::Pointer m_GradientImage;
  typename GradientInterpolatorType::Pointer m_GradientInterpolator;
};




#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleImageDomainWithGradients+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "ParticleImageDomainWithGradients.txx"
#endif

#endif
