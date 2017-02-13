cmake_minimum_required (VERSION 3.3)
project (Ord-TP)

#set (CMAKE_CXX_FLAGS "-fexceptions")
find_package (Threads) # sinon utiliser l'option -lpthread avec gcc

add_executable(thread_periodique.bin thread_periodique.c)
#SET_TARGET_PROPERTIES(thread_periodique.bin PROPERTIES LINK_FLAGS -Wl,-lpthread)

add_executable(threads_concurrents.bin threads_concurrents.c)
#SET_TARGET_PROPERTIES(threads_concurrents.bin PROPERTIES LINK_FLAGS -Wl,-lpthread)

target_link_libraries (thread_periodique.bin ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (threads_concurrents.bin ${CMAKE_THREAD_LIBS_INIT})