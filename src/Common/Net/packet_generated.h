// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_PACKET_SPAC_NET_H_
#define FLATBUFFERS_GENERATED_PACKET_SPAC_NET_H_

#include "flatbuffers/flatbuffers.h"

namespace spac {
namespace net {

struct vec2f;

struct ShipMetadata;

struct ShipDelta;

struct ProjectileMetadata;

struct ProjectileDelta;

struct Perception;

struct Packet;

enum Message {
  Message_NONE = 0,
  Message_Perception = 1,
  Message_MIN = Message_NONE,
  Message_MAX = Message_Perception
};

inline const Message (&EnumValuesMessage())[2] {
  static const Message values[] = {
    Message_NONE,
    Message_Perception
  };
  return values;
}

inline const char * const *EnumNamesMessage() {
  static const char * const names[] = {
    "NONE",
    "Perception",
    nullptr
  };
  return names;
}

inline const char *EnumNameMessage(Message e) {
  if (e < Message_NONE || e > Message_Perception) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesMessage()[index];
}

template<typename T> struct MessageTraits {
  static const Message enum_value = Message_NONE;
};

template<> struct MessageTraits<Perception> {
  static const Message enum_value = Message_Perception;
};

bool VerifyMessage(flatbuffers::Verifier &verifier, const void *obj, Message type);
bool VerifyMessageVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) vec2f FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;

 public:
  vec2f() {
    memset(static_cast<void *>(this), 0, sizeof(vec2f));
  }
  vec2f(float _x, float _y)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)) {
  }
  float x() const {
    return flatbuffers::EndianScalar(x_);
  }
  float y() const {
    return flatbuffers::EndianScalar(y_);
  }
};
FLATBUFFERS_STRUCT_END(vec2f, 8);

struct ShipMetadata FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_NAME = 6
  };
  uint32_t id() const {
    return GetField<uint32_t>(VT_ID, 0);
  }
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_ID) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           verifier.EndTable();
  }
};

struct ShipMetadataBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(uint32_t id) {
    fbb_.AddElement<uint32_t>(ShipMetadata::VT_ID, id, 0);
  }
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(ShipMetadata::VT_NAME, name);
  }
  explicit ShipMetadataBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ShipMetadataBuilder &operator=(const ShipMetadataBuilder &);
  flatbuffers::Offset<ShipMetadata> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ShipMetadata>(end);
    return o;
  }
};

inline flatbuffers::Offset<ShipMetadata> CreateShipMetadata(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t id = 0,
    flatbuffers::Offset<flatbuffers::String> name = 0) {
  ShipMetadataBuilder builder_(_fbb);
  builder_.add_name(name);
  builder_.add_id(id);
  return builder_.Finish();
}

inline flatbuffers::Offset<ShipMetadata> CreateShipMetadataDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t id = 0,
    const char *name = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return spac::net::CreateShipMetadata(
      _fbb,
      id,
      name__);
}

struct ShipDelta FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_POSITION = 6,
    VT_VELOCITY = 8,
    VT_ANGLE = 10,
    VT_ANGULARVELOCITY = 12,
    VT_HEALTH = 14,
    VT_FLAGS = 16
  };
  uint32_t id() const {
    return GetField<uint32_t>(VT_ID, 0);
  }
  const vec2f *position() const {
    return GetStruct<const vec2f *>(VT_POSITION);
  }
  const vec2f *velocity() const {
    return GetStruct<const vec2f *>(VT_VELOCITY);
  }
  float angle() const {
    return GetField<float>(VT_ANGLE, 0.0f);
  }
  float angularVelocity() const {
    return GetField<float>(VT_ANGULARVELOCITY, 0.0f);
  }
  uint8_t health() const {
    return GetField<uint8_t>(VT_HEALTH, 0);
  }
  /// FLAGS (LSB to MSB):
  /// First three bits are booster status (0 to 7 indicating the relative burn mass (proportional to thrust))
  /// Next three bits are turning status (0 to 3, LSB is direction, where 1=positive radial direction)
  /// Next bit is shield status (0x0 = disengaged, 0x1 = engaged).
  uint8_t flags() const {
    return GetField<uint8_t>(VT_FLAGS, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_ID) &&
           VerifyField<vec2f>(verifier, VT_POSITION) &&
           VerifyField<vec2f>(verifier, VT_VELOCITY) &&
           VerifyField<float>(verifier, VT_ANGLE) &&
           VerifyField<float>(verifier, VT_ANGULARVELOCITY) &&
           VerifyField<uint8_t>(verifier, VT_HEALTH) &&
           VerifyField<uint8_t>(verifier, VT_FLAGS) &&
           verifier.EndTable();
  }
};

