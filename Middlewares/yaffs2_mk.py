from pymakelib.Module import ModuleHandle


def getSrcs(m: ModuleHandle):
    yaffs2Path = '/PROJECTS/C_C++/yaffs2/direct/'
    srcs = [
        'yaffsfs.c',
        'yaffs_allocator.c',
        'yaffs_attribs.c',
        'yaffs_bitmap.c',
        'yaffs_checkptrw.c',
        'yaffs_ecc.c',
        'yaffs_error.c',
        'yaffs_guts.c',
        'yaffs_hweight.c',
        'yaffs_nameval.c',
        'yaffs_nand.c',
        'yaffs_packedtags1.c',
        'yaffs_packedtags2.c',
        'yaffs_summary.c',
        'yaffs_tagscompat.c',
        'yaffs_tagsmarshall.c',
        'yaffs_verify.c',
        'yaffs_yaffs1.c',
        'yaffs_yaffs2.c',
    ]
    return [yaffs2Path + s for s in srcs]


def getIncs(m: ModuleHandle):
    return ['/PROJECTS/C_C++/yaffs2/direct']