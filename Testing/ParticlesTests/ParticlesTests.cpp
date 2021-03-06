#include <gtest/gtest.h>

#include <Libs/Particles/ParticleSystem.h>
#include <Libs/Particles/ShapeEvaluation.h>
#include "TestConfiguration.h"
#include <vector>
#include <string>

using namespace shapeworks;

const std::string test_dir = std::string(TEST_DATA_DIR) + "/ellipsoid_particles/";
const std::vector<std::string> filenames = {
  test_dir + "seg.ellipsoid_0.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_1.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_10.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_11.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_12.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_13.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_14.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_15.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_16.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_17.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_18.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_19.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_2.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_20.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_21.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles"
};

//---------------------------------------------------------------------------
TEST(ParticlesTests, compactness_test)
{
  auto particleSystem = ParticleSystem();
  ASSERT_TRUE(particleSystem.LoadParticles(filenames));

  const double compactness = ShapeEvaluation<3>::ComputeCompactness(particleSystem, 1);
  ASSERT_DOUBLE_EQ(compactness, 0.99178682878009183);
}

//---------------------------------------------------------------------------
TEST(ParticlesTests, generalization_test)
{
  auto particleSystem = ParticleSystem();
  ASSERT_TRUE(particleSystem.LoadParticles(filenames));

  const double generalization = ShapeEvaluation<3>::ComputeGeneralization(particleSystem, 1);
  ASSERT_DOUBLE_EQ(generalization, 0.19815116412998687);
}

//---------------------------------------------------------------------------
TEST(ParticlesTests, specificity_test)
{
  auto particleSystem = ParticleSystem();
  ASSERT_TRUE(particleSystem.LoadParticles(filenames));

  const double specificity = ShapeEvaluation<3>::ComputeSpecificity(particleSystem, 1);
  ASSERT_NEAR(specificity, 0.262809, 1e-1f);
}
