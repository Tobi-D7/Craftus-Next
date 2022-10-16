#include <stdlib.h>
#include <stdio.h>
#include <thread>
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
#define __DESKTOP__
#include <NImGui/NImGui.hpp>
#include <Error.hpp>



static const char* const vertexShaderSource = R"text(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 ourColor;
    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        ourColor = aColor;
    }
)text";

static const char* const fragmentShaderSource = R"text(
    #version 330 core
    in vec3 ourColor;
    out vec4 fragColor;
    void main()
    {
        fragColor = vec4(ourColor, 1.0f);
    }
)text";

static GLuint createAndCompileShader(GLenum type, const char* source)
{
    GLint success;
    GLchar msg[512];

    GLuint handle = glCreateShader(type);
    if (!handle)
    {
        return 0;
    }
    glShaderSource(handle, 1, &source, nullptr);
    glCompileShader(handle);
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(handle, sizeof(msg), nullptr, msg);
        glDeleteShader(handle);
        return 0;
    }

    return handle;
}

static GLuint s_program;
static GLuint s_vao, s_vbo;

static void sceneInit()
{
    GLint vsh = createAndCompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLint fsh = createAndCompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    s_program = glCreateProgram();
    glAttachShader(s_program, vsh);
    glAttachShader(s_program, fsh);
    glLinkProgram(s_program);

    GLint success;
    glGetProgramiv(s_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char buf[512];
        glGetProgramInfoLog(s_program, sizeof(buf), nullptr, buf);
    }
    glDeleteShader(vsh);
    glDeleteShader(fsh);

    struct Vertex
    {
        float position[3];
        float color[3];
    };

    static const Vertex vertices[] =
    {
        { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        { {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        { {  0.0f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    };

    glGenVertexArrays(1, &s_vao);
    glGenBuffers(1, &s_vbo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(s_vao);

    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

static void sceneRender()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our first triangle
    glUseProgram(s_program);
    glBindVertexArray(s_vao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

static void sceneExit()
{
    glDeleteBuffers(1, &s_vbo);
    glDeleteVertexArrays(1, &s_vao);
    glDeleteProgram(s_program);
}

NImGui::Timer deltaclock;
NImGui::Timer frameclock;
int frames;
int fps = 0;
float deltatime = 0.f;

bool vsy = true;

std::string FrameRate()
{
    frames ++;
    if ((frameclock.GetAsMs()/1000) >= 1.f)
    {
        fps = frames;
        frames = 0;
        frameclock.Reset();
    }
    return std::to_string(fps);
}

bool initps = true;

void task1(std::string msg)
{
    while (initps)
    {
        //std::cout << "task1 says: " << msg;
    }
}

int main(void)
{   
    NImGui::App app("Craftus-Next", NImGui::Vec2i(900, 400), NImGui::BORDERLESS | NImGui::TRANSPARENT);
    app.SetWindowPos(NImGui::Vec2i((app.GetMonitorSize().x/2)-(app.GetWindowSize().x/2), (app.GetMonitorSize().y/2)-(app.GetWindowSize().y/2)));
    sceneInit();
    ErrorCode code;
    NImGui::Image testt;
    testt.LoadImage("loading.png");
    NImGui::Timer clk;
    std::thread t1(task1, "Hello");
    while(app.IsRunning())
    {
        ImGui::Begin("Test", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowPos(ImVec2(app.GetWindowPos().x, app.GetWindowPos().y));
        ImGui::SetWindowSize(ImVec2(app.GetWindowSize().x, app.GetWindowSize().y));
        ImGui::Image(testt.GetTextureID(), testt.GetSize());

        ImGui::SetCursorPos(ImVec2(37, 380));
        ImGui::Text("Loading %c -> %s", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3], "Craftus-Next");
        ImGui::ProgressBar(((clk.GetAsMs()/1000)/7), ImVec2(-283, 2));
        
        ImGui::End();
        app.SwapBuffers();
        if((clk.GetAsMs()/1000) > 7)
        {
            initps = false;
            t1.join();
            break;
        }
    }
    
    app.SetWindowSize(NImGui::Vec2i(1280, 720));
    bool updt = false;
    while(app.IsRunning())
    {
        if(!updt)
        {
            app.SetWindowPos(NImGui::Vec2i((app.GetMonitorSize().x/2)-(app.GetWindowSize().x/2), (app.GetMonitorSize().y/2)-(app.GetWindowSize().y/2)));
            updt = true;    
        }
        app.SetVsync(vsy);
        deltatime = deltaclock.GetAsMs();
        deltaclock.Reset();
        ImGui::Begin("Fps");
        ImGui::Text("Fps -> %s", FrameRate().c_str());
        ImGui::Text("Delta -> %sms", std::to_string(deltatime).c_str());
        ImGui::Checkbox("Vsync", &vsy);
        ImGui::Text("MouseLeft -> %d", (int)app.IsMouseButtonDown(NImGui::MouseButton::Left));
        ImGui::Image(testt.GetTextureID(), testt.GetSize());
        ImGui::Text("Key W -> %d", (int)app.IsKeyDown(NImGui::KeyCode::W));

        ImGui::End();
        sceneRender();
        app.SwapBuffers();
    }
    sceneExit();
    exit(EXIT_SUCCESS);
}
