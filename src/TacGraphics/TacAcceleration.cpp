#include "TacAcceleration.h"
#include <algorithm>
namespace Tac
{
  void NoAcceleration::GetModels(
    std::vector<ModelComponent*> & models,
    const Frustrum & cameraFrustrum )
  {
    cameraFrustrum;

    models.resize(mModels.size());
    std::copy(mModels.begin(), mModels.end(), models.begin());
  }
}