
# Fluorescence plugin

Plugin for fluorescence visualization and export. It sums a number of molecules of RFP, GFP or YFP within grid cell.

## Example

```xml
<module name="fluorescence" size="200 200" />
```

## Properties

| Name             | Type          | Default            | Description |
| ---------------- | ------------- | ------------------ | ----------- |
| `size`           | `vector[int]` | -                  | Grid size. |
| `layer`          | `string`      | -                  | Visualization layer name. |
| `layer-RFP`      | `string`      | `layer` value      | Visualization layer name for RFP. |
| `layer-GFP`      | `string`      | `layer` value      | Visualization layer name for GFP. |
| `layer-YFP`      | `string`      | `layer` value      | Visualization layer name for YFP. |
| `saturation`     | `unit[#/m2]`  | `100/um2`          | Saturation value for number of molecules per area. |
| `saturation-RFP` | `unit[#/m2]`  | `saturation` value | Saturation value for number of molecules RFP per area. |
| `saturation-GFP` | `unit[#/m2]`  | `saturation` value | Saturation value for number of molecules GFP per area. |
| `saturation-YFP` | `unit[#/m2]`  | `saturation` value | Saturation value for number of molecules YFP per area. |

## Export module

Exports fluorescence data.

### Properties

| Name       | Type              | Default     | Description |
| ---------- | ----------------- | ----------- | ----------- |
| `position` | `vector[unit[m]]` | `0 0`       | Exported area center. |
| `size`     | `vector[unit[m]]` | whole scene | Exported area size. |
| `total`    | `boolean`         | `false`     | If value for whole area is summed. |
| `layer`    | `string`          | -           | Visualization layer name. |
| `color`    | `color`           | `white`     | Exported area rectangle color. |

### Exported values

* `iteration` - Current iteration number.
* `time` - Total simulation time.

In case of `total` is `true`:

* `protein` - Protein name.
* `amount`  - Number of molecules within export area.
* `cells`   - Number of cells within export area.
* `average` - Average number of proteins within one cell (`amount` / `cells`).

Or in case `false`:

* `x`       - X grid coordinate.
* `y`       - Y grid coordinate.
* `xw`      - X world coordinate [um].
* `yw`      - Y world coordinate [um].
* `protein` - Protein name.
* `amount`  - Number of molecules within grid cell.
* `cells`   - Number of cells within grid cell.
* `average` - Average number of proteins within one cell (`amount` / `cells`).
