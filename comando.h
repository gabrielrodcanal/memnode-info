#define NR_OPT 4 /* Number of command options */

#define HELP_MSG "comando -[option]\n-n Nodes info\n-z Zones info\n-p Pages \
info\n\nThis command admits usage of several options at the same time."

extern void show_info(char opt);
extern void read_from_to(char *from, char *to);
