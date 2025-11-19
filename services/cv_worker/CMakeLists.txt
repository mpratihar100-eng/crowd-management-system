project(cv_worker C)

set(SOURCES src/main.c src/detector.c)
add_executable(cv_worker ${SOURCES})
target_include_directories(cv_worker PRIVATE include)
