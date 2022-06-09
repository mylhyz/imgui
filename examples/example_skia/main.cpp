// dear imgui: "null" example application
// (compile and link imgui, create context, run headless with NO INPUTS, NO
// GRAPHICS OUTPUT) This is useful to test building, but you cannot interact
// with anything here!
#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_skia.h"

int main(int, char**) {
  // 创建绘图上下文
  (void)0;
  // 初始化窗口能力
  (void)0;
  //设置ImGui上下文
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  //初始化Platform/Renderer后端
  ImGui_Impl_Skia_Init();

  // Build atlas
  unsigned char* tex_pixels = NULL;
  int tex_w, tex_h;
  io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);

  for (int n = 0; n < 60; n++) {
    printf("NewFrame() Start %d\n", n);
    io.DisplaySize = ImVec2(1920, 1080);
    io.DeltaTime = 1.0f / 60.0f;
    // 开始本帧绘制
    ImGui_Impl_Skia_NewFrame();

    ImGui::NewFrame();

    static float f = 0.0f;
    ImGui::Text("Hello, world!");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / io.Framerate, io.Framerate);
    ImGui::ShowDemoWindow(NULL);

    ImGui::Render();

    // 完成本帧上屏
    if (n == 59) {
      ImGui_Impl_Skia_RenderDrawData(ImGui::GetDrawData());
    }
    printf("NewFrame() End %d\n", n);
  }

  printf("DestroyContext()\n");
  ImGui::DestroyContext();
  return 0;
}
