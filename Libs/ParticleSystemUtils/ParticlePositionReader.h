/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticlePositionReader.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef _ParticlePositionReader_h
#define _ParticlePositionReader_h

#include <itkDataObject.h>
#include <itkObjectFactory.h>
#include <itkPoint.h>
#include <itkWeakPointer.h>
#include <vector>
#include <string>

//using namespace itk;

/** \class ParticlePositionReader
 *  This class reads a set of Points from disk and stores them in a vector.
 *  The file format is simple an ascii list of VDimension-tuples stored one per
 *  line (delimited by std::endl).  There is no header required for the file,
 *  but the file should not contain leading or trailing empty lines.
 *
 * In 3D, for example, a fragment of a points file looks like this:
 *
 * 1.0 2.0 5.4
 * 2.3 8.7 33.0
 * 4.0 8.21 4.44
 *
 * etc..
 */
template <unsigned int VDimension>
class ParticlePositionReader : public itk::DataObject
{
public:
  /** Standard class typedefs */
  typedef ParticlePositionReader Self;
  typedef itk::DataObject Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;
  typedef itk::WeakPointer<const Self>  ConstWeakPointer;

  /** The point type that this class reads. */
  typedef itk::Point<double, VDimension> PointType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticlePositionReader, DataObject);

  /** Get the output of the reader.  The output is a std::vector of PointType. */
  const std::vector<PointType> &GetOutput() const
  {
    return m_Output;
  }

  /** Set/Get the filename. */
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

  /** Read the file. */
  inline void Read()
  { this->Update(); }
  void Update();
  
protected:
  ParticlePositionReader() { }
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  
    os << indent << "ParticlePositionReader: " << std::endl;
  }
  virtual ~ParticlePositionReader() {};

 private:
  ParticlePositionReader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<PointType> m_Output;
  std::string m_FileName;
};

#include "ParticlePositionReader.cpp"

#endif


