#include <u.h>
#include <libc.h>
#include <thread.h>
#include "altid.h"

void
closedata(Datactl *dc)
{
	if(dc == nil)
		return;
	close(dc->feed);
	close(dc->ctl);
	dc->feed = dc->ctl = -1;
	threadint(dc->pid);
}

static void
_dataproc(void *arg)
{
	Datactl *dc;
	//int m;
	//char buf[8096];

	dc = arg;
	threadsetname("dataproc");
	
	/* TODO: send() -1 in counter as first token, with empty string data */

	/* Attempt to seek to last section, if too small just seek start */
	if(seek(dc->feed, -8096, 1) < 0)
		seek(dc->feed, 0, 0);

	while(dc->feed >= 0){
		return;
	/* TODO: Finish reading in, parse into Data tokens, and send() them on the channel */
	}
}

Datactl *
initdata(char *mntpt)
{
	Datactl *dc;
	char *t;

	dc = mallocz(sizeof(Datactl), 1);
	if(dc == nil)
		return nil;
	t = malloc(strlen(mntpt)+5);
	sprint(t, "%s/ctrl", mntpt);
	if((dc->ctl = open(t, ORDWR)) < 0){
		free(t);
		free(dc);
		return nil;
	}
	sprint(t, "%s/feed", mntpt);
	if((dc->feed = open(t, OREAD)) < 0){
		free(t);
		free(dc);
		return nil;
	}
	dc->c = chancreate(sizeof(Data*), 20);
	dc->pid = proccreate(_dataproc, dc, 4096);
	return dc;
}

int
tobuffer(Datactl *dc, char *name)
{
	char *s;
	int n;

	s = malloc(strlen(name)+7);
	sprint(s, "buffer %s", name);
	n = write(dc->ctl, s, strlen(s));
	threadint(dc->pid);
	free(s);

	/* Start a brand new proc for the new buffer */
	dc->pid = proccreate(_dataproc, dc, 4096);
	return n;
}

int
ctldata(Datactl *dc, char *m)
{
	return write(dc->ctl, m, strlen(m));
}
