/***************************************************************************
 # Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.
 #
 # Redistribution and use in source and binary forms, with or without
 # modification, are permitted provided that the following conditions
 # are met:
 #  * Redistributions of source code must retain the above copyright
 #    notice, this list of conditions and the following disclaimer.
 #  * Redistributions in binary form must reproduce the above copyright
 #    notice, this list of conditions and the following disclaimer in the
 #    documentation and/or other materials provided with the distribution.
 #  * Neither the name of NVIDIA CORPORATION nor the names of its
 #    contributors may be used to endorse or promote products derived
 #    from this software without specific prior written permission.
 #
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 # EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 # IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 # PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 # CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 # EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 # PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 # PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 # OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 # (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 # OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **************************************************************************/
#ifndef FALCOR_CORE_SAMPLE_H_
#define FALCOR_CORE_SAMPLE_H_

#include <set>
#include <optional>
#include <vector>
#include <utility>
#include <string>

#include "Falcor/Core/Window.h"
#include "Falcor/Core/API/Device.h"
#include "Renderer.h"
#include "Falcor/Utils/ArgList.h"
#include "Falcor/Utils/Timing/FrameRate.h"
#include "Falcor/Utils/UI/Gui.h"
#include "Falcor/Utils/UI/TextRenderer.h"
#include "Falcor/Utils/UI/PixelZoom.h"
#include "Falcor/Utils/Video/VideoEncoderUI.h"


namespace Falcor {
/** Bootstrapper class for Falcor
    Call Sample::run() to start the sample.
    The render loop will then call the user's Renderer object
*/
class dlldecl Sample : public Window::ICallbacks, public IFramework {
 public:
    /** Entry-point to Sample. User should call this to start processing.
        \param[in] config Requested sample configuration.
        \param[in] pRenderer The user's renderer. The Sample takes ownership of the renderer.
        \param[in] argc Optional. The number of strings in `argv`.
        \param[in] argv Optional. The command line arguments.
        Note that when running a Windows application (with WinMain()), the command line arguments will be retrieved and parsed even if argc and argv are nullptr.
    */
    static void run(Device::SharedPtr pDevice, const SampleConfig& config, IRenderer::UniquePtr& pRenderer, uint32_t argc = 0, char** argv = nullptr);

    /** Entry-point to Sample. User should call this to start processing.
        \param[in] filename A filename containing the sample configuration. If the file is not found, the sample will issue an error and lunch with the default configuration.
        \param[in] pRenderer The user's renderer. The Sample takes ownership of the renderer.
        \param[in] argc Optional. The number of strings in `argv`.
        \param[in] argv Optional. The command line arguments.
        Note that when running a Windows application (with WinMain()), the command line arguments will be retrieved and parsed even if argc and argv are nullptr.
    */
    static void run(Device::SharedPtr pDevice, const std::string& filename, IRenderer::UniquePtr& pRenderer, uint32_t argc = 0, char** argv = nullptr);

    virtual ~Sample();

 protected:
    /************************************************************************/
    /* Callback inherited from ICallbacks                                   */
    /************************************************************************/
    RenderContext* getRenderContext() override { return mpDevice ? mpDevice->getRenderContext() : nullptr; }
    Fbo::SharedPtr getTargetFbo() override { return mpTargetFBO; }
    Window* getWindow() override { return mpWindow.get(); }
    
    //Clock& getGlobalClock() override { return mClock; }

    Clock& getClock() override { return *mClock; }
    FrameRate& getFrameRate() override { return *mFrameRate; }

    void resizeSwapChain(uint32_t width, uint32_t height) override;
    bool isKeyPressed(const KeyboardEvent::Key& key) override;
    void toggleUI(bool showUI) override { mShowUI = showUI; }
    bool isUiEnabled() override { return mShowUI; }
    ArgList getArgList() override { return mArgList; }
    void pauseRenderer(bool pause) override { mRendererPaused = pause; }
    bool isRendererPaused() override { return mRendererPaused; }
    std::string captureScreen(const std::string explicitFilename = "", const std::string explicitOutputDirectory = "") override;
    void shutdown() override { if (mpWindow) { mpWindow->shutdown(); } }
    SampleConfig getConfig() override;
    void renderGlobalUI(Gui* pGui) override;
    std::string getKeyboardShortcutsStr() override;
    void toggleVsync(bool on) override { mVsyncOn = on; }
    bool isVsyncEnabled() override { return mVsyncOn; }

    /** Internal data structures
    */
    Gui::UniquePtr mpGui;                               ///< Main sample GUI
    Fbo::SharedPtr mpTargetFBO;                         ///< The FBO available to renderers
    bool mRendererPaused = false;                       ///< Freezes the renderer
    ArgList mArgList;                                   ///< Arguments passed in by command line
    Window::SharedPtr mpWindow;                         ///< The application's window

    void renderFrame() override;
    void handleWindowSizeChange() override;
    void handleKeyboardEvent(const KeyboardEvent& keyEvent) override;
    void handleMouseEvent(const MouseEvent& mouseEvent) override;
    void handleDroppedFile(const std::string& filename) override;

    void initVideoCapture();

    // Private functions
    void initUI();
    void saveConfigToFile();

    bool startVideoCapture();
    void endVideoCapture();
    void captureVideoFrame();
    void renderUI();

    void runInternal(Device::SharedPtr pDevice, const SampleConfig& config, uint32_t argc, char** argv);

    void startScripting();
    void registerScriptBindings(ScriptBindings::Module& m);

    bool mSuppressInput = false;
    bool mVsyncOn = false;
    bool mShowUI = false;
    bool mCaptureScreen = false;

    FrameRate *mFrameRate;
    Clock *mClock;

    IRenderer::UniquePtr mpRenderer;

    struct VideoCaptureData {
        VideoEncoderUI::UniquePtr pUI;
        VideoEncoder::UniquePtr pVideoCapture;
        std::vector<uint8_t> pFrame;
        double fixedTimeDelta = 0;
        double currentTime = 0;
        bool displayUI = false;
    } mVideoCapture;

    std::set<KeyboardEvent::Key> mPressedKeys;
    PixelZoom::SharedPtr mpPixelZoom;

    Sample(Device::SharedPtr pDevice, IRenderer::UniquePtr& pRenderer);
    Sample(const Sample&) = delete;
    Sample& operator=(const Sample&) = delete;

    Device::SharedPtr mpDevice;
};

}  // namespace Falcor

#endif  // FALCOR_CORE_SAMPLE_H_
