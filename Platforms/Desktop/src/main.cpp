#include <stdlib.h>
#include <stdio.h>
#include <thread>
#define __DESKTOP__
#include <NImGui/NImGui.hpp>
#include <imgui_internal.h>

#include <Error.hpp>
#include <Base.hpp>

#include <filesystem>
#include <algorithm>
#include <numeric>
#include <future>
#include <string>
#include <mutex>

#include <ini.hpp>

#include <json.hpp>

std::string task_ = "";

nlohmann::json blocksj;
nlohmann::json itemsj;

std::string RemoveExtPath(std::string name)
{
    return remove_ext(GetFileName(name));
}

namespace ImGui {
    
    bool BufferingBar(const char* label, float value,  const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;
        
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = size_arg;
        size.x -= style.FramePadding.x * 2;
        
        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;
        
        // Render
        const float circleStart = size.x * 0.7f;
        const float circleEnd = size.x;
        const float circleWidth = circleEnd - circleStart;
        
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart*value, bb.Max.y), fg_col);
        
        const float t = g.Time;
        const float r = size.y / 2;
        const float speed = 1.5f;
        
        const float a = speed*0;
        const float b = speed*0.333f;
        const float c = speed*0.666f;
        
        const float o1 = (circleWidth+r) * (t+a - speed * (int)((t+a) / speed)) / speed;
        const float o2 = (circleWidth+r) * (t+b - speed * (int)((t+b) / speed)) / speed;
        const float o3 = (circleWidth+r) * (t+c - speed * (int)((t+c) / speed)) / speed;
        
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
    }

    bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;
        
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        
        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size((radius )*2, (radius + style.FramePadding.y)*2);
        
        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;
        
        // Render
        window->DrawList->PathClear();
        
        int num_segments = 30;
        int start = abs(ImSin(g.Time*1.8f)*(num_segments-5));
        
        const float a_min = IM_PI*2.0f * ((float)start) / (float)num_segments;
        const float a_max = IM_PI*2.0f * ((float)num_segments-3) / (float)num_segments;

        const ImVec2 centre = ImVec2(pos.x+radius, pos.y+radius+style.FramePadding.y);
        
        for (int i = 0; i < num_segments; i++) {
            const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
            window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a+g.Time*8) * radius,
                                                centre.y + ImSin(a+g.Time*8) * radius));
        }

        window->DrawList->PathStroke(color, false, thickness);
    }
    
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

bool task = true;

Base::BitmapPrinter pr(512, 512);

bool isev(int num)
{
    if ( num % 2 == 0)
    return true;
    else
    return false;

    return false;
}

int prc = 0;
int prj = 0;

unsigned hash_str(const char* s)
{
   unsigned h = 37;
   while (*s) {
     h = (h * 54059) ^ (s[0] * 76963);
     s++;
   }
   return h % 86969; // or return h % C;
}

INI::INIStructure st;
INI::INIStructure it;

