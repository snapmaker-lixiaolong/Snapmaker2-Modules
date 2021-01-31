/*
 * Snapmaker2-Modules Firmware
 * Copyright (C) 2019-2020 Snapmaker [https://github.com/Snapmaker]
 *
 * This file is part of Snapmaker2-Modules
 * (see https://github.com/Snapmaker/Snapmaker2-Modules)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MODULES_WHIMSYCWD_MARLIN_SRC_FEATURE_BLTOUCH_H_
#define MODULES_WHIMSYCWD_MARLIN_SRC_FEATURE_BLTOUCH_H_

#include <stdint.h>
#include "device_base.h"
#include <io.h>

#define PWM_PERIOD  2000

typedef enum {
  BLTOUCH_INIT_STAGE_0,
  BLTOUCH_INIT_STAGE_1,
  BLTOUCH_INIT_STAGE_2,
  BLTOUCH_INIT_STAGE_3,
  BLTOUCH_INIT_STAGE_4,
  BLTOUCH_INIT_STAGE_5,
  BLTOUCH_INIT_STAGE_6,
  BLTOUCH_INIT_STAGE_7,
  BLTOUCH_INIT_STAGE_8,
  BLTOUCH_INIT_STAGE_FINISH
}BLTOUCH_INIT_STAGE;

typedef enum {
  BLTOUCH_CMD_S10  ,
  BLTOUCH_CMD_S60  ,
  BLTOUCH_CMD_S90  ,
  BLTOUCH_CMD_S120 ,
  BLTOUCH_CMD_S130 ,
  BLTOUCH_CMD_S140 ,
  BLTOUCH_CMD_S150 ,
  BLTOUCH_CMD_S160 ,
}BLTOUCH_CMD_ID;

class BLTouchProbe {
 public:
  void SetBLTouch(uint8_t cmd);

  // input
  void SignalPinInit(uint8_t pin, WiringPinMode mode=INPUT_PULLUP);
  uint8_t Read();
  bool CheckStatusLoop();
  void ReportStatus(uint16_t funcid);

  // output
  void PwmPinInit(uint8_t pin);
  void Out(uint8_t out);
  void OutCtrlLoop();

 private:
  void SendCmdToBLTouch(BLTOUCH_CMD_ID cmd);

 private:
  uint8_t signal_pin_;
  uint8_t status_;
  uint8_t cur_statu;
  uint8_t last_statu_;
  uint32_t signal_pin_time_;

  uint8_t out_pin_;
  uint8_t out_val_;
  uint32_t sync_clock_;
  uint32_t out_time_;
  uint32_t period_time_;
  uint32_t duty_cycle_time_;
  uint8_t pwm_pin_index_;

  uint8_t init_stage;
  uint32_t stage_duration_time_;
  uint32_t stage_time_;
  bool bltouch_initialized_;
};

#endif //MODULES_WHIMSYCWD_MARLIN_SRC_FEATURE_SWITCH_H_
