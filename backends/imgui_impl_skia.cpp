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

void ImGui_Impl_Skia_Init() {
  printf("ImGui_Impl_Skia_Init\n");
  //初始化Font
  ImGuiIO io = ImGui::GetIO();
  ImFontAtlas atlas = *io.Fonts;
  SkPaint fontPaint;
  // Build atlas
  unsigned char *tex_pixels = NULL;
  int tex_w, tex_h;
  io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);
  SkImageInfo info = SkImageInfo::MakeA8(tex_w, tex_h);
  SkPixmap pmap(info, tex_pixels, info.minRowBytes());
  SkMatrix localMatrix = SkMatrix::Scale(1.0f / tex_w, 1.0f / tex_h);
  auto fontImage = SkImage::MakeFromRaster(pmap, nullptr, nullptr);
  auto fontShader = fontImage->makeShader(
      SkSamplingOptions(SkFilterMode::kLinear), localMatrix);
  fontPaint.setShader(fontShader);
  fontPaint.setColor(SK_ColorWHITE);
  atlas.TexID = &fontPaint;
}
void ImGui_Impl_Skia_NewFrame() { printf("ImGui_Impl_Skia_NewFrame\n"); }
void ImGui_Impl_Skia_SetupRenderState() {
  printf("ImGui_Impl_Skia_SetupRenderState\n");
}
typedef std::function<void(SkCanvas *)> SkiaWidgetFunc;
void ImGui_Impl_Skia_RenderDrawData(ImDrawData *drawData) {
  printf("ImGui_Impl_Skia_RenderDrawData\n");

  int fb_width = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
  int fb_height = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
  if (fb_width <= 0 || fb_height <= 0)
    return;

  SkSurface *surface = make_surface(fb_width, fb_height);

  SkTArray<SkiaWidgetFunc> fSkiaWidgetFuncs;

  // Then we fetch the most recent data, and convert it so we can render with
  // Skia
  SkTDArray<SkPoint> pos;
  SkTDArray<SkPoint> uv;
  SkTDArray<SkColor> color;
  auto canvas = surface->getCanvas();

  for (int i = 0; i < drawData->CmdListsCount; ++i) {
    const ImDrawList *drawList = drawData->CmdLists[i];

    // De-interleave all vertex data (sigh), convert to Skia types
    pos.rewind();
    uv.rewind();
    color.rewind();
    for (int j = 0; j < drawList->VtxBuffer.size(); ++j) {
      const ImDrawVert &vert = drawList->VtxBuffer[j];
      pos.push_back(SkPoint::Make(vert.pos.x, vert.pos.y));
      uv.push_back(SkPoint::Make(vert.uv.x, vert.uv.y));
      color.push_back(vert.col);
    }
    // ImGui colors are RGBA
    SkSwapRB(color.begin(), color.begin(), color.count());

    int indexOffset = 0;

    // Draw everything with canvas.drawVertices...
    for (int j = 0; j < drawList->CmdBuffer.size(); ++j) {
      const ImDrawCmd *drawCmd = &drawList->CmdBuffer[j];

      SkAutoCanvasRestore acr(canvas, true);

      // TODO: Find min/max index for each draw, so we know how many vertices
      // (sigh)
      if (drawCmd->UserCallback) {
        drawCmd->UserCallback(drawList, drawCmd);
      } else {
        intptr_t idIndex = (intptr_t)drawCmd->TextureId;
        if (idIndex < fSkiaWidgetFuncs.count()) {
          // Small image IDs are actually indices into a list of callbacks. We
          // directly examing the vertex data to deduce the image rectangle,
          // then reconfigure the canvas to be clipped and translated so that
          // the callback code gets to use Skia to render a widget in the middle
          // of an ImGui panel.
          ImDrawIdx rectIndex = drawList->IdxBuffer[indexOffset];
          SkPoint tl = pos[rectIndex], br = pos[rectIndex + 2];
          canvas->clipRect(SkRect::MakeLTRB(tl.fX, tl.fY, br.fX, br.fY));
          canvas->translate(tl.fX, tl.fY);
          fSkiaWidgetFuncs[idIndex](canvas);
        } else {
          SkPaint *paint = nullptr;
          if (drawCmd->TextureId) {
            paint = static_cast<SkPaint *>(drawCmd->TextureId);
          } else {
            paint = new SkPaint();
          }
          SkASSERT(paint);

          canvas->clipRect(
              SkRect::MakeLTRB(drawCmd->ClipRect.x, drawCmd->ClipRect.y,
                               drawCmd->ClipRect.z, drawCmd->ClipRect.w));
          auto vertices = SkVertices::MakeCopy(
              SkVertices::kTriangles_VertexMode, drawList->VtxBuffer.size(),
              pos.begin(), uv.begin(), color.begin(), drawCmd->ElemCount,
              drawList->IdxBuffer.begin() + indexOffset);
          canvas->drawVertices(vertices, SkBlendMode::kModulate, *paint);

          delete paint;
        }
        indexOffset += drawCmd->ElemCount;
      }
    }
  }

  fSkiaWidgetFuncs.reset();
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
