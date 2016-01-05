# PUTSLAM_ROSBAG
Tool used to convert rosbags to PUTSLAM format

Making tool:
``catkin_make --pkg rosbag2file``

Running tool:
``rosrun rosbag2file rosbag2file``

Using time in rosbag:
``rosparam set /use_sim_time true``

PLaying rosbag:
``rosplay 2012-04-06-11-15-29.bag --clock -r X``


