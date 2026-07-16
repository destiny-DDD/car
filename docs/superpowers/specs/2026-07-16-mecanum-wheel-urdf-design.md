# Three Mecanum Wheel URDF Design

## Overview

Replace the placeholder `wheel.urdf.xacro` with a complete, reusable three-mecanum-wheel assembly for the `fang` ROS 2 robot description package.

## Architecture

Two-layer macro design following the existing project pattern (`yuan.urdf.xacro` → `base.urdf.xacro` → `car.urdf.xacro`):

```
wheel.urdf.xacro
├── mecancum_wheel (single wheel, reusable)
└── mecancum_wheels_tri (three-wheel assembly, calls above 3×)
```

## `mecanum_wheel` Macro

A parametric single mecanum wheel.

### Parameters

| Param   | Description                          |
|---------|--------------------------------------|
| `name`  | Link name prefix                     |
| `parent`| Parent link to attach to             |
| `radius`| Wheel radius (m)                     |
| `width` | Wheel width/thickness (m)            |
| `x`     | X position relative to parent        |
| `y`     | Y position relative to parent        |
| `z`     | Z position relative to parent        |
| `roll`  | Roll angle (rad) relative to parent  |
| `pitch` | Pitch angle (rad) relative to parent |
| `yaw`   | Yaw angle (rad) relative to parent   |

### Generated elements

- **1 link**: cylinder (visual + collision + inertia), rotated 90° around X-axis so the wheel stands upright (cylinder axis along Y)
- **1 continuous joint**: connects parent → wheel link, axis along the wheel's rotation direction

### Omissions

- No roller visuals — purely cosmetic, add later if needed
- No friction/damping — out of scope for kinematic URDF

## `mecanum_wheels_tri` Macro

Assembles three `mecanum_wheel` instances in a 120° triangular layout.

### Parameters

| Param          | Description                          | Default       |
|----------------|--------------------------------------|---------------|
| `parent`       | Parent link                          | `base_link`   |
| `wheel_radius` | Radius per wheel (m)                 | (required)    |
| `wheel_width`  | Width per wheel (m)                  | (required)    |
| `base_radius`  | Horizontal distance from center to wheel center (m) | (required) |

### Layout (hardcoded positions, yaw = 0°/120°/240°)

| Wheel    | Yaw  | X                          | Y                          |
|----------|------|----------------------------|----------------------------|
| wheel_0  | 0°   | 0                          | `base_radius`              |
| wheel_1  | 120° | `-base_radius * √3 / 2`   | `-base_radius / 2`         |
| wheel_2  | 240° | `base_radius * √3 / 2`    | `-base_radius / 2`         |

## File Changes

| File                                        | Change                              |
|---------------------------------------------|-------------------------------------|
| `src/fang/urdf/wheel/wheel.urdf.xacro`      | Rewrite — two macros                |
| `src/fang/urdf/car.urdf.xacro` (suggested)  | Add include + `mecanum_wheels_tri` call |

## Include Path Fix

`wheel.urdf.xacro` lives in a subdirectory, so `yuan.urdf.xacro` include must use `../yuan.urdf.xacro`.
