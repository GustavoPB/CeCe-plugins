# Streamlines Plugin

Module that simulate streamlines in whole scene. It using Lattice Boltzman method to calculate velocities in simulation grid. Dynamic obstacles (i.e. objects) are affected by computed velocities.

There are some limitations that come from Lattice Boltzman method. Maximum velocity is limited by grid size, time step and scene/world size. If the velocity is higher than this limit, it's changed to the maximum otherwise the streamlines simulation crashes. Eventhough Lattice Boltzman method is powerfull the proper parameter setting is hard.

## Example:

```xml
<module name="streamlines" inlet-velocity="300um/s" kinematic-viscosity="0.658mm2/s" />
```

## Streamlines module

### Parameters:

| Name                  | Type                      | Default     | Description |
| --------------------- | ------------------------- | ----------- | ----------- |
| `dynamic`             | `boolean`                 | `true`      | Dynamic update of streamlines during simulation. For initialized streamlines without need of update, can be set to `false` to safe the CPU time. |
| `init-iterations`     | `int`                     | `0`         | Number of initial iteration that setup the streamlines. |
| `inner-iterations`    | `int`                     | `1`         | Number of internal iteration within simulation iteration. |
| <del>`grid`</del>     | <del>`vector[int]`</del>  | -           | <del>Grid size.</del> |
| `layout`              | 4x `string`    | `barrier outlet barrier inlet` | Defines scene layout. Four values define layout in order: **top** **right** **bottom** **left**. Possible values are: `none`, `inlet`, `outlet`. |
| `inlet-velocity`      | `unit[m/s]`    | `0`         | Velocity for all inlets. |
| `inlet-velocities`    | 4x `unit[m/s]` | `0 0 0 0`   | Specific inlet velocities for layout. |
| `inlet-type`          | `string`       | `constant`         | Inlet velocity profile type. |
| `inlet-types`         | 4x `string`    | `constant constant constant constant`   | Inlet velocity profile types for layout. |
| `kinematic-viscosity` | `unit[m2/s]`   | `0.658mm/s` | Fluid kinematic viscosity. |
| `init-file`           | `filename`     | -           | Path to precomputed streamlines file. Expression '%temp%' can be used to substitute OS's temporary directory. |
| `char-length`         | `unit[m]`      | `1um`       | Characteristic length. |
| `number-nodes`        | `int`          | `1`         | Number of simulation nodes per characteristic length. |
| `tau`                 | `float`        | -           | LBM Relaxation parameter. |
| `layer-magnitude`     | `name`         | -           | Name of visualization layer used for velocity rendering. |
| `layer-density`       | `name`         | -           | Name of visualization layer used for density rendering. |
| `layer-dynamics`      | `name`         | -           | Name of visualization layer used for fluid nodes rendering. |
| `height`              | `unit[m]`      | -           | Channel height - only used for conversion between volumeric flow rate and velocity. |

### Boundary

`layout` and other parameters don't allow to specify complex boundaries so it's possible define them with boundary element:

### Example:

```xml
<module name="streamlines">
  <boundary name="b1" type="inlet" position="top" offset="-50um" size="100um" inlet-profile="constant" inlet-velocity="50um/s" />
</module>
```

### Parameters:

| Name              | Type         | Default             | Description |
| ----------------- | ------------ | ------------------- | ----------- |
| `name`            | `string`     | -                   | Boundary name. |
| `type`            | `string`     | -                   | Boundary type. One of `inlet`, `outlet` or `none`. |
| `position`        | `string`     | -                   | Boundary position. One of `top`, `bottom`, `left`, `right`. |
| `offset`          | `unit[m]`    | `0`                 | Offset from center of the position. |
| `size`            | `unit[m]`    | whole position size | Boundary size. |
| `inlet-profile`   | `string`     | `auto`              | Velocity profile type. One of `auto`, `constant` (other can be added in the future). `auto` currently means `constant`. |
| `inlet-velocity`  | `unit[m/s]`  | `0`                 | Inlet velocity. |
| `inlet-flow-rate` | `unit[m3/s]` | `0`                 | Inlet volumeric flow rate. With inlet size it's used for velocity calculation. |
