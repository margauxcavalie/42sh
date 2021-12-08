#include <errno.h>
#include <io/cstream.h>
#include <stdio.h>
#include <utils/alloc.h>
#include <utils/attributes.h>

struct cstream_string
{
    struct cstream base;
    char *saved_str;
    char *str;
};

static enum error cstream_string_read(struct cstream *cstream_base, int *c)
{
    struct cstream_string *cstream = (struct cstream_string *)cstream_base;
    if (*cstream->str == '\0')
    {
        *c = EOF;
    }
    else
    {
        *c = *cstream->str;
        cstream->str++;
    }
    return NO_ERROR;
}

enum error cstream_string_free(struct cstream *cstream_base __unused)
{
    struct cstream_string *cstream = (struct cstream_string *)cstream_base;
    free(cstream->saved_str);
    free(cstream_base);
    return NO_ERROR;
}

static const struct cstream_type cstream_string_type = {
    .read = cstream_string_read,
    .free = cstream_string_free,
    .reset = NULL,
    .interactive = false,
};

struct cstream *cstream_string_create(const char *str)
{
    struct cstream_string *cstream = zalloc(sizeof(*cstream));
    cstream->base.type = &cstream_string_type;
    size_t size = strlen(str);
    char *new = zalloc(sizeof(char) * (size + 2));
    strcpy(new, str);
    if (size > 0 && new[size - 1] != '\n')
        new[size] = '\n';
    cstream->str = new;
    cstream->saved_str = new;
    return &cstream->base;
}
