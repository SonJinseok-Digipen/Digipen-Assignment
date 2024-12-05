``` c
cmake_minimum_required(VERSION 3.5.0)

  

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

  

project(HookBill VERSION 0.1.0 LANGUAGES C CXX)

include(Dependency/Dependency.cmake)

  

add_subdirectory(Hookbill-Physics)   ##Physics library

add_subdirectory(Hookbill-Graphics)  ##Graphics library

add_subdirectory(Hookbill-Game)  ## Game.exe      

add_subdirectory(Hookbill-Engine)

add_subdirectory(Hookbill-Editor)
```


## [[HookBill-Engine]]



## [[HookBill-Physics]]






