
#include "stack.h"

void Stack::InitStack(stack* s, uint32_t max, Point* data) {
    s->MAX = max;
    s->top = -1;
    s->data = data;
}

uint8_t Stack::FullStack(stack* s) {
    return ((s->top) + 1) == (s->MAX);
}

uint8_t Stack::EmptyStack(stack* s) {
    return (s->top) == -1;
}

uint8_t Stack::PushStack(stack* s, uint8_t x, uint8_t y) {
    if (FullStack(s))
        return 0;
    Point a;
    a.x = x;
    a.y = y;
    s->data[s->top + 1] = a;
    s->top = s->top + 1;
    return 1;
}

Point Stack::PopStack(stack* s) {
    if (EmptyStack(s)) {
        Point a;
        a.x = 0xff;
        a.y = 0xff;
        return a;
    }
    Point ret = s->data[s->top];
    s->top = s->top - 1;
    return ret;
}
