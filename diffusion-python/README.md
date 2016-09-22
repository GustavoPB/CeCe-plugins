
# Diffusion Python

Python language binding for `diffusion` plugin.

## Example:

This module generates in every iteration some diffusion signal inside defined circle.

```xml
<module name="python:generate"><![CDATA[
import math

def update(dt, simulation):
    diffusion = simulation.useModule("diffusion")
    size = diffusion.gridSize
    radius = size.x / 20
    signalAmount = 1
    for x in range(-radius, radius + 1):
        for y in range(-radius, radius + 1):
            if (math.sqrt(x * x + y * y) <= radius):
                diffusion.setSignal("S1", size.x / 2 + x, size.y / 2 + y, signalAmount)
]]></module>
```

## Class `diffusion.Module`

Diffusion module wrapping class.

### Properties:

| Name          | Type           | Description                             |
| ------------- | -------------- | --------------------------------------- |
| `gridSize`    | `vector[uint]` | Grid size.                              |
| `signalCount` | `uint`         | Number of registered diffusion signals. |

### Methods:

| Name          | Signature                                             | Description                                               |
| ------------- | ----------------------------------------------------- | --------------------------------------------------------- |
| `getSignalId` | `int(name: string)`                                   | Returns signal identifier from signal name.               |
| `getSignal`   | `float(name: string|int, x: int, y: int)`             | Return signal value of given signal at given coordinates. |
| `setSignal`   | `void(name: string|int, x: int, y: int, conc: float)` | Change signal value of given signal at given coordinates. |
