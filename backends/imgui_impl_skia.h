#ifndef __IMGUI_IMPL_SKIA_H__
#define __IMGUI_IMPL_SKIA_H__

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkSurface.h"
#include "include/core/SkSwizzle.h"
#include "include/core/SkVertices.h"

#include "include/gpu/GrContextOptions.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"

#include "src/gpu/gl/GrGLDefines.h"
#include "src/gpu/ganesh/gl/GrGLUtil.h"

#include "imgui.h"

void ImGui_Impl_Skia_Init(const float scaleFactor);
void ImGui_Impl_Skia_NewFrame();
void ImGui_Impl_Skia_RenderDrawData(SkSurface *surface, ImDrawData *draw_data);
void ImGui_Impl_Skia_Shutdown();

#endif //__IMGUI_IMPL_SKIA_H__
