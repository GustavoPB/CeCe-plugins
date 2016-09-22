
# Object generator

Module that generates specified object with some probability and parameters.

## Example:

This example create module that generates different Yeast cells in each iteration with some probability.

```xml
<module name="object-generator">
  <object class="cell.Yeast" rate="0.04/s" programs="make-gfp" distribution="uniform -78um -80um uniform -30um -30um" />
  <object class="cell.Yeast" rate="0.04/s" programs="make-rfp" distribution="uniform -78um -80um uniform 30um 30um" />
  <object class="cell.Yeast" rate="0.04/s">
    <molecule name="GFP" amount="1000" />
  </object>
  <object class="cell.Yeast" rate="0.5/s" active="10-30 60 90-1000" />
</module>
```

### Objects

Object definition is same as for `object` in `simulation` just some additional parameters are required.

##### Parameters:

| Name           | Type               | Description                                   |
| -------------- | ------------------ | --------------------------------------------- |
| `rate`         | `unit[#/s]`        | Rate of object spawning.                      |
| `distribution` | `vector[?]`        | Vector of distributions with 2 parameters. Supported distributions are `uniform` and `normal`. |
| `active`       | `array[range[it]]` | List of ranges when is generator active.      |
| `position`     | `vector[unit[um]]` | Center of spawning rectangle. Cannot be used with `distribution`. |
| `size`         | `vector[unit[um]]` | Size of spawning rectangle. Cannot be used with `distribution`. |
