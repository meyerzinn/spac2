#pragma once
namespace spac::client {
class Scene {
 public:
  Scene() = default;
  virtual void update() = 0;
  virtual void render() = 0;
  virtual ~Scene() = default;
  Scene(const Scene &) = delete;  // disable copy constructor
};

}  // namespace spac::client
