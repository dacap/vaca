// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the Vaca nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include <Vaca/Vaca.h>
#include <vfw.h>

#ifndef WM_CAP_DRIVER_CONNECT

#define WM_CAP_DRIVER_CONNECT		(WM_USER+10)
#define WM_CAP_DRIVER_DISCONNECT	(WM_USER+11)
#define WM_CAP_DRIVER_GET_CAPS		(WM_USER+14)
#define WM_CAP_EDIT_COPY		(WM_USER+30)
#define WM_CAP_SET_PREVIEW		(WM_USER+50)
#define WM_CAP_SET_PREVIEWRATE		(WM_USER+52)
#define WM_CAP_SET_SCALE		(WM_USER+53)
#define WM_CAP_GET_STATUS		(WM_USER+54)
#define WM_CAP_GRAB_FRAME		(WM_USER+60)
#define WM_CAP_SEQUENCE_NOFILE		(WM_USER+63)

typedef struct {
  UINT uiImageWidth;
  UINT uiImageHeight;
  BOOL fLiveWindow;
  BOOL fOverlayWindow;
  BOOL fScale;
  POINT ptScroll;
  BOOL fUsingDefaultPalette;
  BOOL fAudioHardware;
  BOOL fCapFileExists;
  DWORD dwCurrentVideoFrame;
  DWORD dwCurrentVideoFramesDropped;
  DWORD dwCurrentWaveSamples;
  DWORD dwCurrentTimeElapsedMS;
  HPALETTE hPalCurrent;
  BOOL fCapturingNow;
  DWORD dwReturn;
  UINT wNumVideoAllocated;
  UINT wNumAudioAllocated;
} CAPSTATUS;
#endif

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

class DriversComboBox : public ComboBox
{
public:
  DriversComboBox(Widget* parent)
    : ComboBox(parent)
  {
    for (int i=0; i<10; i++) {	// driver index goes from 0 to 9
      Character name[256];
      Character ver[256];
      if (capGetDriverDescription(i, name, sizeof(name), ver, sizeof(ver)))
	addItem(String(name)+" "+ver);
      else
	break;
    }
    // no drivers?
    if (!getItemCount()) {
      addItem("No drivers");
      setEnabled(false);
    }
    else {
      setCurrentItem(0);
    }
  }
};

//////////////////////////////////////////////////////////////////////

class WebCam : public Widget
{
  int m_driverIndex;

public:
  WebCam(Widget* parent)
    // Create the Widget without a Win32 class (this means that the
    // HWND will be created later). See TN002 for more information.
    : Widget(WidgetClassName::None, parent)
    , m_driverIndex(-1)
  {
    // now we can create the widget because in this point is when our
    // version of createHWND (see below) will be called
    create(WidgetClassName::None, parent, ChildStyle);
    
    setPreferredSize(Size(320, 240));
  }

  virtual ~WebCam()
  {
    // disconnect the driver
    setDriver(-1);
  }
  
  void setDriver(int driverIndex)
  {
    if (m_driverIndex != driverIndex) {
      // disconnect from the old driver
      if (m_driverIndex >= 0)
	sendMessage(WM_CAP_DRIVER_DISCONNECT, 0, 0L);

      // connect to the new driver
      m_driverIndex = driverIndex;
      if (m_driverIndex >= 0) {
	if (sendMessage(WM_CAP_DRIVER_CONNECT, m_driverIndex, 0L)) {
	  CAPSTATUS cs;
	  if (sendMessage(WM_CAP_GET_STATUS, sizeof(CAPSTATUS), reinterpret_cast<LPARAM>(&cs))) {
	    setPreferredSize(Size(cs.uiImageWidth, cs.uiImageHeight));
	    getParent()->setSize(getParent()->getPreferredSize());
	  }
	}
      }
    }
  }

  void startPreview()
  {
    if (m_driverIndex >= 0)
      sendMessage(WM_CAP_SET_PREVIEW, TRUE, 0L);
  }

