# Mecanum Wheel URDF Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace the placeholder `wheel.urdf.xacro` with two reusable macros — `mecanum_wheel` (single parametric wheel) and `mecanum_wheels_tri` (three-wheel 120° assembly) — and wire them into the existing car model.

**Architecture:** Two-layer macro design following the existing pattern (`yuan.urdf.xacro` primitives → `wheel.urdf.xacro` compound → `car.urdf.xacro` final assembly). The single-wheel macro uses yuan's built-in geometry primitives but applies its own rotation (cylinder tipped on edge via π/2 around X). The assembly macro writes out three hardcoded 120° positions.

**Tech Stack:** ROS 2 Humble, xacro (XML macro processor), URDF

## Global Constraints

- Target file: `src/fang/urdf/wheel/wheel.urdf.xacro` (rewrite)
- Side-effect file: `src/fang/urdf/car.urdf.xacro` (update macro call)
- Include path for `yuan.urdf.xacro` from wheel subdirectory must use `../yuan.urdf.xacro`
- CMakeLists.txt already does `install(DIRECTORY urdf ...)` — no install-rule changes needed
- Verification: `ros2 launch fang display.launch.py` must succeed and show three wheels in RViz

---

### Task 1: Rewrite wheel.urdf.xacro with both macros

**Files:**
- Rewrite: `src/fang/urdf/wheel/wheel.urdf.xacro`

**Interfaces:**
- Consumes: `../yuan.urdf.xacro` (cylinder_visual, cylinder_collision)
- Produces:
  - `mecanum_wheel(name, parent, radius, width, x, y, z, roll, pitch, yaw)` — single wheel link + continuous joint
  - `mecanum_wheels_tri(parent:=base_link, wheel_radius, wheel_width, base_radius)` — three-wheel assembly

- [ ] **Step 1: Write the rewrite**

```xml
<?xml version="1.0"?>
<robot xmlns:xacro="http://ros.org/wiki/xacro">
    <xacro:include filename="../yuan.urdf.xacro"/>

    <!-- Single mecanum wheel: cylinder tipped on edge, continuous joint to parent -->
    <xacro:macro name="mecanum_wheel" params="name parent radius width x y z roll pitch yaw">
        <link name="${name}">
            <visual>
                <origin xyz="0 0 0" rpy="${pi/2} 0 0"/>
                <geometry>
                    <cylinder radius="${radius}" length="${width}"/>
                </geometry>
                <material name="dark_gray">
                    <color rgba="0.3 0.3 0.3 1.0"/>
                </material>
            </visual>
            <collision>
                <origin xyz="0 0 0" rpy="${pi/2} 0 0"/>
                <geometry>
                    <cylinder radius="${radius}" length="${width}"/>
                </geometry>
            </collision>
            <inertial>
                <mass value="0.1"/>
                <!-- Thin disk inertia: I_axial = m*r²/2, I_radial = m*r²/4 -->
                <inertia ixx="${0.1 * radius * radius / 4}" ixy="0" ixz="0"
                         iyy="${0.1 * radius * radius / 2}" iyz="0"
                         izz="${0.1 * radius * radius / 4}"/>
            </inertial>
        </link>

        <joint name="${name}_joint" type="continuous">
            <parent link="${parent}"/>
            <child link="${name}"/>
            <origin xyz="${x} ${y} ${z}" rpy="${roll} ${pitch} ${yaw}"/>
            <axis xyz="0 1 0"/>
        </joint>
    </xacro:macro>

    <!-- Three mecanum wheels at 120° triangular layout -->
    <xacro:macro name="mecanum_wheels_tri" params="parent:=base_link wheel_radius wheel_width base_radius">
        <!-- wheel_0: yaw=0° — front, at (0, base_radius) -->
        <xacro:mecanum_wheel name="wheel_0" parent="${parent}"
            radius="${wheel_radius}" width="${wheel_width}"
            x="0" y="${base_radius}" z="0"
            roll="0" pitch="0" yaw="0"/>

        <!-- wheel_1: yaw=120° — left-rear -->
        <xacro:mecanum_wheel name="wheel_1" parent="${parent}"
            radius="${wheel_radius}" width="${wheel_width}"
            x="${-base_radius * 0.8660254}" y="${-base_radius * 0.5}" z="0"
            roll="0" pitch="0" yaw="${2 * pi / 3}"/>

        <!-- wheel_2: yaw=240° — right-rear -->
        <xacro:mecanum_wheel name="wheel_2" parent="${parent}"
            radius="${wheel_radius}" width="${wheel_width}"
            x="${base_radius * 0.8660254}" y="${-base_radius * 0.5}" z="0"
            roll="0" pitch="0" yaw="${4 * pi / 3}"/>
    </xacro:macro>
</robot>
```

- [ ] **Step 2: Commit**

```bash
git add src/fang/urdf/wheel/wheel.urdf.xacro
git commit -m "feat: add mecanum_wheel and mecanum_wheels_tri macros"
```

---

### Task 2: Update car.urdf.xacro to use new macros

**Files:**
- Modify: `src/fang/urdf/car.urdf.xacro`

**Interfaces:**
- Consumes: `wheel/wheel.urdf.xacro` (mecanum_wheels_tri)
- Produces: (none — terminal assembly)

- [ ] **Step 1: Replace old wheel include and macro call**

Replace the old `<xacro:include filename="wheel/wheel.urdf.xacro"/>` line and `<xacro:wheel .../>` call with the new `mecanum_wheels_tri` call.

The file should read:

```xml
<?xml version="1.0"?>
<robot xmlns:xacro="http://ros.org/wiki/xacro" name="car">
    <xacro:include filename="base.urdf.xacro"/>
    <xacro:include filename="wheel/wheel.urdf.xacro"/>

    <xacro:base_xacro length="0.3" radius="0.3"/>
    <xacro:mecanum_wheels_tri parent="base_link" wheel_radius="0.05" wheel_width="0.02" base_radius="0.2"/>
</robot>
```

- [ ] **Step 2: Verify file reads correctly**

```bash
cat src/fang/urdf/car.urdf.xacro
```

- [ ] **Step 3: Commit**

```bash
git add src/fang/urdf/car.urdf.xacro
git commit -m "feat: wire mecanum_wheels_tri into car model"
```

---

### Task 3: Build and verify

**Files:**
- (none — build and runtime verification)

- [ ] **Step 1: Build the workspace**

```bash
cd /home/destiny/nav && colcon build --packages-select fang
```

Expected: build succeeds with no errors.

- [ ] **Step 2: Source and launch**

```bash
source /home/destiny/nav/install/setup.bash && ros2 launch fang display.launch.py
```

Expected: launch succeeds, no xacro processing errors, RViz opens showing:
- Base cylinder (blue)
- Three dark-gray wheel cylinders at 120° around the base
- joint_state_publisher GUI opens for manual joint control

- [ ] **Step 3: Verify xacro output manually**

```bash
xacro /home/destiny/nav/src/fang/urdf/car.urdf.xacro
```

Expected: valid URDF XML printed to stdout with no errors, containing three wheel links and three continuous joints.

- [ ] **Step 4: Commit (if any fixes were needed)**

Only needed if validation revealed issues.
