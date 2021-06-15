#include <hal.h>
#include "util.h"
#include "matrix.h"
#include "i2c_master.h"
#include QMK_KEYBOARD_H


#define I2C_ADDR 0b01000000
#define I2C_IODIRA 0x0
#define I2C_IODIRB 0x1
#define I2C_GPIOA 0x12
#define I2C_GPIOB 0x13
#define I2C_OLATA 0x14
#define I2C_OLATB 0x15
#define I2C_GPPUA 0x0C
#define I2C_GPPUB 0x0D

uint8_t init_mcp23017(void);

static uint8_t mcp23017_reset_loop = 0;

static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

volatile matrix_row_t raw_matrix[MATRIX_ROWS];
volatile uint8_t debounce_matrix[MATRIX_ROWS * MATRIX_COLS];

static matrix_row_t read_cols(uint8_t row);
static void init_cols(void);
static void unselect_rows(void);
static void select_row(uint8_t row);
static void init_rows(void);

__attribute__((weak)) void matrix_init_user(void) {}

__attribute__((weak)) void matrix_scan_user(void) {}

__attribute__((weak)) void matrix_init_kb(void) {
  matrix_init_user();
}

__attribute__((weak)) void matrix_scan_kb(void) {
  matrix_scan_user();
}

volatile int mcp23017_status = 0x20;
uint8_t      i2c_initializied = 0;

uint8_t init_mcp23017(void) {
    if (!i2c_initializied) {
        i2c_init();
        i2c_initializied = 1;
    }

    uint8_t data[2];
    data[0] = 0x0;
    data[1] = 0b00111111;
    mcp23017_status = i2c_writeReg(I2C_ADDR, I2C_IODIRA, data, 2, 50000);
    if (mcp23017_status) goto out;
    data[0] = 0xFFU;
    mcp23017_status = i2c_writeReg(I2C_ADDR, I2C_GPIOA, data, 1, 5000);
    if (mcp23017_status) goto out;
    mcp23017_status = i2c_writeReg(I2C_ADDR, I2C_GPPUB, data+1, 1, 2);
    if (mcp23017_status) goto out;


 out:
    return mcp23017_status;
}


void matrix_init(void) {
  mcp23017_status = init_mcp23017();
  (void) mcp23017_reset_loop;

  // initialize row and col
  init_rows();
  unselect_rows();
  init_cols();
  // initialize matrix state: all keys off
  for (uint8_t i=0; i < MATRIX_ROWS; i++) {
    matrix[i] = 0;
    matrix_debouncing[i] = 0;
    raw_matrix[i] = 0;
    for (uint8_t j = 0; j < MATRIX_COLS; ++j) {
      debounce_matrix[i * MATRIX_COLS + j] = 0;
    }
  }

  matrix_init_quantum();
}

matrix_row_t debounce_mask(matrix_row_t rawcols, uint8_t row) {
  matrix_row_t result = 0;
  matrix_row_t change = rawcols ^raw_matrix[row];
  raw_matrix[row] = rawcols;
  for (uint8_t i = 0; i < MATRIX_COLS; ++i) {
    if (debounce_matrix[row * MATRIX_COLS + i]) {
      --debounce_matrix[row * MATRIX_COLS + i];
    } else {
      result |= (1 << i);
    }
    if (change & (1 << i)) {
      debounce_matrix[row * MATRIX_COLS + i] = DEBOUNCE;
    }
  }
  return result;
}

matrix_row_t debounce_read_cols(uint8_t row) {
  // Read the row without debouncing filtering and store it for later usage.
  matrix_row_t cols = read_cols(row);
  // Get the Debounce mask.
  matrix_row_t mask = debounce_mask(cols, row);
  // debounce the row and return the result.
  return (cols & mask) | (matrix[row] & ~mask);;
}

uint8_t matrix_scan(void) {
  if (mcp23017_status) {
    if (++mcp23017_reset_loop == 0) {
      mcp23017_status = init_mcp23017();
      if (!mcp23017_status) {
	/* blink led */
      }
    }
  }

  for (uint8_t i = 0; i < MATRIX_ROWS_PER_SIDE; i++) {
    select_row(i);
    select_row(i + MATRIX_ROWS_PER_SIDE);

    matrix[i] = debounce_read_cols(i);
    matrix[i + MATRIX_ROWS_PER_SIDE] = debounce_read_cols(i + MATRIX_ROWS_PER_SIDE);

    unselect_rows();
  }
  matrix_scan_quantum();
  return 0;
}

void matrix_print(void) {}

inline matrix_row_t matrix_get_row(uint8_t row) {
  return matrix[row];
}

static void  init_cols(void) {
  palSetGroupMode(GPIOA, 0b111111, 0U, PAL_MODE_INPUT_PULLUP);
}

static void init_rows(void) {
  palSetGroupMode(GPIOB, 0b111111, 8U, PAL_MODE_OUTPUT_PUSHPULL);
}

static matrix_row_t read_cols(uint8_t row){
  if (row < MATRIX_ROWS_PER_SIDE) {
    uint8_t data_p = (GPIOA -> IDR);
    uint8_t data = data_p;
    return ((~data) & 0x3f);
  }

  uint8_t data = 0xFF;
  if (!mcp23017_status) {
    uint8_t regAddr = I2C_GPIOB;
    mcp23017_status = i2c_readReg(I2C_ADDR, regAddr, &data, 1, 10);
  }
  if (mcp23017_status) {
    return 0;
  }
  return (~data) & 0x3F;
}

static void unselect_rows(void){
  /* GPIOB->BSRR = 0b11111111 << 8U; */
  palSetGroupMode(GPIOB, 0b111111, 8U, PAL_MODE_INPUT);
}

static void select_row(uint8_t row) {
  if (row < MATRIX_ROWS_PER_SIDE) {
    palSetGroupMode(GPIOB, 1 << row, 8U, PAL_MODE_OUTPUT_PUSHPULL);
    GPIOB->BRR = (1 << row) << 8U;
  } else {
    if (!mcp23017_status) {
      uint8_t data = (0xFF & ~(1 << (row - MATRIX_COLS)));
      mcp23017_status = i2c_writeReg(I2C_ADDR, I2C_GPIOA, &data, 1, 10);
    }
  }
}
