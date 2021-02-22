#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ROS_INFO_STREAM("Moving robot to position");

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x  = lin_x;
    srv.request.angular_z = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int third = img.width/3;
    int two_third = third*2;
    int pixel_pos = 1;
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    for (uint32_t pix = 0; pix < (img.height*img.step); pix+=3) {
	pixel_pos = pixel_pos%img.width;
	if(img.data[pix] == img.data[pix+1] && img.data[pix+1] == img.data[pix+2] && img.data[pix+2] == white_pixel){
	  if(pixel_pos < third){drive_robot(0.0,0.5);}
	  else if(pixel_pos < two_third){drive_robot(0.5,0.0);}
	  else{drive_robot(0.0,-0.5);}
	  return;
	}
	pixel_pos++;
    }
    ROS_INFO_STREAM("No white pixel detected !");
    drive_robot(0.0,0.0);

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;
    ROS_INFO_STREAM("Process Image initiated");
    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
