#!../../js

var a;

function init() {
    a = { foo: 123 };

    Duktape.fin(a, function (x, heapDestruct) {
	if (heapDestruct) {
		print('hd:finalizer, foo ->', x.foo);
		return;
        }
        try {
            print('normal:finalizer, foo ->', x.foo);
        } catch (e) {
            print('bad:finalizer failed (ignoring): ' + e);
        }
    });
}

init();
print('refcount finalizer');
//a = null;
print('mark-and-sweep finalizer')
Duktape.gc();

