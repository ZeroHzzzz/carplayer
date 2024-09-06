#ifndef CODE_STACK_H_
#define CODE_STACK_H_

#define Stack_Size 3000

#include "headfile.h"
#include "imagehandler.h"

class StackHandler {
   public:
    void InitStack(stack* s, uint32_t max, Point* data);

    uint8_t FullStack(stack* s);

    uint8_t EmptyStack(stack* s);

    uint8_t PushStack(stack* s, uint8_t x, uint8_t y);

    Point PopStack(stack* s);
};

StackHandler stackhandler;
#endif