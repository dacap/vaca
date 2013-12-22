// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <Vaca/Vaca.h>
#include <vfw.h>
#include "../resource.h"

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
      Char name[256];
      Char ver[256];
      if (capGetDriverDescription(i, name, sizeof(name), ver, sizeof(ver)))
	addItem(String(name)+L" "+ver);
      else
	break;
    }
    // no drivers?
    if (!getItemCount()) {
      addItem(L"No drivers");
      setEnabled(false);
    }
    else {
      setSelectedItem(0);
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
    : Widget(WidgetClassName::None, parent, Widget::Styles::None)
    , m_driverIndex(-1)
  {
    // now we can create the widget because in this point is when our
    // version of createHandle (see below) will be called
    create(WidgetClassName::None, parent, Widget::Styles::Visible);
    
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
  virtual HWND createHandle(LPCTSTR className, Widget* parent, Style style)
  {
    // this routine is from VFW
    return capCreateCaptureWindow(L"WebCam",
				  style.regular,
				  0, 0, 160, 120,
				  parent ? parent->getHandle(): NULL,
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
  TextEdit m_rateEdit;

public:
  MainFrame()
    : Frame(L"WebCam")
    , m_driverLabel(L"Driver:", this)
    , m_driver(this)
    , m_webcam(this)
    , m_start(L"Start", this)
    , m_capture(L"Capture", this)
    , m_copy(L"Copy", this)
    , m_rateLabel(L"Preview Rate (milliseconds):", this)
    , m_rate(1, 1000, 100, this)
    , m_rateEdit(L"", this, TextEdit::Styles::Default +
			    TextEdit::Styles::ReadOnly)
  {
    setLayout(Bix::parse(L"Y[X[%,f%],X[fX[],%,fX[]],X[fX[],%,%,%,fX[]],%,X[f%,%]]",
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

    m_start.Click.connect(Bind(&MainFrame::onStart, this));
    m_capture.Click.connect(Bind(&MainFrame::onCapture, this));
    m_copy.Click.connect(Bind(&WebCam::copy, &m_webcam));
    m_rate.Change.connect(Bind(&MainFrame::onChangeRate, this));

    // update m_rateEdit text
    onChangeRate();

    setSize(getPreferredSize());
  }

private:
  void onStart()
  {
    if (m_start.isSelected()) {
      m_start.setText(L"Stop");

      m_webcam.setDriver(m_driver.getSelectedItem());
      m_webcam.startPreview();
      m_webcam.setPreviewRate(m_rate.getValue());
    }
    else {
      m_start.setText(L"Start");

      m_webcam.stopPreview();
    }
  }

  void onCapture()
  {
    if (m_start.isSelected()) {
      m_start.setText(L"Start");
      m_start.setSelected(false);
    }
    else {
      m_webcam.setDriver(m_driver.getSelectedItem());
    }
    m_webcam.capture();
  }
  
  void onChangeRate()
  {
    int rate = m_rate.getValue();

    m_rateEdit.setText(convert_to<String>(rate));
    m_webcam.setPreviewRate(rate);
  }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
