from setuptools import find_packages, setup

package_name = 'joy_to_turtle'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='alpaca',
    maintainer_email='9987jdvrb@gmail.com',
    description='Get joystick input to control turtle',
    license='Apache-2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'joy_to_turtle = joy_to_turtle.joy_to_turtle:main'
        ],
    },
)
