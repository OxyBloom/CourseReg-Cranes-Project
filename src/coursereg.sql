PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
ANALYZE sqlite_schema;
CREATE TABLE staff (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    name TEXT,
    position TEXT
);
INSERT INTO staff VALUES(1,'admin','admin123','David','Admin');
CREATE TABLE courses (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    credit INTEGER NOT NULL,
    year INTEGER NOT NULL
);
INSERT INTO courses VALUES(23,'Mathematics-1',4,1);
INSERT INTO courses VALUES(24,'Physics',3,1);
INSERT INTO courses VALUES(25,'Introduction to Programming',3,1);
INSERT INTO courses VALUES(26,'Electronics Fundamentals',3,1);
INSERT INTO courses VALUES(27,'Mathematics-1',4,1);
INSERT INTO courses VALUES(28,'Physics',3,1);
INSERT INTO courses VALUES(29,'Introduction to Robotics',3,1);
INSERT INTO courses VALUES(30,'Data Structures (CSE)',3,2);
INSERT INTO courses VALUES(31,'Computer Networks',3,2);
INSERT INTO courses VALUES(32,'Digital Electronics (ECE)',3,2);
INSERT INTO courses VALUES(33,'Analog Circuits (ECE)',3,2);
INSERT INTO courses VALUES(34,'Kinematics and Dynamics (Robotics)',3,2);
INSERT INTO courses VALUES(35,'Control Systems (Robotics)',3,2);
INSERT INTO courses VALUES(36,'Operating Systems (CSE)',3,3);
INSERT INTO courses VALUES(37,'Database Systems (CSE)',3,3);
INSERT INTO courses VALUES(38,'Microprocessors (ECE)',3,3);
INSERT INTO courses VALUES(39,'Signal Processing (ECE)',3,3);
INSERT INTO courses VALUES(40,'Computer Vision (Robotics)',3,3);
INSERT INTO courses VALUES(41,'Robotic Operating System (Robotics)',2,4);
INSERT INTO courses VALUES(42,'Machine Learning (Robotics)',3,3);
INSERT INTO courses VALUES(43,'Artificial Intelligence',3,4);
INSERT INTO courses VALUES(44,'Cloud Computing (CSE)',3,4);
INSERT INTO courses VALUES(45,'VLSI Design (ECE)',3,4);
INSERT INTO courses VALUES(46,'Communication Systems (ECE)',3,4);
INSERT INTO courses VALUES(47,'Advanced Robotics (Robotics)',3,4);
INSERT INTO courses VALUES(48,'Human-Robot Interaction (Robotics)',3,4);
CREATE TABLE enrollments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    student_id INTEGER,
    course_id INTEGER,
    enrollment_date TEXT,
    FOREIGN KEY (student_id) REFERENCES students(id),
    FOREIGN KEY (course_id) REFERENCES courses(id)
);
INSERT INTO enrollments VALUES(16,1,40,'2025-03-21');
INSERT INTO enrollments VALUES(17,1,42,'2025-03-21');
INSERT INTO enrollments VALUES(18,1,38,'2025-03-21');
CREATE TABLE grades (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    student_id INTEGER,
    course_id INTEGER,
    grade TEXT,
    FOREIGN KEY (student_id) REFERENCES students(id),
    FOREIGN KEY (course_id) REFERENCES courses(id)
);
CREATE TABLE notifications (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER,
    message TEXT,
    date TEXT
);
CREATE TABLE logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER,
    action TEXT,
    timestamp TEXT
);
CREATE TABLE students (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    name TEXT, reg_number TEXT UNIQUE NOT NULL,
    year INTEGER,
    department TEXT
);
INSERT INTO students VALUES(1,'student1','pass123','Patrick Wah Sekey','2201020626',1,'CSE-Systems');
INSERT INTO students VALUES(2,'student2','pass456','Abinash Mondal','22010201006',2,'CSE');
INSERT INTO students VALUES(3,'student3','pass789','David Esuga-Mopah','2201020867',3,'RAI');
CREATE TABLE course_stats (
    name TEXT,
    year INTEGER,
    enrollment_count INTEGER,
    UNIQUE (name, year)
);
INSERT INTO course_stats VALUES('Computer Vision (Robotics)',3,1);
INSERT INTO course_stats VALUES('Machine Learning (Robotics)',3,1);
INSERT INTO course_stats VALUES('Microprocessors (ECE)',3,1);
DELETE FROM sqlite_sequence;
INSERT INTO sqlite_sequence VALUES('staff',1);
INSERT INTO sqlite_sequence VALUES('students',3);
INSERT INTO sqlite_sequence VALUES('courses',48);
INSERT INTO sqlite_sequence VALUES('enrollments',18);
COMMIT;