bool task1(std::string msg)
{
    task_ = "Init Loader!";
    Base::BitmapPrinter ll(256, 256);
    NImGui::Timer tm;
    
    pr.SetDecoder(Base::BITMAP2PNG2TEX);

    int fi = 0;
    int fi2 = 0;
    task_ = "Setup Block TexMap";
    if (std::filesystem::is_directory("block"))
    {
    for(auto const& direntd : std::filesystem::directory_iterator{std::filesystem::path{"block"}})
    {
        prc++;
    }
    pr.Clear();
    
    for(auto const& direntd : std::filesystem::directory_iterator{std::filesystem::path{"block"}})
    {
        if(fi*16 > 512)
        {
            fi2++;
            fi = 0;
        }
        if(fi2*16 < 512)
        {
            auto g = direntd.path();
            std::string wk = RemoveExtPath(g.generic_string());
            ll.DecodePNGFile(g.generic_string());
            if(!(ll.GetBitmap().bmp_info_header.width > 16) && !(ll.GetBitmap().bmp_info_header.height > 16))
            {
                pr.DrawBitmap(fi*16, fi2*16, ll.GetBitmap());
                /*st[std::to_string(hash_str(wk.c_str()))]["name"] = wk;
                st[std::to_string(hash_str(wk.c_str()))]["u1"] = std::to_string((float)(((float)fi*(float)16)/(float)512));
                st[std::to_string(hash_str(wk.c_str()))]["v1"] = std::to_string((float)(((float)fi2*(float)16)/(float)512));
                st[std::to_string(hash_str(wk.c_str()))]["u2"] = std::to_string((float)((((float)fi*(float)16)+(float)16)/(float)512));
                st[std::to_string(hash_str(wk.c_str()))]["v2"] = std::to_string((float)((((float)fi2*(float)16)+(float)16)/(float)512));*/
                blocksj[std::to_string(hash_str(wk.c_str()))] = {{"name", wk}, {"u1", (float)(((float)fi*(float)16)/(float)512)}, {"v1", (float)(((float)fi2*(float)16)/(float)512)}, {"u2", (float)((((float)fi*(float)16)+(float)16)/(float)512)}, {"v2", (float)((((float)fi2*(float)16)+(float)16)/(float)512)},};
                fi++;
                task_ = "Generating TexMap:" + wk;
                
            }
            
        }
        
        prj++;
    }
    
    //blockss.generate(st);
    //blockss.write(st);
    
    pr.SavePng("blocks.png");
    }
    pr.Clear();
    fi = 0;
    fi2 = 0;
    task_ = "Setup Item TexMap";
    if (std::filesystem::is_directory("item"))
    {
    for(auto const& direntd : std::filesystem::directory_iterator{std::filesystem::path{"item"}})
    {
        prc++;
    }
    for(auto const& direntd : std::filesystem::directory_iterator{std::filesystem::path{"item"}})
    {
        if(fi*16 > 512)
        {
            fi2++;
            fi = 0;
        }
        if(fi2*16 < 512)
        {
            auto g = direntd.path();
            std::string wk = RemoveExtPath(g.generic_string());
            ll.DecodePNGFile(g.generic_string());
            if(!(ll.GetBitmap().bmp_info_header.width > 16) && !(ll.GetBitmap().bmp_info_header.height > 16))
            {
                pr.DrawBitmap(fi*16, fi2*16, ll.GetBitmap());
                /*it[std::to_string(hash_str(wk.c_str()))]["name"] = wk;
                it[std::to_string(hash_str(wk.c_str()))]["u1"] = std::to_string((float)(((float)fi*(float)16)/(float)512));
                it[std::to_string(hash_str(wk.c_str()))]["v1"] = std::to_string((float)(((float)fi2*(float)16)/(float)512));
                it[std::to_string(hash_str(wk.c_str()))]["u2"] = std::to_string((float)((((float)fi*(float)16)+(float)16)/(float)512));
                it[std::to_string(hash_str(wk.c_str()))]["v2"] = std::to_string((float)((((float)fi2*(float)16)+(float)16)/(float)512));*/
                itemsj[std::to_string(hash_str(wk.c_str()))] = {{"name", wk}, {"u1", (float)(((float)fi*(float)16)/(float)512)}, {"v1", (float)(((float)fi2*(float)16)/(float)512)}, {"u2", (float)((((float)fi*(float)16)+(float)16)/(float)512)}, {"v2", (float)((((float)fi2*(float)16)+(float)16)/(float)512)},};
                fi++;
                task_ = "Generating TexMap:" + wk;
            }
        }
        
        prj++;
    }
    //itemss.write(it);
    std::ofstream blocksl("blocks.json");
    std::ofstream itemsl("items.json");
    blocksl << blocksj.dump(4);
    itemsl << itemsj.dump(4);
    blocksl.close();
    itemsl.close();
    std::cout << "Createt Bitmap in " << tm.GetAsMs()/1000 << "s" << std::endl;
    tm.Reset();
    pr.SavePng("items.png");
    }
    std::cout << "Saved Bitmap in " << tm.GetAsMs()/1000 << "s" << std::endl;
    tm.Reset();
    pr.UpdateScreen();
    std::cout << "Updated Bitmap in " << tm.GetAsMs()/1000 << "s" << std::endl;
    tm.Reset();
    
    task = false;
    return true;
}

bool prevcrash = false;

