#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <string>
#include <sstream>
#include <fstream>

// C++11: threads
#include <thread>

#define CYCLIC_BUFFER_SIZE 5000

cv::Mat *dMap;
cv::Mat *rgbImg;
cv::Mat *cameraImg;
int i = 0, j = 0;
bool experiment_end = 0;
std::string path = "/home/michalnowicki/Desktop/MIT/";

// Saving depth data
//		k -> number of frame
void Dsave(int k) {

	std::ostringstream ss;
	ss << "depth_" << std::setw(5) << std::setfill('0') << k << ".png";

	cv::imwrite(path + ss.str(), dMap[k % CYCLIC_BUFFER_SIZE]);
	dMap[k % CYCLIC_BUFFER_SIZE] = cv::Mat();
}

// Saving rgb data
//		k -> number of frame
void RGBsave(int k) {
	std::ostringstream ss;
	ss << "rgb_" << std::setw(5) << std::setfill('0') << k << ".png";

	cv::imwrite(path + ss.str(), rgbImg[k % CYCLIC_BUFFER_SIZE]);
	rgbImg[k % CYCLIC_BUFFER_SIZE] = cv::Mat();
}

// Thread responsible for saving data to files
void savingThread() {
	while (true) {
		if (j >= i) {
			if (experiment_end == 1)
				break;
			usleep(10);
		} else {
			Dsave(j);
			RGBsave(j);

			j++;
		}
		printf("i, j : %d , %d\n", i, j);
	}
}

class KinectDataDumper {
	typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image,
			sensor_msgs::Image> MySyncPolicy;
	message_filters::Subscriber<sensor_msgs::Image> visual_sub_;
	message_filters::Subscriber<sensor_msgs::Image> depth_sub_;
	message_filters::Synchronizer<MySyncPolicy> sync_;

	ros::NodeHandle nh_;


	std::ofstream timestampStream;

public:
	KinectDataDumper(ros::NodeHandle nh) :
			visual_sub_(nh, "/camera/rgb/image_raw", 10000), depth_sub_(nh,
					"/camera/depth/image_raw", 10000), sync_(MySyncPolicy(10000),
					visual_sub_, depth_sub_) {
		std::cout << "KinectDataDumper() - start!" << std::endl;
		nh_ = nh;
		sync_.registerCallback(
				boost::bind(&KinectDataDumper::callback, this, _1, _2));

		std::string fileName = path + "timestamps.txt";
		timestampStream.open(fileName.c_str());

		std::cout << "KinectDataDumper() - end!" << std::endl;

	}

	~KinectDataDumper() {
		timestampStream.close();
	}
	void callback(const sensor_msgs::ImageConstPtr& rgbImage,
			const sensor_msgs::ImageConstPtr& depthImage) // uint16 depth image in mm, the native OpenNI format.
			{

		timestampStream << ros::Time::now() << std::endl;
		timestampStream.flush();

		cv_bridge::CvImagePtr cv_ptr, cv_depth_ptr;
		try {
			rgbImg[i % CYCLIC_BUFFER_SIZE] = cv::Mat();
			dMap[i % CYCLIC_BUFFER_SIZE] = cv::Mat();

			cv_ptr = cv_bridge::toCvCopy(rgbImage,
					sensor_msgs::image_encodings::BGR8);
			cv_depth_ptr = cv_bridge::toCvCopy(depthImage,
					sensor_msgs::image_encodings::TYPE_16UC1);

			rgbImg[i % CYCLIC_BUFFER_SIZE] = cv_ptr->image;
			dMap[i % CYCLIC_BUFFER_SIZE] = cv_depth_ptr->image;
		} catch (cv_bridge::Exception& e) {
			ROS_ERROR("cv_bridge exception: %s", e.what());
			return;
		}

		// Creating new MATs
//		std::ostringstream ss, ss2;
//		ss << "rgb_" << std::setw(5) << std::setfill('0') << iterationCounter << ".png";
//		ss2 << "depth_" << std::setw(5) << std::setfill('0') << iterationCounter << ".png";
//
//
//		cv::imwrite(path + ss.str(), cv_ptr->image);
//		cv::imwrite(path + ss2.str(), cv_depth_ptr->image);
//

		i++;
	}

};

int main(int argc, char** argv) {
	// Creating cyclic buffers
	rgbImg = new cv::Mat [CYCLIC_BUFFER_SIZE];
	dMap = new cv::Mat [CYCLIC_BUFFER_SIZE];
	cameraImg = new cv::Mat [CYCLIC_BUFFER_SIZE];

	// Starting saving thread
	std::thread t(savingThread);


	ros::init(argc, argv, "image_converter");
	ros::NodeHandle nh;
	KinectDataDumper ic(nh);
	ros::spin();
	return 0;
}
