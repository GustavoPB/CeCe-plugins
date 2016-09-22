
# Cell Python Plugin

Adds access to `cell` plugin objects from Python.

## Objects

| Name            | Parent             | Description        |
| --------------- | ------------------ | ------------------ |
| `cell.CellBase` | `simulator.Object` | Base cell object.  |
| `cell.Yeast`    | `cell.CellBase`    | Yeast cell.        |

### Object `cell.CellBase`

Base class for all cell objects.

#### Properties:

| Name           | Type           | Mode | Description            |
| -------------- | -------------- | ---- | ---------------------- |
| `volume`       | `float[um3]`   | RW   | Cell volume.           |
| `volumeMax`    | `float[um3]`   | RW   | Max cell volume.       |
| `growthRate`   | `float[um3/s]` | RW   | Cell growth rate.      |
| `radius`       | `float[um]`    | R    | Cell radius.           |
| `saturationGFP`| `float[#/um3]` | RW   | GFP saturation volume. |
| `saturationRFP`| `float[#/um3]` | RW   | RFP saturation volume. |
| `saturationYFP`| `float[#/um3]` | RW   | YFP saturation volume. |

#### Methods:

| Name            | Return  | Arguments | Description       |
| --------------- | ------- | --------- | ----------------- |
| `moleculeCount` | `float` | `string`  | Return amount of required molecule stored in cell. |
| `kill`          | -       | -         | Kills the cell.   |

### Object `cell.Yeast`

Nothing special.