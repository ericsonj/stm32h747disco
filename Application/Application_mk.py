from pymakelib.Module import ModuleHandle


def getSrcs(m: ModuleHandle):
    srcs =  [
        'App.c',
#       'AudioPlayer.c',
        'SDFatFs.c',
        'AudioDrive.c',
        'yaffs_nandflash.c',
        'yaffs_osglue.c',
    ]
    return m.getFileByNames(srcs)


def getIncs(m: ModuleHandle):
    return m.getAllIncsC()
