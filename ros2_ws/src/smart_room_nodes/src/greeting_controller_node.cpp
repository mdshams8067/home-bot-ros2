#include <memory>
#include <string>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "std_msgs/msg/bool.hpp"
#include "smart_room_msgs/action/greet.hpp"

using namespace std::chrono_literals;
using Greet = smart_room_msgs::action::Greet;

class GreetingControllerNode : public rclcpp::Node
{
public:
  GreetingControllerNode() : Node("greeting_controller_node")
  {
    // Parameters — all changeable at runtime with ros2 param set
    this->declare_parameter("greeting_message", "Hello welcome home");
    this->declare_parameter("language", "en");
    this->declare_parameter("volume", 0.8);
    this->declare_parameter("greeting_cooldown_sec", 10.0);

    greeting_message_ = this->get_parameter("greeting_message").as_string();
    language_ = this->get_parameter("language").as_string();
    volume_ = this->get_parameter("volume").as_double();
    cooldown_sec_ = this->get_parameter("greeting_cooldown_sec").as_double();

    // Track previous person_detected state for edge detection
    // Start as true so we don't greet immediately on startup
    prev_person_detected_ = true;

    // Track when we last greeted — start at zero
    last_greeting_time_ = this->now() - rclcpp::Duration::from_seconds(cooldown_sec_);

    // Track if speaker is currently busy
    speaker_busy_ = false;

    // Subscribe to person detection topic
    person_subscriber_ = this->create_subscription<std_msgs::msg::Bool>(
      "/room/person_detected",
      10,
      std::bind(&GreetingControllerNode::person_callback, this, std::placeholders::_1)
    );

    // Create action client — connects to speaker action server
    greeting_client_ = rclcpp_action::create_client<Greet>(
      this,
      "/speaker/greet"
    );

    // Parameter callback
    param_callback_handle_ = this->add_on_set_parameters_callback(
      std::bind(&GreetingControllerNode::parameter_callback, this, std::placeholders::_1)
    );

    RCLCPP_INFO(this->get_logger(),
      "Greeting controller started. Message: '%s', Cooldown: %.1fs",
      greeting_message_.c_str(), cooldown_sec_);
  }

private:
  void person_callback(const std_msgs::msg::Bool::SharedPtr msg)
  {
    bool person_detected = msg->data;

    // Edge detection — only react when state changes false→true
    if (person_detected && !prev_person_detected_) {
      RCLCPP_INFO(this->get_logger(), "Person entered the room");

      // Check cooldown — how long since last greeting?
      double seconds_since_last = (this->now() - last_greeting_time_).seconds();

      if (seconds_since_last < cooldown_sec_) {
        RCLCPP_INFO(this->get_logger(),
          "Cooldown active — %.1fs remaining before next greeting",
          cooldown_sec_ - seconds_since_last);

      } else if (speaker_busy_) {
        RCLCPP_INFO(this->get_logger(),
          "Speaker is busy — skipping greeting");

      } else {
        send_greeting();
      }
    }

    // Update previous state for next callback
    prev_person_detected_ = person_detected;
  }

  void send_greeting()
  {
    // Check action server is available
    if (!greeting_client_->wait_for_action_server(1s)) {
      RCLCPP_WARN(this->get_logger(),
        "Speaker action server not available — is speaker_node running?");
      return;
    }

    // Build the goal
    auto goal = Greet::Goal();
    goal.message = greeting_message_;
    goal.language = language_;
    goal.volume = static_cast<float>(volume_);

    // Set up callbacks for feedback and result
    auto send_goal_options = rclcpp_action::Client<Greet>::SendGoalOptions();

    // Called when feedback arrives — while speaker is talking
    send_goal_options.feedback_callback =
      [this](rclcpp_action::ClientGoalHandle<Greet>::SharedPtr,
             const std::shared_ptr<const Greet::Feedback> feedback) {
        RCLCPP_INFO(this->get_logger(),
          "Speaking: '%s' (%.0f%% done)",
          feedback->current_words.c_str(),
          feedback->progress * 100.0);
      };

    // Called when result arrives — speaker is done
    send_goal_options.result_callback =
      [this](const rclcpp_action::ClientGoalHandle<Greet>::WrappedResult & result) {
        speaker_busy_ = false;
        if (result.result->success) {
          RCLCPP_INFO(this->get_logger(),
            "Greeting complete: '%s' in %.2fs",
            result.result->message_spoken.c_str(),
            result.result->duration_seconds);
        } else {
          RCLCPP_ERROR(this->get_logger(), "Greeting failed");
        }
      };

    // Send the goal — non-blocking, callbacks handle the rest
    speaker_busy_ = true;
    last_greeting_time_ = this->now();
    greeting_client_->async_send_goal(goal, send_goal_options);

    RCLCPP_INFO(this->get_logger(),
      "Sent greeting goal: '%s'", greeting_message_.c_str());
  }

  rcl_interfaces::msg::SetParametersResult parameter_callback(
    const std::vector<rclcpp::Parameter> & params)
  {
    for (const auto & param : params) {
      if (param.get_name() == "greeting_message") {
        greeting_message_ = param.as_string();
        RCLCPP_INFO(this->get_logger(),
          "greeting_message updated to: '%s'", greeting_message_.c_str());
      } else if (param.get_name() == "language") {
        language_ = param.as_string();
      } else if (param.get_name() == "volume") {
        volume_ = param.as_double();
      } else if (param.get_name() == "greeting_cooldown_sec") {
        cooldown_sec_ = param.as_double();
        RCLCPP_INFO(this->get_logger(),
          "cooldown updated to %.1fs", cooldown_sec_);
      }
    }
    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;
    return result;
  }

  // Subscriber and action client
  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr person_subscriber_;
  rclcpp_action::Client<Greet>::SharedPtr greeting_client_;
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_callback_handle_;

  // Parameters
  std::string greeting_message_;
  std::string language_;
  double volume_;
  double cooldown_sec_;

  // State tracking
  bool prev_person_detected_;
  bool speaker_busy_;
  rclcpp::Time last_greeting_time_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<GreetingControllerNode>());
  rclcpp::shutdown();
  return 0;
}
