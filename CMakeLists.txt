CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(main)
set(CMAKE_CXX_STANDARD 17)
link_libraries(stdc++fs)
link_libraries(nvinfer nvonnxparser nvcaffe_parser)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_libraries(${OpenCV_LIBS})

find_package(CUDA REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})
link_libraries(${CUDA_LIBRARIES})

link_libraries(avcodec avformat avutil swresample swscale)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)
AUX_SOURCE_DIRECTORY(deepsort DIR_SRCS)
ADD_EXECUTABLE(main ${DIR_SRCS})