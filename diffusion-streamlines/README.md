
# Diffusion Streamlines Plugin

Extension of diffusion plugin to support streamlines. With this `diffusion` module from this plugin the `streamlines` module affect diffusion signal so it travel according to computed streamlines.

The plugin takes signal from `diffusion` module grid and move it according to velocity field from `streamlines` module. For high velocities the result may contain artefacts because signals is moved over multiple `diffusion` grid cells. To get rid of those artefacts you can increase `inner-iterations` property.

> Replaces `diffusion` plugin and they cannot be used together.

## Properties:

| Name               | Type  | Default | Description                 |
| ------------------ | ----- | ------- | --------------------------- |
| `inner-iterations` | `int` | `1`     | Number of inner iterations. |
