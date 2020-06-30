from pymakelib.Module import ModuleHandle, StaticLibrary
import hashlib
from pathlib import Path, PosixPath

def init(mh: ModuleHandle):
    staticLib = StaticLibrary(name='drivers', outputDir='Release/STM32F7xx')
    if mh.getGoal() == 'all':
        try:
            linked = hashlib.md5(open('Port/STM32H7xx/linker/STM32H747XIHx_FLASH_CM7_ETH.ld', 'rb').read()).hexdigest()
            staticLib.rebuildByCheckStr(str(mh.getGeneralCompilerOpts().opts) + str(linked))
        except Exception as e:
            print(e)
    return staticLib


def getSrcs(mh: ModuleHandle):
    try:

        bsp = 'Drivers/BSP/STM32H747I-Discovery'
        bsp_srcs = list(Path(bsp).rglob('*.c'))

        hal = '/PROJECTS/ARM/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver'
        hal_srcs = list(Path(hal).rglob('*.c'))

        no_hal_temp = list(Path(hal).rglob('*_template.c'))
        hal_srcs = list(set(hal_srcs) - set(no_hal_temp))

        no_hal_ll = list(Path(hal).rglob('stm32h7xx_ll_*'))
        hal_srcs = list(set(hal_srcs) - set(no_hal_ll))

        bsp_srcs.remove(PosixPath('Drivers/BSP/STM32H747I-Discovery/stm32h747i_discovery_ts.c'))
        bsp_srcs.remove(PosixPath('Drivers/BSP/STM32H747I-Discovery/stm32h747i_discovery_camera.c'))

        ll_srcs = [
            '/PROJECTS/ARM/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_fmc.c',
            '/PROJECTS/ARM/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_sdmmc.c',
            '/PROJECTS/ARM/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_delayblock.c'
        ]

        comp_srcs = [
            '/PROJECTS/ARM/STM32CubeH7/Drivers/BSP/Components/lan8742/lan8742.c',
            '/PROJECTS/ARM/STM32CubeH7/Drivers/BSP/Components/otm8009a/otm8009a.c',
            '/PROJECTS/ARM/STM32CubeH7/Drivers/BSP/Components/otm8009a/otm8009a_reg.c',
            '/PROJECTS/ARM/STM32CubeH7/Drivers/BSP/Components/wm8994/wm8994.c',
            '/PROJECTS/ARM/STM32CubeH7/Drivers/BSP/Components/wm8994/wm8994_reg.c',
            '/PROJECTS/ARM/STM32CubeH7/Drivers/BSP/Components/is42s32800j/is42s32800j.c',
            '/PROJECTS/ARM/STM32CubeH7/Drivers/BSP/Components/mt25tl01g/mt25tl01g.c'
        ]

        return bsp_srcs + hal_srcs + comp_srcs + ll_srcs

    except Exception as e:
        print(e)


def getIncs(mk: ModuleHandle):
    return [
        'Drivers/BSP/STM32H747I-Discovery',
        '/PROJECTS/ARM/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc',
        '/PROJECTS/ARM/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy',
        '/PROJECTS/ARM/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include',
        '/PROJECTS/ARM/STM32CubeH7/Drivers/CMSIS/Include',
        'Drivers/BSP/Components/lan8742',
        'Drivers/BSP/Components/mt25tl01g',
        'Drivers/BSP/Components/otm8009a',
        'Drivers/BSP/Components/wm8994',
        'Drivers/BSP/Components/Common',
        'Drivers/BSP/Components/is42s32800j'
    ]
