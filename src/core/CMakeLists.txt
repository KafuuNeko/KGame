cmake_minimum_required(VERSION 3.9)

add_library(core STATIC 
    #context
    context/context.cc

    #items
    items/items_manager.cc
)

target_link_libraries(core PRIVATE 
    sqlite 
    cppcrc
    aes 
    md5 
    fmt
)

