/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkMaximumEntropyCorrespondenceSampler.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef _MaximumEntropyCorrespondenceSampler_txx
#define _MaximumEntropyCorrespondenceSampler_txx


//using namespace itk;


template <class TImage>
MaximumEntropyCorrespondenceSampler<TImage>::MaximumEntropyCorrespondenceSampler()
{
  m_LinkingFunction = ParticleDualVectorFunction<Dimension>::New();
  m_EnsembleEntropyFunction = ParticleEnsembleEntropyFunction<Dimension>::New();
  m_EnsembleRegressionEntropyFunction = ParticleEnsembleEntropyFunction<Dimension>::New();
  m_EnsembleMixedEffectsEntropyFunction = ParticleEnsembleEntropyFunction<Dimension>::New();
  m_MeshBasedGeneralEntropyGradientFunction = ParticleMeshBasedGeneralEntropyGradientFunction<Dimension>::New();

  m_ShapeMatrix = ParticleShapeMatrixAttribute<double, Dimension>::New();
  m_GeneralShapeMatrix = ParticleGeneralShapeMatrix<double, Dimension>::New();
  m_GeneralShapeGradMatrix = ParticleGeneralShapeGradientMatrix<double, Dimension>::New();

  m_LinearRegressionShapeMatrix = ParticleShapeLinearRegressionMatrixAttribute<double, Dimension>::New();
  m_MixedEffectsShapeMatrix = ParticleShapeMixedEffectsMatrixAttribute<double, Dimension>::New();

  m_EnsembleEntropyFunction->SetShapeMatrix(m_ShapeMatrix);
  
  m_EnsembleRegressionEntropyFunction->SetShapeMatrix(m_LinearRegressionShapeMatrix);
  m_EnsembleMixedEffectsEntropyFunction->SetShapeMatrix(m_MixedEffectsShapeMatrix);

  m_MeshBasedGeneralEntropyGradientFunction->SetShapeData(m_GeneralShapeMatrix);
  m_MeshBasedGeneralEntropyGradientFunction->SetShapeGradient(m_GeneralShapeGradMatrix);

  Superclass::m_ParticleSystem->RegisterAttribute(m_ShapeMatrix);
  Superclass::m_ParticleSystem->RegisterAttribute(m_LinearRegressionShapeMatrix);
  Superclass::m_ParticleSystem->RegisterAttribute(m_MixedEffectsShapeMatrix);

  m_CorrespondenceMode = 1;
}

template<class TImage>
void
MaximumEntropyCorrespondenceSampler<TImage>::AllocateDataCaches()
{
  Superclass::AllocateDataCaches();
  //   m_CurvatureEnsembleMeanFunction->SetMeanCurvatureCache(Superclass::m_MeanCurvatureCache);
}

template <class TImage>
void
MaximumEntropyCorrespondenceSampler<TImage>::GenerateData()
{    
  this->SetInPlace(false); // this is required so that we don't release our inputs
  if (this->GetInitialized() == false)
    {
    this->AllocateDataCaches();
    this->SetAdaptivityMode(Superclass::m_AdaptivityMode);
    this->SetCorrespondenceMode(m_CorrespondenceMode);
    this->GetOptimizer()->SetGradientFunction(m_LinkingFunction);
    m_LinkingFunction->SetAOn();
    m_LinkingFunction->SetBOn();

    this->AllocateDomainsAndNeighborhoods();

    // Point the optimizer to the particle system.
    this->GetOptimizer()->SetParticleSystem(this->GetParticleSystem());
    this->ReadTransforms();
    this->ReadPointsFiles();
    this->InitializeOptimizationFunctions();    


    this->SetInitialized(true);
    }

  if (this->GetInitializing() == true) return;

  this->GetOptimizer()->StartOptimization();
}
template <class TImage>
void
MaximumEntropyCorrespondenceSampler<TImage>::InitializeOptimizationFunctions()
{
  Superclass::InitializeOptimizationFunctions();
  m_LinearRegressionShapeMatrix->Initialize();
  m_MixedEffectsShapeMatrix->Initialize();
  m_ShapeMatrix->Initialize();

  m_GeneralShapeMatrix->Initialize();
  m_GeneralShapeGradMatrix->Initialize();

}



#endif
