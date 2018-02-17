#pragma once
#include <vector>
#include "TacCamera.h"

namespace Tac
{
  class ModelComponent;
  class AccelerationStructure
  {
  public:
    virtual void GetModels(
      std::vector<ModelComponent*> & models,
      const Frustrum & cameraFrustrum) = 0;
  };

  class NoAcceleration : public AccelerationStructure
  {
  public:
    void GetModels(
      std::vector<ModelComponent*> & models,
      const Frustrum & cameraFrustrum) override;
  private:
    std::vector<ModelComponent*> mModels;
  };
}
