#include "matrix.h"
#include "i2c_master.h"
#include "wait.h"

// MCP23017 I2C Address (7-bit 0x20 shifted left by 1 for QMK)
#define MCP23017_ADDR (0x20 << 1)

// MCP23017 Register Addresses
#define IODIRA 0x00  // Port A Direction (1 = Input, 0 = Output)
#define IODIRB 0x01  // Port B Direction
#define GPPUA  0x0C  // Port A Pull-up Resistors (1 = Enabled)
#define GPIOA  0x12  // Port A Read Register
#define GPIOB  0x13  // Port B Write Register

#define I2C_TIMEOUT 100

void matrix_init_custom(void) {
    i2c_init();

    // Set Port A (Rows) as inputs (0xFF) and enable internal 100k pull-ups (0xFF)
    uint8_t iodira = 0xFF;
    uint8_t gppua  = 0xFF;
    i2c_write_register(MCP23017_ADDR, IODIRA, &iodira, 1, I2C_TIMEOUT);
    i2c_write_register(MCP23017_ADDR, GPPUA,  &gppua,  1, I2C_TIMEOUT);

    // Set Port B (Columns) as outputs (0x00) and drive them HIGH (0xFF)
    uint8_t iodirb = 0x00;
    uint8_t gpiob  = 0xFF;
    i2c_write_register(MCP23017_ADDR, IODIRB, &iodirb, 1, I2C_TIMEOUT);
    i2c_write_register(MCP23017_ADDR, GPIOB,  &gpiob,  1, I2C_TIMEOUT);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool matrix_has_changed = false;
    matrix_row_t next_matrix[MATRIX_ROWS] = {0};

    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        // Drive the active column LOW while keeping other columns HIGH
        uint8_t col_mask = ~(1 << col);
        i2c_write_register(MCP23017_ADDR, GPIOB, &col_mask, 1, I2C_TIMEOUT);

        // Brief delay to allow signal to settle
        wait_us(10);

        // Read all rows on Port A at once
        uint8_t row_data = 0xFF;
        if (i2c_read_register(MCP23017_ADDR, GPIOA, &row_data, 1, I2C_TIMEOUT) == I2C_STATUS_SUCCESS) {
            // Invert row_data because pressed buttons pull the pin LOW (0)
            row_data = ~row_data;

            for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
                if (row_data & (1 << row)) {
                    next_matrix[row] |= (1 << col);
                }
            }
        }
    }

    // Reset all columns back to HIGH
    uint8_t reset_cols = 0xFF;
    i2c_write_register(MCP23017_ADDR, GPIOB, &reset_cols, 1, I2C_TIMEOUT);

    // Check if the matrix state changed compared to the last scan
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        if (current_matrix[row] != next_matrix[row]) {
            current_matrix[row] = next_matrix[row];
            matrix_has_changed = true;
        }
    }

    return matrix_has_changed;
}
