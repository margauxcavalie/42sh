#pragma once

#include <err.h>

#include "ast_node.h"

enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
};

enum parser_status parse();
enum parser_status handle_parse_error(enum parser_status status,
                                      struct ast_node **ast);

