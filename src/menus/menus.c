#include "menus.h"


// HELPER FUNCTIONS -------------------------------------------------------------------------------------------------------


bool menu_has_default(const menu_t *menu) {
    return menu->default_ptr != NULL;
}

int get_default_index(const menu_t *menu) {
    // get the index by getting the size difference from the start of the array and divide by item size
    return menu->default_ptr - menu->options;
}

int str_to_lower(char *str) {
    const int a_diff = 'A' - 'a';

    // iterate each character and set to lowercase
    for ( int i = 0; i < strlen(str); i++ ) {
        char cur_char = str[i];

        // if the character can be lowercase in english alphabet
        if ( cur_char >= 'A' && cur_char <= 'Z' ) {
            str[i] = cur_char - a_diff;
        }
    }

    return 0;
}

int str_to_upper(char *str) {
    const int a_diff = 'A' - 'a';

    // iterate each character and set to lowercase
    for ( int i = 0; i < strlen(str); i++ ) {
        char cur_char = str[i];

        // if the character can be lowercase in english alphabet
        if ( cur_char >= 'a' && cur_char <= 'z' ) {
            str[i] = cur_char + a_diff;
        }
    }

    return 0;
}

int read_from_stdin(char *buf, size_t buf_size) {
    int i = 0;
    char c;

    // the max number of characters to read
    const int read_until = buf_size - 1;

    // read until new line ( which is when user pressed enter )
    while ( (c = fgetc(stdin)) != '\n' ) {
        // add characters to buffer until we fill all but one char
        // DON'T BREAK BECAUSE WE NEED TO READ INTO NO WHERE TO REMOVE NEXT INPUT
        if ( i < read_until ) {
            // set buf character and increment i
            buf[i++] = c;
        }
    }

    // set the end of the string with null terminator
    buf[i] = '\0';

    return 0;
}

// INITIALISATION FUNCTIONS -----------------------------------------------------------------------------------------------


menu_t create_menu(char *name, char *message) {
    // create a new menu
    menu_t menu;
    
    // this must be set to 0 otherwise we will have segfault
    menu.num_options = 0;

    // initialise booleans
    menu.is_valid = true;   
    menu.default_ptr = NULL;

    // copy the strings into the memory
    if ( strlen(name) > MAX_MENU_NAME_LENGTH ) {
        menu.is_valid = false;
        return menu;
    }

    // copy the name into the name
    strcpy(menu.name, name);
    
    if ( strlen(message) > MAX_MENU_MESSAGE_LENGTH ) {
        menu.is_valid = false;
        return menu;
    }
    
    // copy the message into the message field
    strcpy(menu.message, message);

    // return the menu
    return menu;
}

menu_item_t *add_menu_item(menu_t *menu, char *name, bool is_default) {
    // printf("Add new item called\n");

    // basic validation
    if ( menu->num_options >= MAX_MENU_ITEMS ) {
        return NULL;
    }

    // create new menu item
    menu_item_t new_item;

    // initialise its values
    new_item.name = name;

    if ( menu_has_default(menu) ) {
        // set default status to false
        is_default = false;
    }

    // set default to is default ( will be false if already has default or whatever the user selects otherwise )
    new_item.is_default = is_default;
    
    // add the item and increment the size count
    menu->options[menu->num_options] = new_item;

    // create a pointer to the current item ( needed for default ptr and return value )
    menu_item_t *option_ptr = &menu->options[menu->num_options];

    // if the current item is default then set the default ptr to this item
    if ( is_default ) {
        menu->default_ptr = option_ptr;
    }

    // increment option count
    menu->num_options = menu->num_options + 1;
    
    // return a pointer to the new item
    return option_ptr;
}


// STANDARD MENU FUNCTIONS ------------------------------------------------------------------------------------------------


menu_return_t standard_menu(const menu_t *menu) {
    menu_return_t return_val;

    // if 0, then they match
    const int name_exists = strlen(menu->name) != 0;
    if ( name_exists ) { printf("-- %s --\n", menu->name); }

    const int menu_options_length = menu->num_options;

    for (int i = 0; i < menu_options_length; i++) {
        const char* current = menu->options[i].name;

        printf("%d - %s\n", i + 1, current);
    }

    const int message_exists = strlen(menu->message) != 0;
    if ( message_exists == 1 ) { printf("%s\n", menu->message); }

    // read buffer
    char buf[10];

    while ( true ) {
        printf(">>> ");

        int option;

        read_from_stdin(buf, sizeof(buf));
        int res = sscanf(buf, "%d", &option);

        // if sscanf is successful, process the data, else try again
        if ( res == 1 ) {
            const int option_is_valid = option >= 1 && option <= menu_options_length;

            if ( option_is_valid ) {
                return_val.idx = option - 1;
                return_val.str = menu->options[option - 1].name;

                return return_val;
            }
        }

        printf("Please try again\n");
    }
}


// TEXT MENU FUNCTIONS ----------------------------------------------------------------------------------------------------


// struct used to be able to reorder a sorted list
typedef struct {
    char* str;
    char* alias;

    unsigned int idx;
} reorder_item_t;

