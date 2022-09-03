enum CRW_STATUS {
    CRW_CONTINUE = 0,
    CRW_DONE
};

struct crw_state {
    enum CRW_STATUS status;
    struct head *head;
    struct cell *cell;
    struct stack_item *stack;
    struct closure *closure;
    bool (*next)(struct crw_state *ctx);
};

struct crw_state *crw_new_state_context(struct cell* root, struct closure *closure, struct stack_item *stack);
