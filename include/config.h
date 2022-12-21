/*
        Pin setup
*/
#define INTERRUPT_PIN 36 // Literally anything on the ESP32
#define LED_PIN       2  // Connected to DevKit LED

/*
        OUTPUTS
*/
// uncomment "OUTPUT_READABLE_QUATERNION" if you want to see the actual
// quaternion components in a [w, x, y, z] format (not best for parsing
// on a remote host such as Processing or something though)
// #define OUTPUT_READABLE_QUATERNION

// uncomment "OUTPUT_READABLE_EULER" if you want to see Euler angles
// (in degrees) calculated from the quaternions coming from the FIFO.
// Note that Euler angles suffer from gimbal lock (for more info, see
// http://en.wikipedia.org/wiki/Gimbal_lock)
// #define OUTPUT_READABLE_EULER

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
// #define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_REALACCEL" if you want to see acceleration
// components with gravity removed. This acceleration reference frame is
// not compensated for orientation, so +X is always +X according to the
// sensor, just without the effects of gravity. If you want acceleration
// compensated for orientation, us OUTPUT_READABLE_WORLDACCEL instead.
#define OUTPUT_READABLE_REALACCEL

// uncomment "OUTPUT_READABLE_WORLDACCEL" if you want to see acceleration
// components with gravity removed and adjusted for the world frame of
// reference (yaw is relative to initial orientation, since no magnetometer
// is present in this case). Could be quite handy in some cases.
// #define OUTPUT_READABLE_WORLDACCEL

// uncomment "OUTPUT_TEMPERATURE" if you want to see the temperature
// measured by the MPU6050's built-in thermometer
// #define OUTPUT_TEMPERATURE

// uncomment "OUTPUT_TEAPOT" if you want output that matches the
// format used for the InvenSense teapot demo
// #define OUTPUT_TEAPOT

/*
        MPU6050 Config
*/
// Uncomment to wait for a serial input before configuring the DMP
// #define DMP_PROGRAMMING_WAIT_FOR_SERIAL

// Uncomment to use the 400kHz I2C clock
// The faster clock is better but can cause compilation difficuties
#define WIRE_USE_400_kHz_CLOCK

/*
        Logging Config
*/
// Uncomment to use the builtin ESP-IDF logger over the Arduino one
// #define USE_ESP_IDF_LOG

// Logging tag, must be defined if using the ESP-IDF logger
#ifdef USE_ESP_IDF_LOG
#define TAG "ARDUINO"
#endif

// Uncomment to use colors in the log
#define CONFIG_ARDUHAL_LOG_COLORS 1
