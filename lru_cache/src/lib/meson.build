lib = library(
    'lru_cache',
    [ 'frames.c', 'lrul.c', 'hmap.c', 'lru_cache.c' ],
    install : true,
    include_directories : inc,
    )

pkg = import('pkgconfig')
pkg.generate(lib)
