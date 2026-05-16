#include <memory>
#include <string>
#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/temperature.hpp"
#include "smart_room_msgs/srv/set_fan_speed.hpp"

using namespace std::chrono_literals;

class FanControllerNode : public rclcpp::Node
{
public:
  FanControllerNode() : Node("fan_controller_node")
  {
    this->declare_parameter("temp_threshold", 28.0);
    this->declare_parameter("fan_speed_high", 0.8);
    this->declare_parameter("fan_speed_low", 0.0);
    this->declare_parameter("room_id", "living_room");

    temp_threshold_ = this->get_parameter("temp_threshold").as_double();
    fan_speed_high_ = this->get_parameter("fan_speed_high").as_double();
    fan_speed_low_ = this->get_parameter("fan_speed_low").as_double();
    room_id_ = this->get_parameter("room_id").as_string();

    temperature_subscriber_ = this->create_subscription<sensor_msgs::msg::Temperature>(
      "/room/temperature",
      10,
      std::bind(&FanControllerNode::temperature_callback, this, std::placeholders::_1)
    );

    fan_client_ = this->create_client<smart_room_msgs::srv::SetFanSpeed>("/fan/set_speed");

    param_callback_handle_ = this->add_on_set_parameters_callback(
      std::bind(&FanControllerNode::parameter_callback, this, std::placeholders::_1)
    );

    fan_is_on_ = false;

    RCLCPP_INFO(this->get_logger(),
      "Fan controller node started. Threshold: %.1f°C, Room: %s",
      temp_threshold_, room_id_.c_str());
  }

private:
  void temperature_callback(const sensor_msgs::msg::Temperature::SharedPtr msg)
  {
    double current_temp = msg->temperature;

    if (current_temp > temp_threshold_ && !fan_is_on_) {
      RCLCPP_INFO(this->get_logger(),
        "Temperature %.2f°C exceeds threshold %.2f°C — turning fan ON",
        current_temp, temp_threshold_);
      call_fan_service(fan_speed_high_, "temperature_high");
      fan_is_on_ = true;

    } else if (current_temp <= temp_threshold_ && fan_is_on_) {
      RCLCPP_INFO(this->get_logger(),
        "Temperature %.2f°C below threshold %.2f°C — turning fan OFF",
        current_temp, temp_threshold_);
      call_fan_service(fan_speed_low_, "temperature_normal");
      fan_is_on_ = false;
    }
  }

  void call_fan_service(float speed, const std::string & reason)
  {
    if (!fan_client_->wait_for_service(1s)) {
      RCLCPP_WARN(this->get_logger(),
        "Fan service not available — is fan_node running?");
      return;
    }

    auto request = std::make_shared<smart_room_msgs::srv::SetFanSpeed::Request>();
    request->speed = speed;
    request->reason = reason;

    auto future = fan_client_->async_send_request(
      request,
      [this](rclcpp::Client<smart_room_msgs::srv::SetFanSpeed>::SharedFuture future) {
        auto response = future.get();
        if (response->success) {
          RCLCPP_INFO(this->get_logger(),
            "Fan service responded: %s (actual speed: %.2f)",
            response->message.c_str(), response->actual_speed);
        } else {
          RCLCPP_ERROR(this->get_logger(),
            "Fan service failed: %s", response->message.c_str());
        }
      }
    );
  }

  rcl_interfaces::msg::SetParametersResult parameter_callback(
    const std::vector<rclcpp::Parameter> & params)
  {
    for (const auto & param : params) {
      if (param.get_name() == "temp_threshold") {
        temp_threshold_ = param.as_double();
        RCLCPP_INFO(this->get_logger(),
          "temp_threshold updated to %.1f°C", temp_threshold_);
      } else if (param.get_name() == "fan_speed_high") {
        fan_speed_high_ = param.as_double();
      } else if (param.get_name() == "fan_speed_low") {
        fan_speed_low_ = param.as_double();
      } else if (param.get_name() == "room_id") {
        room_id_ = param.as_string();
      }
    }
    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;
    return result;
  }

  rclcpp::Subscription<sensor_msgs::msg::Temperature>::SharedPtr temperature_subscriber_;
  rclcpp::Client<smart_room_msgs::srv::SetFanSpeed>::SharedPtr fan_client_;
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_callback_handle_;

  double temp_threshold_;
  double fan_speed_high_;
  double fan_speed_low_;
  std::string room_id_;
  bool fan_is_on_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<FanControllerNode>());
  rclcpp::shutdown();
  return 0;
}
