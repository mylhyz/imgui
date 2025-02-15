#include "imgui_impl_skia.h"

// Data
struct ImGui_Impl_Skia_Data {
  SkPaint *FontTexturePaint;
  SkScalar Scalar;
  ImGui_Impl_Skia_Data() { memset(this, 0, sizeof(*this)); }
};

static ImGui_Impl_Skia_Data *ImGui_Impl_Skia_CreateBackendData() {
  return IM_NEW(ImGui_Impl_Skia_Data)();
}
static ImGui_Impl_Skia_Data *ImGui_Impl_Skia_GetBackendData() {
  return (ImGui_Impl_Skia_Data *)ImGui::GetIO().BackendRendererUserData;
}
static void ImGui_Impl_Skia_DestroyBackendData() {
  IM_DELETE(ImGui_Impl_Skia_GetBackendData());
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

void ImGui_Impl_Skia_Init(const float scaleFactor) {
  ImGuiIO &io = ImGui::GetIO();
  IM_ASSERT(io.BackendRendererUserData == NULL &&
            "Already initialized a platform backend!");
  ImGui_Impl_Skia_Data *bd = ImGui_Impl_Skia_CreateBackendData();
  bd->Scalar = scaleFactor;
  io.BackendRendererUserData = bd;
  io.BackendRendererName = "imgui_impl_skia";
  // 构造Font纹理
  bd->FontTexturePaint = IM_NEW(SkPaint)();
  ImFontAtlas &atlas = *ImGui::GetIO().Fonts;
  atlas.Clear();
  ImFontConfig cfg;
  cfg.SizePixels = 13 * 1;
  atlas.AddFontDefault(&cfg);
  build_ImFontAtlas(atlas, bd->FontTexturePaint);
}

void ImGui_Impl_Skia_Shutdown() {
  ImGui_Impl_Skia_Data *bd = ImGui_Impl_Skia_GetBackendData();
  IM_DELETE(bd->FontTexturePaint);
  ImGui_Impl_Skia_DestroyBackendData();
}

void ImGui_Impl_Skia_NewFrame() {}

void ImGui_Impl_Skia_RenderDrawData(SkSurface *surface, ImDrawData *drawData) {
  // Then we fetch the most recent data, and convert it so we can render with
  // Skia
  SkTDArray<SkPoint> pos;
  SkTDArray<SkPoint> uv;
  SkTDArray<SkColor> color;
  auto canvas = surface->getCanvas();

  canvas->save();
  ImGui_Impl_Skia_Data *bd = ImGui_Impl_Skia_GetBackendData();
  canvas->scale(bd->Scalar, bd->Scalar);

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
        indexOffset += drawCmd->ElemCount;
      }
    }
  }

  canvas->restore();

  surface->flushAndSubmit();
}
