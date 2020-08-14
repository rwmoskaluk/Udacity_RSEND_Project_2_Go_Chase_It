# Udacity_RSEND_Project_2_Go_Chase_It
Udacity Robotics Software Engineer Nanodegree project 2

[image1]: ./pictures/Rviz_Final.png "Rviz Final Setup"
[image2]: ./pictures/Rviz_Frame.png "Rviz Frame Setup"
[image3]: ./pictures/Rviz_Camera.png "Rviz Camera Setup"
[image4]: ./pictures/Rviz_Laser.png "Rviz Laser Setup"
[image5]: ./pictures/Rviz_Model.png "Rviz Model Setup"





The objective of this project was to build a simplified robot with a sensor that could follow a white ball around in the world environment.

<img src="pictures/Robot_Demo.gif?raw=true" width="720px">

### To Run
```
source devel/setup.bash
roslaunch my_robot world.launch
```
Note: this will also launch ball_chaser.launch

Place the white ball model in front of the robot and it will start to move and follow it.

### Rviz setup
![alt text][image1]

```
change the fixed frame to odom in the drop down menu
```
![alt text][image2]

```
add in the Camera and set the topic to /camera/rgb/image_raw
```
![alt text][image3]

```
add in the Laserscan and set the topic to /scan
```
![alt text][image4]

```
Lastly, add in the robot model
```
![alt text][image5]

