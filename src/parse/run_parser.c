#include "../gekkota.h"

#include "parse_utils.c"

#include "close_cell_incr.c"
#include "whitespace_incr.c"
#include "key_incr.c"
#include "number_incr.c"
#include "not_incr.c"
#include "open_cell_incr.c"
#include "quote_incr.c"
#include "string_incr.c"
#include "super_incr.c"
#include "symbol_incr.c"

struct parse_ctx *new_parse_ctx(){
    struct parse_ctx *ctx = malloc(sizeof(struct parse_ctx));

    if(ctx == NULL){
        return NULL;
    }
    memset(ctx, 0, sizeof(struct parse_ctx));
    ctx->root = ctx->cell = new_cell(NULL);
    return ctx;
}

static struct match_pattern *setup_pattern(pattern_incr_func func){
    struct match_pattern *pattern = malloc(sizeof(struct match_pattern));
    memset(pattern, 0, sizeof(struct match_pattern));
    pattern->incr = func;
    pattern->token = new_string();
    return pattern;
}

void setup_parse_ctx(struct parse_ctx *ctx){
  int i = 0;
  ctx->patterns[i++] = setup_pattern(string_incr);
  ctx->patterns[i++] = setup_pattern(whitespace_incr);
  ctx->patterns[i++] = setup_pattern(number_incr);
  ctx->patterns[i++] = setup_pattern(key_incr);
  ctx->patterns[i++] = setup_pattern(open_cell_incr);
  ctx->patterns[i++] = setup_pattern(close_cell_incr);
  ctx->patterns[i++] = setup_pattern(symbol_incr);
  /*
  ctx->patterns[i++] = setup_pattern(not_incr);
  ctx->patterns[i++] = setup_pattern(quote_incr);
  ctx->patterns[i++] = setup_pattern(super_incr);
  */
  ctx->patterns[i++] = NULL;
}

static struct stack_item *push_parse_stack(struct stack_item *existing, struct cell *cell, struct head *head){
    struct stack_item *item = new_stack_item(existing, cell, head);
    return item;
}

struct cell *parse_all(char *script){
    struct parse_ctx *ctx = new_parse_ctx();

    if(ctx == NULL){
        char msg[] = "Error allocating parse context, aborting";
        write(STDERR, msg, strlen(msg));
        exit(1);
    }

    setup_parse_ctx(ctx);

    char *p = script;
    while(*p != '\0'){
       parse_char(ctx, *p++);
    }

    return ctx->root; 
}

struct cell *parse_file(int fd){
    char buffer[1];

    struct parse_ctx *ctx = new_parse_ctx();
    if(ctx == NULL){
        char msg[] = "Error allocating parse context, aborting";
        write(STDERR, msg, strlen(msg));
        exit(1);
    }

    setup_parse_ctx(ctx);
    
    while(read(fd, buffer, 1) > 0){
       parse_char(ctx, buffer[0]);
    }
    return ctx->root; 
}

void parse_char(struct parse_ctx *ctx, char c){
    struct match_pattern *pattern = NULL;
    int idx = 0;
    while((pattern = ctx->patterns[idx++])){
        printf("idx:%c:%d\n",c, idx);

        pattern->incr(pattern, ctx, c);

        if(pattern->state != GKA_PARSE_NOT_STARTED){
            /* close the current pattern if exists */
            if(ctx->current && ctx->current != pattern && ctx->current->state == GKA_PARSE_IN_MATCH){
                ctx->current->incr(ctx->current, ctx, '\0');
            }
            if(pattern->state == GKA_PARSE_DONE){
                pattern->state = GKA_PARSE_NOT_STARTED;
            }else{
                ctx->current = pattern;
            }
            break;
        }
    }
}
