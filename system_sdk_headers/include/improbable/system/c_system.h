// Confidential and only for the information of the intended recipient.
// Copyright 2023 Improbable Worlds Limited.

#ifndef LATTICE_SYSTEM_H
#define LATTICE_SYSTEM_H

#include <improbable/system/c_system_error.h>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

#ifndef DLL_PUBLIC
#ifdef _WIN32
#define DLL_PUBLIC __declspec(dllexport)
#else
#define DLL_PUBLIC __attribute__((visibility("default")))
#endif  // _WIN32
#endif  // DLL_PUBLIC

/*
 * VERSIONING
 */
#define SYSTEM_API_VERSION_MAJOR 0
#define SYSTEM_API_VERSION_MINOR 0
#define SYSTEM_API_VERSION_PATCH 0

/*
 * DECLARATIONS
 */

/*
 * The System EntityIterator is the way in which the System identifies itself when making calls to
 * the Runtime. The System EntityIterator is provided by the Runtime to the System when the System's
 * callback is called. It iterates through the entities that the System is responsible for updating
 * components on this tick.
 *
 * The EntityIterator's lifetime is restricted to the duration of the current tick. I.e. it is only
 * valid within the tick callback that provides it, see System_TickCallbackType.
 */
typedef struct System_EntityIterator_Data* System_EntityIterator;
typedef struct System_Handle_Data* System_Handle;

typedef uint32_t System_Id;
typedef uint32_t System_ComponentId;
typedef uint32_t System_EntityIndex;
typedef int64_t System_RequestId;

/*
 * INITIALISATION/DESTRUCTION
 * -----------------------------------------------------------------------------
 */

/*
 * Allocates a System Handle.
 */
DLL_PUBLIC System_Handle System_Init();

/*
 * Destroys a System Handle.
 */
DLL_PUBLIC void System_Destroy(System_Handle handle);

/*
 * ITERATION
 * -----------------------------------------------------------------------------
 */

DLL_PUBLIC bool System_IterationFinished(const System_EntityIterator entity_iterator);

/*
 * Increments the EntityIterator to point to the next Entity that
 * this System is responsible for updating Components on.
 */
DLL_PUBLIC System_StatusCode System_NextEntity(System_EntityIterator entity_iterator);

/*
 * Creates a copy of the System EntityIterator to allow for batched stores.
 * I.e. A user can use the initial EntityIterator to iterate through the entities
 * getting the components, perform an update on the batch, then iterate
 * through the entities storing the results using the copy of the original
 * EntityIterator.
 *
 * The copied System EntityIterator has the same lifetime as the System EntityIterator
 * being copied, that is the duration of the current tick.
 */
DLL_PUBLIC System_StatusCode System_CopyEntityIterator(const System_EntityIterator entity_iterator,
                                                       System_EntityIterator* entity_iterator_copy);

/*
 * GETTING AND STORING COMPONENTS
 * -----------------------------------------------------------------------------
 */

/*
 * For the current entity copies the current value of the specified component
 * into the user provided buffer component_data_out, which must be a valid
 * block of memory. The component_data_size must be the size of the provided
 * buffer, and must match the size of the Component data stored by the Runtime.
 *
 * The component_data_out buffer will be populated with the Component data
 * in ECS inlined format. To see how to generate code in this format, see here:
 * https://docs.production-intinf-eu2.int.improbabledb.io/skyral/2022.1/simfunction-reference/component-data-format/
 */
DLL_PUBLIC System_StatusCode System_GetComponent(const System_EntityIterator entity_iterator,
                                                 System_ComponentId component_id,
                                                 uint8_t* component_data_out,
                                                 uint32_t component_data_size);

/*
 * For the current entity, validates that the
 * System has write access to the Component, and ensures the component data is
 * valid before copying it from user provided buffer into the Runtime's
 * database. The component_data_size, must be the size of the provided buffer,
 * and must match the size of the Component data expected by the Runtime.
 *
 * The component_data_in buffer must provide the Component data in ECS inlined
 * format. To see how to generate code in this format, see here:
 * https://docs.production-intinf-eu2.int.improbabledb.io/skyral/2022.1/simfunction-reference/component-data-format/

 *
 * The updated Component data will not be visible to Systems on the current
 * tick but will be visible on their next tick.
 */
DLL_PUBLIC System_StatusCode System_UpdateComponent(System_EntityIterator entity_iterator,
                                                    System_ComponentId component_id,
                                                    uint8_t* component_data_in,
                                                    uint32_t component_data_size);

/*
 * Add and Remove Components, Create and Delete Entities
 * -----------------------------------------------------------------------------
 */

/*
 * Remove a Component from the entity that the EntityIterator refers to.
 *
 * For this to be successful the current System must have write access
 * to the Component being removed.
 *
 * The removed Component data will be visible to Systems on the current
 * tick but will be removed before their next tick.
 */
DLL_PUBLIC System_StatusCode System_RemoveComponent(System_EntityIterator entity_iterator,
                                                    System_ComponentId component_id);

/*
 * The component_data must be provided in ECS inlined
 * format. To see how to generate code in this format, see here:
 * https://docs.production-intinf-eu2.int.improbabledb.io/skyral/2022.1/simfunction-reference/component-data-format/
 */
