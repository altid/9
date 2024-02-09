#include <u.h>
#include <libc.h>
#include <thread.h>
#include "altid.h"

void
closetabs(Tabsctl *tc)
{
	if(tc == nil)
		return;
	close(tc->fd);
	tc->fd = -1;
	threadint(tc->pid);
}

static void
_tabsproc(void *arg)
{	
	Tabsctl *tp;

	tp = arg;
	threadsetname("tabsproc");

	USED(tp);
	// Read in tabs, output each in order 
	// Current is blue
	// Sleep 10 seconds, reread
	// Always send EClear on the start of the loop
}

Tabsctl *
inittabs(char *mntpt)
{
	Tabsctl *tc;
	char *t;

	tc = mallocz(sizeof(Tabsctl), 1);
	if(tc == nil)
		return nil;

	t = malloc(strlen(mntpt) + 20);
	sprint(t, "%s/tabs", mntpt);
	tc->fd = open(t, OREAD);
	free(t);

	tc->c = chancreate(sizeof(Tabs*), 5);
	tc->pid = proccreate(_tabsproc, tc, 4096);
	return tc;
}
