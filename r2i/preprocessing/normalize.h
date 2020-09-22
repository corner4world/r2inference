/* Copyright (C) 2020 RidgeRun, LLC (http://www.ridgerun.com)
 * All Rights Reserved.
 *
 * The contents of this software are proprietary and confidential to RidgeRun,
 * LLC.  No part of this program may be photocopied, reproduced or translated
 * into another programming language without prior written consent of
 * RidgeRun, LLC.  The user is free to modify the source code after obtaining
 * a software license from RidgeRun.  All source code changes must be provided
 * back to RidgeRun without any encumbrance.
*/

#ifndef R2I_NORMALIZE_PREPROCESSING_H
#define R2I_NORMALIZE_PREPROCESSING_H

#include <r2i/ipreprocessing.h>

namespace r2i {

class Normalize: public r2i::IPreprocessing {
 public:
  Normalize ();
  r2i::RuntimeError Apply(std::shared_ptr<r2i::IFrame> in_frame,
                          std::shared_ptr<r2i::IFrame> out_frame, int required_width, int required_height,
                          r2i::ImageFormat::Id required_format_id) override;
  std::vector<r2i::ImageFormat> GetAvailableFormats() override;
  std::vector<std::tuple<int, int>> GetAvailableDataSizes() override;

 protected:
  double mean_red = 0;
  double mean_green = 0;
  double mean_blue = 0;
  double std_dev_red = 1;
  double std_dev_green = 1;
  double std_dev_blue = 1;

  std::shared_ptr<float> processed_data;
  std::vector<std::tuple<int, int>> dimensions;
  std::vector<r2i::ImageFormat> formats;

  r2i::RuntimeError Validate (int required_width, int required_height,
                              r2i::ImageFormat::Id required_format_id);
  std::shared_ptr<float> PreProcessImage (const unsigned char *input,
                                          int width, int height, int required_width, int required_height);
  virtual r2i::RuntimeError SetNormalizationParameters ();
};

}  // namespace r2i

#endif  // R2I_NORMALIZE_PREPROCESSING_H
