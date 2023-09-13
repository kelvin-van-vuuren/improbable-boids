#include <improbable/standard_library.h>
#include <improbable/system/c_query.h>
#include <improbable/system/c_system.h>
#include <improbable/system/c_system_error.h>
#include <myschema.h>

#include <array>
#include <iostream>
#include <memory>

#include <random>

static constexpr double random_lower_bound = 0.1;
static constexpr double random_upper_bound = 0.2;
static constexpr double vision_radius = 1;
static constexpr double field_of_vision = 1;

namespace {

constexpr auto kEntityCount = 10;
const auto sideLength = static_cast<int>(sqrt(kEntityCount));

using RAIISystemHandle =
    std::unique_ptr<System_Handle_Data, decltype(&System_Destroy)>;

// A simple error-handling wrapper around the C API for sending log messages
void SendLogMessage(System_Handle system_handle, System_LogLevel level,
                    const std::string &log_message) {
  auto message = System_LogMessageInfo{level, log_message.c_str()};
  if (auto rc = System_SendLogMessage(system_handle, &message);
      rc != SYSTEM_STATUS_CODE_SUCCESS) {
    std::cerr << "System failed to send log message \"" << log_message.c_str()
              << "\" (received status code: " << rc << ")" << std::endl;
    exit(1);
  }
}

// A simple function that populates the simulation with entities
void CreateEntities(System_Handle system_handle) {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<> dist(random_lower_bound, random_upper_bound);
  double random_velocity_val = static_cast<double>(dist(rng));

  for (int i = 0; i < kEntityCount; ++i) {
    int row = i / sideLength;
    int col = i % sideLength;

    auto position = Position{
        Coordinates{static_cast<double>(row), 0, static_cast<double>(col)}};
    auto velocity = Velocity{random_velocity_val, 0, random_velocity_val};
    auto acceleration = Acceleration{0.1};

    auto position_component_instance = System_ComponentInstanceType{
        Position::kComponentId, reinterpret_cast<uint8_t *>(&position),
        sizeof(position), "movement_layer"};

    auto velocity_component_instance = System_ComponentInstanceType{
        Velocity::kComponentId, reinterpret_cast<uint8_t *>(&velocity),
        sizeof(velocity), "movement_layer"};

    auto acceleration_component_instance = System_ComponentInstanceType{
        Acceleration::kComponentId, reinterpret_cast<uint8_t *>(&acceleration),
        sizeof(acceleration), "movement_layer"};

    std::array<System_ComponentInstanceType, 3> components = {
        position_component_instance, velocity_component_instance,
        acceleration_component_instance};
    if (auto rc = System_CreateEntity(system_handle, components.data(),
                                      components.size());
        rc != SYSTEM_STATUS_CODE_SUCCESS) {
      std::cerr << "System failed to create entity (received status code: "
                << rc << ")" << std::endl;
      exit(1);
    }
  }
}

// The callback that fires every system tick
System_StatusCode TickCallback(System_Handle system_handle,
                               System_EntityIterator entity_iterator,
                               void *user_context, uint32_t ticks_fired) {

  SendLogMessage(system_handle, LOG_LEVEL_INFO, "My movement system ticking");
  // Create objects to store corresponding components of the current entity
  auto position = std::make_unique<Position>();
  auto velocity = std::make_unique<Velocity>();
  auto acceleration = std::make_unique<Acceleration>();

  // Iterate over the entity iterator
  while (!System_IterationFinished(entity_iterator)) {
    // Get the current entity's position component
    if (auto rc = System_GetComponent(
            entity_iterator, Position::kComponentId,
            reinterpret_cast<uint8_t *>(position.get()), sizeof(*position));
        rc != SYSTEM_STATUS_CODE_SUCCESS) {
      SendLogMessage(system_handle, LOG_LEVEL_ERROR,
                     "Failed to get current entity position");
      return SYSTEM_STATUS_CODE_ABORT;
    }

    // Get the current entity's velocity component
    if (auto rc = System_GetComponent(
            entity_iterator, Velocity::kComponentId,
            reinterpret_cast<uint8_t *>(velocity.get()), sizeof(*velocity));
        rc != SYSTEM_STATUS_CODE_SUCCESS) {
      SendLogMessage(system_handle, LOG_LEVEL_ERROR,
                     "Failed to get current entity velocity");
      return SYSTEM_STATUS_CODE_ABORT;
    }

    // Get the current entity's acceleration component
    if (auto rc = System_GetComponent(
            entity_iterator, Acceleration::kComponentId,
            reinterpret_cast<uint8_t *>(acceleration.get()), sizeof(*acceleration));
        rc != SYSTEM_STATUS_CODE_SUCCESS) {
      SendLogMessage(system_handle, LOG_LEVEL_ERROR,
                     "Failed to get current entity acceleration");
      return SYSTEM_STATUS_CODE_ABORT;
    }

    // Update the position component with a fixed velocity. While we hope that
    // `ticks_fired` is 1, the system may miss ticks when running in real-time
    // mode. We multiply the velocity by this value to compensate for missed
    // ticks.
    // position->coords.z += velocity->value * ticks_fired;

    // direction based on rules
    // get entities within range of current position
    // steer away from other boids
    // steer to move in same direction as nearby boids
    // steer towards center of nearby boids
    System_Double3 position_sd3{position->coords.x, position->coords.y,
                                position->coords.z};
    auto absolute_sphere_constraint =
        System_Query_Constraint_CreateAbsoluteSphere(position_sd3,
                                                     vision_radius);
    auto query_handle = std::unique_ptr<System_Query_Handle_Data,
                                        decltype(&System_Query_Destroy)>{
        System_Query_Create(&absolute_sphere_constraint), System_Query_Destroy};

    if (!query_handle) {
      SendLogMessage(system_handle, LOG_LEVEL_ERROR,
                     "Failed to create absolute sphere query result");
      return SYSTEM_STATUS_CODE_ERROR;
    }

    while (!System_Query_IterationFinished(query_handle.get())) {

      // Get neighbour boid's position component
      auto neighbour_position = std::make_unique<Position>();
      if (auto rc = System_Query_GetComponent(
              query_handle.get(), Position::kComponentId,
              reinterpret_cast<uint8_t *>(neighbour_position.get()),
              sizeof(*neighbour_position));
          rc != SYSTEM_STATUS_CODE_SUCCESS) {
        SendLogMessage(system_handle, LOG_LEVEL_ERROR,
                       "Failed to get neighbour boid's position");
        return SYSTEM_STATUS_CODE_ABORT;
      }

      // Get neighbour boid's direction component
      auto neighbour_acceleration = std::make_unique<Acceleration>();
      if (auto rc = System_Query_GetComponent(
              query_handle.get(), Acceleration::kComponentId,
              reinterpret_cast<uint8_t *>(neighbour_acceleration.get()),
              sizeof(*neighbour_acceleration));
          rc != SYSTEM_STATUS_CODE_SUCCESS) {
        SendLogMessage(system_handle, LOG_LEVEL_ERROR,
                       "Failed to get neighbour boid's acceleration");
        return SYSTEM_STATUS_CODE_ABORT;
      }

      // steer away from other boids
      // steer to move in same direction as nearby boids
      // steer towards center of nearby boids
      if (auto rc = System_Query_NextEntity(query_handle.get());
          rc != SYSTEM_STATUS_CODE_SUCCESS) {
        SendLogMessage(system_handle, LOG_LEVEL_ERROR,
                       "Failed to increment query handle");
        return SYSTEM_STATUS_CODE_ABORT;
      }
    }

    // Send updated position to Lattice
    if (auto rc = System_UpdateComponent(
            entity_iterator, Position::kComponentId,
            reinterpret_cast<uint8_t *>(position.get()), sizeof(Position));
        rc != SYSTEM_STATUS_CODE_SUCCESS) {
      SendLogMessage(system_handle, LOG_LEVEL_ERROR,
                     "Failed to update current entity position");
      return SYSTEM_STATUS_CODE_ABORT;
    }
    
    // Send updated velocity to Lattice
    if (auto rc = System_UpdateComponent(
            entity_iterator, Velocity::kComponentId,
            reinterpret_cast<uint8_t *>(velocity.get()), sizeof(Velocity));
        rc != SYSTEM_STATUS_CODE_SUCCESS) {
      SendLogMessage(system_handle, LOG_LEVEL_ERROR,
                     "Failed to update current entity velocity");
      return SYSTEM_STATUS_CODE_ABORT;
    }

    // // Send updated direction to Lattice
    // if (auto rc = System_UpdateComponent(
    //         entity_iterator, Direction::kComponentId,
    //         reinterpret_cast<uint8_t *>(direction.get()), sizeof(Direction));
    //     rc != SYSTEM_STATUS_CODE_SUCCESS) {
    //   SendLogMessage(system_handle, LOG_LEVEL_ERROR,
    //                  "Failed to update current entity position");
    //   return SYSTEM_STATUS_CODE_ABORT;
    // }

    // Advance the entity iterator
    if (auto rc = System_NextEntity(entity_iterator);
        rc != SYSTEM_STATUS_CODE_SUCCESS) {
      SendLogMessage(system_handle, LOG_LEVEL_ERROR,
                     "Failed to advance the entity iterator");
      return SYSTEM_STATUS_CODE_ABORT;
    }
  }

  return SYSTEM_STATUS_CODE_SUCCESS;
}

} // namespace

int main() {
  // Create system handle
  RAIISystemHandle system_handle{System_Init(), System_Destroy};
  SendLogMessage(system_handle.get(), LOG_LEVEL_INFO,
                 "My movement system started");

  // Populate the simulation
  CreateEntities(system_handle.get());

  // Run the system
  System_StatusCode run_status_code =
      SYSTEM_RUN(system_handle.get(), TickCallback, nullptr);
  SendLogMessage(system_handle.get(), LOG_LEVEL_INFO,
                 "My movement system finished with status code: " +
                     std::to_string(run_status_code));

  // Return an exit code based on the run status code
  const auto exit_code = run_status_code == SYSTEM_STATUS_CODE_SUCCESS ? 0 : 1;
  SendLogMessage(system_handle.get(), LOG_LEVEL_INFO,
                 "My movement system exiting with code: " +
                     std::to_string(run_status_code));
  return exit_code;
}