struct ShipDeltaBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(uint32_t id) {
    fbb_.AddElement<uint32_t>(ShipDelta::VT_ID, id, 0);
  }
  void add_position(const vec2f *position) {
    fbb_.AddStruct(ShipDelta::VT_POSITION, position);
  }
  void add_velocity(const vec2f *velocity) {
    fbb_.AddStruct(ShipDelta::VT_VELOCITY, velocity);
  }
  void add_angle(float angle) {
    fbb_.AddElement<float>(ShipDelta::VT_ANGLE, angle, 0.0f);
  }
  void add_angularVelocity(float angularVelocity) {
    fbb_.AddElement<float>(ShipDelta::VT_ANGULARVELOCITY, angularVelocity, 0.0f);
  }
  void add_health(uint8_t health) {
    fbb_.AddElement<uint8_t>(ShipDelta::VT_HEALTH, health, 0);
  }
  void add_flags(uint8_t flags) {
    fbb_.AddElement<uint8_t>(ShipDelta::VT_FLAGS, flags, 0);
  }
  explicit ShipDeltaBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ShipDeltaBuilder &operator=(const ShipDeltaBuilder &);
  flatbuffers::Offset<ShipDelta> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ShipDelta>(end);
    return o;
  }
};

inline flatbuffers::Offset<ShipDelta> CreateShipDelta(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t id = 0,
    const vec2f *position = 0,
    const vec2f *velocity = 0,
    float angle = 0.0f,
    float angularVelocity = 0.0f,
    uint8_t health = 0,
    uint8_t flags = 0) {
  ShipDeltaBuilder builder_(_fbb);
  builder_.add_angularVelocity(angularVelocity);
  builder_.add_angle(angle);
  builder_.add_velocity(velocity);
  builder_.add_position(position);
  builder_.add_id(id);
  builder_.add_flags(flags);
  builder_.add_health(health);
  return builder_.Finish();
}

struct ProjectileMetadata FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_OWNER = 6
  };
  uint32_t id() const {
    return GetField<uint32_t>(VT_ID, 0);
  }
  uint32_t owner() const {
    return GetField<uint32_t>(VT_OWNER, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_ID) &&
           VerifyField<uint32_t>(verifier, VT_OWNER) &&
           verifier.EndTable();
  }
};

struct ProjectileMetadataBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(uint32_t id) {
    fbb_.AddElement<uint32_t>(ProjectileMetadata::VT_ID, id, 0);
  }
  void add_owner(uint32_t owner) {
    fbb_.AddElement<uint32_t>(ProjectileMetadata::VT_OWNER, owner, 0);
  }
  explicit ProjectileMetadataBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ProjectileMetadataBuilder &operator=(const ProjectileMetadataBuilder &);
  flatbuffers::Offset<ProjectileMetadata> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ProjectileMetadata>(end);
    return o;
  }
};

inline flatbuffers::Offset<ProjectileMetadata> CreateProjectileMetadata(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t id = 0,
    uint32_t owner = 0) {
  ProjectileMetadataBuilder builder_(_fbb);
  builder_.add_owner(owner);
  builder_.add_id(id);
  return builder_.Finish();
}

struct ProjectileDelta FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_POSITION = 6,
    VT_VELOCITY = 8
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const vec2f *position() const {
    return GetStruct<const vec2f *>(VT_POSITION);
  }
  const vec2f *velocity() const {
    return GetStruct<const vec2f *>(VT_VELOCITY);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID) &&
           VerifyField<vec2f>(verifier, VT_POSITION) &&
           VerifyField<vec2f>(verifier, VT_VELOCITY) &&
           verifier.EndTable();
  }
};

struct ProjectileDeltaBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(ProjectileDelta::VT_ID, id, 0);
  }
  void add_position(const vec2f *position) {
    fbb_.AddStruct(ProjectileDelta::VT_POSITION, position);
  }
  void add_velocity(const vec2f *velocity) {
    fbb_.AddStruct(ProjectileDelta::VT_VELOCITY, velocity);
  }
  explicit ProjectileDeltaBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ProjectileDeltaBuilder &operator=(const ProjectileDeltaBuilder &);
  flatbuffers::Offset<ProjectileDelta> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ProjectileDelta>(end);
    return o;
  }
};

inline flatbuffers::Offset<ProjectileDelta> CreateProjectileDelta(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    const vec2f *position = 0,
    const vec2f *velocity = 0) {
  ProjectileDeltaBuilder builder_(_fbb);
  builder_.add_velocity(velocity);
  builder_.add_position(position);
  builder_.add_id(id);
  return builder_.Finish();
}

