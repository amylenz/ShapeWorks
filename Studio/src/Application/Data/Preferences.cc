/*
 * Shapeworks license
 */

// qt
#include <QThread>
#include <QSize>
#include <Data/Preferences.h>


//-----------------------------------------------------------------------------
Preferences::Preferences()
  : settings_( "Scientific Computing and Imaging Institute", 
    "ShapeWorksStudio" ), saved_(true) {
  this->restore_defaults();
  this->settings_.setFallbacksEnabled(false);
}

std::map<std::string, QVariant> Preferences::getAllPreferences() {
  std::map<std::string, QVariant> ans;
  for (auto &a : this->settings_.allKeys()) {
    if (a.toStdString().find("/") == std::string::npos) {
      ans.insert(std::make_pair(a.toStdString(), this->settings_.value(a)));
    }
  }
  return ans;
}

bool Preferences::not_saved() {
  return !this->saved_;
}

void Preferences::set_saved(bool saved) {
  this->saved_ = saved;
}

QStringList Preferences::get_recent_files()
{
  return this->settings_.value("Main/recentFileList").toStringList();
}

void Preferences::add_recent_file(QString file)
{
  QStringList files = this->get_recent_files();
  files.removeAll(file);
  files.prepend(file);
  while (files.size() > Preferences::MAX_RECENT_FILES)
  {
    files.removeLast();
  }

  this->settings_.setValue("Main/recentFileList", files);
}

//-----------------------------------------------------------------------------
void Preferences::restore_defaults(bool force) {
  if (!this->settings_.contains("cache_enabled") || force)
    this->settings_.setValue("cache_enabled", true);
  if (!this->settings_.contains("parallel_enabled") || force)
    this->settings_.setValue("parallel_enabled", true);
  if (!this->settings_.contains("cache_memory") || force)
    this->settings_.setValue("cache_memory", 25);
  if (!this->settings_.contains("glyph_size") || force)
    this->settings_.setValue("glyph_size", 5.);
  if (!this->settings_.contains("glyph_quality") || force)
    this->settings_.setValue("glyph_quality", 5.);
  if (!this->settings_.contains("num_threads") || force)
    this->settings_.setValue("num_threads", 100);
  if (!this->settings_.contains("pca_range") || force)
    this->settings_.setValue("pca_range", 2.f);
  if (!this->settings_.contains("pca_steps") || force)
    this->settings_.setValue("pca_steps", 20);
  if (!this->settings_.contains("regression_steps") || force)
    this->settings_.setValue("regression_steps", 50);
  if (!this->settings_.contains("smoothing_amount") || force)
    this->settings_.setValue("smoothing_amount", 0.);
  if (!this->settings_.contains("cache_epsilon") || force)
    this->settings_.setValue("cache_epsilon", 1e-3f);
  if (!this->settings_.contains("spacing") || force)
    this->settings_.setValue("spacing", 9.f);
  if (!this->settings_.contains("neighborhood") || force)
    this->settings_.setValue("neighborhood", 9);
  if (!this->settings_.contains("color_scheme") || force)
    this->settings_.setValue("color_scheme", 0);
  if (!this->settings_.contains("groom_center") || force)
    this->settings_.setValue("groom_center", true);
  if (!this->settings_.contains("groom_antialias") || force)
    this->settings_.setValue("groom_antialias", true);
  if (!this->settings_.contains("groom_pad") || force)
    this->settings_.setValue("groom_pad", true);
  if (!this->settings_.contains("groom_fastmarching") || force)
    this->settings_.setValue("groom_fastmarching", false);
  if (!this->settings_.contains("groom_blur") || force)
    this->settings_.setValue("groom_blur", true);
  if (!this->settings_.contains("groom_crop") || force)
    this->settings_.setValue("groom_crop", false);
  if (!this->settings_.contains("groom_isolate") || force)
    this->settings_.setValue("groom_isolate", true);
  if (!this->settings_.contains("groom_fill_holes") || force)
    this->settings_.setValue("groom_fill_holes", true);
  if (!this->settings_.contains("groom_antialias_amount") || force)
    this->settings_.setValue("groom_antialias_amount", 100);
  if (!this->settings_.contains("groom_fastmarching_isovalue") || force)
    this->settings_.setValue("groom_fastmarching_isovalue", 0.5);
  if (!this->settings_.contains("groom_fastmarching_sigma") || force)
    this->settings_.setValue("groom_fastmarching_sigma", 2.0);
  if (!this->settings_.contains("groom_blur_sigma") || force)
    this->settings_.setValue("groom_blur_sigma", 2.0);
  if (!this->settings_.contains("groom_pad_value") || force)
    this->settings_.setValue("groom_pad_value", 5);
  if (!this->settings_.contains("optimize_start_reg") || force)
    this->settings_.setValue("optimize_start_reg", 1000);
  if (!this->settings_.contains("optimize_particles") || force)
    this->settings_.setValue("optimize_particles", 128);
  if (!this->settings_.contains("optimize_decimation") || force)
    this->settings_.setValue("optimize_decimation", 0.3);
  if (!this->settings_.contains("optimize_end_reg") || force)
    this->settings_.setValue("optimize_end_reg", 10);
  if (!this->settings_.contains("optimize_iters") || force)
    this->settings_.setValue("optimize_iters", 1000);
  if (!this->settings_.contains("optimize_weight") || force)
    this->settings_.setValue("optimize_weight", 1.0);
  if (!this->settings_.contains("optimize_decay_span") || force)
    this->settings_.setValue("optimize_decay_span", 1000);
  if (!this->settings_.contains("optimize_procrustes_interval") || force)
    this->settings_.setValue("optimize_procrustes_interval", 0);
  if (!this->settings_.contains("optimize_maxAngle") || force)
    this->settings_.setValue("optimize_maxAngle", 30.);
  if (!this->settings_.contains("optimize_clusters") || force)
    this->settings_.setValue("optimize_clusters", 5);
  this->saved_ = true;
}

void Preferences::delete_entry(std::string entry) {
  auto allPref = this->getAllPreferences();
  std::vector<std::string> toRemove;
  for (auto &p : allPref) {
    if (p.first.rfind(entry) != std::string::npos) {
      this->settings_.remove(QString::fromStdString(p.first));
    }
  }
  allPref = this->getAllPreferences();
  this->saved_ = false;
}
