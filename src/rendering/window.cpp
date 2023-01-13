#include "rendering/window.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Tracy.hpp"
#include <array>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Dear students;
// if you have found your way here, rest assured that understanding the rest of this file
// is *not* required to complete the lab. This file contains a lot of implementation for
// the rendering helper functionalities. Just have a look at the corresponding header file
// "rendering.h" and look at the structs and functions that are exposed there. They are
// all documented so that looking at the source code should not be necessary.
// Having said that, if you are interested in anything, of course continue browsing here

constexpr size_t VBO_CAP = 1024*1024;

// Internal definition of window implementation
struct rendering::Window::Impl {
    Impl(std::string_view title, int width, int height);
    ~Impl();

    GLFWwindow* window;
    
    GLuint program;
    GLuint vao;
    GLuint vbo;
};

// This structure represents how the points is stored in the vertex buffer
struct Point {
    vec2     position;
    float    scale;
    uint32_t color_packed;
};

/**
 * Checks the compilation status of the shader passed into it and prints out a message in
 * case the shader was not compiled successfully. If the shader is successfully compiled,
 * no message is printed. This function does **not** compile the shader.
 * This function only performs a check if it is compiled in Debug mode. In release builds
 * these checks will be omitted.
 *
 * \param shader The GL object of the shader object that should be checked
 * \param name The human-readable name of the shader object used in the printing
 *
 * \throw std::runtime_error An exception is raised if the compilation of the shader
 *        failed
 * \pre \p shader must be a valid GL shader object
 * \pre \p name must not be empty
 * \post Only the compile status of \p shader might be modified
 */
static bool checkShader([[ maybe_unused ]] GLuint shader,
                 [[ maybe_unused ]] std::string_view name)
{
    ZoneScoped

    assert(shader != 0);
    assert(!name.empty());

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> buf;
        buf.resize(logLength);
        glGetShaderInfoLog(shader, logLength, NULL, buf.data());

        std::cout << "Error compiling shader: " << name << '\n' << std::string(buf.data());
    }

    return status == GL_TRUE;
}

/**
 * Checks the linking status of the passed shader program passed into it and prints out a
 * message in case the program was not linked successfully. If the program was linked
 * successfully, no message is printed. This function does **not** link the program.
 * This function only performs a check if it is compiled in Debug mode. In release builds
 * these checks will be omitted.
 *
 * \param program The shader program that is to be checked
 * \param name The human-readable name of the shader program used in the printing
 *
 * \throw std::runtime_error An exception is raised if the linking of the program failed
 *
 * \pre \p program must be a valid GL program object
 * \pre \p name must not be empty
 * \post Only the link status of \p program might be modified
 */
static bool checkProgram([[ maybe_unused ]] GLuint program,
                  [[ maybe_unused ]] std::string_view name)
{
    ZoneScoped

    assert(program != 0);
    assert(!name.empty());

    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> buf;
        buf.resize(logLength);
        glGetProgramInfoLog(program, logLength, NULL, buf.data());

        std::cout << "Error linking program: " << name << '\n' << std::string(buf.data());
    }

    return status == GL_TRUE;
}


/**
 * Checks whether any of the GL functions called since the last call to this function have
 * raised any errors. If there were multiple errors, only the first error will be printed.
 * If there haven't been any errors, no message is printed. If this function was compiled
 * in a release build, all error messages are omitted.
 *
 * \param name The human-readable name that is used in the logging
 *
 * \throw std::runtime_error An exception is raised if there was an OpenGL error
 *
 * \pre \p name must not be empty
 * \post If compiling this function in Debug mode, the OpenGL error will be GL_NO_ERROR
 */
static void checkOpenGLError([[ maybe_unused ]] std::string_view name) {
#ifdef _DEBUG
    GLenum e = glGetError();
    if (e != GL_NO_ERROR) {
        std::cout << "OpenGL error occurred( " << name << ") " << e << '\n';
        throw std::runtime_error("OpenGL error occurred");
    }
#endif // _DEBUG
}

// Creates the shader program for points.
static GLuint createPointProgram() {
    ZoneScoped

    constexpr const char* vsSrc[1] = { R"(
        #version 330
        layout(location = 0) in vec2  in_position;
        layout(location = 1) in float in_scale;
        layout(location = 2) in vec4  in_color;

        out vec4 vs_color;

        void main() {
            vs_color = in_color;
            gl_PointSize = in_scale;
            gl_Position = vec4(in_position, 0.0, 1.0);
        }
    )" };

    constexpr const char* fsSrc[1] = { R"(
        #version 330
        in vec4 vs_color;
        out vec4 out_color;

        void main() {
            out_color = vec4(vs_color);
        }
    )" };
    
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, vsSrc, nullptr);
    glCompileShader(vertex);

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, fsSrc, nullptr);
    glCompileShader(fragment);

    GLuint program = 0;
    if (checkShader(vertex, "point-vertex") && checkShader(fragment, "point-fragment")) {
        program = glCreateProgram();
        
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        
        glLinkProgram(program);
        checkProgram(program, "point-program");

        glDetachShader(program, vertex);
        glDetachShader(program, fragment);
    }
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

namespace rendering {

Window::Impl::Impl(std::string_view, int width, int height) {
    ZoneScoped

    // Initialize GLFW for window handling
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, "Particle System", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);    

    // Initialize the GLAD OpenGL wrapper
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    // Initialize ImGui UI library
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    ImGui_ImplOpenGL3_Init();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.f;
    style.WindowTitleAlign.x = 0.5f;
    style.WindowMenuButtonPosition = 0;

    // Create GL objects
    program = createPointProgram();
    glGenVertexArrays(3, &vao);
    glGenBuffers(3, &vbo);

