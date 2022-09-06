#include "../external.h"
#include "../ssimple.h"
#include "../types/types.h"
#include "../core/core.h"
#include "../operators/operator.h"
#include "run.h"

static void passthrough(struct head *head, struct head *previous){
    if(head->operator){
        head->operator->handle(head->operator, head, previous->value);
    }else{
        head->value = previous->value;
    }
}

static struct stack_item *push_stack(struct crw_state *ctx){
    ctx->nesting++;
    struct stack_item *item = new_stack_item(ctx->stack, ctx->cell, ctx->head);
    return item;
}

static void pop_stack(struct crw_state *ctx){
    struct head *previous = ctx->head;
    ctx->head = ctx->stack->head;
    if(ctx->stack->cell){
        ctx->cell = ctx->stack->cell->next;
    }else{
        ctx->cell = NULL;
    }
    ctx->stack = ctx->stack->previous;
    passthrough(ctx->head, previous);
    ctx->nesting--;
}


static void next_step(struct crw_state *ctx);

struct crw_state *crw_new_state_context(struct cell* root, struct closure *closure, struct stack_item *stack){
   struct crw_state *state = malloc(sizeof(struct crw_state)); 

   if(state == NULL){
      return NULL;
   }

   memset(state, 0, sizeof(struct crw_state));

   state->closure = closure;
   state->cell = root;
   state->stack = stack;
   state->status = CRW_CONTINUE;
   state->next = next_step;

   return state;
}

static void next_step(struct crw_state *ctx){
    struct value_obj *value = swap_for_symbol(ctx->closure, ctx->cell->value);
    /* if we see keys in the open they can be skipped */
    bool in_key = crw_process_keys(ctx);
    if(in_key){
        ctx->cell = ctx->cell->next;
        return;
    }
    if(ctx->head == NULL){
        ctx->head = setup_new_head(new_head(), ctx->cell, ctx->closure);
    }else if(ctx->cell->branch || (value && value->type == SL_TYPE_CELL)){
        struct cell *branch = ctx->cell->branch;
        if(value && value->type == SL_TYPE_CELL){
           branch = value->slot.cell; 
        }
        ctx->stack = push_stack(ctx);
        ctx->head = setup_new_head(new_head(), branch, ctx->closure);
        ctx->cell = branch;
    }else{
        if(ctx->head->operator){
            if(value && value->type == SL_TYPE_CELL){
                value = value->slot.cell->value;
            }
            enum SL_BRANCH_TYPE branch_type = ctx->head->operator->handle(ctx->head->operator, ctx->head, value);
            /* if the handle has communicated that it no longer wants to 
             * run the rest of the cells, setting cell->next to NULL here
             * will cause the if/else branch following to pull from the
             * previous stack entry
             */
            if(branch_type == SL_BREAK){
                pop_stack(ctx);
                return CRW_CONTINUE;
            }
        }
    }

    ctx->cell = ctx->cell->next;

    if(ctx->cell == NULL){
        while(ctx->cell == NULL && ctx->stack){
            pop_stack(ctx);
        }
    }
    if(!ctx->cell){
        ctx->status = CRW_DONE;
        return;
    }
    ctx->status =  CRW_CONTINUE;
}