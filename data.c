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
	/* Send back all of our tokens in place on the channel */
	/* Send clearcode on start */
	Datactl *dc;
	//int m;
	//char buf[8096];

	dc = arg;
	threadsetname("dataproc");
	
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
	t = malloc(strlen(mntpt)+20);
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
ctldata(Datactl *dc, char *m)
{
	/* TODO: Parse command, if it's a buffer command we kill dc->pid and restart ioproc */
	//threadint(dc->pid);
	return write(dc->ctl, m, strlen(m));
}