struct Perception FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TIMESTAMP = 4,
    VT_FUEL = 6,
    VT_REMOVED = 8,
    VT_SHIPMETAS = 10,
    VT_PROJECTILEMETAS = 12,
    VT_SHIPDELTAS = 14,
    VT_PROJECTILEDELTAS = 16
  };
  int64_t timestamp() const {
    return GetField<int64_t>(VT_TIMESTAMP, 0);
  }
  float fuel() const {
    return GetField<float>(VT_FUEL, 0.0f);
  }
  const flatbuffers::Vector<uint32_t> *removed() const {
    return GetPointer<const flatbuffers::Vector<uint32_t> *>(VT_REMOVED);
  }
  const flatbuffers::Vector<flatbuffers::Offset<ShipMetadata>> *shipMetas() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<ShipMetadata>> *>(VT_SHIPMETAS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<ProjectileMetadata>> *projectileMetas() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<ProjectileMetadata>> *>(VT_PROJECTILEMETAS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<ShipDelta>> *shipDeltas() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<ShipDelta>> *>(VT_SHIPDELTAS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<ProjectileDelta>> *projectileDeltas() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<ProjectileDelta>> *>(VT_PROJECTILEDELTAS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int64_t>(verifier, VT_TIMESTAMP) &&
           VerifyField<float>(verifier, VT_FUEL) &&
           VerifyOffset(verifier, VT_REMOVED) &&
           verifier.VerifyVector(removed()) &&
           VerifyOffset(verifier, VT_SHIPMETAS) &&
           verifier.VerifyVector(shipMetas()) &&
           verifier.VerifyVectorOfTables(shipMetas()) &&
           VerifyOffset(verifier, VT_PROJECTILEMETAS) &&
           verifier.VerifyVector(projectileMetas()) &&
           verifier.VerifyVectorOfTables(projectileMetas()) &&
           VerifyOffset(verifier, VT_SHIPDELTAS) &&
           verifier.VerifyVector(shipDeltas()) &&
           verifier.VerifyVectorOfTables(shipDeltas()) &&
           VerifyOffset(verifier, VT_PROJECTILEDELTAS) &&
           verifier.VerifyVector(projectileDeltas()) &&
           verifier.VerifyVectorOfTables(projectileDeltas()) &&
           verifier.EndTable();
  }
};

struct PerceptionBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_timestamp(int64_t timestamp) {
    fbb_.AddElement<int64_t>(Perception::VT_TIMESTAMP, timestamp, 0);
  }
  void add_fuel(float fuel) {
    fbb_.AddElement<float>(Perception::VT_FUEL, fuel, 0.0f);
  }
  void add_removed(flatbuffers::Offset<flatbuffers::Vector<uint32_t>> removed) {
    fbb_.AddOffset(Perception::VT_REMOVED, removed);
  }
  void add_shipMetas(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ShipMetadata>>> shipMetas) {
    fbb_.AddOffset(Perception::VT_SHIPMETAS, shipMetas);
  }
  void add_projectileMetas(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ProjectileMetadata>>> projectileMetas) {
    fbb_.AddOffset(Perception::VT_PROJECTILEMETAS, projectileMetas);
  }
  void add_shipDeltas(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ShipDelta>>> shipDeltas) {
    fbb_.AddOffset(Perception::VT_SHIPDELTAS, shipDeltas);
  }
  void add_projectileDeltas(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ProjectileDelta>>> projectileDeltas) {
    fbb_.AddOffset(Perception::VT_PROJECTILEDELTAS, projectileDeltas);
  }
  explicit PerceptionBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  PerceptionBuilder &operator=(const PerceptionBuilder &);
  flatbuffers::Offset<Perception> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Perception>(end);
    return o;
  }
};

inline flatbuffers::Offset<Perception> CreatePerception(
    flatbuffers::FlatBufferBuilder &_fbb,
    int64_t timestamp = 0,
    float fuel = 0.0f,
    flatbuffers::Offset<flatbuffers::Vector<uint32_t>> removed = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ShipMetadata>>> shipMetas = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ProjectileMetadata>>> projectileMetas = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ShipDelta>>> shipDeltas = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ProjectileDelta>>> projectileDeltas = 0) {
  PerceptionBuilder builder_(_fbb);
  builder_.add_timestamp(timestamp);
  builder_.add_projectileDeltas(projectileDeltas);
  builder_.add_shipDeltas(shipDeltas);
  builder_.add_projectileMetas(projectileMetas);
  builder_.add_shipMetas(shipMetas);
  builder_.add_removed(removed);
  builder_.add_fuel(fuel);
  return builder_.Finish();
}

