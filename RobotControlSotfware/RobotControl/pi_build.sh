g++ -O2 src/robotmanager/sorvocontrol/servos_manager.cpp src/robotmanager/sensormonitor/sensor_manager.cpp src/robotmanager/sensormonitor/Sonar.cpp src/controlmanager/control/LineFollower.cpp src/robotmanager/sorvocontrol/PID.cpp src/controlmanager/networkmanager/NetworkUDP.cpp src/controlmanager/networkmanager/UdpSendJpeg.cpp src/controlmanager/networkmanager/UdpSendMap.cpp src/controlmanager/control/KeyboardSetup.cpp src/robotmanager/sorvocontrol/Servos.cpp src/robotmanager/robotvision/Detector.cpp src/robotmanager/robotvision/Recognizer.cpp src/robotmanager/robotvision/RobotVisionManager.cpp -o LineFollower -I/usr/local/include -Isrc/controlmanager/control/include -Isrc/controlmanager/networkmanager/include -Isrc/robotmanager/robotvision/include -Isrc/robotmanager/sensormonitor/include -Isrc/robotmanager/sensormonitor/Api/core/inc -Isrc/robotmanager/sensormonitor/platform/inc -Isrc/robotmanager/sensormonitor/user_api/inc -Isrc/robotmanager/sorvocontrol/include -I/home/pi/git/robidouille/raspicam_cv -L/home/pi/git/robidouille/raspicam_cv -L/usr/local/lib -lraspicamcv -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_video -lwiringPi -lvl53l0x -std=c++11 -pthread
