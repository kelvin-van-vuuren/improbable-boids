// Confidential and only for the information of the intended recipient.
// Copyright 2023 Improbable Worlds Limited.

#ifndef LATTICE_SYSTEM_QUERY_H
#define LATTICE_SYSTEM_QUERY_H

#include <improbable/system/c_system.h>
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

/* ---------------------------------------------------------------------------------------------- *
 * QUERYING                                                                                       *
 *                                                                                                *
 * Queries are used to retrieve information about entities in the simulation. Queries execute     *
 * over all entities in the simulation and provide read-only results via an iterator-based        *
 * interface.                                                                                     *
 * ---------------------------------------------------------------------------------------------- */

typedef struct System_Query_Handle_Data* System_Query_Handle;

/* ---------------------------------------------------------------------------------------------- *
 * CONSTRAINTS                                                                                    *
 * ---------------------------------------------------------------------------------------------- */

typedef enum System_Query_ConstraintType {
  // An absolute sphere constraint will match any entities whose `Position` component is within the
  // bounds of the given sphere.
  SYSTEM_QUERY_CONSTRAINT_TYPE_ABSOLUTE_SPHERE,
  // An entity index constraint will match only the entity with the given index. If the entity index
  // is in range, this constraint will always return a match, even if the entity has no components.
  // WARNING: We will work with entity indices for now; this is likely to change in the future.
  SYSTEM_QUERY_CONSTRAINT_TYPE_ENTITY_INDEX,
  // A component constraint will match all entities that have the given component id.
  SYSTEM_QUERY_CONSTRAINT_TYPE_COMPONENT,
} System_Query_ConstraintType;

typedef struct System_Double3 {
  double x;
  double y;
  double z;
} System_Double3;

typedef struct System_Query_Constraint_AbsoluteSphere {
  System_Double3 center;
  double radius;
} System_Query_Constraint_AbsoluteSphere;

typedef struct System_Query_Constraint_EntityIndex {
  System_EntityIndex entity_index;
} System_Query_Constraint_EntityIndex;

typedef struct System_Query_Constraint_Component {
  System_ComponentId component_id;
} System_Query_Constraint_Component;

typedef struct System_Query_Constraint {
  uint8_t constraint_type;
  System_Query_Constraint_AbsoluteSphere absolute_sphere_constraint;
  System_Query_Constraint_EntityIndex entity_index_constraint;
  System_Query_Constraint_Component component_constraint;
} System_Query_Constraint;

/* Helper functions to create constraints */
DLL_PUBLIC System_Query_Constraint
System_Query_Constraint_CreateAbsoluteSphere(System_Double3 center, double radius);
DLL_PUBLIC System_Query_Constraint
System_Query_Constraint_CreateEntityIndex(System_EntityIndex entity_index);
DLL_PUBLIC System_Query_Constraint
System_Query_Constraint_CreateComponent(System_ComponentId component_id);

/* ---------------------------------------------------------------------------------------------- *
 * QUERY INITIALISATION AND DESTRUCTION                                                           *
 * ---------------------------------------------------------------------------------------------- */

/*
 * Creates a query. If creation fails, a nullptr is returned. After use, the returned handle must be
 * destroyed with System_Query_Destroy. Ownership of the constraint is not transferred. The query
 * iterator will point to the first entity that matches the query. Saving queries between tick
 * callbacks is strongly discouraged: a query's view of the world is not updated dynamically.
 * Instead, create a new query each tick.
 */
DLL_PUBLIC System_Query_Handle System_Query_Create(System_Query_Constraint* constraint);

/*
 * Destroys a query, freeing associated resources.
 */
DLL_PUBLIC System_StatusCode System_Query_Destroy(System_Query_Handle query_handle);

/* ---------------------------------------------------------------------------------------------- *
 * QUERY RESULT ITERATION                                                                         *
 * ---------------------------------------------------------------------------------------------- */

/*
 * Increments the iterator to the next entity which matches the query.
 */
DLL_PUBLIC System_StatusCode System_Query_NextEntity(System_Query_Handle query_handle);

/*
 * Returns whether the iterator has reached its end, i.e., all query matches have been returned.
 */
DLL_PUBLIC bool System_Query_IterationFinished(System_Query_Handle query_handle);

/* ---------------------------------------------------------------------------------------------- *
 * QUERY RESULT ACCESS                                                                            *
 * ---------------------------------------------------------------------------------------------- */

/*
 * For the current entity that matches the query, copies the current value of the specified
 * component into the `component_data_out` buffer in inline schema format. `component_data_out`
 * must be a valid block of memory. `component_data_size` must be the size of the provided buffer;
 * this must match the size of the component data stored by the runtime.
 */
DLL_PUBLIC System_StatusCode System_Query_GetComponent(System_Query_Handle query_handle,
                                                       System_ComponentId component_id,
                                                       uint8_t* component_data_out,
                                                       uint32_t component_data_size);

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  // LATTICE_SYSTEM_QUERY_H
