#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_SSE2
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>
#include <stb_image_write.h>

#include <tiny_obj_loader.h>

#include <EnTT/entt.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <node.hpp>

#include <ctime>
#include <chrono>

#include <set>
#include <queue>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <string_view>

#include <memory>
#include <random>
#include <stdint.h>
#include <functional>
#include <filesystem>

#include <assert.h>