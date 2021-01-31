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

#include <board/board.h>
#include <src/core/can_bus.h>
#include <wirish_time.h>
#include "bltouch.h"
#include "src/configuration.h"
#include "src/registry/registry.h"
#include "src/device/soft_pwm.h"

void BLTouchProbe::SignalPinInit(uint8_t pin, WiringPinMode mode) {
  this->signal_pin_ = pin;
  pinMode(pin, mode);
  this->signal_pin_time_ = millis();
  this->status_ = 0;
  digitalWrite(pin, 1);
  this->cur_statu = digitalRead(this->signal_pin_);
}

bool BLTouchProbe::CheckStatusLoop() {
  this->cur_statu = !digitalRead(this->signal_pin_);

  if (this->cur_statu != this->last_statu_) {
    this->last_statu_ = this->cur_statu;
    return true;
  }

  return false;

#if 0
  bool ret = false;
  if ((this->signal_pin_time_ + 1) > millis()) {
    return ret;
  }

  // Disappears Shakes
  this->signal_pin_time_ = millis();
  uint8_t cur_statu = digitalRead(this->signal_pin_);
  this->status_ = (this->status_ << 1) | (cur_statu != 0);
  if (((this->status_ & 0x0f) == 0x0f) || ((this->status_ & 0x0f) == 0x0)) {
    this->cur_statu = this->status_ & 0x1;
  }
  if (this->cur_statu != this->last_statu_) {
    ret = true;
    this->last_statu_ = this->cur_statu;
  }
  return ret;
#endif
}

uint8_t BLTouchProbe::Read() {
  return this->cur_statu;
}

void BLTouchProbe::ReportStatus(uint16_t funcid) {
  uint16_t msgid = registryInstance.FuncId2MsgId(funcid);
  if (msgid != INVALID_VALUE) {
    uint8_t statu = this->Read();
    canbus_g.PushSendStandardData(msgid, &statu, 1);
  }
}

void BLTouchProbe::PwmPinInit(uint8_t pin) {
  this->pwm_pin_index_ = soft_pwm_g.AddPwm(pin, PWM_PERIOD);
  soft_pwm_g.ChangeSoftPWM(this->pwm_pin_index_, 0);
}

void BLTouchProbe::Out(uint8_t out) {
  digitalWrite(this->out_pin_, out);
}

// called every 10 microseconds
void BLTouchProbe::OutCtrlLoop() {

}

void BLTouchProbe::SendCmdToBLTouch(BLTOUCH_CMD_ID cmd) {
  switch (cmd) {
    case BLTOUCH_CMD_S10:
      soft_pwm_g.ChangeSoftPWM(this->pwm_pin_index_, 65);
      break;
    case BLTOUCH_CMD_S60:
      soft_pwm_g.ChangeSoftPWM(this->pwm_pin_index_, 116);
      break;
     case BLTOUCH_CMD_S90:
      soft_pwm_g.ChangeSoftPWM(this->pwm_pin_index_, 147);
      break;
    case BLTOUCH_CMD_S120:
      soft_pwm_g.ChangeSoftPWM(this->pwm_pin_index_, 178);
      break;
    case BLTOUCH_CMD_S130:
      soft_pwm_g.ChangeSoftPWM(this->pwm_pin_index_, 188);
      break;
    case BLTOUCH_CMD_S140:
      soft_pwm_g.ChangeSoftPWM(this->pwm_pin_index_, 199);
      break;
    case BLTOUCH_CMD_S150:
      soft_pwm_g.ChangeSoftPWM(this->pwm_pin_index_, 209);
      break;
    case BLTOUCH_CMD_S160:
      soft_pwm_g.ChangeSoftPWM(this->pwm_pin_index_, 219);
      break;
    default:
      soft_pwm_g.ChangeSoftPWM(this->pwm_pin_index_, 147);
      break;
  }
}

void BLTouchProbe::SetBLTouch(uint8_t cmd) {
  switch (cmd) {
    case 10:
      SendCmdToBLTouch(BLTOUCH_CMD_S10);
      break;
    case 60:
      SendCmdToBLTouch(BLTOUCH_CMD_S60);
      break;
    case 90:
      SendCmdToBLTouch(BLTOUCH_CMD_S90);
      break;
    case 120:
      SendCmdToBLTouch(BLTOUCH_CMD_S120);
      break;
    case 130:
      SendCmdToBLTouch(BLTOUCH_CMD_S130);
      break;
    case 140:
      SendCmdToBLTouch(BLTOUCH_CMD_S140);
      break;
    case 150:
      SendCmdToBLTouch(BLTOUCH_CMD_S150);
      break;
    case 160:
      SendCmdToBLTouch(BLTOUCH_CMD_S160);
      break;
    default:
      break;
  }

  uint16_t msgid = registryInstance.FuncId2MsgId(FUNC_SET_BLTOUCH_CMD);
  if (msgid != INVALID_VALUE) {
    uint8_t statu = this->Read();
    canbus_g.PushSendStandardData(msgid, &statu, 1);
  }
}