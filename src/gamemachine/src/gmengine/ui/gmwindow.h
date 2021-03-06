﻿#ifndef __GMWINDOW_H__
#define __GMWINDOW_H__
#include <gmcommon.h>
BEGIN_NS

class GM_EXPORT GMRenderContext : public IRenderContext
{
public:
	virtual IWindow* getWindow() const override;
	virtual IGraphicEngine* getEngine() const override;

public:
	inline void setWindow(IWindow* window)
	{
		this->window = window;
	}

	inline void setEngine(IGraphicEngine* engine)
	{
		this->engine = engine;
	}

private:
	IWindow* window = nullptr;
	IGraphicEngine* engine = nullptr;
};

GM_PRIVATE_CLASS(GMWindow);
class GM_EXPORT GMWindow : public IWindow
{
	GM_DECLARE_PRIVATE(GMWindow)
	GM_DISABLE_COPY_ASSIGN(GMWindow)

public:
	GMWindow();
	~GMWindow();

public:
	virtual IInput* getInputManager() override;
	virtual void msgProc(const GMMessage& message) override;
	virtual GMRect getWindowRect() override;
	virtual GMRect getRenderRect() override;
	virtual GMRect getFramebufferRect() override;
	virtual void centerWindow() override;
	virtual bool isWindowActivate() override;
	virtual void setWindowCapture(bool capture) override;
	virtual void showWindow() override;
	virtual GMWindowHandle create(const GMWindowDesc& desc) override;
	virtual GMWindowHandle getWindowHandle() const override;
	virtual bool addWidget(GMWidget* widget) override;
	virtual void setHandler(AUTORELEASE IGameHandler* handler) override;
	virtual IGameHandler* getHandler() override;
	virtual const GMWindowStates& getWindowStates() override;
	virtual void setCursor(GMCursorType cursorType) override;
	virtual bool canClose() override;
	virtual GMWindowProcHandler getProcHandler() override;
	virtual void setMultithreadRenderingFlag(GMMultithreadRenderingFlag) override;

public:
	virtual bool getInterface(GameMachineInterfaceID id, void** out) override;
	virtual bool setInterface(GameMachineInterfaceID id, void* in) override;

public:
	void setWindowHandle(GMWindowHandle handle, bool autoRelease);

	// 以下是由GMWindow子类override的
public:
	virtual bool handleSystemEvent(GMSystemEvent* event, REF GMLResult& result);
	virtual const GMwchar* getWindowClassName();

protected:
	virtual void changeCursor();
	virtual void onWindowCreated(const GMWindowDesc& wndAttrs) {}
	virtual void onWindowDestroyed();
};

class GMWindowFactory
{
public:
	static bool createWindowWithOpenGL(GMInstance instance, IWindow* parent, OUT IWindow** window);
	static bool createWindowWithDx11(GMInstance instance, IWindow* parent, OUT IWindow** window);
	static bool createTempWindow(GMbyte colorDepth, GMbyte alphaBits, GMbyte depthBits, GMbyte stencilBits, OUT GMWindowHandle& tmpWnd, OUT GMDeviceContextHandle& tmpDC, OUT GMOpenGLRenderContextHandle& tmpRC);
	static bool destroyTempWindow(GMWindowHandle tmpWnd, GMDeviceContextHandle tmpDC, GMOpenGLRenderContextHandle tmpRC);
};
END_NS
#endif