import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Temperature
from rcl_interfaces.msg import SetParametersResult
import math
import time

class TempSensorNode(Node):
    def __init__(self):
        super().__init__('temp_sensor_node')

        self.declare_parameter('publish_frequency', 1.0)  # Default to 1 Hz
        self.declare_parameter('room_id', 'living_room')  # Default room ID
        self.declare_parameter('base_temperature', 25.0)  # Default base temperature in Celsius

        freq = self.get_parameter('publish_frequency').value
        self.room_id = self.get_parameter('room_id').value
        self.base_temp = self.get_parameter('base_temperature').value

        self.publisher = self.create_publisher(
            Temperature,
            '/room/temperature',
            10
        )

        timer_period = 1.0 / freq
        self.timer = self.create_timer(timer_period, self.publish_temperature)

        self.start_time = time.time()

        self.publish_count = 0
        
        self.add_on_set_parameters_callback(self.parameter_callback)

        self.get_logger().info(
            f'Temperature sensor node started. '
            f'Publishing to /room/temperature at {freq} Hz. '
            f'Room: {self.room_id}'
        )

    def parameter_callback(self, params):
        for param in params:
            if param.name == 'base_temperature':
                self.base_temp = param.value
                self.get_logger().info(
                    f'base_temperature updated to {self.base_temp}°C'
                )
            elif param.name == 'room_id':
                self.room_id = param.value
                self.get_logger().info(
                    f'room_id updated to {self.room_id}'
                )
        return SetParametersResult(successful=True)

    def publish_temperature(self):

        elapsed = time.time() - self.start_time
        slow_variation = 3.0 * math.sin(elapsed / 30.0)
        fast_variation = 0.5 * math.sin(elapsed / 3.0)
        temperature = self.base_temp + slow_variation + fast_variation

        msg = Temperature()

        msg.header.stamp = self.get_clock().now().to_msg()
        msg.header.frame_id = 'temp_sensor_link'

        msg.temperature = temperature

        msg.variance = 0.0

        self.publisher.publish(msg)

        self.publish_count += 1
        if self.publish_count % 5 == 0:
            self.get_logger().info(
                f'[{{self.room_id}}] Temperature: {temperature:.2f} °C '
                f'(published {self.publish_count} times)'
                )
            
def main(args=None):
    rclpy.init(args=args)

    node = TempSensorNode()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('Temperature sensor node shutting down.')
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()