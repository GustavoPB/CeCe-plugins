
# Obstacle Plugin

Plugin with static objects - obstacles.

## Objects

| Name                 | Description         |
| -------------------- | ------------------- |
| `obstacle.Circle`    | Circle obstacle.    |
| `obstacle.Rectangle` | Rectangle obstacle. |
| `obstacle.Polygon`   | Polygon obstacle.   |

### Circle object

| Property  | Type      | Description    | Example |
| --------- | --------- | -------------- | ------- |
| `radius`  | `unit[m]` | Circle radius. | `20um`  |

### Rectangle object

| Property | Type              | Description     | Example     |
| -------- | ----------------- | --------------- | ----------- |
| `size`   | `vector[unit[m]]` | Rectangle size. | `20um 50um` |

### Polygon object

| Property   | Type                    | Description                 | Example |
| ---------- | ----------------------- | --------------------------- | ------- |
| `vertices` | `list[vector[unit[m]]]` | List of vertices positions. | `20um 50um 10um 30um 15um 50um` |
