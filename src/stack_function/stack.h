#pragma once

struct stack
{
    struct vector *vec;
    struct stack *next;
};

struct stack *stack_push(struct stack *s, struct vector *vec);
struct stack *stack_pop(struct stack *s);
struct vector *stack_peek(struct stack *s);
