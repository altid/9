#include <u.h>
#include <libc.h>
#include <thread.h>
#include <stdio.h>
#include <draw.h>
#include <keyboard.h>
#include <mouse.h>
#include "altid.h"

/* knobs */
enum {
	Off = 3,
};

/* Drawers */
static int minheight;
static Data *data;
static Tabs *tabs;
static Image *cback, *ctext;
static Font *f;

void
redraw(void)
{
	static char s[8096];
	Rectangle r;
	Point p;
	Tabs *tab;
	//Data *chunk;

	r = screen->r;

	p.x = r.min.x + Off;
	p.y = r.min.y + Off;
	draw(screen, r, cback, nil, ZP);
	/* TODO: Buflist shown along the top, clickable */
	/* TODO: Title gets prepended to the data, status inline for feeds? */
	for(tab = tabs; tab != nil; tab = tab->next) {
		// TODO: Move p along x, see if it fits, move along y
		//       if tab->data has a newline, wrap at that char?
		string(screen, p, tab->color, ZP, f, tab->data);
	}
	p.y += f->height;
	string(screen, p, data->color, ZP, f, data->data);
	flushimage(display, 1);
}

void
usage(void)
{
	fprint(2, "usage: %s [-m mtpt] addr\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char *argv[])
{
	/* Network data */
	Datactl *dctl;
	Data *d, *ep;
	Tabsctl *tctl;
	Tabs *t, *tp;

	/* Periphery */
	Keyboardctl *kctl;
	Rune key;
	Mousectl *mctl;
	Mouse m;

	int fd;
	char *mntpt = "/mnt/altid";

	ARGBEGIN{
	case 'm':
		mntpt = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND

	if(argc != 1)
		usage();

	fd = dial(netmkaddr(argv[0], "tcp", nil), 0, 0, 0);
	if(fd < 0)
		sysfatal("dial: %r");

	if(mount(fd, -1, mntpt, MBEFORE, "") < 0)
		sysfatal("mount: %r");
	if(initdraw(nil, nil, "altid") < 0)
		sysfatal("initdraw: %r");
	if((mctl  = initmouse(nil, screen)) == nil)
		sysfatal("initmouse: %r");
	if((kctl = initkeyboard(nil)) == nil)
		sysfatal("initkeyboard: %r");
	if((dctl = initdata(mntpt)) == nil)
		sysfatal("initdata: %r");
	if((tctl = inittabs(mntpt)) == nil)
		sysfatal("inittabs: %r");

	/* TODO: libtheme maybe */
	f = display->defaultfont;
	cback = allocimage(display, Rect(0,0,1,1), RGB24, 1, DWhite);
	ctext = allocimage(display, Rect(0,0,1,1), RGB24, 1, DBlack);

	data = mallocz(sizeof(Data), 1);
	data->data = "Select a buffer to continue";
	data->color = ctext;
	ep = data;
	tabs = mallocz(sizeof(Tabs), 1);
	tabs->data = "";
	tabs->color = ctext;
	tp = tabs;

	enum { ADATA, ATABS, AKBD, AMOUSE, ARESIZE, AEND };
	Alt alts[] = {
		[ADATA]	{ dctl->c,		&d,	CHANRCV },
		[ATABS]    { tctl->c,		&t,	CHANRCV },
		[AKBD]	{ kctl->c,		&key,	CHANRCV },
		[AMOUSE]	{ mctl->c,	&m,	CHANRCV },
		[ARESIZE]	{ mctl->resizec,	nil,	CHANRCV },
		[AEND]	{ nil,		nil,	CHANEND },
	};

	redraw();

	for(;;){
		switch(alt(alts)){
		case ADATA:
			if(d->counter == -1){
				while(data != nil){
					ep = data;
					data = data->next;
					free(ep);
				}
				data = d;
				ep = d;
			} else {
				ep->next = d;
				ep = ep->next;
			}
			redraw();
			break;

		case ATABS:
			if(t->index == -1){
				while(tabs != nil){
					tp = tabs;
					tabs = tabs->next;
					free(tp);
				}
				tabs = t;
				tp = t;
			} else {
				tp->next = t;
				tp = tp->next;
			}
			redraw();
			break;

		case AKBD:
			if(key == Kdel){
				unmount("", mntpt);
				threadexitsall(nil);
			}
			/* TODO: Build up our command/message */
			break;
		case AMOUSE:
			break;
		case ARESIZE:
			if(getwindow(display, Refnone) < 0)
				sysfatal("getwindow: %r");
			break;
		}

		redraw();
	}
}
