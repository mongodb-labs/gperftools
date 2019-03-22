# -*- mode: python -*-

Import("env")
Import("has_option")
Import("debugBuild")

env = env.Clone()

files = [
    'src/base/dynamic_annotations.c',
    'src/base/elf_mem_image.cc',
    'src/base/logging.cc',
    'src/base/spinlock.cc',
    'src/base/spinlock_internal.cc',
    'src/base/sysinfo.cc',
    'src/base/vdso_support.cc',
    'src/central_freelist.cc',
    'src/common.cc',
    'src/internal_logging.cc',
    'src/malloc_extension.cc',
    'src/malloc_hook.cc',
    'src/memfs_malloc.cc',
    'src/page_heap.cc',
    'src/sampler.cc',
    'src/span.cc',
    'src/stack_trace_table.cc',
    'src/stacktrace.cc',
    'src/static_vars.cc',
    'src/symbolize.cc',
    'src/thread_cache.cc',
    ]

if env.TargetOSIs('windows'):
    files += [
        'src/tcmalloc.cc',
        'src/windows/port.cc',
        'src/windows/system-alloc.cc',
        'src/fake_stacktrace_scope.cc',
        'src/windows/null_patch_functions.cc',
        ]

    # warning C4141: 'inline': used more than once
    # warning C4305: 'argument': truncation from 'ssize_t' to 'double'
    env.Append(CXXFLAGS=["/wd4141", "/wd4305"])

else:
    files += [
        'src/emergency_malloc_for_stacktrace.cc',
        'src/maybe_threads.cc',
        'src/system-alloc.cc',
        ]

    if not debugBuild:
        files += ['src/tcmalloc.cc'],
    else:
        files += ['src/debugallocation.cc']

if has_option( 'use-cpu-profiler' ):
    files += [
        'src/profile-handler.cc',
        'src/profiledata.cc',
        'src/profiler.cc',
        ]


conf = Configure(env.Clone())

if has_option('use-cpu-profiler'):
    if not conf.CheckLib('unwind', autoadd=False):
        env.ConfError("Compiling with --use-cpu-profiler requires having libunwind installed")
    conf.env.Append(
        CPPDEFINES=["NO_FRAME_POINTER", ("HAVE_LIBUNWIND_H", "1"), 'HAVE_UCONTEXT_H'],
        SYSLIBDEPS=['unwind']
    )

env = conf.Finish()

env.Append(CPPPATH=["build_" + env["TARGET_OS"] + "_" + env["TARGET_ARCH"]])

env.Append(
    CPPDEFINES=["NO_HEAP_CHECK"],
)

# The build system doesn't define NDEBUG globally for historical reasons, however, TCMalloc
# expects that NDEBUG is used to select between preferring the mmap or the sbrk allocator. For
# non-debug builds, we want to prefer the sbrk allocator since this is TCMallocs preferred
# production deployment configuration. See the use of NDEBUG and kDebugMode in
# src/system-alloc.cc for more details.
if not debugBuild:
    env.Append( CPPDEFINES=["NDEBUG"] )

# For debug builds we want to capture stacks during (de)allocations,
# but we don't want to pay that cost for release builds. For non-debug
# builds we use NO_TCMALLOC_SAMPLES to disable the stack trace
# collection. For debug builds we enable stack capture, but only on
# intel targets, since tcmalloc's unwinder is very slow on other
# platforms (see SERVER-28502).
if (not debugBuild) or (not env['TARGET_ARCH'] in ['x86_64', 'i386']):
    env.Append(CPPDEFINES=["NO_TCMALLOC_SAMPLES"])

env.Prepend( CPPPATH=["src/"] )

def removeIfPresent(lst, item):
    try:
        lst.remove(item)
    except ValueError:
        pass

for to_remove in ['-Werror', "-Wsign-compare","-Wall","-Werror=unused-result"]:
    removeIfPresent(env['CCFLAGS'], to_remove)

# GCC on PowerPC under C++11 mode does not define __linux which gperftools depends on
if env['TARGET_ARCH'] == 'ppc64le':
    env.Append( CPPDEFINES=[ "__linux"] )

env.Library(
    target='tcmalloc_minimal',
    source=files,
)
