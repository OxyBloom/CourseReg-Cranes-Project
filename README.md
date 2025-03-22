# CourseReg-Cranes-Project

A C based course registration and management system that uses basic GUI and integrated with sql database
# System Setup
OS: Ubuntu 22.04

(Feel free to make a windows compactible version if I haven't by the time you find this repository)

# Dependencies
Sqlite3 - Database

GTK - For Gui Design

```
Install dependencies with sudo apt
```
sudo apt install sqlite3
sudo apt install build-essential libgtk-3-dev libsqlite3-dev 
```
#SETUP
Inside the project directory do
```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
# Usage
### To use
Inside the project directory do
```
cd build
./course_reg
```

You are always Welcome to contribute or reach out incase you face issues
