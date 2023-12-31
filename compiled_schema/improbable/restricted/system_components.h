// Generated by Improbable codegen. DO NOT EDIT!
// source: improbable/restricted/system_components.schema

#ifndef IMPROBABLE_SCHEMA_improbable_restricted_system_components_schema_15576345259896213388_INCLUDED
#define IMPROBABLE_SCHEMA_improbable_restricted_system_components_schema_15576345259896213388_INCLUDED

#include <improbable/worker/system/c_system.h>
#include <cstdint>

// ---------------------- Enumerations ---------------------- 

enum Connection_ConnectionStatus {
  UNKNOWN = 0, AWAITING_WORKER_CONNECTION = 1, CONNECTED = 2, DISCONNECTED = 3,
};

// ---------------------- Types ---------------------- 

struct Connection {
  Connection_ConnectionStatus status;
  std::uint32_t data_latency_ms;
  std::uint64_t connected_since_utc;

  inline bool operator==(const Connection& other) const {
    return status == other.status && data_latency_ms == other.data_latency_ms && connected_since_utc == other.connected_since_utc;
  }

  inline bool operator!=(const Connection& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(Connection) == 16);

struct DisconnectRequest {
  inline bool operator==(const DisconnectRequest&) const {
    return true;
  }

  inline bool operator!=(const DisconnectRequest& other) const {
    return !operator==(other);
  }
};
static_assert((sizeof(DisconnectRequest) == 0) || (sizeof(DisconnectRequest) == 1));

struct DisconnectResponse {
  inline bool operator==(const DisconnectResponse&) const {
    return true;
  }

  inline bool operator!=(const DisconnectResponse& other) const {
    return !operator==(other);
  }
};
static_assert((sizeof(DisconnectResponse) == 0) || (sizeof(DisconnectResponse) == 1));

struct System {
  static constexpr std::uint32_t kComponentId = 59;

  inline bool operator==(const System&) const {
    return true;
  }

  inline bool operator!=(const System& other) const {
    return !operator==(other);
  }
};
static_assert(sizeof(System) == 1);

#endif  // IMPROBABLE_SCHEMA_improbable_restricted_system_components_schema_15576345259896213388_INCLUDED