// Single source of truth for every MCP tool this app calls. Names and
// argument shapes must stay in sync with
// main/boards/yana-wheelbot/wheelbot_controller.cc,
// cliff_sensor_controller.cc, dual_led_controller.cc, arm_neck_controller.cc,
// and yana_wheelbot_board.cc (self.screen.set_theme is generic in
// main/mcp_server.cc; self.screen.set_orientation is board-specific).

export const Tools = {
  moveForward: "self.wheelbot.move_forward",
  moveBackward: "self.wheelbot.move_backward",
  turnLeft: "self.wheelbot.turn_left",
  turnRight: "self.wheelbot.turn_right",
  stop: "self.wheelbot.stop",
  setMotorType: "self.wheelbot.set_motor_type",
  setMotorPins: "self.wheelbot.set_motor_pins",
  getMotorConfig: "self.wheelbot.get_motor_config",
  setServoStopPulse: "self.wheelbot.set_servo_stop_pulse",
  setServoReverse: "self.wheelbot.set_servo_reverse",
  getControlStatus: "self.wheelbot.get_control_status",

  cliffSetEnabled: "self.cliff_sensor.set_enabled",
  cliffSetThreshold: "self.cliff_sensor.set_threshold",
  cliffGetConfig: "self.cliff_sensor.get_config",
  cliffTestNow: "self.cliff_sensor.test_now",

  ledSetMode: "self.led.set_mode",
  ledGetMode: "self.led.get_mode",

  armSetAngle: "self.arm.set_angle",
  neckSetAngle: "self.neck.set_angle",
  armWave: "self.arm.wave",
  neckTurn: "self.neck.turn",
  armRelease: "self.arm.release",
  neckRelease: "self.neck.release",

  screenSetTheme: "self.screen.set_theme",
  screenSetOrientation: "self.screen.set_orientation",
} as const;

export type MoveArgs = { duration_ms: number; speed: number };
