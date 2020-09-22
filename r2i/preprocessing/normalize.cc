/*
 * Copyright (C) 2020 RidgeRun, LLC (http://www.ridgerun.com)
 * All Rights Reserved.
 *
 * The contents of this software are proprietary and confidential to RidgeRun,
 * LLC.  No part of this program may be photocopied, reproduced or translated
 * into another programming language without prior written consent of
 * RidgeRun, LLC.  The user is free to modify the source code after obtaining
 * a software license from RidgeRun.  All source code changes must be provided
 * back to RidgeRun without any encumbrance.
 */

#include <memory>
#include <vector>

#include <r2i/preprocessing/normalize.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

namespace r2i {

Normalize::Normalize () {
  /* Set supported formats */
  this->formats.push_back(r2i::ImageFormat(r2i::ImageFormat::Id::RGB));
}

r2i::RuntimeError Normalize::Apply(std::shared_ptr<r2i::IFrame>
                                   in_frame,
                                   std::shared_ptr<r2i::IFrame> out_frame, int required_width, int required_height,
                                   r2i::ImageFormat::Id required_format_id) {
  r2i::RuntimeError error;
  int width = 0;
  int height = 0;
  const unsigned char *data;

  if (!in_frame or !out_frame) {
    error.Set (r2i::RuntimeError::Code::NULL_PARAMETER, "Null IFrame parameters");
    return error;
  }

  error = Validate(required_width, required_height, required_format_id);
  if (error.IsError ()) {
    return error;
  }

  width = in_frame->GetWidth();
  height = in_frame->GetHeight();
  data = static_cast<const unsigned char *>(in_frame->GetData());

  if (!data) {
    error.Set (r2i::RuntimeError::Code::NULL_PARAMETER, "Null frame data");
    return error;
  }

  this->processed_data = PreProcessImage(data, width, height, required_width,
                                         required_height);
  error = out_frame->Configure (processed_data.get(), required_width,
                                required_height,
                                required_format_id);
  if (error.IsError ()) {
    return error;
  }
  return error;
}

std::vector<r2i::ImageFormat> Normalize::GetAvailableFormats() {
  return this->formats;
}

std::vector<std::tuple<int, int>>
Normalize::GetAvailableDataSizes() {
  return this->dimensions;
}

r2i::RuntimeError Normalize::Validate (int required_width,
                                       int required_height,
                                       r2i::ImageFormat::Id required_format_id) {

  r2i::RuntimeError error;
  r2i::ImageFormat format;
  bool match_dimensions = false;
  bool match_format = false;
  int width = 0;
  int height = 0;

  /* Verify if the required dimensions are supported */
  for (unsigned int i = 0; i < this->dimensions.size(); i++) {
    width = std::get<0>(this->dimensions.at(i));
    height = std::get<1>(this->dimensions.at(i));
    if (width == required_width and height == required_height) {
      match_dimensions = true;
      break;
    }
  }

  if (!match_dimensions) {
    error.Set (r2i::RuntimeError::Code::MODULE_ERROR,
               "Required output dimensions are not supported in the preprocessing module");
    return error;
  }

  /* Verify if the required format is supported */
  for (unsigned int i = 0; i < this->formats.size(); i++) {
    format = this->formats.at(i);
    if (format.GetId() == required_format_id) {
      match_format = true;
      break;
    }
  }

  if (!match_format) {
    error.Set (r2i::RuntimeError::Code::MODULE_ERROR,
               "Required output image format is not supported in the preprocessing module");
    return error;
  }

  return error;
}

r2i::RuntimeError Normalize::SetNormalizationParameters () {
  return r2i::RuntimeError();
}

std::shared_ptr<float> Normalize::PreProcessImage (
  const unsigned char *input,
  int width, int height, int required_width, int required_height) {

  const int channels = 3;
  const int scaled_size = channels * required_width * required_height;
  std::shared_ptr<unsigned char> scaled_ptr;
  std::shared_ptr<float> adjusted_ptr;
  float *adjusted;
  unsigned char *scaled;

  scaled_ptr = std::shared_ptr<unsigned char>(new unsigned char[scaled_size],
               std::default_delete<const unsigned char[]>());
  adjusted_ptr = std::shared_ptr<float>(new float[scaled_size],
                                        std::default_delete<float[]>());

  adjusted = adjusted_ptr.get();
  scaled = scaled_ptr.get();

  stbir_resize_uint8(input, width, height, 0, scaled, required_width,
                     required_height, 0, channels);

  SetNormalizationParameters();

  for (int i = 0; i < scaled_size; i += channels) {
    /* RGB = (RGB - Mean)/StdDev */
    adjusted[i + 0] = (static_cast<float>(scaled[i + 0]) - this->mean) /
                      this->std_dev;
    adjusted[i + 1] = (static_cast<float>(scaled[i + 1]) - this->mean) /
                      this->std_dev;
    adjusted[i + 2] = (static_cast<float>(scaled[i + 2]) - this->mean) /
                      this->std_dev;
  }

  return adjusted_ptr;
}

}  // namespace r2i