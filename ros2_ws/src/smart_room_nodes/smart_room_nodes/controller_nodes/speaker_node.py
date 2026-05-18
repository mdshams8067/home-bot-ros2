import rclpy
from rclpy.node import Node
from rclpy.action import ActionServer
from smart_room_msgs.action import Greet
import time

class SpeakerNode(Node):
    def __init__(self):
        super().__init__('speaker_node')
        self._action_server = ActionServer(
            self, 
            Greet, 
            '/speaker/greet',
            self.execute_callback
        )

    def execute_callback(self, goal_handle):
        message = goal_handle.request.message
        language = goal_handle.request.language
        volume = goal_handle.request.volume

        self.get_logger().info(
            f'Received goal: "{message}" language={language} volume={volume:.2f}'
        )

        words = message.split()
        start_time = time.time()

        for i, word in enumerate(words):
            feedback = Greet.Feedback()
            feedback.current_words = word
            feedback.progress = (i + 1) / len(words)
            goal_handle.publish_feedback(feedback)

            self.get_logger().info(
                f'Speaking: "{word}" (progress: {feedback.progress:.0%} done)'
            )
            
            time.sleep(0.3)
        goal_handle.succeed()

        result = Greet.Result()
        result.success = True
        result.message_spoken = message
        result.duration_seconds = float(time.time() - start_time)

        self.get_logger().info(
            f'Finished speaking: "{message}" in {result.duration_seconds:.2f}s'
        )
        return result
    
def main(args=None):
    rclpy.init(args=args)
    node = SpeakerNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('Speaker node shutting down')
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()