typedef struct System_ComponentInstance {
  System_ComponentId component_id;
  uint8_t* component_data;
  uint32_t component_data_size;
  const char* layer_name;
} System_ComponentInstanceType;

/*
 * Add a Component to the entity that the EntityIterator refers to.
 *
 * For this to be successful the current System must have write access over
 * Components of this type. (Defined in the System's configuration file).
 * The writer for this component will be set to a System from the same layer
 * as the System making the System_AddComponent request.
 * The Component data and size are also validated.
 *
 * The added Component data will not be visible to Systems on the current
 * tick but will be available on their next tick.
 */
DLL_PUBLIC System_StatusCode System_AddComponent(System_EntityIterator entity_iterator,
                                                 System_ComponentInstanceType* component_instance);

/*
 * Delete the entity that the EntityIterator refers to
 *
 * The Entity will be deleted at the end of the current tick, after all updates
 * have taken place, hence the entity and its components will be visible to
 * Systems on the current tick, but will be removed before they are ticked
 * again.
 */
DLL_PUBLIC System_StatusCode System_DeleteEntity(System_EntityIterator entity_iterator);

/*
 * Create a new Entity, a pointer to an array containing the info needed to
 * create the intial components is required.
 *
 * As this Entity is being created it is permissible for the System to add
 * Components to it that it would not normally have write access over.
 *
 * The new Entity will not be visible to Systems on the current tick, but
 * will be visible on their next tick.
 */
DLL_PUBLIC System_StatusCode System_CreateEntity(System_Handle system_handle,
                                                 System_ComponentInstanceType* initial_components,
                                                 uint32_t initial_component_count);

/*
 * CHANGING THE WRITER
 * -----------------------------------------------------------------------------
 */

/*
 * Request an writer change for a component on the current Entity to a System
 * that is part of a specified Layer.
 *
 * A Layer is a group of Systems that is associated with a load balancing
 * strategy, the load balancing strategy splits the computation between the
 * Systems in that Layer.
 *
 * As Systems are unaware of the load balancing strategy within a
 * particular Layer, including their own Layer, writer changes must be on a
 * per-layer basis.
 *
 * If successful the writer for this component will be updated the next time a
 * System is ticked after the current tick.
 */
DLL_PUBLIC System_StatusCode System_SetWriter(System_EntityIterator entity_iterator,
                                              System_ComponentId component_id,
                                              const char* system_layer);

/*
 * LOGGING
 * -----------------------------------------------------------------------------
 */

typedef enum System_LogLevel {
  LOG_LEVEL_TRACE = 0,
  LOG_LEVEL_DEBUG = 1,
  LOG_LEVEL_INFO = 2,
  LOG_LEVEL_WARN = 3,
  LOG_LEVEL_ERROR = 4,
  LOG_LEVEL_ALWAYS = 6,
} System_LogLevel;

typedef struct System_LogMessageInfo {
  System_LogLevel level;  // Level of the message
  const char* message;    // Null terminated message string
} System_LogMessageInfoType;

/*
 * Log a message.
 */
DLL_PUBLIC System_StatusCode System_SendLogMessage(System_Handle system_handle,
                                                   System_LogMessageInfoType* msg_info);

/*
 * RUNNING A SYSTEM
 * -----------------------------------------------------------------------------
 */

/*
 * System_TickCallbackType
 *
 * System's implement a callback with this signature, which the Runtime
 * will call at the interval specified in the System's configuration file.
 * The callback will be provided with a user specified context and a iterator
 * which allows the System to iterate through the entities containing components
 * it needs to update.
 * Ticks fired is used when the System falls behind (only applicable for Real
 * Time simulations) it will be set to the number of 'ticks' that have taken
 * place since the callback was last called. This gives the System the chance
 * to catch up.
 *
 * Callbacks must return SYSTEM_STATUS_CODE_SUCCESS if they complete
 * successfully. If an non success status code is returned then the simulation
 * will be aborted, so the callback must only return a non success error code
 * in the case of a fatal error.
 */
typedef System_StatusCode (*System_TickCallbackType)(System_Handle system_handle,
                                                     System_EntityIterator entity_iterator,
                                                     void* user_context, uint32_t ticks_fired);

/*
 * Registers a System and its callback, along with an optional user context
 * passed to the callback on each tick
 * The System will be ticked at the rate defined in the configuration file
 *
 * System_Run blocks until the Simulation finishes
 */
DLL_PUBLIC System_StatusCode System_Run(int major_version, int minor_version, int patch_version,
                                        System_Handle handle,
                                        System_TickCallbackType system_tick_cb, void* user_context);

/*
 * SYSTEM_RUN Wraps System_Run and populates the API version information for the
 * user
 */
#define SYSTEM_RUN(system_handle, system_tick_cb, user_context)                            \
  System_Run(SYSTEM_API_VERSION_MAJOR, SYSTEM_API_VERSION_MINOR, SYSTEM_API_VERSION_PATCH, \
             (system_handle), (system_tick_cb), (user_context))

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  // LATTICE_SYSTEM_H
