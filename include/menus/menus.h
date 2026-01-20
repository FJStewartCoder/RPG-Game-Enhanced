#ifndef MENUS_H
#define MENUS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_MENU_ITEMS 100
#define MAX_MENU_NAME_LENGTH 20
#define MAX_MENU_MESSAGE_LENGTH 100


typedef struct {
    char name[MAX_MENU_NAME_LENGTH];
    char message[MAX_MENU_MESSAGE_LENGTH];

    // list of string
    char *( options[MAX_MENU_ITEMS] );
} menu_t;


typedef enum {
    STANDARD,
    TEXT
} menu_type_t;


int show_menu(const menu_t *menu, menu_type_t menu_type);


#endif // MENUS_H