#ifndef __IMGUI_IMPL_SKIA_H__
#define __IMGUI_IMPL_SKIA_H__

#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkSurface.h"
#include "include/core/SkVertices.h"
#include "include/core/SkSwizzle.h"

#include "imgui.h"

void ImGui_Impl_Skia_Init();
void ImGui_Impl_Skia_NewFrame();
void ImGui_Impl_Skia_RenderDrawData(ImDrawData *draw_data);

#endif //__IMGUI_IMPL_SKIA_H__
