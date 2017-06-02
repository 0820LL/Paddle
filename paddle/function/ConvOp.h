/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserve.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once

#include "Function.h"

namespace paddle {

/*
 * Function Arguments:
 *
 * \param inputs[0]  Input image data, is NCHW format, where N is batch size,
 *                   C is the number of channels, H and W is the height and
 *                   width of input image.
 * \param inputs[1]  Filter data, is MCHW, where M is the number of output
 *                   channels, C is the number of input channels, H and W
 *                   is height and width of filter.
 * \param outputs[0] Output image data, is NCHW format, where N is batch size,
 *                   C is the number of channels, H and W is the height and
 *                   width of output image.
 *
 * \note Implemented based on the ConvFunctionBase class only supports
 *       input data in the NCHW format.
 */
class ConvFunctionBase : public FunctionBase {
public:
  void init(const FuncConfig& config) override {
    // function arguments
    strides_ = config.get<std::vector<size_t>>("strides");
    paddings_ = config.get<std::vector<size_t>>("paddings");
    groups_ = config.get<size_t>("groups");

    // number of inputs and outputs
    numInputs_ = 2;
    numOutputs_ = 1;
  }

  virtual void calc(const BufferArgs& inputs, const BufferArgs& outputs) {}

  void check(const BufferArgs& inputs, const BufferArgs& outputs) override {
    CHECK_EQ(numInputs_, inputs.size());
    CHECK_EQ(numOutputs_, outputs.size());

    CHECK_EQ(inputs[0].shape().ndims(), (size_t)4);
    CHECK_EQ(inputs[1].shape().ndims(), (size_t)4);
    CHECK_EQ(outputs[0].shape().ndims(), (size_t)4);

    CHECK(inputs[0].shape()[0] == outputs[0].shape()[0]);
    CHECK(inputs[0].shape()[1] / groups_ == inputs[1].shape()[1]);
    CHECK(outputs[0].shape()[1] == inputs[1].shape()[0]);
  }

protected:
  std::vector<size_t> strides_;
  std::vector<size_t> paddings_;
  /// Group size, refer to grouped convolution in
  /// Alex Krizhevsky's paper: when group=2, the first half of the
  /// filters are only connected to the first half of the input channels,
  /// and the second half only connected to the second half.
  size_t groups_;
  inline int strideH() const { return strides_[0]; }

  inline int strideW() const { return strides_[1]; }

  inline int paddingH() const { return paddings_[0]; }

  inline int paddingW() const { return paddings_[1]; }
};

}  // namespace paddle
