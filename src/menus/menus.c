#include "menus.h"


// INITIALISATION FUNCTIONS -----------------------------------------------------------------------------------------------


menu_t create_menu(char *name, char *message) {
    // create a new menu
    menu_t menu;

    menu.is_valid = true;

    // copy the strings into the memory
    if ( strlen(name) + 1 <= MAX_MENU_NAME_LENGTH ) {
        strcpy(menu.name, name);
    }
    else {
        menu.is_valid = false;
        return menu;
    }
    
    if ( strlen(message) + 1 <= MAX_MENU_MESSAGE_LENGTH ) {
        strcpy(menu.message, message);
    }
    else {
        menu.is_valid = false;
        return menu;
    }

    // return the menu
    return menu;
}

menu_item_t *add_menu_item(menu_t *menu, char *name, bool is_default) {
    // basic validation
    if ( menu->num_options >= MAX_MENU_ITEMS ) {
        return NULL;
    }

    // create new menu item
    menu_item_t new_item;

    // initialise its values
    new_item.name = name;
    new_item.is_default;

    // add the item and increment the size count
    menu->options[menu->num_options] = new_item;
    menu->num_options = menu->num_options + 1;
    
    // return a pointer to the new item
    return &menu->options[menu->num_options - 1];
}


// HELPER FUNCTIONS -------------------------------------------------------------------------------------------------------



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

    while ( 1 ) {
        printf(">>> ");

        int option;
        int res = scanf("%d", &option);

        const int option_is_valid = option >= 1 && option <= menu_options_length;

        if ( option_is_valid ) {
            return_val.idx = option - 1;
            return_val.str = menu->options[option - 1].name;

            return return_val;
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
    // TODO: fix buffer overflow
    char *buf = malloc( buf_size );
    memset(buf, '\0', buf_size);

    int return_idx = -1;

    while ( 1 ) {
        scanf("%s", buf);

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