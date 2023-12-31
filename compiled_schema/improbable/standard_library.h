// Generated by Improbable codegen. DO NOT EDIT!
// source: improbable/standard_library.schema

#ifndef IMPROBABLE_SCHEMA_improbable_standard_library_schema_15375418286384799311_INCLUDED
#define IMPROBABLE_SCHEMA_improbable_standard_library_schema_15375418286384799311_INCLUDED

#include <improbable/worker/system/c_system.h>
#include <cstdint>

// ---------------------- Types ---------------------- 

struct Coordinates {
  double x;
  double y;
  double z;

  inline bool operator==(const Coordinates& other) const {
    return x == other.x && y == other.y && z == other.z;
  }

  inline bool operator!=(const Coordinates& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(Coordinates) == 24);

struct EdgeLength {
  double x;
  double y;
  double z;

  inline bool operator==(const EdgeLength& other) const {
    return x == other.x && y == other.y && z == other.z;
  }

  inline bool operator!=(const EdgeLength& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(EdgeLength) == 24);

struct ComponentInterest_BoxConstraint {
  Coordinates center;
  EdgeLength edge_length;

  inline bool operator==(const ComponentInterest_BoxConstraint& other) const {
    return center == other.center && edge_length == other.edge_length;
  }

  inline bool operator!=(const ComponentInterest_BoxConstraint& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(ComponentInterest_BoxConstraint) == 48);

struct ComponentInterest_CylinderConstraint {
  Coordinates center;
  double radius;

  inline bool operator==(const ComponentInterest_CylinderConstraint& other) const {
    return center == other.center && radius == other.radius;
  }

  inline bool operator!=(const ComponentInterest_CylinderConstraint& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(ComponentInterest_CylinderConstraint) == 32);

struct ComponentInterest_RelativeBoxConstraint {
  EdgeLength edge_length;

  inline bool operator==(const ComponentInterest_RelativeBoxConstraint& other) const {
    return edge_length == other.edge_length;
  }

  inline bool operator!=(const ComponentInterest_RelativeBoxConstraint& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(ComponentInterest_RelativeBoxConstraint) == 24);

struct ComponentInterest_RelativeCylinderConstraint {
  double radius;

  inline bool operator==(const ComponentInterest_RelativeCylinderConstraint& other) const {
    return radius == other.radius;
  }

  inline bool operator!=(const ComponentInterest_RelativeCylinderConstraint& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(ComponentInterest_RelativeCylinderConstraint) == 8);

struct ComponentInterest_RelativeSphereConstraint {
  double radius;

  inline bool operator==(const ComponentInterest_RelativeSphereConstraint& other) const {
    return radius == other.radius;
  }

  inline bool operator!=(const ComponentInterest_RelativeSphereConstraint& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(ComponentInterest_RelativeSphereConstraint) == 8);

struct ComponentInterest_SphereConstraint {
  Coordinates center;
  double radius;

  inline bool operator==(const ComponentInterest_SphereConstraint& other) const {
    return center == other.center && radius == other.radius;
  }

  inline bool operator!=(const ComponentInterest_SphereConstraint& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(ComponentInterest_SphereConstraint) == 32);

struct Persistence {
  static constexpr std::uint32_t kComponentId = 55;

  inline bool operator==(const Persistence&) const {
    return true;
  }

  inline bool operator!=(const Persistence& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(Persistence) == 1);

struct Position {
  static constexpr std::uint32_t kComponentId = 54;

  Coordinates coords;

  inline bool operator==(const Position& other) const {
    return coords == other.coords;
  }

  inline bool operator!=(const Position& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(Position) == 24);

#endif  // IMPROBABLE_SCHEMA_improbable_standard_library_schema_15375418286384799311_INCLUDED