// uint8_t servo_state = IDLE;

// uint32_t servo_time = 0;
// uint32_t servo_ptme = 0;

// void servoWork(uint16_t pos)
// {
//   servo_time = micros();
//   switch (servo_state)
//   {
//     case IDLE: {
//       if(servo_time - servo_ptme > 10000)
//       {
//         servo_ptme = micros();
//         servo_state = FRUP;
//         digitalWrite(SERV_PIN, 1);
//       }
//       R1;
//       break;
//     }
//     case FRUP: {
//       if(servo_time - servo_ptme > pos)
//       {
//         servo_ptme = micros();
//         servo_state = FRDN;
//         digitalWrite(SERV_PIN, 0);
//       }
//       G1;
//       break;
//     }
//     case FRDN: {
//       servo_ptme = micros();
//       servo_state = IDLE;
//       B1;
//       break;
//     }
//   }
// }