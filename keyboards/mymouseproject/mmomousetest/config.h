// Define the SPI pins for the PixArt Sensor
// #define PMW33XX_CS_PIN B6

// Define the rotary encoder pins
// #define ENCODERS_PAD_A { D0 }
// #define ENCODERS_PAD_B { D1 }
// #define ENCODER_RESOLUTION 4 //Adjust based on encoder detents

// Define matrix (for now matrix is straight to pins, will be changed to use MCP23017 expension chip that will communicate through I2C)
#define MATRIX_ROWS 4
#define MATRIX_COLS 3

#define DIODE_DIRECTION ROW2COL
