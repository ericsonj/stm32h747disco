from pymakelib.Module import ModuleHandle, StaticLibrary
import hashlib

def init(mh: ModuleHandle):
    staticLib = StaticLibrary(name='middlewares', outputDir='Release/STM32F7xx')
    if mh.getGoal() == 'all':
        try:
            linked = hashlib.md5(open('Port/STM32H7xx/linker/STM32H747XIHx_FLASH_CM7_ETH.ld', 'rb').read()).hexdigest()
            staticLib.rebuildByCheckStr(str(mh.getGeneralCompilerOpts().opts) + str(linked))
        except Exception as e:
            print(e)
    return staticLib


def getSrcs(m: ModuleHandle):
    srcs = [
        # LwIP
        'Middlewares/Third_Party/LwIP/src/apps/httpd/fs.c',
        'Middlewares/Third_Party/LwIP/src/apps/httpd/httpd.c',
        'Middlewares/Third_Party/LwIP/src/netif/ethernet.c',
        'Middlewares/Third_Party/LwIP/system/OS/sys_arch.c',
        'Middlewares/Third_Party/LwIP/src/core/ipv4/autoip.c',
        'Middlewares/Third_Party/LwIP/src/core/ipv4/dhcp.c',
        'Middlewares/Third_Party/LwIP/src/core/ipv4/etharp.c',
        'Middlewares/Third_Party/LwIP/src/core/ipv4/icmp.c',
        'Middlewares/Third_Party/LwIP/src/core/ipv4/igmp.c',
        'Middlewares/Third_Party/LwIP/src/core/ipv4/ip4.c',
        'Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_addr.c',
        'Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_frag.c',
        'Middlewares/Third_Party/LwIP/src/core/def.c',
        'Middlewares/Third_Party/LwIP/src/core/dns.c',
        'Middlewares/Third_Party/LwIP/src/core/inet_chksum.c',
        'Middlewares/Third_Party/LwIP/src/core/init.c',
        'Middlewares/Third_Party/LwIP/src/core/ip.c',
        'Middlewares/Third_Party/LwIP/src/core/mem.c',
        'Middlewares/Third_Party/LwIP/src/core/memp.c',
        'Middlewares/Third_Party/LwIP/src/core/netif.c',
        'Middlewares/Third_Party/LwIP/src/core/pbuf.c',
        'Middlewares/Third_Party/LwIP/src/core/raw.c',
        'Middlewares/Third_Party/LwIP/src/core/stats.c',
        'Middlewares/Third_Party/LwIP/src/core/sys.c',
        'Middlewares/Third_Party/LwIP/src/core/tcp.c',
        'Middlewares/Third_Party/LwIP/src/core/tcp_in.c',
        'Middlewares/Third_Party/LwIP/src/core/tcp_out.c',
        'Middlewares/Third_Party/LwIP/src/core/timeouts.c',
        'Middlewares/Third_Party/LwIP/src/core/udp.c',
        'Middlewares/Third_Party/LwIP/src/api/api_lib.c',
        'Middlewares/Third_Party/LwIP/src/api/api_msg.c',
        'Middlewares/Third_Party/LwIP/src/api/err.c',
        'Middlewares/Third_Party/LwIP/src/api/netbuf.c',
        'Middlewares/Third_Party/LwIP/src/api/netdb.c',
        'Middlewares/Third_Party/LwIP/src/api/netifapi.c',
        'Middlewares/Third_Party/LwIP/src/api/sockets.c',
        'Middlewares/Third_Party/LwIP/src/api/tcpip.c',
        # FreeRTOS
        'Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c',
        'Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c',
        'Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1/port.c',
        'Middlewares/Third_Party/FreeRTOS/Source/croutine.c',
        'Middlewares/Third_Party/FreeRTOS/Source/list.c',
        'Middlewares/Third_Party/FreeRTOS/Source/queue.c',
        'Middlewares/Third_Party/FreeRTOS/Source/tasks.c',
        'Middlewares/Third_Party/FreeRTOS/Source/timers.c',
        # FatFs
        'Middlewares/Third_Party/FatFs/src/diskio.c',
        'Middlewares/Third_Party/FatFs/src/ff.c',
        'Middlewares/Third_Party/FatFs/src/ff_gen_drv.c',
        'Middlewares/Third_Party/FatFs/src/option/unicode.c',
        'Middlewares/Third_Party/FatFs/src/option/syscall.c'
    ]
    return ['/PROJECTS/ARM/STM32CubeH7/' + s for s in srcs]


def getIncs(m: ModuleHandle):
    incs = [
        # LwIP
        'Middlewares/Third_Party/LwIP/src/include',
        'Middlewares/Third_Party/LwIP/system',
        'Middlewares/Third_Party/LwIP/src/apps/httpd',
        # FreeRTOS
        'Middlewares/Third_Party/FreeRTOS/Source',
        'Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS',
        'Middlewares/Third_Party/FreeRTOS/Source/include',
        'Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1',
        # FatFs
        'Middlewares/Third_Party/FatFs/src',
        'Middlewares/Third_Party/FatFs/src/option'
    ]
    return ['/PROJECTS/ARM/STM32CubeH7/' + i for i in incs]