int len_sort(const void *a, const void *b) {
    const reorder_item_t* a2 = a;
    const reorder_item_t* b2 = b;

    return strlen(a2->str) - strlen(b2->str);
}

int idx_sort(const void *a, const void *b) {
    const reorder_item_t* a2 = a;
    const reorder_item_t* b2 = b;

    return a2->idx - b2->idx;
}

typedef struct {
    // long line of memory storing the aliases
    char *alias_string;

    // to show if errors have occured
    int error;
} aliases_t;

aliases_t get_aliases(reorder_item_t option_aliases[MAX_MENU_ITEMS], unsigned int length) {
    aliases_t aliases;
    
    // allocate a size of memory required to store all of the aliases as a single long line in the worst case
    // this is sum of length of each string + total number of aliases ( for \0 ) 
    unsigned int mem_size = 0;

    for ( int i = 0; i < length; i++ ) {
        mem_size += strlen(option_aliases[i].str) + 1;
    }

    // allocate memory
    aliases.alias_string = malloc(mem_size * sizeof(char));

    // if null, return error
    if ( aliases.alias_string == NULL ) {
        aliases.error = 1;
        return aliases;
    }

    // a pointer to the start of the current string
    char *cur_ptr = aliases.alias_string;

    // iterate each option
    for (int i = 0; i < length; i++) {
        // get the current string option
        const char *option = option_aliases[i].str;

        // set the alias to the pointer to the start of the word in our new block of memory 
        option_aliases[i].alias = cur_ptr;
        char *alias = option_aliases[i].alias;

        // check each length of string e.g abcdef -> a, ab, abc, abcd, abcde, abcdef
        // and compare to the already existing aliases
        for (int j = 0; j < strlen(option); j++) {
            alias[j] = option[j];
            alias[j + 1] = '\0';

            int valid = 1;

            // check all previous aliases
            for (int k = 0; k < i; k++) {
                if ( strcmp(alias, option_aliases[k].alias) == 0 ) {
                    valid = 0;
                    break;
                }
            }

            if ( valid ) {
                // set the current pointer to the end of the new string
                cur_ptr += j + 2;  // + 2 is after the '\0'
                break;
            }
        }
    }

    return aliases;
}

int free_aliases(aliases_t *aliases) {
    if ( aliases->error == 1) {
        return 1;
    }

    free(aliases->alias_string);
    return 0;
}

menu_return_t text_menu(const menu_t *menu) {
    menu_return_t res;

    const int message_exists = strlen(menu->message) != 0;
    
    if ( message_exists ) {
        printf("%s: ", menu->message);
    }
    else {
        printf("Select an option: ");
    }

    // array of pointers to items
    reorder_item_t option_alias[MAX_MENU_ITEMS];

    const int menu_options_length = menu->num_options;
    
    // create an array that matches the main options but able to be unsorted after processing
    for (int i = 0; i < menu_options_length; i++) {
        option_alias[i].idx = i;
        option_alias[i].str = menu->options[i].name;
    }

    // sort in ascending order of length
    qsort(option_alias, menu_options_length, sizeof(reorder_item_t), len_sort);

    // create all of the aliases and find the length of the longest string
    aliases_t aliases = get_aliases(option_alias, menu_options_length);
    // required to malloc memory to store the use input in
    const int longest_str = strlen(option_alias[menu_options_length - 1].str);

    qsort(option_alias, menu_options_length, sizeof(reorder_item_t), idx_sort);

    for (int i = 0; i < menu_options_length; i++) {
        // printf("%s %s, %d\n", option_alias[i].str, option_alias[i].alias, option_alias[i].idx);
        // printf("(%s)%s, %d\n", option_alias[i].alias, option_alias[i].str + strlen(option_alias[i].alias), option_alias[i].idx);

        char *template = "(%s)%s, ";

        if ( i == menu_options_length - 1 ) {
            template = "(%s)%s: ";
        }

        printf(template, option_alias[i].alias, option_alias[i].str + strlen(option_alias[i].alias));
    }

    const unsigned int buf_size = sizeof(char) * (longest_str + 2);

    // allocate some memory and set all chars to null terminators
    char *buf = malloc( buf_size );
    memset(buf, '\0', buf_size);

    int return_idx = -1;

    while ( true ) {
        read_from_stdin(buf, buf_size);

        const bool buf_is_empty = strcmp(buf, "") == 0;

        if ( buf_is_empty && menu_has_default(menu) ) {
            return_idx = get_default_index(menu);
        }

        for (int i = 0; i < menu_options_length; i++) {
            if ( strcmp(option_alias[i].alias, buf) == 0 ) {
                return_idx = i;
                break;
            }
        }

        if ( return_idx != -1 ) {
            break;
        }

        printf("Please try again\n");
    }

    free_aliases(&aliases);
    free(buf);

    res.idx = return_idx;
    res.str = menu->options[return_idx].name;

    return res;
}


// SHOW MENU FUNCTIONS ----------------------------------------------------------------------------------------------------


menu_return_t show_menu(const menu_t *menu, menu_type_t menu_type) {
    menu_return_t res = { 0, "" };

    switch ( menu_type ) {
        case STANDARD:
            res = standard_menu(menu);
            break;

        case TEXT:
            res = text_menu(menu);
            break;
    }

    return res;
}