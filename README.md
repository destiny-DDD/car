# CAR
## 如何使用
### 依赖
#### *serial-ros2*
##### **获取代码：**
`https://github.com/RoverRobotics-forks/serial-ros2.git`
##### **构建：***(cmake3.8及以上)*
`cd serial-ros2`
`cmake -DCMAKE_INSTALL_PREFIX=/usr/local -B build`
`make`
##### **安装:**
`sudo make install`

#### *imu插件*
`sudo apt install ros-jazzy-rviz-imu-plugin`

#### *融合*
`sudo apt install ros-jazzy-robot-localization`

### 编译
#### **获取代码：**
`git clone https://github.com/destiny-DDD/car.git`
#### **第一次构建：**
`cd ./nav`
`./src/livox_ros_driver2/build.sh jazzy`
#### **添加文件：**
`touch ./src/livox_ros_driver2/COLCON_IGNORE`
#### **以后构建均可使用：**
`colcon build --symlink-install`

### 妙妙小工具
#### *usbtop*
##### **install**
`sudo apt install usbtop`
##### **启动**
第一次下载要输入`sudo modprobe usbmon`，启动是`sudo usbtop`，这个小工具可以查看usb口是否有数据传入，是检查上下位机是否有数据的好东西
#### *minicom*
##### **install**
`sudo apt install minicom`
##### **启动**
更改设备`sudo minicom -s`
启动`sudo minicom`