struct string *new_string(){
    struct string *string = malloc(sizeof(struct string));

    if(string == NULL){
        return NULL;
    }

    memset(string, 0, sizeof(struct string));

    string->content = malloc(STRING_DEFAULT_SIZE);
    if(string->content == NULL){
        return NULL;
    }
    string->allocated = STRING_DEFAULT_SIZE;
    
    return string;
}

struct string *string_free(struct string *string){
    free(string->content);
    free(string);
}

int _string_resize(struct string *string, size_t length){
    if(string->allocated < length){
        while(string->allocated < length){
            string->allocated = string->allocated * 2;
        }
        string->content = realloc(string->content, string->allocated);
        if(string->content == NULL){
            fprintf(stderr, "String reallocation in resize failed, aborting");
            exit(1);
        }
    }
}

int string_append(struct string *string, struct string *additional){
    _string_resize(string, string->length+additional->length);
    memcpy(string->content+(string->length-1), additional->content, additional->length);
    string->length = string->length+additional->length;
    return string->length; 
}

int string_append_char(struct string *string, char c){
    printf("appending char\n");
    fflush(stdout);
    _string_resize(string, string->length+1);
    string->content[string->length] = c;
    string->length++;
    return 0; 
}

struct string *string_from_cstring(char *cstring){
    size_t length = strlen(cstring);
    
    struct string *string = new_string();

    if(string == NULL){
        char msg[] = "Error allocating string aborting";
        exit(1);
    }

    string->content = malloc(length);
    if(string->content == NULL){
        char msg[] = "Error allocating string aborting";
        exit(1);
    }

    memcpy(string->content, cstring, length);

    string->length = length;

    return string;
}
