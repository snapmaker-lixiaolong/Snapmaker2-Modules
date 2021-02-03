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

#include <src/device/fan.h>
#include <board/board.h>
#include "print_head.h"
#include "src/registry/context.h"
#include "src/core/can_bus.h"
#include <wirish_time.h>
#include "io.h"
//  Periph initialization according schema
void PrintHead::PeriphInit() {

}
void PrintHead::Init() {
  // Fan
  fan_1_.Init(FAN_1_PIN);
  fan_2_.Init(FAN_2_PIN);
  switch_probe_.Init(PA7);
  switch_cut_.Init(PB0);
  bltouch_.PwmPinInit(PA3);
  bltouch_.SignalPinInit(PB6, INPUT_PULLDOWN);
  temperature_.InitCapture(6, PA6, 4);
  temperature_.InitOutCtrl(2, 2, PA1);
  uint32_t moduleType = registryInstance.module();
  if (MODULE_PRINT_V_SM1 == moduleType) {
    pinMode(PA2, OUTPUT);
    digitalWrite(PA2, HIGH);
  }
}

void PrintHead::set_probe_sensor(uint8_t sensor_index) {
  switch (sensor_index) {
    case 0:
      this->bltouch_.SetBLTouch(90);
      this->probe_sensor = PROBE_SENSOR_SWITCH;
      break;
    case 1:
      this->bltouch_.SetBLTouch(10);
      this->probe_sensor = PROBE_SENSOR_BLTOUCH;
      break;
    default:
      this->probe_sensor = PROBE_SENSOR_SWITCH;
      break;
  }

  delay(2000);

  uint16_t msgid = registryInstance.FuncId2MsgId(FUNC_SET_PROBE_SENSOR);
  if (msgid != INVALID_VALUE) {
    uint8_t sensor = (uint8_t)(this->probe_sensor);
    canbus_g.PushSendStandardData(msgid, &sensor, 1);
  }
}

void PrintHead::HandModule(uint16_t func_id, uint8_t * data, uint8_t data_len) {
  float val = 0.0;
  switch ((uint32_t)func_id) {
    case FUNC_REPORT_CUT:
      this->switch_cut_.ReportStatus(FUNC_REPORT_CUT);
      break;
    case FUNC_SET_PROBE_SENSOR:
      this->set_probe_sensor(data[0]);
    break;
    case FUNC_SET_BLTOUCH_CMD:
      //if (this->probe_sensor != PROBE_SENSOR_BLTOUCH) return;
      this->bltouch_.SetBLTouch(data[0]);
    break;
    case FUNC_REPORT_PROBE:
      this->switch_probe_.ReportStatus(FUNC_REPORT_PROBE);
      break;
    case FUNC_SET_FAN:
      fan_1_.ChangePwm(data[1], data[0]);
      break;
    case FUNC_SET_FAN2:
      fan_2_.ChangePwm(data[1], data[0]);
      break;
    case FUNC_SET_TEMPEARTURE:
      temperature_.ChangeTarget((contextInstance.data_[0] << 8) | contextInstance.data_[1]);
      break;
    case FUNC_REPORT_TEMPEARTURE:
      temperature_.ReportTemprature();
      break;
    case FUNC_REPORT_TEMP_PID:
      temperature_.ReportPid();
      break;
    case FUNC_SET_PID:
      val = (float)(((data[1]) << 24) | ((data[2]) << 16) | ((data[3]) << 8 | (data[4]))) / 1000;
      temperature_.SetPID(data[0], val);
      break;
  }
}

void PrintHead::EmergencyStop() {
  temperature_.ChangeTarget(0);
  fan_1_.ChangePwm(0, 0);
  fan_2_.ChangePwm(0, 0);
}

void PrintHead::Loop() {
  this->temperature_.Maintain();
  if ((temp_report_time_ + 500) < millis()) {
    temp_report_time_ = millis();
    temperature_.ReportTemprature();
  }

  if (switch_cut_.CheckStatusLoop()) {
    if (this->is_report_cut_) {
      this->is_report_cut_ = false;
    } else {
      this->is_report_cut_ = true;
    }
    this->cut_report_time_ = millis();
  }

  if (this->is_report_cut_ && ((this->cut_report_time_ + 500) > millis())) {
    this->is_report_cut_ = false;
    switch_cut_.ReportStatus(FUNC_REPORT_CUT);
  }

  if (this->probe_sensor == PROBE_SENSOR_SWITCH) {
    if (switch_probe_.CheckStatusLoop()) {
      switch_probe_.ReportStatus(FUNC_REPORT_PROBE);
    }
  }
  else if (this->probe_sensor == PROBE_SENSOR_BLTOUCH) {
     if (bltouch_.CheckStatusLoop()) {
      bltouch_.ReportStatus(FUNC_REPORT_PROBE);
    }
  }

  this->fan_1_.Loop();
  this->fan_2_.Loop();
}