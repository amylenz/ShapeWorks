/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleImplicitSurfaceDomain.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleImplicitSurfaceDomain_txx
#define __itkParticleImplicitSurfaceDomain_txx

#include "vnl/vnl_math.h"
#include "vnl/vnl_cross.h"
#define PARTICLE_DEBUG_FLAG 1

namespace itk
{

template<class T, unsigned int VDimension>
void
ParticleImplicitSurfaceDomain<T, VDimension>::
SetCuttingPlane(const vnl_vector<double> &a, const vnl_vector<double> &b,
                const vnl_vector<double> &c)
{
  // See http://mathworld.wolfram.com/Plane.html, for example
  vnl_vector<double> q;
  if (VDimension == 3)  q = vnl_cross_3d((b-a),(c-a));
  else if (VDimension == 2)  q = vnl_cross_2d((b-a),(c-a));
  
  if (q.magnitude() > 0.0)
  {
    m_CuttingPlaneNormal.push_back(q / q.magnitude()); //m_CuttingPlaneNormal = q / q.magnitude();
    m_CuttingPlanePoint.push_back(a); //m_CuttingPlanePoint = a;
    m_UseCuttingPlane = true;

    m_a.push_back(a); //m_a = a;
    m_b.push_back(b); //m_b = b;
    m_c.push_back(c); //m_c = c;
  }
}

// Prateep
template<class T, unsigned int VDimension>
void
ParticleImplicitSurfaceDomain<T, VDimension>::
TransformCuttingPlane(const TransformType &Trans, const vnl_vector<double> &base_a,
                      const vnl_vector<double> &base_b, const vnl_vector<double> &base_c )
{
    if (m_UseCuttingPlane == true)
    {
/*
        // Transform m_CuttingPlanePoint
//        vnl_vector_fixed<double, VDimension> point;
//        point[0] = Trans[0][0] * m_CuttingPlanePoint[0] + Trans[0][1] * m_CuttingPlanePoint[1] + Trans[0][2] * m_CuttingPlanePoint[2] + Trans[0][3];
//        point[1] = Trans[1][0] * m_CuttingPlanePoint[0] + Trans[1][1] * m_CuttingPlanePoint[1] + Trans[1][2] * m_CuttingPlanePoint[2] + Trans[1][3];
//        point[2] = Trans[2][0] * m_CuttingPlanePoint[0] + Trans[2][1] * m_CuttingPlanePoint[1] + Trans[2][2] * m_CuttingPlanePoint[2] + Trans[2][3];

//        // Transform m_CuttingPlaneNormal
//        vnl_vector_fixed<double, VDimension> normal;

////        TransformType TransInv = vnl_inverse(Trans);
////        TransformType TransInvT;
////        for (unsigned int i = 0; i <= VDimension; i++) {
////            for (unsigned int j = 0; j <= VDimension; j++) {
////                TransInvT[i][j] = TransInv[j][i];
////            }
////        }
////        normal[0] = TransInvT[0][0] * m_CuttingPlaneNormal[0] + TransInvT[0][1] * m_CuttingPlaneNormal[1] + TransInvT[0][2] * m_CuttingPlaneNormal[2];
////        normal[1] = TransInvT[1][0] * m_CuttingPlaneNormal[0] + TransInvT[1][1] * m_CuttingPlaneNormal[1] + TransInvT[1][2] * m_CuttingPlaneNormal[2];
////        normal[2] = TransInvT[2][0] * m_CuttingPlaneNormal[0] + TransInvT[2][1] * m_CuttingPlaneNormal[1] + TransInvT[2][2] * m_CuttingPlaneNormal[2];

//        normal[0] = Trans[0][0] * m_CuttingPlaneNormal[0] + Trans[0][1] * m_CuttingPlaneNormal[1] + Trans[0][2] * m_CuttingPlaneNormal[2];
//        normal[1] = Trans[1][0] * m_CuttingPlaneNormal[0] + Trans[1][1] * m_CuttingPlaneNormal[1] + Trans[1][2] * m_CuttingPlaneNormal[2];
//        normal[2] = Trans[2][0] * m_CuttingPlaneNormal[0] + Trans[2][1] * m_CuttingPlaneNormal[1] + Trans[2][2] * m_CuttingPlaneNormal[2];

//        normal /= normal.magnitude();
*/
        // Transform m_a
        vnl_vector<double> pa(3);
        pa[0] = Trans[0][0] * base_a[0] + Trans[0][1] * base_a[1] + Trans[0][2] * base_a[2] + Trans[0][3];
        pa[1] = Trans[1][0] * base_a[0] + Trans[1][1] * base_a[1] + Trans[1][2] * base_a[2] + Trans[1][3];
        pa[2] = Trans[2][0] * base_a[0] + Trans[2][1] * base_a[1] + Trans[2][2] * base_a[2] + Trans[2][3];

        // Transform base_b
        vnl_vector<double> pb(3);
        pb[0] = Trans[0][0] * base_b[0] + Trans[0][1] * base_b[1] + Trans[0][2] * base_b[2] + Trans[0][3];
        pb[1] = Trans[1][0] * base_b[0] + Trans[1][1] * base_b[1] + Trans[1][2] * base_b[2] + Trans[1][3];
        pb[2] = Trans[2][0] * base_b[0] + Trans[2][1] * base_b[1] + Trans[2][2] * base_b[2] + Trans[2][3];

        // Transform base_c
        vnl_vector<double> pc(3);
        pc[0] = Trans[0][0] * base_c[0] + Trans[0][1] * base_c[1] + Trans[0][2] * base_c[2] + Trans[0][3];
        pc[1] = Trans[1][0] * base_c[0] + Trans[1][1] * base_c[1] + Trans[1][2] * base_c[2] + Trans[1][3];
        pc[2] = Trans[2][0] * base_c[0] + Trans[2][1] * base_c[1] + Trans[2][2] * base_c[2] + Trans[2][3];

        vnl_vector<double> pq;
        if (VDimension == 3)  pq = vnl_cross_3d((pb-pa),(pc-pa));
        else if (VDimension == 2)  pq = vnl_cross_2d((pb-pa),(pc-pa));

        m_CuttingPlaneNormal[0] = pq / pq.magnitude();
        m_CuttingPlanePoint[0] = pa;
        m_a[0] = pa;
        m_b[0] = pb;
        m_c[0] = pc;

#ifdef PARTICLE_DEBUG_FLAG
//        std::cout << "Mag(normal) : " << m_CuttingPlaneNormal[0].magnitude() << std::endl;
//        std::cout << "Point (a) : "   << m_a[0] << std::endl;
//        std::cout << "Point (b) : "   << m_b[0] << std::endl;
//        std::cout << "Point (c) : "   << m_c[0] << std::endl;
#endif
    }
}

#ifdef SW_USE_MESH
template<class T, unsigned int VDimension>
void
ParticleImplicitSurfaceDomain<T, VDimension>::
SetMesh(TriMesh *mesh, const char *gfile)
{
  m_mesh = mesh;

  m_mesh->need_faces();	
  m_mesh->need_neighbors();
	
  orient(m_mesh);

  // Recompute values that are no longer correct
  if (!m_mesh->normals.empty()) m_mesh->normals.clear();
  m_mesh->need_normals();

	if (!m_mesh->adjacentfaces.empty()) m_mesh->adjacentfaces.clear();
	m_mesh->need_adjacentfaces();

	if (!m_mesh->across_edge.empty()) m_mesh->across_edge.clear();
	  m_mesh->need_across_edge();

  if (!m_mesh->tstrips.empty()) m_mesh->tstrips.clear();
  m_mesh->need_tstrips();

  m_mesh->need_faceedges();
  m_mesh->need_oneringfaces();
  m_mesh->need_abs_curvatures();
  m_mesh->need_speed();
  m_mesh->setSpeedType(1);
  m_mesh->need_kdtree();

  double d = m_mesh->bsphere.r * PI * 2.0f * 0.1f;
  m_fim->SetStopDistance(d);

  m_fim->loadGeodesicFile(m_mesh, gfile);
}
#endif

// Praful - fea mesh support
#ifdef SW_USE_FEAMESH
template<class T, unsigned int VDimension>
void
ParticleImplicitSurfaceDomain<T, VDimension>::
SetMesh(TriMesh *mesh)
{
  m_mesh = mesh;
}

template<class T, unsigned int VDimension>
void
ParticleImplicitSurfaceDomain<T, VDimension>::
SetFeaMesh(const char *feaFile)
{
    m_mesh->ReadFeatureFromFile(feaFile);
}

template<class T, unsigned int VDimension>
void
ParticleImplicitSurfaceDomain<T, VDimension>::
SetFeaGrad(const char *feaGradFile)
{
    m_mesh->ReadFeatureGradientFromFile(feaGradFile);
}

template<class T, unsigned int VDimension>
void
ParticleImplicitSurfaceDomain<T, VDimension>::
SetFids(const char *fidsFile)
{
    m_mesh->ReadFaceIndexMap(fidsFile);
    const typename ImageType::PointType orgn = this->GetImage()->GetOrigin();
    m_mesh->imageOrigin[0] = orgn[0];
    m_mesh->imageOrigin[1] = orgn[1];
    m_mesh->imageOrigin[2] = orgn[2];
    typename ImageType::RegionType::SizeType sz = this->GetImage()->GetRequestedRegion().GetSize();
    m_mesh->imageSize[0]   = sz[0];
    m_mesh->imageSize[1]   = sz[1];
    m_mesh->imageSize[2]   = sz[2];
    typename ImageType::SpacingType sp = this->GetImage()->GetSpacing();
    m_mesh->imageSpacing[0] = sp[0];
    m_mesh->imageSpacing[1] = sp[1];
    m_mesh->imageSpacing[2] = sp[2];
    typename ImageType::RegionType::IndexType idx = this->GetImage()->GetRequestedRegion().GetIndex();
    m_mesh->imageIndex[0]   = idx[0];
    m_mesh->imageIndex[1]   = idx[1];
    m_mesh->imageIndex[2]   = idx[2];
}
#endif

template<class T, unsigned int VDimension>
bool
ParticleImplicitSurfaceDomain<T, VDimension>::
ApplyVectorConstraints(vnl_vector_fixed<double, VDimension> &gradE,
                       const PointType &pos,
                       double maxtimestep) const
{

    bool flag = false;
  //ShapeWorksRun4.5 - Ensuring that the update does not violate constraints
  vnl_vector_fixed<double, VDimension> x;
  vnl_vector_fixed<double, VDimension> xPos;
  for (unsigned int i = 0; i < VDimension; i++)
  {
      x[i] = pos[i] - gradE[i];
      xPos[i] = pos[i];
  }
  double gradMag = gradE.magnitude();

  if (this->IsCuttingPlaneDefined() && gradMag > 0.0)
  {
      for (unsigned int i = 0; i < this->GetNumberOfPlanes(); i++)
      {
          double D = dot_product(this->GetCuttingPlaneNormal(i), x-this->GetCuttingPlanePoint(i));
          if (D < 0)
          {
              flag = true;
              double D_pos = dot_product(this->GetCuttingPlaneNormal(i), xPos-this->GetCuttingPlanePoint(i));
              if (D_pos < 0)
                for (unsigned int n = 0; n < VDimension; n++)
                    gradE[n] *= -0.5*sqrt(-D_pos)/gradMag;
              else
                for (unsigned int n = 0; n < VDimension; n++)
                    gradE[n] *= 0.85*sqrt(D_pos)/gradMag;
          }
      }
  }
  return flag;
   gradMag = gradE.magnitude();

//  return Superclass::ApplyVectorConstraints(gradE,pos,maxtimestep);
    // disabled sphere part
  if (this->IsCuttingSphereDefined() && gradMag > 0.0)
  {
      for (unsigned int i = 0; i < this->GetNumberOfSpheres(); i++)
      {
          double rad = this->GetSphereRadius(i);
          double D = dot_product(x-this->GetSphereCenter(i), x-this->GetSphereCenter(i));
          double CToP = sqrt(dot_product(xPos-this->GetSphereCenter(i), xPos-this->GetSphereCenter(i)));
          if (rad < 0) // go inside sphere
          {
//              if (CToP > abs(rad)) // pos outside sphere - bring it in
//              {
//                  if (D < CToP) // gradient towards sphere - right direction
//                  {
//                      if (D > abs(rad)) // still outside sphere - bring it in
//                      {
//                          double D_pos = CToP - abs(rad);
//                          for (unsigned int n = 0; n < VDimension; n++)
//                              gradE[n] *= 1.1*D_pos/gradE.magnitude();
//                      }
//                  }
//                  else // project point into sphere
//                  {
//                      vnl_vector_fixed<double, VDimension> q;
//                      vnl_vector_fixed<double, VDimension> spherept;
//                      q = xPos - this->GetSphereCenter(i);
//                      q.normalize();

//                      spherept = this->GetSphereCenter(i) + (0.95*abs(rad))*q;
//                      for (unsigned int j = 0; j < VDimension; j++)
//                        gradE[j] = spherept[j]-xPos[j];
//                  }
//              }
//              else // pos inside sphere - correct place
//              {
                  if (D >= abs(rad)) // being projected outside sphere
                  {
                      double D_pos = abs(rad)-CToP;
                      for (unsigned int n = 0; n < VDimension; n++)
                          gradE[n] *= 0.9*D_pos/gradMag;
                  }
//              }
          }
          else // stay outside sphere
          {

//              if (CToP < abs(rad)) // pos inside sphere - push it out
//              {
//                  if (D > CToP) // gradient pushing outside sphere - right direction
//                  {
//                      if (D < abs(rad) ) // still inside sphere - bring it out
//                      {
//                          double D_pos = abs(rad) - CToP;
//                          for (unsigned int n = 0; n < VDimension; n++)
//                              gradE[n] *= 1.1*D_pos/gradE.magnitude();
//                      }
//                  }
//                  else // project point outside sphere
//                  {
//                      vnl_vector_fixed<double, VDimension> q;
//                      vnl_vector_fixed<double, VDimension> spherept;
//                      q = xPos - this->GetSphereCenter(i);
//                      q.normalize();

//                      spherept = this->GetSphereCenter(i) + (1.1*abs(rad))*q;
//                      for (unsigned int j = 0; j < VDimension; j++)
//                        gradE[j] = spherept[j]-xPos[j];
//                  }
//              }
//              else // pos outside sphere - correct place
//              {
                  if (D < abs(rad)) // being projected inside sphere
                  {
                      double D_pos = CToP-abs(rad);
                      for (unsigned int n = 0; n < VDimension; n++)
                          gradE[n] *= 0.9*D_pos/gradMag;
                  }
//              }
          }


      }
  }

  return Superclass::ApplyVectorConstraints(gradE,pos,maxtimestep);
}


template<class T, unsigned int VDimension>
bool
ParticleImplicitSurfaceDomain<T, VDimension>::ApplyConstraints(PointType &p) const
{
  // First apply and constraints imposed by superclasses.  This will
  // guarantee the point starts in the correct image domain.
  bool flag = Superclass::ApplyConstraints(p);

  if (this->m_ConstraintsEnabled == true)
    {
  
    unsigned int k = 0;
    double mult = 1.0;
    
    const T epsilon = m_Tolerance * 0.001;
    T f = this->Sample(p);
    
    T gradmag = 1.0;
    while ( fabs(f) > (m_Tolerance * mult) || gradmag < epsilon)
      //  while ( fabs(f) > m_Tolerance || gradmag < epsilon)
      {
      vnl_vector_fixed<T, VDimension> grad = this->SampleGradientVnl(p);
      
      gradmag = grad.magnitude();
      vnl_vector_fixed<T, VDimension> vec   =  grad  * ( f / (gradmag + epsilon) );
      for (unsigned int i = 0; i < VDimension; i++)
        {
        p[i] -= vec[i];
        }
      
      f = this->Sample(p);
      
      // Raise the tolerance if we have done too many iterations.
      k++;
      if (k > 10000)
        {
        mult *= 2.0;
        k = 0;
        }
      } // end while

//#ifdef  PARTICLE_DEBUG_FLAG
//      if ( ! this->IsInsideBuffer(p) )
//        {
//          std::cout<<"A Point, " << p << ", was projected outside the given image domain." ;
//        }
//#endif
//#ifdef  PARTICLE_DEBUG_FLAG
//      if ( gradmag < epsilon && fabs(f) > m_Tolerance)
//        {
//          std::cout << "Newton-Raphson iteration failed to find the zero level-set.  Gradient is zero, but f = "  <<  f << std::endl;
//        }
//#endif

    } // end if m_ConstraintsEnabled == true


  return flag; 
}

template <class T, unsigned int VDimension>
double
ParticleImplicitSurfaceDomain<T, VDimension>::Distance(const PointType &a, const PointType &b) const
{
#ifndef SW_USE_MESH
  // Check to see if the normals are >= 90 degrees apart.
  if ( dot_product(this->SampleGradientVnl(a), this->SampleGradientVnl(b) ) <= 0.0  )
  {
    return -1.0;
  }
  else  // Return distance
  {
    double sum = 0.0;
    for (unsigned int i = 0; i < VDimension; i++)
    {
      sum += (b[i]-a[i]) * (b[i]-a[i]);
    }
    return sqrt(sum);
  }
#else
  if (m_mesh != NULL)
  {
    point p1;
    p1[0] = a[0];
    p1[1] = a[1];
    p1[2] = a[2];    
    
    point p2;
    p2[0] = b[0];
    p2[1] = b[1];
    p2[2] = b[2];

    return ( m_mesh->GetGeodesicDistance(p1,p2) );
  }
#endif   
}

} // end namespace
#endif
