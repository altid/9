/* Token type */

typedef struct Datactl Datactl;
typedef struct Tabsctl Tabsctl;
typedef struct Data Data;
typedef struct Tabs Tabs;
typedef struct Channel Channel;

enum {
	ENormalText,
	EColorName,
	EColorText,
	EUrlName,
	EUrlText,
	EImagePath,
	EImageText,
	EImageLink,
	EBoldText,
	EStrikeText,
	EEmText,
	EUnderlineText,
	ErrorText,
};

struct	Datactl
{
	Channel *c;
	int feed;
	int ctl;
	int offset; /* Feed read offset */
	int pid; /* Of slave proc */
};

struct	Data
{
	int counter;
	int type;
	struct Image *color;
	char *data;
	Data *next;
};

struct	Tabsctl
{
	Channel *c;
	Tabs *current;
	int fd;
	int pid; /* slave pid */
};

struct	Tabs
{
	int index;
	struct Image *color;
	char *data;
	Tabs *next;
};

Tabsctl*	inittabs(char *);
Datactl*	initdata(char *);
int	ctldata(Datactl *, char *);
void	closedata(Datactl *);
void	closetabs(Tabsctl *);
