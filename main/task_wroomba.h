#ifndef TASK_WROOMBA_H
#define TASK_WROOMBA_H

  #include "wrapper.h"

  #define ROOMBA_COMMAND_START    128
  #define ROOMBA_COMMAND_CLEAN    135
  #define ROOMBA_COMMAND_DOCK     143

  typedef enum WroombaTaskState {
    WROOMBA_TASK_IDLE   = 1,
    WROOMBA_TASK_START  = 2,
    WROOMBA_TASK_STOP   = 3
  } wroomba_task_state_t;

  void vATaskWroomba(void *pvParameters);

#endif
