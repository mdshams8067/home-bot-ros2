import rclpy
from rclpy.node import Node
from smart_room_msgs.srv import SetFanSpeed

class FanNode(Node):
    def __init__(self):
        super().__init__('fan_node')
        self.current_speed = 0.0

        self.service = self.create_service(
            SetFanSpeed,
            '/fan/set_speed',
            self.handle_set_speed
        )

        self.get_logger().info('Fan node started. Listening on /fan/set_speed')

    def handle_set_speed(self, request, response):
        clamped_speed = max(0.0, min(1.0, request.speed))
        self.current_speed = clamped_speed  
        response.success = True
        response.actual_speed = clamped_speed
        self.get_logger().info(
            f'Fan speed set to {clamped_speed:.2f} - reason: {request.reason}'
        )

        if clamped_speed == 0.0:
            response.message = f'Fan turned OFF (reason: {request.reason})'
        else:
            response.message = f'Fan speed set to {clamped_speed:.2f} (reason: {request.reason})'
        return response

def main(args=None):
    rclpy.init(args=args)
    node = FanNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('Fan node shutting down')
    finally:
        node.destroy_node()
        rclpy.shutdown()