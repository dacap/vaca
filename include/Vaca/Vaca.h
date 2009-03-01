// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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
// * Neither the name of the author nor the names of its contributors
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

#ifndef VACA_VACA_H
#define VACA_VACA_H

#include "Vaca/base.h"

#include "Vaca/Anchor.h"
#include "Vaca/AnchorLayout.h"
#include "Vaca/Application.h"
#include "Vaca/BandedDockArea.h"
#include "Vaca/BasicDockArea.h"
#include "Vaca/Bind.h"
#include "Vaca/Bix.h"
#include "Vaca/BoxConstraint.h"
#include "Vaca/BoxLayout.h"
#include "Vaca/Brush.h"
#include "Vaca/Button.h"
#include "Vaca/ButtonBase.h"
#include "Vaca/CancelableEvent.h"
#include "Vaca/CheckBox.h"
#include "Vaca/ClientLayout.h"
#include "Vaca/Clipboard.h"
#include "Vaca/CloseEvent.h"
#include "Vaca/Color.h"
#include "Vaca/ColorDialog.h"
#include "Vaca/ComboBox.h"
#include "Vaca/Command.h"
#include "Vaca/CommonDialog.h"
#include "Vaca/Component.h"
#include "Vaca/ConditionVariable.h"
#include "Vaca/Constraint.h"
#include "Vaca/ConsumableEvent.h"
#include "Vaca/Cursor.h"
#include "Vaca/CustomButton.h"
#include "Vaca/CustomLabel.h"
#include "Vaca/Debug.h"
#include "Vaca/Dialog.h"
#include "Vaca/DockArea.h"
#include "Vaca/DockBar.h"
#include "Vaca/DockFrame.h"
#include "Vaca/DropFilesEvent.h"
#include "Vaca/Enum.h"
#include "Vaca/Event.h"
#include "Vaca/Exception.h"
#include "Vaca/FileDialog.h"
#include "Vaca/FindTextDialog.h"
#include "Vaca/Font.h"
#include "Vaca/FontDialog.h"
#include "Vaca/Frame.h"
#include "Vaca/Graphics.h"
#include "Vaca/GraphicsPath.h"
#include "Vaca/GroupBox.h"
#include "Vaca/HttpRequest.h"
#include "Vaca/Icon.h"
#include "Vaca/Image.h"
#include "Vaca/ImageList.h"
#include "Vaca/KeyEvent.h"
#include "Vaca/Keys.h"
#include "Vaca/Label.h"
#include "Vaca/Layout.h"
#include "Vaca/LinkLabel.h"
#include "Vaca/ListBox.h"
#include "Vaca/ListView.h"
#include "Vaca/Mdi.h"
#include "Vaca/Menu.h"
#include "Vaca/MenuItemEvent.h"
#include "Vaca/Message.h"
#include "Vaca/MouseEvent.h"
#include "Vaca/MsgBox.h"
#include "Vaca/Mutex.h"
#include "Vaca/PaintEvent.h"
#include "Vaca/ParseException.h"
#include "Vaca/Pen.h"
#include "Vaca/Point.h"
#include "Vaca/ProgressBar.h"
#include "Vaca/RadioButton.h"
#include "Vaca/ReBar.h"
#include "Vaca/Rect.h"
#include "Vaca/Referenceable.h"
#include "Vaca/Region.h"
#include "Vaca/Register.h"
#include "Vaca/RichEdit.h"
#include "Vaca/Scintilla.h"
#include "Vaca/ScopedLock.h"
#include "Vaca/ScrollEvent.h"
#include "Vaca/ScrollInfo.h"
#include "Vaca/Separator.h"
#include "Vaca/SharedPtr.h"
#include "Vaca/Signal.h"
#include "Vaca/Size.h"
#include "Vaca/Slider.h"
#include "Vaca/Slot.h"
#include "Vaca/SpinButton.h"
#include "Vaca/Spinner.h"
#include "Vaca/SplitBar.h"
#include "Vaca/StatusBar.h"
#include "Vaca/String.h"
#include "Vaca/Style.h"
#include "Vaca/System.h"
#include "Vaca/Tab.h"
#include "Vaca/TextEdit.h"
#include "Vaca/Thread.h"
#include "Vaca/TimePoint.h"
#include "Vaca/Timer.h"
#include "Vaca/ToggleButton.h"
#include "Vaca/ToolBar.h"
#include "Vaca/TreeNode.h"
#include "Vaca/TreeView.h"
#include "Vaca/TreeViewEvent.h"
#include "Vaca/Vaca.h"
#include "Vaca/Widget.h"
#include "Vaca/WidgetClass.h"

#endif // VACA_VACA_H