    // Allocate vertex buffer memory
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, VBO_CAP * sizeof(Point), nullptr, GL_DYNAMIC_DRAW);

    // Setup vertex attribute pointers for Points
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);

    glBindVertexArray(0);
 
    checkOpenGLError("postInit");
}

Window::Impl::~Impl() {
    ZoneScoped

    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);

    // @NOTE: This is should be performed outside the lifetime of a single window
    glfwTerminate();
}

Window::Window(std::string_view title, int width, int height) : impl(new Impl(title, width, height)) {}

Window::~Window() {}

void Window::clear(Color color) {
    // Clear the rendering buffer with the selected background color
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

double Window::time() const {
    return glfwGetTime();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(impl->window);
}

void Window::beginFrame() {
    ZoneScoped
    checkOpenGLError("beginFrame");

    // Query the events from the operating system, such as input from mouse or keyboards
    glfwPollEvents();

    // Set the default OpenGL state
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    int width, height;
    glfwGetWindowSize(impl->window, &width, &height);

    // Update render window
    glViewport(0, 0, width, height);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::drawPoint(vec2 pos, float radius, Color color) {
    drawPoints(&pos, &radius, &color, 1);
}

void Window::drawPoints(std::span<const vec2> pos, std::span<const float> radius, std::span<const Color> color) {
    const size_t count = pos.size();
    assert(count == radius.size() && count == color.size());
    drawPoints(pos.data(), radius.data(), color.data(), count);
}

static constexpr inline uint32_t packColor(Color color) {
    unsigned int out = 0;
    out |= ((unsigned int)(std::clamp(color.r, 0.0f, 1.0f) * 255.0f + 0.5f)) << 0;
    out |= ((unsigned int)(std::clamp(color.g, 0.0f, 1.0f) * 255.0f + 0.5f)) << 8;
    out |= ((unsigned int)(std::clamp(color.b, 0.0f, 1.0f) * 255.0f + 0.5f)) << 16;
    out |= ((unsigned int)(std::clamp(color.a, 0.0f, 1.0f) * 255.0f + 0.5f)) << 24;
    return out;
}

void Window::drawPoints(const vec2* pos, const float* radius, const Color* color, size_t count, size_t stride_in_bytes) {
    ZoneScoped
    // Plot the number of points and make them available through Tracy
    TracyPlot("Points", int64_t(count));
    
    if (count > VBO_CAP) {
        throw std::runtime_error("Too many rectangles to draw in a single call");
    }
        
    // Upload the passed particle information to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, impl->vbo);
    Point* point_data = static_cast<Point*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    if (point_data) {
        for (size_t i = 0; i < count; ++i) {
            point_data[i] = {pos[i], radius[i], packColor(color[i])};
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    } else {
        throw std::runtime_error("Failed to map buffer");
    }
    
    
    glBindVertexArray(impl->vao);
    glUseProgram(impl->program);
    glDrawArrays(GL_POINTS, 0, static_cast<int>(count));
    glUseProgram(0);
    glBindVertexArray(0);

    checkOpenGLError("drawPoint");
}


void Window::endFrame() {
    ZoneScoped

    {
        ZoneScopedN("Render UI")
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    {
        // Swapping the front and back buffer. Since we are doing v-sync is enabled, this
        // call will block until its our turn to swap the buffers (usually every 16.6 ms
        // on a 60 Hz monitor
        ZoneScopedN("Swap buffers")
        glfwSwapBuffers(impl->window);
    }

    checkOpenGLError("endFrame");
    FrameMark
}

// @TODO: PETER: Touch this up with a scalable solution which puts it on the heap if it does not fit

// Copy string_views into a zero terminated stack based string which is compatible with ImGui
// That is implicitly convertible to c-style strings (const char*)
struct CStr {
    std::array<char, 128> buf;

    constexpr CStr(std::string_view str) {
        const size_t size = std::min(str.size(), buf.size() - 1);
        std::copy_n(str.data(), size, buf.data());
        buf[size] = '\0';
    }

    constexpr operator const char*() const {
        return buf.data();
    }
};

void Window::beginGuiWindow(std::string_view label) {
    ImGui::Begin(CStr(label));
}

void Window::endGuiWindow() {
    ImGui::End();
}

void Window::text(std::string_view text) {
    ZoneScoped
    ImGui::Text(CStr(text));
}

void Window::text(std::string_view text, Color color) {
    ZoneScoped
    ImGui::TextColored({color.r, color.g, color.b, 1.0f}, CStr(text));
}

bool Window::sliderFloat(std::string_view label, float& value, float minValue, float maxValue) {
    ZoneScoped
    return ImGui::SliderFloat(CStr(label), &value, minValue, maxValue);
}

bool Window::sliderInt(std::string_view label, int& value, int minValue, int maxValue) {
    ZoneScoped
    return ImGui::SliderInt(CStr(label), &value, minValue, maxValue);
}

bool Window::sliderVec2(std::string_view label, vec2& value, float minValue, float maxValue) {
    ZoneScoped
    const float speed = (maxValue - minValue) / 2000.f;
    return ImGui::DragFloat2(CStr(label), &value.x, speed, minValue, maxValue);
}

bool Window::colorPicker(std::string_view label, Color& color) {
    ZoneScoped
    return ImGui::ColorEdit3(CStr(label), &color.r);
}

bool Window::button(std::string_view label) {
    ZoneScoped
    return ImGui::Button(CStr(label));
}

bool Window::checkbox(std::string_view label, bool& value) {
    ZoneScoped
    return ImGui::Checkbox(CStr(label), &value);
}

void Window::separator() {
    ImGui::Separator();
}

}  // namespace rendering