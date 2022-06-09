#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkSurface.h"
#include "include/core/SkVertices.h"
#include "include/effects/SkGradientShader.h"

int main() {
  int screen_width = 640;
  int screen_height = 480;
  // 创建SkImageInfo
  SkColorType ct = SkColorType::kRGBA_8888_SkColorType;
  SkAlphaType at = SkAlphaType::kPremul_SkAlphaType;
  SkImageInfo* info =
      new SkImageInfo(SkImageInfo::Make(screen_width, screen_height, ct, at));
  //创建SkSurface
  SkPixelGeometry geo = kUnknown_SkPixelGeometry;
  SkSurfaceProps surfProps(0, geo);
  SkSurface* surface = SkSurface::MakeRaster(*info, &surfProps).release();

  // 获取canvas并进行绘制
  SkCanvas* canvas = surface->getCanvas();

  SkPaint paint;
  SkPoint points[] = {{0, 0}, {250, 0}, {100, 100}, {0, 250}};
  SkPoint texs[] = {{0, 0}, {0, 250}, {250, 250}, {250, 0}};
  SkColor colors[] = {SK_ColorRED, SK_ColorBLUE, SK_ColorYELLOW, SK_ColorCYAN};
  paint.setShader(SkGradientShader::MakeLinear(points, colors, nullptr, 4,
                                               SkTileMode::kClamp));
  auto vertices =
      SkVertices::MakeCopy(SkVertices::kTriangleFan_VertexMode,
                           SK_ARRAY_COUNT(points), points, texs, colors);
  canvas->drawVertices(vertices, SkBlendMode::kDarken, paint);

  //导出图像数据
  SkImage* image = surface->makeImageSnapshot().release();
  SkData* data = image->encodeToData().release();
  image->unref();
  FILE* f = fopen("skia_main_demo.png", "wb");
  fwrite(data->data(), data->size(), 1, f);
  fclose(f);
  data->unref();
  // 释放surface
  surface->unref();

  return 0;
}
