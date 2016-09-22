
# Background Plugin

Plugin that renders defined image in each iteration.

Supported image types are:

 * PNG
 * JPEG

Image file is taken from the same directory where the simulation file is located.

## Example

```xml
<module name="background" image="background.png" />
```

## Parameters

| Name         | Type       | Default | Description |
| ------------ | ---------- | ------- | ----------- |
| `image`      | `resource` | -       | Name of image resource. |
