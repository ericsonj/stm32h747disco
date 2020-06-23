import os
from os.path import basename
from pymakelib import git
from pymakelib import MKVARS


PROJECT_NAME = basename(os.getcwd())
FOLDER_OUT = 'Release/STM32F7xx/'
TARGET_ELF = FOLDER_OUT + PROJECT_NAME + ".elf"
TARGET_MAP = FOLDER_OUT + PROJECT_NAME + ".map"
TARGET_HEX = FOLDER_OUT + PROJECT_NAME + ".hex"
TARGET_BIN = FOLDER_OUT + PROJECT_NAME + ".bin"
TARGET_SIZE = FOLDER_OUT + PROJECT_NAME + ".size"

def getProjectSettings():
    return {
        'PROJECT_NAME': PROJECT_NAME,
        'FOLDER_OUT':   FOLDER_OUT,
    }
    
def getTargetsScript():

    TARGETS = {
        'TARGET': {
            'LOGKEY':  'OUT',
            'FILE':    TARGET_ELF,
            'SCRIPT':  [MKVARS.LD, '-o', '$@', MKVARS.OBJECTS, MKVARS.LDFLAGS, MKVARS.STATIC_LIBS]
        },
        'TARGET_HEX': {
            'LOGKEY':   'HEX',
            'FILE':     TARGET_HEX,
            'SCRIPT':   [MKVARS.OBJCOPY, '-O', 'ihex', MKVARS.TARGET, TARGET_HEX]
        },
        'TARGET_BIN': {
            'LOGKEY':   'BIN',
            'FILE':     TARGET_BIN,
            'SCRIPT':   [MKVARS.OBJCOPY, '-O', 'binary', MKVARS.TARGET, TARGET_BIN]
        },
        'TARGET_SIZE': {
            'LOGKEY':   'SIZE',
            'FILE':     TARGET_SIZE,
            'SCRIPT':   [MKVARS.SIZE, '-Ax', MKVARS.TARGET, '>', TARGET_SIZE]
        },
        'RESUME':   {
            'LOGKEY':   '>>',
            'FILE':     'RESUME',
            'SCRIPT':   ['@pybuildanalyzer', TARGET_MAP]
        }
    }

    return TARGETS



def getCompilerSet():
    toolchain = '/opt/gcc-arm-none-eabi-8-2018-q4-major/'
    pfx = toolchain + 'bin/arm-none-eabi-'
    return {
        'CC':       pfx + 'gcc',
        'CXX':      pfx + 'g++',
        'LD':       pfx + 'gcc',
        'AR':       pfx + 'ar',
        'AS':       pfx + 'as',
        'OBJCOPY':  pfx + 'objcopy',
        'SIZE':     pfx + 'size',
        'OBJDUMP':  pfx + 'objdump',
        'INCLUDES': [
            toolchain + 'arm-none-eabi/include',
            toolchain + 'arm-none-eabi/include/c++/8.2.1',
            toolchain + 'arm-none-eabi/include/c++/8.2.1/arm-none-eabi',
            toolchain + 'arm-none-eabi/include/c++/8.2.1/backward',
            toolchain + 'lib/gcc/arm-none-eabi/8.2.1/include',
            toolchain + 'lib/gcc/arm-none-eabi/8.2.1/include-fixed'
        ]
    }


def getCompilerOpts():

    PROJECT_DEF = {
        'USE_HAL_DRIVE':            None,
        'STM32H747xx':              None,
        'USE_STM32H747I_Discovery': None,
        'USE_IOEXPANDER':           None,
        'CORE_CM7':                 None,
        'VERSION':                  "1.0.1",
        'IS_BETA':                  False,
        'APP_NUMBER':               1,
        'COMMIT_HASH':              git.getCommitHash(),
        'BRANCH_NAME':              git.getBranchName(),
        'CODE_VERSION':             git.getDescribe(),
        'HAVE_CONFIG_H':            None,
        'HAVE_ALLOCA_H':            None,
        'CONFIG_YAFFS_DIRECT':      None,
        'CONFIG_YAFFS_DEFINES_TYPES': None,
        '__rtems__':                None
    }

    return {
        'MACROS': PROJECT_DEF,
        'MACHINE-OPTS': [
            '-mcpu=cortex-m7',
            '-mfpu=fpv5-d16',
            '-mfloat-abi=hard',
            '-mthumb'
        ],
        'OPTIMIZE-OPTS': [
            '-O0'
        ],
        'OPTIONS': [
            '-ffunction-sections',
            '-fstack-usage'
        ],
        'DEBUGGING-OPTS': [
            '-g3'
        ],
        'PREPROCESSOR-OPTS': [
            '-MP',
            '-MMD'
        ],
        'WARNINGS-OPTS': [
            '-Wall'
        ],
        'CONTROL-C-OPTS': [
            '-std=gnu11'
        ],
        'GENERAL-OPTS': [
            '--specs=nano.specs'
        ]
    }


def getLinkerOpts():
    return {
        'LINKER-SCRIPT': [
            '-TPort/STM32H7xx/linker/STM32H747XIHx_FLASH_CM7_ETH.ld'
        ],
        'MACHINE-OPTS': [
            '-mcpu=cortex-m7',
            '-mfpu=fpv5-d16',
            '-mfloat-abi=hard',
            '-mthumb'
        ],
        'GENERAL-OPTS': [
            '--specs=nosys.specs'
        ],
        'LINKER-OPTS': [
            '-Wl,-Map='+TARGET_MAP,
            '-Wl,--gc-sections',
            '-static',
            '-Wl,--start-group',
            '-lc',
            '-lm',
            '-Wl,--end-group'
        ]
    }
