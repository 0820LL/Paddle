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

#include "paddle/operators/prelu_op.h"
#include "paddle/operators/net_op.h"

namespace paddle {
namespace operators {

class PreluOp : public framework::OperatorWithKernel {
 public:
  PreluOp(const std::string &type, const framework::VariableNameMap &inputs,
          const framework::VariableNameMap &outputs,
          const framework::AttributeMap &attrs)
      : OperatorWithKernel(type, inputs, outputs, attrs) {}

 protected:
  void InferShape(const framework::InferShapeContext &ctx) const override {
    auto *in = ctx.Input<framework::Tensor>("X");
    auto *out = ctx.Output<framework::LoDTensor>("Out");
    out->Resize(in->dims());
  }
};

template <typename AttrType>
class PreluOpMaker : public framework::OpProtoAndCheckerMaker {
 public:
  PreluOpMaker(framework::OpProto *proto, framework::OpAttrChecker *op_checker)
      : OpProtoAndCheckerMaker(proto, op_checker) {
    AddInput("X", "The input tensor of prelu operator.").NotInGradient();
    AddOutput("Out", "The output tensor of prelu operator.").NotInGradient();
    AddComment(R"DOC(Prelu operator

The equation is:
f(x) = alpha * x , for x < 0
f(x) = x         , for x >= 0
)DOC");
    AddAttr<AttrType>("alpha", "The scaling factor alpha of prelu.")
        .SetDefault(0.0);
  }
};

// The operator to calculate gradients of a prelu operator.
class PreluGradOp : public framework::OperatorWithKernel {
 public:
  using framework::OperatorWithKernel::OperatorWithKernel;

 protected:
  void InferShape(const framework::InferShapeContext &ctx) const override {
    auto X_grad = ctx.Output<framework::LoDTensor>(framework::GradVarName("X"));
    auto X = ctx.Input<Tensor>("X");

    X_grad->Resize(X->dims());
  }
};

}  // namespace operators
}  // namespace paddle

namespace ops = paddle::operators;

REGISTER_OP(prelu, ops::PreluOp, ops::PreluOpMaker<float>, prelu_grad,
            ops::PreluGradOp);
REGISTER_OP_CPU_KERNEL(prelu,
                       ops::PreluKernel<paddle::platform::CPUPlace, float>);
REGISTER_OP_CPU_KERNEL(prelu_grad,
                       ops::PreluGradKernel<paddle::platform::CPUPlace, float>);
