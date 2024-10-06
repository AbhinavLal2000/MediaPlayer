/*
 * sd.h
 *
 *  Created on: 23-Sep-2024
 *      Author: user
 */

#ifndef SD_H_
#define SD_H_

#define RESET_DUMMY_CYCLES 10
#define START_BITS 0x40
#define STOP_BITS 0x01
#define NUM_ARGS 4
#define NUM_R3_RESP_BYTES 5
#define READ_BYTE_DELAY 8
#define CARD_IDLE 1
#define CMD_OK 0
#define RW_OK 0xFE
#define DATA_ACCEPTED 2
#define INIT_MAX_ATTEMPTS 10000
#define READ_MAX_ATTEMPTS 10000

struct command {
    uint8_t cmd_bits;
    uint8_t args[NUM_ARGS];
    uint8_t crc;
};

const struct command GO_IDLE_STATE = {
    0,
    {0x00, 0x00, 0x00, 0x00},
    0x4A};

const struct command SEND_IF_COND = {
    8,
    {0x00, 0x00, 0x01, 0xAA},
    0x43};

const struct command APP_CMD = {
    55,
    {0x00, 0x00, 0x00, 0x00},
    0xFF};

const struct command SD_SEND_OP_COND = {
    41,
    {0x40, 0x00, 0x00, 0xA0},
    0xFF};

const struct command READ_OCR = {
    58,
    {0x00, 0x00, 0x00, 0x00},
    0xFF};

const struct command READ_SINGLE_BLOCK = {
    17,
    {0x00, 0x00, 0x00, 0x00},  // These will be replaced by addr bytes
    0xFF};

const struct command READ_MULTIPLE_BLOCK = {
    18,
    {0x00, 0x00, 0x00, 0x00},  // These will be replaced by addr bytes
    0xFF};

const struct command STOP_TRANSMISSION = {
    12,
    {0x00, 0x00, 0x00, 0x00},
    0xFF};

const struct command WRITE_BLOCK = {
    24,
    {0x00, 0x00, 0x00, 0x00},  // These will be replaced by addr bytes
    0xFF};

const struct command WRITE_MULTIPLE_BLOCK = {
    25,
    {0x00, 0x00, 0x00, 0x00},  // These will be replaced by addr bytes
    0xFF};

#endif /* SD_H_ */
