# CMakeList.txt : CMake project for Raytracer, include source and define
# project specific logic here.
#

include(FetchContent)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8 # or latest stable tag
)
FetchContent_MakeAvailable(glm)

# Add source to this project's executable.
add_executable (Raytracer "main.cpp" "Sphere.cpp" "HitPool.cpp" "Camera.cpp" "Interval.cpp" "AABB.cpp" "BVH.cpp" "TextureWrapper.cpp"  "ImageLoader.cpp" "Perlin.cpp")
target_link_libraries(Raytracer PRIVATE glm)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Raytracer PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
