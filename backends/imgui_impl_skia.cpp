#include "imgui_impl_skia.h"

static SkSurface *make_surface(int32_t w, int32_t h) {
  // 创建SkImageInfo
  SkColorType ct = SkColorType::kRGBA_8888_SkColorType;
  SkAlphaType at = SkAlphaType::kPremul_SkAlphaType;
  SkImageInfo *info = new SkImageInfo(SkImageInfo::Make(w, h, ct, at));
  //创建SkSurface
  SkPixelGeometry geo = kUnknown_SkPixelGeometry;
  SkSurfaceProps surfProps(0, geo);
  SkSurface *result = SkSurface::MakeRaster(*info, &surfProps).release();
  //将SkSurface指针返回
  return result;
}

static int g_png_index = 0;

void ImGui_Impl_Skia_Init() { printf("ImGui_Impl_Skia_Init\n"); }
void ImGui_Impl_Skia_NewFrame() { printf("ImGui_Impl_Skia_NewFrame\n"); }
void ImGui_Impl_Skia_SetupRenderState() {
  printf("ImGui_Impl_Skia_SetupRenderState\n");
}
void ImGui_Impl_Skia_RenderDrawData(ImDrawData *draw_data) {
  printf("ImGui_Impl_Skia_RenderDrawData\n");
  int fb_width =
      (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
  int fb_height =
      (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
  if (fb_width <= 0 || fb_height <= 0)
    return;

  SkSurface *surface = make_surface(fb_width, fb_height);
  SkCanvas *canvas = surface->getCanvas();

  // Will project scissor/clipping rectangles into framebuffer space
  ImVec2 clip_off = draw_data->DisplayPos; // (0,0) unless using multi-viewports
  ImVec2 clip_scale =
      draw_data->FramebufferScale; // (1,1) unless using retina display which
                                   // are often (2,2)

  // Render command lists
  // (Because we merged all buffers into a single one, we maintain our own
  // offset into them)
  for (int n = 0; n < draw_data->CmdListsCount; n++) {
    const ImDrawList *cmd_list = draw_data->CmdLists[n];
    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
      const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
      if (pcmd->UserCallback != NULL) {
        // User callback, registered via ImDrawList::AddCallback()
        // (ImDrawCallback_ResetRenderState is a special callback value used by
        // the user to request the renderer to reset render state.)
        if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
          ImGui_Impl_Skia_SetupRenderState();
        else
          pcmd->UserCallback(cmd_list, pcmd);
      } else {
        // Project scissor/clipping rectangles into framebuffer space
        ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x,
                        (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
        ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x,
                        (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);

        // Clamp to viewport as vkCmdSetScissor() won't accept values that are
        // off bounds
        if (clip_min.x < 0.0f) {
          clip_min.x = 0.0f;
        }
        if (clip_min.y < 0.0f) {
          clip_min.y = 0.0f;
        }
        if (clip_max.x > fb_width) {
          clip_max.x = (float)fb_width;
        }
        if (clip_max.y > fb_height) {
          clip_max.y = (float)fb_height;
        }
        if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
          continue;

        // Draw
        // TODO 使用Skia绘制三角形
      }
    }
  }
  //导出图像数据
  SkImage *image = surface->makeImageSnapshot().release();
  SkData *data = image->encodeToData().release();
  image->unref();
  char path[256];
  sprintf(path, "%d.png", g_png_index++);
  FILE *f = fopen(path, "wb");
  fwrite(data->data(), data->size(), 1, f);
  fclose(f);
  data->unref();
  //释放surface
  surface->unref();
}
