# CAR
## 如何使用
### 依赖
#### serial-ros2
1. 获取代码：
`https://github.com/RoverRobotics-forks/serial-ros2.git`
2. 构建(cmake3.8及以上)：
`cd serial-ros2`
`cmake -DCMAKE_INSTALL_PREFIX=/usr/local -B build`
`make`
3. 安装:
`sudo make install`

### 编译
1. 获取代码：
`git clone https://github.com/destiny-DDD/car.git`
2. 第一次构建：
`cd ./nav`
`./src/livox_ros_driver2/build.sh jazzy`
3. 添加文件：
`touch ./src/livox_ros_driver2/COLCON_IGNORE`
4. 以后构建均可使用：
`colcon build --symlink-install`