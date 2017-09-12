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

#include "paddle/operators/gemm_conv_op.h"

namespace paddle {
namespace operators {

int outputSize(int input_size, int filter_size, int padding, int stride) {
  int output_size = (input_size - filter_size + 2 * padding) / stride + 1;
  return output_size;
}

class Conv2DOp : public framework::OperatorWithKernel {
 public:
  using framework::OperatorWithKernel::OperatorWithKernel;

 protected:
  void InferShape(const framework::InferShapeContext &ctx) const override {
    auto in = ctx.Input<Tensor>("Input");
    auto filter = ctx.Input<Tensor>("Filter");
    auto out = ctx.Output<Tensor>("Output");
    PADDLE_ENFORCE_EQ(in->dims().size(), 4, "Conv2DOp intput should be 4-D.");
    PADDLE_ENFORCE_EQ(filter->dims().size(), 4,
                      "Conv2DOp filter should be 4-D.");

    std::vector<int> strides = Attr<std::vector<int>>("strides");
    std::vector<int> paddings = Attr<std::vector<int>>("paddings");
    auto output_height =
        outputSize(in->dims()[2], filter->dims()[2], paddings[0], strides[0]);
    auto output_width =
        outputSize(in->dims()[3], filter->dims()[3], paddings[1], strides[1]);
    out->Resize(
        {in->dims()[0], filter->dims()[0], output_height, output_width});
  }
};

class Conv2DOpMaker : public framework::OpProtoAndCheckerMaker {
 public:
  Conv2DOpMaker(framework::OpProto *proto, framework::OpAttrChecker *op_checker)
      : OpProtoAndCheckerMaker(proto, op_checker) {
    AddInput(
        "Input",
        "The input tensor of convolution operator. "
        "The format of input tensor is NCHW. Where N is batch size, C is the "
        "number of channels, H and W is the height and width of image.");
    AddInput(
        "Filter",
        "The filter tensor of convolution operator."
        "The format of the filter tensor is MCHW, where M is the number of "
        "output "
        "image channels, C is the number of input image channels, H and W is "
        "height and width of filter.");
    AddOutput("Output",
              "The output tensor of convolution operator."
              "The format of output tensor is also NCHW.");
    AddComment(R"DOC(
The convolution operation calculates the output based on
the input, filter and strides, paddings parameters.
)DOC");
    AddAttr<std::vector<int>>("strides", "strides of convolution operator.");
    AddAttr<std::vector<int>>("paddings", "paddings of convolution operator.");
  }
};

class Conv2DOpGrad : public framework::OperatorWithKernel {
 public:
  using framework::OperatorWithKernel::OperatorWithKernel;

 protected:
  void InferShape(const framework::InferShapeContext &ctx) const override {
    auto in = ctx.Input<Tensor>("Input");
    auto filter = ctx.Input<Tensor>("Filter");
    auto d_in = ctx.Output<Tensor>(framework::GradVarName("Input"));
    auto d_filter = ctx.Output<Tensor>(framework::GradVarName("Filter"));
    d_in->Resize(in->dims());
    d_filter->Resize(filter->dims());
  }
};

}  // namespace operators
}  // namespace paddle

namespace ops = paddle::operators;
REGISTER_OP(conv2d, ops::Conv2DOp, ops::Conv2DOpMaker, conv2d_grad,
            ops::Conv2DOpGrad);

REGISTER_OP_CPU_KERNEL(conv2d,
                       ops::GemmConvKernel<paddle::platform::CPUPlace, float>);
REGISTER_OP_CPU_KERNEL(
    conv2d_grad, ops::GemmConvGradKernel<paddle::platform::CPUPlace, float>);
