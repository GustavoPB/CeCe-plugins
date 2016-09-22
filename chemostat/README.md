
# Chemostat plugin

Creates a scene with predefined topology.

## Example

```xml
<plugin name="chemostat" pipe-top="20um" pipe-radius="40um" size="200um 200um" />
```

## Parameters

| Name          | Type              | Description             |
| ------------- | ----------------- | ----------------------- |
| `pipe-top`    | `unit[m]`         | Pipe distance from top. |
| `pipe-radius` | `unit[m]`         | Pipe radius.            |
| `size`        | `vector[unit[m]]` | Chamber size.           |
