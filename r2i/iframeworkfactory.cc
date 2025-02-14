/* Copyright (C) 2018 RidgeRun, LLC (http://www.ridgerun.com)
 * All Rights Reserved.
 *
 * The contents of this software are proprietary and confidential to RidgeRun,
 * LLC.  No part of this program may be photocopied, reproduced or translated
 * into another programming language without prior written consent of
 * RidgeRun, LLC.  The user is free to modify the source code after obtaining
 * a software license from RidgeRun.  All source code changes must be provided
 * back to RidgeRun without any encumbrance.
*/

#include "iframeworkfactory.h"

#include <functional>
#include <unordered_map>

#include "config.h"
#include "coral/frameworkfactory.h"
#include "nnapi/frameworkfactory.h"
#include "onnxrt/frameworkfactory.h"
#include "onnxrt_acl/frameworkfactory.h"
#include "onnxrt_openvino/frameworkfactory.h"
#include "tensorflow/frameworkfactory.h"
#include "tensorrt/frameworkfactory.h"
#include "tflite/frameworkfactory.h"

namespace r2i {

#ifdef HAVE_CORAL
static std::unique_ptr<IFrameworkFactory>
MakeCoralFactory (RuntimeError &error) {
  return std::unique_ptr<coral::FrameworkFactory> (new
         coral::FrameworkFactory);
}
#endif // HAVE_CORAL

#ifdef HAVE_ONNXRT
static std::unique_ptr<IFrameworkFactory>
MakeOnnxrtFactory(RuntimeError &error) {
  return std::unique_ptr<onnxrt::FrameworkFactory>(new
         onnxrt::FrameworkFactory);
}
#endif // HAVE_ONNXRT

#ifdef HAVE_ONNXRT_ACL
static std::unique_ptr<IFrameworkFactory>
MakeOnnxrtAclFactory (RuntimeError &error) {
  return std::unique_ptr<onnxrt_acl::FrameworkFactory> (new
         onnxrt_acl::FrameworkFactory);
}
#endif // HAVE_ONNXRT_ACL

#ifdef HAVE_ONNXRT_OPENVINO
static std::unique_ptr<IFrameworkFactory>
MakeOnnxrtOpenvinoFactory (RuntimeError &error) {
  return std::unique_ptr<onnxrt_openvino::FrameworkFactory> (new
         onnxrt_openvino::FrameworkFactory);
}
#endif // HAVE_ONNXRT_OPENVINO

#ifdef HAVE_TENSORFLOW
static std::unique_ptr<IFrameworkFactory>
MakeTensorflowFactory (RuntimeError &error) {
  return std::unique_ptr<tensorflow::FrameworkFactory> (new
         tensorflow::FrameworkFactory);
}
#endif // HAVE_TENSORFLOW

#ifdef HAVE_TFLITE
static std::unique_ptr<IFrameworkFactory>
MakeTfLiteFactory (RuntimeError &error) {
  return std::unique_ptr<tflite::FrameworkFactory> (new
         tflite::FrameworkFactory);
}
#endif // HAVE_TFLITE

#ifdef HAVE_TENSORRT
static std::unique_ptr<IFrameworkFactory>
MakeTensorRTFactory (RuntimeError &error) {
  return std::unique_ptr<tensorrt::FrameworkFactory> (new
         tensorrt::FrameworkFactory);
}
#endif // HAVE_TENSORRT

#ifdef HAVE_NNAPI
static std::unique_ptr<IFrameworkFactory>
MakeNNAPIFactory (RuntimeError &error) {
  return std::unique_ptr<nnapi::FrameworkFactory> (new
         nnapi::FrameworkFactory);
}
#endif // HAVE_NNAPI

typedef std::function<std::unique_ptr<IFrameworkFactory>(RuntimeError &)>
MakeFactory;
const std::unordered_map<int, MakeFactory> frameworks ({

#ifdef HAVE_CORAL
  {FrameworkCode::CORAL, MakeCoralFactory},
#endif //HAVE_CORAL

#ifdef HAVE_ONNXRT
  {FrameworkCode::ONNXRT, MakeOnnxrtFactory},
#endif //HAVE_ONNXRT

#ifdef HAVE_ONNXRT_ACL
  {FrameworkCode::ONNXRT_ACL, MakeOnnxrtAclFactory},
#endif //HAVE_ONNXRT_ACL

#ifdef HAVE_ONNXRT_OPENVINO
  {FrameworkCode::ONNXRT_OPENVINO, MakeOnnxrtOpenvinoFactory},
#endif //HAVE_ONNXRT_OPENVINO

#ifdef HAVE_TENSORFLOW
  {FrameworkCode::TENSORFLOW, MakeTensorflowFactory},
#endif //HAVE_TENSORFLOW

#ifdef HAVE_TFLITE
  {FrameworkCode::TFLITE, MakeTfLiteFactory},
#endif //HAVE_TFLITE

#ifdef HAVE_TENSORRT
  {FrameworkCode::TENSORRT, MakeTensorRTFactory},
#endif //HAVE_TENSORRT

#ifdef HAVE_NNAPI
  {FrameworkCode::NNAPI, MakeNNAPIFactory},
#endif //HAVE_NNAPI

});

std::unique_ptr<IFrameworkFactory>
IFrameworkFactory::MakeFactory (FrameworkCode code, RuntimeError &error) {
  auto match = frameworks.find (code);

  /* No match found */
  if (match == frameworks.end ()) {
    error.Set (r2i::RuntimeError::Code::UNSUPPORTED_FRAMEWORK,
               "The framework is invalid or not supported in this system");
    return nullptr;
  }

  return match->second (error);
}

std::vector<FrameworkMeta>
IFrameworkFactory::List(RuntimeError &error) {
  std::vector<FrameworkMeta> metas;

  for (auto &fw : frameworks) {
    auto factory = IFrameworkFactory::MakeFactory (static_cast<FrameworkCode>
                   (fw.first), error);
    metas.push_back (factory->GetDescription (error));
  }

  return metas;
}

} // namespace r2i
