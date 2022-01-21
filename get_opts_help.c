/**
 * @file get_opts_help.c
 * @author Martin Mickels (dv20mms@cs.umu.se)
 * @date 2022-01-08
 * 
 * Help for getting options, interpreting arguments
 */
#include "get_opts_help.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

bool is_number(char *str, int len) {
    for (int i = 0; i < len; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}


bool is_number_above(char *str, int min) {
    if (!is_number(str, strlen(str))) {
        return false;
    }
    if (atoi(str) > min) {
        return true;
    }
    return false;
}


bool is_number_below(char *str, int max) {
    if (!is_number(str, strlen(str))) {
        return false;
    }
    if (atoi(str) < max) {
        return true;
    }
    return false;
}