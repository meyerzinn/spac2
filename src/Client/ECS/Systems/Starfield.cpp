// based on https://nullprogram.com/blog/2011/06/13/
#include "Starfield.h"
#include <boost/functional/hash.hpp>
#include <cmath>
#include <iostream>
#include <utility>
namespace spac::client::system {
constexpr int32_t STARFIELD_SEED = 0x9d2c5681;
constexpr int32_t STARFIELD_TILE_SIZE = 2048;

Starfield::Starfield(entt::registry& registry, Camera2D* camera) : System(registry), mCamera(camera) {}

void Starfield::update() {
  Rectangle bounds;
  bounds.x = mCamera->target.x;
  bounds.y = mCamera->target.y;
  bounds.width = GetScreenWidth();
  bounds.height = GetScreenHeight();
  draw(bounds, 4, DARKGRAY);
  //  draw(bounds, 4, GRAY);
  draw(bounds, 1, WHITE);
}

void Starfield::draw(Rectangle bounds, int32_t scale, Color color) {
  const int32_t size = STARFIELD_TILE_SIZE / scale;  // todo DPI should be considered here
  const float x = bounds.x;
  const float y = bounds.y;
  const float width = bounds.width;
  const float height = bounds.height;
  const int32_t sx = std::floor(std::round((x - width / 2) / size) * size - size);
  const int32_t sy = std::round((y - height / 2) / size) * size - size;
  // divide the screen into chunks and use the hash of the chunk coordinates to determine the stars
  for (auto i = sx; i <= x + width + size; i += size) {
    for (auto j = sy; j <= y + height + size; j += size) {
      //      int32_t hash = mix(STARFIELD_SEED, i, j);
      int32_t hash = mix(STARFIELD_SEED, i, j);
      //      boost::hash_combine(hash, STARFIELD_SEED);
      //      boost::hash_combine(hash, i);
      //      boost::hash_combine(hash, j);
      for (int n = 0; n < (sizeof(hash) * 8) / 3; n++) {
        float px = float((hash % size)) + i - x;
        hash >>= 3;
        float py = float((hash % size)) + j - y;
        hash >>= 3;
        DrawCircleV(Vector2{px, py}, 1, color);
      }
    }
  }
}

// 96-bit mix function
int32_t Starfield::mix(int32_t a, int32_t b, int32_t c) {
  a -= b;
  a -= c;
  a ^= c >> 13;
  b -= c;
  b -= a;
  b ^= a << 8;
  c -= a;
  c -= b;
  c ^= b >> 13;
  a -= b;
  a -= c;
  a ^= c >> 12;
  b -= c;
  b -= a;
  b ^= a << 16;
  c -= a;
  c -= b;
  c ^= b >> 5;
  a -= b;
  a -= c;
  a ^= c >> 3;
  b -= c;
  b -= a;
  b ^= a << 10;
  c -= a;
  c -= b;
  c ^= b >> 15;
  return c;
}

}  // namespace spac::client::system