inline flatbuffers::Offset<Perception> CreatePerceptionDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int64_t timestamp = 0,
    float fuel = 0.0f,
    const std::vector<uint32_t> *removed = nullptr,
    const std::vector<flatbuffers::Offset<ShipMetadata>> *shipMetas = nullptr,
    const std::vector<flatbuffers::Offset<ProjectileMetadata>> *projectileMetas = nullptr,
    const std::vector<flatbuffers::Offset<ShipDelta>> *shipDeltas = nullptr,
    const std::vector<flatbuffers::Offset<ProjectileDelta>> *projectileDeltas = nullptr) {
  auto removed__ = removed ? _fbb.CreateVector<uint32_t>(*removed) : 0;
  auto shipMetas__ = shipMetas ? _fbb.CreateVector<flatbuffers::Offset<ShipMetadata>>(*shipMetas) : 0;
  auto projectileMetas__ = projectileMetas ? _fbb.CreateVector<flatbuffers::Offset<ProjectileMetadata>>(*projectileMetas) : 0;
  auto shipDeltas__ = shipDeltas ? _fbb.CreateVector<flatbuffers::Offset<ShipDelta>>(*shipDeltas) : 0;
  auto projectileDeltas__ = projectileDeltas ? _fbb.CreateVector<flatbuffers::Offset<ProjectileDelta>>(*projectileDeltas) : 0;
  return spac::net::CreatePerception(
      _fbb,
      timestamp,
      fuel,
      removed__,
      shipMetas__,
      projectileMetas__,
      shipDeltas__,
      projectileDeltas__);
}

struct Packet FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_MESSAGE_TYPE = 4,
    VT_MESSAGE = 6
  };
  Message message_type() const {
    return static_cast<Message>(GetField<uint8_t>(VT_MESSAGE_TYPE, 0));
  }
  const void *message() const {
    return GetPointer<const void *>(VT_MESSAGE);
  }
  template<typename T> const T *message_as() const;
  const Perception *message_as_Perception() const {
    return message_type() == Message_Perception ? static_cast<const Perception *>(message()) : nullptr;
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_MESSAGE_TYPE) &&
           VerifyOffset(verifier, VT_MESSAGE) &&
           VerifyMessage(verifier, message(), message_type()) &&
           verifier.EndTable();
  }
};

template<> inline const Perception *Packet::message_as<Perception>() const {
  return message_as_Perception();
}

struct PacketBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_message_type(Message message_type) {
    fbb_.AddElement<uint8_t>(Packet::VT_MESSAGE_TYPE, static_cast<uint8_t>(message_type), 0);
  }
  void add_message(flatbuffers::Offset<void> message) {
    fbb_.AddOffset(Packet::VT_MESSAGE, message);
  }
  explicit PacketBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  PacketBuilder &operator=(const PacketBuilder &);
  flatbuffers::Offset<Packet> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Packet>(end);
    return o;
  }
};

inline flatbuffers::Offset<Packet> CreatePacket(
    flatbuffers::FlatBufferBuilder &_fbb,
    Message message_type = Message_NONE,
    flatbuffers::Offset<void> message = 0) {
  PacketBuilder builder_(_fbb);
  builder_.add_message(message);
  builder_.add_message_type(message_type);
  return builder_.Finish();
}

inline bool VerifyMessage(flatbuffers::Verifier &verifier, const void *obj, Message type) {
  switch (type) {
    case Message_NONE: {
      return true;
    }
    case Message_Perception: {
      auto ptr = reinterpret_cast<const Perception *>(obj);
      return verifier.VerifyTable(ptr);
    }
    default: return false;
  }
}

inline bool VerifyMessageVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types) {
  if (!values || !types) return !values && !types;
  if (values->size() != types->size()) return false;
  for (flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
    if (!VerifyMessage(
        verifier,  values->Get(i), types->GetEnum<Message>(i))) {
      return false;
    }
  }
  return true;
}

inline const spac::net::Packet *GetPacket(const void *buf) {
  return flatbuffers::GetRoot<spac::net::Packet>(buf);
}

inline const spac::net::Packet *GetSizePrefixedPacket(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<spac::net::Packet>(buf);
}

inline bool VerifyPacketBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<spac::net::Packet>(nullptr);
}

inline bool VerifySizePrefixedPacketBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<spac::net::Packet>(nullptr);
}

inline void FinishPacketBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<spac::net::Packet> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedPacketBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<spac::net::Packet> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace net
}  // namespace spac

#endif  // FLATBUFFERS_GENERATED_PACKET_SPAC_NET_H_
