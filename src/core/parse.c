#include "../external.h"
#include "../ssimple.h"
#include "../types/types.h"
#include "core.h"

void parse_char(struct parse_ctx *ctx, char c);

struct parse_ctx *new_parse_ctx(){
    struct parse_ctx *ctx = malloc(sizeof(struct parse_ctx));

    if(ctx == NULL){
        return NULL;
    }
    memset(ctx, 0, sizeof(struct parse_ctx));
    return ctx;
}


struct cell *parse_all(struct string *string){
    struct parse_ctx *ctx = new_parse_ctx();

    if(ctx == NULL){
        char msg[] = "Error allocating parse context, aborting";
        write(STDERR, msg, strlen(msg));
        exit(1);
    }

    int l = string->length;
    for(int i = 0; i<l; i++){
       parse_char(ctx, string->content[i]);
    }

    return ctx->root; 
}

struct cell*parse_file(int fd){
    char buffer[1];

    struct parse_ctx *ctx = new_parse_ctx();
    if(ctx == NULL){
        char msg[] = "Error allocating parse context, aborting";
        write(STDERR, msg, strlen(msg));
        exit(1);
    }
    
    while(read(fd, buffer, 1) > 0){
       parse_char(ctx, buffer[0]);
    }
    return ctx->root; 
}

void assign_cell_attributes(enum SL_PARSE_STATE state, struct cell *cell, struct string *token){
    cell->value = value_from_token(state, token);
}

void finalize_cell(struct parse_ctx *ctx){
    if(ctx->token){
        assign_cell_attributes(ctx->state, ctx->current, ctx->token);

        ctx->token = NULL;
    }

    ctx->state = START;
    if(ctx->state != START){
        ctx->prev_state = ctx->state;
    }
}

struct string *get_or_create_token(struct parse_ctx *ctx){
    if(!ctx->token)
        ctx->token = new_string();
    return ctx->token;
}

void parse_char(struct parse_ctx *ctx, char c){

    printf("%c, ", c);
    fflush(stdout);

    struct cell *slot;
    struct cell *new;
    struct cell *stack_cell;
    struct symbol *symbol;
    
    if(ctx->state == IN_COMMENT){
        if(c == '\n'){
            finalize_cell(ctx);
            ctx->state = START;

            return;
        }else{
            string_append_char(get_or_create_token(ctx), c);
        }

        return;
    }


    if(ctx->state == IN_STRING){
        if(c == '\\' && !ctx->in_escape){
            finalize_cell(ctx);
            ctx->in_escape = 1;
            return;
        }

        if(!ctx->in_escape && c == '"'){
            finalize_cell(ctx);
            ctx->state = START;

            return;
        }else{
            string_append_char(get_or_create_token(ctx), c);
        }

        return;
    }

    if(c == '('){
        finalize_cell(ctx);

        new = new_cell();
        stack_cell = new_cell();
        if(new == NULL || stack_cell == NULL){
            fprintf(stderr, "Error allocating root cell aborting");
            exit(1);
        }

        stack_cell->branch = new;
        slot = ctx->current;

        ctx->current = new;
        ctx->stack = push_stack(ctx->stack, stack_cell, NULL);

        if(!ctx->root){
            ctx->root = stack_cell;
        }else{
            if(slot){
                slot->next = stack_cell;
            }
        }

        ctx->state = IN_CELL;
        return;
    }

    if(c == ')'){
        finalize_cell(ctx);
        if(ctx->stack){
            ctx->current = ctx->stack->cell;
            ctx->stack = ctx->stack->previous;
        }else{
            ctx->current = NULL;
        }
        return;
    }

    if(c == ' ' || c == '\t' || c == '\n'){
        finalize_cell(ctx);
        return;
    }

    if(ctx->state == START){
        new = new_cell();
        if(new == NULL){
            char msg[] = "Error allocating root cell aborting";
            exit(1);
        }

        slot = ctx->current;
        ctx->current = new;
        if(!ctx->root){
            ctx->root = new;
        }else{
            if(slot){
                slot->next = new;
            }
        }
        ctx->state = IN_CELL;
    }

    if(c == '"'){
       ctx->state = IN_STRING; 
       return;
    }

    if(c == '/'){
       if(ctx->has_comment_char){
          ctx->state = IN_COMMENT;
       }
       ctx->has_comment_char = 1; 
       return;
    }

    string_append_char(get_or_create_token(ctx), c);
}