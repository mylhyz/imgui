#include "imgui_impl_skia.h"

// Data
struct ImGui_Impl_Skia_Data {
  SkPaint *FontTexturePaint;
  ImGui_Impl_Skia_Data() { memset(this, 0, sizeof(*this)); }
};

static ImGui_Impl_Skia_Data *ImGui_Impl_Skia_CreateBackendData() {
  return IM_NEW(ImGui_Impl_Skia_Data)();
}
static ImGui_Impl_Skia_Data *ImGui_Impl_Skia_GetBackendData() {
  return (ImGui_Impl_Skia_Data *)ImGui::GetIO().BackendPlatformUserData;
}
static void ImGui_Impl_Skia_DestroyBackendData() {
  IM_DELETE(ImGui_Impl_Skia_GetBackendData());
}

static SkSurface *ImGui_Impl_Skia_CreateBackendSurface(int32_t w, int32_t h) {
  // 创建SkImageInfo
  SkColorType ct = SkColorType::kRGBA_8888_SkColorType;
  SkAlphaType at = SkAlphaType::kPremul_SkAlphaType;
  SkImageInfo *info = new SkImageInfo(SkImageInfo::Make(w, h, ct, at));
  //创建SkSurface
  SkPixelGeometry geo = kUnknown_SkPixelGeometry;
  SkSurfaceProps surfProps(0, geo);
  SkSurface *surface = SkSurface::MakeRaster(*info, &surfProps).release();
  //将SkSurface指针返回
  return surface;
}

static void build_ImFontAtlas(ImFontAtlas &atlas, SkPaint *fontPaint) {
  int w, h;
  unsigned char *pixels;
  atlas.GetTexDataAsAlpha8(&pixels, &w, &h);
  SkImageInfo info = SkImageInfo::MakeA8(w, h);
  SkPixmap pmap(info, pixels, info.minRowBytes());
  SkMatrix localMatrix = SkMatrix::Scale(1.0f / w, 1.0f / h);
  auto fontImage = SkImage::MakeFromRaster(pmap, nullptr, nullptr);
  auto fontShader = fontImage->makeShader(
      SkSamplingOptions(SkFilterMode::kLinear), localMatrix);
  fontPaint->setShader(fontShader);
  fontPaint->setColor(SK_ColorWHITE);
  atlas.SetTexID(fontPaint);
}

void ImGui_Impl_Skia_Init() {
  ImGuiIO &io = ImGui::GetIO();
  IM_ASSERT(io.BackendPlatformUserData == NULL &&
            "Already initialized a platform backend!");
  ImGui_Impl_Skia_Data *bd = ImGui_Impl_Skia_CreateBackendData();
  io.BackendPlatformUserData = bd;
  io.BackendPlatformName = io.BackendRendererName = "skia-cross-platform";
  // 构造Font纹理
  bd->FontTexturePaint = IM_NEW(SkPaint)();
  build_ImFontAtlas(*ImGui::GetIO().Fonts, bd->FontTexturePaint);
}

void ImGui_Impl_Skia_Destroy() {
  ImGui_Impl_Skia_Data *bd = ImGui_Impl_Skia_GetBackendData();
  IM_DELETE(bd->FontTexturePaint);
  ImGui_Impl_Skia_DestroyBackendData();
}
void ImGui_Impl_Skia_NewFrame(float width, float height) {
  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize = ImVec2(width, height);
}
void ImGui_Impl_Skia_SetupRenderState() {}
typedef std::function<void(SkCanvas *)> SkiaWidgetFunc;
void ImGui_Impl_Skia_RenderDrawData(SkSurface *surface, ImDrawData *drawData) {

  int fb_width = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
  int fb_height = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
  if (fb_width <= 0 || fb_height <= 0)
    return;

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
          SkPaint *paint = static_cast<SkPaint *>(drawCmd->TextureId);
          SkASSERT(paint);

          canvas->clipRect(
              SkRect::MakeLTRB(drawCmd->ClipRect.x, drawCmd->ClipRect.y,
                               drawCmd->ClipRect.z, drawCmd->ClipRect.w));
          auto vertices = SkVertices::MakeCopy(
              SkVertices::kTriangles_VertexMode, drawList->VtxBuffer.size(),
              pos.begin(), uv.begin(), color.begin(), drawCmd->ElemCount,
              drawList->IdxBuffer.begin() + indexOffset);
          canvas->drawVertices(vertices, SkBlendMode::kModulate, *paint);
        }
        indexOffset += drawCmd->ElemCount;
      }
    }
  }

  fSkiaWidgetFuncs.reset();
  //释放surface
  surface->unref();
}
