#pragma once

#include "Shapeworks.h"
#include "ImageUtils.h"
#include <limits>
#include <itkTranslationTransform.h>

namespace shapeworks {

class Image
{
public:
  static const unsigned dims = 3;
  using PixelType = float;
  using ImageType = itk::Image<PixelType, dims>;

  Image() {}
  Image(const std::string &inFilename) { read(inFilename); }

  bool read(const std::string &inFilename);
  bool write(const std::string &outFilename, bool useCompression = true);
  bool antialias(unsigned numIterations = 50, float maxRMSErr = 0.01f, unsigned numLayers = dims);
  bool binarize(PixelType threshold = std::numeric_limits<PixelType>::epsilon(),
                PixelType inside = itk::NumericTraits<PixelType>::One,
                PixelType outside = itk::NumericTraits<PixelType>::Zero);
  bool recenter();
  bool isoresample(double isoSpacing = 1.0f, Dims outputSize = Dims());
  bool pad(int padding, PixelType value);
  bool centerofmassalign(bool useCenterOfMass = true, float centerX = 0, float centerY = 0, float centerZ = 0, const std::string &dataFilename="data.txt");
  // bool nextfunction(...);

  bool compare_equal(const Image &other);

private:
  ImageType::Pointer image;
};

} // shapeworks
