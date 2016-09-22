
# Cytometry plugin

Plugin' main module measures events (entry and exit) for objects within specified area.

## Example

```xml
<module name="cytometry" position="0 0" size="1um 50um" layer="cytometry" color="red" />
```

## Parameters

| Name       | Type              | Description                |
| ---------- | ----------------- | -------------------------- |
| `position` | `vector[unit[m]]` | Rectangle center position. |
| `size`     | `vector[unit[m]]` | Rectangle area size.       |
| `layer`    | `string`          | Visualization layer name.  |
| `color`    | `color`           | Rectanle area color.       |

## Exported values

* `iteration` - Iteration number.
* `time`      - Total simulation time.
* `object_id` - Object ID which caused event.
* `size`      - Cell radius.
* `gfp`       - Amount of GFP within cell.
* `rfp`       - Amount of RFP within cell.
* `yfp`       - Amount of YFP within cell.
