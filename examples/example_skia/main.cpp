// dear imgui: "null" example application
// (compile and link imgui, create context, run headless with NO INPUTS, NO
// GRAPHICS OUTPUT) This is useful to test building, but you cannot interact
// with anything here!
#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_skia.h"

static SkSurface* make_surface(int32_t w, int32_t h) {
  // 创建SkImageInfo
  SkColorType ct = SkColorType::kRGBA_8888_SkColorType;
  SkAlphaType at = SkAlphaType::kPremul_SkAlphaType;
  SkImageInfo* info = new SkImageInfo(SkImageInfo::Make(w, h, ct, at));
  //创建SkSurface
  SkPixelGeometry geo = kUnknown_SkPixelGeometry;
  SkSurfaceProps surfProps(0, geo);
  SkSurface* result = SkSurface::MakeRaster(*info, &surfProps).release();
  //将SkSurface指针返回
  return result;
}

static void emit_png(const char* path, SkSurface* surface) {
  SkImage* image = surface->makeImageSnapshot().release();
  SkData* data = image->encodeToData().release();
  image->unref();
  FILE* f = fopen(path, "wb");
  fwrite(data->data(), data->size(), 1, f);
  fclose(f);
  data->unref();
}

void draw(SkCanvas* canvas) {
  //绘制背景色
  SkPaint* fill = new SkPaint();
  fill->setColor(SkColorSetARGB(0xFF, 0x00, 0x00, 0xFF));
  canvas->drawPaint(*fill);

  fill->setColor(SkColorSetARGB(0xFF, 0x00, 0xFF, 0xFF));
  SkRect rect = SkRect::MakeLTRB(100.0f, 100.0f, 540.0f, 380.0f);
  canvas->drawRect(rect, *fill);

  SkPaint* stroke = new SkPaint();
  stroke->setColor(SkColorSetARGB(0xFF, 0xFF, 0x00, 0x00));
  stroke->setAntiAlias(true);
  stroke->setStroke(true);
  stroke->setStrokeWidth(5.0f);

  SkPathBuilder* path_builder = new SkPathBuilder();
  path_builder->moveTo(50.0f, 50.0f);
  path_builder->lineTo(590.0f, 50.0f);
  path_builder->cubicTo(-490.0f, 50.0f, 1130.0f, 430.0f, 50.0f, 430.0f);
  path_builder->lineTo(590.0f, 430.0f);

  SkPath* path = new SkPath(path_builder->detach());
  canvas->drawPath(*path, *stroke);

  fill->setColor(SkColorSetARGB(0x80, 0x00, 0xFF, 0x00));
  SkRect rect2 = SkRect::MakeLTRB(120.0f, 120.0f, 520.0f, 360.0f);
  canvas->drawOval(rect2, *fill);

  delete path_builder;
  delete path;
  delete stroke;
  delete fill;
}

int main(int, char**) {
  //创建surface
  SkSurface* surface = make_surface(640, 480);
  //获取canvas
  SkCanvas* canvas = surface->getCanvas();
  //在canvas上绘制
  draw(canvas);
  //将结果导出到图像
  emit_png("skia-c-example.png", surface);
  //对surface解引用？
  surface->unref();
  return 0;
}
