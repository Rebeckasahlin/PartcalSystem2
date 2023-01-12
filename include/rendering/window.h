#ifndef __RENDERING_H__
#define __RENDERING_H__

#include <core/color.h>
#include <core/vec2.h>
#include <string_view>
#include <span>

/// This namespace contains objects that are helper functions that support the rendering
/// of general ui elements, particles, emitters, and forces.
namespace rendering {


class Window {
public:
    Window();
    ~Window();
    
    // Returns a time value in seconds since some epoch
    double time() const;

    // Draw and UI operations should be committed between begin and end frame
    void beginFrame();
    void endFrame();

    // Clear the window with specific color
    void clear(Color color);

    void drawRectangle(vec2 pos, vec2 size, Color color);
    void drawRectangle(std::span<vec2> pos, std::span<vec2> size, std::span<Color> color);

    // Draws a rectangle by extracting fields from packed data.
    // The stride represents the number of bytes between each rectangle primitive.
    void drawRectangle(const vec2* pos, const vec2* size, const Color* color, size_t count, size_t stride_in_bytes = 0);

    void drawSphere(vec2 pos, float radius, Color color);
    void drawSphere(std::span<vec2> pos, std::span<float> radius, std::span<Color> color);
    
    // Draws a sphere by extracting fields from packed data.
    // The stride represents the number of bytes between each sphere primitive.
    void drawSphere(const vec2* pos, const float* radius, const Color* color, size_t count, size_t stride_in_bytes = 0);
    
    // UI
    bool button(std::string_view label) const;
    bool sliderFloat(std::string_view id, float& value, float min, float max) const;
    bool sliderVec2(std::string_view id, vec2& value, float min, float max) const;
    bool sliderInt(std::string_view id, int& value, int min, int max) const;
    bool colorPicker(std::string_view id, Color& color) const;

    // Horizontal line separator
    void separator() const;

private:
    struct WindowHandle;
    WindowHandle* hwnd;
};



/**
 * Sets the background color that is used for clearing the window.
 *
 * \param r The red channel of the background color in the range [0, 1]
 * \param g The green channel of the background color in the range [0, 1]
 * \param b The blue channel of the background color in the range [0, 1]

 * \pre \p r must be in the range [0, 1]
 * \pre \p g must be in the range [0, 1]
 * \pre \p b must be in the range [0, 1]
 */
void setBackgroundColor(float r, float g, float b);

/**
 * Sets up the initial frame state and must be called in the beginning of the frame. It
 * will poll and respond to all the keyboard and mouse input and set up the user interface
 * that can be called during the frame.
 *
 * \return The time that has passed since the beginning of the last frame in seconds
 *         (= delta time, dt)
 */
[[ nodiscard ]] float beginFrame();

/**
 * Renders a list of particles.
 * 
 * \param particles The information about the particles that are rendered in the next call
 *        to the render function
 * \pre The createWindow function has been called exactly once in the application
 * \pre The beginFrame has been called since the beginning of this frame
 */
void renderParticles(const std::vector<ParticleInfo>& particles);

/**
 * Renders list of emitters.
 *
 * \param emitters The information about the emitters that are rendered in the next call
 *        to the render function
 * \pre The createWindow function has been called exactly once in the application
 * \pre The beginFrame has been called since the beginning of this frame
 */
void renderEmitters(const std::vector<EmitterInfo>& emitters);

/**
 * Renders list of forces.
 *
 * \param forces The information about the forces that are rendered in the next call to
 *        the render function
 * \pre The createWindow function has been called exactly once in the application
 * \pre The beginFrame has been called since the beginning of this frame
 */
void renderForces(const std::vector<ForceInfo>& forces);

/**
 * Finalizes the current frame and swaps the front and back buffers for double buffering.
 * The function returns whether the render loop should continue or if the user requested
 * that the window should close.
 *
 * \return \c true if the rendering should continue, \c false if the rendering should end
 */
[[ nodiscard ]] bool endFrame();

} // namespace rendering

/**
 * This namespace contains helper functions that can be used to draw user interfaces. Most
 * of the functions operate in a similar fashion; within the ui::begin() and ui::end() you
 * the functions you need to create your UI.
 * In most cases the function returns \c true if the user interacted with the UI (pressed
 * a button, changed a value, etc) and the variable(s) passed as parameters to the
 * function are changed if the user interacted with the element).
 *
 * The one exception to this are the Group functions, which will create a group by calling
 * beginGroup() and will end the group by calling endGroup()
 *
 */
namespace ui {

/**
 * \brief RAII object for drawing ImGUI components.
 * On construction: Tells ImGui that we are starting a new frame . 
 * On destruction: Finalizes the ImGui UI elements and renders them to the screen.
 */
struct GuiScope {
    GuiScope();
    // Disallow copying and assignment
    GuiScope(GuiScope const&) = delete;
    GuiScope& operator=(GuiScope const& that) = delete;
    GuiScope(GuiScope&& rhs) = delete;
    GuiScope& operator=(GuiScope&& that) = delete;

    ~GuiScope();
};




/**
* This UI element shows a simple colored text in the user interface
* 
* \param text The text that should be rendered
* \param color Color of the text [optional] (default is white)
* \pre \p text must not be empty
*/
void text(const char* text, Color color = {1,1,1});

/**
* * Draws a slider with the provided \p label and the minimum value of \p minValue
* and maximum value of \p maxValue.
* 
* If the user changed the value for the slider, the return value will be
* \c true and \p value contains the changed value. If the user
* did not interact with the slider, the value passed to \p value will not be
* modified.
* 
* \pre \p label must not be a nullptr
* \pre \p label must not be empty
* \pre \p minValue must be smaller than \p maxValue
*
* \param value The in/out parameter for the value displayed by the slider
* \return \c true if the user interacted with the slider, \c false otherwise
*/
bool sliderFloat(const char* label, float& value, float minValue, float maxValue);
bool sliderInt(const char* label, int& value, int minValue, int maxValue);
bool sliderVec2(const char* label, vec2& value, float minValue, float maxValue);

/**
* Draws a color picker into the UI. If the user changed the color, the return
* value will be \c true and \p r, \p g, and \p b contain the color components of the
* new color. If the user did not interact with the picker, the passed color will not
* be modified.
*
* \pre \p label must not be a nullptr
* \pre \p label must not be empty
* \param color The in/out parameter for the color, all components (r,g,b) operate in the range [0, 1]
* \return \c true if the user picked a new color, \c false otherwise
*/
bool colorPicker(const char* label, Color& color);


/**
* Draws a new button with the provided \p label. If the user pressed the button, this function returns
* \c true, otherwise, \c false is returned.
*
* \param label The text that is written inside the button
* \pre \p label must not be a nullptr
* \pre \p label must not be empty
*
* \return \c true if the user pressed the button, \c false otherwise
*/
bool button(const char* label);


bool checkbox(const char* label, bool& value);

/**
* Creates a group with a specific name \p label.
* Should be terminated with endGroup.
*
* \param label The name of the group
* \pre \p label must not be a nullptr
* \pre \p label must not be empty
*/
void beginGroup(const char* label);
void endGroup();

} // namespace ui

#endif // __RENDERING_H__