int main(void)
{   
    NImGui::App app("Craftus-Next", NImGui::Vec2i(900, 400), NImGui::BORDERLESS | NImGui::TRANSPARENT);
    app.SetWindowPos(NImGui::Vec2i((app.GetMonitorSize().x/2)-(app.GetWindowSize().x/2), (app.GetMonitorSize().y/2)-(app.GetWindowSize().y/2)));
    app.LoadIcon("res/icon.png");
    const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

    ErrorCode code;
    NImGui::Image testt;
    testt.LoadImage("res/loading.png");
    std::cout << Base::GetVersion() << " " << Base::GetName() << " " << Base::GetPlatform() << std::endl;
    NImGui::Timer clk;
    //std::thread t1(task1, "Hello");
    std::future<bool> ftt = std::async(task1, "NULL");
    while(app.IsRunning())
    {
        ImGui::Begin("Test", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs);
        prevcrash = true;
        ImGui::SetWindowPos(ImVec2(app.GetWindowPos().x, app.GetWindowPos().y));
        ImGui::SetWindowSize(ImVec2(app.GetWindowSize().x, app.GetWindowSize().y));
        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::Image(testt.GetTextureID(), testt.GetSize());
        
        ImGui::SetCursorPos(ImVec2(37, 375));
        ImGui::Spinner("T", 5, 2, col);
        ImGui::SameLine();
        ImGui::Text("Loading -> %s", task_.c_str());
        //ImGui::Text("Loading %c -> %s", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3], "Craftus-Next");
        
        //ImGui::BufferingBar("T", (prj/prc), ImVec2(600, 6), bg, col);
        //ImGui::ProgressBar((prj/prc), ImVec2(600, 2));
        
        ImGui::End();
        prevcrash = false;
        app.SwapBuffers();
        if(!task)
        {
            initps = false;
            //t1.join();
            break;
        }
    }
    
    app.SetWindowSize(NImGui::Vec2i(1280, 720));
    bool updt = false;
    app.SetFullScreen(true);
    NImGui::Image img;
    img.LoadImage("blocks.png");
    NImGui::Image img2;
    img2.LoadImage("items.png");

    Base::BitmapPrinter llj(20, 20);
    llj.SetDecoder(Base::BITMAP2PNG2TEX);
    llj.Clear();
    llj.DrawRectFilled(0, 0, 20, 20, 255, 255, 255, 255);
    llj.UpdateScreen();
    nlohmann::json lbl;
    nlohmann::json lil;
    std::ifstream fb("blocks.json");
    std::ifstream fi("items.json");
    lbl = nlohmann::json::parse(fb);
    lil = nlohmann::json::parse(fi);
    fb.close();
    fi.close();

    int bid = 1;
    int iid = 1;
    std::vector<std::string> idsb;
    for (auto& el : lbl.items())
    {
        idsb.push_back(el.key());
    }
    std::vector<std::string> idsi;
    for (auto& el : lil.items())
    {
        idsi.push_back(el.key());
    }
    while(app.IsRunning())
    {
        if (bid < 1)
        {
            bid = 1;
        }
        if (iid < 1)
        {
            iid = 1;
        }
        if (bid > lbl.size())
        {
            bid = lbl.size();
        }
        if (iid > lil.size())
        {
            iid = lil.size();
        }
        
        if (prevcrash)
        {
            ImGui::End();
            prevcrash = false;
        }
        
        if(!updt)
        {
            app.SetWindowPos(NImGui::Vec2i((app.GetMonitorSize().x/2)-(app.GetWindowSize().x/2), (app.GetMonitorSize().y/2)-(app.GetWindowSize().y/2)));
            updt = true;    
        }
        //pr.UpdateScreen();
        app.SetVsync(vsy);
        deltatime = deltaclock.GetAsMs();
        deltaclock.Reset();
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        ImGui::Begin("Fps", NULL, ImGuiWindowFlags_NoDocking);
        if(ImGui::Button("Close")) break;
        ImGui::Text("Fps -> %s", FrameRate().c_str());
        ImGui::Text("Delta -> %sms", std::to_string(deltatime).c_str());
        ImGui::Checkbox("Vsync", &vsy);
        ImGui::Text("MouseLeft -> %d", (int)app.IsMouseButtonDown(NImGui::MouseButton::Left));
        ImGui::Text("Key W -> %d", (int)app.IsKeyDown(NImGui::KeyCode::W));
        ImGui::Image((ImTextureID)llj.GetImage().GetRegID(), ImVec2(20, 20));
        nlohmann::json bbj = lbl.at(idsb[bid-1]);
        ImGui::Text("Block (%d/%d) -> %s", bid, idsb.size(), bbj.at("name").get<std::string>().c_str());
        ImGui::InputInt("Block", &bid);
        ImGui::SameLine();
        ImGui::Image(img.GetTextureID(), ImVec2(16, 16), ImVec2(bbj.at("u1").get<float>(), bbj.at("v1").get<float>()), ImVec2(bbj.at("u2").get<float>(), bbj.at("v2").get<float>()));
        
        nlohmann::json ibj = lil.at(idsi[iid-1]);

        ImGui::Text("Item (%d/%d) -> %s", iid, idsi.size(), ibj.at("name").get<std::string>().c_str());
        ImGui::InputInt("Item", &iid);
        ImGui::SameLine();
        ImGui::Image(img2.GetTextureID(), ImVec2(16, 16), ImVec2(ibj.at("u1").get<float>(), ibj.at("v1").get<float>()), ImVec2(ibj.at("u2").get<float>(), ibj.at("v2").get<float>()));
        
        ImGui::End();
        app.SwapBuffers();
    }

    return 0;
}
