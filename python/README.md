# Python Plugin

Python support for dynamic programming.

> Python plugin doesn't support `unit` data type and `float` type is used instead.

## Programs
Dynamic program generated from Python source code. Called (function `call`) once in each iteration per object that uses this program.

```python
# Optional initialization code

# Required function prototype.
# object: Object that owns this program.
# simulation: Current simulation.
# dt: Current iteration time step.
def call(object, simulation, dt):
    pass
```

## Modules

Dynamic modules generated from Python source code. Module's `update` function is called once in each iteration.

```python
# Optional initialization code

# Optional function prototype
# @param simulator.Simulation    simulation Current simulation.
# @param simulator.Configuration config     Configuration from XML file.
def configure(simulation, config):
    pass

# Recommended function prototype
# @param simulator.Simulation simulation Current simulation.
# @param float                dt         Current iteration  time step.
def update(simulation, dt):
    pass

# Optional function prototype.
# @param render.Context       context    Rendering context.
# @param simulator.Simulation simulation Current simulation.
def draw(context, simulation):
    pass
```

## Classes

There are some wrappers around simulator core classes.

### class `simulator.Configuration`

Configuration class.

#### Methods:

| Name  | Signature                                  | Description                                   |
| ------| ------------------------------------------ | --------------------------------------------- |
| `get` | `string(name: string [, default: scalar])` | Returns configuration value under given name. |


### class `simulator.Simulation`

Main simulation class that contains everything about simulation.

#### Properties:

| Name           | Type            | Description                  |
| -------------- | --------------- | ---------------------------- |
| `worldSize`    | `vector[float]` | Simulation scene/world size. |
| `iteration`    | `uint`          | Current iteration number.    |
| `iterations`   | `uint`          | Total number of required iterations. Can be `0`, if there is no limitation. |
| `timeStep`     | `float`         | Simulation time step.        |
| `totalTime`    | `float`         | Total time spend in simulation in seconds. |
| `objectsCount` | `uint`          | Number of objects in scene.  |

#### Methods:

| Name              | Signature                                          | Description   |
| ----------------- | -------------------------------------------------- | --------- |
| `getModule`       | `simulator.Module(typename: string)`               | Returns required module. If module is not used, it is created by with default configuration. |
| `createObject`    | `simulator.Object(typename: string [, type: int])` | Create a new object. The first argument is class name and second one is if object should be static (non-movable). |
| `objectCountType` | `int(typename: string)`                            | Count number of objects with given typename. |
| `getParameter`    | `string(name: string [, default: string])`         | Returns simulation parameter. |
| `getObject`       | `simulator.Object(index: int)`                     | Returns an simulation object by index from range [0, `objectCount`] |

### class `simulator.Module`

Base class for all modules. It doesn't offer anything.

### class `simulator.Object`

Basic simulation object.

#### Properties:

| Name           | Type            | Description                  |
| -------------- | --------------- | ---------------------------- |
| `id`           | `int`           | Grid size.                   |
| `position`     | `vector[float]` | Number of different signals. |
| `rotation`     | `float`         | Object rotation in radians.  |
| `velocity`     | `vector[float]` | Object velocity.             |
| `density`      | `float`         | Object density.              |
| `mass`         | `float`         | Object mass.                 |
| `typeName`     | `string`        | Object typename. Contains name of custom type if it's used. |
| `realTypeName` | `string`        | Original typename without user defined type. |

#### Methods:

| Name         | Signature                       | Description                            |
| ------------ | ------------------------------- | -------------------------------------- |
| `applyForce` | `void(force: core.VectorForce)` | Add program with given name to object. |
| `useProgram` | `void(name: string)`            | Add program with given name to object. |
| `destroy`    | `void(void)`                    | Destroys object. |

## Functions

All functions can be found in `core` module.

```python
cell.volume = core.um3(30);
```

* `m`         - Create a length unit value from meters.
* `mm`        - Create a length unit value from millimeters.
* `um`        - Create a length unit value from micrometers.
* `kg`        - Create a mass unit value from kilograms.
* `g`         - Create a mass unit value from grams.
* `mg`        - Create a mass unit value from milligrams.
* `ug`        - Create a mass unit value from micrograms.
* `ng`        - Create a mass unit value from nanograms.
* `pg`        - Create a mass unit value from pikograms.
* `s`         - Create a time unit value from seconds.
* `ms`        - Create a time unit value from milliseconds.
* `us`        - Create a time unit value from microseconds.
* `min`       - Create a time unit value from minutes.
* `h`         - Create a time unit value from hours.
* `m2`        - Create a area unit value from meters square.
* `mm2`       - Create a area unit value from millimeters square.
* `um2`       - Create a area unit value from micrometers square.
* `m3`        - Create a volume unit value from cubic meters.
* `mm3`       - Create a volume unit value from cubic millimeters.
* `um3`       - Create a volume unit value from cubic micrometers.
* `m_s`       - Create a velocity unit value from meters per second.
* `mm_s`      - Create a velocity unit value from millimeters per second.
* `um_s`      - Create a velocity unit value from micrometers per second.
* `m_s2`      - Create a acceleration unit value from meters per second square.
* `mm_s2`     - Create a acceleration unit value from millimeters per second square.
* `um_s2`     - Create a acceleration unit value from micrometers per second square.
* `kgm_s2`    - Create a force unit value from kilogram meters per second square.
* `gm_s2`     - Create a force unit value from gram meters per second square.
* `mgm_s2`    - Create a force unit value from milligram meters per second square.
* `N`         - Create a force unit value from newtons.
* `mN`        - Create a force unit value from millinewtons.
* `uN`        - Create a force unit value from micronewtons.
* `kg_ms`     - Create a dynamic viscosity unit value from kilograms per meter and second.
* `g_ms`      - Create a dynamic viscosity unit value from grams per meter and second.
* `Ns_m2`     - Create a dynamic viscosity unit value from newton seconds per meter square.
* `Pas`       - Create a dynamic viscosity unit value from pascal seconds.
* `mPas`      - Create a dynamic viscosity unit value from millipascal seconds.
* `m2_s`      - Create a kinematic viscosity unit value from meter square per second.
* `mm2_s`     - Create a kinematic viscosity unit value from millimeter square per second.
* `um2_s`     - Create a kinematic viscosity unit value from micrometer square per second.
* `mol`       - Create an amount of substance unit value from mole.
* `mmol`      - Create an amount of substance unit value from millimole.
* `umol`      - Create an amount of substance unit value from micromole.
* `nmol`      - Create an amount of substance unit value from nanomole.
* `mol_m3`    - Create a molar concentration unit value from mole per cubic meter.
* `mmol_m3`   - Create a molar concentration unit value from millimole per cubic meter.
* `umol_m3`   - Create a molar concentration unit value from micromole per cubic meter.
* `nmol_m3`   - Create a molar concentration unit value from nanomole per cubic meter.
* `mol_um3`   - Create a molar concentration unit value from mole per cubic micrometer.
* `mmol_um3`  - Create a molar concentration unit value from millimole per cubic micrometer.
* `umol_um3`  - Create a molar concentration unit value from micromole per cubic micrometer.
* `nmol_um3`  - Create a molar concentration unit value from nanomole per cubic micrometer.
* `mM`        - Create a molar concentration unit value from millimolar.
* `uM`        - Create a molar concentration unit value from micromolar.
* `nM`        - Create a molar concentration unit value from nanomolar.
* `pM`        - Create a molar concentration unit value from picomolar.
* `rad`       - Create an angle unit value from radians.
* `deg`       - Create an angle unit value from degrees.
* `precent`   - Create a probability value from precent.
