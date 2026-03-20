#include <image_transport/image_transport.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

class RtspPublisher : public rclcpp::Node {
public:
    RtspPublisher() : Node("front_cam_publisher")
    {
        // Low latency RTSP options
        std::string rtsp_url =
            "rtsp://192.168.123.161:8551/front_video"
            "?rtsp_transport=tcp"
            "&fflags=nobuffer"        
            "&flags=low_delay"        
            "&bufsize=400000"         
            "&max_delay=0"            
            "&reorder_queue_size=0";  

        cap_.open(rtsp_url, cv::CAP_FFMPEG);
        if (!cap_.isOpened()) {
            RCLCPP_ERROR(this->get_logger(), "Failed to open RTSP stream");
            rclcpp::shutdown();
            return;
        }

        // Reduce OpenCV internal buffer to minimum (1 frame only)
        cap_.set(cv::CAP_PROP_BUFFERSIZE, 1);

        RCLCPP_INFO(this->get_logger(), "RTSP stream opened successfully");

        // Image transport
        it_ = std::make_shared<image_transport::ImageTransport>(shared_from_this());
        pub_ = it_->advertise("front_camera/image_raw", 10);

        // Start capture loop
        capture_loop();
    }

private:
    void capture_loop()
    {
        while (rclcpp::ok()) {
            cv::Mat frame;

            // Drop old frames (drain decoder buffer)
            // grab() discards stale frames, retrieve() gets the freshest one
            for (int i = 0; i < 3; i++) {
                cap_.grab();
            }

            if (!cap_.retrieve(frame)) {
                RCLCPP_WARN(this->get_logger(), "Frame missing");
                continue;
            }

            auto msg = cv_bridge::CvImage(
                std_msgs::msg::Header(),
                "bgr8",
                frame
            ).toImageMsg();

            msg->header.stamp = this->now();
            pub_.publish(msg);
        }
    }

    cv::VideoCapture cap_;
    std::shared_ptr<image_transport::ImageTransport> it_;
    image_transport::Publisher pub_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<RtspPublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}