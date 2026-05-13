from setuptools import find_packages, setup

package_name = 'smart_room_nodes'

setup(
    name=package_name,
    version='0.0.1',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Shams',
    maintainer_email='mdshams013@gmail.com',
    description='Smart Room sensor and controller nodes',
    license='Apache-2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'temp_sensor = smart_room_nodes.sensor_nodes.temp_sensor_node:main',
            'humidity_sensor = smart_room_nodes.sensor_nodes.humidity_sensor_node:main',
        ],
    },
)
