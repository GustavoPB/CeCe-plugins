
# Diffusion Plugin

The main module adds support for global diffusion in simulation. Diffusion can handle any number of separate signals. They cannot affect each other.

## Example:

Enables diffusion with two signals named *GFP* and *RFP* with diffusion and degradation rates. Those signals have specified colors how they are rendered.

```xml
<module name="diffusion" grid="100">
  <signal name="GFP" diffusion-rate="30um2/s" degradation-rate="0.1/s" color="green" />
  <signal name="RFP" diffusion-rate="5um2/s" color="red" saturation="1uM" />
</module>
```

## Parameters:

| Name         | Type           | Default        | Description |
| ------------ | -------------- | -------------- | ----------- |
| `grid`       | `vector[uint]` | -              | Diffusion grid size. |
| `background` | `color`        | `transparent`  | Background color. |

## Signals

You can specify any number of different signals, there is no limitation. Each signal is specified separately.

### Parameters:

| Name               | Type           | Default        | Description |
| ------------------ | -------------- | -------------- | ----------- |
| `name`             | `string`       | -              | Signal name. |
| `diffusion-rate`   | `unit[m2/s]`   | -              | Diffusion rate. |
| `degradation-rate` | `unit[/s]`     | `0/s`          | Degradation rate. |
| `color`            | `color`        | Predefined     | Signal color when the module is rendered. |
| `saturation`       | `unit[mol/m3]` | `1umol/um3`    | Defines concentration when signal color is 100%. |

## Additional modules

### `Export`

Module store data into CSV file.

### Example:

Store diffusion data for all iterations.

```xml
<module name="diffusion.export" filename="diffusion.csv" active="100-200" signals="S1 S2" />
```

### Parameters:

| Name        | Type           | Default | Description | Example |
| ----------- | -------------- | ------- | ----------- | ------- |
| `filename`  | `string`       | -       | Name of output file. | |
| `active`    | `-`            | -       | List of iteration when is module active. | `1-10 20-50 59 70-73` |
| `signals`   | `list[string]` | -       | List of exported signals. If not present (or empty) all signals are exported. | |
| `obstacles` | `boolean`      | `false` | If column with obstacle value is stored. | |

##### Stored data:

* `iteration` - Iteration number.
* `totalTime` - Simulation time in seconds.
* `x`         - Grid X coordinate.
* `y`         - Grid Y coordinate.
* `name`      - Signal name.
* `concentration` - Signal concentration.
* `obstacle`  - If grid node is an obstacle.

### `Generator`

Diffusion generator which can generate signal withing specified region.

### Example:

Store diffusion data for all iterations.

```xml
<module name="diffusion.generator">
  <source signal="S1" position="30um 0" size="30um 20um" production="30nM/s" active="1-200" />
</module>
```

### Source parameters:

| Name        | Type               | Default | Description |
| ----------- | ------------------ | ------- | ----------- |
| `signal`     | `string`          | -       | Signal name. |
| `position`   | `vector[m]`       | -       | Source position. |
| `size`       | `vector[m]`       | -       | Source size - rectangle. |
| `production` | `unit[mol/um3/s]` | -       | Signal production per second. |
| `active`     | `-`               | -       | List of iteration when is module active. |
