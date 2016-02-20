#include "utils.h"
#include "uproxy.h"

OS_INITER;

int main(int argc, char **argv)
{
    char *path = NULL;
    int err = 0;
    	
	err = um_cfg_load();
    if (err<0) {
		goto finish;
	}
    
    err = um_ubus_init(path);
    if (err<0) {
		goto finish;
	} 
    
	uloop_run();
    err = 0;
    
finish:
	um_ubus_fini();
	
	return err;
}
/******************************************************************************/
