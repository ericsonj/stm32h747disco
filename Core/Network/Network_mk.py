from pymakelib.Module import ModuleHandle

def getSrcs(mh: ModuleHandle):
    srcs_names = [
        'app_ethernet.c',
        'ethernetif.c',
        'httpserver_netconn.c'
    ]
    return mh.getFileByNames(srcs_names)


def getIncs(mh: ModuleHandle):
    incs = [
        'Src',
        'Inc'
    ]
    return mh.getSrcsByPath(incs)