# Lattice Developer Tools

The developer tools package contains utilities to help develop, debug and test Lattice simulations.

## State Verifier

The state verifier is used to inspect the state of the simulation.
It is expected to be used by a Test System, within a Simulation where all
entities have a TestComponent configured, which the Test System has set as its
write component.

The Tick rate of the Test System determines when the checks are ran.

Checks can be added using the State Verifier. Checks specify:

* A Test System Tick on which to run the check
* An Entity Index to check
* A component type to check
* A function, which returns success if the check has passed

The Simulation is aborted early if, one of the checks fails, or can't be run due
to the specified entity not being part of the simulation.

## Digital Rain System

The digital rain system is a read-only text-based inspector. It queries the standard library position of all entities
and renders
them as text to standard output. It is extremely useful for visualising the spatial state of a simulation. Digital Rain
is a 2D visualiser, but standard library simulations are 3D. We map the 3D x and z axes to the 2D x and y axes respectively.

Using the digital rain system is as simple as adding the system specification as the final entry to the simulation configuration:

```json
{
  "system_bundle_uri": "/path/to/digital_rain_system_bundle.zip",
  "system_info": {
    "layer": "digital_rain_layer",
    "command": "./digital_rain_system_binary <visualiser character width> <visualiser character height> <world minimum x> <world maximum x> <world minimum z> <world maximum z> <render (default=1)>",
    "tick_rate": "0.1s",
    "write_components": [
      54
    ]
  },
  "load_balancing": {
    "entity_id_sharding": {
      "num_shards": 1
    }
  }
}
```

Sensible defaults are as follows:

- Visualiser character width: something less than the width of your terminal, e.g., 50 characters.
- Visualiser character height: something less than the height of your terminal, e.g., 50 characters.
- World minimum x: 0
- World maximum x: 100
- World minimum z: 0
- World maximum z: 100
- Render: 1 (use 0 to disable rendering)

We recommend keeping the aspect ratio between the world and the visualiser the same, otherwise the visualisation will be
distorted.
