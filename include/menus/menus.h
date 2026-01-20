#ifndef MENUS_H
#define MENUS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_MENU_ITEMS 5
#define MAX_MENU_NAME_LENGTH 20
#define MAX_MENU_MESSAGE_LENGTH 100

typedef struct {
    char *name;
    bool is_default;
} menu_item_t;

typedef struct {
    char name[MAX_MENU_NAME_LENGTH];
    char message[MAX_MENU_MESSAGE_LENGTH];

    // list of string
    menu_item_t options[MAX_MENU_ITEMS];
    unsigned int num_options;

    bool is_valid;
} menu_t;

typedef struct {
    unsigned int idx;
    char *str; 
} menu_return_t;

typedef enum {
    STANDARD,
    TEXT
} menu_type_t;


menu_t create_menu(char *name, char *message);

menu_item_t *add_menu_item(menu_t *menu, char *name, bool is_default);

menu_return_t show_menu(const menu_t *menu, menu_type_t menu_type);


#endif // MENUS_H