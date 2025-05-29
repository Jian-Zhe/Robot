import rclpy
from rclpy.node import Node

from sensor_msgs.msg import Joy
from geometry_msgs.msg import Twist

class JoyToTurtle(Node):
    def __init__(self):
        super().__init__('joy_to_turtle')

        # 訂閱搖桿訊息
        self.joy_subscriber = self.create_subscription(
            Joy,
            'joy',
            self.joy_callback,
            10)

        # 發布速度指令
        self.cmd_vel_publisher = self.create_publisher(
            Twist,
            '/turtle1/cmd_vel',
            10)

        self.get_logger().info('Joystick to TurtleSim Node Started')

    def joy_callback(self, msg: Joy):
        twist = Twist()

        # 假設 axes[1] 是前後移動，axes[0] 是左右轉動
        linear_axis = msg.axes[1]  # 左搖桿上下
        angular_axis = msg.axes[3]  # 左搖桿左右

        # 可調整比例，讓移動更流暢
        twist.linear.x = linear_axis * 5.0
        twist.angular.z = angular_axis * 2.0

        self.cmd_vel_publisher.publish(twist)

def main(args=None):
    rclpy.init(args=args)
    node = JoyToTurtle()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