  void stopPreview()
  {
    if (m_driverIndex >= 0)
      sendMessage(WM_CAP_SET_PREVIEW, FALSE, 0L);
  }

  void setPreviewRate(int msecs)
  {
    if (m_driverIndex >= 0)
      sendMessage(WM_CAP_SET_PREVIEWRATE, msecs, 0L);
  }

  void capture()
  {
    if (m_driverIndex >= 0)
      sendMessage(WM_CAP_GRAB_FRAME, 0, 0L);
  }

  void copy()
  {
    if (m_driverIndex >= 0)
      sendMessage(WM_CAP_EDIT_COPY, 0, 0L);
  }

private:
  // This is a tricky situation, we have to override this method to
  // obtain a custom HWND that isn't created with Win32's CreateWindowEx
  virtual HWND createHWND(LPCTSTR className, Widget* parent, Style style)
  {
    // this routine is from VFW
    return capCreateCaptureWindow(_T("WebCam"),
				  style.regular,
				  0, 0, 160, 120,
				  parent ? parent->getHWND(): NULL,
				  0);
  }
};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Label m_driverLabel;
  DriversComboBox m_driver;
  WebCam m_webcam;
  ToggleButton m_start;
  Button m_capture;
  Button m_copy;
  Label m_rateLabel;
  Slider m_rate;
  Edit m_rateEdit;

public:
  MainFrame()
    : Frame("WebCam")
    , m_driverLabel("Driver:", this)
    , m_driver(this)
    , m_webcam(this)
    , m_start("Start", this)
    , m_capture("Capture", this)
    , m_copy("Copy", this)
    , m_rateLabel("Preview Rate (milliseconds):", this)
    , m_rate(1, 1000, 100, this)
    , m_rateEdit("", this, EditStyle + ReadOnlyEditStyle)
  {
    setLayout(Bix::parse("Y[X[%,f%],X[fX[],%,fX[]],X[fX[],%,%,%,fX[]],%,X[f%,%]]",
			 &m_driverLabel, &m_driver,
			 &m_webcam,
			 &m_start, &m_capture, &m_copy,
			 &m_rateLabel,
			 &m_rate, &m_rateEdit));

    m_start.setEnabled(m_driver.isEnabled());
    m_capture.setEnabled(m_driver.isEnabled());
    m_copy.setEnabled(m_driver.isEnabled());
    m_rate.setEnabled(m_driver.isEnabled());

    m_rateEdit.setPreferredSize(Size(64, m_rateEdit.getPreferredSize().h));

    m_start.Action.connect(Bind(&MainFrame::onStart, this));
    m_capture.Action.connect(Bind(&MainFrame::onCapture, this));
    m_copy.Action.connect(Bind(&WebCam::copy, &m_webcam));
    m_rate.Change.connect(Bind(&MainFrame::onChangeRate, this));

    // update m_rateEdit text
    onChangeRate();

    setSize(getPreferredSize());
  }

private:
  void onStart()
  {
    if (m_start.isSelected()) {
      m_start.setText("Stop");

      m_webcam.setDriver(m_driver.getCurrentItem());
      m_webcam.startPreview();
      m_webcam.setPreviewRate(m_rate.getValue());
    }
    else {
      m_start.setText("Start");

      m_webcam.stopPreview();
    }
  }

  void onCapture()
  {
    if (m_start.isSelected()) {
      m_start.setText("Start");
      m_start.setSelected(false);
    }
    else {
      m_webcam.setDriver(m_driver.getCurrentItem());
    }
    m_webcam.capture();
  }
  
  void onChangeRate()
  {
    m_rateEdit.setText(String::fromInt(m_rate.getValue()));
    m_webcam.setPreviewRate(m_rate.getValue());
  }

};

//////////////////////////////////////////////////////////////////////

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Application app;
  MainFrame frm;
  frm.setVisible(true);
  app.run();
  return 0;
}
