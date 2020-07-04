#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

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

float ball_offset(int remainder, int direction, int left_bound, int right_bound) {
    //calculates the index offset for driving the robot in the correct direction
    float offset = 0.0;
    float center = 0.0;
    switch (direction) {
        case 1: //left
            center = (left_bound / 2.0);
            offset = (1.0 / (static_cast<float>(remainder) / center)) * 0.5;
            //offset = 0.5;
            break;
        case 2: //middle
            offset = 0.0;
            break;
        case 3: //right
            center = (left_bound / 2.0) + right_bound;
            offset = (static_cast<float>(remainder) / center) * 0.5 * -1.0;
            //offset = -0.5;
            break;
    }

    ROS_INFO_STREAM("Debugging information: center = " + std::to_string(center) + " offset = " + std::to_string(offset));
    ROS_INFO_STREAM("Debugging information: remainder = " + std::to_string(remainder) + " left = " + std::to_string(left_bound) + " right = " + std::to_string(right_bound));

    return offset;
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int left = static_cast<int>(img.step / 3.0);
    int right = static_cast<int>(img.step * (2.0/3.0));
    bool white_found = false;
    float x = 0.5;
    float z = 0.0;
    int index = 0;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    for (int i = 0; i < img.height * img.step; i++) {
        if (img.data[i] == white_pixel) {
            index = i;
            white_found = true;
            break;
        }
    }
    //test where i is in the data array to determine if left, middle, right of image
    if (white_found) {
        int remainder = index % img.step;
        if (remainder < left) {
            //move to the left
            ROS_INFO_STREAM("Robot moving to the left!");
            z = ball_offset(remainder, 1, left, right);

        } else if (remainder > right) {
            //move to the right
            ROS_INFO_STREAM("Robot moving to the right!");
            z = ball_offset(remainder, 2, left, right);
        } else {
            //move in the middle
            ROS_INFO_STREAM("Robot moving down the middle!");
            z = ball_offset(remainder, 3, left, right);
        }
    }
    else {
        //no white detected
        ROS_INFO_STREAM("Robot standing still, no white ball detected!");
        x = 0.0;
    }
    ROS_INFO_STREAM("Image step = " + std::to_string(img.step));
    ROS_INFO_STREAM("x = " + std::to_string(x) + " z = " + std::to_string(z));
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