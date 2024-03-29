cmake_minimum_required(VERSION 3.4.1)
# set targetPlatform, will be passed in from gradle when this sample is completed
# openGL Supportability
# platform         status
#   (0 12)           ES2/ES3 not supported
#   [12, 18)         ES2 only; for ES3, app do dynamic load/detection
#                    this applies to the situations that:
#                        - minimum API is set to less than 18. In this case
#                          there is no ES3 header/lib support inside NDK
#                        - the built APK might be running on newer API phones
#                    with dynamic loading of ES3, the same APK would still be able
#                    to use ES3. Otherwise, app would stuck with ES2 even phone is
#                    is newer than the minimum API level (for example, Android-27 etc).
#
#   [18, 24)         ES2 & ES3
#                    If app is built to only support API-18 or later,
#                    set minimum api level to 18 is good enough, NDK supprts ES3
#                    with the right header and lib files. No need to use ES3 dynamic
#                    detection.
#   [24, infinite)   ES2 & ES3 & Vulkan

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")
set(OPENGL_LIB GLESv3)

add_library(minesweeper SHARED
            stb_image.h
            stb_image_write.h
            stb.h
            stb.cpp
            linear.h
            render.h
            board.h
            board.cpp
            geom.h
            geom.cpp
            multrect.h
            multrect.cpp
            rectangle.h
            rectangle.cpp
            prog.h
            prog.cpp
            minesweeper.cpp )

# Include libraries needed for minesweeper lib
target_link_libraries(minesweeper
            ${OPENGL_LIB}
            android
            EGL
            log
            m)
