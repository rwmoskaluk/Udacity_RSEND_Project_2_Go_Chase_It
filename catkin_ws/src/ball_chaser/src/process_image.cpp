#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <string>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Driving the robot");

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call service drive_robot");

}

float ball_offset(int location, int direction, int left_bound, int right_bound, int center) {
    //calculates the index offset for driving the robot in the correct direction
    float offset = 0.0;
    switch (direction) {
        case 1: //left
            //offset = (std::abs((center - location)) / center) * 1.0;
            offset = 0.1;
            break;
        case 2: //middle
            offset = 0.0;
            break;
        case 3: //right
            //offset = (std::abs((center - location)) / center) * - 1.0;
            offset = -0.1;
            break;
    }

    //ROS_INFO_STREAM("Debugging information: center = " + std::to_string(center) + " offset = " + std::to_string(offset));
    //ROS_INFO_STREAM("Debugging information: location = " + std::to_string(location) + " left = " + std::to_string(left_bound) + " right = " + std::to_string(right_bound));

    return offset;
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int left = static_cast<int>(img.step / 3.0);
    int right = static_cast<int>(img.step * (2.0/3.0));
    int center = static_cast<int>(img.step * 2.0);
    bool white_found = false;
    float x = 0.5;
    float z = 0.0;
    int index = 0;
    int location;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera


    for (int i = 0; i+2 < img.height * img.step; i+=3) {
        if (img.data[i] == white_pixel  && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel) {
            index = i;
            white_found = true;
            break;
        }
    }
    //test where i is in the data array to determine if left, middle, right of image
    if (white_found) {
        location = index % img.step;
        if (location < left) {
            //move to the left
            ROS_INFO_STREAM("Robot moving to the left!");
            z = ball_offset(location, 1, left, right, center);

        } else if (location > right) {
            //move to the right
            ROS_INFO_STREAM("Robot moving to the right!");
            z = ball_offset(location, 2, left, right, center);
        } else {
            //move in the middle
            ROS_INFO_STREAM("Robot moving down the middle!");
            z = ball_offset(location, 3, left, right, center);
        }
    }
    else {
        //no white detected
        ROS_INFO_STREAM("Robot standing still, no white ball detected!");
        x = 0.0;
    }
    //ROS_INFO_STREAM("Image step = " + std::to_string(img.step));
    ROS_INFO_STREAM("x = " + std::to_string(x) + " z = " + std::to_string(z));
    ROS_INFO_STREAM("Location = " + std::to_string(location));
    drive_robot(x, z);

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}