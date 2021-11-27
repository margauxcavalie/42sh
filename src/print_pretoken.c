#include <err.h>
#include <prelexer/prelexer.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
        errx(1, "Usage: ./prelexer [SCRIPT]");
    struct pretoken_vector *vec = prelexify(argv[1]);
    print_pretoken_list(vec);
    free_pretoken_list(vec);
    return 0;
}
