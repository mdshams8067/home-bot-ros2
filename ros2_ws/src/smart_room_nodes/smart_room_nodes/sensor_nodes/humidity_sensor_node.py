import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32
from rcl_interfaces.msg import SetParametersResult
import math
import time

class HumiditySensorNode(Node):
    def __init__(self):
        super().__init__('humidity_sensor_node')

        self.declare_parameter('publish_frequency', 1.0)  # Default to 1 Hz
        self.declare_parameter('room_id', 'living_room')  # Default room ID
        self.declare_parameter('base_humidity', 60.0)  # Default base humidity in percentage

        freq = self.get_parameter('publish_frequency').value
        self.room_id = self.get_parameter('room_id').value
        self.base_humidity = self.get_parameter('base_humidity').value

        self.publisher = self.create_publisher(
            Float32,
            '/room/humidity',
            10
        )

        timer_period = 1.0 / freq
        self.timer = self.create_timer(timer_period, self.publish_humidity)

        self.start_time = time.time()

        self.publish_count = 0
        
        self.add_on_set_parameters_callback(self.parameter_callback)

        self.get_logger().info(
            f'Humidity sensor node started. '
            f'Publishing to /room/humidity at {freq} Hz. '
            f'Room: {self.room_id}'
        )

    def parameter_callback(self, params):
        for param in params:
            if param.name == 'base_humidity':
                self.base_humidity = param.value
                self.get_logger().info(
                    f'base_humidity updated to {self.base_humidity}%'
                )
            elif param.name == 'room_id':
                self.room_id = param.value
                self.get_logger().info(
                    f'room_id updated to {self.room_id}'
                )
        return SetParametersResult(successful=True)

    def publish_humidity(self):

        elapsed = time.time() - self.start_time
        slow_variation = 3.0 * math.sin(elapsed / 30.0)
        fast_variation = 0.5 * math.sin(elapsed / 3.0)
        humidity = self.base_humidity + slow_variation + fast_variation

        humidity = max(0.0, min(100.0, humidity))

        msg = Float32()
        msg.data = humidity

        self.publisher.publish(msg)

        self.publish_count += 1
        if self.publish_count % 5 == 0:
            self.get_logger().info(
                f'[{self.room_id}] Humidity: {humidity:.2f}% '
                f'(published {self.publish_count} times)'
                )
            
def main(args=None):
    rclpy.init(args=args)

    node = HumiditySensorNode()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('Humidity sensor node shutting down.')
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()