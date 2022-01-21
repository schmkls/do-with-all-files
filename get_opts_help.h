/**
 * @file get_opts_help.h
 * @author Martin Mickels (dv20mms@cs.umu.se)
 * @date 2022-01-08
 * 
 * Help for getting options, interpreting arguments
 */
#ifndef GET_OPTS_HELP_H
#define GET_OPTS_HELP_H

#include <stdbool.h>


/**
 * @brief Tells if string is a number
 * 
 * @param str string
 * @param len length of string
 * @return true string is a number, else false
 */
bool is_number(char *str, int len);


/**
 * @brief Tells if string is a number above a minumum number
 * 
 * @param str string
 * @param min minumum number
 * @return true if string is a number above minimum number
 */
bool is_number_above(char *str, int min);


/**
 * @brief Tells if string is a number above below maximum number
 * 
 * @param str string
 * @param min maximum number
 * @return true if string is a number below maximum number
 */
bool is_number_below(char *str, int max);

#endif